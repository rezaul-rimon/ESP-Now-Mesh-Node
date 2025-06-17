#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRac.h>

const uint16_t kIrLedPin = 27; // GPIO for IR LED
// Include the necessary libraries for IR control

#include <ir_Tcl.h>

const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;

IRsend irsend(kIrLedPin);

// IRCoolixAC coolixAC(kIrLedPin);
// IRGoodweatherAc goodweatherAC(kIrLedPin); // Goodweather AC object
// IRMitsubishiAC mitsubishiAC(kIrLedPin);
IRTcl112Ac tcl112ACS(kIrLedPin);
// IRElectraAc electraAC(kIrLedPin);

#define LED_PIN 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

const char* nodeID = "node-02";
bool isRepeater   = true;
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

unsigned long lastHBPublishTime = 0;
const unsigned long hbPublishInterval = 1 * 60 * 1000;

struct Command {
  String powerOn;     // on/off_status
  String temperature; // e.g. "24"
  String mode;        // e.g. "cool"
  String fanSpeed;    // e.g. "auto"
  String protocol;    // e.g. "tcl112"
  String v_swing;     // optional
  String h_swing;     // optional
};

Command parseCommand(const String &cmdStr) {
  Command c;
  int start = 0, idx;
  int field = 0;
  while ((idx = cmdStr.indexOf('/', start)) != -1 && field < 6) {
    String part = cmdStr.substring(start, idx);
    switch (field) {
      case 0: c.powerOn     = part; break;
      case 1: c.temperature = part; break;
      case 2: c.mode        = part; break;
      case 3: c.fanSpeed    = part; break;
      case 4: c.protocol    = part; break;
      case 5: c.v_swing     = part; break;
    }
    start = idx + 1;
    field++;
  }
  // Last segment (rest of the string)
  String last = cmdStr.substring(start);
  if      (field == 0) c.powerOn     = last;
  else if (field == 1) c.temperature = last;
  else if (field == 2) c.mode        = last;
  else if (field == 3) c.fanSpeed    = last;
  else if (field == 4) c.protocol    = last;
  else if (field == 5) c.v_swing     = last;
  else                 c.h_swing     = last;
  return c;
}

// cache last handled CMD id to avoid dup exec
String lastCmdID;
// cache recent rebroadcasts to stop loops
std::deque<String> fwdCache;
const size_t MAX_FWDS=20;

bool alreadyForwarded(const String &key) {
  return std::find(fwdCache.begin(), fwdCache.end(), key) != fwdCache.end();
}
void recordForward(const String &key) {
  fwdCache.push_back(key);
  if (fwdCache.size()>MAX_FWDS) fwdCache.pop_front();
}

void rebroadcastIfNeeded(const String &msg_id, const String &type, const String &raw) {
  if (!isRepeater) {
    Serial.println("ℹ️ Not a repeater, skipping rebroadcast.");
    return;
  }

  String key = type + ":" + msg_id;
  if (alreadyForwarded(key)) {
    Serial.println("🔁 Already forwarded: " + key);
    return;
  }

  // Parse fields to extract sender and receiver
  int i1 = raw.indexOf(',');
  int i2 = raw.indexOf(',', i1 + 1);
  if (i1 < 0 || i2 < 0) {
    Serial.println("❌ Invalid format in rebroadcast check: " + raw);
    return;
  }

  String sender = raw.substring(0, i1);
  String receiver = raw.substring(i1 + 1, i2);

  // Skip CMDs targeted to self
  if (type == "cmd" && receiver == nodeID) {
    Serial.println("⏭ CMD intended for me (" + receiver + "), not rebroadcasting.");
    return;
  }

// Skip self-originated messages
  if (sender == nodeID) {
    Serial.println("⏭ Message originated by me (" + sender + "), not rebroadcasting.");
    return;
  }

  // Random short delay to avoid ESP-NOW collision
  delay(random(10, 51));

  esp_now_send(broadcastAddress, (uint8_t *)raw.c_str(), raw.length());
  Serial.println("🔁 Re-broadcasted: " + raw);
  recordForward(key);
  leds[0] = CRGB::White;  // Indicate rebroadcast with blue LED
  FastLED.show();
  delay(100);  // Short delay to show the blue LED
  leds[0] = CRGB::Black; // Turn off LED after rebroadcast  
  FastLED.show();
}

