#include <config.h>
/////----------------//////
// Handle TCL112 AC commands
void handleTCL112(const Command& ac) {
    if (ac.powerOn.equalsIgnoreCase("on")) {
    tcl112ACS.on();
    tcl112ACS.setTemp(ac.temperature.toFloat());

    if      (ac.fanSpeed.equalsIgnoreCase("high")) tcl112ACS.setFan(kTcl112AcFanHigh);
    else if (ac.fanSpeed.equalsIgnoreCase("med"))  tcl112ACS.setFan(kTcl112AcFanMed);
    else if (ac.fanSpeed.equalsIgnoreCase("low"))  tcl112ACS.setFan(kTcl112AcFanLow);
    else if (ac.fanSpeed.equalsIgnoreCase("auto")) tcl112ACS.setFan(kTcl112AcFanAuto);
    else Serial.println("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) tcl112ACS.setMode(kTcl112AcCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  tcl112ACS.setMode(kTcl112AcFan);
    // else if (ac.mode.equalsIgnoreCase("auto")) tcl112ACS.setMode(kTcl112AcAuto);
    else Serial.println("⚠️ Unknown mode: " + ac.mode);
    } else {
    tcl112ACS.off();
    }

    delay(100);
    tcl112ACS.send();
    Serial.println("✅ TCL112 AC command sent.");
}

/////--------Handle Coolix AC--------//////
void handleCoolix(const Command& ac) {
    if (ac.powerOn.equalsIgnoreCase("on")) {
    coolixAC.on();
    coolixAC.setTemp(ac.temperature.toInt());

    if      (ac.fanSpeed.equalsIgnoreCase("high")) coolixAC.setFan(kCoolixFanMax);
    else if (ac.fanSpeed.equalsIgnoreCase("med"))  coolixAC.setFan(kCoolixFanMed);
    else if (ac.fanSpeed.equalsIgnoreCase("low"))  coolixAC.setFan(kCoolixFanMin);
    else if (ac.fanSpeed.equalsIgnoreCase("auto")) coolixAC.setFan(kCoolixFanAuto);
    else Serial.println("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) coolixAC.setMode(kCoolixCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  coolixAC.setMode(kCoolixFan);
    else Serial.println("⚠️ Unknown mode: " + ac.mode);
    } else {
    coolixAC.off();
    }

    delay(100);
    coolixAC.send();
    Serial.println("✅ Coolix AC command sent.");
}

/////--------Handle Goodweather AC--------//////
void handleGoodweather(const Command& ac) {
    if (ac.powerOn.equalsIgnoreCase("on")) {
    goodweatherAC.on();
    goodweatherAC.setTemp(ac.temperature.toFloat());

    if      (ac.fanSpeed.equalsIgnoreCase("high")) goodweatherAC.setFan(kGoodweatherFanHigh);
    else if (ac.fanSpeed.equalsIgnoreCase("med"))  goodweatherAC.setFan(kGoodweatherFanMed);
    else if (ac.fanSpeed.equalsIgnoreCase("low"))  goodweatherAC.setFan(kGoodweatherFanLow);
    else if (ac.fanSpeed.equalsIgnoreCase("auto")) goodweatherAC.setFan(kGoodweatherFanAuto);
    else Serial.println("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) goodweatherAC.setMode(kGoodweatherCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  goodweatherAC.setMode(kGoodweatherFan);
    else Serial.println("⚠️ Unknown mode: " + ac.mode);
    } else {
    goodweatherAC.off();
    }

    delay(100);
    goodweatherAC.send();
    Serial.println("✅ Goodweather AC command sent.");
}

/////--------Handle Electra AC--------//////
void handleElectra(const Command& ac) {
    if (ac.powerOn.equalsIgnoreCase("on")) {
    electraAC.on();
    electraAC.setTemp(ac.temperature.toFloat());

    if      (ac.fanSpeed.equalsIgnoreCase("high")) electraAC.setFan(kElectraAcFanHigh);
    else if (ac.fanSpeed.equalsIgnoreCase("med"))  electraAC.setFan(kElectraAcFanMed);
    else if (ac.fanSpeed.equalsIgnoreCase("low"))  electraAC.setFan(kElectraAcFanLow);
    else if (ac.fanSpeed.equalsIgnoreCase("auto")) electraAC.setFan(kElectraAcFanAuto);
    else Serial.println("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) electraAC.setMode(kElectraAcCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  electraAC.setMode(kElectraAcFan);
    else Serial.println("⚠️ Unknown mode: " + ac.mode);
    } else {
    electraAC.off();
    }

    delay(100);
    electraAC.send();
    Serial.println("✅ Electra AC command sent.");
}

/////--------Handle Mitsubishi AC--------//////
void handleMitsubishi(const Command& ac) {
    if (ac.powerOn.equalsIgnoreCase("on")) {
    mitsubishiAC.on();
    mitsubishiAC.setTemp(ac.temperature.toInt());

    if      (ac.fanSpeed.equalsIgnoreCase("high")) mitsubishiAC.setFan(kMitsubishiAcFanMax);
    else if (ac.fanSpeed.equalsIgnoreCase("med"))  mitsubishiAC.setFan(kMitsubishiAcFanAuto);
    else if (ac.fanSpeed.equalsIgnoreCase("low"))  mitsubishiAC.setFan(kMitsubishiAcFanQuiet);
    else if (ac.fanSpeed.equalsIgnoreCase("auto")) mitsubishiAC.setFan(kMitsubishiAcFanAuto);
    else Serial.println("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) mitsubishiAC.setMode(kMitsubishiAcCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  mitsubishiAC.setMode(kMitsubishiAcFan);
    else Serial.println("⚠️ Unknown mode: " + ac.mode);
    } else {
    mitsubishiAC.off();
    }

    delay(100);
    mitsubishiAC.send();
    Serial.println("✅ Mitsubishi AC command sent.");
}

void handleCarrierAC40(const Command& ac) {
    uint8_t temp = ac.temperature.toInt();

    if (ac.powerOn.equalsIgnoreCase("off")) {
        irsend.sendCarrierAC40(0x098100830, 40); // Power off
        Serial.println("❄️ Carrier AC turned OFF");
        return;
    }

  // === Fan Mode ===
    if (ac.mode.equalsIgnoreCase("fan")) {
        if (ac.fanSpeed.equalsIgnoreCase("auto")) {
            irsend.sendCarrierAC40(0x50E8D00830, 40);
        } else if (ac.fanSpeed.equalsIgnoreCase("max")) {
            irsend.sendCarrierAC40(0x10E8900830, 40);
        } else {
            Serial.println("⚠️ Unknown fan speed in FAN mode: " + ac.fanSpeed);
            return;
        }
        Serial.println("❄️ Carrier AC in FAN mode");
        return;
    }

  // === Cool Mode ===
    if (!ac.mode.equalsIgnoreCase("cool")) {
        Serial.println("⚠️ Unknown mode for Carrier AC: " + ac.mode);
        return;
    }

    if (temp < 16 || temp > 30) {
        Serial.println("⚠️ Invalid temperature for Carrier AC (must be 16–30)");
        return;
    }

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
    uint64_t code;

    if (ac.fanSpeed.equalsIgnoreCase("max")) {
        code = baseCoolMax[idx];
    } else if (ac.fanSpeed.equalsIgnoreCase("auto")) {
        code = baseCoolAuto[idx];
    } else {
        Serial.println("⚠️ Unknown fan speed in COOL mode: " + ac.fanSpeed);
    return;
    }

    irsend.sendCarrierAC40(code, 40);
    Serial.printf("✅ Carrier AC Cool mode sent: Temp %d, Fan %s\n", temp, ac.fanSpeed.c_str());
}
