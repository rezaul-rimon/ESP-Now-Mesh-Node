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

Preferences preferences;

char nodeID[16];     // Global, mutable buffer
bool isRepeater = false;

#define CHANGE_DEVICE_CONFIG 0

#if CHANGE_DEVICE_CONFIG
  #define IS_REPEATER false
  #define NODE_ID "20139"
#endif

//Device Setup
// const char* nodeID = "01001";
// bool isRepeater   = false;
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

unsigned long lastHBPublishTime = 0;
const unsigned long hbPublishInterval = 1 * 60 * 1000;

#define DEBUG_MODE true
#define DEBUG_PRINT(x)  if (DEBUG_MODE) { Serial.print(x); }
#define DEBUG_PRINTF(x)  if (DEBUG_MODE) { Serial.printf(x); }
#define DEBUG_PRINTLN(x) if (DEBUG_MODE) { Serial.println(x); }

//Define PIN 27 for IR LED
const uint16_t kIrLedPin = 27;

const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
IRsend irsend(kIrLedPin);

//Status LED Pin Setup
#define LED_PIN 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

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

