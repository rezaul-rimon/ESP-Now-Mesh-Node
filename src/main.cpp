#include <config.h>

//---Parsing AC commands from string---
// Command parseCommand(const String &cmdStr) {
//   Command c;
//   int start = 0, idx;
//   int field = 0;
//   while ((idx = cmdStr.indexOf('/', start)) != -1 && field < 6) {
//     String part = cmdStr.substring(start, idx);
//     switch (field) {
//       case 0: c.powerOn     = part; break;
//       case 1: c.temperature = part; break;
//       case 2: c.mode        = part; break;
//       case 3: c.fanSpeed    = part; break;
//       case 4: c.protocol    = part; break;
//       case 5: c.v_swing     = part; break;
//     }
//     start = idx + 1;
//     field++;
//   }
//   // Last segment (rest of the string)
//   String last = cmdStr.substring(start);
//   if      (field == 0) c.powerOn     = last;
//   else if (field == 1) c.temperature = last;
//   else if (field == 2) c.mode        = last;
//   else if (field == 3) c.fanSpeed    = last;
//   else if (field == 4) c.protocol    = last;
//   else if (field == 5) c.v_swing     = last;
//   else                 c.h_swing     = last;
//   return c;
// }

// cache last handled CMD id to avoid dup exec
String lastCmdID;
// cache recent rebroadcasts to stop loops
std::deque<String> fwdCache;
const size_t MAX_FWDS=20;

// Function to generate a unique 4-character message ID (hex)
String generateMessageID() {
  uint16_t randNum = esp_random() & 0xFFFF;
  char id[5];
  sprintf(id, "%04X", randNum);
  return String(id);
}

// Convert address to string format (HEX 16 chars)
String addressToString(const DeviceAddress deviceAddress) {
  String id = "";
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) id += "0";
    id += String(deviceAddress[i], HEX);
  }
  id.toUpperCase();
  return id;
}

// Function to send heartbeat message
void SendHeartBeat() {
  String hb = String(nodeID) + ",gw,heartbeat/R:" + (isRepeater ? "1" : "0") + ",c_hb," + generateMessageID();
  DEBUG_PRINTLN("Heartbeat: " + hb);
  esp_now_send(broadcastAddress, (uint8_t *)hb.c_str(), hb.length());

  #ifdef USE_FastLED
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
  #endif
}

//Send NTC Data
#ifdef USE_NTC
  void sendNTCData() {
    // Placeholder for future NTC data sending logic
    int adcValue = analogRead(ADC_PIN);

      // Convert ADC to voltage
      float voltage = adcValue * (3.3 / 4095.0);

      // Calculate thermistor resistance
      float resistance = SERIES_RESISTOR * ((3.3 / voltage) - 1);

      // Temperature calculation using Beta formula
      float steinhart;
      steinhart = resistance / NOMINAL_RESISTANCE;     // (R/R0)
      steinhart = log(steinhart);                      // ln(R/R0)
      steinhart /= BETA;                               // 1/B * ln(R/R0)
      steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + 1/T0
      steinhart = 1.0 / steinhart;                     // Invert
      float temperatureC = steinhart - 273.15;         // Kelvin → °C

      // Serial.print("ADC: ");
      // Serial.print(adcValue);
      // Serial.print("  Voltage: ");
      // Serial.print(voltage, 3);
      // Serial.print(" V  Temp: ");
      // Serial.print(temperatureC, 2);
      // Serial.println(" °C");

      String ntcMsg = String(nodeID) + ",gw,NTC/" + String(temperatureC, 2) + ",c_tmp," + generateMessageID();
      DEBUG_PRINTLN("NTC Temperature Message: " + ntcMsg);
      esp_now_send(broadcastAddress, (uint8_t *)ntcMsg.c_str(), ntcMsg.length());

      #ifdef USE_FastLED
        leds[0] = CRGB::Blue;  // Indicate heartbeat with yellow LED
        FastLED.show();
        delay(200);  // Short delay to show the yellow LED
        leds[0] = CRGB::Black; // Turn off LED after heartbeat
        FastLED.show();
        delay(100);
        leds[0] = CRGB::Green;  // Indicate heartbeat with yellow LED
        FastLED.show();
        delay(200);  // Short delay to show the yellow LED
        leds[0] = CRGB::Black; // Turn off LED after heartbeat
        FastLED.show();
      #endif
  }
