#ifndef PROMAKE_RINGBUFFER_H_INCLUDED
#define PROMAKE_RINGBUFFER_H_INCLUDED


class ProMake_RingBuffer
{
public:
	ProMake_RingBuffer(unsigned int size);
	~ProMake_RingBuffer();

	void reset();
	void init();
	void push(char c);
	int getPos();
	bool endsWith(const char* str);
	void getStr(char * destination, unsigned int skipChars);
	void getStrN(char * destination, unsigned int skipChars, unsigned int num);


private:

	unsigned int _size;
	char* ringBuf;
	char* ringBufEnd;
	char* ringBufP;

};

#endif