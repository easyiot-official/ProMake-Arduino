#ifndef PROMAKE_PCA9555_H_INCLUDED
#define PROMAKE_PCA9555_H_INCLUDED

#include "ProMake_Core.h"

#define PCA95XX_IN_ALL 0xFFFF
#define PCA95XX_OUT_ALL 0x0000
#define PCA95XX_POL_DEFAULT_ALL 0x0000
#define PCA95XX_POL_INVERTED_ALL 0xFFFF

class ProMake_PCA9555 : public ProMake_CoreClass
{
public:
    void configure(uint8_t addr, uint16_t polInv = PCA95XX_POL_DEFAULT_ALL, uint16_t io = PCA95XX_OUT_ALL);

    uint16_t in(uint8_t addr);
    void out(uint8_t addr, uint16_t data);

private:

};

#endif