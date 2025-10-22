#include "config.h"
#include "wifi_handler.h"
#include "time_handler.h"
#include "sensors.h"
#include "display_handler.h"
#include "firebase_handler.h"
#include "fan_control.h"

// ======== Định nghĩa các đối tượng toàn cục (Thay đổi) ========
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
FirebaseApp app;
WiFiClientSecure ssl_client_stream; // Client cho Stream
WiFiClientSecure ssl_client_tasks;  // Client cho Tasks

AsyncClient aClient_Stream(ssl_client_stream); // Client cho Stream
AsyncClient aClient_Tasks(ssl_client_tasks);
RealtimeDatabase Database;
AsyncResult dbResult;
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

// ...
bool firebaseConnected = false;
bool fanStreamStarted = false;

Dht11Data dht11Data;
MQ135Data mq135Data;
PM25Data pm25Data;
FanData fanData;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // Thiết lập SSL client
    ssl_client_stream.setInsecure();
    ssl_client_stream.setTimeout(1000);
    ssl_client_stream.setHandshakeTimeout(10);

    ssl_client_tasks.setInsecure();
    ssl_client_tasks.setTimeout(1000);
    ssl_client_tasks.setHandshakeTimeout(10);

    setupDHT11();
    setupMQ135();
    setupPM25();
    setupOLED();
    setupWiFi();
    setupNTP();
    setupFan();
    setupFirebase();

    Serial.println("====== Khoi tao he thong hoan tat ======");
}

void loop()
{
    readDHT11();
    readMQ135();
    readPM25();

    app.loop();

    if (app.ready())
    {
        firebaseConnected = true;

        // Gửi lên Firebase
        if (millis() - lastSend >= SEND_INTERVAL)
        {
            Serial.println("TEMP: " + String(dht11Data.temperature) + " C, HUMI: " + String(dht11Data.humidity) + " %, PPM: " + String(mq135Data.ppm) + " ug/m3" + ", PM2.5: " + String(pm25Data.concentration) + " ug/m3" + "STATUS: " + getAirQualityLabel(mq135Data.ppm, pm25Data.concentration, dht11Data.temperature, dht11Data.humidity));
            displayOLED(dht11Data.temperature, dht11Data.humidity, mq135Data.ppm, getAirQualityLabel(mq135Data.ppm, pm25Data.concentration, dht11Data.temperature, dht11Data.humidity));
            sendToFirebase();
            lastSend = millis();
        }

        if (!fanData.isManualMode && (millis() - autoFanLastUpload >= UPLOAD_AUTOFAN))
        {
            uploadAutoFanState();
            autoFanLastUpload = millis();
        }

        // set fan mode and fan state first time
        if (fanData.lastCheck == 0)
        {
            uploadFanData();
            fanData.lastCheck = 1;
        }

        if (!fanStreamStarted)
        {
            delay(200);
            startFanStream();
        }
    }
    else
    {
        Serial.println("Firebase app is not ready. Resetting all flags...");
        firebaseConnected = false;
        fanStreamStarted = false;
        fanData.lastCheck = 0;
    }

    if (!fanData.isManualMode)
    {
        handleAutoFanControl();
    }

    delay(100);
}