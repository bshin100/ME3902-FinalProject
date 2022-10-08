#include <Arduino.h>
#include "Median_filter.h"

#pragma once

#define TRIGGER_PIN 2
#define ECHO_PIN 3

class Ultrasonic {
    public:
        void setup();
        void loop();
        float getDistance();
        float getDistanceFiltered();
    private:
        float distanceOutput = 0;
};