#include "ProMake_SHT20.h"
#include <ProMake_debug.h>

#define SLAVE_ADDRESS 0x40

#define TRIGGER_TEMP_MEASURE_HOLD 0xE3
#define TRIGGER_HUMD_MEASURE_HOLD 0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD 0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD 0xF5

#define WRITE_USER_REG 0xE6
#define READ_USER_REG 0xE7
#define SOFT_RESET 0xFE

#define USER_REGISTER_RESOLUTION_MASK 0x81
#define USER_REGISTER_RESOLUTION_RH12_TEMP14 0x00
#define USER_REGISTER_RESOLUTION_RH8_TEMP12 0x01
#define USER_REGISTER_RESOLUTION_RH10_TEMP13 0x80
#define USER_REGISTER_RESOLUTION_RH11_TEMP11 0x81
#define USER_REGISTER_END_OF_BATTERY 0x40
#define USER_REGISTER_HEATER_ENABLED 0x04
#define USER_REGISTER_DISABLE_OTP_RELOAD 0x02

#define MAX_WAIT 100
#define DELAY_INTERVAL 10
#define SHIFTED_DIVISOR 0x988000
#define MAX_COUNTER (MAX_WAIT / DELAY_INTERVAL)

using namespace ProMake;

uint16_t SHT20::readValue(byte cmd)
{
    write1(SLAVE_ADDRESS, cmd);
    uint32_t readValue = request24(SLAVE_ADDRESS, DELAY_INTERVAL, MAX_COUNTER);

    if (readValue == ERROR_I2C_TIMEOUT)
    {
        return (ERROR_I2C_TIMEOUT);
    }

    uint8_t checksum = (uint8_t)((readValue & 0x00FF));
    uint16_t rawValue = (uint16_t)((readValue & 0x00FFFF00) >> 8);

    if (checkCRC(rawValue, checksum) != 0)
    {
        return (ERROR_BAD_CRC);
    }
    return rawValue & 0xFFFC;
}

float SHT20::readHumidity(void)
{
    uint16_t rawHumidity = readValue(TRIGGER_HUMD_MEASURE_NOHOLD);
    if (rawHumidity == ERROR_I2C_TIMEOUT || rawHumidity == ERROR_BAD_CRC)
    {
        return (rawHumidity);
    }
    float tempRH = rawHumidity * (125.0 / 65536.0);
    float rh = tempRH - 6.0;
    return (rh);
}

float SHT20::readTemperatureC(void)
{
    uint16_t rawTemperature = readValue(TRIGGER_TEMP_MEASURE_NOHOLD);
    if (rawTemperature == ERROR_I2C_TIMEOUT || rawTemperature == ERROR_BAD_CRC)
    {
        return (rawTemperature);
    }
    float tempTemperature = rawTemperature * (175.72 / 65536.0);
    float realTemperatureC = tempTemperature - 46.85;
    return (realTemperatureC);
}

float SHT20::readTemperatureF(void)
{
    uint16_t rawTemperature = readValue(TRIGGER_TEMP_MEASURE_NOHOLD);
    if (rawTemperature == ERROR_I2C_TIMEOUT || rawTemperature == ERROR_BAD_CRC)
    {
        return (rawTemperature);
    }
    float tempTemperature = rawTemperature * (175.72 / 65536.0);
    float realTemperatureC = tempTemperature - 46.85;
    float realTemperatureF = (realTemperatureC * 1.8) + 32;
    return (realTemperatureF);
}

float SHT20::vpd()
{
    float tempC = readTemperatureC();
    float RH = readHumidity();

    float es = 0.6108 * exp(17.27 * tempC / (tempC + 237.3));
    float ae = RH / 100 * es;
    float vpd_kPa = es - ae;

    return vpd_kPa;
}

float SHT20::dew_pointC()
{
    float tempC = readTemperatureC();
    float RH = readHumidity();

    float tem = -1.0 * tempC;
    float esdp = 6.112 * exp(-1.0 * 17.67 * tem / (243.5 - tem));
    float ed = RH / 100.0 * esdp;
    float eln = log(ed / 6.112);
    float dew_pointC = -243.5 * eln / (eln - 17.67);
    return dew_pointC;
}

float SHT20::dew_pointF()
{
    float tempC = readTemperatureC();
    float RH = readHumidity();

    float tem = -1.0 * tempC;
    float esdp = 6.112 * exp(-1.0 * 17.67 * tem / (243.5 - tem));
    float ed = RH / 100.0 * esdp;
    float eln = log(ed / 6.112);
    float dew_pointC = -243.5 * eln / (eln - 17.67);
    float dew_pointF = (dew_pointC * 1.8) + 32;
    return dew_pointF;
}

void SHT20::setResolution(byte resolution)
{
    byte userRegister = readUserRegister();
    userRegister &= 0b01111110;
    resolution &= 0b10000001;
    userRegister |= resolution;
    writeUserRegister(userRegister);
}

byte SHT20::readUserRegister(void)
{
    byte value = 0;
    read8(SLAVE_ADDRESS, READ_USER_REG, value);
    return value;
}

void SHT20::writeUserRegister(byte val)
{
    write8(SLAVE_ADDRESS, WRITE_USER_REG, val);
}

byte SHT20::checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor)
{
    uint32_t remainder = (uint32_t)message_from_sensor << 8;
    remainder |= check_value_from_sensor;
    uint32_t divsor = (uint32_t)SHIFTED_DIVISOR;
    for (int i = 0; i < 16; i++)
    {
        if (remainder & (uint32_t)1 << (23 - i))
        {
            remainder ^= divsor;
        }
        divsor >>= 1;
    }
    return (byte)remainder;
}

void SHT20::showResult(const __FlashStringHelper *prefix, int val)
{
    PROMAKE_LOGDEBUG0(prefix);
    if (val)
    {
        PROMAKE_LOGDEBUG("yes");
    }
    else
    {
        PROMAKE_LOGDEBUG("no");
    }
}

void SHT20::checkSHT20(void)
{
    byte reg = readUserRegister();
    showResult(F("End of battery: "), reg & USER_REGISTER_END_OF_BATTERY);
    showResult(F("Heater enabled: "), reg & USER_REGISTER_HEATER_ENABLED);
    showResult(F("Disable OTP reload: "), reg & USER_REGISTER_DISABLE_OTP_RELOAD);
}