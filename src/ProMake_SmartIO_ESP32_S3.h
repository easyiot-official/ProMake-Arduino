#ifndef PROMAKE_SMARTIO_ESP32_S3_H_INCLUDED
#define PROMAKE_SMARTIO_ESP32_S3_H_INCLUDED

#include "RTClib.h"
#include "Adafruit_NeoPixel.h"
#include "ProMake_PCA9554.h"
#include "ProMake_SHT20.h"

namespace ProMake
{

    class SmartIO_ESP32_S3
    {
    public:
        enum RGBLED : uint8_t
        {
            MSG1,
            NET1,
            MSG2,
            NET2
        };

        enum Relay : uint8_t
        {
            RLY1,
            RLY2,
            RLY3,
            RLY4
        };

    public:
        SmartIO_ESP32_S3();
        void begin();

        bool keyPressed(void);
        void setBuzzerTone(int freq);

        bool rtcLostPower(void);
        void rtcAdjust(const DateTime &dt);
        DateTime rtcNow();

        void setRgbLedColor(RGBLED led, uint8_t r, uint8_t g, uint8_t b);
        void setRelay(Relay id, bool on);

        float readHumidity(void);
        float readTemperatureC(void);
        float readTemperatureF(void);

    private:
        RTC_PCF8563 m_rtc;
        Adafruit_NeoPixel m_pixels;
        PCA9554 m_ioex;
        SHT20 m_sht20;
    };

}

#endif
