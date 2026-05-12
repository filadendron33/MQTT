
  #include <Arduino.h>
  #include "MQTTClient.h"

  String MQTTClient::currentMessage = "";
  String MQTTClient::currentTopic = "";
  MQTTState MQTTClient::eMqttState;


  MQTTClient::MQTTClient(PubSubClient& client, const char* serverIP) : mqtt_server(serverIP){
    pMqtt_client = &client;
    cmdTopic = "";
    dataTopic = "";
    eMqttState = MQTTState::NotConnected;
    subscribed = false;
  }

  MQTTClient::~MQTTClient(){

  } 

  //Recieving data
  void MQTTClient::callback (char* topic ,byte* payload, unsigned int lenght){
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
      currentTopic = String(topic);

      currentMessage = "";
      for (int i = 0; i < lenght; i++ ){
        currentMessage += (char)payload[i];
      }

      Serial.println(currentMessage);
      
      Serial.println();
      MQTTClient::eMqttState = MQTTState::Parsing;
  }


  void MQTTClient::parseMessageArrived(String message, String topic){

    Serial.println("Sending message and topic to TCP " +  topic + " " + message);
    if(topic == cmdTopic){
        stToTCP.command = message;
        stToTCP.send = true;
    }else if (topic == dataTopic){
        stToTCP.inputValue = message;
    }
    eMqttState = MQTTState::Connected;


  }

  bool MQTTClient::connectToMqtt(){

      String clientID = "MQTTClient - ";
      clientID += String(random(1,100));

      Serial.println("Trying to connect to MQTT Server with " + clientID);


      if (pMqtt_client->connect(clientID.c_str())) {

        Serial.println("Connected to MQTT Server with " + clientID);
        return true;

      }else{
            Serial.println("Didn't connect. Try again.");
          Serial.println(pMqtt_client->state());
          Serial.println("Try again in 5 sec ");
          delay(5000);
          return false;
        }
    
  }


  void MQTTClient::publish(String topic, String message){
      if(pMqtt_client->connected()){
          
          if(pMqtt_client->publish(topic.c_str(), message.c_str())){
            Serial.println("Message with value " + message + " " + topic + " should be sent. Wait for callback");
            eMqttState = MQTTState::Connected;
          }
            
        } 
  }

  bool MQTTClient::subscribeToTopic(const char* topic){

      if(pMqtt_client->subscribe(topic))
      {
        Serial.println("Subscribed to topic");
        return true;
      }else {
        Serial.println("Failed to subscribed.");
        return false;
      }
        
  }


  void MQTTClient::mqttSetup(){
    cmdTopic = "command/cmd";
    dataTopic = "data/inputData";
    if (subscribeToTopic(cmdTopic) && subscribeToTopic(dataTopic)){
      Serial.println("subscribed");
      subscribed = true;
    }
  }

  void MQTTClient::cyclicLogic()
  {

    switch (eMqttState){
      
      case MQTTState::NotConnected:
        if(!pMqtt_client->connected()){
            eMqttState = MQTTState::Connect;
            subscribed = false;
            connected = false;
        }else{
            eMqttState = MQTTState::Connected;
        }
          
          break;

      case MQTTState::Connect:
          if (connectToMqtt()) {
            eMqttState = MQTTState::Connected;
            connected = true;
          }else{
            eMqttState = MQTTState::NotConnected;
          }
          break;

      case MQTTState::Connected:
          if(!subscribed){
            eMqttState = MQTTState::Subscribe;
          }else{
            pMqtt_client->loop();
          }
          
          if(!pMqtt_client->connected()){
            Serial.println("MQTT Lost connection.");
            eMqttState = MQTTState::Connect;
          }
          break;
      case MQTTState::Subscribe:
          Serial.println("Subscribe State");
          mqttSetup();
          eMqttState = MQTTState::Connected;
          break;

      case MQTTState::Parsing:
          parseMessageArrived(currentMessage, currentTopic);
          break;
   
    }
  
}