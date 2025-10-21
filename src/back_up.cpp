// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <DHT.h>
// #include <Firebase_ESP_Client.h>
// #include <addons/TokenHelper.h>
// #include <addons/RTDBHelper.h>
// #include <WiFi.h>
// #include <time.h> // Thư viện lấy thời gian thực NTP
// #include <math.h>

// // ======== Cấu hình cảm biến ========
// #define DHTPIN 4
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);
// #define MQ135_PIN 34

// // ======== Cấu hình WiFi ========
// #define WIFI_SSID "Xiaomi 11T"
// #define WIFI_PASSWORD "111111111"

// // ======== Cấu hình Firebase ========
// #define API_KEY "AIzaSyCVzXG0ctuMb4EOmQPz2he58gj4TDsWFl4"
// #define DATABASE_URL "https://air-quality-database-b2e01-default-rtdb.asia-southeast1.firebasedatabase.app/"
// String EMAIL = "esp32_1@gmail.com";
// String PASSWORD = "esp32_1_123";
// String DEVICE_ID = "node1";
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;

// // ======== Cấu hình OLED ========
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// // ===============================

// // Hàm đọc DHT11

// // ===============================

// void readDHT11(float &temperature, float &humidity)

// {

//     temperature = dht.readTemperature();

//     humidity = dht.readHumidity();

//     if (isnan(temperature) || isnan(humidity))

//     {

//         Serial.println("Lỗi đọc DHT11!");

//         temperature = humidity = 0;
//     }
// }

// int readMQ135()

// {

//     return analogRead(MQ135_PIN);
// }

// float getPPM(int adcValue, float R0 = 72000.0, float RL = 10000.0, float Vcc = 5.0)

// {

//     if (adcValue <= 0)

//         return 0; // tránh chia cho 0

//     // 1. Tính Vout

//     float Vout = (adcValue / 4095.0) * Vcc;

//     // 2. Tính Rs

//     float Rs = RL * (Vcc / Vout - 1.0);

//     // 3. Tính tỉ lệ Rs/R0

//     float ratio = Rs / R0;

//     // 4. Tính ppm theo công thức thực nghiệm

//     float a = 116.6020682;

//     float b = 2.769034857;

//     float ppm = a * pow(ratio, -b);

//     return ppm;
// }

// String getAirQualityLabel(float ppmValue)

// {

//     if (ppmValue < 400)

//         return "Good";

//     else if (ppmValue < 2000)

//         return "Average";

//     else if (ppmValue < 5000)

//         return "Poor";

//     else

//         return "Very Poor";
// }

// // ===============================

// // Lấy thời gian thực từ NTP

// // ===============================

// String getCurrentTime()

// {

//     struct tm timeinfo;

//     if (!getLocalTime(&timeinfo))

//     {

//         return "Unknown";
//     }

//     char timeString[30];

//     strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);

//     return String(timeString);
// }

// // ===============================

// // Hiển thị OLED

// // ===============================

// void displayOLED(float t, float h, float ppm, const String &airQuality)

// {

//     display.clearDisplay();

//     display.setTextColor(SSD1306_WHITE);

//     display.setTextSize(1);

//     display.setCursor(0, 5);

//     display.printf("Nhiet do: %.1f *C", t);

//     display.setCursor(0, 20);

//     display.printf("Do am: %.1f %%", h);

//     display.setCursor(0, 35);

//     display.printf("MQ135: %.2f", ppm);

//     display.setCursor(0, 50);

//     display.print("Chat luong: ");

//     display.print(airQuality);

//     display.display();
// }

// // ===============================

// // Gửi dữ liệu lên Firebase

// // ===============================

// void sendToFirebase(float temp, float humi, float ppm, const String &status)

// {

//     double ppm_value = round((double)ppm * 100.0) / 100.0;

//     String timestamp = getCurrentTime();

//     // Dữ liệu gửi lên

//     FirebaseJson json;

//     json.set("TEMP", temp);

//     json.set("HUMI", humi);

//     json.set("PPM", ppm_value); // làm tròn 2 chữ số thập phân

//     json.set("STATUS", status);

//     json.set("TIMESTAMP", timestamp);

//     json.set("TIMESTAMP", timestamp);

