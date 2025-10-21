#include "config.h"
#include "display_handler.h"

void setupOLED()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("Loi OLED!");
        while (true)
            ; // Treo ở đây nếu lỗi OLED
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println("Khoi tao man hinh...");
    display.display();
    delay(1000);
}

void displayOLED(float t, float h, float ppm, const String &airQuality)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 5);
    display.printf("Nhiet do: %.1f *C", t);

    display.setCursor(0, 20);
    display.printf("Do am: %.1f %%", h);

    display.setCursor(0, 35);
    display.printf("MQ135: %.2f", ppm);

    display.setCursor(0, 50);
    display.print("Chat luong: ");
    display.print(airQuality);

    display.display();
}