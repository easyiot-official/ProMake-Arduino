/********************************************************

 	Core Functions include:
 	-	I2C Communication
 	-	Inter device communication

*********************************************************/

#ifndef	ProMake_Core_h
#define ProMake_Core_h

#pragma once

#include "inttypes.h"
#include <Arduino.h>
#include "ProMake_Version.h"
#include "Wire.h"


/*********************************************************/
class ProMake_CoreClass{
	public:
		ProMake_CoreClass(){}
		uint16_t	request16(byte device);
		uint32_t	request24(byte device, byte delay, byte timeout);
		void 		write(byte value);
		void 		write1(byte device, byte value);
		void 		write8(byte device, byte reg, byte value);
		void 		write16(byte device, byte reg, uint16_t value);
		uint8_t		readStream(byte device);
		uint8_t 	read8(byte device, byte reg);
		uint16_t	read16(byte device, byte reg);
		int16_t 	readS16(byte device,byte reg);
		uint16_t  	read16_LE(byte device, byte reg);
		int16_t   	readS16_LE(byte device, byte reg);
		uint32_t 	read24(byte device, byte reg);
		bool		ping(byte device);
};

extern ProMake_CoreClass ProMakeCore;
// end of header
#endif
