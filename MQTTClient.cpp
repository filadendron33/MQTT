
#include <Arduino.h>
#include "MQTTClient.h"


MQTTClient::MQTTClient(PubSubClient& client, const char* serverIP) : mqtt_server(serverIP){
  pMqtt_client = &client;
  Serial.println("Pointer to client set " + (uint32_t)pMqtt_client);
}

MQTTClient::~MQTTClient(){

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


void MQTTClient::connectToMqtt(String value){
  

  while (!pMqtt_client->connected())
  {
     
    String clientID = "MQTTClient - ";
    clientID += String(random(1,100));

    Serial.println("Trying to connect to MQTT Server with " + clientID);


    if (pMqtt_client->connect(clientID.c_str())) {

      Serial.println("Connected to MQTT Server with " + clientID);



       if (pMqtt_client->subscribe("SensorTopic"))
       {
          Serial.println("Successfully subscribed to topic");
       }

      if(pMqtt_client->publish("SensorTopic", value.c_str())){
          Serial.println ("Message with value " + value + " should be sent. Wait for callback");
      }

      

      

    }
    else {
      Serial.println("Didn't connect. Try again.");
      Serial.println(pMqtt_client->state());
      Serial.println("Try again in 5 sec ");

      delay(5000);

    }
  }

}


void MQTTClient::mqttSetup(){
}