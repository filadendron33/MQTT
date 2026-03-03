#include <PubSubClient.h>
#include "WifiConfig.h"






class MQTTClient{
    public:
        MQTTClient(WiFiClient& wifi, const char* serverIP);
        ~MQTTClient();
        void connectToMqtt(String value);
        void callback (char* topic , byte* payload, unsigned int lenght);
        PubSubClient mqtt_client;
    private:
        const char* mqtt_server;

};