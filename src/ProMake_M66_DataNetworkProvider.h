#ifndef _PROMAKE_M66_DATANETWORKPROVIDER_
#define _PROMAKE_M66_DATANETWORKPROVIDER_

#include <ProMake_GSM_ProviderBase.h>
#include "ProMake_M66_AccessProvider.h"

#include <IPAddress.h>

class ProMakeM66DataNetworkProvider : public ProMakeGsmProviderBase
{
private:
    char *_apn;    // APN for GPRS
    char *_user;   // Username for GPRS
    char *_passwd; // Password for GPRS

    /** Continue to attach GPRS function
     */
    void attachGPRSContinue();

    /** Continue to detach GPRS function
     */
    void detachGPRSContinue();

    /** Parse QILOCIP response
        @param LocalIP			Buffer for save local IP address
        @param LocalIPlength	Buffer size
        @param rsp				Returns true if expected response exists
        @return true if command executed correctly
     */
    // bool parseQILOCIP_rsp(char *LocalIP, int LocalIPlength, bool &rsp);

    /** Continue to get IP function
     */
    // void getIPContinue();

    /** Implementation of inet_aton standard function
        @param aIPAddrString	IP address in characters buffer
        @param aResult			IP address in IPAddress format
        @return 1 if the address is successfully converted, or 0 if the conversion failed
     */
    // int inet_aton(const char *aIPAddrString, IPAddress &aResult);

    char *countryCode;
    char *networkCode;
    char *locationArea;
    char *cellId;

    char *bufferICCID;

    /** Continue to getLocation function
     */
    void getLocationContinue();

    /** Continue to getICCID function
     */
    void getICCIDContinue();

    bool parseQENG_available(bool &rsp);

    bool parseQCCID_available(bool &rsp);

public:
    /** Constructor
     */
    ProMakeM66DataNetworkProvider(ProMake_M66_Modem *Modem);

    /** Attach to GPRS/GSM network
        @param networkId 	APN GPRS
        @param user			Username
        @param pass			Password
        @return connection status
     */
    ProMake_GSM_NetworkStatus_t networkAttach(char *networkId, char *user, char *pass)
    {
        return attachGPRS(networkId, user, pass);
    };

    /** Detach GPRS/GSM network
        @return connection status
     */
    ProMake_GSM_NetworkStatus_t networkDetach() { return detachGPRS(); };

    /** Attach to GPRS service
        @param apn			APN GPRS
        @param user_name	Username
        @param password		Password
        @param synchronous	Sync mode
        @return connection status
     */
    ProMake_GSM_NetworkStatus_t attachGPRS(char *apn, char *user_name, char *password, bool synchronous = true);

    /** Detach GPRS service
        @param synchronous	Sync mode
        @return connection status
     */
    ProMake_GSM_NetworkStatus_t detachGPRS(bool synchronous = true);

    /** Returns 0 if last command is still executing
        @return 1 if success, >1 if error
     */
    int ready() { return ProMakeGsmProviderBase::ready(); };

    /** Get network status (connection)
        @return status
     */
    inline ProMake_GSM_NetworkStatus_t getStatus();

    /** Get actual assigned IP address
        @param LocalIP			Buffer for copy IP address
        @param LocalIPlength	Buffer length
        @return command error if exists
     */
    // int getIP(char *LocalIP, int LocalIPlength);

    /** Get actual assigned IP address in IPAddress format
        @return IP address in IPAddress format
     */
    // IPAddress getIPAddress();

    /** getLocation
     @return current cell location
    */
    int getLocation(char *country, char *network, char *area, char *cell);

    /** getICCID
     */
    int getICCID(char *iccid);

    /** Manages modem response
        @param from 		Initial byte of buffer
        @param to 			Final byte of buffer
     */
    void manageResponse(byte from, byte to);
};

#endif