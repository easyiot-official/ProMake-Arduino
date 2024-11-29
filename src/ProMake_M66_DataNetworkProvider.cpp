#include "ProMake_M66_DataNetworkProvider.h"
#include "ProMake_GSM_Modem.h"
#include <ProMake_debug.h>

const char _command_CGATT[] = "AT+CGATT=";
const char _command_SEPARATOR[] = "\",\"";

#define __TOUTGPRS__ 5000
ProMakeM66DataNetworkProvider::ProMakeM66DataNetworkProvider(ProMake_GSM_Modem *Modem) : ProMakeGsmProviderBase(Modem)
{
    _theProMakeGsmModem->registerUMProvider(this);
}
// Attach GPRS main function.
ProMake_GSM_NetworkStatus_t ProMakeM66DataNetworkProvider::attachGPRS(char *apn, char *user_name, char *password, bool synchronous)
{
    _apn = apn;
    _user = user_name;
    _passwd = password;

    _theProMakeGsmModem->openCommand(this, ATTACHGPRS);
    _theProMakeGsmModem->setStatus(NET_STATUS_CONNECTING);

    attachGPRSContinue();

    // If synchronous, wait till attach is over, or not.
    if (synchronous)
    {
        // if we shorten this delay, the command fails
        unsigned long timeOut = millis();
        while (((millis() - timeOut) < 20000) & (ready() == 0))
            delay(100);
    }

    return _theProMakeGsmModem->getStatus();
}

// Atthach GPRS continue function.
void ProMakeM66DataNetworkProvider::attachGPRSContinue()
{
    bool resp;
    // 1: Attach to GPRS service "AT+CGATT=1"
    // 2: Wait attach OK and Set the context 0 as FGCNT "AT+QIFGCNT=0"
    // 3: Wait context OK and Set bearer type as GPRS, APN, user name and pasword "AT+QICSGP=1..."
    // 4: Wait bearer OK and Enable the function of MUXIP "AT+QIMUX=1"
    // 5: Wait for disable MUXIP OK and Set the session mode as non transparent "AT+QIMODE=0"
    // 6: Wait for session mode OK and Enable notification when data received "AT+QINDI=1"
    // 8: Wait domain name OK and Register the TCP/IP stack "AT+QIREGAPP"
    // 9: Wait for Register OK and Activate FGCNT "AT+QIACT"
    // 10: Wait for activate OK

    int ct = _theProMakeGsmModem->getCommandCounter();
    if (ct == 1)
    {
        // AT+CGATT
        _theProMakeGsmModem->genericCommand_rqc(_command_CGATT, false);
        _theProMakeGsmModem->print(1);
        _theProMakeGsmModem->print('\r');
        _theProMakeGsmModem->setCommandCounter(2);
        _theProMakeGsmModem->takeMilliseconds();
    }
    else if (ct == 2)
    {
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIFGCNT
                _theProMakeGsmModem->genericCommand_rqc("AT+QIFGCNT=0");
                _theProMakeGsmModem->setCommandCounter(3);
            }
            else
            {
                if (_theProMakeGsmModem->takeMilliseconds() > __TOUTGPRS__)
                {
                    _theProMakeGsmModem->closeCommand(CMD_UNEXP);
                }
            }
        }
    }
    else if (ct == 3)
    {
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // Great. Go for the next step
                PROMAKE_LOGDEBUG("AT+QICSGP.");
                _theProMakeGsmModem->genericCommand_rqc("AT+QICSGP=1,\"", false);
                _theProMakeGsmModem->print(_apn);
                _theProMakeGsmModem->genericCommand_rqc(_command_SEPARATOR, false);
                _theProMakeGsmModem->print(_user);
                _theProMakeGsmModem->genericCommand_rqc(_command_SEPARATOR, false);
                _theProMakeGsmModem->print(_passwd);
                _theProMakeGsmModem->print("\"\r");
                _theProMakeGsmModem->setCommandCounter(4);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 4)
    {
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIMUX=1 for multisocket
                _theProMakeGsmModem->genericCommand_rqc("AT+QIMUX=0");
                _theProMakeGsmModem->setCommandCounter(5);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 5)
    {
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIMODE=0 for multisocket
                _theProMakeGsmModem->genericCommand_rqc("AT+QIMODE=1");
                _theProMakeGsmModem->setCommandCounter(6);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 6)
    {
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QINDI=1
                _theProMakeGsmModem->genericCommand_rqc("AT+QINDI=1");
                _theProMakeGsmModem->setCommandCounter(8);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 8)
    {
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIREGAPP
                _theProMakeGsmModem->genericCommand_rqc("AT+QIREGAPP");
                _theProMakeGsmModem->setCommandCounter(9);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 9)
    {
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIACT
                _theProMakeGsmModem->genericCommand_rqc("AT+QIACT");
                _theProMakeGsmModem->setCommandCounter(10);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 10)
    {
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            if (resp)
            {
                _theProMakeGsmModem->setStatus(NET_STATUS_GPRS_READY);
                _theProMakeGsmModem->closeCommand(CMD_OK);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
    }
}

