#include <config.h>
#include<handleAC.h>

//---Parsing AC commands from string---
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

// Check if a message has already been forwarded
bool alreadyForwarded(const String &key) {
  return std::find(fwdCache.begin(), fwdCache.end(), key) != fwdCache.end();
}

// Record a forwarded message to avoid rebroadcasting
void recordForward(const String &key) {
  fwdCache.push_back(key);
  if (fwdCache.size()>MAX_FWDS) fwdCache.pop_front();
}

// Function to rebroadcast messages if this node is a repeater
void rebroadcastIfNeeded(const String &msg_id, const String &type, const String &raw) {
  if (!isRepeater) {
    DEBUG_PRINTLN("ℹ️ Not a repeater, skipping rebroadcast.");
    return;
  }

  String key = type + ":" + msg_id;
  if (alreadyForwarded(key)) {
    DEBUG_PRINTLN("🔁 Already forwarded: " + key);
    return;
  }

  // Parse fields to extract sender and receiver
  int i1 = raw.indexOf(',');
  int i2 = raw.indexOf(',', i1 + 1);
  if (i1 < 0 || i2 < 0) {
    DEBUG_PRINTLN("❌ Invalid format in rebroadcast check: " + raw);
    return;
  }

  String sender = raw.substring(0, i1);
  String receiver = raw.substring(i1 + 1, i2);

  // Skip CMDs targeted to self
  if (type == "cmd" && receiver == nodeID) {
    DEBUG_PRINTLN("⏭ CMD intended for me (" + receiver + "), not rebroadcasting.");
    return;
  }

// Skip self-originated messages
  if (sender == nodeID) {
    DEBUG_PRINTLN("⏭ Message originated by me (" + sender + "), not rebroadcasting.");
    return;
  }

  // Random short delay to avoid ESP-NOW collision
  delay(random(20, 71));

  esp_now_send(broadcastAddress, (uint8_t *)raw.c_str(), raw.length());
  DEBUG_PRINTLN("🔁 Re-broadcasted: " + raw);
  recordForward(key);
}

// Callback function to handle incoming ESP-NOW messages
//--------------------------//
void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
  String msg((char *)data, len);
  DEBUG_PRINTLN("\n📥 " + msg);

  // Validate comma count
  int commas = std::count(msg.begin(), msg.end(), ',');
  if (commas != 4) {
    DEBUG_PRINTLN("❌ Invalid message format,(expecting 5 fields), not rebroadcast");
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
    DEBUG_PRINTLN("ℹ️ ACK received, no further processing.");
    return;
  }

  // Prevent duplicate command execution
  if (msg_id == lastCmdID) {
    DEBUG_PRINTLN("⚠️ Duplicate CMD ignored.");
    return;
  }
  lastCmdID = msg_id;

  // Show command info
  DEBUG_PRINTLN("✅ CMD: " + command);

  // === LED Actions ===
  if (command == "red")        leds[0] = CRGB::Red;
  else if (command == "green") leds[0] = CRGB::Green;
  else if (command == "blue")  leds[0] = CRGB::Blue;
  else if (command == "orange")leds[0] = CRGB::Orange;
  else if (command == "purple")leds[0] = CRGB::Purple;
  else if (command == "yellow")leds[0] = CRGB::Yellow;
  else if (command == "white") leds[0] = CRGB::White;
  else if (command == "off")   leds[0] = CRGB::Black;
  else goto skip_led_block;  // If no LED match, go check repeater and AC
  FastLED.show();
  return;  // ✅ Stop here if LED command matched

  skip_led_block:

  if (command == "repeater:on") {
    isRepeater = true;

    // ✅ Open Preferences
    preferences.begin("device_config", false);
    preferences.putBool("is_repeater", true);
    preferences.end(); // ✅ Close after writing

    DEBUG_PRINTLN("🔁 Repeater mode enabled (live update).");

    String ack = String(nodeID) + "," + sender + "," + command + ",ack," + msg_id;
    DEBUG_PRINTLN("📤 ACK: " + ack);
    esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());

    // 🔵 1st Blink — ACK (Blue)
    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(300);
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(200);

    // 🟢 2nd Blink — Repeater ON (Green)
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(500);
    leds[0] = CRGB::Black;
    FastLED.show();

    return;

  } else if (command == "repeater:off") {
    isRepeater = false;

    // ✅ Open Preferences
    preferences.begin("device_config", false);
    preferences.putBool("is_repeater", false);
    preferences.end(); // ✅ Close after writing

    DEBUG_PRINTLN("🔁 Repeater mode disabled (live update).");

    String ack = String(nodeID) + "," + sender + "," + command + ",ack," + msg_id;
    DEBUG_PRINTLN("📤 ACK: " + ack);
    esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());

    // 🔵 1st Blink — ACK (Blue)
    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(300);
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(200);

    // 🔴 2nd Blink — Repeater OFF (Red)
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(500);
    leds[0] = CRGB::Black;
    FastLED.show();

    return;
  }

  // === Try to parse command as structured AC command ===
  Command ac = parseCommand(command);
  DEBUG_PRINTLN("🔍 Parsed Command:");
  DEBUG_PRINTLN("  Power On:    " + ac.powerOn);
  DEBUG_PRINTLN("  Temperature: " + ac.temperature);
  DEBUG_PRINTLN("  Mode:        " + ac.mode);
  DEBUG_PRINTLN("  Fan Speed:   " + ac.fanSpeed);
  DEBUG_PRINTLN("  Protocol:    " + ac.protocol);
  DEBUG_PRINTLN("  V Swing:     " + ac.v_swing);
  DEBUG_PRINTLN("  H Swing:     " + ac.h_swing);

  // === Handle AC commands ===
  if (ac.protocol.equalsIgnoreCase("tcl112")) {
  handleTCL112(ac);
  } else if (ac.protocol.equalsIgnoreCase("coolix")) {
    handleCoolix(ac);
  } else if (ac.protocol.equalsIgnoreCase("goodweather")) {
    handleGoodweather(ac);
  } else if (ac.protocol.equalsIgnoreCase("mitsubishi")) {
    handleMitsubishi(ac);
  } else if (ac.protocol.equalsIgnoreCase("electra")) {
    handleElectra(ac);
  } else if (ac.protocol.equalsIgnoreCase("carrier40")) {
    handleCarrierAC40(ac);
  } else if (ac.protocol.equalsIgnoreCase("sanyo")) {
    handleSanyo(ac);
  } else if (ac.protocol.equalsIgnoreCase("panasonic")) {
    handlePanasonic(ac);
  } else if (ac.protocol.equalsIgnoreCase("kelvinator")) {
    handleKelvinator(ac);
  } else if (ac.protocol.equalsIgnoreCase("daikin")) {
    handleDaikin(ac);
  } else if (ac.protocol.equalsIgnoreCase("haier")) {
    handleHaier(ac);
  } else if (ac.protocol.equalsIgnoreCase("lg")) {
    handleLg(ac);
  } else if (ac.protocol.equalsIgnoreCase("gree")) {
    handleGree(ac);
  } else if (ac.protocol.equalsIgnoreCase("mirage")) {
    handleMirage(ac);
  } else if (ac.protocol.equalsIgnoreCase("carrier64")) {
    handleCarrier64(ac);
  }
  else {
    leds[0] = CRGB::DeepPink;  // Indicate error with red LED
    FastLED.show();
    delay(200);  // Show red LED for 0.5 seconds
    leds[0] = CRGB::Black; // Turn off LED after error
    FastLED.show();
    delay(100);
    leds[0] = CRGB::DeepPink;  // Indicate error with red LED
    FastLED.show();
    delay(200);  // Show red LED for 0.5 seconds
    leds[0] = CRGB::Black; // Turn off LED after error
    FastLED.show();
    DEBUG_PRINTLN("❌ Unsupported protocol: " + ac.protocol);
  }



  // === Send ACK ===
  String ack = String(nodeID) + "," + sender + "," + command + ",ack," + msg_id;
  DEBUG_PRINTLN("📤 ACK: " + ack);
  esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());
}

