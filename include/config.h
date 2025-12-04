#define USE_DS18B20
// #define USE_NTC
// #define USE_PZEM004T
#define USE_FastLED

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PZEM004Tv30.h>

//=============================================

//PZEM-004T Setup
#ifdef USE_PZEM004T
  #define PZEM_RX_PIN 16  // ESP32 RX (Connect to PZEM TX)
  #define PZEM_TX_PIN 17  // ESP32 TX (Connect to PZEM RX)
  PZEM004Tv30 pzem(Serial2, PZEM_RX_PIN, PZEM_TX_PIN);
  bool isEspRestarted = false;
#endif
//=============================================

// DS18B20 Setup
#ifdef USE_DS18B20
  #define ONE_WIRE_BUS 21      // DS18B20 Data Pin

  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire);

  // Maximum expected sensors (safe upper limit)
  #define MAX_SENSORS 5

  DeviceAddress sensorAddress[MAX_SENSORS];   // Store all sensor addresses
  int sensorCount = 0;
#endif
//=============================================

//NTC Setup
#ifdef USE_NTC
  #define ADC_PIN 36
  #define SERIES_RESISTOR 10000.0   // fixed resistor 10k
  #define NOMINAL_RESISTANCE 10000.0 // NTC value at 25°C
  #define NOMINAL_TEMPERATURE 25.0   // 25°C
  #define BETA 3950.0
#endif
//=============================================

Preferences preferences;
bool isButtonPressed = false; // Global flag for button press state

char nodeID[16];     // Global, mutable buffer
bool isRepeater = false;

#define CHANGE_DEVICE_CONFIG 0

#if CHANGE_DEVICE_CONFIG
  #define IS_REPEATER false
  #define NODE_ID "C0006"
#endif

//Device Setup
// const char* nodeID = "01001";
// bool isRepeater   = false;
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

unsigned long lastHBPublishTime = 0;
const unsigned long hbPublishInterval = 1 * 60 * 1000;

unsigned long lastDataPublishTime = 0;
const unsigned long dataPublishInterval = 2 * 60 * 1000;

#define DEBUG_MODE true
#define DEBUG_PRINT(x)  if (DEBUG_MODE) { Serial.print(x); }
#define DEBUG_PRINTF(x)  if (DEBUG_MODE) { Serial.printf(x); }
#define DEBUG_PRINTLN(x) if (DEBUG_MODE) { Serial.println(x); }


#ifdef USE_FastLED
  //Status LED Pin Setup
  #define LED_PIN 4
  #define NUM_LEDS 1
  CRGB leds[NUM_LEDS];
#endif


//Structure for Parsing AC Command
// struct Command {
//   String powerOn;     // on/off_status
//   String temperature; // e.g. "24"
//   String mode;        // e.g. "cool"
//   String fanSpeed;    // e.g. "auto"
//   String protocol;    // e.g. "tcl112"
//   String v_swing;     // optional
//   String h_swing;     // optional
// };