// Detach GPRS main function.
ProMake_GSM_NetworkStatus_t ProMakeM66DataNetworkProvider::detachGPRS(bool synchronous)
{
    _theProMakeGsmModem->openCommand(this, DETACHGPRS);
    _theProMakeGsmModem->setStatus(NET_STATUS_CONNECTING);
    detachGPRSContinue();

    if (synchronous)
    {
        unsigned long timeOut = millis();
        while (((millis() - timeOut) < 20000) & (ready() == 0))
            delay(100);
    }

    return _theProMakeGsmModem->getStatus();
}

void ProMakeM66DataNetworkProvider::detachGPRSContinue()
{
    bool resp;
    // 1: Detach to GPRS service "AT+CGATT=0"
    // 2: Wait dettach +PDP DEACT
    // 3: Wait for OK

    switch (_theProMakeGsmModem->getCommandCounter())
    {
    case 1:
        // AT+CGATT=0
        _theProMakeGsmModem->genericCommand_rqc(_command_CGATT, false);
        _theProMakeGsmModem->print(0);
        _theProMakeGsmModem->print('\r');
        _theProMakeGsmModem->setCommandCounter(2);
        break;
    case 2:
        if (_theProMakeGsmModem->theBuffer().locate("+PDP DEACT"))
        {
            if (resp)
            {
                // Received +PDP DEACT;
                _theProMakeGsmModem->setCommandCounter(3);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    case 3:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // OK received
            if (resp)
            {
                _theProMakeGsmModem->setStatus(NET_STATUS_GSM_READY);
                _theProMakeGsmModem->closeCommand(CMD_OK);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        _theProMakeGsmModem->theBuffer().flush();
        break;
    }
}

int ProMakeM66DataNetworkProvider::getLocation(char *country, char *network, char *area, char *cell)
{
    if ((_theProMakeGsmModem->getStatus() != NET_STATUS_GSM_READY) && (_theProMakeGsmModem->getStatus() != NET_STATUS_GPRS_READY))
        return 2;

    countryCode = country;
    networkCode = network;
    locationArea = area;
    cellId = cell;

    _theProMakeGsmModem->openCommand(this, GETLOCATION);
    getLocationContinue();

    unsigned long timeOut = millis();
    while (((millis() - timeOut) < 5000) & (ready() == 0))
        ;

    return _theProMakeGsmModem->getCommandError();
}

void ProMakeM66DataNetworkProvider::getLocationContinue()
{
    bool resp;

    switch (_theProMakeGsmModem->getCommandCounter())
    {
    case 1:
        delay(3000);
        _theProMakeGsmModem->setCommandCounter(2);
        _theProMakeGsmModem->genericCommand_rqc("AT+QENG=1", false);
        _theProMakeGsmModem->print("\r");
        break;
    case 2:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            delay(3000);
            _theProMakeGsmModem->setCommandCounter(3);
            _theProMakeGsmModem->genericCommand_rqc("AT+QENG?", false);
            _theProMakeGsmModem->print("\r");
        }
        else
            _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        break;
    case 3:
        if (resp)
        {
            parseQENG_available(resp);
            _theProMakeGsmModem->closeCommand(CMD_OK);
        }
        else
            _theProMakeGsmModem->closeCommand(CMD_ERROR);
        break;
    }
}

int ProMakeM66DataNetworkProvider::getICCID(char *iccid)
{
    if ((_theProMakeGsmModem->getStatus() != NET_STATUS_GSM_READY) && (_theProMakeGsmModem->getStatus() != NET_STATUS_GPRS_READY))
        return 2;

    bufferICCID = iccid;
    _theProMakeGsmModem->openCommand(this, GETICCID);
    getICCIDContinue();

    unsigned long timeOut = millis();
    while (((millis() - timeOut) < 5000) & (ready() == 0))
        ;

    return _theProMakeGsmModem->getCommandError();
}

void ProMakeM66DataNetworkProvider::getICCIDContinue()
{
    bool resp;

    switch (_theProMakeGsmModem->getCommandCounter())
    {
    case 1:
        _theProMakeGsmModem->setCommandCounter(2);
        _theProMakeGsmModem->genericCommand_rqc("AT+QCCID");
        break;
    case 2:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            parseQCCID_available(resp);
            _theProMakeGsmModem->closeCommand(CMD_OK);
        }
        else
            _theProMakeGsmModem->closeCommand(CMD_ERROR);
        break;
    }
}

int ProMakeM66DataNetworkProvider::getIMEI(char *imei)
{
    bufferIMEI = imei;
    _theProMakeGsmModem->openCommand(this, GETIMEI);
    getIMEIContinue();

    unsigned long timeOut = millis();
    while (((millis() - timeOut) < 5000) & (ready() == 0))
        ;

    return _theProMakeGsmModem->getCommandError();
}

void ProMakeM66DataNetworkProvider::getIMEIContinue()
{
    bool resp;

    switch (_theProMakeGsmModem->getCommandCounter())
    {
    case 1:
        _theProMakeGsmModem->setCommandCounter(2);
        _theProMakeGsmModem->genericCommand_rqc("AT+GSN");
        break;
    case 2:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            parseGSN_available(resp);
            _theProMakeGsmModem->closeCommand(CMD_OK);
        }
        else
            _theProMakeGsmModem->closeCommand(CMD_ERROR);
        break;
    }
}

