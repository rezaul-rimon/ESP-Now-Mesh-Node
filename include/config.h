#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRac.h>

const uint16_t kIrLedPin = 27; // GPIO for IR LED
// Include the necessary libraries for IR control

#include <ir_Airton.h>
#include <ir_Airwell.h>
#include <ir_Amcor.h>
#include <ir_Argo.h>
#include <ir_Bosch.h>
#include <ir_Carrier.h>
#include <ir_Coolix.h>
#include <ir_Corona.h>
#include <ir_Daikin.h>
#include <ir_Ecoclim.h>
#include <ir_Goodweather.h>
#include <ir_Gree.h>
#include <ir_Kelon.h>
#include <ir_Kelvinator.h>
#include <ir_Lg.h>
#include <ir_Magiquest.h>
#include <ir_Midea.h>
#include <ir_Mirage.h>
#include <ir_Mitsubishi.h>
#include <ir_Nec.h>
#include <ir_Neoclima.h>
#include <ir_Panasonic.h>
#include <ir_Rhoss.h>
#include <ir_Samsung.h>
#include <ir_Sanyo.h>
#include <ir_Sharp.h>
#include <ir_Tcl.h>
#include <ir_Teco.h>
#include <ir_Transcold.h>
#include <ir_Trotec.h>
#include <ir_Truma.h>
#include <ir_Voltas.h>
#include <ir_York.h>

const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
IRsend irsend(kIrLedPin);

IRCoolixAC coolixAC(kIrLedPin);
IRGoodweatherAc goodweatherAC(kIrLedPin);
IRMitsubishiAC mitsubishiAC(kIrLedPin);
IRTcl112Ac tcl112ACS(kIrLedPin);
IRElectraAc electraAC(kIrLedPin);

#define LED_PIN 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

const char* nodeID = "node-00";
bool isRepeater   = true;
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

unsigned long lastHBPublishTime = 0;
const unsigned long hbPublishInterval = 1 * 60 * 1000;

struct Command {
  String powerOn;     // on/off_status
  String temperature; // e.g. "24"
  String mode;        // e.g. "cool"
  String fanSpeed;    // e.g. "auto"
  String protocol;    // e.g. "tcl112"
  String v_swing;     // optional
  String h_swing;     // optional
};