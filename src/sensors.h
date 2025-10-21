#ifndef SENSORS_HANDLER_H
#define SENSORS_HANDLER_H

#include <Arduino.h>

void setupDHT11();
void setupMQ135();
void setupPM25();

void readDHT11();
void readMQ135();
void readPM25();

float MQ135_calculator(int adcValue);
float PM25_calculator(int adcValue);
String getAirQualityLabel(float MQ135PPM, float PM25Concentration);

#endif