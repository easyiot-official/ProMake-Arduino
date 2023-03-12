#ifndef _PROMAKE_GSM_BASEPROVIDER_
#define _PROMAKE_GSM_BASEPROVIDER_

#include <inttypes.h>
#include <stddef.h>

#ifndef byte
#define byte uint8_t
#endif

enum ProMake_GSM_commandType_e
{
    XON,
    NONE,
    MODEMCONFIG,
    ALIVETEST,
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
    GETICCID
};
class ProMake_M66_Modem;

class ProMakeGsmProviderBase
{
protected:
    ProMake_M66_Modem *_theProMakeM66Modem;

public:
    ProMakeGsmProviderBase(ProMake_M66_Modem *Modem);
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