// Handle TCL112 AC commands
void handleTCL112(const Command& ac) {
  if (ac.powerOn.equalsIgnoreCase("on")) {
    tcl112ACS.on();
    tcl112ACS.setTemp(ac.temperature.toFloat());

    if      (ac.fanSpeed.equalsIgnoreCase("high")) tcl112ACS.setFan(kTcl112AcFanHigh);
    else if (ac.fanSpeed.equalsIgnoreCase("med"))  tcl112ACS.setFan(kTcl112AcFanMed);
    else if (ac.fanSpeed.equalsIgnoreCase("low"))  tcl112ACS.setFan(kTcl112AcFanLow);
    else if (ac.fanSpeed.equalsIgnoreCase("auto")) tcl112ACS.setFan(kTcl112AcFanAuto);
    else Serial.println("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) tcl112ACS.setMode(kTcl112AcCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  tcl112ACS.setMode(kTcl112AcFan);
    // else if (ac.mode.equalsIgnoreCase("auto")) tcl112ACS.setMode(kTcl112AcAuto);
    else Serial.println("⚠️ Unknown mode: " + ac.mode);
  } else {
    tcl112ACS.off();
  }

  delay(100);
  tcl112ACS.send();
  Serial.println("✅ TCL112 AC command sent.");
}


void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
  String msg((char *)data, len);
  Serial.println("\n📥 " + msg);

  // Validate comma count
  int commas = std::count(msg.begin(), msg.end(), ',');
  if (commas != 4) {
    Serial.println("❌ Invalid message format,(expecting 5 fields), not rebroadcast");
    return;
  }

  // Parse 5 fields: sender_id, receiver_id, command, message_type, message_id
  int i1 = msg.indexOf(','),
      i2 = msg.indexOf(',', i1 + 1),
      i3 = msg.indexOf(',', i2 + 1),
      i4 = msg.indexOf(',', i3 + 1);

  String sender   = msg.substring(0, i1);
  String receiver = msg.substring(i1 + 1, i2);
  String command  = msg.substring(i2 + 1, i3);
  String type     = msg.substring(i3 + 1, i4);
  String msg_id   = msg.substring(i4 + 1);

  // Rebroadcast if needed
  rebroadcastIfNeeded(msg_id, type, msg);

  // Check if message is intended for this node
  if (receiver != nodeID) {
    Serial.printf("⏭ Not my message (receiver: %s)\n", receiver.c_str());
    return;
  }

  // Handle ACKs (don't process further)
  if (type == "ack") {
    Serial.println("ℹ️ ACK received, no further processing.");
    return;
  }

  // Prevent duplicate command execution
  if (msg_id == lastCmdID) {
    Serial.println("⚠️ Duplicate CMD ignored.");
    return;
  }
  lastCmdID = msg_id;

  // Show command info
  Serial.println("✅ CMD: " + command);

  // === LED Actions ===
  if (command == "red")        leds[0] = CRGB::Red;
  else if (command == "green") leds[0] = CRGB::Green;
  else if (command == "blue")  leds[0] = CRGB::Blue;
  else if (command == "orange")leds[0] = CRGB::Orange;
  else if (command == "purple")leds[0] = CRGB::Purple;
  else if (command == "yellow")leds[0] = CRGB::Yellow;
  else if (command == "white") leds[0] = CRGB::White;
  else if (command == "off")   leds[0] = CRGB::Black;
  FastLED.show();

  // === Try to parse command as structured AC command ===
  Command ac = parseCommand(command);
  Serial.println("🔍 Parsed Command:");
  Serial.println("  Power On:    " + ac.powerOn);
  Serial.println("  Temperature: " + ac.temperature);
  Serial.println("  Mode:        " + ac.mode);
  Serial.println("  Fan Speed:   " + ac.fanSpeed);
  Serial.println("  Protocol:    " + ac.protocol);
  Serial.println("  V Swing:     " + ac.v_swing);
  Serial.println("  H Swing:     " + ac.h_swing);

  // === Handle AC commands ===
  if (ac.protocol.equalsIgnoreCase("tcl112")) {
  handleTCL112(ac);
  } else if (ac.protocol.equalsIgnoreCase("coolix")) {
    // handleCoolix(ac);
  } else if (ac.protocol.equalsIgnoreCase("goodweather")) {
    //handleGoodweather(ac);  // You’ll create this too
  } else {
    Serial.println("❌ Unsupported protocol: " + ac.protocol);
  }



  // === Send ACK ===
  String ack = String(nodeID) + "," + sender + "," + command + ",ack," + msg_id;
  Serial.println("📤 ACK: " + ack);
  esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());
  // leds[0] = CRGB::Green;  // Indicate ACK with green LED
  // FastLED.show(); 
  // delay(100);  // Short delay to show the green LED
  // leds[0] = CRGB::Black; // Turn off LED after ACK
  // FastLED.show();
}

void setup(){
  Serial.begin(115200);

  /////////////////////
  #if defined(ESP8266)
    Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
    #else
    Serial.begin(kBaudRate, SERIAL_8N1);
    #endif
    while (!Serial);
    assert(irutils::lowLevelSanityCheck() == 0);

  // coolixAC.begin();
  // goodweatherAC.begin();
  // mitsubishiAC.begin();
  tcl112ACS.begin();
  // electraAC.begin();
  irsend.begin();
  ////////////////////
  
  WiFi.mode(WIFI_STA); WiFi.disconnect();
  FastLED.addLeds<NEOPIXEL,LED_PIN>(leds,NUM_LEDS);
  FastLED.setBrightness(50); // Set initial brightness
  leds[0] = CRGB::Orange; FastLED.show();
  delay(1000); // Show orange LED for 1 second
  leds[0]=CRGB::Black; FastLED.show();
  if(esp_now_init()!=ESP_OK){ Serial.println("Init FAIL"); return; }
  esp_now_peer_info_t pi={};
  memcpy(pi.peer_addr,broadcastAddress,6);
  pi.channel=0; pi.encrypt=false;
  esp_now_add_peer(&pi);
  esp_now_register_recv_cb(onReceive);
  Serial.printf("Node %s ready, repeater=%d\n", nodeID, isRepeater);
}

// Function to generate a unique 4-character message ID (hex)
String generateMessageID() {
  uint16_t randNum = esp_random() & 0xFFFF;
  char id[5];
  sprintf(id, "%04X", randNum);
  return String(id);
}

void loop() {
  unsigned long now = millis();

  // 💓 Send heartbeat every 30 seconds
  if (now - lastHBPublishTime >= hbPublishInterval) {
    lastHBPublishTime = now;

    String hb = String(nodeID) + "," + "gw" + "," + "heartbeat" + ",hb," + generateMessageID();
    Serial.println("Heartbeat: " + hb);
    esp_now_send(broadcastAddress, (uint8_t *)hb.c_str(), hb.length());
    leds[0] = CRGB::Blue;  // Indicate heartbeat with yellow LED
    FastLED.show();
    delay(100);  // Short delay to show the yellow LED
    leds[0] = CRGB::Black; // Turn off LED after heartbeat
    FastLED.show();
  }

  delay(50);  // Optional: can remove later for non-blocking loop
}