bool ProMakeM66DataNetworkProvider::parseGSN_available(bool &rsp)
{
    char c;
    bool imeiFound = false;
    int i = 0;

    while (((c = _theProMakeGsmModem->theBuffer().read()) != 0) & (i < 15))
    {
        if ((c < 58) & (c > 47))
            imeiFound = true;

        if (imeiFound)
        {
            bufferIMEI[i] = c;
            i++;
        }
    }
    bufferIMEI[i] = 0;

    return true;
}

bool ProMakeM66DataNetworkProvider::parseQCCID_available(bool &rsp)
{
    char c;
    bool iccidFound = false;
    int i = 0;

    while (((c = _theProMakeGsmModem->theBuffer().read()) != 0) & (i < 20))
    {
        if ((c < 58) & (c > 47))
            iccidFound = true;

        if (iccidFound)
        {
            bufferICCID[i] = c;
            i++;
        }
    }
    bufferICCID[i] = 0;

    return true;
}

bool ProMakeM66DataNetworkProvider::parseQENG_available(bool &rsp)
{
    char c;
    char location[50] = "";
    int i = 0;

    if (!(_theProMakeGsmModem->theBuffer().chopUntil("+QENG: ", true)))
        rsp = false;
    else
        rsp = true;

    if (!(_theProMakeGsmModem->theBuffer().chopUntil("+QENG:", true)))
        rsp = false;
    else
        rsp = true;

    while (((c = _theProMakeGsmModem->theBuffer().read()) != 0) & (i < 50))
    {
        location[i] = c;
        i++;
    }
    location[i] = 0;

    char *res_tok = strtok(location, ",");
    res_tok = strtok(NULL, ",");
    strcpy(countryCode, res_tok);
    res_tok = strtok(NULL, ",");
    strcpy(networkCode, res_tok);
    res_tok = strtok(NULL, ",");
    strcpy(locationArea, res_tok);
    res_tok = strtok(NULL, ",");
    strcpy(cellId, res_tok);

    return true;
}

// Response management.
void ProMakeM66DataNetworkProvider::manageResponse(byte from, byte to)
{
    switch (_theProMakeGsmModem->getOngoingCommand())
    {
    case ATTACHGPRS:
        attachGPRSContinue();
        break;
    case DETACHGPRS:
        detachGPRSContinue();
        break;
    case GETICCID:
        getICCIDContinue();
        break;
    case GETIMEI:
        getIMEIContinue();
        break;
        /*
        case GETIP:
            getIPContinue();
            break;
        */
    }
}

ProMake_GSM_NetworkStatus_t ProMakeM66DataNetworkProvider::getStatus() { return _theProMakeGsmModem->getStatus(); };

///////////////////////////////////////////////////////UNSOLICITED RESULT CODE (URC) FUNCTIONS///////////////////////////////////////////////////////////////////

// URC recognize.
bool ProMakeM66DataNetworkProvider::recognizeUnsolicitedEvent(byte oldTail)
{

    if (_theProMakeGsmModem->theBuffer().locate("+PDP DEACT"))
    {
        _theProMakeGsmModem->setStatus(NET_STATUS_GSM_READY);
        _theProMakeGsmModem->theBuffer().flush();
        return true;
    }

    return false;
}
