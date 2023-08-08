#ifndef PROMAKE_DHT11_H_INCLUDED
#define PROMAKE_DHT11_H_INCLUDED

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class ProMake_DHT11{
public:
  /**
   * @fn read
   * @brief Read DHT11 data
   * @param pin: Connect the IO port of the DHT11 data port.
   * @return NONE    
   */
  void read(int pin);
  int humidity;
  int temperature;
};
#endif
