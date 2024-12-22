#include "ProMake_DHT11.h"
#include <ProMake_debug.h>
#include <ProMake_Core.h>
using namespace ProMake;

bool DHT11::read(int pin)
{
	uint8_t bits[5];
	uint8_t cnt = 7;
	uint8_t idx = 0;
	// Reset 40 bits of received data to zero.
	bits[0] = bits[1] = bits[2] = bits[3] = bits[4] = 0;

#if defined(ESP8266) || defined(ESP32)
	yield(); // Handle WiFi / reset software watchdog
#endif

	// Send start signal.  See DHT datasheet for full signal diagram:

	// Go into high impedence state to let pull-up raise data line level and
	// start the reading process.
	pinMode(pin, INPUT_PULLUP);
	delay(1);

	// First set data line low for a period according to sensor type
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delay(20); // data sheet says at least 18ms, 20ms just to be safe

	// End the start signal by setting data line high for 40 microseconds.
	pinMode(pin, INPUT_PULLUP);
	delayMicroseconds(40);

	// Turn off interrupts temporarily because the next sections
	// are timing critical and we don't want any interruptions.
	InterruptLock lock;

	// wating for get ready response
	unsigned int loopCnt = 10000;
	while (digitalRead(pin) == LOW)
	{
		if (loopCnt-- == 0)
		{
			PROMAKE_LOGDEBUG(F("DHT get reading timed out!"));
			return false;
		}
	}
	// waiting for first bit
	loopCnt = 30000;
	while (digitalRead(pin) == HIGH)
	{
		if (loopCnt-- == 0)
		{
			PROMAKE_LOGDEBUG(F("DHT first bit timed out!"));
			return false;
		}
	}
	// reading 40 bits
	for (int i = 0; i < 40; i++)
	{
		// start bit receive
		loopCnt = 10000;
		while (digitalRead(pin) == LOW)
		{
			if (loopCnt-- == 0)
			{
				PROMAKE_LOGDEBUG1(F("DHT bit start timed out! bit:"), i);
				return false;
			}
		}
		// measuring high duration
		unsigned long t = micros();
		while (digitalRead(pin) == HIGH)
		{
			if (loopCnt-- == 0)
			{
				PROMAKE_LOGDEBUG1(F("DHT bit length timed out! bit:"), i);
				return false;
			}
		}
		if ((micros() - t) > 50)
			bits[idx] |= (1 << cnt);
		if (cnt == 0)
		{
			cnt = 7;
			idx++;
		}
		else
		{
			cnt--;
		}
	}
	// Check we read 40 bits and that the checksum matches.
	if (bits[4] == ((bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF))
	{
		humidity = bits[0];
		temperature = bits[2];
		return true;
	}
	else
	{
		PROMAKE_LOGDEBUG(F("DHT checksum failure!"));
		return false;
	}
}
