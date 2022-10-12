#include "Ultrasonic.h"

// Variables
static unsigned long startTime;
static unsigned long roundTripTime;
static unsigned long pingTimer;    

// Instantiate median filter
//MedianFilter ultraFilter;

/**
 * Interrupt service routine for the echo pin
 * The echo pin goes high when the ultrasonic burst is sent out and goes low when the
 * echo is received. On the burst, the time is recorded, and on the echo the round trip
 * time is computed. This is used when requesting a distance by the getDistance methods.
 */
void ultrasonicISR() {
    if (digitalRead(ECHO_PIN)) {
        startTime = micros();
    } else {
        roundTripTime = micros() - startTime;
    }
}

void Ultrasonic::setup() {
    pingTimer = 0;
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(ECHO_PIN), ultrasonicISR, CHANGE);

    //ultraFilter.Init();
}

void Ultrasonic::loop() {
    if(millis() > pingTimer) {
        // trigger the ping
        digitalWrite(TRIGGER_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIGGER_PIN, LOW);
        pingTimer = millis() + 100;
    }
}

/**
 * Return the distance of the ultrasonic sensor
 * @return Detected distance in CM.
 */
float Ultrasonic::getDistance() {
    long rt;
    cli();
    rt = roundTripTime;
    sei();
    distance = (rt * 0.0343) / 2.0;
    return distance;
}

/**
 * Return the distance of the ultrasonic sensor, but filtered.
 * @return Detected distance in CM. 
 */
float Ultrasonic::getDistanceFiltered() {
    //return ultraFilter.Filter(getDistance());
    return 0.0;
}