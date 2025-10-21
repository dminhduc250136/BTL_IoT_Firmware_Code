#include "config.h"
#include "time_handler.h"

void setupNTP()
{
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("Đang đồng bộ thời gian NTP...");

    // Chờ lấy thời gian
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Lỗi đồng bộ NTP. Kiểm tra lại Internet!");
        return;
    }
    Serial.println("Đồng bộ thời gian thành công!");
    Serial.println(getCurrentTime());
}

String getCurrentTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return "Unknown";
    }
    char timeString[30];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(timeString);
}