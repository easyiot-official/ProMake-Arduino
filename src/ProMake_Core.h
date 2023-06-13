/********************************************************

 	Core Functions include:
 	-	I2C Communication
 	-	Inter device communication

*********************************************************/

#ifndef	PROMAKE_Core_H_INCLUDED
#define PROMAKE_Core_H_INCLUDED

#pragma once

#include "inttypes.h"
#include <Arduino.h>
#include "ProMake_Version.h"
#include "Wire.h"

#if not defined(LED_RED) || not defined(LED_GREEN) || not defined(LED_BUILTIN)
#ifdef ESP8266				 // CW01
#define LED_RED 12
#define LED_GREEN 13
#if not defined(LED_BUILTIN) // LED_BLUE
#define LED_BUILTIN 5
#endif
#elif ESP32					  // CW02
#define LED_RED 25
#define LED_GREEN 26
#if not defined(LED_BUILTIN)
#define LED_BUILTIN 27
#endif
#elif ARDUINO_SAMD_ZERO		  // CS11
#define LED_RED 11
#define LED_GREEN 12
#if not defined(LED_BUILTIN)
#define LED_BUILTIN 13
#endif
#endif
#endif

#define ERROR_I2C_TIMEOUT                     998
#define ERROR_BAD_CRC                         999

/*********************************************************/
class ProMake_CoreClass{
	private:
		//int a,b,c,d;
	public:
		ProMake_CoreClass(){}
		uint8_t		request8(byte device);
		uint16_t	request16(byte device);
		uint32_t	request24(byte device, byte delay, byte timeout);
		void 		write(byte value);
		void 		write1(byte device, byte value);
		void 		write8(byte device, byte reg, byte value);
		void 		write16(byte device, byte reg, uint16_t value);
		uint8_t		readStream(byte device);
		uint8_t 	read8(byte device, byte reg);
		uint16_t	read16(byte device, byte reg);
		uint16_t	read16(byte device, byte reg, byte sendStop);
		int16_t 	readS16(byte device,byte reg);
		uint16_t  	read16_LE(byte device, byte reg);
		uint16_t  	read16_LE(byte device, byte reg, byte sendStop);
		int16_t   	readS16_LE(byte device, byte reg);
		uint32_t 	read24(byte device, byte reg);
		bool		ping(byte device);
};

extern ProMake_CoreClass ProMakeCore;
// end of header
#endif
