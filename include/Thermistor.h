#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ADS1X15.h>
//#include "Median_filter.h"

#pragma once

class Thermistor {
    public:
        void setup();
        void loop();
        float getTemp();
        float getTempF();
        float getTempFiltered();
};