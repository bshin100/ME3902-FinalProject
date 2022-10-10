#include <Arduino.h>
#include "Ultrasonic.h"
#include "Thermistor.h"
#include "AirTemp.h"
#include "ESP8266.h"

// ## CONFIGURATION ## //
#define RELAY_W_PIN 5                   // Water Pump Relay
#define RELAY_H_PIN 6                   // Heater Relay
#define SILENCE_BUTTON 7                // HW Silence Button
#define ACTION_BUTTON 1                 // HW Action Button
#define GREEN_LED_PIN 11                // Green LED
#define YELLOW_LED_PIN 12               // Yellow LED
#define RED_LED_PIN 13                  // Red LED

const float WATER_THRESH = 10.0;        // Water height threshold - values > this triggers refill
const float WATER_THRESH_MAX = 30.0;    // Maximum distance to water
const float TEMP_UPPER_THRESH = 85.0;   // Water temperature upper boundary (too hot!), deg F
const float TEMP_LOWER_THRESH = 25.0;   // Water temperature lower boundary (too cold!), deg F

const bool bypassWaterLevel = false;
// ## CONFIG END ## //

// Instantiate our sensors
Ultrasonic ultrasonic;
Thermistor thermistor;
AirTemp airTemp;
ESP8266 wifi;

// Define enumerations for system states
enum States { MONITOR, TEMP_ALARM, WATER_ALARM, AWAIT_CONFIRM, WATER_PUMP } state;
const char *stateNames[] = {"MONITOR", "TEMP_ALARM", "WATER_ALARM", "AWAIT_CONFIRM", "WATER_PUMP"};

bool paused = false;
bool tempAlarmReset = false;
int silenceButton = 0;
int actionButton = 0;

void turnOffLEDs() {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
}

