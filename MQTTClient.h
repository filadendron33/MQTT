#ifndef _MQTT_CLIENT_H_
#define _MQTT_CLIENT_H_

#include <PubSubClient.h>
#include "WifiConfig.h"

    enum MQTTState{
        NotConnected,
        Connect,
        Connected,
        Subscribe,
        Subscribed,
        Parsing, 
        Sending
    };

    struct toTCP{
        //Interface with TCP
        String command;
        String inputValue;
        bool send;
    };

    struct fromTCP{
        String sensorValueRecieved;
        String writingDone;
        bool recieved;
    };





class MQTTClient{
    public:


    //Constructor and Destructor
     MQTTClient(PubSubClient& client, const char* serverIP);
     ~MQTTClient();
     static MQTTState eMqttState;
     toTCP stToTCP;
     fromTCP stFromTCP;
     //Functions
     bool connectToMqtt();
     void publish(String topic, String message);
     bool subscribeToTopic(const char* topic);
     void parseMessageArrived(String message, String topic);
     static void callback (char* topic , byte* payload, unsigned int lenght);
     void mqttSetup();
     void cyclicLogic();

     //MQTT client stuff
     PubSubClient* pMqtt_client;
     const char* mqtt_server;
   
     //MQTT callback 
     static String currentMessage;
     static String currentTopic;

     bool connected;
     
    private:
        bool subscribed;
        const char* cmdTopic;
        const char* dataTopic;
        

};

#endif