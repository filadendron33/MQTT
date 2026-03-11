
#include <Arduino.h>
#include "MQTTClient.h"

String MQTTClient::messageArrived = "";
String MQTTClient::topicArrived = "";


MQTTClient::MQTTClient(PubSubClient& client, const char* serverIP) : mqtt_server(serverIP){
  pMqtt_client = &client;
}

MQTTClient::~MQTTClient(){

} 


void MQTTClient::callback (char* topic , byte* payload, unsigned int lenght){
   Serial.print("Message arrived [");
   Serial.print(topic);
   Serial.print("] ");
   MQTTClient::topicArrived = String(topic);

    MQTTClient::messageArrived = "";
    for (int i = 0; i < lenght; i++ ){
      Serial.print((char)payload[i]);
      MQTTClient::messageArrived += (char)payload[i];
    }
    
    Serial.println();
}


void MQTTClient::connectToMqtt(){
    while (!pMqtt_client->connected()){
     
    String clientID = "MQTTClient - ";
    clientID += String(random(1,100));

    Serial.println("Trying to connect to MQTT Server with " + clientID);


    if (pMqtt_client->connect(clientID.c_str())) {

      Serial.println("Connected to MQTT Server with " + clientID);


    }else{
          Serial.println("Didn't connect. Try again.");
        Serial.println(pMqtt_client->state());
        Serial.println("Try again in 5 sec ");

        delay(5000);
      }

  }
}


void MQTTClient::sendToBroker(String value)
{
  if(pMqtt_client->connected()){

     

      if(pMqtt_client->publish("SensorValue", value.c_str())){
          Serial.println ("Message with value " + value + " should be sent. Wait for callback");
      }

  }    

}

void MQTTClient::subscribeToTopic(){

  if (!subscribed){
    if (pMqtt_client->subscribe("SensorValue")){
          Serial.println("Successfully subscribed to topic");
        }
    if (pMqtt_client->subscribe("IpAddress")){
          Serial.println("Successfully subscribed to topic");
        }

    if (pMqtt_client->subscribe("MQTTState")){
       Serial.println("Successfully subscribed to topic");
      }
      subscribed = true;
  }

}
void MQTTClient::parseMessageArrived(String& message, String& topic){
  lastMessage = message;
  lastTopic = topic;
  if (topic == "MQTTState"){
    if (message = 1)
    {
      mqttClientState = true;
    }else if (message == 0){
      mqttClientState = false;
    }
  }
  message = "";
  topic = "";
}


void MQTTClient::mqttSetup(){
   connectToMqtt();
   subscribeToTopic();
}

void MQTTClient::cyclicLogic()
{
  pMqtt_client->loop();
  if (messageArrived != lastMessage || topicArrived != lastTopic){
    parseMessageArrived(messageArrived, topicArrived);
  }
  
}