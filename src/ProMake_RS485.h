#ifndef PROMAKE_RS485_H_INCLUDED
#define PROMAKE_RS485_H_INCLUDED

#include <Arduino.h>

#define RS485_DEFAULT_PRE_DELAY 50
#define RS485_DEFAULT_POST_DELAY 50

class ProMake_RS485 : public Stream {
  public:
#ifdef __MBED__
    ProMake_RS485(Stream& Serial, PinName dePin, PinName rePin);
#endif
    ProMake_RS485(Stream& Serial, int dePin, int rePin);

    virtual void begin(unsigned long baudrate);
    virtual void begin(unsigned long baudrate, int predelay, int postdelay);
    virtual void end();
    virtual int available();
    virtual int peek();
    virtual int read(void);
    virtual void flush();
    virtual size_t write(uint8_t b);
    using Print::write; // pull in write(str) and write(buf, size) from Print
    virtual operator bool();

    void beginTransmission();
    void endTransmission();
    void receive();
    void noReceive();

    void sendBreak(unsigned int duration);
    void sendBreakMicroseconds(unsigned int duration);

    void setPins(int dePin, int rePin);

    void setDelays(int predelay, int postdelay);

  private:
    Stream* _serial;
    int _dePin;
    int _rePin;
    int _predelay = 0;
    int _postdelay = 0;

    bool _transmisionBegun;
    unsigned long _baudrate;
};

#endif
