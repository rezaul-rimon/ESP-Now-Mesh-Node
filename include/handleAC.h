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

IRCoolixAC coolixAC(kIrLedPin);
IRGoodweatherAc goodweatherAC(kIrLedPin);
IRMitsubishiAC mitsubishiAC(kIrLedPin);
IRTcl112Ac tcl112ACS(kIrLedPin);
IRElectraAc electraAC(kIrLedPin);

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
    else DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) tcl112ACS.setMode(kTcl112AcCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  tcl112ACS.setMode(kTcl112AcFan);
    // else if (ac.mode.equalsIgnoreCase("auto")) tcl112ACS.setMode(kTcl112AcAuto);
    else DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
    } else {
    tcl112ACS.off();
    }

    delay(100);
    tcl112ACS.send();
    DEBUG_PRINTLN("✅ TCL112 AC command sent.");
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
    else DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) coolixAC.setMode(kCoolixCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  coolixAC.setMode(kCoolixFan);
    else DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
    } else {
    coolixAC.off();
    }

    delay(100);
    coolixAC.send();
    DEBUG_PRINTLN("✅ Coolix AC command sent.");
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
    else DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) goodweatherAC.setMode(kGoodweatherCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  goodweatherAC.setMode(kGoodweatherFan);
    else DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
    } else {
    goodweatherAC.off();
    }

    delay(100);
    goodweatherAC.send();
    DEBUG_PRINTLN("✅ Goodweather AC command sent.");
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
    else DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) electraAC.setMode(kElectraAcCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  electraAC.setMode(kElectraAcFan);
    else DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
    } else {
    electraAC.off();
    }

    delay(100);
    electraAC.send();
    DEBUG_PRINTLN("✅ Electra AC command sent.");
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
    else DEBUG_PRINTLN("⚠️ Unknown fan speed: " + ac.fanSpeed);

    if      (ac.mode.equalsIgnoreCase("cool")) mitsubishiAC.setMode(kMitsubishiAcCool);
    else if (ac.mode.equalsIgnoreCase("fan"))  mitsubishiAC.setMode(kMitsubishiAcFan);
    else DEBUG_PRINTLN("⚠️ Unknown mode: " + ac.mode);
    } else {
    mitsubishiAC.off();
    }

    delay(100);
    mitsubishiAC.send();
    DEBUG_PRINTLN("✅ Mitsubishi AC command sent.");
}

void handleCarrierAC40(const Command& ac) {
    uint8_t temp = ac.temperature.toInt();

    if (ac.powerOn.equalsIgnoreCase("off")) {
        irsend.sendCarrierAC40(0x098100830, 40); // Power off
        DEBUG_PRINTLN("❄️ Carrier AC turned OFF");
        return;
    }

  // === Fan Mode ===
    if (ac.mode.equalsIgnoreCase("fan")) {
        if (ac.fanSpeed.equalsIgnoreCase("auto")) {
            irsend.sendCarrierAC40(0x50E8D00830, 40);
        } else if (ac.fanSpeed.equalsIgnoreCase("max")) {
            irsend.sendCarrierAC40(0x10E8900830, 40);
        } else {
            DEBUG_PRINTLN("⚠️ Unknown fan speed in FAN mode: " + ac.fanSpeed);
            return;
        }
        DEBUG_PRINTLN("❄️ Carrier AC in FAN mode");
        return;
    }

  // === Cool Mode ===
    if (!ac.mode.equalsIgnoreCase("cool")) {
        DEBUG_PRINTLN("⚠️ Unknown mode for Carrier AC: " + ac.mode);
        return;
    }

    if (temp < 16 || temp > 30) {
        DEBUG_PRINTLN("⚠️ Invalid temperature for Carrier AC (must be 16–30)");
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
        DEBUG_PRINTLN("⚠️ Unknown fan speed in COOL mode: " + ac.fanSpeed);
    return;
    }

    irsend.sendCarrierAC40(code, 40);
    Serial.printf("✅ Carrier AC Cool mode sent: Temp %d, Fan %s\n", temp, ac.fanSpeed.c_str());
}
