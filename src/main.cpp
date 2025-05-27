#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#define LED_PIN     4
#define NUM_LEDS    1
CRGB leds[NUM_LEDS];


const char* nodeID = "node-05";
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  String msg = "";
  for (int i = 0; i < len; i++) {
    msg += (char)incomingData[i];
  }

  int firstComma = msg.indexOf(',');
  int secondComma = msg.indexOf(',', firstComma + 1);

  if (firstComma > 0 && secondComma > firstComma) {
    String gwID = msg.substring(0, firstComma);
    String targetNode = msg.substring(firstComma + 1, secondComma);
    String command = msg.substring(secondComma + 1);

    if (targetNode == nodeID) {
      Serial.print("📩 Command from ");
      Serial.print(gwID);
      Serial.print(": ");
      Serial.println(command);

      // Set color based on command
      if (command == "red")       leds[0] = CRGB::Red;
      else if (command == "green") leds[0] = CRGB::Green;
      else if (command == "blue")  leds[0] = CRGB::Blue;
      else                         leds[0] = CRGB::Black;

      FastLED.show();

      // Send back reply
      String reply = String(nodeID) + ",I am " + command;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)reply.c_str(), reply.length());
      Serial.println(result == ESP_OK ? "✅ Reply sent!" : "❌ Reply failed");
    } else {
      Serial.print("⏩ Message not for me: ");
      Serial.println(msg);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  randomSeed(esp_random());

  // Setup LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  leds[0] = CRGB::Black;
  FastLED.show();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (!esp_now_add_peer(&peerInfo)) {
    Serial.println("Broadcast peer added.");
  }

  esp_now_register_recv_cb(onReceive);
  Serial.print("✅ Node Ready: ");
  Serial.println(nodeID);
}

void loop() {
  // No loop logic required
}