//     // Tạo node mới trong /history (Firebase sẽ tự sinh ID)

//     if (Firebase.RTDB.pushJSON(&fbdo, "/history/" + DEVICE_ID, &json))

//     {

//         Serial.println("Đã lưu bản ghi mới lên Firebase!");
//     }

//     else

//     {

//         Serial.printf("Lỗi Firebase: %s\n", fbdo.errorReason().c_str());
//     }

//     // Nếu vẫn muốn cập nhật dữ liệu hiện tại (realtime)

//     String basePath = "/latest/" + DEVICE_ID;

//     Firebase.RTDB.setFloat(&fbdo, basePath + "/TEMP", temp);

//     Firebase.RTDB.setFloat(&fbdo, basePath + "/HUMI", humi);

//     Firebase.RTDB.setInt(&fbdo, basePath + "/PPM", ppm_value);

//     Firebase.RTDB.setString(&fbdo, basePath + "/STATUS", status);

//     Firebase.RTDB.setString(&fbdo, basePath + "/TIMESTAMP", timestamp);
// }

// double calculatePPM(int adcVal)

// {

//     double Vcc = 5.0;

//     double RL = 10000.0;

//     double R0 = 71900.0;

//     double A = 1.0;

//     double B = -0.4;

//     int ADC_max = 4095;

//     if (adcVal <= 0 || adcVal >= ADC_max)

//         return -1; // tránh chia 0

//     // 1️⃣ Chuyển ADC -> điện áp đầu ra

//     double Vout = (double)adcVal / ADC_max * Vcc;

//     // 2️⃣ Tính Rs (điện trở cảm biến)

//     double Rs = RL * (Vcc - Vout) / Vout;

//     // 3️⃣ Tính tỉ lệ Rs/R0

//     double ratio = Rs / R0;

//     // 4️⃣ Tính ppm theo mô hình lũy thừa (Rs/R0 = A * ppm^B)

//     double ppm = pow((ratio / A), (1.0 / B));

//     return ppm;
// }

// // ===============================

// // SETUP

// // ===============================

// void setup()

// {

//     Serial.begin(115200);

//     dht.begin();

//     if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))

//     {

//         Serial.println("Loi OLED!");

//         while (true)

//             ;
//     }

//     display.clearDisplay();

//     display.display();

//     // WiFi

//     WiFi.mode(WIFI_STA);

//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

//     Serial.print("🔌 Đang kết nối WiFi");

//     while (WiFi.status() != WL_CONNECTED)

//     {

//         delay(300);

//         Serial.print(".");
//     }

//     Serial.println("\n WiFi connected!");

//     Serial.print("IP: ");

//     Serial.println(WiFi.localIP());

//     // --- Thiết lập múi giờ và đồng bộ NTP ---

//     configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");

//     Serial.println("Đang đồng bộ thời gian NTP...");

//     delay(2000);

//     Serial.println("Đồng bộ thời gian thành công!");

//     // Firebase setup

//     config.api_key = API_KEY;

//     config.database_url = DATABASE_URL;

//     auth.user.email = EMAIL;

//     auth.user.password = PASSWORD;

//     if (Firebase.signUp(&config, &auth, "", ""))

//     {

//         Serial.println("Firebase SignUp thành công!");
//     }

//     else

//     {

//         Serial.printf("Firebase SignUp thất bại: %s\n", config.signer.signupError.message.c_str());
//     }

//     // Bật log token

//     config.token_status_callback = tokenStatusCallback;

//     Firebase.begin(&config, &auth);

//     Firebase.reconnectWiFi(true);
// }

// // ===============================

// // LOOP

// // ===============================

// void loop()

// {

//     float temperature, humidity;

//     readDHT11(temperature, humidity);

//     int mqValue = readMQ135();

//     float ppmValue = getPPM(mqValue);

//     String airQuality = getAirQualityLabel(ppmValue);

//     Serial.printf("Nhiet do: %.1f°C | Do am: %.1f%% | Nong do khi: %.2f | %s\n",

//                   temperature, humidity, ppmValue, airQuality.c_str());

//     displayOLED(temperature, humidity, ppmValue, airQuality);

//     sendToFirebase(temperature, humidity, ppmValue, airQuality);

//     delay(5000);
// }