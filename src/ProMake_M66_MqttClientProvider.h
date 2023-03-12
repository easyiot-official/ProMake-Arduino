#ifndef _PROMAKE_M66_MQTTCLIENTPROVIDER_
#define _PROMAKE_M66_MQTTCLIENTPROVIDER_

#include <ProMake_GSM_ProviderBase.h>
#include "ProMake_M66_AccessProvider.h"

#include <IPAddress.h>

#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*,char*)


class ProMakeM66MqttClientProvider : public ProMakeGsmProviderBase
{
private:
    int _remotePort; // Current operation remote port.
    const char *_remoteServer;
    int _keepAliveTime;

    const char *_willTopic;
    const char *_willPayload;
    int _willQoS;
    int _willRetain;

    /** Continue to connect MQTT client function
     */
    void connectMQTTClientContinue();

    /** Continue to disconnect MQTT client function
     */
    void disconnectMqttContinue();

    const char *_pubTopic;
    const char *_pubPayload;
    int _pubQos;
    int _pubRetain;
    /** Continue to publish function
     */
    void publishContinue();

    const char *_subTopic;
    int _subQos;
    MQTT_CALLBACK_SIGNATURE;
    /** Continue to subscribe function
     */
    void subscribeContinue();

public:
    /** Constructor */
    ProMakeM66MqttClientProvider(ProMake_M66_Modem *Modem, MQTT_CALLBACK_SIGNATURE);

    void setCallback(MQTT_CALLBACK_SIGNATURE);
    /** Connect to a remote TCP server
        @param server		String with IP or server name
        @param port 		Remote port number
        @param id_socket	Local socket number
        @return 0 if command running, 1 if success, otherwise error
     */
    int connectMqttClient(const char *server, int port, const char *willTopic, const char *willPayload, int willQoS, int willRetain, int keepAliveTime, bool synchronous = true);

    /** Close a socket
    @param client1Server0	1 if modem acts as client, 0 if acts as server
    @param id_socket		Socket
    @return 0 if command running, 1 if success, otherwise error
    */
    int disconnectMqtt(bool synchronous = true);

    int publish(const char *topic, const char *payload, int qos, int retain, bool synchronous = true);
    int subscribe(const char *topic, int qos, bool synchronous = true);

    /** Recognize unsolicited event
        @param oldTail
        @return true if successful
     */
    bool recognizeUnsolicitedEvent(byte from);

    /** Manages modem response
        @param from 		Initial byte position
        @param to 			Final byte position
     */
    void manageResponse(byte from, byte to);

    /** Returns 0 if last command is still executing
    @return 1 if success, >1 if error
    */
    int ready() { return ProMakeGsmProviderBase::ready(); };
};

#endif
