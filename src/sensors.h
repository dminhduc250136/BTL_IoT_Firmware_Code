#ifndef SENSORS_HANDLER_H
#define SENSORS_HANDLER_H

#include <Arduino.h>

void setupDHT11();
void setupMQ135();
void setupPM25();

void readDHT11();
void readMQ135();
void readPM25();

double MQ135_calculator(int adcValue);
double PM25_calculator(int adcValue);
String getAirQualityLabel(double MQ135PPM, double PM25Concentration, float Temperature, float Humidity);

#endif