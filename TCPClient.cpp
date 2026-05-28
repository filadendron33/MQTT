#include "esp32-hal.h"
#include "TCPClient.h"

TCPClientClass* TCPClientClass::pSelf = nullptr;

TCPClientClass::TCPClientClass(){
  client = nullptr;
  hasAClient = false;
  recv = nullptr;
  dataRecieved = false;
  eState = TCPClientClass::ConnectStart;
  connectStartTime = 0;
  pSelf = this;
  stToMQTT.connectionState = "DISCONNECTED";
}

TCPClientClass::~TCPClientClass(){
  if(client){
    delete client;
    client = nullptr;
  }
}


//Setting ethernet 
bool TCPClientClass::setup_ethernet(){
  
    Serial.println("Starting Ethernet...");


      bool start = ETH.begin();

      if (!start){
        Serial.println("ETH.begin() returned false. Need diffrent arguments");
        return false;
      }

      ETH.config(clientIP, subnet);

      unsigned long waitTime = millis();
      Serial.println("Waiting for ethernet link");
      while(!ETH.linkUp() && (millis()-waitTime < 10000)){
        Serial.print(".");
        delay(200);
      }

      

      if(ETH.linkUp()){
        Serial.print("Link is up, IP address is: ");
        Serial.println(ETH.localIP());
        return true;
      } else{
        Serial.println("Bad link.");
        return false;
      }

}

//call back on connection
void TCPClientClass::onConnect(void* arg, AsyncClient* c){
  Serial.println("TCP CONNECTED");
  pSelf->stToMQTT.connectionState = "CONNECTED";

}

//Callback on Disconnect
void TCPClientClass::onDisconnect(void* arg, AsyncClient* c){
  Serial.println("TCP DISCONNECTED");
  pSelf->eState = ClientState::ConnectStart;
  pSelf->stToMQTT.connectionState = "DISCONNECTED";
}

//calll back on Error
void TCPClientClass::onError(void* arg, AsyncClient* c, int8_t error){
  Serial.printf("TCP ERROR: %d\n", error);

}

//Call back on data, parse data to message
void TCPClientClass::onData(void* arg, AsyncClient* c, void* data, size_t len){
    
    pSelf->message = "";
    pSelf->stToMQTT.send = false;

   

   if (len > 0){
    pSelf->recv = (uint8_t*)data;


    for (size_t i = 0; i < len; i++) {
      if (pSelf->recv[i] != '\r'){
          pSelf->message.concat((char)pSelf->recv[i]);
      }
     
    }
     Serial.println(pSelf->message);
     pSelf->dataRecieved = true;
     
   }
    
   memset(pSelf->recv,0,len);
}


//Connect to ESP
bool TCPClientClass::connectToServer(){

    Serial.println("Connnecting to Server");

    
    if (hasAClient){
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
      return false;
    }
    else {
      Serial.println("Connect sent to Server");
      hasAClient = true;
      return true;
    }

}

//Function for sending message to Server
void TCPClientClass::send(){
  
  Serial.println("Sending to PLC");
    if (client->canSend()){
      if(*stFromMQTT.commandRecieved == "write"){
        String command = "M" + *stFromMQTT.valueRecieved + 0x0D;
        Serial.println("Command " + command);
        client->write(command.c_str());
        eState = ClientState::RecieveData;
      }else if (*stFromMQTT.commandRecieved == "read"){
          client->write("DMW\r");    
          eState = ClientState::RecieveData;
      }
    }

}


String TCPClientClass::parsingMessage(String message){
  //Writing command MMW=42 + 0x0D + 0x0A + ">" = Ox15
  //Reading command DMW=43 + 0x0D + 0x0A + ">" = 0x15
  String parsed;
  if (message.startsWith("M")){
    //Writing
    Serial.println("Parsing write");
    if(message.endsWith(0x0D + 0x0A + ">" + 0x15)){
      parsed = "Done";
      Serial.println("Message recieved " + parsed);
      return parsed;
    }else {
      parsed = "Failed";
    }
  }else{
    //Reading
    const char* arr = message.c_str();
    Serial.println("Parsing read");

    for(int i = 0; i < message.length(); i++){
      if (arr[i-1] == '='){
        while(arr[i] != 0x0A){
          parsed.concat(arr[i]);
          i++;
        }
      }
    }

    Serial.println("Message recieved " + parsed);

    return parsed;
  }
}

//Client logic
void TCPClientClass::cyclicLogic(){

  
  switch(eState){

    case ClientState::ConnectStart:

        if(!setup_ethernet()){
          break;
        }else if (connectToServer()){
          
          eState = ClientState::WaitingConnection;
        }

        if (error == true){
         eState = ClientState::Error;
        }

      break;

    case ClientState::WaitingConnection:
        if (client && client->connected())
        {
          eState = ClientState::ClientConnected;
        }
        else if (error == true){
        eState = ClientState::Error;
        }
        break;

    case ClientState::ClientConnected:
      if(!client->connected()){
        delay(2000);
        eState = ClientState::ConnectStart;
      }
      
    case ClientState::RecieveData:
        if (dataRecieved){
          stToMQTT.send = true;
          dataRecieved = false;

          if (*stFromMQTT.commandRecieved == "write")
          {
            
            stToMQTT.writingDone = parsingMessage(message);

            eState = ClientState::ClientConnected;

          }else if (*stFromMQTT.commandRecieved == "read"){
            Serial.println("Sending message to MQTT to publsih.");
            // stToMQTT.sensorValue = message;
            stToMQTT.sensorValue = parsingMessage(message);

            Serial.println(stToMQTT.sensorValue);
            eState = ClientState::ClientConnected;
          }
          
        }
        break;
    
    case ClientState::Error:
        error = false;
        if (!client->connected()){
          Serial.println("Error state : Starting connection again");
          eState = ClientState::ConnectStart;
        }
        break;

  }
}


