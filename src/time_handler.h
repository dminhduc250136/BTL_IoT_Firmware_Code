#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include <Arduino.h> // Cần cho kiểu "String"

void setupNTP();
String getCurrentTime();

#endif