
  #include <Arduino.h>
  #include "MQTTClient.h"

  // String MQTTClient::currentMessage = "";
  // String MQTTClient::currentTopic = "";
  // MQTTState MQTTClient::State;
  MQTTClient* MQTTClient::pSelf = nullptr;


  MQTTClient::MQTTClient(PubSubClient& client, const char* pServerIP){
    Client.pIPAddres = pServerIP;
    Client.pMqtt_client = &client;
    Data.pCmdTopic = "";
    Data.pDataTopic = "";
    State = MQTTState::NotConnected;
    Data.bSubscribed = false;
    Data.bMessageArrived = false;
    Data.sConnectionStatus = "DISCONNECTED";
    pSelf = this;
  }

  MQTTClient::~MQTTClient(){

  } 

  //Recieving data
  void MQTTClient::Callback (char* pTopic ,byte* pPayload, unsigned int nLenght){
    Serial.print("Message arrived [");
    Serial.print(pTopic);
    Serial.print("] ");
      pSelf->Data.sCurrentTopic = String(pTopic);

      pSelf->Data.sCurrentMessage = "";
      for (int i = 0; i < nLenght; i++ ){
        pSelf->Data.sCurrentMessage += (char)pPayload[i];
      }

      Serial.println(pSelf->Data.sCurrentMessage);
      pSelf->Data.bMessageArrived = true;
      Serial.println();
  }


  void MQTTClient::ParseMessageArrived(String sMessage, String sTopic){

    Serial.println("Sending bMessage and topic to TCP " +  sTopic + " " + sMessage);
    if(sTopic == Data.pCmdTopic){
        SendToTCP.sCommand = sMessage;
        SendToTCP.bSend = true;
    }else if (sTopic == Data.pDataTopic){
        SendToTCP.sInputValue = "";
        SendToTCP.sInputValue = sMessage;
    }
    State = MQTTState::Connected;
    Data.bMessageArrived = false;


  }

  bool MQTTClient::ConnectToMqtt(){

      String sClientID = "MQTTClient - ";
      sClientID += String(random(1,100));

      Serial.println("Trying to connect to MQTT Server with " + sClientID);


      if (Client.pMqtt_client->connect(sClientID.c_str(),"plc/status/MQTTConnection", 1, true, "DISCONNECTED")) {

        Serial.println("Connected to MQTT Server with " + sClientID);
        return true;

      }else{
          Serial.println("Didn't connect. Try again.");
          Serial.println(Client.pMqtt_client->state());
          Serial.println("Try again in 5 sec ");
          delay(5000);
          return false;
        }
    
  }


  void MQTTClient::Publish(String sTopic, String sMessage){
      if(Client.pMqtt_client->connected()){
          
          if(Client.pMqtt_client->publish(sTopic.c_str(), sMessage.c_str())){
            // Serial.println("Message with value " + bMessage + " " + topic + " should be sent. Wait for Callback");
            State = MQTTState::Connected;
          }
            
        } 
  }

  bool MQTTClient::SubscribeToTopic(const char* pTopic){

      if(Client.pMqtt_client->subscribe(pTopic))
      {
        Serial.println("Subscribed to topic");
        return true;
      }else {
        Serial.println("Failed to subscribed.");
        return false;
      }
        
  }


  void MQTTClient::MqttSetup(){
    Data.pCmdTopic = "command/cmd";
    Data.pDataTopic = "data/inputData";
    if (SubscribeToTopic(Data.pCmdTopic) && SubscribeToTopic(Data.pDataTopic)){
      Serial.println("subscribed");
      Data.bSubscribed = true;
    }
  }

  void MQTTClient::CyclicLogic()
  {

    switch (State){
      
      case MQTTState::NotConnected:
        if(!Client.pMqtt_client->connected()){
            State = MQTTState::Connect;
            Data.bSubscribed = false;
            Client.bConnected = false;
        }else{
            State = MQTTState::Connected;
        }
          
          break;

      case MQTTState::Connect:
          if (ConnectToMqtt()) {
            State = MQTTState::Connected;
            Data.sConnectionStatus = "CONNECTED";
            
            Client.bConnected = true;
          }else{
            State = MQTTState::NotConnected;
          }
          break;

      case MQTTState::Connected:
          if(!Data.bSubscribed){
            State = MQTTState::Subscribe;
          }else{
            Client.pMqtt_client->loop();
            Publish("plc/status/TCPConnection", *RecieveFromTCP.pConnectionStateRecieved);
            Publish("plc/status/MQTTConnection", Data.sConnectionStatus);
            if (Data.bMessageArrived){
              State = MQTTState::Parsing;
            }

            if(!Client.pMqtt_client->connected()){
              Serial.println("MQTT Lost connection.");
              State = MQTTState::Connect;
            }
          }
          
          break;
      case MQTTState::Subscribe:
          Serial.println("Subscribe State");
          MqttSetup();
          State = MQTTState::Connected;
          break;

      case MQTTState::Parsing:
          Serial.println("Parsing state");
          ParseMessageArrived(Data.sCurrentMessage, Data.sCurrentTopic);
          break;
   
    }
  
}