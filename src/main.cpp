#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>

// ------------------- Configuration -------------------
#define LED_PIN     4       // Pin connected to RGB LED
#define NUM_LEDS    1       // Number of LEDs
CRGB leds[NUM_LEDS];        // LED buffer

const char* nodeID = "node-00";   // Unique ID for this node

// Broadcast address for ESP-NOW communication (send to all)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ------------------- Message Structure -------------------
struct Message {
  String gw_id;
  String node_id;
  String command;
  String type;      // "cmd" or "ack"
  String msg_id;
};

// ------------------- Parse CSV Message into Struct -------------------
Message parseMessage(const String& msg) {
  Message m;
  int idx1 = msg.indexOf(',');
  int idx2 = msg.indexOf(',', idx1 + 1);
  int idx3 = msg.indexOf(',', idx2 + 1);
  int idx4 = msg.indexOf(',', idx3 + 1);

  if (idx1 > 0 && idx2 > idx1 && idx3 > idx2 && idx4 > idx3) {
    m.gw_id   = msg.substring(0, idx1);
    m.node_id = msg.substring(idx1 + 1, idx2);
    m.command = msg.substring(idx2 + 1, idx3);
    m.type    = msg.substring(idx3 + 1, idx4);
    m.msg_id  = msg.substring(idx4 + 1);
  }

  return m;
}

// ------------------- Callback on Data Receive -------------------
void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  String msg = "";
  for (int i = 0; i < len; i++) msg += (char)incomingData[i];
  Serial.println("📥 Data Received: " + msg);
  // Parse and validate message
  Message m = parseMessage(msg);

  if (m.node_id != nodeID || m.type != "cmd") {
    Serial.println("⏩ Ignored: Not for me or not CMD => " + msg);
    return;
  }

  Serial.println("📩 CMD Received: " + m.command);

  // ------------------- Handle LED Command -------------------
  if (m.command == "red")       leds[0] = CRGB::Red;
  else if (m.command == "green") leds[0] = CRGB::Green;
  else if (m.command == "blue")  leds[0] = CRGB::Blue;
  else                           leds[0] = CRGB::Black;
  FastLED.show();

  // ------------------- Send ACK Back -------------------
  String ack = String(nodeID) + "," + m.command + ",ack," + m.msg_id;
  esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());
  Serial.println("📤 Sent ACK: " + ack);
}

// ------------------- Setup -------------------
void setup() {
  Serial.begin(115200);

  // Initialize LED (off state)
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  leds[0] = CRGB::Black;
  FastLED.show();

  // Setup WiFi in Station mode (required for ESP-NOW)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Not using standard WiFi
  randomSeed(esp_random()); // Seed RNG

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW Init Failed");
    return;
  }

  // Add broadcast peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; // Default channel
  peerInfo.encrypt = false;
  if (!esp_now_add_peer(&peerInfo)) {
    Serial.println("✅ Broadcast peer added.");
  }

  // Register receive callback
  esp_now_register_recv_cb(onReceive);

  Serial.print("✅ Node Ready: ");
  Serial.println(nodeID);
}

// ------------------- Main Loop -------------------
void loop() {
  // Nothing needed in loop for now
}
