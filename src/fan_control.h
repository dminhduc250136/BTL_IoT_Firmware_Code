#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

void setupFan();

void setFanState(bool manualStateOn);

void handleAutoFanControl();

#endif