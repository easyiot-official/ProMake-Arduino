#include "ProMake_RingBuffer.h"

#include <Arduino.h>

ProMake_RingBuffer::ProMake_RingBuffer(unsigned int size)
{
	_size = size;
	// add one char to terminate the string
	ringBuf = new char[size+1];
	ringBufEnd = &ringBuf[size];
	init();
}

ProMake_RingBuffer::~ProMake_RingBuffer() {}

void ProMake_RingBuffer::reset()
{
	ringBufP = ringBuf;
}

void ProMake_RingBuffer::init()
{
	ringBufP = ringBuf;
	memset(ringBuf, 0, _size+1);
}

void ProMake_RingBuffer::push(char c)
{
	*ringBufP = c;
	ringBufP++;
	if (ringBufP>=ringBufEnd)
		ringBufP = ringBuf;
}



bool ProMake_RingBuffer::endsWith(const char* str)
{
	int findStrLen = strlen(str);

	// b is the start position into the ring buffer
	char* b = ringBufP-findStrLen;
	if(b < ringBuf)
		b = b + _size;

	char *p1 = (char*)&str[0];
	char *p2 = p1 + findStrLen;

	for(char *p=p1; p<p2; p++)
	{
		if(*p != *b)
			return false;

		b++;
		if (b == ringBufEnd)
			b=ringBuf;
	}

	return true;
}



void ProMake_RingBuffer::getStr(char * destination, unsigned int skipChars)
{
	int len = ringBufP-ringBuf-skipChars;

	// copy buffer to destination string
	strncpy(destination, ringBuf, len);

	// terminate output string
	//destination[len]=0;
}

void ProMake_RingBuffer::getStrN(char * destination, unsigned int skipChars, unsigned int num)
{
	int len = ringBufP-ringBuf-skipChars;

	if (len>num)
		len=num;

	// copy buffer to destination string
	strncpy(destination, ringBuf, len);

	// terminate output string
	//destination[len]=0;
}
