/********************************************************

	Core Functions include:
	-	I2C Communication
	-	Inter device communication

*********************************************************/

#include "ProMake_Core.h"

using namespace ProMake;

/********************************************************
	Writes an 8 bit value over I2C
*********************************************************/
bool CoreClass::write8(byte device, byte reg, byte value)
{
	wire->beginTransmission((uint8_t)device);
	if (wire->write((uint8_t)reg) != 1)
	{
		return false;
	}
	if (wire->write((uint8_t)value) != 1)
	{
		return false;
	}
	if (wire->endTransmission())
	{
		return false;
	}
	return true;
}

/********************************************************
	Writes over I2C
*********************************************************/
bool CoreClass::write(byte value)
{
	if (wire->write((uint8_t)value) != 1)
	{
		return false;
	}
	return true;
}

bool CoreClass::write1(byte device, byte value)
{
	wire->beginTransmission((uint8_t)device);
	if (wire->write((uint8_t)value) != 1)
	{
		return false;
	}
	if (wire->endTransmission())
	{
		return false;
	}
	return true;
}

/********************************************************
	Writes an 16 bit value over I2C
*********************************************************/
bool CoreClass::write16(byte device, byte reg, uint16_t value)
{
	wire->beginTransmission((uint8_t)device);

	if (wire->write((uint8_t)reg) != 1)
	{
		return false;
	}
	// wire->write(0xFF & (value >> 0));
	// wire->write(0xFF & (value << 8));

	if (wire->write(value >> 0) != 1)
	{
		return false;
	}

	if (wire->write(value >> 8) != 1)
	{
		return false;
	}
	if (wire->endTransmission())
	{
		return false;
	}
	return true;
}

/********************************************************
	Reads an Stream over I2C
*********************************************************/
uint8_t CoreClass::readStream(byte device)
{
	uint8_t value = 0;
	wire->requestFrom((uint8_t)device, (uint8_t)1);
	if (wire->available())
	{
		value = wire->read();
	}
	return value;
}

/********************************************************
	Reads an 8 bit value over I2C
*********************************************************/
bool CoreClass::read8(byte device, byte reg, byte &value)
{
	wire->beginTransmission((uint8_t)device);
	if (wire->write((uint8_t)reg) != 1)
	{
		return false;
	}
	if (wire->endTransmission())
	{
		return false;
	}
	if (wire->requestFrom((uint8_t)device, (uint8_t)1) != 1)
	{
		return false;
	}
	if (wire->available())
	{
		value = wire->read();
		return true;
	}
	return false;
}
/********************************************************
	Reads an 16 bit value over I2C
*********************************************************/
bool CoreClass::read16(byte device, byte reg, uint16_t &value)
{
	wire->beginTransmission((uint8_t)device);
	if (wire->write((uint8_t)reg) != 1)
	{
		return false;
	}
	if (wire->endTransmission())
	{
		return false;
	}
	if (wire->requestFrom((uint8_t)device, (uint8_t)2) != 2)
	{
		return false;
	}
	if (wire->available())
	{
		value = wire->read();
		value <<= 8;
		value |= wire->read();
		return true;
	}
	return false;
}

bool CoreClass::read16(byte device, byte reg, bool sendStop, uint16_t &value)
{
	wire->beginTransmission((uint8_t)device);
	if (wire->write((uint8_t)reg) != 1)
	{
		return false;
	}
	if (wire->endTransmission(sendStop))
	{
		return false;
	}
	if (wire->requestFrom((uint8_t)device, (uint8_t)2) != 2)
	{
		return false;
	}

	if (wire->available())
	{
		value = wire->read();
		value <<= 8;
		value |= wire->read();
		return true;
	}
	return false;
}

bool CoreClass::readS16(byte device, byte reg, int16_t &value)
{
	return read16(device, reg, (uint16_t &)value);
}

bool CoreClass::readS16_LE(byte device, byte reg, int16_t &value)
{
	return read16_LE(device, reg, (uint16_t &)value);
}

bool CoreClass::read16_LE(byte device, byte reg, uint16_t &value)
{
	uint16_t temp = 0;
	if (read16(device, reg, temp))
	{
		value = (temp >> 8) | (temp << 8);
		return true;
	}
	return false;
}

bool CoreClass::read16_LE(byte device, byte reg, bool sendStop, uint16_t &value)
{
	uint16_t temp = 0;
	if (read16(device, reg, sendStop, temp))
	{
		value = (temp >> 8) | (temp << 8);
		return true;
	}
	return false;
}

uint8_t CoreClass::request8(byte device, bool sendStop)
{
	uint8_t value = 0;
	wire->requestFrom((uint8_t)device, (size_t)1, sendStop);
	if (wire->available())
	{
		value = wire->read();
	}
	return value;
}

uint16_t CoreClass::request16(byte device, bool sendStop)
{
	uint16_t value = 0;
	wire->requestFrom((uint8_t)device, (size_t)2, sendStop);
	if (wire->available())
	{
		value = wire->read();
		value <<= 8;
		value |= wire->read();
	}
	return value;
}

uint32_t CoreClass::request24(byte device, byte delay_int, byte timeout)
{
	uint32_t value = 0;
	byte counter = 0;
	byte toRead = wire->requestFrom((uint8_t)device, (uint8_t)3);
	while (toRead != 3 && counter < timeout)
	{
		delay(delay_int);
		toRead = wire->requestFrom((uint8_t)device, (uint8_t)3);
		counter++;
	}

	if (counter == timeout)
	{
		return (ERROR_I2C_TIMEOUT);
	}

	value = wire->read();
	value <<= 8;
	value |= wire->read();
	value <<= 8;
	value |= wire->read();

	return value;
}

/********************************************************
	Reads a signed 24 bit value over I2C
*********************************************************/
uint32_t CoreClass::read24(byte device, byte reg)
{
	uint32_t value = 0;
	wire->beginTransmission((uint8_t)device);
	wire->write((uint8_t)reg);
	wire->endTransmission();
	wire->requestFrom((uint8_t)device, (uint8_t)3);
	if (wire->available())
	{
		value = wire->read();
		value <<= 8;
		value |= wire->read();
		value <<= 8;
		value |= wire->read();
	}
	return value;
}

/********************************************************
	Ping and I2C device address
*********************************************************/
bool CoreClass::ping(byte device)
{
	// Set flag status
	bool status = false;
	// Check the flag status to check if OD01 is connected and ping was succesful
	for (byte i = 0; i < 10; i++)
	{
		wire->beginTransmission((uint8_t)device);
		if (wire->endTransmission() == 0)
		{				   // Device is connected and ping successful
			status = true; // set flag
			break;		   // exit loop
		}
		else if (i == 9)
		{					// Device not found cannot ping device address
			status = false; // set flag
			break;			// exit loop
		}
		delay(1);
	}
	return status;
}

