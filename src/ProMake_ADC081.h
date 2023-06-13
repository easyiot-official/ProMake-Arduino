/*
	Analog IN -	Max Input 3.3V

	The board uses I2C for communication.
	
	The board communicates with one I2C devices:
	- ADC081C021
	
	Data Sheets:
	ADC081C021 - http://www.ti.com/lit/ds/symlink/adc081c021.pdf
*/

#ifndef PROMAKE_ADC081_H_INCLUDED
#define PROMAKE_ADC081_H_INCLUDED

// System Include
#include <inttypes.h>
#include "ProMake_Core.h"

// System Defines
#define ADC_REG_RESULT			0x00
#define ADC_REG_ALERT			0x01
#define ADC_REG_CONF			0x02
#define ADC_REG_LOW_LIM			0x03
#define ADC_REG_HIGH_LIM		0x04
#define ADC_REG_HYSTERESIS		0x05
#define ADC_REG_LOW_CONVR		0x06
#define ADC_REG_HIGH_COVR		0x07

#define ADC_ALERT_OVER_V		0x01
#define ADC_ALERT_UNDER_V		0x00

#define ADC_CONF_CYC_TIME_OFF	0x00
#define ADC_CONF_CYC_TIME_32	0x20	
#define ADC_CONF_CYC_TIME_64	0x40
#define ADC_CONF_CYC_TIME_128	0x50
#define ADC_CONF_CYC_TIME_256	0x80
#define ADC_CONF_CYC_TIME_512	0xA0
#define ADC_CONF_CYC_TIME_1024	0xC0
#define ADC_CONF_CYC_TIME_2048	0xE0
#define ADC_CONF_ALERT_MAN		0x01
#define ADC_CONF_FLAG_EN		0x08

class ProMake_ADC081: public ProMake_CoreClass
{
	public:
		/**
		* Constructor
		* Creates a new instance of Sensor class.
		*/	
		ProMake_ADC081();
		ProMake_ADC081(uint8_t addr);

		/*
		* Runs the setup of the sensor. 
		* Call this in setup(), before reading any sensor data.
		*
		* @return true if setup was successful.
		*/
		void 	begin(void);

		/*
		* Used read all raw sensors data and convert to usefull results. 
		* Call this in loop(). Used to call sensor data reads.
		*
		* @return none
		*/
		void 	poll(void);

		/*
		* Used to get the voltage measurement from the sensor
		* Call this in loop(). 
		*
		* @return voltage. Returns float value of input voltage.
		*/	
		float	getVoltage(void);
	
	private:
		/*
		* Used to read the raw input voltage from sensor and convert to 
		* usable float value. Called inside poll().
		*
		* @return none.
		*/	
		void 	readVoltage(void);
		
		// Device I2C Address
		uint8_t ADC_I2C_ADDRESS;

		float	voltage;
		uint8_t a,b;
};

#endif
