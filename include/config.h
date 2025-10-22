#ifndef CONFIG_H
#define CONFIG_H

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

// ======== Thư viện ========
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <time.h>
#include <math.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <FirebaseJSON.h>

// ======== Cấu hình cảm biến ========
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135_PIN 34
#define MQ_R0 79000.0
#define MQ_RL 10000.0
#define MQ_VCC 5.0
#define RELAY_PIN 5
#define PM25_LED_PIN 18
#define PM25_SENSOR_PIN 35
#define FAN_ON HIGH
#define FAN_OFF LOW

// ======== Cấu hình WiFi ========
#define WIFI_SSID "Xiaomi 11T"
#define WIFI_PASSWORD "111111111"

// ======== Cấu hình Firebase ========
#define API_KEY "AIzaSyCVzXG0ctuMb4EOmQPz2he58gj4TDsWFl4"
#define DATABASE_URL "https://air-quality-database-b2e01-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "esp_so1@gmail.com"
#define USER_PASSWORD "123456"

// Đổi tên DEVICE_ID thành ROOT để giống repo
#define ROOT "/node1"

// ======== Cấu hình OLED ========
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define TEMP_THRESHOLD_HIGH 37.78
#define TEMP_THRESHOLD_LOW 32.0
#define HUMIDITY_THRESHOLD_HIGH 80.0
#define HUMIDITY_THRESHOLD_LOW 60.0
#define MQ135PPM_THRESHOLD_HIGH 400.0
#define MQ135PPM_THRESHOLD_LOW 100.0
#define PM25_THRESHOLD_HIGH 12
#define PM25_THRESHOLD_LOW 5

// ======== CÁC BIẾN HẸN GIỜ (TIMERS) ========
static unsigned long lastSend = 0;
#define SEND_INTERVAL 5000 // 5 giây (Gửi data)

static unsigned long lastSerialPrint = 0;
#define READ_INTERVAL 5000  // 5 giây (In log)
#define PRINT_INTERVAL 5000 // 5 giây (In log)

// TIMER MỚI: Khởi động lại stream mỗi 5 phút
static unsigned long lastStreamStart = 0;
#define STREAM_RESTART_INTERVAL 300000 // 300.000ms = 5 phút
// ============================================================

// ======== Khai báo các đối tượng toàn cục ========
extern DHT dht;
extern Adafruit_SSD1306 display;

// THAY ĐỔI đối tượng Firebase toàn cục (giống repo)
extern FirebaseApp app;
extern WiFiClientSecure ssl_client_stream; // Client cho Stream
extern WiFiClientSecure ssl_client_tasks;  // Client cho Tasks
using AsyncClient = AsyncClientClass;      // Giống repo
extern AsyncClient aClient_Tasks;
extern AsyncClient aClient_Stream;
extern RealtimeDatabase Database;
extern AsyncResult dbResult;
extern UserAuth user_auth; // Đối tượng xác thực

// Biến trạng thái
extern bool firebaseConnected;
extern bool fanStreamStarted;

struct Dht11Data
{
    float temperature = 0;
    float humidity = 0;
    unsigned long lastRead = 0;
    bool initialized = false;
    bool error = false;
};
extern Dht11Data dht11Data;

struct FanData
{
    bool isManualMode = false;
    bool manualStateOn = false;
    bool fanOn = false;
    unsigned long lastCheck = 0;
    bool initialized = false;
};
extern FanData fanData;

struct MQ135Data
{
    float ppm = 0;
    unsigned long lastRead = 0;
    bool initialized = false;
    bool error = false;
};
extern MQ135Data mq135Data;
struct PM25Data
{
    float concentration = 0;
    unsigned long lastRead = 0;
    bool initialized = false;
    bool error = false;
};
extern PM25Data pm25Data;

#endif // CONFIG_H