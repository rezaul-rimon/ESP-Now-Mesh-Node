#define USE_DS18B20
// #define USE_SHT3X

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>

#if defined(USE_SHT3X)
    #include <Wire.h>
    #include "Adafruit_SHT31.h"
#endif
//=============================================//

// Sensor setup
#if defined(USE_SHT3X)
    Adafruit_SHT31 sht = Adafruit_SHT31();
    bool shtInitialized = false;
#endif
//=============================================//

#if defined(USE_DS18B20)
    // DS18B20 Setup
    #include <OneWire.h>
    #include <DallasTemperature.h>

    #define ONE_WIRE_BUS 21      // DS18B20 Data Pin

    OneWire oneWire(ONE_WIRE_BUS);
    DallasTemperature sensors(&oneWire);

    // Maximum expected sensors (safe upper limit)
    #define MAX_SENSORS 1

    DeviceAddress sensorAddress[MAX_SENSORS];   // Store all sensor addresses
    int sensorCount = 0;
#endif
//=============================================//

#define LED_PIN 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
//=============================================//

const char* nodeID = "11008";
bool isRepeater   = false;
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
//============================================================//

static unsigned long lastHBPublishTime = 0;
const unsigned long hbPublishInterval = 2 * 60 * 1000; // 60 seconds
//===============================================================//

// cache recent rebroadcasts to stop loops
std::deque<String> fwdCache;
const size_t MAX_FWDS=20;
//===============================================================//