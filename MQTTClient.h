#ifndef _MQTT_CLIENT_H_
#define _MQTT_CLIENT_H_

#include <PubSubClient.h>
#include "WifiConfig.h"






class MQTTClient{
    public:
        MQTTClient(PubSubClient& client, const char* serverIP);
        ~MQTTClient();
        void connectToMqtt();
        void sendToBroker(String value);
        void subscribeToTopic();
        void parseMessageArrived(String& message, String& topic);
        static void callback (char* topic , byte* payload, unsigned int lenght);
        void mqttSetup();

        void cyclicLogic();
        PubSubClient* pMqtt_client;
        const char* mqtt_server;

        //MQTT callback 
        static String messageArrived;
        static String topicArrived;
        String lastMessage;
        String lastTopic;
        bool mqttClientState;
        
    private:
        bool subscribed;
        

};

#endif