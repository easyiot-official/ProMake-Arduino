#include "ProMake_M66_MqttClientProvider.h"
#include "ProMake_M66_Modem.h"

#define __TOUTMQTT__ 5000

ProMakeM66MqttClientProvider::ProMakeM66MqttClientProvider(ProMake_M66_Modem *Modem, MQTT_CALLBACK_SIGNATURE) : ProMakeGsmProviderBase(Modem)
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
    _theProMakeM66Modem->openCommand(this, CONNECTMQTTCLIENT);
    _theProMakeM66Modem->registerUMProvider(this);
    connectMQTTClientContinue();
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

    switch (_theProMakeM66Modem->getCommandCounter())
    {
    case 1:
        _theProMakeM66Modem->genericCommand_rqc("AT+QMTCFG=\"WILL\",0,", false);
        _theProMakeM66Modem->print(_willFlag);
        _theProMakeM66Modem->print(',');
        _theProMakeM66Modem->print(_willQoS);
        _theProMakeM66Modem->print(',');
        _theProMakeM66Modem->print(_willRetain);
        _theProMakeM66Modem->print(',');
        _theProMakeM66Modem->print('"');
        _theProMakeM66Modem->print(_willTopic);
        _theProMakeM66Modem->print('"');
        _theProMakeM66Modem->print(',');
        _theProMakeM66Modem->print('"');
        _theProMakeM66Modem->print(_willPayload);
        _theProMakeM66Modem->print('"');
        _theProMakeM66Modem->print('\r');

        _theProMakeM66Modem->setCommandCounter(2);
        break;
    case 2:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeM66Modem->genericCommand_rqc("AT+QMTCFG=\"TIMEOUT\",0,50,5,0", true);
                _theProMakeM66Modem->setCommandCounter(3);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    case 3:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeM66Modem->genericCommand_rqc("AT+QMTCFG=\"SESSION\",0,1", true);
                _theProMakeM66Modem->setCommandCounter(4);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    case 4:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeM66Modem->genericCommand_rqc("AT+QMTCFG=\"KEEPALIVE\",0,", false);
                _theProMakeM66Modem->print(_keepAliveTime);
                _theProMakeM66Modem->print('\r');
                _theProMakeM66Modem->setCommandCounter(5);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    case 5:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeM66Modem->genericCommand_rqc("AT+QMTCFG=\"VERSION\",0,1", true);
                _theProMakeM66Modem->setCommandCounter(6);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    case 6:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeM66Modem->genericCommand_rqc("AT+QMTCFG=\"SSL\",0,0", true);
                _theProMakeM66Modem->setCommandCounter(7);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    case 7:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                // AT+QMTOPEN
                _theProMakeM66Modem->genericCommand_rqc("AT+QMTOPEN=0,\"", false);
                _theProMakeM66Modem->print(_remoteServer);
                _theProMakeM66Modem->print('"');
                _theProMakeM66Modem->print(',');
                _theProMakeM66Modem->print(_remotePort);
                _theProMakeM66Modem->print('\r');
                _theProMakeM66Modem->setCommandCounter(8);
                _theProMakeM66Modem->takeMilliseconds();
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    case 8:
        if (_theProMakeM66Modem->genericParse_rsp(resp, "+QMTOPEN: 0,0"))
        {
            // Response received
            if (resp)
            {
                _theProMakeM66Modem->genericCommand_rqc("AT+QMTCONN=0,\"", false);
                _theProMakeM66Modem->print(_clientId);
                _theProMakeM66Modem->print('"');
                if (_username != 0)
                {
                    _theProMakeM66Modem->print(',');
                    _theProMakeM66Modem->print('"');
                    _theProMakeM66Modem->print(_username);
                    _theProMakeM66Modem->print('"');
                }
                if (_password != 0)
                {
                    _theProMakeM66Modem->print(',');
                    _theProMakeM66Modem->print('"');
                    _theProMakeM66Modem->print(_password);
                    _theProMakeM66Modem->print('"');
                }
                _theProMakeM66Modem->print('\r');

                _theProMakeM66Modem->setCommandCounter(9);
                _theProMakeM66Modem->takeMilliseconds();
            }
            else
            {
                if (_theProMakeM66Modem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeM66Modem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    case 9:
        if (_theProMakeM66Modem->genericParse_rsp(resp, "+QMTCONN: 0,0"))
        {
            // Response received
            if (resp)
            {
                // Great. We're done
                _theProMakeM66Modem->setStatus(NET_STATUS_MQTT_CONNECTED);
                //_theProMakeM66Modem->theBuffer().chopUntil(auxLocate, true);
                _theProMakeM66Modem->closeCommand(CMD_OK);
            }
            else
            {
                if (_theProMakeM66Modem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeM66Modem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    }
}

// Disconnect TCP main function.
ProMake_GSM_CommandError_t ProMakeM66MqttClientProvider::disconnectMqtt(bool synchronous)
{
    _theProMakeM66Modem->openCommand(this, DISCONNECTMQTT);
    _theProMakeM66Modem->unRegisterUMProvider(this);
    disconnectMqttContinue();
    // If synchronous, wait till attach is over, or not.
    if (synchronous)
    {
        // if we shorten this delay, the command fails
        unsigned long timeOut = millis();
        while (((millis() - timeOut) < 10000) & (ready() == 0))
            delay(100);
    }
    return _theProMakeM66Modem->getCommandError();
}

void ProMakeM66MqttClientProvider::disconnectMqttContinue()
{
    bool resp;
    // 0: Disconnect
    // 1: close

    switch (_theProMakeM66Modem->getCommandCounter())
    {
    case 1:
        _theProMakeM66Modem->genericCommand_rqc("AT+QMTDISC=0");
        _theProMakeM66Modem->setCommandCounter(2);
        break;
    case 2:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                _theProMakeM66Modem->genericCommand_rqc("AT+QMTCLOSE=0");
                _theProMakeM66Modem->setCommandCounter(3);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    case 3:
        if (_theProMakeM66Modem->genericParse_rsp(resp))
        {
            // Response received
            if (resp)
            {
                // Great. We're done
                _theProMakeM66Modem->setStatus(NET_STATUS_GPRS_READY);
                _theProMakeM66Modem->closeCommand(CMD_OK);
            }
            else
                _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        break;
    }
}

ProMake_GSM_CommandError_t ProMakeM66MqttClientProvider::publish(const char *topic, const char *payload, int qos, int retain, bool synchronous)
{
    if (_theProMakeM66Modem->getStatus() == NET_STATUS_MQTT_CONNECTED)
    {
        _pubTopic = topic;
        _pubPayload = payload;
        _pubQos = qos;
        _pubRetain = retain;
        _theProMakeM66Modem->openCommand(this, MQTTPub);
        publishContinue();
        // If synchronous, wait till attach is over, or not.
        if (synchronous)
        {
            // if we shorten this delay, the command fails
            unsigned long timeOut = millis();
            while (((millis() - timeOut) < 5000) & (ready() == 0))
                delay(100);
        }
        return _theProMakeM66Modem->getCommandError();
    }
    return CMD_ERROR;
}

void ProMakeM66MqttClientProvider::publishContinue()
{
    bool resp;

    switch (_theProMakeM66Modem->getCommandCounter())
    {
    case 1:
        _theProMakeM66Modem->genericCommand_rqc("AT+QMTPUB=0,0,", false);
        _theProMakeM66Modem->print(_pubQos);
        _theProMakeM66Modem->print(',');
        _theProMakeM66Modem->print(_pubRetain);
        _theProMakeM66Modem->print(',');
        _theProMakeM66Modem->print('"');
        _theProMakeM66Modem->print(_pubTopic);
        _theProMakeM66Modem->print('"');
        _theProMakeM66Modem->print('\r');
        _theProMakeM66Modem->setCommandCounter(2);
        _theProMakeM66Modem->takeMilliseconds();
        break;
    case 2:
        if (_theProMakeM66Modem->genericParse_rsp(resp, ">"))
        {
            // Response received
            if (resp)
            {
                _theProMakeM66Modem->print(_pubPayload);
                _theProMakeM66Modem->write(26);
                _theProMakeM66Modem->print('\r');
                _theProMakeM66Modem->setCommandCounter(3);
                _theProMakeM66Modem->takeMilliseconds();
            }
            else
            {
                if (_theProMakeM66Modem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeM66Modem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    case 3:
        if (_theProMakeM66Modem->genericParse_rsp(resp, "+QMTPUB: 0,0"))
        {
            // Response received
            if (resp)
            {
                // Great. We're done
                _theProMakeM66Modem->closeCommand(CMD_OK);
            }
            else
            {
                if (_theProMakeM66Modem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeM66Modem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    }
}

ProMake_GSM_CommandError_t ProMakeM66MqttClientProvider::subscribe(const char *topic, int qos, bool synchronous)
{
    if (_theProMakeM66Modem->getStatus() == NET_STATUS_MQTT_CONNECTED)
    {
        _subTopic = topic;
        _subQos = qos;
        _theProMakeM66Modem->openCommand(this, MQTTSub);
        subscribeContinue();
        // If synchronous, wait till attach is over, or not.
        if (synchronous)
        {
            // if we shorten this delay, the command fails
            unsigned long timeOut = millis();
            while (((millis() - timeOut) < 5000) & (ready() == 0))
                delay(100);
        }
        return _theProMakeM66Modem->getCommandError();
    }
    return CMD_ERROR;
}

void ProMakeM66MqttClientProvider::subscribeContinue()
{
    bool resp;

    switch (_theProMakeM66Modem->getCommandCounter())
    {
    case 1:
        _theProMakeM66Modem->genericCommand_rqc("AT+QMTSUB=0,1,", false);
        _theProMakeM66Modem->print('"');
        _theProMakeM66Modem->print(_subTopic);
        _theProMakeM66Modem->print('"');
        _theProMakeM66Modem->print(',');
        _theProMakeM66Modem->print(_subQos);
        _theProMakeM66Modem->print('\r');
        _theProMakeM66Modem->setCommandCounter(2);
        _theProMakeM66Modem->takeMilliseconds();
        break;
    case 2:
        if (_theProMakeM66Modem->genericParse_rsp(resp, "+QMTSUB: 0,1,0"))
        {
            // Response received
            if (resp)
            {
                // Great. We're done
                _theProMakeM66Modem->closeCommand(CMD_OK);
            }
            else
            {
                if (_theProMakeM66Modem->takeMilliseconds() > __TOUTMQTT__)
                {
                    _theProMakeM66Modem->closeCommand(CMD_UNEXP);
                }
            }
        }
        break;
    }
}

void ProMakeM66MqttClientProvider::manageResponse(byte from, byte to)
{
    switch (_theProMakeM66Modem->getOngoingCommand())
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
    if (_theProMakeM66Modem->theBuffer().locate("+QMTRECV:"))
    {
        // +QMTRECV: 0,0,SAMA/865006047165213,ID/101
        _theProMakeM66Modem->theBuffer().chopUntil("+QMTRECV: ", true);
        char c;
        char msg[50] = "";
        int i = 0;
        while (((c = _theProMakeM66Modem->theBuffer().read()) != 0) & (i < 50))
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

        //_theProMakeM66Modem->theBuffer().extractSubstring()

        return true;
    }
    if (_theProMakeM66Modem->theBuffer().locate("+QMTSTAT:"))
    {
        _theProMakeM66Modem->theBuffer().flush();
        return true;
    }
    /*
    if (_theProMakeM66Modem->theBuffer().locate("+QMTOPEN:"))
    {
        _theProMakeM66Modem->theBuffer().flush();
        return true;
    }
    if (_theProMakeM66Modem->theBuffer().locate("+QMTCONN:"))
    {
        _theProMakeM66Modem->theBuffer().flush();
        return true;
    }
    if (_theProMakeM66Modem->theBuffer().locate("+QMTSUB:"))
    {
        _theProMakeM66Modem->theBuffer().flush();
        return true;
    }
    if (_theProMakeM66Modem->theBuffer().locate("+QMTPUB:"))
    {
        _theProMakeM66Modem->theBuffer().flush();
        return true;
    }
    if (_theProMakeM66Modem->theBuffer().locate("+QMTDISC:"))
    {
        _theProMakeM66Modem->theBuffer().flush();
        return true;
    }
    if (_theProMakeM66Modem->theBuffer().locate("+QMTCLOSE:"))
    {
        _theProMakeM66Modem->theBuffer().flush();
        return true;
    }
    */
    return false;
}