#endif
//==========================//

// Function to send temperature data
#ifdef USE_DS18B20
  void SendTemperatureData() {
    // Placeholder for future data sending logic
    sensors.requestTemperatures();   // Trigger conversion
    for (int i = 0; i < sensorCount; i++) {
      float temperature = sensors.getTempC(sensorAddress[i]);
      String id = addressToString(sensorAddress[i]);
      Serial.print(id);
      Serial.print(",");
      Serial.println(temperature);   // Print exactly as requested

      String tempMsg = String(nodeID) + ",gw," + id + "/" + String(temperature, 2) + ",c_tmp," + generateMessageID();
      DEBUG_PRINTLN("Temperature Message: " + tempMsg);
      esp_now_send(broadcastAddress, (uint8_t *)tempMsg.c_str(), tempMsg.length());
      delay(100); // Short delay between messages
    }

    #ifdef USE_FastLED
      leds[0] = CRGB::Blue;  // Indicate heartbeat with yellow LED
      FastLED.show();
      delay(200);  // Short delay to show the yellow LED
      leds[0] = CRGB::Black; // Turn off LED after heartbeat
      FastLED.show();
      delay(100);
      leds[0] = CRGB::Green;  // Indicate heartbeat with yellow LED
      FastLED.show();
      delay(200);  // Short delay to show the yellow LED
      leds[0] = CRGB::Black; // Turn off LED after heartbeat
      FastLED.show();
    #endif

    Serial.println("----------------------------");
  }

#endif
//===================================================//

//Function for null check
#ifdef USE_PZEM004T
String safeValue(float val, uint8_t decimals = 2) {
  if (isnan(val)){
    if(isEspRestarted == false){
      isEspRestarted = true;
      ESP.restart();
    }
    return "nan";
  } 
    

  char buff[16];
  dtostrf(val, 0, decimals, buff);  
  return String(buff);
}

