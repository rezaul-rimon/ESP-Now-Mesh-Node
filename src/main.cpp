#include <WiFi.h>
#include <esp_now.h>

String lastMessage = "";

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  lastMessage = "";
  for (int i = 0; i < len; i++) {
    lastMessage += (char)incomingData[i];
  }

  // ✅ Extract Gateway ID and Message
  int separatorIndex = lastMessage.indexOf(',');
  if (separatorIndex != -1) {
    String gatewayID = lastMessage.substring(0, separatorIndex);
    String message = lastMessage.substring(separatorIndex + 1);

    Serial.print("📡 From Gateway: ");
    Serial.println(gatewayID);
    Serial.print("📩 Message: ");
    Serial.println(message);
  } else {
    Serial.println("⚠️ Malformed message received!");
  }
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
  // Idle loop
}
