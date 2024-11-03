#include "ProMake_PCA9555.h"
#include <ProMake_debug.h>

#define PCA9555_IN_REG1 0x00
#define PCA9555_IN_REG2 0x01
#define PCA9555_OUT_REG1 0x02
#define PCA9555_OUT_REG2 0x03
#define PCA9555_POL_REG1 0x04
#define PCA9555_POL_REG2 0x05
#define PCA9555_CONF_REG1 0x06
#define PCA9555_CONF_REG2 0x07

// All pins in output mode by default
void ProMake_PCA9555::configure(uint8_t addr, uint16_t polInv, uint16_t io)
{
    ProMakeCore.write16(addr, PCA9555_POL_REG1, polInv);
    ProMakeCore.write16(addr, PCA9555_CONF_REG1, io);
    delay(15);
}

uint16_t ProMake_PCA9555::in(uint8_t addr)
{
    if (ProMakeCore.write1(addr, PCA9555_IN_REG1))
        return 1;
    delay(15);
    uint16_t readValue = ProMakeCore.request16(addr, true);
    if (readValue == ERROR_I2C_TIMEOUT)
    {
        return (ERROR_I2C_TIMEOUT);
    }
    return readValue;
}

void ProMake_PCA9555::out(uint8_t addr, uint16_t data)
{
    if (ProMakeCore.write16(addr, PCA9555_OUT_REG1, data))
    {
        PROMAKE_LOGDEBUG("ProMake_PCA9555::out success");
    }
    else
    {
        PROMAKE_LOGDEBUG("ProMake_PCA9555::out failed");
    }
}
