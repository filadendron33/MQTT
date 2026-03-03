#include "MQTTClient.h"


MQTTClient::MQTTClient(WiFiClient& wifi) : mqtt_client(wifi){

}



void MQTTClient::callback (char* topic , byte* payload, unsigned int lenght)
{
   Serial.print("Message arrived [");
   Serial.print(topic);
   Serial.print("] ");

    for (int i = 0; i < lenght; i++ ){
      Serial.print((char)payload[i]);
    }

    Serial.println();
}