#include "config.h"
#include "fan_control.h"

void setupFan()
{
    Serial.println("Initializing fan...");
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, FAN_OFF);
    fanData.initialized = true;
    fanData.fanOn = false;
    Serial.println("Success!");
}

void setFanState(bool manualStateOn)
{
    if (manualStateOn != fanData.fanOn)
    {
        digitalWrite(RELAY_PIN, manualStateOn ? FAN_ON : FAN_OFF);
        fanData.fanOn = manualStateOn;
        Serial.printf("Fan turned %s\n", manualStateOn ? "ON" : "OFF");

        if (firebaseConnected && !fanData.isManualMode)
        {
            Database.set(aClient_Tasks, "/controls" + String(ROOT) + "/fan", manualStateOn ? "ON" : "OFF");
        }
    }
}

void handleAutoFanControl()
{

    float temp = dht11Data.temperature;
    float hum = dht11Data.humidity;

    float mq135_ppm = mq135Data.ppm;
    float pm25_conc = pm25Data.concentration;

    if (temp > TEMP_THRESHOLD_HIGH || hum > HUMIDITY_THRESHOLD_HIGH || mq135_ppm > MQ135PPM_THRESHOLD_HIGH || pm25_conc > PM25_THRESHOLD_HIGH)
    {
        setFanState(true);
    }
    else if (temp < TEMP_THRESHOLD_LOW && hum < HUMIDITY_THRESHOLD_LOW && mq135_ppm < MQ135PPM_THRESHOLD_LOW && pm25_conc < PM25_THRESHOLD_LOW)
    {
        setFanState(false);
    }
    return;
}