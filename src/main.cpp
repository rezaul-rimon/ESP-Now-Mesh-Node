#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>

// Include the necessary libraries for IR control
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define LED_PIN 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

const char* nodeID = "temp-05";
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

void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
  String msg((char *)data, len);
  Serial.println("\n📥 " + msg);

  // Validate comma count (expecting 5 fields)
  int commas = std::count(msg.begin(), msg.end(), ',');
  if (commas != 4) {
    Serial.println("❌ Invalid message format, not rebroadcasted");
    return;
  }

  // Parse 5 fields
  int i1 = msg.indexOf(',');
  int i2 = msg.indexOf(',', i1 + 1);
  int i3 = msg.indexOf(',', i2 + 1);
  int i4 = msg.indexOf(',', i3 + 1);

  String sender   = msg.substring(0, i1);
  String receiver = msg.substring(i1 + 1, i2);
  String command  = msg.substring(i2 + 1, i3);
  String type     = msg.substring(i3 + 1, i4);
  String msg_id   = msg.substring(i4 + 1);

  // 🔁 Rebroadcast any valid message (cmd, ack, tmp, etc.)
  rebroadcastIfNeeded(msg_id, type, msg);

  // ❌ Do NOT process any command or ACK
  if (receiver == nodeID) {
    Serial.printf("ℹ️ Message is for me, but this node is a temp-hub. Ignoring command.\n");
  }
}

String readTemperature() {
  // Simulated temperature and humidity values with two decimal places
  float temp = random(2000, 3000) / 100.0; // 20.00 to 30.00 °C
  float hum  = random(5000, 8000) / 100.0; // 50.00% to 80.00% RH

  return String(temp, 2) + "/" + String(hum, 2);
}


void setup(){
  Serial.begin(115200);
  
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

  // 💓 Send temperature every 30 seconds
  if (now - lastHBPublishTime >= hbPublishInterval) {
    lastHBPublishTime = now;

    String temp = readTemperature();  // Replace with your actual reading
    String msg = String(nodeID) + ",gw," + temp + ",tmp," + generateMessageID();

    Serial.println("📤 Sending Temp: " + msg);
    esp_now_send(broadcastAddress, (uint8_t *)msg.c_str(), msg.length());

    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(100);
    leds[0] = CRGB::Black;
    FastLED.show();
  }

  delay(10);  // Minimal blocking
}
