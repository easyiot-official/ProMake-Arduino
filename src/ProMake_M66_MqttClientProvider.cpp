#include "ProMake_M66_MqttClientProvider.h"
#include "ProMake_GSM_Modem.h"

#define __TOUTMQTT__ 5000

ProMakeM66MqttClientProvider::ProMakeM66MqttClientProvider(ProMake_GSM_Modem *Modem, MQTT_CALLBACK_SIGNATURE) : ProMakeGsmProviderBase(Modem)
{
    setCallback(callback);
}

void ProMakeM66MqttClientProvider::setCallback(void (*callback)(char *, char *))
{
    this->callback = callback;
}

// Connect MQTT main function.
ProMake_GSM_NetworkStatus_t ProMakeM66MqttClientProvider::connectMqttClient(const char *clientId, const char *server, int port, const char *username, const char *password, int willFlag, const char *willTopic, const char *willPayload, int willQoS, int willRetain, int keepAliveTime, bool synchronous)
{
    _clientId = clientId;
    _remotePort = port;
    _remoteServer = server;
    _username = username;
    _password = password;
    _willFlag = willFlag;
    _willTopic = willTopic;
    _willPayload = willPayload;
    _willQoS = willQoS;
    _willRetain = willRetain;
    _keepAliveTime = keepAliveTime;
    _theProMakeGsmModem->openCommand(this, CONNECTMQTTCLIENT);
    _theProMakeGsmModem->registerUMProvider(this);
    connectMQTTClientContinue();
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

// Connect MQTT continue function.
void ProMakeM66MqttClientProvider::connectMQTTClientContinue()
{
    bool resp;
    // 0: Config Will
    // 1: Config Timeouts
    // 2: Config Session
    // 3: Config Keep alive
    // 4: Config Version
    // 5: Config SSL
    // 6: Open client
    // 7: Connecting

    switch (_theProMakeGsmModem->getCommandCounter())
    {
    case 1:
        _theProMakeGsmModem->genericCommand_rqc("AT+QMTCFG=\"WILL\",0,", false);
        _theProMakeGsmModem->print(_willFlag);
        _theProMakeGsmModem->print(',');
        _theProMakeGsmModem->print(_willQoS);
        _theProMakeGsmModem->print(',');
        _theProMakeGsmModem->print(_willRetain);
        _theProMakeGsmModem->print(',');
        _theProMakeGsmModem->print('"');
        _theProMakeGsmModem->print(_willTopic);
        _theProMakeGsmModem->print('"');
        _theProMakeGsmModem->print(',');
        _theProMakeGsmModem->print('"');
        _theProMakeGsmModem->print(_willPayload);
        _theProMakeGsmModem->print('"');
        _theProMakeGsmModem->print('\r');

        _theProMakeGsmModem->setCommandCounter(2);
        break;
    case 2:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeGsmModem->genericCommand_rqc("AT+QMTCFG=\"TIMEOUT\",0,50,5,0", true);
                _theProMakeGsmModem->setCommandCounter(3);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    case 3:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeGsmModem->genericCommand_rqc("AT+QMTCFG=\"SESSION\",0,1", true);
                _theProMakeGsmModem->setCommandCounter(4);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    case 4:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeGsmModem->genericCommand_rqc("AT+QMTCFG=\"KEEPALIVE\",0,", false);
                _theProMakeGsmModem->print(_keepAliveTime);
                _theProMakeGsmModem->print('\r');
                _theProMakeGsmModem->setCommandCounter(5);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    case 5:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeGsmModem->genericCommand_rqc("AT+QMTCFG=\"VERSION\",0,1", true);
                _theProMakeGsmModem->setCommandCounter(6);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    case 6:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeGsmModem->genericCommand_rqc("AT+QMTCFG=\"SSL\",0,0", true);
                _theProMakeGsmModem->setCommandCounter(7);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    case 7:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                // AT+QMTOPEN
                _theProMakeGsmModem->genericCommand_rqc("AT+QMTOPEN=0,\"", false);
                _theProMakeGsmModem->print(_remoteServer);
                _theProMakeGsmModem->print('"');
                _theProMakeGsmModem->print(',');
                _theProMakeGsmModem->print(_remotePort);
                _theProMakeGsmModem->print('\r');
                _theProMakeGsmModem->setCommandCounter(8);
                _theProMakeGsmModem->takeMilliseconds();
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    case 8:
        if (_theProMakeGsmModem->genericParse_rsp(resp, "+QMTOPEN: 0,0"))
        {
            // Response received
            if (resp)
            {
                _theProMakeGsmModem->genericCommand_rqc("AT+QMTCONN=0,\"", false);
                _theProMakeGsmModem->print(_clientId);
                _theProMakeGsmModem->print('"');
                if (_username != 0)
                {
                    _theProMakeGsmModem->print(',');
                    _theProMakeGsmModem->print('"');
                    _theProMakeGsmModem->print(_username);
                    _theProMakeGsmModem->print('"');
                }
                if (_password != 0)
                {
                    _theProMakeGsmModem->print(',');
                    _theProMakeGsmModem->print('"');
                    _theProMakeGsmModem->print(_password);
                    _theProMakeGsmModem->print('"');
                }
                _theProMakeGsmModem->print('\r');

                _theProMakeGsmModem->setCommandCounter(9);
                _theProMakeGsmModem->takeMilliseconds();
            }
            else
            {
                if (_theProMakeGsmModem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeGsmModem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    case 9:
        if (_theProMakeGsmModem->genericParse_rsp(resp, "+QMTCONN: 0,0"))
        {
            // Response received
            if (resp)
            {
                // Great. We're done
                _theProMakeGsmModem->setStatus(NET_STATUS_MQTT_CONNECTED);
                //_theProMakeGsmModem->theBuffer().chopUntil(auxLocate, true);
                _theProMakeGsmModem->closeCommand(CMD_OK);
            }
            else
            {
                if (_theProMakeGsmModem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeGsmModem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    }
}

// Disconnect TCP main function.
ProMake_GSM_CommandError_t ProMakeM66MqttClientProvider::disconnectMqtt(bool synchronous)
{
    _theProMakeGsmModem->openCommand(this, DISCONNECTMQTT);
    _theProMakeGsmModem->unRegisterUMProvider(this);
    disconnectMqttContinue();
    // If synchronous, wait till attach is over, or not.
    if (synchronous)
    {
        // if we shorten this delay, the command fails
        unsigned long timeOut = millis();
        while (((millis() - timeOut) < 10000) & (ready() == 0))
            delay(100);
    }
    return _theProMakeGsmModem->getCommandError();
}

void ProMakeM66MqttClientProvider::disconnectMqttContinue()
{
    bool resp;
    // 0: Disconnect
    // 1: close

    switch (_theProMakeGsmModem->getCommandCounter())
    {
    case 1:
        _theProMakeGsmModem->genericCommand_rqc("AT+QMTDISC=0");
        _theProMakeGsmModem->setCommandCounter(2);
        break;
    case 2:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeGsmModem->genericCommand_rqc("AT+QMTCLOSE=0");
                _theProMakeGsmModem->setCommandCounter(3);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    case 3:
        if (_theProMakeGsmModem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                // Great. We're done
                _theProMakeGsmModem->setStatus(NET_STATUS_GPRS_READY);
                _theProMakeGsmModem->closeCommand(CMD_OK);
            }
            else
                _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
        break;
    }
}

ProMake_GSM_CommandError_t ProMakeM66MqttClientProvider::publish(const char *topic, const char *payload, int qos, int retain, bool synchronous)
{
    if (_theProMakeGsmModem->getStatus() == NET_STATUS_MQTT_CONNECTED)
    {
        _pubTopic = topic;
        _pubPayload = payload;
        _pubQos = qos;
        _pubRetain = retain;
        _theProMakeGsmModem->openCommand(this, MQTTPub);
        publishContinue();
        // If synchronous, wait till attach is over, or not.
        if (synchronous)
        {
            // if we shorten this delay, the command fails
            unsigned long timeOut = millis();
            while (((millis() - timeOut) < 5000) & (ready() == 0))
                delay(100);
        }
        return _theProMakeGsmModem->getCommandError();
    }
    return CMD_ERROR;
}

void ProMakeM66MqttClientProvider::publishContinue()
{
    bool resp;

    switch (_theProMakeGsmModem->getCommandCounter())
    {
    case 1:
        _theProMakeGsmModem->genericCommand_rqc("AT+QMTPUB=0,0,", false);
        _theProMakeGsmModem->print(_pubQos);
        _theProMakeGsmModem->print(',');
        _theProMakeGsmModem->print(_pubRetain);
        _theProMakeGsmModem->print(',');
        _theProMakeGsmModem->print('"');
        _theProMakeGsmModem->print(_pubTopic);
        _theProMakeGsmModem->print('"');
        _theProMakeGsmModem->print('\r');
        _theProMakeGsmModem->setCommandCounter(2);
        _theProMakeGsmModem->takeMilliseconds();
        break;
    case 2:
        if (_theProMakeGsmModem->genericParse_rsp(resp, ">"))
        {
            // Response received
            if (resp)
            {
                _theProMakeGsmModem->print(_pubPayload);
                _theProMakeGsmModem->write(26);
                _theProMakeGsmModem->print('\r');
                _theProMakeGsmModem->setCommandCounter(3);
                _theProMakeGsmModem->takeMilliseconds();
            }
            else
            {
                if (_theProMakeGsmModem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeGsmModem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    case 3:
        if (_theProMakeGsmModem->genericParse_rsp(resp, "+QMTPUB: 0,0"))
        {
            // Response received
            if (resp)
            {
                // Great. We're done
                _theProMakeGsmModem->closeCommand(CMD_OK);
            }
            else
            {
                if (_theProMakeGsmModem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeGsmModem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    }
}

ProMake_GSM_CommandError_t ProMakeM66MqttClientProvider::subscribe(const char *topic, int qos, bool synchronous)
{
    if (_theProMakeGsmModem->getStatus() == NET_STATUS_MQTT_CONNECTED)
    {
        _subTopic = topic;
        _subQos = qos;
        _theProMakeGsmModem->openCommand(this, MQTTSub);
        subscribeContinue();
        // If synchronous, wait till attach is over, or not.
        if (synchronous)
        {
            // if we shorten this delay, the command fails
            unsigned long timeOut = millis();
            while (((millis() - timeOut) < 5000) & (ready() == 0))
                delay(100);
        }
        return _theProMakeGsmModem->getCommandError();
    }
    return CMD_ERROR;
}

void ProMakeM66MqttClientProvider::subscribeContinue()
{
    bool resp;

    switch (_theProMakeGsmModem->getCommandCounter())
    {
    case 1:
        _theProMakeGsmModem->genericCommand_rqc("AT+QMTSUB=0,1,", false);
        _theProMakeGsmModem->print('"');
        _theProMakeGsmModem->print(_subTopic);
        _theProMakeGsmModem->print('"');
        _theProMakeGsmModem->print(',');
        _theProMakeGsmModem->print(_subQos);
        _theProMakeGsmModem->print('\r');
        _theProMakeGsmModem->setCommandCounter(2);
        _theProMakeGsmModem->takeMilliseconds();
        break;
    case 2:
        if (_theProMakeGsmModem->genericParse_rsp(resp, "+QMTSUB: 0,1,0"))
        {
            // Response received
            if (resp)
            {
                // Great. We're done
                _theProMakeGsmModem->closeCommand(CMD_OK);
            }
            else
            {
                if (_theProMakeGsmModem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeGsmModem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    }
}

void ProMakeM66MqttClientProvider::manageResponse(byte from, byte to)
{
    switch (_theProMakeGsmModem->getOngoingCommand())
    {
    case CONNECTMQTTCLIENT:
        connectMQTTClientContinue();
        break;
    case DISCONNECTMQTT:
        disconnectMqttContinue();
        break;
    case MQTTPub:
        publishContinue();
        break;
    case MQTTSub:
        subscribeContinue();
        break;
    }
}

// URC recognize.
// Yes, we recognize "closes" in client mode
bool ProMakeM66MqttClientProvider::recognizeUnsolicitedEvent(byte oldTail)
{
    if (_theProMakeGsmModem->theBuffer().locate("+QMTRECV:"))
    {
        // +QMTRECV: 0,0,SAMA/865006047165213,ID/101
        _theProMakeGsmModem->theBuffer().chopUntil("+QMTRECV: ", true);
        char c;
        char msg[50] = "";
        int i = 0;
        while (((c = _theProMakeGsmModem->theBuffer().read()) != 0) & (i < 50))
        {
            msg[i] = c;
            i++;
        }
        msg[i] = 0;

        if (callback)
        {
            char *res_tok = strtok(msg, ","); //<TCP_connectID>
            res_tok = strtok(NULL, ",");      //<msgID>

            char topic[50] = "";
            res_tok = strtok(NULL, ","); //<topic>
            strcpy(topic, res_tok);

            char payload[50] = "";

            res_tok = strtok(NULL, ","); //<payload>
            strcpy(payload, res_tok);

            callback(topic, payload);
        }

        //_theProMakeGsmModem->theBuffer().extractSubstring()

        return true;
    }
    if (_theProMakeGsmModem->theBuffer().locate("+QMTSTAT:"))
    {
        _theProMakeGsmModem->theBuffer().flush();
        return true;
    }
    /*
    if (_theProMakeGsmModem->theBuffer().locate("+QMTOPEN:"))
    {
        _theProMakeGsmModem->theBuffer().flush();
        return true;
    }
    if (_theProMakeGsmModem->theBuffer().locate("+QMTCONN:"))
    {
        _theProMakeGsmModem->theBuffer().flush();
        return true;
    }
    if (_theProMakeGsmModem->theBuffer().locate("+QMTSUB:"))
    {
        _theProMakeGsmModem->theBuffer().flush();
        return true;
    }
    if (_theProMakeGsmModem->theBuffer().locate("+QMTPUB:"))
    {
        _theProMakeGsmModem->theBuffer().flush();
        return true;
    }
    if (_theProMakeGsmModem->theBuffer().locate("+QMTDISC:"))
    {
        _theProMakeGsmModem->theBuffer().flush();
        return true;
    }
    if (_theProMakeGsmModem->theBuffer().locate("+QMTCLOSE:"))
    {
        _theProMakeGsmModem->theBuffer().flush();
        return true;
    }
    */
    return false;
}
