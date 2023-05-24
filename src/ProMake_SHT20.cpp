#include "ProMake_SHT20.h"


uint16_t ProMake_SHT20::readValue(byte cmd)
{
    ProMakeCore.write1(SLAVE_ADDRESS, cmd);
    uint32_t readValue = ProMakeCore.request24(SLAVE_ADDRESS, DELAY_INTERVAL, MAX_COUNTER);

    if(readValue == ERROR_I2C_TIMEOUT){
        return (ERROR_I2C_TIMEOUT);
    }

    uint8_t checksum= (uint8_t)((readValue & 0x00FF));
    uint16_t rawValue = (uint16_t)((readValue & 0x00FFFF00) >> 8);


    if(checkCRC(rawValue, checksum) != 0){
        return (ERROR_BAD_CRC);
    }
    return rawValue & 0xFFFC;
}

float ProMake_SHT20::readHumidity(void)
{
    uint16_t rawHumidity = readValue(TRIGGER_HUMD_MEASURE_NOHOLD);
    if(rawHumidity == ERROR_I2C_TIMEOUT || rawHumidity == ERROR_BAD_CRC){
        return(rawHumidity);
    }
    float tempRH = rawHumidity * (125.0 / 65536.0);
    float rh = tempRH - 6.0;
    return (rh);
}

float ProMake_SHT20::readTemperature(void)
{
    uint16_t rawTemperature = readValue(TRIGGER_TEMP_MEASURE_NOHOLD);
    if(rawTemperature == ERROR_I2C_TIMEOUT || rawTemperature == ERROR_BAD_CRC){
        return(rawTemperature);
    }
    float tempTemperature = rawTemperature * (175.72 / 65536.0);
    float realTemperature = tempTemperature - 46.85;
    return (realTemperature);
}

void ProMake_SHT20::setResolution(byte resolution)
{
    byte userRegister = readUserRegister();
    userRegister &= B01111110;
    resolution &= B10000001;
    userRegister |= resolution;
    writeUserRegister(userRegister);
}

byte ProMake_SHT20::readUserRegister(void)
{
    return ProMakeCore.read8(SLAVE_ADDRESS, READ_USER_REG);
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
    for(int i = 0 ; i < 16 ; i++){
        if(remainder & (uint32_t)1 << (23 - i)){
            remainder ^= divsor;
        }
        divsor >>= 1;
    }
    return (byte)remainder;
}

void ProMake_SHT20::showResult(const char *prefix, int val)
{
    Serial.print(prefix);
    if(val){
        Serial.println("yes");
    }else{
        Serial.println("no");
    }
}

void ProMake_SHT20::checkSHT20(void)
{
    byte reg = readUserRegister();
    showResult("End of battery: ", reg & USER_REGISTER_END_OF_BATTERY);
    showResult("Heater enabled: ", reg & USER_REGISTER_HEATER_ENABLED);
    showResult("Disable OTP reload: ", reg & USER_REGISTER_DISABLE_OTP_RELOAD);
}