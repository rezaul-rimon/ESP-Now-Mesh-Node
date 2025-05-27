#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const char* nodeID = "node01";  // Unique ID for this node

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  String msg = "";
  for (int i = 0; i < len; i++) {
    msg += (char)incomingData[i];
  }

  int separator = msg.indexOf(',');
  if (separator != -1) {
    String gatewayID = msg.substring(0, separator);
    String content = msg.substring(separator + 1);
    Serial.print("📡 From Gateway: ");
    Serial.println(gatewayID);
    Serial.print("📩 Message: ");
    Serial.println(content);

    // 🔁 If received "hello-node", respond
    if (content == "hello-node") {
      String reply = String(nodeID) + "," + "I am here";
      esp_now_send(broadcastAddress, (uint8_t *)reply.c_str(), reply.length());
      Serial.println("📤 Sent response: I am here");
    }
  } else {
    Serial.println("⚠️ Malformed message from gateway.");
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

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (!esp_now_add_peer(&peerInfo)) {
    Serial.println("Broadcast peer added.");
  }

  esp_now_register_recv_cb(onReceive);  // ✅ Receive from gateway
  Serial.println("Node Ready. Waiting for messages...");
}

void loop() {
  // Nothing here – everything happens in callback
}
