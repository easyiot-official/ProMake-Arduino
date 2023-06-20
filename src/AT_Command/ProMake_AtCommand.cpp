#include "AT_Command/ProMake_AtCommand.h"
#include <ProMake_debug.h>
#include <Arduino.h>




const char* ESPTAGS[] =
{
    "\r\nOK\r\n",
	"\r\nERROR\r\n",
	"\r\nFAIL\r\n",
    "\r\nSEND OK\r\n",
    " CONNECT\r\n"
};


ProMake_AtCommand::ProMake_AtCommand(Stream *serial):ringBuf(32)
{
    _serial = serial;
	PROMAKE_LOGDEBUG(F("> wifiDriverInit"));
}



/*
* Sends the AT command and stops if any of the TAGS is found.
* Extract the string enclosed in the passed tags and returns it in the outStr buffer.
* Returns true if the string is extracted, false if tags are not found of timed out.
*/
bool ProMake_AtCommand::sendCmdGet(const __FlashStringHelper* cmd, const char* startTag, const char* endTag, char* outStr, int outStrLen)
{
    int idx;
	bool ret = false;

	outStr[0] = 0;

	emptyBuf();

	PROMAKE_LOGDEBUG(F("SCG----------------------------------------------"));
	PROMAKE_LOGDEBUG1(F(">>"), cmd);

	// send AT command to ESP
	_serial->println(cmd);

	// read result until the startTag is found
	idx = readUntil(1000, startTag);

	if(idx==NUMESPTAGS)
	{
		// clean the buffer to get a clean string
		ringBuf.init();

		// start tag found, search the endTag
		idx = readUntil(500, endTag);

		if(idx==NUMESPTAGS)
		{
			// end tag found
			// copy result to output buffer avoiding overflow
			ringBuf.getStrN(outStr, strlen(endTag), outStrLen-1);

			// read the remaining part of the response
			readUntil(2000);

			ret = true;
		}
		else
		{
			PROMAKE_LOGWARN(F("End tag not found"));
		}
	}
	else if(idx>=0 and idx<NUMESPTAGS)
	{
		// the command has returned but no start tag is found
		PROMAKE_LOGDEBUG1(F("No start tag found:"), idx);
	}
	else
	{
		// the command has returned but no tag is found
		PROMAKE_LOGWARN(F("No tag found"));
	}

	PROMAKE_LOGDEBUG1(F("---------------------------------------------- >"), outStr);
	PROMAKE_LOGDEBUG();

	return ret;
}

bool ProMake_AtCommand::sendCmdGet(const __FlashStringHelper* cmd, const __FlashStringHelper* startTag, const __FlashStringHelper* endTag, char* outStr, int outStrLen)
{
	char _startTag[strlen_P((char*)startTag)+1];
	strcpy_P(_startTag,  (char*)startTag);

	char _endTag[strlen_P((char*)endTag)+1];
	strcpy_P(_endTag,  (char*)endTag);

	return sendCmdGet(cmd, _startTag, _endTag, outStr, outStrLen);
}


/*
* Sends the AT command and returns the id of the TAG.
* Return -1 if no tag is found.
*/
int ProMake_AtCommand::sendCmd(const __FlashStringHelper* cmd, int timeout)
{
    emptyBuf();

	PROMAKE_LOGDEBUG(F("SC----------------------------------------------"));
	PROMAKE_LOGDEBUG1(F(">>"), cmd);

	_serial->println(cmd);

	int idx = readUntil(timeout);

	PROMAKE_LOGDEBUG1(F("---------------------------------------------- >"), idx);
	PROMAKE_LOGDEBUG();

    return idx;
}


/*
* Sends the AT command and returns the id of the TAG.
* The additional arguments are formatted into the command using sprintf.
* Return -1 if no tag is found.
*/
int ProMake_AtCommand::sendCmd(const __FlashStringHelper* cmd, int timeout, ...)
{
	char cmdBuf[CMD_BUFFER_SIZE];

	va_list args;
	va_start (args, timeout);
	vsnprintf_P (cmdBuf, CMD_BUFFER_SIZE, (char*)cmd, args);
	va_end (args);

	emptyBuf();

	PROMAKE_LOGDEBUG(F("----------------------------------------------"));
	PROMAKE_LOGDEBUG1(F(">>"), cmdBuf);

	_serial->println(cmdBuf);

	int idx = readUntil(timeout);

	PROMAKE_LOGDEBUG1(F("---------------------------------------------- >"), idx);
	PROMAKE_LOGDEBUG();

	return idx;
}


// Read from serial until one of the tags is found
// Returns:
//   the index of the tag found in the ESPTAGS array
//   -1 if no tag was found (timeout)
int ProMake_AtCommand::readUntil(int timeout, const char* tag, bool findTags)
{
	ringBuf.reset();

	char c;
    unsigned long start = millis();
	int ret = -1;

	while ((millis() - start < timeout) and ret<0)
	{
        if(_serial->available())
		{
            c = (char)_serial->read();
			PROMAKE_LOGDEBUG0(c);
			ringBuf.push(c);

			if (tag!=NULL)
			{
				if (ringBuf.endsWith(tag))
				{
					ret = NUMESPTAGS;
					//PROMAKE_LOGDEBUG1("xxx");
				}
			}
			if(findTags)
			{
				for(int i=0; i<NUMESPTAGS; i++)
				{
					if (ringBuf.endsWith(ESPTAGS[i]))
					{
						ret = i;
						break;
					}
				}
			}
		}
    }

	if (millis() - start >= timeout)
	{
		PROMAKE_LOGWARN(F(">>> TIMEOUT >>>"));
	}

    return ret;
}

void ProMake_AtCommand::emptyBuf(bool warn)
{
    char c;
	int i=0;
	while(_serial->available() > 0)
    {
		c = _serial->read();
		if (i>0 and warn==true)
			PROMAKE_LOGDEBUG0(c);
		i++;
	}
	if (i>0 and warn==true)
    {
		PROMAKE_LOGDEBUG(F(""));
		PROMAKE_LOGDEBUG1(F("Dirty characters in the serial buffer! >"), i);
	}
}
