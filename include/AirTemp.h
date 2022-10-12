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
        int getAirTempF();
        int getHumidity();
    private:
        int tempF, humidity;
        int lastTF, lastHumidity;
};