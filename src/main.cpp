#include <WiFi.h>
#include <esp_now.h>

const char* nodeID = "node-04";
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  String msg = "";
  for (int i = 0; i < len; i++) {
    msg += (char)incomingData[i];
  }

  int firstComma = msg.indexOf(',');
  int secondComma = msg.indexOf(',', firstComma + 1);

  if (firstComma == -1 || secondComma == -1) return;

  String senderID = msg.substring(0, firstComma);
  String targetNode = msg.substring(firstComma + 1, secondComma);
  String content = msg.substring(secondComma + 1);

  if (targetNode != nodeID) {
    Serial.print("🚫 Not my message. I'm ");
    Serial.print(nodeID);
    Serial.print(", target was ");
    Serial.println(targetNode);
    return;
  }

  // ✅ Target matched — reply
  Serial.print("📨 Received from ");
  Serial.print(senderID);
  Serial.print(": ");
  Serial.println(content);

  delay(random(10, 100));  // Optional anti-collision delay

  String reply = String(nodeID) + "," + senderID + "," + "I am here my dear";
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)reply.c_str(), reply.length());
  Serial.println(result == ESP_OK ? "📤 Sent reply" : "❌ Reply failed");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  randomSeed(esp_random());

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
