#include <config.h>

#ifdef USE_NTC_SENSOR
  #define NTC_PIN 35  // GPIO35 for NTC sensor
  #define NTC_ADC_MAX 4095.0
  #define NTC_VREF 3.3
  #define NTC_SERIES_RESISTOR 4500.0
  #define NTC_NOMINAL_RESISTANCE 10000.0
  #define NTC_NOMINAL_TEMPERATURE 25.0
  #define NTC_BETA 3950.0
  #define NTC_SAMPLE_COUNT 20
  #define NTC_OFFSET_TEMP 3.5f

  NTC ntcSensor(NTC_PIN, NTC_ADC_MAX, NTC_VREF, NTC_SERIES_RESISTOR, NTC_NOMINAL_RESISTANCE, NTC_NOMINAL_TEMPERATURE, NTC_BETA, NTC_SAMPLE_COUNT, NTC_OFFSET_TEMP);
#endif

// Check if a message has already been forwarded
bool alreadyForwarded(const String &key) {
  return std::find(fwdCache.begin(), fwdCache.end(), key) != fwdCache.end();
}

// Record a forwarded message to prevent rebroadcasting
void recordForward(const String &key) {
  fwdCache.push_back(key);
  if (fwdCache.size()>MAX_FWDS) fwdCache.pop_front();
}

// Function to rebroadcast messages if needed
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
  // leds[0] = CRGB::White;  // Indicate rebroadcast with blue LED
  // FastLED.show();
  //delay(100);  // Short delay to show the blue LED
  // leds[0] = CRGB::Black; // Turn off LED after rebroadcast  
  // FastLED.show();
}

// Callback function to handle incoming ESP-NOW messages
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

  // ❌ Do NOT process any command or ACK
  if (receiver == nodeID) {
    Serial.printf("ℹ️ Message is for me, but this node is a temp-hub. Ignoring command.\n");
  }
}

#if defined(USE_SHT3X)
  String readTemperature() {
    if (!shtInitialized) return "NA/NA";

    float temp = sht.readTemperature();
    float hum  = sht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {
      return String(temp, 2) + "/" + String(hum, 2);
    } else {
      return "ERR/ERR";
    }
  }
#endif

#if defined(USE_DS18B20)
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
#endif

// Setup function to initialize everything
void setup(){
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL,LED_PIN>(leds,NUM_LEDS);
  FastLED.setBrightness(50); // Set initial brightness
  leds[0] = CRGB::Orange; FastLED.show();
  delay(500); // Show orange LED for 1 second
  leds[0]=CRGB::Black; FastLED.show();

  // First attempt
  #if defined(USE_SHT3X)
    if (sht.begin(0x44)) {
      shtInitialized = true;
      Serial.println("✅ SHT3x sensor initialized.");
      leds[0] = CRGB::Green; FastLED.show();
      delay(1000);
      leds[0] = CRGB::Black; FastLED.show();
    } else {
      Serial.println("⏳ SHT3x not found, will retry in loop.");
    }
  #endif

  //USE DS18B20
  #if defined(USE_DS18B20)
    sensors.begin();
    Serial.println("Searching for DS18B20 sensors...");
    sensorCount = sensors.getDeviceCount();

    Serial.print("Found ");
    Serial.print(sensorCount);
    Serial.println(" sensor(s).");

    // if (sensorCount == 0) {
    //     Serial.println("No sensors detected!");
    //     return;
    // }

    while(sensorCount == 0) {
      Serial.println("No sensors detected!");
      leds[0] = CRGB::Red;
      FastLED.show();
      delay(500);
      leds[0] = CRGB::Black;
      FastLED.show();
      delay(500);
    }

    // Store addresses at startup
    for (int i = 0; i < sensorCount; i++) {
        if (sensors.getAddress(sensorAddress[i], i)) {
          Serial.print("Sensor ");
          Serial.print(i);
          Serial.print(" Address: ");
          Serial.println(addressToString(sensorAddress[i]));

          leds[0] = CRGB::Green; 
          FastLED.show();
          delay(300);
          leds[0] = CRGB::Black; 
          FastLED.show();
          delay(200);
        } else {
          Serial.print("Could not read address for sensor ");
          Serial.println(i);
        }
    }
  #endif

  pinMode(BTN_PIN, INPUT_PULLUP);
  
  WiFi.mode(WIFI_STA); WiFi.disconnect();
  
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

// Main loop function
void loop() {
  
  // 🔴 Handle sensor reinitialization & LED blinking if not ready
  #if defined(USE_SHT3X)
    if (!shtInitialized) {
      static unsigned long lastAttempt = 0;
      static unsigned long lastBlink = 0;
      static bool ledOn = false;

      // 🔄 Retry sensor init every 10 seconds
      if (millis() - lastAttempt > 10000) {
        Serial.println("🔄 Retrying SHT3x init...");
        if (sht.begin(0x44)) {
          shtInitialized = true;
          Serial.println("✅ SHT3x initialized during loop.");
          leds[0] = CRGB::Green;
          FastLED.show();
          delay(1000);
          leds[0] = CRGB::Black;
          FastLED.show();
        }
        lastAttempt = millis();
      }

      // 🔴 Blink red LED every 500ms
      if (millis() - lastBlink > 500) {
        lastBlink = millis();
        ledOn = !ledOn;
        leds[0] = ledOn ? CRGB::Red : CRGB::Black;
        FastLED.show();
      }
    }
  #endif

  // 📤 Send sensor data (or error) every 30 seconds
  unsigned long now = millis();
  if (now - lastHBPublishTime >= hbPublishInterval || (digitalRead(BTN_PIN) == LOW) && btn == false) {
    lastHBPublishTime = now;
    btn = true;

    #if defined(USE_SHT3X)
      String tempHum;
      if (shtInitialized) {
        tempHum = readTemperature();  // Returns "25.66/66.58" or similar
      } else {
        tempHum = "Error/Error";
      }
      String msg = String(nodeID) + ",gw," + tempHum + ",tmp," + generateMessageID();

      Serial.println("📤 Sending Temp: " + msg);
      esp_now_send(broadcastAddress, (uint8_t *)msg.c_str(), msg.length());
    #endif

    #if defined(USE_DS18B20)
      sensors.requestTemperatures();   // Trigger conversion

      for (int i = 0; i < sensorCount; i++) {
        float temperature = sensors.getTempC(sensorAddress[i]);

        String id = addressToString(sensorAddress[i]);

        Serial.print(id);
        Serial.print(",");
        Serial.println(temperature);   // Print exactly as requested

        delay(100); // Small delay between readings

        String msg = String(nodeID) + ",gw," + String(temperature, 2) + "/55.55" ",tmp," + generateMessageID();

        Serial.println("📤 Sending Temp: " + msg);
        esp_now_send(broadcastAddress, (uint8_t *)msg.c_str(), msg.length());
        delay(250);  // Small delay between sends
      }
    #endif

    #if defined(USE_NTC_SENSOR)
      // NTC Sensor Reading
      float temp = ntcSensor.readTemperature();

      String tempStr = String(temp, 2);
      String msg = String(nodeID) + ",gw," + tempStr + "/55.66" ",tmp," + generateMessageID();

      Serial.println("📤 Sending Temp: " + msg);
      esp_now_send(broadcastAddress, (uint8_t *)msg.c_str(), msg.length());
    #endif

    // 🔵 Blink blue LED briefly to show transmission
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(250);
    leds[0] = CRGB::Black;
    FastLED.show();
  }

  if (digitalRead(BTN_PIN) == HIGH) {
    btn = false;
  }

  delay(10);  // Minimal blocking
}
