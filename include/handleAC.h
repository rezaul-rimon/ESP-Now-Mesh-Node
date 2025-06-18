// #include <config.h>

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

IRTcl112Ac tcl112ACS(kIrLedPin);
IRCoolixAC coolixAC(kIrLedPin);
IRGoodweatherAc goodweatherAC(kIrLedPin);
IRMitsubishiAC mitsubishiAC(kIrLedPin);
IRElectraAc electraAC(kIrLedPin);
IRSanyoAc sanyoAC(kIrLedPin);
IRPanasonicAc panasonicAC(kIrLedPin);
IRKelvinatorAC kelvinatorAC(kIrLedPin);
IRDaikin128 daikinAC(kIrLedPin);
IRHaierAC haierAC(kIrLedPin);
IRLgAc lgAC(kIrLedPin);
IRGreeAC greeAC(kIrLedPin);
IRMirageAc mirageAC(kIrLedPin);
IRCarrierAc64 carrierAC(kIrLedPin);
// IRYorkAc yorkAC(kIrLedPin);

/////----------------//////
// Handle TCL112 AC commands
void handleTCL112(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        float temp = ac.temperature.toFloat();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        tcl112ACS.on();
        tcl112ACS.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max")) tcl112ACS.setFan(kTcl112AcFanHigh);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))  tcl112ACS.setFan(kTcl112AcFanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))  tcl112ACS.setFan(kTcl112AcFanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto")) tcl112ACS.setFan(kTcl112AcFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) tcl112ACS.setMode(kTcl112AcCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  tcl112ACS.setMode(kTcl112AcFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        tcl112ACS.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        tcl112ACS.send();
        DEBUG_PRINTLN("✅ TCL112 AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

/////--------Handle Coolix AC--------//////
void handleCoolix(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        coolixAC.on();
        coolixAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max")) coolixAC.setFan(kCoolixFanMax);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))  coolixAC.setFan(kCoolixFanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))  coolixAC.setFan(kCoolixFanMin);
        else if (ac.fanSpeed.equalsIgnoreCase("auto")) coolixAC.setFan(kCoolixFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) coolixAC.setMode(kCoolixCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  coolixAC.setMode(kCoolixFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        coolixAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        coolixAC.send();
        DEBUG_PRINTLN("✅ Coolix AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

/////--------Handle Goodweather AC--------//////
void handleGoodweather(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        goodweatherAC.on();
        goodweatherAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max")) goodweatherAC.setFan(kGoodweatherFanHigh);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))  goodweatherAC.setFan(kGoodweatherFanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))  goodweatherAC.setFan(kGoodweatherFanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto")) goodweatherAC.setFan(kGoodweatherFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) goodweatherAC.setMode(kGoodweatherCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  goodweatherAC.setMode(kGoodweatherFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        goodweatherAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        goodweatherAC.send();
        DEBUG_PRINTLN("✅ Goodweather AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

/////--------Handle Electra AC--------//////
void handleElectra(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        electraAC.on();
        electraAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max")) electraAC.setFan(kElectraAcFanHigh);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))  electraAC.setFan(kElectraAcFanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))  electraAC.setFan(kElectraAcFanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto")) electraAC.setFan(kElectraAcFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) electraAC.setMode(kElectraAcCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  electraAC.setMode(kElectraAcFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        electraAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        electraAC.send();
        DEBUG_PRINTLN("✅ Electra AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

/////--------Handle Mitsubishi AC--------//////
void handleMitsubishi(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        mitsubishiAC.on();
        mitsubishiAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  mitsubishiAC.setFan(kMitsubishiAcFanMax);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   mitsubishiAC.setFan(kMitsubishiAcFanAuto);  // No dedicated med speed?
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   mitsubishiAC.setFan(kMitsubishiAcFanQuiet);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  mitsubishiAC.setFan(kMitsubishiAcFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) mitsubishiAC.setMode(kMitsubishiAcCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  mitsubishiAC.setMode(kMitsubishiAcFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        mitsubishiAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        mitsubishiAC.send();
        DEBUG_PRINTLN("✅ Mitsubishi AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Carrier AC commands
void handleCarrierAC40(const Command& ac) {
    bool validCommand = true;
    uint8_t temp = ac.temperature.toInt();

    // === Power OFF ===
    if (ac.powerOn.equalsIgnoreCase("off")) {
        irsend.sendCarrierAC40(0x098100830, 40); // Power off
        DEBUG_PRINTLN("❄️ Carrier AC turned OFF");
        leds[0] = CRGB::Green;
        FastLED.show();
        delay(200);
        leds[0] = CRGB::Black;
        FastLED.show();
        return;
    }

    // === Validate Mode ===
    if (!ac.mode.equalsIgnoreCase("cool") && !ac.mode.equalsIgnoreCase("fan")) {
        DEBUG_PRINTLN("⚠️ Unknown mode for Carrier AC: " + ac.mode);
        validCommand = false;
    }

    // === Validate Fan Speed ===
    if (!ac.fanSpeed.equalsIgnoreCase("auto") && !ac.fanSpeed.equalsIgnoreCase("max")) {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
    }

    // === Validate Temperature ===
    if (ac.mode.equalsIgnoreCase("cool")) {
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature for Carrier AC (must be 16–30)");
        validCommand = false;
        }
    }

    // === Abort if any validation failed ===
    if (!validCommand) {
        leds[0] = CRGB::HotPink;
        FastLED.show();
        delay(200);
        leds[0] = CRGB::Black;
        FastLED.show();
        return;
    }

    // === FAN Mode ===
    if (ac.mode.equalsIgnoreCase("fan")) {
        if (ac.fanSpeed.equalsIgnoreCase("auto")) {
        irsend.sendCarrierAC40(0x50E8D00830, 40);
        } else { // max
        irsend.sendCarrierAC40(0x10E8900830, 40);
        }
        DEBUG_PRINTLN("❄️ Carrier AC in FAN mode");
        leds[0] = CRGB::Green;
        FastLED.show();
        delay(200);
        leds[0] = CRGB::Black;
        FastLED.show();
        return;
    }

    // === COOL Mode ===
    const uint64_t baseCoolAuto[] = {
        0x1088100830, 0x1048100830, 0x10C8100830, 0x1028100830, 0x10A8100830,
        0x1068100830, 0x10E8100830, 0x1018100830, 0x1098100830, 0x1058100830,
        0x10D8100830, 0x1038100830, 0x10B8100830, 0x1078100830, 0x10F8100830
    };

    const uint64_t baseCoolMax[] = {
        0x1088900830, 0x1048900830, 0x10C8900830, 0x1028900830, 0x10A8900830,
        0x1068900830, 0x10E8900830, 0x1018900830, 0x1098900830, 0x1058900830,
        0x10D8900830, 0x1038900830, 0x10B8900830, 0x1078900830, 0x10F8900830
    };

    uint8_t idx = temp - 16;
    uint64_t code = ac.fanSpeed.equalsIgnoreCase("max") ? baseCoolMax[idx] : baseCoolAuto[idx];

    irsend.sendCarrierAC40(code, 40);
    Serial.printf("✅ Carrier AC Cool mode sent: Temp %d, Fan %s\n", temp, ac.fanSpeed.c_str());

    leds[0] = CRGB::Green;
    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Sanyo AC commands
void handleSanyo(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        sanyoAC.on();
        sanyoAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  sanyoAC.setFan(kSanyoAcFanHigh);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   sanyoAC.setFan(kSanyoAcFanMedium);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   sanyoAC.setFan(kSanyoAcFanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  sanyoAC.setFan(kSanyoAcFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) sanyoAC.setMode(kSanyoAcCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  sanyoAC.setMode(kSanyoAcAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        sanyoAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        sanyoAC.send();
        DEBUG_PRINTLN("✅ Sanyo AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Panasonic AC commands
void handlePanasonic(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        panasonicAC.on();
        panasonicAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  panasonicAC.setFan(kPanasonicAcFanMax);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   panasonicAC.setFan(kPanasonicAcFanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   panasonicAC.setFan(kPanasonicAcFanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  panasonicAC.setFan(kPanasonicAcFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) panasonicAC.setMode(kPanasonicAcCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  panasonicAC.setMode(kPanasonicAcFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        panasonicAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        panasonicAC.send();
        DEBUG_PRINTLN("✅ Panasonic AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Kelvinator AC commands
void handleKelvinator(const Command& ac) {
    bool validCommand = true;
    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        kelvinatorAC.on();
        kelvinatorAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  kelvinatorAC.setFan(kKelvinatorFanMax);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   kelvinatorAC.setFan(kKelvinatorFanAuto);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   kelvinatorAC.setFan(kKelvinatorFanMin);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  kelvinatorAC.setFan(kKelvinatorFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) kelvinatorAC.setMode(kKelvinatorCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  kelvinatorAC.setMode(kKelvinatorFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        kelvinatorAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        kelvinatorAC.send();
        DEBUG_PRINTLN("✅ Kelvinator AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Daikin AC commands
void handleDaikin(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        daikinAC.setPowerToggle(true);
        daikinAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  daikinAC.setFan(kDaikin128FanHigh);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   daikinAC.setFan(kDaikin128FanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   daikinAC.setFan(kDaikin128FanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  daikinAC.setFan(kDaikin128FanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) daikinAC.setMode(kDaikin128Cool);
        else if (ac.mode.equalsIgnoreCase("fan"))  daikinAC.setMode(kDaikin128Fan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        daikinAC.setPowerToggle(false);
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        daikinAC.send();
        DEBUG_PRINTLN("✅ Daikin AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Haier AC commands
void handleHaier(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        haierAC.setCommand(kHaierAcCmdOn);
        haierAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  haierAC.setFan(kHaierAcFanHigh);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   haierAC.setFan(kHaierAcFanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   haierAC.setFan(kHaierAcFanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  haierAC.setFan(kHaierAcFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) haierAC.setMode(kHaierAcCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  haierAC.setMode(kHaierAcFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        haierAC.setCommand(kHaierAcCmdOff);
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        haierAC.send();
        DEBUG_PRINTLN("✅ Haier AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle LG AC commands
void handleLg(const Command& ac) {
    bool validCommand = true;
    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        lgAC.on();
        lgAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  lgAC.setFan(kLgAcFanMax);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   lgAC.setFan(kLgAcFanMedium);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   lgAC.setFan(kLgAcFanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  lgAC.setFan(kLgAcFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) lgAC.setMode(kLgAcCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  lgAC.setMode(kLgAcFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        lgAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        lgAC.send();
        DEBUG_PRINTLN("✅ LG AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Gree AC commands
void handleGree(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        greeAC.on();
        greeAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  greeAC.setFan(kGreeFanMax);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   greeAC.setFan(kGreeFanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   greeAC.setFan(kGreeFanMin);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  greeAC.setFan(kGreeFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) greeAC.setMode(kGreeCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  greeAC.setMode(kGreeFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        greeAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        greeAC.send();
        DEBUG_PRINTLN("✅ Gree AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Mirage AC commands
void handleMirage(const Command& ac) {
    bool validCommand = true;

    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        mirageAC.on();
        mirageAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  mirageAC.setFan(kMirageAcFanHigh);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   mirageAC.setFan(kMirageAcFanMed);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   mirageAC.setFan(kMirageAcFanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  mirageAC.setFan(kMirageAcFanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) mirageAC.setMode(kMirageAcCool);
        else if (ac.mode.equalsIgnoreCase("fan"))  mirageAC.setMode(kMirageAcFan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        mirageAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        mirageAC.send();
        DEBUG_PRINTLN("✅ Mirage AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}

// Handle Carrier64 AC commands
void handleCarrier64(const Command& ac) {
    bool validCommand = true;
    if (ac.powerOn.equalsIgnoreCase("on")) {
        int temp = ac.temperature.toInt();
        if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature: " + String(temp));
        validCommand = false;
        } else {
        carrierAC.on();
        carrierAC.setTemp(temp);
        }

        if      (ac.fanSpeed.equalsIgnoreCase("max"))  carrierAC.setFan(kCarrierAc64FanHigh);
        else if (ac.fanSpeed.equalsIgnoreCase("med"))   carrierAC.setFan(kCarrierAc64FanMedium);
        else if (ac.fanSpeed.equalsIgnoreCase("min"))   carrierAC.setFan(kCarrierAc64FanLow);
        else if (ac.fanSpeed.equalsIgnoreCase("auto"))  carrierAC.setFan(kCarrierAc64FanAuto);
        else {
        DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);
        validCommand = false;
        }

        if      (ac.mode.equalsIgnoreCase("cool")) carrierAC.setMode(kCarrierAc64Cool);
        else if (ac.mode.equalsIgnoreCase("fan"))  carrierAC.setMode(kCarrierAc64Fan);
        else {
        DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
        validCommand = false;
        }

    } else if (ac.powerOn.equalsIgnoreCase("off")) {
        carrierAC.off();
    } else {
        DEBUG_PRINTLN("⚠️ Unknown power command: " + ac.powerOn);
        validCommand = false;
    }

    delay(100);

    if (validCommand) {
        carrierAC.send();
        DEBUG_PRINTLN("✅ Carrier AC command sent.");
        leds[0] = CRGB::Green;
    } else {
        leds[0] = CRGB::HotPink;
    }

    FastLED.show();
    delay(200);
    leds[0] = CRGB::Black;
    FastLED.show();
}
