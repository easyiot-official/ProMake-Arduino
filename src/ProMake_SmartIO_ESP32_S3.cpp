#include "ProMake_SmartIO_ESP32_S3.h"
#include <ProMake_debug.h>

#define PIN_BUZZER 46    // buzzer pin
#define PIN_RFINPUT 15   // RF receiver pin
#define KEY1_PIN 0       // push button 1 pin
#define WS2812B_PIN 48   // WS2812B data pin
#define WS2812B_NUMBER 4 // number of WS2812B LEDs
#define PCA_ADDR 0x3C

using namespace ProMake;

SmartIO_ESP32_S3::SmartIO_ESP32_S3() : m_pixels(WS2812B_NUMBER, WS2812B_PIN, NEO_GRB + NEO_KHZ800) {}

void SmartIO_ESP32_S3::begin()
{
    pinMode(KEY1_PIN, INPUT); // init user button key
    pinMode(PIN_BUZZER, OUTPUT); 
    pinMode(PIN_RFINPUT, INPUT);

    Wire.begin();
    m_rtc.begin();

    // init RGB LED
    m_pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    m_pixels.clear(); // Set all pixel colors to 'off'

    m_ioex.attach(Wire, PCA_ADDR);
    m_ioex.polarity(PCA9554::PolarityAll::ORIGINAL_ALL);
    m_ioex.direction(0xF0);
    m_ioex.write(0x0F);

    m_sht20.attach(Wire);
    m_sht20.checkSHT20(); // Check SHT20 Sensor
}

bool SmartIO_ESP32_S3::keyPressed(void){
    return digitalRead(KEY1_PIN) == LOW;
}

void SmartIO_ESP32_S3::setBuzzerTone(int freq){
    analogWrite(PIN_BUZZER, freq);
}

bool SmartIO_ESP32_S3::rtcLostPower()
{
    return m_rtc.lostPower();
}

void SmartIO_ESP32_S3::rtcAdjust(const DateTime &dt)
{
    m_rtc.adjust(dt);
}
DateTime SmartIO_ESP32_S3::rtcNow()
{
    return m_rtc.now();
}

void SmartIO_ESP32_S3::setRgbLedColor(RGBLED led, uint8_t r, uint8_t g, uint8_t b)
{
    m_pixels.setPixelColor(led, r, g, b);
    m_pixels.show();
}

void SmartIO_ESP32_S3::setRelay(Relay id, bool on)
{
    switch (id)
    {
    case Relay::RLY1:
        m_ioex.write(PCA9554::Port::P0, on ? PCA9554::Level::L : PCA9554::Level::H);
        break;
    case Relay::RLY2:
        m_ioex.write(PCA9554::Port::P1, on ? PCA9554::Level::L : PCA9554::Level::H);
        break;
    case Relay::RLY3:
        m_ioex.write(PCA9554::Port::P2, on ? PCA9554::Level::L : PCA9554::Level::H);
        break;
    case Relay::RLY4:
        m_ioex.write(PCA9554::Port::P3, on ? PCA9554::Level::L : PCA9554::Level::H);
        break;

    default:
        break;
    }
}

float SmartIO_ESP32_S3::readHumidity(void)
{
    return m_sht20.readHumidity();
}

float SmartIO_ESP32_S3::readTemperatureC(void)
{
    return m_sht20.readTemperatureC();
}

float SmartIO_ESP32_S3::readTemperatureF(void)
{
    return m_sht20.readTemperatureF();
}
