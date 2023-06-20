#include "ProMake_RS485.h"

#ifdef __MBED__
#include "pinDefinitions.h"
ProMake_RS485::ProMake_RS485(Stream &Serial, PinName dePin, PinName rePin) : _serial(&Serial),
                                                                             _dePin(PinNameToIndex(dePin)),
                                                                             _rePin(PinNameToIndex(rePin)),
                                                                             _transmisionBegun(false)
{
}
#endif

ProMake_RS485::ProMake_RS485(Stream &Serial, int dePin, int rePin) : _serial(&Serial),
                                                                     _dePin(dePin),
                                                                     _rePin(rePin),
                                                                     _transmisionBegun(false)
{
}

void ProMake_RS485::begin(unsigned long baudrate)
{
    begin(baudrate, RS485_DEFAULT_PRE_DELAY, RS485_DEFAULT_POST_DELAY);
}

void ProMake_RS485::begin(unsigned long baudrate, int predelay, int postdelay)
{
    _baudrate = baudrate;

    // Set only if not already initialized with ::setDelays
    _predelay = _predelay == 0 ? predelay : _predelay;
    _postdelay = _postdelay == 0 ? postdelay : _postdelay;

    if (_dePin > -1)
    {
        pinMode(_dePin, OUTPUT);
        digitalWrite(_dePin, LOW);
    }

    if (_rePin > -1)
    {
        pinMode(_rePin, OUTPUT);
        digitalWrite(_rePin, HIGH);
    }

    _transmisionBegun = false;
    /*
    #if defined(ARDUINO_OPTA)
      auto _opta_uart = static_cast<UART *>(_serial);
      _opta_uart->begin(baudrate, true);
    #else
      _serial->begin(baudrate);
    #endif
    */
}

void ProMake_RS485::end()
{
    //_serial->end();

    if (_dePin > -1)
    {
        digitalWrite(_dePin, LOW);
        pinMode(_dePin, INPUT);
    }

    if (_rePin > -1)
    {
        digitalWrite(_rePin, LOW);
        pinMode(_rePin, INPUT);
    }
}

int ProMake_RS485::available()
{
    return _serial->available();
}

int ProMake_RS485::peek()
{
    return _serial->peek();
}

int ProMake_RS485::read(void)
{
    return _serial->read();
}

void ProMake_RS485::flush()
{
    return _serial->flush();
}

size_t ProMake_RS485::write(uint8_t b)
{
    if (!_transmisionBegun)
    {
        setWriteError();
        return 0;
    }

    return _serial->write(b);
}

ProMake_RS485::operator bool()
{
    return true;
}

void ProMake_RS485::beginTransmission()
{
    if (_dePin > -1)
    {
        digitalWrite(_dePin, HIGH);
        if (_predelay)
            delayMicroseconds(_predelay);
    }

    _transmisionBegun = true;
}

void ProMake_RS485::endTransmission()
{
    _serial->flush();

    if (_dePin > -1)
    {
        if (_postdelay)
            delayMicroseconds(_postdelay);
        digitalWrite(_dePin, LOW);
    }

    _transmisionBegun = false;
}

void ProMake_RS485::receive()
{
    if (_rePin > -1)
    {
        digitalWrite(_rePin, LOW);
    }
}

void ProMake_RS485::noReceive()
{
    if (_rePin > -1)
    {
        digitalWrite(_rePin, HIGH);
    }
}

void ProMake_RS485::sendBreak(unsigned int duration)
{
    _serial->flush();
    //_serial->end();
    delay(duration);
    //_serial->begin(_baudrate);
}

void ProMake_RS485::sendBreakMicroseconds(unsigned int duration)
{
    _serial->flush();
    //_serial->end();
    delayMicroseconds(duration);
    //_serial->begin(_baudrate);
}

void ProMake_RS485::setPins(int dePin, int rePin)
{
    _dePin = dePin;
    _rePin = rePin;
}

void ProMake_RS485::setDelays(int predelay, int postdelay)
{
    _predelay = predelay;
    _postdelay = postdelay;
}