#include "config.h"
#include "firebase_handler.h"
#include "time_handler.h"
#include "sensors.h"
#include "fan_control.h"

void setupFirebase()
{
    initializeApp(aClient_Tasks, app, getAuth(user_auth), processData, "Auth Task");
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
    Serial.println("Khoi tao Firebase (Async Client)...");
}

void sendToFirebase()
{
    if (!firebaseConnected)
    {
        Serial.println("Firebase chua ket noi, hoac du lieu cam bien chua san sang. Bo qua gui du lieu.");
        return;
    }

    // 1. Dùng FirebaseJSON
    FirebaseJson jsonPayload;
    jsonPayload.set("TEMP", dht11Data.temperature);
    jsonPayload.set("HUMI", dht11Data.humidity);
    jsonPayload.set("PPM", mq135Data.ppm);
    jsonPayload.set("PM25", pm25Data.concentration);
    jsonPayload.set("STATUS", getAirQualityLabel(mq135Data.ppm, pm25Data.concentration, dht11Data.temperature, dht11Data.humidity));
    jsonPayload.set("TIMESTAMP", getCurrentTime());

    object_t payloadObject(jsonPayload.raw());

    String latestPath = "/latest" + String(ROOT);
    // Gửi đi và chỉ định hàm processData làm callback
    Database.set<object_t>(aClient_Tasks, latestPath, payloadObject, processData, "set latest");
    String historyPath = "/history" + String(ROOT);
    Database.push<object_t>(aClient_Tasks, historyPath, payloadObject, processData, "push history");
}

// Hàm callback chung cho TẤT CẢ các tác vụ
void processData(AsyncResult &aResult)
{
    if (!aResult.isResult())
        return;

    // In ra kết quả của tác vụ (ví dụ: "set latest", "push history")
    if (aResult.isDebug())
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

    if (aResult.isError())
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

    // Nếu thành công, có thể in payload
    if (aResult.available())
        Firebase.printf("Task %s thanh cong, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}

void uploadFanData()
{
    if (!firebaseConnected)
        return;
    FirebaseJson json;
    json.set("autoMode", false);
    json.set("fan", String("OFF"));
    String path = "/controls" + String(ROOT);
    Database.set<object_t>(aClient_Stream, path, object_t(json.raw()), processData, "initialized fan mode");
    return;
}

void processFanData(AsyncResult &aResult)
{
    if (aResult.isError())
    {
        Serial.println("----------------------------");
        Firebase.printf("LỖI STREAM QUẠT: %s\n", aResult.error().message().c_str());
        Firebase.printf("Resetting fanStreamStarted flag.\n");
        fanStreamStarted = false; // "Gác máy"
        return;
    }

    if (!aResult.isResult())
    {
        return;
    }

    if (aResult.available())
    {
        RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();

        // Chỉ xử lý nếu đây là một sự kiện stream
        if (stream.isStream())
        {
            Serial.println("----------------------------");
            Firebase.printf("Fan Stream Event Received!\n");
            Firebase.printf("path: %s\n", stream.dataPath().c_str());
            Firebase.printf("Data: %s\n", stream.to<const char *>());

            bool isManual = fanData.isManualMode;
            bool manualStateOn = fanData.manualStateOn;

            if (stream.dataPath() == "/")
            {
                FirebaseJson json;
                json.setJsonData(stream.to<String>());
                FirebaseJsonData data;

                if (json.get(data, "autoMode"))
                {
                    isManual = (data.to<bool>() == false);
                }
                if (json.get(data, "fan"))
                {
                    manualStateOn = (data.to<String>() == "ON");
                }
            }
            else if (stream.dataPath() == "/fan")
            {
                String state = stream.to<String>();
                manualStateOn = (state == "ON");
            }
            else if (stream.dataPath() == "/autoMode")
            {
                bool mode = stream.to<bool>();
                isManual = (mode == false);
            }

            fanData.isManualMode = isManual;
            fanData.manualStateOn = manualStateOn;

            if (fanData.isManualMode)
            {
                Serial.println("Mode is MANUAL. Applying state from Firebase.");
                setFanState(fanData.manualStateOn);
            }
            else
            {
                Serial.println("Mode is AUTO. Control will be handled by sensors in the main loop.");
                setFanState(false); // Tắt quạt khi chuyển sang chế độ tự động
            }
        }
        else
        {
            // --- BƯỚC 3: STREAM ĐÃ ĐÓNG (Timeout hoặc bị server ngắt) ---
            Serial.println("----------------------------");
            Serial.println("Stream QUẠT đã đóng (timeout hoặc server ngắt).");
            Serial.println("Resetting fanStreamStarted flag.");
            fanStreamStarted = false;
        }
    }
}

void startFanStream()
{
    if (fanStreamStarted)
        return;
    Serial.println("Starting fan stream...");
    String path = "/controls" + String(ROOT);
    Database.get(aClient_Stream, path, processFanData, true /*SSE*/, "fanStreamTask");
    fanStreamStarted = true;
    return;
}

void uploadAutoFanState()
{
    if (firebaseConnected)
    {
        String path = "/controls" + String(ROOT) + "/fan";
        Database.set(aClient_Tasks, path, fanData.fanOn ? "ON" : "OFF", processData, "update auto fan status to firebase");
    }
}