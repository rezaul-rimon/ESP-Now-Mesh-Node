#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>
#include <Preferences.h>

void handleSwitches(String message);

Preferences preferences;
bool isButtonPressed = false; // Global flag for button press state

char nodeID[20];     // Global, mutable buffer
bool isRepeater = false;

#define CHANGE_DEVICE_CONFIG 0

#if CHANGE_DEVICE_CONFIG
  #define IS_REPEATER false
  #define NODE_ID "1225102510150031"
#endif

//Device Setup
// const char* nodeID = "01001";
// bool isRepeater   = false;
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

unsigned long lastHBPublishTime = 0;
const unsigned long hbPublishInterval = 5 * 60 * 1000;

#define DEBUG_MODE true
#define DEBUG_PRINT(x)  if (DEBUG_MODE) { Serial.print(x); }
#define DEBUG_PRINTF(x)  if (DEBUG_MODE) { Serial.printf(x); }
#define DEBUG_PRINTLN(x) if (DEBUG_MODE) { Serial.println(x); }

#define Fast_LED 1

//Status LED Pin Setup
#if Fast_LED
  #define LED_PIN 4
  #define NUM_LEDS 1
  CRGB leds[NUM_LEDS];
#endif

//Switch Pins Setup
#define SW_PIN1 25  
#define SW_PIN2 26  
#define SW_PIN3 27 
#define SW_PIN4 14

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

