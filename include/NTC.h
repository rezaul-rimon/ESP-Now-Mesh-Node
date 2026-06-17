#pragma once

#if defined(USE_NTC_SENSOR)

    #include <math.h>
    //========================= NTC Class Definition =============================//
    class NTC {
    private:
        uint8_t pin;
        float adcMax;
        float vRef;
        float seriesResistor;
        float nominalRes;
        float nominalTemp;
        float beta;
        int sampleCount;
        float offsetTemp;

        // Internal storage of last averaged values
        float avgVoltage = 0;
        float avgResistance = 0;

        // Private: read raw ADC voltage
        float readVoltage() {
        uint32_t adcSum = 0;
        for (int i = 0; i < sampleCount; i++) {
            adcSum += analogRead(pin);
            delay(5);
        }
        float adcAvg = adcSum / (float)sampleCount;
            avgVoltage = (adcAvg / adcMax) * vRef;
            return avgVoltage;
        }

        // Private: calculate NTC resistance
        float readResistance(float voltage) {
            avgResistance = seriesResistor * ((vRef / voltage) - 1.0);
            return avgResistance;
        }

        // Private: calculate temperature from resistance
        float calculateTemp(float resistance) {
            float steinhart = resistance / nominalRes;
            steinhart = log(steinhart);
            steinhart /= beta;
            steinhart += 1.0 / (nominalTemp + 273.15);
            steinhart = 1.0 / steinhart;
            steinhart -= 273.15;

            return steinhart + offsetTemp;
        }

    public:
        // Constructor with initializer list
        NTC(uint8_t adcPin, float adcMaxVal, float vRefVal,
        float seriesR, float nominalR, float nominalT,
        float betaVal, int samples, float offset)
        : pin(adcPin), adcMax(adcMaxVal), vRef(vRefVal), seriesResistor(seriesR),
            nominalRes(nominalR), nominalTemp(nominalT), beta(betaVal),
            sampleCount(samples), offsetTemp(offset)
        {
        analogReadResolution(12);
        analogSetAttenuation(ADC_11db); // ESP32 0–3.3V full range
        }

        // Public: read temperature (°C)
        float readTemperature() {
        float voltage = readVoltage();
        float resistance = readResistance(voltage);
        return calculateTemp(resistance);
        }

        // Optional getters for voltage and resistance
        float getVoltage() { return avgVoltage; }
        float getResistance() { return avgResistance; }
    };
    //========================= End Class =============================//

    // NTC ntcSensor(35); // GPIO35 analog pin

    // void setup() {
    //     Serial.begin(115200);
    //     delay(1000);
    //     Serial.println("ESP32 NTC Temperature Sensor (OOP) Started");
    // }

    // void loop() {
    //     float tempC = ntcSensor.readTemperature();

    //     Serial.print("Temperature: "); Serial.print(tempC, 2); Serial.println(" °C");
    //     Serial.print("Voltage: "); Serial.print(ntcSensor.getVoltage(), 3); Serial.println(" V");
    //     Serial.print("Resistance: "); Serial.print(ntcSensor.getResistance(), 1); Serial.println(" Ohm");

    //     delay(2000);
    // }


#endif