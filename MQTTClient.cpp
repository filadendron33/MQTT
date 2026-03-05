#include "MQTTClient.h"


MQTTClient::MQTTClient(WiFiClient& wifi, const char* serverIP) : mqtt_client(wifi), mqtt_server(serverIP){

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
  

  while (!mqtt_client.connected())
  {
     
    String clientID = "MQTTClient - ";
    clientID += String(random(0,100));

    Serial.println("Trying to connect to MQTT Server with " + clientID);


    if (mqtt_client.connect(clientID.c_str())) {

      Serial.println("Connected to MQTT Server with " + clientID);



       if (mqtt_client.subscribe("SensorTopic"))
       {
          Serial.println("Successfully subscribed to topic");
       }

      if(mqtt_client.publish("SensorTopic", value.c_str())){
          Serial.println ("Message with value " + value + " should be sent. Wait for callback");
      }

      

      

    }
    else {
      Serial.println("Didn't connect. Try again.");
      Serial.println(mqtt_client.state());
      Serial.println("Try again in 5 sec ");

      delay(5000);

    }
  }

}


void MQTTClient::mqttSetup(){
    mqtt_client.setServer(mqtt_server,1884);
    mqtt_client.setCallback(callback);
}