#ifndef PROMAKE_PCA9554_H_INCLUDED
#define PROMAKE_PCA9554_H_INCLUDED

#include "ProMake_Core.h"

#define PCA95XX_IN_ALL 0xFF
#define PCA95XX_OUT_ALL 0x00
#define PCA95XX_POL_DEFAULT_ALL 0x00
#define PCA95XX_POL_INVERTED_ALL 0xFF

class ProMake_PCA9554 : public ProMake_CoreClass
{
public:
    void configure(uint8_t addr, uint8_t polInv = PCA95XX_POL_DEFAULT_ALL, uint8_t io = PCA95XX_OUT_ALL);

    uint8_t in(uint8_t addr);
    void out(uint8_t addr, uint8_t data);

private:

};

#endif