// Logic for the state machine
void stateController() {
    static States previousState = MONITOR;
    if (state != previousState) {
        Serial.println(stateNames[state]);
        previousState = state;
    }

    switch (state) {
        case MONITOR:
            //Serial.println(thermistor.getTempF());
            //Serial.println(ultrasonic.getDistance());
            //Serial.print("Air Temp: "); Serial.println(airTemp.getAirTempF());
            //Serial.print("Humidity: "); Serial.println(airTemp.getHumidity());

            // Keep water heater on during normal operation
            digitalWrite(RELAY_H_PIN, HIGH); // High turns off the NC outlet and turns on the NO outlets

            // Update status LEDs
            turnOffLEDs();
            if(!tempAlarmReset) {
                digitalWrite(GREEN_LED_PIN, HIGH);
            } else {
                // Green + Red: Silenced temperature alarm.
                digitalWrite(GREEN_LED_PIN, HIGH);
                digitalWrite(RED_LED_PIN, HIGH);
            }

            // Check if water temperature is too high or too low
            // TODO: Implement 1 minute buffer/moving average?
            if (thermistor.getTempF() > TEMP_UPPER_THRESH && !tempAlarmReset) {
                Serial.println("Water temperature is too hot. Entering alarm state.");
                state = TEMP_ALARM;
            } else if (thermistor.getTempF() < TEMP_LOWER_THRESH) {
                Serial.println("Water temperature is too cold. No action taken.");
            }

            // Check if water level is too low, and disregard erroneous readings
            if (ultrasonic.getDistance() > WATER_THRESH && ultrasonic.getDistance() < WATER_THRESH_MAX && !bypassWaterLevel) {
                Serial.println("Water level is too low. Entering alarm state.");
                state = WATER_ALARM;
            }
        break;

        // Immediately turn off heater relay and alarm user
        case TEMP_ALARM:
            digitalWrite(RELAY_H_PIN, LOW); // LOW turns off NO outlets and turns on the NC outlet
            Serial.print("Water temperature is too hot! Current temperature: ");
            Serial.print(thermistor.getTempF()); Serial.println(" F. Turning heater off!");
            tempAlarmReset = false;

            // Status LED Red
            turnOffLEDs();
            digitalWrite(RED_LED_PIN, HIGH);

            silenceButton = digitalRead(SILENCE_BUTTON);
            if(silenceButton == HIGH) {
                Serial.println("Silencing temperature alarm. Returning to MONITOR.");
                tempAlarmReset = true;
                state = MONITOR;
            }
            //if (tempAlarmReset) state = MONITOR; // TODO: Implement logging and retention
        break;

        case WATER_ALARM:
            Serial.println("Water level is too low! Confirm refill or silence.");

            // Status LED Yellow
            turnOffLEDs();
            digitalWrite(YELLOW_LED_PIN, HIGH);

            // TODO: Silence button to return to MONITOR state
            state = AWAIT_CONFIRM;
        break;

        // Await user confirmation before activating water pump
        case AWAIT_CONFIRM:
            Serial.println("Awaiting user confirmation to refill tank.");
            // static int tempCount1 = 0;
            // if(tempCount1 == 0) {
            //     Serial.println("Awaiting user confirmation to refill tank.");
            //     //paused = true;
            //     tempCount1++;
            // }
            
            silenceButton = digitalRead(SILENCE_BUTTON);
            actionButton = digitalRead(ACTION_BUTTON);
            if(silenceButton == HIGH) {
                Serial.println("Silencing water level alarm. Returning to MONITOR.");
                state = MONITOR;
            }
            if(actionButton == HIGH) {
                Serial.println("Beginning refill of the tank.");
                state = WATER_PUMP;
            }
            // TODO: poll input from  webserver
            //if(!paused) state = WATER_PUMP;+
            //state = WATER_PUMP;
        break;

        // Pump water until water level is reached
        case WATER_PUMP:
            float currTime, delta = 0;
            float startTime = millis();
            digitalWrite(RELAY_W_PIN, LOW); // Turn on water pump (NO circuit)
            ultrasonic.loop(); // Keep pinging sensor

            // Keep on pumpin', minimum 5 seconds.
            while (/*ultrasonic.getDistance() > WATER_THRESH && ultrasonic.getDistance() < WATER_THRESH_MAX &&*/ delta < 5000.0) {
                ultrasonic.loop(); // Keep pinging sensor
                Serial.print("Filling tank. Current water level: ");
                Serial.println(ultrasonic.getDistance());

                digitalWrite(YELLOW_LED_PIN, LOW); // Blink yellow LED while filling
                delay(50);
                digitalWrite(YELLOW_LED_PIN, HIGH);
                delay(50);
                
                currTime = millis();
                delta = currTime - startTime;
            }

            Serial.println(delta);

            // FIXME: Keep pump on until water level reached without crashing the program
            digitalWrite(RELAY_W_PIN, HIGH); // Turn off water pump
            digitalWrite(YELLOW_LED_PIN, LOW);
            Serial.println("Water level reached. Returning to MONTIOR in 2 seconds.");

            delay(2000); // Wait 2 seconds to settle
            state = MONITOR;
            
        break;
    } 
}

void setup() {
    Serial.begin(9600);

    // Call our `setup()` functions to initialize sensors
    ultrasonic.setup();
    thermistor.setup();
    airTemp.setup();
    wifi.setup();

    pinMode(RELAY_W_PIN, OUTPUT);
    pinMode(RELAY_H_PIN, OUTPUT);
    pinMode(SILENCE_BUTTON, INPUT);
    pinMode(ACTION_BUTTON, INPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(YELLOW_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);

    // Make sure our devices start at the right state on startup
    turnOffLEDs();
    digitalWrite(RELAY_H_PIN, HIGH); // High turns off the NC outlet and turns on the NO outlets
    digitalWrite(RELAY_W_PIN, HIGH);

    delay(2000); // Delay program for 2 seconds to mitigate startup glitchiness
    Serial.println("Starting program!");
    state = MONITOR;
}

void loop() {
    ultrasonic.loop();
    thermistor.loop();
    airTemp.loop();
    wifi.loop();

    if(!paused) stateController();
    delay(250);
}