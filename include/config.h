#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>
#include <Preferences.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRac.h>
//==============================================================

#define USE_NTC
#define CHANGE_DEVICE_CONFIG 0
unsigned long lastHBPublishTime = 0;
const unsigned long hbPublishInterval = 5 * 60 * 1000; // 15 seconds

#if CHANGE_DEVICE_CONFIG
  #define IS_REPEATER false
  #define NODE_ID "20337"
#endif
char nodeID[16];     // Global, mutable buffer
bool isRepeater = false;
bool isButtonPressed = false;
//==============================================================

// Global Preferences Object
Preferences preferences;
//==============================================================

// NTC Temperature Reading Function
#if defined(USE_NTC)
  /* ================= NTC CONFIG ================= */
  #define ADC_PIN            32        // GPIO36 (ADC1_CH0)
  #define ADC_MAX            4095.0
  #define VREF               3.3        // ESP32 ADC reference
  #define SERIES_RESISTOR    9640.0    // 10k fixed resistor
  #define NOMINAL_RESISTANCE 10000.0    // 10k NTC @ 25C
  #define NOMINAL_TEMP       25.0       // °C
  #define B_COEFFICIENT      3950.0
  #define SAMPLE_COUNT       20         // ADC averaging
  #define OFFSET_TEMPERATURE  3.0f        // Calibration offset
  /* =============================================== */
#endif
//=============================================

//Device Setup
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// Debugging Macros
#define DEBUG_MODE true
#define DEBUG_PRINT(x)  if (DEBUG_MODE) { Serial.print(x); }
#define DEBUG_PRINTF(x)  if (DEBUG_MODE) { Serial.printf(x); }
#define DEBUG_PRINTLN(x) if (DEBUG_MODE) { Serial.println(x); }
//=============================================================

//Define PIN 27 for IR LED
const uint16_t kIrLedPin = 27;

const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
IRsend irsend(kIrLedPin);

//Status LED Pin Setup
#define LED_PIN 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
//=============================================================

//Structure for Parsing AC Command
struct Command {
  String powerOn;     // on/off_status
  String temperature; // e.g. "24"
  String mode;        // e.g. "cool"
  String fanSpeed;    // e.g. "auto"
  String protocol;    // e.g. "tcl112"
  String v_swing;     // optional
  String h_swing;     // optional
};

