/*
	This is a library for the SX02 
	Analog IN -	Max Input 3.3V

	The board uses I2C for communication.
	
	The board communicates with one I2C devices:
	- ADC081C021
	
	Data Sheets:
	ADC081C021 - http://www.ti.com/lit/ds/symlink/adc081c021.pdf
*/

#include <ProMake_ADC081.h>
#include <math.h>

/********************************************************
 	Constructor
*********************************************************/
ProMake_ADC081::ProMake_ADC081(){
	voltage = 0.0;
	ADC_I2C_ADDRESS = 0x50;
}

ProMake_ADC081::ProMake_ADC081(uint8_t addr){
	voltage = 0.0;
	ADC_I2C_ADDRESS = addr;
}

/********************************************************
 	Configure Sensor
*********************************************************/
void ProMake_ADC081::begin(){
	ProMakeCore.write8(ADC_I2C_ADDRESS, ADC_REG_CONF, ADC_CONF_CYC_TIME_256); 
}

/********************************************************
 	Read Data from ADC081C021
*********************************************************/
void ProMake_ADC081::poll(){
	readVoltage();
}

/********************************************************
 	Request ADC voltage
*********************************************************/
float ProMake_ADC081::getVoltage(){
	return voltage;
}

/* Private Functions */
 
/********************************************************
 	Read Voltage
*********************************************************/
void ProMake_ADC081::readVoltage(){
	uint16_t data;
	
	data = ProMakeCore.read16(ADC_I2C_ADDRESS, ADC_REG_RESULT);
	
	a = (uint8_t)((data & 0xFF00) >> 8);
	b = (uint8_t)((data & 0x00FF) >> 0);
	
	voltage = ((((a & 0x0F)*256) + (b & 0xF0))/0x10)*(3.3/256);
}





