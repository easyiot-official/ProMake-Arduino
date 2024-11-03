#ifndef PROMAKE_M66_MODEM_H_INCLUDED
#define PROMAKE_M66_MODEM_H_INCLUDED

#include <HardwareSerial.h>
#include "AT_Command/ProMake_CircularBuffer.h"
#include "ProMake_GSM_ProviderBase.h"
#include "ProMake_M66_AccessProvider.h"
#include <Print.h>

#define UMPROVIDERS 3

class ProMake_GSM_Modem : public ProMake_CircularBufferManager, public Print
{
private:
    ProMake_CircularBuffer cb;
    HardwareSerial &m_serial;

    // 0 = ongoing
    // 1 = OK
    // 2 = Error. Incorrect state
    // 3 = Unexpected modem message
    // 4 = OK but not available data.
    ProMake_GSM_CommandError_t _commandError;

    // Counts the steps by the command
    uint8_t _commandCounter;

    // Presently ongoing command
    ProMake_GSM_commandType_e _ongoingCommand;

    // This is the modem (known) status
    ProMake_GSM_NetworkStatus_t _status;

    ProMakeGsmProviderBase *_UMProvider[UMPROVIDERS];
    ProMakeGsmProviderBase *_activeProvider;

    unsigned long _milliseconds;

public:
    /** Constructor **/
    ProMake_GSM_Modem(HardwareSerial &serial, bool debug = false);

    /** Get command error
        @return command error
     */
    ProMake_GSM_CommandError_t getCommandError() { return _commandError; };

    /** Establish a command error
        @param n			Command error
     */
    void setCommandError(ProMake_GSM_CommandError_t n) { _commandError = n; };

    /** Get command counter
        @return command counter
     */
    uint8_t getCommandCounter() { return _commandCounter; };

    /** Set command counter
        @param c			Initial value
     */
    void setCommandCounter(uint8_t c) { _commandCounter = c; };

    /** Get ongoing command
        @return command
     */
    ProMake_GSM_commandType_e getOngoingCommand() { return _ongoingCommand; };

    /** Set ongoing command
        @param c			 New ongoing command
     */
    void setOngoingCommand(ProMake_GSM_commandType_e c) { _ongoingCommand = c; };

    /** Open command
      @param activeProvider Active provider
      @param c        Command for open
     */
    void openCommand(ProMakeGsmProviderBase *activeProvider, ProMake_GSM_commandType_e c);

    /** Close command
      @param code     Close code
     */
    void closeCommand(ProMake_GSM_CommandError_t code);

    /** Generic response parser
        @param rsp			Returns true if expected response exists
        @param string		Substring expected in response
        @param string2		Second substring expected in response
        @return true if parsed correctly
     */
    bool genericParse_rsp(bool &rsp, const char *string = 0, const char *string2 = 0);

    /** Generates a generic AT command request from a simple char buffer
        @param str			Buffer with AT command
        @param addCR		Carriadge return adding automatically
     */
    void genericCommand_rqc(const char *str, bool addCR = true);

    /** Returns the circular buffer
        @return circular buffer
     */
    inline ProMake_CircularBuffer &theBuffer() { return cb; };

    /** Establish a new network status
        @param status		Network status
     */
    inline void setStatus(ProMake_GSM_NetworkStatus_t status) { _status = status; };

    /** Returns actual network status
        @return network status
     */
    inline ProMake_GSM_NetworkStatus_t getStatus() { return _status; };

    /** Register provider as willing to receive unsolicited messages
        @param provider	Pointer to provider able to receive unsolicited messages
    */
    void registerUMProvider(ProMakeGsmProviderBase *provider);

    /** unegister provider as willing to receive unsolicited messages
        @param provider	Pointer to provider able to receive unsolicited messages
    */
    void unRegisterUMProvider(ProMakeGsmProviderBase *provider);

    /** Register a provider as "dialoguing" talking in facto with the modem
        @param provider	Pointer to provider receiving responses
    */
    void registerActiveProvider(ProMakeGsmProviderBase *provider) { _activeProvider = provider; };

    /** Write a character in serial
        @param c			Character
        @return size
     */
    size_t write(uint8_t c);

    /** Chronometer. Measure milliseconds from last call
      @return milliseconds from las time function was called
    */
    unsigned long takeMilliseconds();

    void recv();

private:
    void parseResp();
    void spaceAvailable(){};

    /** Generic response parser
      @param rsp      Returns true if expected response exists
      @param string   Substring expected in response
      @param string2    Second substring expected in response
      @return true if parsed correctly
     */
    // bool recognizeUnsolicitedEvent(byte oldTail);

private:
    /** Continue to modem configuration function
     */
    void ModemConfigurationContinue();
};

#endif
