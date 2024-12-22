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

namespace ProMake{

class ADC081: public CoreClass
{
	public:
		/**
		* Constructor
		* Creates a new instance of Sensor class.
		*/	
		ADC081();
		ADC081(uint8_t addr);

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

}
#endif
