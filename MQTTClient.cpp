
  #include <Arduino.h>
  #include "MQTTClient.h"

  // String MQTTClient::currentMessage = "";
  // String MQTTClient::currentTopic = "";
  // MQTTState MQTTClient::State;
  MQTTClient* MQTTClient::pSelf = nullptr;


  MQTTClient::MQTTClient(PubSubClient& client, const char* pServerIP) : Client.pIPAddres(pServerIP){
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

      pSelf->currentMessage = "";
      for (int i = 0; i < nLenght; i++ ){
        pSelf->Data.sCurrentMessage += (char)pPayload[i];
      }

      Serial.println(pSelf->Data.sCurrentMessage);
      pSelf->Data.bMessageArrived = true;
      Serial.println();
  }


  void MQTTClient::ParseMessageArrived(String sMessage, String sTopic){

    Serial.println("Sending bMessage and topic to TCP " +  sTopic + " " + sMessage);
    if(sTopic == cmdTopic){
        SendToTCP.sCommand = sMessage;
        SendToTCP.bSend = true;
    }else if (sTopic == dataTopic){
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


      if (pMqtt_client->connect(sClientID.c_str(),"plc/status/MQTTConnection", 1, true, "DISCONNECTED")) {

        Serial.println("Connected to MQTT Server with " + sClientID);
        return true;

      }else{
          Serial.println("Didn't connect. Try again.");
          Serial.println(pMqtt_client->state());
          Serial.println("Try again in 5 sec ");
          delay(5000);
          return false;
        }
    
  }


  void MQTTClient::Publish(String sTopic, String sMessage){
      if(pMqtt_client->connected()){
          
          if(pMqtt_client->Publish(sTopic.c_str(), sMessage.c_str())){
            // Serial.println("Message with value " + bMessage + " " + topic + " should be sent. Wait for Callback");
            State = MQTTState::Connected;
          }
            
        } 
  }

  bool MQTTClient::SubscribeToTopic(const char* pTopic){

      if(pMqtt_client->subscribe(pTopic))
      {
        Serial.println("Subscribed to topic");
        return true;
      }else {
        Serial.println("Failed to subscribed.");
        return false;
      }
        
  }


  void MQTTClient::MqttSetup(){
    Data.sCmdTopic = "command/cmd";
    Data.sDataTopic = "data/inputData";
    if (SubscribeToTopic(Data.sCmdTopic) && SubscribeToTopic(Data.sDataTopic)){
      Serial.println("subscribed");
      Data.bSubscribed = true;
    }
  }

  void MQTTClient::CyclicLogic()
  {

    switch (State){
      
      case MQTTState::NotConnected:
        if(!pMqtt_client->connected()){
            State = MQTTState::Connect;
            Data.bSubscribed = false;
            ClientData.bConnected = false;
        }else{
            State = MQTTState::Connected;
        }
          
          break;

      case MQTTState::Connect:
          if (ConnectToMqtt()) {
            State = MQTTState::Connected;
            Data.sConnectionStatus = "CONNECTED";
            
            ClientData.bConnected = true;
          }else{
            State = MQTTState::NotConnected;
          }
          break;

      case MQTTState::Connected:
          if(!Data.bSubscribed){
            State = MQTTState::Subscribe;
          }else{
            pMqtt_client->loop();
            Publish("plc/status/TCPConnection", *stFromTCP.connectionStateRecieved);
            Publish("plc/status/MQTTConnection", Data.sConnectionStatus);
            if (Data.sMessageArrived){
              State = MQTTState::Parsing;
            }

            if(!pMqtt_client->connected()){
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