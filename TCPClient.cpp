#include "TCPClient.h"


TCPClientClass::TCPClientClass()
{
  client = nullptr;
  hasAClient = false;
  recv = nullptr;
  sent = false;
  dataRecieved = false;
  state = TCPClientClass::ConnectStart;
  connectStartTime = 0;
  mqtt_state = true;
}

TCPClientClass::~TCPClientClass()
{
  if(client)
  {
    delete client;
    client = nullptr;
  }
}


void TCPClientClass::setup_ethernet(){
Serial.println("Starting Ethernet...");


      bool start = ETH.begin();

      if (!start){
        Serial.println("ETH.begin() returned false. Need diffrent arguments");
      }

 
      unsigned long waitTime = millis();
      Serial.println("Waiting for ethernet link");
      while(!ETH.linkUp() && (millis()-waitTime < 10000)){
        Serial.print(".");
        delay(200);
      }

      ETH.config(clientIP, subnet);

      if(ETH.linkUp()){
        Serial.print("Link is up, IP address is: ");
        Serial.println(ETH.localIP());
      } else{
        Serial.println("Bad link.");
      }

}

void TCPClientClass::onConnect(void* arg, AsyncClient* c){
  Serial.println("TCP CONNECTED");

}

void TCPClientClass::onDisconnect(void* arg, AsyncClient* c){
  TCPClientClass* self = static_cast<TCPClientClass*>(arg);
  Serial.println("TCP DISCONNECTED");
  self->state = TCPClientClass::ConnectStart;
}

void TCPClientClass::onError(void* arg, AsyncClient* c, int8_t error){
  Serial.printf("TCP ERROR: %d\n", error);

}


void TCPClientClass::onData(void* arg, AsyncClient* c, void* data, size_t len){
    TCPClientClass* self = static_cast<TCPClientClass*>(arg);
    self->message = "";
    self->dataRecieved = false;

   
    self->recv = (uint8_t*)data;


    for (size_t i = 0; i < len; i++) {
      if (self->recv[i] != '\r'){
          self->message.concat((char)self->recv[i]);
      }
     
    }

   
  Serial.println(self->message);

  self->dataRecieved = true;
  memset(self->recv,0,len);


}



void TCPClientClass::connectToServer()
{

    Serial.println("Connnecting to Server");

    sent = false;
    
    if (hasAClient) {
      delete client; 
      client = nullptr;
      hasAClient = false;
    }
    client = new AsyncClient();
    


    client->onConnect(onConnect, nullptr);
    client->onDisconnect(onDisconnect, this);
    client->onError(onError, nullptr);
    client->onData(onData, this);


    if (!client->connect(serverIP,port)){
      connectStartTime = millis();
      Serial.println("Failed connection to Server");
      delete client;
      client = nullptr;
    }
    else {
      Serial.println("Connect sent to Server");
      hasAClient = true;
    }

}

void TCPClientClass::send()
{
  
  if (mqtt_state) {
    Serial.println("Sending read to Server");
    String message = "DEW1\r";

    if (client->canSend()){
      client->write(message.c_str(),message.length());
    }
  }
  else {
    Serial.println("Sending write to Server");
    String message = "DEW1\r";

    if (client->canSend()){
      client->write(message.c_str(),message.length());
    }
  }

  

}



uint8_t* TCPClientClass::getRecv(){

  return recv;
}


void TCPClientClass::cyclicLogic(){
  switch(state){
    case TCPClientClass::ConnectStart:
      
        connectToServer();
        state = TCPClientClass::WaitingConnection;
       
        if (error == true){
         state = TCPClientClass::Error;
        }
      break;
    case TCPClientClass::WaitingConnection:
        if (client && client->connected())
        {
          state = TCPClientClass::SendToServer;
        }
        if (error == true){
        state = TCPClientClass::Error;
        }
        break;
    case TCPClientClass::SendToServer:
          if (client->connected() && !sent){
            send();
            sent = true;
            state = TCPClientClass::RecieveData;
          }
          else if (error) {
            state = TCPClientClass::Error;
          }
          break;
      case TCPClientClass::RecieveData:
        if (sent && !dataRecieved){
          break;
        }else if (sent && dataRecieved)
        {
          state = TCPClientClass::SendToServer;
          sent = false;
          break;
        }

      case TCPClientClass::Error:
        error = false;
        state = TCPClientClass::ConnectStart;
        break;

  }
}
