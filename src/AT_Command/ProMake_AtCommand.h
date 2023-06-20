#ifndef PROMAKE_AT_COMMAND_H_INCLUDED
#define PROMAKE_AT_COMMAND_H_INCLUDED

#include "Stream.h"

#include "ProMake_RingBuffer.h"

// maximum size of AT command
#define CMD_BUFFER_SIZE 200

#define NUMESPTAGS 5
typedef enum
{
	TAG_OK,
	TAG_ERROR,
	TAG_FAIL,
	TAG_SENDOK,
	TAG_CONNECT
} TagsEnum;

class ProMake_AtCommand
{

public:
    ProMake_AtCommand(Stream *serial);

    int sendCmd(const __FlashStringHelper *cmd, int timeout = 1000);
    int sendCmd(const __FlashStringHelper *cmd, int timeout, ...);

    bool sendCmdGet(const __FlashStringHelper *cmd, const char *startTag, const char *endTag, char *outStr, int outStrLen);
    bool sendCmdGet(const __FlashStringHelper *cmd, const __FlashStringHelper *startTag, const __FlashStringHelper *endTag, char *outStr, int outStrLen);

    int readUntil(int timeout, const char *tag = NULL, bool findTags = true);

	void emptyBuf(bool warn=true);

private:
    Stream *_serial;

    // the ring buffer is used to search the tags in the stream
    ProMake_RingBuffer ringBuf;
};

#endif