//Function for Energy Data send.
void SendEnergyData() {
  float voltage    = pzem.voltage();
  float current    = pzem.current();
  float power      = pzem.power();
  float energy     = pzem.energy();
  float frequency  = pzem.frequency();
  float pf         = pzem.pf();

  Serial.println("PZEM Readings:");
  Serial.printf(" Voltage: %s V\n",      safeValue(voltage, 2).c_str());
  Serial.printf(" Current: %s A\n",      safeValue(current, 2).c_str());
  Serial.printf(" Power:   %s W\n",      safeValue(power, 2).c_str());
  Serial.printf(" Power Factor: %s\n",   safeValue(pf, 2).c_str());
  Serial.printf(" Energy:  %s Wh\n",     safeValue(energy, 2).c_str());
  Serial.printf(" Frequency: %s Hz\n",   safeValue(frequency, 0).c_str());
  Serial.println("----------------------------\n");

  String pzemMsg = String(nodeID) + ",gw," +
                    safeValue(energy, 2)    + "/" +
                    safeValue(voltage, 2)   + "/" +
                    safeValue(current, 2)   + "/" +
                    safeValue(power, 2)     + "/" +
                    safeValue(pf, 2)        + "/" +
                    safeValue(frequency, 0) +
                    ",c_em," +
                    generateMessageID();

  DEBUG_PRINTLN("PZEM Data Message: " + pzemMsg);

  esp_now_send(broadcastAddress, (uint8_t *)pzemMsg.c_str(), pzemMsg.length());

  #ifdef USE_FastLED
    leds[0] = CRGB::Blue;  // Indicate heartbeat with yellow LED
    FastLED.show();
    delay(200);  // Short delay to show the yellow LED
    leds[0] = CRGB::Black; // Turn off LED after heartbeat
    FastLED.show();
    delay(100);
    leds[0] = CRGB::Green;  // Indicate heartbeat with yellow LED
    FastLED.show();
    delay(200);  // Short delay to show the yellow LED
    leds[0] = CRGB::Black; // Turn off LED after heartbeat
    FastLED.show();
  #endif
}
#endif
//--------------------------//

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
  #ifdef USE_FastLED
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
  #endif

  if (command == "repeater:on") {
    isRepeater = true;

    // ✅ Open Preferences
    preferences.begin("device_config", false);
    preferences.putBool("is_repeater", true);
    preferences.end(); // ✅ Close after writing

    DEBUG_PRINTLN("🔁 Repeater mode enabled (live update).");

    String ack = String(nodeID) + "," + sender + "," + command + ",c_ack," + msg_id;
    DEBUG_PRINTLN("📤 ACK: " + ack);
    esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());

    #ifdef USE_FastLED
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
    #endif

    return;

  } else if (command == "repeater:off") {
    isRepeater = false;

    // ✅ Open Preferences
    preferences.begin("device_config", false);
    preferences.putBool("is_repeater", false);
    preferences.end(); // ✅ Close after writing

    DEBUG_PRINTLN("🔁 Repeater mode disabled (live update).");

    String ack = String(nodeID) + "," + sender + "," + command + ",c_ack," + msg_id;
    DEBUG_PRINTLN("📤 ACK: " + ack);
    esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());

    #ifdef USE_FastLED
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
    #endif

    return;
  }

  if(command == "ping") {
    // Respond to ping command
    DEBUG_PRINTLN("🏓 Ping received, sending data...");
    String pong = String(nodeID) + "," + sender + "," + "available" + ",c_ack," + msg_id;
    DEBUG_PRINTLN("📤 Pong: " + pong);
    esp_now_send(broadcastAddress, (uint8_t *)pong.c_str(), pong.length());
    DEBUG_PRINTLN("🏓 Data sent in response to ping.");

    return;
  }

  if(command == "data") {

    // Send all sensor data
    #ifdef USE_DS18B20
      SendTemperatureData();
      delay(500);
    #endif

    // Send PZEM-004T data
    #ifdef USE_PZEM004T
      SendEnergyData();
      delay(500);
    #endif

    // Send NTC data
    #ifdef USE_NTC
      sendNTCData();
    #endif

    return;
  }

  if(command == "hb") {
    // Send Heartbeat
    SendHeartBeat();
    return;

  }

  if(command == "restart") {
    DEBUG_PRINTLN("🔄 Restart command received, restarting...");
    String restartAck = String(nodeID) + "," + sender + "," + ",restarted!," + ",c_ack," + msg_id;
    DEBUG_PRINTLN("📤 RestartAck: " + restartAck);
    esp_now_send(broadcastAddress, (uint8_t *)restartAck.c_str(), restartAck.length());\

    #ifdef USE_FastLED
      leds[0] = CRGB::Red;  // Indicate restart with purple LED
      FastLED.show();
      delay(500);  // Short delay to show the purple LED
      leds[0] = CRGB::Black; // Turn off LED after restart indication
      FastLED.show();
    #endif
    delay(500); // Give time for restartAck to be sent
    ESP.restart();
  }

  // === Try to parse command as structured AC command ===
  // Command ac = parseCommand(command);
  // DEBUG_PRINTLN("🔍 Parsed Command:");
  // DEBUG_PRINTLN("  Power On:    " + ac.powerOn);
  // DEBUG_PRINTLN("  Temperature: " + ac.temperature);
  // DEBUG_PRINTLN("  Mode:        " + ac.mode);
  // DEBUG_PRINTLN("  Fan Speed:   " + ac.fanSpeed);
  // DEBUG_PRINTLN("  Protocol:    " + ac.protocol);
  // DEBUG_PRINTLN("  V Swing:     " + ac.v_swing);
  // DEBUG_PRINTLN("  H Swing:     " + ac.h_swing);


  // === Send ACK ===
  // String ack = String(nodeID) + "," + sender + "," + command + ",ack," + msg_id;
  // DEBUG_PRINTLN("📤 ACK: " + ack);
  // esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());
}

