#include "esp32-hal.h"
#include "TCPClient.h"

TCPClientClass* TCPClientClass::pSelf = nullptr;

TCPClientClass::TCPClientClass(){
  mServer.pClient = nullptr;
  mServer.bHasAClient = false;
  Client.pRecv = nullptr;
  Client.bDataRecieved = false;
  State = TCPClientClass::ConnectStart;
  Client.bconnectStartTime = 0;
  pSelf = this;
  SendToMQTT.connectionState = "DISCONNECTED";
}

TCPClientClass::~TCPClientClass(){
  if(client){
    delete Client.pClient;
    Client.pClient = nullptr;
  }
}


//Setting ethernet 
bool TCPClientClass::SetupEthernet(){
  
    Serial.println("Starting Ethernet...");


      bool bStart = ETH.begin();

      if (!bStart){
        Serial.println("ETH.begin() returned false. Need diffrent arguments");
        return false;
      }

      ETH.config(mServer.clientIP, mServer.subnet);

      unsigned long nWaitTime = millis();
      Serial.println("Waiting for ethernet link");
      while(!ETH.linkUp() && (millis()-nWaitTime < 10000)){
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
  pSelf->SendToMQTT.connectionState = "CONNECTED";

}

//Callback on Disconnect
void TCPClientClass::onDisconnect(void* arg, AsyncClient* c){
  Serial.println("TCP DISCONNECTED");
  pSelf->State = ClientState::ConnectStart;
  pSelf->SendToMQTT.connectionState = "DISCONNECTED";
}

//calll back on Error
void TCPClientClass::onError(void* arg, AsyncClient* c, int8_t bError){
  Serial.printf("TCP ERROR: %d\n", bError);

}

//Call back on data, parse data to message
void TCPClientClass::onData(void* arg, AsyncClient* c, void* data, size_t len){
    
    pSelf->bMessage = "";
    pSelf->SendToMQTT.Send = false;

   

   if (len > 0){
    pSelf->Client.pRecv = (uint8_t*)data;


    for (size_t i = 0; i < len; i++) {
      if (pSelf->Client.pRecv[i] != '\r'){
          pSelf->bMessage.concat((char)pSelf->Client.pRecv[i]);
      }
     
    }
     Serial.println(pSelf->bMessage);
     pSelf->Client.bDataRecieved = true;
     
   }
    
   memset(pSelf->Client.pRecv,0,len);
}


//Connect to ESP
bool TCPClientClass::ConnectToServer(){

    Serial.println("Connnecting to Server");

    
    if (Client.bHasAClient){
      delete Client.pClient; 
      Client.pClient = nullptr;
      Client.bHasAClient = false;
    }
    Client.pClient = new AsyncClient();
    


    Client.pClient ->onConnect(onConnect, nullptr);
    Client.pClient ->onDisconnect(onDisconnect, this);
    Client.pClient ->onError(onError, nullptr);
    Client.pClient ->onData(onData, this);


    if (!Client.pClient ->connect(serverIP,port)){
      mServer.nConnectionStartTime = millis();
      Serial.println("Failed connection to Server");
      delete Client.pClient ;
      Client.pClient  = nullptr;
      return false;
    }
    else {
      Serial.println("Connect sent to Server");
      Client.bHasAClient = true;
      return true;
    }

}

//Function for sending bMessage to Server
void TCPClientClass::Send(){
  
  Serial.println("Sending to PLC");
    if (Client.pClient ->canSend()){
      if(*RecieveFromMQTT.pCommandRecieved == "write"){
        String sCommand = "M" + *RecieveFromMQTT.pValueRecieved + 0x0D;
        Serial.println("Command " + sCommand);
        Client.pClient ->write(sCommand.c_str());
        State = ClientState::RecieveData;
      }else if (*RecieveFromMQTT.pCommandRecieved == "read"){
          Client.pClient ->write("DMW\r");    
          State = ClientState::RecieveData;
      }
    }

}


String TCPClientClass::ParsingMessage(String bMessage){
  //Writing command MMW=42 + 0x0D + 0x0A + ">" = Ox15
  //Reading command DMW=43 + 0x0D + 0x0A + ">" = 0x15
  String sParsed;
  if (bMessage.startsWith("M")){
    //Writing
    Serial.println("Parsing write");
    if(bMessage.endsWith(0x0D + 0x0A + ">" + 0x15)){
      sParsed = "Done";
      Serial.println("Message recieved " + sParsed);
      return sParsed;
    }else {
      sParsed = "Failed";
    }
  }else{
    //Reading
    const char* pArr = bMessage.c_str();
    Serial.println("Parsing read");

    for(int i = 0; i < bMessage.length(); i++){
      if (pArr[i-1] == '='){
        while(pArr[i] != 0x0A){
          sParsed.concat(pArr[i]);
          i++;
        }
      }
    }

    Serial.println("Message recieved " + sParsed);

    return sParsed;
  }
}

//Client logic
void TCPClientClass::CyclicLogic(){

  
  switch(State){

    case ClientState::ConnectStart:

        if(!SetupEthernet()){
          break;
        }else if (ConnectToServer()){
          
          State = ClientState::WaitingConnection;
        }

        if (bError == true){
         State = ClientState::Error;
        }

      break;

    case ClientState::WaitingConnection:
        if (Client.pClient  && Client.pClient ->connected())
        {
          State = ClientState::ClientConnected;
        }
        else if (bError == true){
        State = ClientState::Error;
        }
        break;

    case ClientState::ClientConnected:
      if(!Client.pClient ->connected()){
        delay(2000);
        State = ClientState::ConnectStart;
      }
      
    case ClientState::RecieveData:
        if (Client.bDataRecieved){
          SendToMQTT.Send = true;
          Client.bDataRecieved = false;

          if (*RecieveFromMQTT.pCommandRecieved == "write")
          {
            
            SendToMQTT.writingDone = ParsingMessage(bMessage);

            State = ClientState::ClientConnected;

          }else if (*RecieveFromMQTT.pCommandRecieved == "read"){
            Serial.println("Sending bMessage to MQTT to publsih.");
            // SendToMQTT.sensorValue = bMessage;
            SendToMQTT.sensorValue = ParsingMessage(bMessage);

            Serial.println(SendToMQTT.sensorValue);
            State = ClientState::ClientConnected;
          }
          
        }
        break;
    
    case ClientState::Error:
        bError = false;
        if (!client->connected()){
          Serial.println("Error state : Starting connection again");
          State = ClientState::ConnectStart;
        }
        break;

  }
}


