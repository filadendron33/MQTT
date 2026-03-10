#ifndef _MQTT_CLIENT_H_
#define _MQTT_CLIENT_H_

#include <PubSubClient.h>
#include "WifiConfig.h"






class MQTTClient{
    public:
        MQTTClient(PubSubClient& client, const char* serverIP);
        ~MQTTClient();
        void connectToMqtt(String value);
        static void callback (char* topic , byte* payload, unsigned int lenght);
        void mqttSetup();
        PubSubClient* pMqtt_client;
        const char* mqtt_server;
    private:
        

};

#endif