// Function to generate a unique 4-character message ID (hex)
String generateMessageID() {
  uint16_t randNum = esp_random() & 0xFFFF;
  char id[5];
  sprintf(id, "%04X", randNum);
  return String(id);
}


void setup(){
  Serial.begin(115200);

  preferences.begin("device_config", false);  // Open Preferences (NVS)
  static String node_id;
  static bool is_repeater;

  #if CHANGE_DEVICE_CONFIG
    node_id = String(NODE_ID);
    is_repeater = IS_REPEATER;

    preferences.putString("node_id", node_id);
    Serial.println("Device ID updated in Preferences: " + node_id);

    preferences.putBool("is_repeater", IS_REPEATER);
    Serial.println("Repeater status updated in Preferences: " + String(IS_REPEATER));
  #else
    node_id = preferences.getString("node_id", "UNKNOWN");
    is_repeater = preferences.getBool("is_repeater", false);

    Serial.println("Restored Device ID from Preferences: " + node_id);
    Serial.println("Repeater status restored from Preferences: " + String(is_repeater));
  #endif

  strncpy(nodeID, node_id.c_str(), sizeof(nodeID));  // Safely copy to global buffer
  nodeID[sizeof(nodeID) - 1] = '\0'; // Ensure null-termination
  isRepeater = is_repeater;

  preferences.end();

  /////////////////////
  #if defined(ESP8266)
    Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
    #else
    Serial.begin(kBaudRate, SERIAL_8N1);
    #endif
    while (!Serial);
    assert(irutils::lowLevelSanityCheck() == 0);

  tcl112ACS.begin();
  coolixAC.begin();
  goodweatherAC.begin();
  electraAC.begin();
  mitsubishiAC.begin();

  irsend.begin();
  ////////////////////
  
  WiFi.mode(WIFI_STA); WiFi.disconnect();
  FastLED.addLeds<NEOPIXEL,LED_PIN>(leds,NUM_LEDS);
  FastLED.setBrightness(150); // Set initial brightness
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

void loop() {
  unsigned long now = millis();

  // 💓 Send heartbeat every 30 seconds
  if (now - lastHBPublishTime >= hbPublishInterval) {
    lastHBPublishTime = now;

    String hb = String(nodeID) + ",gw,heartbeat/R:" + (isRepeater ? "1" : "0") + ",hb," + generateMessageID();
    DEBUG_PRINTLN("Heartbeat: " + hb);
    esp_now_send(broadcastAddress, (uint8_t *)hb.c_str(), hb.length());

    leds[0] = CRGB::Blue;  // Indicate heartbeat with yellow LED
    FastLED.show();
    delay(200);  // Short delay to show the yellow LED
    leds[0] = CRGB::Black; // Turn off LED after heartbeat
    FastLED.show();
    delay(100);
    leds[0] = CRGB::Blue;  // Indicate heartbeat with yellow LED
    FastLED.show();
    delay(200);  // Short delay to show the yellow LED
    leds[0] = CRGB::Black; // Turn off LED after heartbeat
    FastLED.show();
  }

  delay(50);  // Optional: can remove later for non-blocking loop
}
