#ifndef PROMAKE_DHT11_H_INCLUDED
#define PROMAKE_DHT11_H_INCLUDED

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
namespace ProMake
{
  class DHT11
  {
  public:
    /**
     * @fn read
     * @brief Read DHT11 data
     * @param pin: Connect the IO port of the DHT11 data port.
     * @return Checksum OK
     */
    bool read(int pin);
    int humidity;
    int temperature;
  };
} // namespace ProMake

#endif
