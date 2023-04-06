/********************************************************

 	Core Functions include:								
 	-	I2C Communication												
 	-	Inter device communication

*********************************************************/
 
#include "ProMake_Core.h"

/********************************************************
 	Writes an 8 bit value over I2C
*********************************************************/
void ProMake_CoreClass::write8(byte device, byte reg, byte value) {
	Wire.beginTransmission((uint8_t)device);
	Wire.write((uint8_t)reg);
	Wire.write((uint8_t)value);
	Wire.endTransmission();
}

/********************************************************
 	Writes over I2C
*********************************************************/
void ProMake_CoreClass::write(byte value){
	Wire.write((uint8_t)value);
}

void ProMake_CoreClass::write1(byte device, byte value){
	Wire.beginTransmission((uint8_t)device);
	Wire.write((uint8_t)value);
	Wire.endTransmission();
}

/********************************************************
 	Writes an 16 bit value over I2C
*********************************************************/
void ProMake_CoreClass::write16(byte device, byte reg, uint16_t value) {
	Wire.beginTransmission((uint8_t)device);
	Wire.write((uint8_t)reg);
	//Wire.write(0xFF & (value >> 0));
	//Wire.write(0xFF & (value << 8));
	Wire.write(value >> 0);
	Wire.write(value << 8);	
	Wire.endTransmission();
}

/********************************************************
 	Reads an Stream over I2C
*********************************************************/
uint8_t ProMake_CoreClass::readStream(byte device){
	uint8_t value = 0;
	Wire.requestFrom((uint8_t)device, (uint8_t)1);
	if(Wire.available()){
		value = Wire.read();
	}
	return value;
}

/********************************************************
 	Reads an 8 bit value over I2C
*********************************************************/
uint8_t ProMake_CoreClass::read8(byte device, byte reg) {
	uint8_t value = 0;
	Wire.beginTransmission((uint8_t)device);
	Wire.write((uint8_t)reg);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)device, (uint8_t)1);
	if(Wire.available())
	{
		value = Wire.read();
	}
	return value;
}

/********************************************************
 	Reads an 16 bit value over I2C
*********************************************************/
uint16_t ProMake_CoreClass::read16(byte device, byte reg) {
	uint16_t value = 0;
	Wire.beginTransmission((uint8_t)device);
	Wire.write((uint8_t)reg);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)device, (uint8_t)2);
	if(Wire.available()){
		value = Wire.read(); 
		value <<= 8;
		value |= Wire.read();
	}
	return value;
}

int16_t ProMake_CoreClass::readS16(byte device, byte reg) {
	return (int16_t)read16(device, reg);
}

int16_t ProMake_CoreClass::readS16_LE(byte device, byte reg){
    return (int16_t)read16_LE(device, reg);
}

uint16_t ProMake_CoreClass::read16_LE(byte device, byte reg) {
    uint16_t temp = read16(device, reg);
    return (temp >> 8) | (temp << 8);
}

uint16_t ProMake_CoreClass::request16(byte device){
	uint16_t value = 0;
	Wire.requestFrom((uint8_t)device,(uint8_t)2);
	if(Wire.available()){
		value = Wire.read();
		value <<= 8;
		value |= Wire.read();
	}
	return value;
} 

/********************************************************
 	Reads a signed 24 bit value over I2C
*********************************************************/
uint32_t ProMake_CoreClass::read24(byte device, byte reg) {
	uint32_t value = 0;
	Wire.beginTransmission((uint8_t)device);
	Wire.write((uint8_t)reg);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)device, (uint8_t)3);
	if(Wire.available()){	
		value = Wire.read();
		value <<= 8;
		value |= Wire.read();
		value <<= 8;
		value |= Wire.read();
	}
	return value;
}

/********************************************************
 	Ping and I2C device address
*********************************************************/
bool ProMake_CoreClass::ping(byte device) {
  // Set flag status
  bool status = false;
  // Check the flag status to check if OD01 is connected and ping was succesful
  for (byte i = 0; i < 10; i++) {
    Wire.beginTransmission((uint8_t)device);
    if (Wire.endTransmission() == 0) { // Device is connected and ping successful
      status = true; // set flag
      break; // exit loop
    }
    else if (i == 9) { // Device not found cannot ping device address
      status = false; // set flag
      break; // exit loop
    }
    delay(1);
  }
  return status;
}

ProMake_CoreClass ProMakeCore = ProMake_CoreClass();
