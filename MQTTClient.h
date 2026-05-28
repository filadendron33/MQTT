#ifndef _MQTT_CLIENT_H_
#define _MQTT_CLIENT_H_

#include <PubSubClient.h>
#include "WifiConfig.h"





class MQTTClient{
    public:
        enum MQTTState{
            NotConnected,
            Connect,
            Connected,
            Subscribe,
            Subscribed,
            Parsing, 
            Sending
        };

        struct ToTCP{
            //Interface with TCP
            String sCommand;
            String sInputValue;
            bool bSend;
        };

        struct FromTCP{
            String* pSensorValueRecieved;
            String* pWritingDone;
            String* pConnectionStateRecieved;
            bool bRecieved;
        };

        struct MQTTData{

            bool bSubscribed;
            const char* pCmdTopic;
            const char* pDataTopic;
            String sConnectionStatus;
            bool bMessageArrived;
            String sCurrentMessage;
            String sCurrentTopic;

        };

        struct MQTTClientData{
            
            PubSubClient* pMqtt_client;
            const char* pIPAddres;
            bool bConnected;

        };

        //Constructor and Destructor
        MQTTClient(PubSubClient& client, const char* serverIP);
        ~MQTTClient();
        //Functions
        bool ConnectToMqtt();
        void Publish(String sTopic, String bMessage);
        bool SubscribeToTopic(const char* pTopic);
        void ParseMessageArrived(String bMessage, String sTopic);
        static void Callback (char* topic , byte* payload, unsigned int lenght);
        void MqttSetup();
        void CyclicLogic();
        static MQTTClient* pSelf;
        MQTTState State;
        ToTCP SendToTCP;
        FromTCP RecieveFromTCP;
        //MQTT client stuff
        //  PubSubClient* pMqtt_client;
        //  const char* mqtt_server;
        MQTTClientData Client;
        //MQTT Callback 
        
        

    private:
        // bool subscribed;
        // const char* cmdTopic;
        // const char* dataTopic;
        // String connectionStatus;
        // bool messageArrived;
        MQTTData Data;
        

};

#endif