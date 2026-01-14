#include <Arduino.h>
#include <Preferences.h>

void handleSwitches(String message);

Preferences preferences;
bool isButtonPressed = false; // Global flag for button press state

char nodeID[20];     // Global, mutable buffer
bool isRepeater = false;

#define CHANGE_DEVICE_CONFIG 0

#if CHANGE_DEVICE_CONFIG
  #define IS_REPEATER false
  #define NODE_ID "1225102510150089"
#endif

