#include "config.h"
#include "wifi_handler.h"
#include <WiFi.h>

void setupWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Đang kết nối WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    ssl_client_stream.setInsecure();
    ssl_client_stream.setTimeout(1000);
    ssl_client_stream.setHandshakeTimeout(5);

    ssl_client_tasks.setInsecure();
    ssl_client_tasks.setTimeout(1000);
    ssl_client_tasks.setHandshakeTimeout(5);
}