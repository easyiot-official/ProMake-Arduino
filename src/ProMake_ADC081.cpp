#include <ProMake_ADC081.h>
#include <math.h>
#include <ProMake_debug.h>

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

using namespace ProMake;
/********************************************************
	Constructor
*********************************************************/
ADC081::ADC081()
{
	voltage = 0.0;
	ADC_I2C_ADDRESS = 0x50;
}

ADC081::ADC081(uint8_t addr)
{
	voltage = 0.0;
	ADC_I2C_ADDRESS = addr;
}

/********************************************************
	Configure Sensor
*********************************************************/
void ADC081::begin()
{
	if (write8(ADC_I2C_ADDRESS, ADC_REG_CONF, ADC_CONF_CYC_TIME_256))
	{
		PROMAKE_LOGDEBUG("ADC081::begin() success");
	}
	else
	{
		PROMAKE_LOGDEBUG("ADC081::begin() failed");
	}
}

/********************************************************
	Read Data from ADC081C021
*********************************************************/
void ADC081::poll()
{
	readVoltage();
}

/********************************************************
	Request ADC voltage
*********************************************************/
float ADC081::getVoltage()
{
	return voltage;
}

/* Private Functions */

/********************************************************
	Read Voltage
*********************************************************/
void ADC081::readVoltage()
{
	uint16_t data;

	if (read16(ADC_I2C_ADDRESS, ADC_REG_RESULT, data))
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
