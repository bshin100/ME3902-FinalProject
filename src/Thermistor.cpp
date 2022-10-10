#include "Thermistor.h"

// Instantiate median filter
//MedianFilter tempFilter;

// Instantiate ADC
Adafruit_ADS1115 ads;
int16_t rawADCvalue;

float Vmid, TC_Reg;
float voltMax = 4.096;

// 5th Order Polynomial Regression Coefficients
float a5 = -0.927; float a4 = 11; float a3 = -50.9;
float a2 = 117; float a1 = -154; float a0 = 138;

void Thermistor::setup() {
    //tempFilter.Init();
    ads.begin(0x48);
    ads.setGain(GAIN_ONE);
}

void Thermistor::loop() {
    rawADCvalue = ads.readADC_Differential_0_1();
    Vmid = float(rawADCvalue / 32767.0) * voltMax;

    TC_Reg = a0 + a1*Vmid + a2*pow(Vmid, 2) + a3*pow(Vmid, 3) + a4*pow(Vmid, 4) + a5*pow(Vmid, 5);
}

/**
 * Return the temperature as read from the thermistor.
 * @return Detected temperature in degrees Celsius.
 */
float Thermistor::getTemp() {
    return TC_Reg;
}

/**
 * Return the temperature as read from the thermistor.
 * @return Detected temperature in degrees Fahrenheit.
 */
float Thermistor::getTempF() {
    return (TC_Reg * 1.8) + 32.0;
}

/**
 * Return the temperature as read from the thermistor.
 * @return Filtered temperature in degrees Celsius. 
 */
float Thermistor::getTempFiltered() {
    //return tempFilter.Filter(getTemp());
    return 0.0;
}