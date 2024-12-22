#ifndef PROMAKE_SHT20_H_INCLUDED
#define PROMAKE_SHT20_H_INCLUDED

#include "ProMake_Core.h"


namespace ProMake
{

    class SHT20 : public CoreClass
    {
    public:
        void checkSHT20(void);
        void setResolution(byte resBits);
        void writeUserRegister(byte val);

        void showResult(const __FlashStringHelper *prefix, int val);
        float readHumidity(void);
        float readTemperatureC(void);
        float readTemperatureF(void);
        float vpd();
        float dew_pointC();
        float dew_pointF();
        byte readUserRegister(void);

    private:
        byte checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor);
        uint16_t readValue(byte cmd);
    };

}

#endif