#include "config.h"
#include "fan_control.h"
#include "sensors.h"
void setupFan()
{
    Serial.println("Initializing fan...");
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, FAN_OFF);
    fanData.initialized = true;
    fanData.fanOn = false;
    Serial.println("Success!");
}

void setFanState(bool on)
{
    if (on != fanData.fanOn)
    {
        digitalWrite(RELAY_PIN, on ? FAN_ON : FAN_OFF);
        fanData.fanOn = on;
        Serial.printf("Fan turned %s\n", on ? "ON" : "OFF");
    }
}

void handleAutoFanControl()
{

    String currentStatus = getAirQualityLabel(mq135Data.ppm, pm25Data.concentration, dht11Data.temperature, dht11Data.humidity);
    if (currentStatus == "Very Poor" || currentStatus == "Poor")
    {
        setFanState(true);
    }
    else if (currentStatus == "Good")
    {
        setFanState(false);
    }

    else if (currentStatus == "Average")
    {
    }
    return;
}