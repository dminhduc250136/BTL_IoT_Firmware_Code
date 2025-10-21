#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <Arduino.h> // Cần cho "String"

void setupOLED();
void displayOLED(float t, float h, float ppm, const String &airQuality);

#endif