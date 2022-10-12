#include "AirTemp.h"

// Instantiate DHT11 sensor
DHT dht(DHT_PIN, DHT_TYPE);

void AirTemp::setup() {
    dht.begin();
}

void AirTemp::loop() {
    // Store previous valid data since this sensor is slow
    if (!isnan(tempF) || !isnan(humidity)) {
        lastTF = tempF;
        lastHumidity = humidity;
    }

    // Poll for new values
    tempF = dht.readTemperature(true);
    humidity = dht.readHumidity();
}

/**
 * Return the temperature as read from the DHT11.
 * @return Detected temperature in degrees Fahrenheit.
 */
int AirTemp::getAirTempF() {
    return isnan(tempF) ? lastTF : tempF;
}

/**
 * Return the humidity as read from the DHT11.
 * @return Detected humidity percentage. 
 */
int AirTemp::getHumidity() {
    return isnan(humidity) ? lastHumidity : humidity;
}