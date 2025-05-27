#include <WiFi.h>
#include <esp_now.h>

String lastMessage = "";  // 🔸 Variable to store the last received message

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  lastMessage = "";  // Clear previous message
  for (int i = 0; i < len; i++) {
    lastMessage += (char)incomingData[i];
  }

  Serial.print("📩 Received message: ");
  Serial.println(lastMessage);

  // ✅ You can now act based on lastMessage
  // Example: if (lastMessage == "red") { turnOnRedLED(); }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
  Serial.println("Node Ready. Waiting for messages...");
}

void loop() {
  // You can check `lastMessage` here if needed
  // Example: if (lastMessage == "green") { ... }
}
