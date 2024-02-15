#include <ProMake_ADC081.h>
#include <math.h>
#include <ProMake_debug.h>
/********************************************************
	Constructor
*********************************************************/
ProMake_ADC081::ProMake_ADC081()
{
	voltage = 0.0;
	ADC_I2C_ADDRESS = 0x50;
}

ProMake_ADC081::ProMake_ADC081(uint8_t addr)
{
	voltage = 0.0;
	ADC_I2C_ADDRESS = addr;
}

/********************************************************
	Configure Sensor
*********************************************************/
void ProMake_ADC081::begin()
{
	if (ProMakeCore.write8(ADC_I2C_ADDRESS, ADC_REG_CONF, ADC_CONF_CYC_TIME_256))
	{
		PROMAKE_LOGDEBUG("ProMake_ADC081::begin() success");
	}
	else
	{
		PROMAKE_LOGDEBUG("ProMake_ADC081::begin() failed");
	}
}

/********************************************************
	Read Data from ADC081C021
*********************************************************/
void ProMake_ADC081::poll()
{
	readVoltage();
}

/********************************************************
	Request ADC voltage
*********************************************************/
float ProMake_ADC081::getVoltage()
{
	return voltage;
}

/* Private Functions */

/********************************************************
	Read Voltage
*********************************************************/
void ProMake_ADC081::readVoltage()
{
	uint16_t data;

	if (ProMakeCore.read16(ADC_I2C_ADDRESS, ADC_REG_RESULT, data))
	{

		a = (uint8_t)((data & 0xFF00) >> 8);
		b = (uint8_t)((data & 0x00FF) >> 0);

		voltage = ((((a & 0x0F) * 256) + (b & 0xF0)) / 0x10) * (3.3 / 256);
	}
	else
	{
		voltage = -1.0;
	}
}
