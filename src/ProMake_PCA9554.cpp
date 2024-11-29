#include "ProMake_PCA9554.h"
#include <ProMake_debug.h>

#define PCA95XX_IN_REG 0x00
#define PCA95XX_OUT_REG 0x01
#define PCA95XX_POL_REG 0x02
#define PCA95XX_CONF_REG 0x03

// All pins in output mode by default
void ProMake_PCA9554::configure(uint8_t addr, uint8_t polInv, uint8_t io)
{
    ProMakeCore.write8(addr, PCA95XX_POL_REG, polInv);
    ProMakeCore.write8(addr, PCA95XX_CONF_REG, io);
    delay(15);
}

uint8_t ProMake_PCA9554::in(uint8_t addr)
{
    if (ProMakeCore.write1(addr, PCA95XX_IN_REG))
        return 1;
    delay(15);
    uint8_t readValue = ProMakeCore.request8(addr, true);

    return readValue;
}

void ProMake_PCA9554::out(uint8_t addr, uint8_t data)
{
    ProMakeCore.write8(addr, PCA95XX_OUT_REG, data);
}
