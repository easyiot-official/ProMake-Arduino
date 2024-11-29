#ifndef PROMAKE_GSM_BASEPROVIDER_H_INCLUDED
#define PROMAKE_GSM_BASEPROVIDER_H_INCLUDED

#include <inttypes.h>
#include <stddef.h>
#include <Arduino.h>

#ifndef byte
#define byte uint8_t
#endif

enum ProMake_GSM_commandType_e
{
    XON,
    NONE,
    MODEMCONFIG,
    ALIVETEST,
    SIGNALQUALITY,
    BEGINSMS,
    ENDSMS,
    AVAILABLESMS,
    FLUSHSMS,
    VOICECALL,
    ANSWERCALL,
    HANGCALL,
    RETRIEVECALLINGNUMBER,
    ATTACHGPRS,
    DETACHGPRS,
    CONNECTTCPCLIENT,
    DISCONNECTTCP,
    CONNECTMQTTCLIENT,
    DISCONNECTMQTT,
    MQTTPub,
    MQTTSub,
    BEGINWRITESOCKET,
    ENDWRITESOCKET,
    AVAILABLESOCKET,
    FLUSHSOCKET,
    CONNECTSERVER,
    GETIP,
    GETCONNECTSTATUS,
    GETLOCATION,
    GETICCID,
    GETIMEI
};

enum ProMake_GSM_CommandError_t
{
    CMD_ONGOING,
    CMD_OK,
    CMD_ERROR,
    CMD_UNEXP,
    CMD_OK_NO_DATA
};

class ProMake_GSM_Modem;

class ProMakeGsmProviderBase
{
protected:
    ProMake_GSM_Modem *_theProMakeGsmModem;

public:
    ProMakeGsmProviderBase(ProMake_GSM_Modem *Modem);
    /** Get last command status
        @return Returns 0 if last command is still executing, 1 success, >1 error
    */
    int ready();

    /** Manages modem response
        @param from 		Initial byte of buffer
        @param to 			Final byte of buffer
    */
    virtual void manageResponse(byte from, byte to) = 0;

    /** Recognize URC
        @param from
        @return true if successful (default: false)
    */
    virtual bool recognizeUnsolicitedEvent(byte from) { return false; };
};

#endif
