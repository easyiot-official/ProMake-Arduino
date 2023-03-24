#include "ProMake_M66_DataNetworkProvider.h"
#include "ProMake_M66_Modem.h"

const char _command_CGATT[] = "AT+CGATT=";
const char _command_SEPARATOR[] = "\",\"";
ProMakeM66DataNetworkProvider::ProMakeM66DataNetworkProvider(ProMake_M66_Modem *Modem) : ProMakeGsmProviderBase(Modem)
{
}
// Attach GPRS main function.
ProMake_GSM_NetworkStatus_t ProMakeM66DataNetworkProvider::attachGPRS(char *apn, char *user_name, char *password, bool synchronous)
{
    _apn = apn;
    _user = user_name;
    _passwd = password;

    _theProMakeM66Modem->openCommand(this, ATTACHGPRS);
    _theProMakeM66Modem->setStatus(NET_STATUS_CONNECTING);

    attachGPRSContinue();

    // If synchronous, wait till attach is over, or not.
    if (synchronous)
    {
        // if we shorten this delay, the command fails
        unsigned long timeOut = millis();
        while (((millis() - timeOut) < 20000) & (ready() == 0))
            delay(100);
    }

    return _theProMakeM66Modem->getStatus();
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

    int ct = _theProMakeM66Modem->getCommandCounter();
    if (ct == 1)
    {
        // AT+CGATT
        _theProMakeM66Modem->genericCommand_rqc(_command_CGATT, false);
        _theProMakeM66Modem->print(1);
        _theProMakeM66Modem->print('\r');
        _theProMakeM66Modem->setCommandCounter(2);
    }
    else if (ct == 2)
    {
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIFGCNT
                _theProMakeM66Modem->genericCommand_rqc("AT+QIFGCNT=0");
                _theProMakeM66Modem->setCommandCounter(3);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 3)
    {
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // Great. Go for the next step
                // DEBUG
                // Serial.println("AT+QICSGP.");
                _theProMakeM66Modem->genericCommand_rqc("AT+QICSGP=1,\"", false);
                _theProMakeM66Modem->print(_apn);
                _theProMakeM66Modem->genericCommand_rqc(_command_SEPARATOR, false);
                _theProMakeM66Modem->print(_user);
                _theProMakeM66Modem->genericCommand_rqc(_command_SEPARATOR, false);
                _theProMakeM66Modem->print(_passwd);
                _theProMakeM66Modem->print("\"\r");
                _theProMakeM66Modem->setCommandCounter(4);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 4)
    {
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIMUX=1 for multisocket
                _theProMakeM66Modem->genericCommand_rqc("AT+QIMUX=0");
                _theProMakeM66Modem->setCommandCounter(5);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 5)
    {
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIMODE=0 for multisocket
                _theProMakeM66Modem->genericCommand_rqc("AT+QIMODE=1");
                _theProMakeM66Modem->setCommandCounter(6);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 6)
    {
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QINDI=1
                _theProMakeM66Modem->genericCommand_rqc("AT+QINDI=1");
                _theProMakeM66Modem->setCommandCounter(8);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 8)
    {
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIREGAPP
                _theProMakeM66Modem->genericCommand_rqc("AT+QIREGAPP");
                _theProMakeM66Modem->setCommandCounter(9);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 9)
    {
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            if (resp)
            {
                // AT+QIACT
                _theProMakeM66Modem->genericCommand_rqc("AT+QIACT");
                _theProMakeM66Modem->setCommandCounter(10);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
    }
    else if (ct == 10)
    {
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            if (resp)
            {
                _theProMakeM66Modem->setStatus(NET_STATUS_GPRS_READY);
                _theProMakeM66Modem->closeCommand(CMD_OK);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
    }
}

// Detach GPRS main function.
ProMake_GSM_NetworkStatus_t ProMakeM66DataNetworkProvider::detachGPRS(bool synchronous)
{
    _theProMakeM66Modem->openCommand(this, DETACHGPRS);
    _theProMakeM66Modem->setStatus(NET_STATUS_CONNECTING);
    detachGPRSContinue();

    if (synchronous)
    {
        unsigned long timeOut = millis();
        while (((millis() - timeOut) < 20000) & (ready() == 0))
            delay(100);
    }

    return _theProMakeM66Modem->getStatus();
}

void ProMakeM66DataNetworkProvider::detachGPRSContinue()
{
    bool resp;
    // 1: Detach to GPRS service "AT+CGATT=0"
    // 2: Wait dettach +PDP DEACT
    // 3: Wait for OK

    switch (_theProMakeM66Modem->getCommandCounter())
    {
    case 1:
        // AT+CGATT=0
        _theProMakeM66Modem->genericCommand_rqc(_command_CGATT, false);
        _theProMakeM66Modem->print(0);
        _theProMakeM66Modem->print('\r');
        _theProMakeM66Modem->setCommandCounter(2);
        break;
    case 2:
        if (_theProMakeM66Modem->theBuffer().locate("+PDP DEACT"))
        {
            if (resp)
            {
                // Received +PDP DEACT;
                _theProMakeM66Modem->setCommandCounter(3);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    case 3:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // OK received
            if (resp)
            {
                _theProMakeM66Modem->setStatus(NET_STATUS_GSM_READY);
                _theProMakeM66Modem->closeCommand(CMD_OK);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        _theProMakeM66Modem->theBuffer().flush();
        break;
    }
}

int ProMakeM66DataNetworkProvider::getLocation(char *country, char *network, char *area, char *cell)
{
    if ((_theProMakeM66Modem->getStatus() != NET_STATUS_GSM_READY) && (_theProMakeM66Modem->getStatus() != NET_STATUS_GPRS_READY))
        return 2;

    countryCode = country;
    networkCode = network;
    locationArea = area;
    cellId = cell;

    _theProMakeM66Modem->openCommand(this, GETLOCATION);
    getLocationContinue();

    unsigned long timeOut = millis();
    while (((millis() - timeOut) < 5000) & (ready() == 0))
        ;

    return _theProMakeM66Modem->getCommandError();
}

void ProMakeM66DataNetworkProvider::getLocationContinue()
{
    bool resp;

    switch (_theProMakeM66Modem->getCommandCounter())
    {
    case 1:
        delay(3000);
        _theProMakeM66Modem->setCommandCounter(2);
        _theProMakeM66Modem->genericCommand_rqc("AT+QENG=1", false);
        _theProMakeM66Modem->print("\r");
        break;
    case 2:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            delay(3000);
            _theProMakeM66Modem->setCommandCounter(3);
            _theProMakeM66Modem->genericCommand_rqc("AT+QENG?", false);
            _theProMakeM66Modem->print("\r");
        }
        else
            _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        break;
    case 3:
        if (resp)
        {
            parseQENG_available(resp);
            _theProMakeM66Modem->closeCommand(CMD_OK);
        }
        else
            _theProMakeM66Modem->closeCommand(CMD_ERROR);
        break;
    }
}

int ProMakeM66DataNetworkProvider::getICCID(char *iccid)
{
    if ((_theProMakeM66Modem->getStatus() != NET_STATUS_GSM_READY) && (_theProMakeM66Modem->getStatus() != NET_STATUS_GPRS_READY))
        return 2;

    bufferICCID = iccid;
    _theProMakeM66Modem->openCommand(this, GETICCID);
    getICCIDContinue();

    unsigned long timeOut = millis();
    while (((millis() - timeOut) < 5000) & (ready() == 0))
        ;

    return _theProMakeM66Modem->getCommandError();
}

void ProMakeM66DataNetworkProvider::getICCIDContinue()
{
    bool resp;

    switch (_theProMakeM66Modem->getCommandCounter())
    {
    case 1:
        _theProMakeM66Modem->setCommandCounter(2);
        _theProMakeM66Modem->genericCommand_rqc("AT+QCCID");
        break;
    case 2:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            parseQCCID_available(resp);
            _theProMakeM66Modem->closeCommand(CMD_OK);
        }
        else
            _theProMakeM66Modem->closeCommand(CMD_ERROR);
        break;
    }
}

int ProMakeM66DataNetworkProvider::getIMEI(char *imei)
{
    bufferIMEI = imei;
    _theProMakeM66Modem->openCommand(this, GETIMEI);
    getIMEIContinue();

    unsigned long timeOut = millis();
    while (((millis() - timeOut) < 5000) & (ready() == 0))
        ;

    return _theProMakeM66Modem->getCommandError();
}

void ProMakeM66DataNetworkProvider::getIMEIContinue()
{
    bool resp;

    switch (_theProMakeM66Modem->getCommandCounter())
    {
    case 1:
        _theProMakeM66Modem->setCommandCounter(2);
        _theProMakeM66Modem->genericCommand_rqc("AT+GSN");
        break;
    case 2:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            parseGSN_available(resp);
            _theProMakeM66Modem->closeCommand(CMD_OK);
        }
        else
            _theProMakeM66Modem->closeCommand(CMD_ERROR);
        break;
    }
}

bool ProMakeM66DataNetworkProvider::parseGSN_available(bool &rsp)
{
    char c;
    bool imeiFound = false;
    int i = 0;

    while (((c = _theProMakeM66Modem->theBuffer().read()) != 0) & (i < 15))
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

    while (((c = _theProMakeM66Modem->theBuffer().read()) != 0) & (i < 20))
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

    if (!(_theProMakeM66Modem->theBuffer().chopUntil("+QENG: ", true)))
        rsp = false;
    else
        rsp = true;

    if (!(_theProMakeM66Modem->theBuffer().chopUntil("+QENG:", true)))
        rsp = false;
    else
        rsp = true;

    while (((c = _theProMakeM66Modem->theBuffer().read()) != 0) & (i < 50))
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
    switch (_theProMakeM66Modem->getOngoingCommand())
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

ProMake_GSM_NetworkStatus_t ProMakeM66DataNetworkProvider::getStatus() { return _theProMakeM66Modem->getStatus(); };