//================= Setup & Loop =================//
void setup(){
  Serial.begin(115200);

  //Device ID Setup
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
  //✅ Close Preferences after reading

  WiFi.mode(WIFI_STA); WiFi.disconnect();

  #ifdef USE_FastLED
    FastLED.addLeds<NEOPIXEL,LED_PIN>(leds,NUM_LEDS);
    FastLED.setBrightness(150); // Set initial brightness
    leds[0] = CRGB::Orange; FastLED.show();
    delay(1000); // Show orange LED for 1 second
    leds[0]=CRGB::Black; FastLED.show();
  #endif

  if(esp_now_init()!=ESP_OK){ Serial.println("Init FAIL"); return; }

  esp_now_peer_info_t pi={};
  memcpy(pi.peer_addr,broadcastAddress,6);
  pi.channel=0; 
  pi.encrypt=false;
  esp_now_add_peer(&pi);
  esp_now_register_recv_cb(onReceive);
  Serial.printf("Node %s ready, repeater=%d\n", nodeID, isRepeater);

  Serial.println("----------------------------");
  Serial.println();

  //=================================================================

  //=== Initialize DS18B20 Sensors ===//
  #ifdef USE_DS18B20
    sensors.begin();
    Serial.println("Searching for DS18B20 sensors...");
    sensorCount = sensors.getDeviceCount();

    Serial.print("Found ");
    Serial.print(sensorCount);
    Serial.println(" sensor(s).");

    if (sensorCount == 0) {
      Serial.println("No sensors detected!");
      return;
    }

    // Store addresses at startup
    for (int i = 0; i < sensorCount; i++) {
      if (sensors.getAddress(sensorAddress[i], i)) {
        Serial.print("Sensor ");
        Serial.print(i);
        Serial.print(" Address: ");
        Serial.println(addressToString(sensorAddress[i]));
      } else {
        Serial.print("Could not read address for sensor ");
        Serial.println(i);
      }
    }
    Serial.println("----------------------------");
    Serial.println();
  #endif
  //=======================================================

  //=== Initialize PZEM-004T Sensor ===//
  #ifdef USE_PZEM004T
    Serial.println("Initializing PZEM-004T V3.0 Power Meter...");
    Serial.print("Custom Address: ");
    Serial.println(pzem.readAddress(), HEX);
  #endif

  Serial.println("----------------------------");
  Serial.println();
}

// Main loop
void loop() {
  unsigned long now = millis();
  // 💓 Send heartbeat every 30 seconds

  if ((now - lastHBPublishTime >= hbPublishInterval) || (isButtonPressed == false && digitalRead(0) == LOW)) {
    lastHBPublishTime = now; 

    if(digitalRead(0)==LOW) {
    isButtonPressed = true;
    }
    SendHeartBeat();
  }

  if(digitalRead(0)==HIGH) {
    isButtonPressed = false;
  }
  //===============================================


  if(now-lastDataPublishTime >= dataPublishInterval){
    lastDataPublishTime = now;

    #ifdef USE_DS18B20
      SendTemperatureData();
      delay(500);
    #endif

    #ifdef USE_PZEM004T
      SendEnergyData();
      delay(500);
    #endif

    #ifdef USE_NTC
      sendNTCData();
    #endif
  }

  delay(100);  // Optional: can remove later for non-blocking loop
}

