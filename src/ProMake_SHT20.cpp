#include "ProMake_SHT20.h"
#include <ProMake_debug.h>

uint16_t ProMake_SHT20::readValue(byte cmd)
{
    ProMakeCore.write1(SLAVE_ADDRESS, cmd);
    uint32_t readValue = ProMakeCore.request24(SLAVE_ADDRESS, DELAY_INTERVAL, MAX_COUNTER);

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

float ProMake_SHT20::readHumidity(void)
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

float ProMake_SHT20::readTemperatureC(void)
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

float ProMake_SHT20::readTemperatureF(void)
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

float ProMake_SHT20::vpd()
{
    float tempC = readTemperatureC();
    float RH = readHumidity();

    float es = 0.6108 * exp(17.27 * tempC / (tempC + 237.3));
    float ae = RH / 100 * es;
    float vpd_kPa = es - ae;

    return vpd_kPa;
}

float ProMake_SHT20::dew_pointC()
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

float ProMake_SHT20::dew_pointF()
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

void ProMake_SHT20::setResolution(byte resolution)
{
    byte userRegister = readUserRegister();
    userRegister &= 0b01111110;
    resolution &= 0b10000001;
    userRegister |= resolution;
    writeUserRegister(userRegister);
}

byte ProMake_SHT20::readUserRegister(void)
{
    byte value = 0;
    ProMakeCore.read8(SLAVE_ADDRESS, READ_USER_REG, value);
    return value;
}

void ProMake_SHT20::writeUserRegister(byte val)
{
    ProMakeCore.write8(SLAVE_ADDRESS, WRITE_USER_REG, val);
}

byte ProMake_SHT20::checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor)
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

void ProMake_SHT20::showResult(const __FlashStringHelper *prefix, int val)
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

void ProMake_SHT20::checkSHT20(void)
{
    byte reg = readUserRegister();
    showResult(F("End of battery: "), reg & USER_REGISTER_END_OF_BATTERY);
    showResult(F("Heater enabled: "), reg & USER_REGISTER_HEATER_ENABLED);
    showResult(F("Disable OTP reload: "), reg & USER_REGISTER_DISABLE_OTP_RELOAD);
}