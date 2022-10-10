//////////////////////
// Library Includes //
//////////////////////
// SoftwareSerial is required (even you don't intend on
// using it).
#include <SoftwareSerial.h> 
#include <SparkFunESP8266WiFi.h>

#pragma once

class ESP8266 {
    public:
        void setup();
        void loop();
        void initializeESP8266();
        void connectESP8266();
        void displayConnectInfo();
        void clientDemo();
        void serverSetup();
        void serverDemo();
        void errorLoop(int);
        void serialTrigger(String);
};