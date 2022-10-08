#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#pragma once

#define DHT_PIN 4
#define DHT_TYPE DHT11

class AirTemp {
    public:
        void setup();
        void loop();
        float getAirTemp();
        float getAirTempF();
        float getHumidity();
    private:
        float tempC, tempF, humidity;
        float lastTC, lastTF, lastHumidity;
};