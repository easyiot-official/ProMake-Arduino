#ifndef _PROMAKE_M66_ACCESSPROVIDER_
#define _PROMAKE_M66_ACCESSPROVIDER_

#include "ProMake_GSM_ProviderBase.h"

enum ProMake_GSM_NetworkStatus_t
{
    ERROR,
    IDLE,
    CONNECTING,
    GSM_READY,
    GPRS_READY,
    TRANSPARENT_CONNECTED,
    OFF
};
class ProMake_M66_Modem;

class ProMakeM66AccessProvider : public ProMakeGsmProviderBase
{
private:
    int retryCnt;
    ProMake_M66_Modem *_theProMakeM66Modem;
    /** Initialize main modem configuration
        @param pin			PIN code
        @return command error if exists
     */
    int ModemConfiguration(char *pin);

    /** Continue to modem configuration function
     */
    void ModemConfigurationContinue();

    /** Continue to check if modem alive function
     */
    void isModemAliveContinue();

public:
    /** Constructor
        @param debug		Determines debug mode
     */

    ProMakeM66AccessProvider(ProMake_M66_Modem *Modem, bool debug = false);

    /** Start the GSM/GPRS modem, attaching to the GSM network
        @param pin 			SIM PIN number (4 digits in a string, example: "1234"). If
                            NULL the SIM has no configured PIN.
        @param restart		Restart the modem. Default is TRUE. The modem receives
                            a signal through the Ctrl/D7 pin. If it is shut down, it will
                            start-up. If it is running, it will restart. Takes up to 10
                            seconds
        @param synchronous	If TRUE the call only returns after the Start is complete
                            or fails. If FALSE the call will return inmediately. You have
                            to call repeatedly ready() until you get a result. Default is TRUE.
        @return If synchronous, GSM3_NetworkStatus_t. If asynchronous, returns 0.
    */
    ProMake_GSM_NetworkStatus_t begin(char *pin = 0, bool restart = true, bool synchronous = true);

    /** Check network access status
        @return 1 if Alive, 0 if down
    */
    int isAccessAlive();

    /** Shutdown the modem (power off really)
        @return true if successful
    */
    bool shutdown();

    /** Secure shutdown the modem (power off really)
        @return true if successful
     */
    bool secureShutdown();

    /** Returns 0 if last command is still executing
        @return 1 if success, >1 if error
    */
    int ready() { return ProMakeGsmProviderBase::ready(); };

    /** Returns modem status
        @return modem network status
    */
    inline ProMake_GSM_NetworkStatus_t getStatus();

    void manageResponse(byte from, byte to);

    /** Restart the modem (will shut down if running)
        @return 1 if success, >1 if error
    */
    int HWrestart();

    /** Start the modem (will not shut down if running)
        @return 1 if success, >1 if error
    */
    int HWstart();
};

#endif
