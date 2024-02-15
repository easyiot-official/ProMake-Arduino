/********************************************************

	Core Functions include:
	-	I2C Communication
	-	Inter device communication

*********************************************************/

#ifndef PROMAKE_Core_H_INCLUDED
#define PROMAKE_Core_H_INCLUDED

#pragma once

#include "inttypes.h"
#include <Arduino.h>
#include "ProMake_Version.h"
#include "Wire.h"

#define ERROR_I2C_TIMEOUT 998
#define ERROR_BAD_CRC 999

/*********************************************************/
class ProMake_CoreClass
{
private:
	// int a,b,c,d;
public:
	ProMake_CoreClass() {}
	uint8_t request8(byte device);
	uint16_t request16(byte device);
	uint32_t request24(byte device, byte delay, byte timeout);
	bool write(byte value);
	bool write1(byte device, byte value);
	bool write8(byte device, byte reg, byte value);
	bool write16(byte device, byte reg, uint16_t value);
	uint8_t readStream(byte device);
	bool read8(byte device, byte reg, byte &value);
	bool read16(byte device, byte reg, uint16_t &value);
	bool read16_LE(byte device, byte reg, uint16_t &value);
	bool read16(byte device, byte reg, bool sendStop, uint16_t &value);
	bool read16_LE(byte device, byte reg, bool sendStop, uint16_t &value);
	bool readS16(byte device, byte reg, int16_t &value);
	bool readS16_LE(byte device, byte reg, int16_t &value);
	uint32_t read24(byte device, byte reg);
	bool ping(byte device);
};

/*!
 *  @brief  Class that defines Interrupt Lock Avaiability
 */
class InterruptLock
{
public:
	InterruptLock()
	{
#if !defined(ARDUINO_ARCH_NRF52)
		noInterrupts();
#endif
	}
	~InterruptLock()
	{
#if !defined(ARDUINO_ARCH_NRF52)
		interrupts();
#endif
	}
};

extern ProMake_CoreClass ProMakeCore;
// end of header
#endif
