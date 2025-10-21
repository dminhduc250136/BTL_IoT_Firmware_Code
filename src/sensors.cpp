#include "config.h"
#include "sensors.h"

void setupDHT11()
{

    dht.begin();
    delay(1000);

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (isnan(temp) || isnan(hum))
    {
        Serial.println("Failed!");
        dht11Data.error = true;
        return;
    }
    dht11Data.initialized = true;

    Serial.println("Khởi tạo cảm biến thành công.");
    return;
}

void setupMQ135()
{
    float mq135_initial_adc = analogRead(MQ135_PIN);
    float initial_ppm = MQ135_calculator(mq135_initial_adc);

    if (initial_ppm <= 0)
    {
        mq135Data.error = true;
        Serial.println("MQ135 initialization error!");
        return;
    }

    mq135Data.initialized = true;
    Serial.println("MQ135 initialized successfully.");
    return;
}

void setupPM25()
{

    pinMode(PM25_LED_PIN, OUTPUT);
    pinMode(PM25_SENSOR_PIN, INPUT);
    float pm25_initial_adc = analogRead(PM25_SENSOR_PIN);
    delayMicroseconds(40);
    digitalWrite(PM25_LED_PIN, HIGH);
    delayMicroseconds(9680);

    float initial_pm25 = PM25_calculator(pm25_initial_adc);

    if (initial_pm25 < 0)
    {
        pm25Data.error = true;
        Serial.println("PM2.5 initialization error!");
        return;
    }

    pm25Data.initialized = true;
    Serial.println("PM2.5 initialized successfully.");
    return;
}

void readDHT11()
{
    unsigned long currentTime = millis();

    if (currentTime - dht11Data.lastRead < READ_INTERVAL)
    {
        return;
    }
    dht11Data.lastRead = currentTime;

    if (!dht11Data.initialized)
    {
        return;
    }

    dht11Data.temperature = dht.readTemperature();
    dht11Data.humidity = dht.readHumidity();

    if (isnan(dht11Data.temperature) || isnan(dht11Data.humidity))
    {
        Serial.println("DHT11 reading error!");
        dht11Data.error = true;
        return;
    }

    dht11Data.error = false;
    return;
}

void readMQ135()
{
    unsigned long currentTime = millis();

    if (currentTime - mq135Data.lastRead < READ_INTERVAL)
    {
        return;
    }
    mq135Data.lastRead = currentTime;
    if (!mq135Data.initialized)
    {
        return;
    }
    int adcValue = analogRead(MQ135_PIN);
    mq135Data.ppm = MQ135_calculator(adcValue);

    if (mq135Data.ppm <= 0)
    {
        Serial.println("MQ135 reading error!");
        mq135Data.error = true;
        return;
    }
    mq135Data.error = false;
    return;
}

void readPM25()
{
    unsigned long currentTime = millis();

    if (currentTime - pm25Data.lastRead < READ_INTERVAL)
    {
        return;
    }
    pm25Data.lastRead = currentTime;
    if (!pm25Data.initialized)
    {
        return;
    }
    pinMode(PM25_LED_PIN, OUTPUT);
    pinMode(PM25_SENSOR_PIN, INPUT);
    float pm25_initial_adc = analogRead(PM25_SENSOR_PIN);
    delayMicroseconds(40);
    digitalWrite(PM25_LED_PIN, HIGH);
    delayMicroseconds(9680);
    pm25Data.concentration = PM25_calculator(pm25_initial_adc);

    if (pm25Data.concentration < 0)
    {
        Serial.println("PM2.5 reading error!");
        pm25Data.error = true;
        return;
    }
    pm25Data.error = false;
    return;
}

float MQ135_calculator(int adcValue)
{
    if (adcValue <= 0)
        return 0;

    // 1. Tính Vout
    float Vout = (adcValue / 4095.0) * MQ_VCC;

    // 2. Tính Rs
    float Rs = MQ_RL * (MQ_VCC / Vout - 1.0);

    // 3. Tính tỉ lệ Rs/R0
    float ratio = Rs / MQ_R0;

    // 4. Tính ppm theo công thức thực nghiệm
    float a = 116.6020682;
    float b = 2.769034857;
    float ppm = a * pow(ratio, -b);

    return ppm;
}

float PM25_calculator(int adcValue)
{
    if (adcValue <= 0)
        return 0;

    // Công thức chuyển đổi
    float voltage = (adcValue / 4095.0) * 5.0;
    float concentration = voltage * 100.0;

    return concentration;
}

String getAirQualityLabel(float MQ135PPM, float PM25Concentration)
{
    if (MQ135PPM <= 400 && PM25Concentration <= 12)
    {
        return "Good";
    }
    else if (MQ135PPM <= 2000 && PM25Concentration <= 35.4)
    {
        return "Average";
    }
    else if (MQ135PPM <= 5000 && PM25Concentration <= 55.4)
    {
        return "Poor";
    }
    return "Very Poor";
}
