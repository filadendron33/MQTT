#include "esp32-hal.h"
#include "TCPClient.h"

TCPClientClass* TCPClientClass::pSelf = nullptr;

TCPClientClass::TCPClientClass(){
  Client.pClient = nullptr;
  Client.bHasAClient = false;
  Client.pRecv = nullptr;
  Client.bDataRecieved = false;
  State = TCPClientClass::ConnectStart;
  Server.nConnectStartTime = 0;
  pSelf = this;
  SendToMQTT.sConnectionState = "DISCONNECTED";
}

TCPClientClass::~TCPClientClass(){
  if(Client.pClient){
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

      ETH.config(Server.clientIP, Server.subnet);

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
  pSelf->SendToMQTT.sConnectionState = "CONNECTED";

}

//Callback on Disconnect
void TCPClientClass::onDisconnect(void* arg, AsyncClient* c){
  Serial.println("TCP DISCONNECTED");
  pSelf->State = ClientState::ConnectStart;
  pSelf->SendToMQTT.sConnectionState = "DISCONNECTED";
}

//calll back on Error
void TCPClientClass::onError(void* arg, AsyncClient* c, int8_t bError){
  Serial.printf("TCP ERROR: %d\n", bError);

}

//Call back on data, parse data to message
void TCPClientClass::onData(void* arg, AsyncClient* c, void* data, size_t len){
    
    pSelf->Client.sMessage = "";
    pSelf->SendToMQTT.bSend = false;

   

   if (len > 0){
    pSelf->Client.pRecv = (uint8_t*)data;


    for (size_t i = 0; i < len; i++) {
      if (pSelf->Client.pRecv[i] != '\r'){
          pSelf->Client.sMessage.concat((char)pSelf->Client.pRecv[i]);
      }
     
    }
     Serial.println(pSelf->Client.sMessage);
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


    if (!Client.pClient->connect(Server.serverIP,Server.nPort)){
      Server.nConnectStartTime = millis();
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


String TCPClientClass::ParsingMessage(String sMessage){
  //Writing command MMW=42 + 0x0D + 0x0A + ">" = Ox15
  //Reading command DMW=43 + 0x0D + 0x0A + ">" = 0x15
  String sParsed;
  if (sMessage.startsWith("M")){
    //Writing
    Serial.println("Parsing write");
    if(sMessage.endsWith(0x0D + 0x0A + ">" + 0x15)){
      sParsed = "Done";
      Serial.println("Message recieved " + sParsed);
      return sParsed;
    }else {
      sParsed = "Failed";
    }
  }else{
    //Reading
    const char* pArr = sMessage.c_str();
    Serial.println("Parsing read");

    for(int i = 0; i < sMessage.length(); i++){
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

        if (Client.bError == true){
         State = ClientState::Error;
        }

      break;

    case ClientState::WaitingConnection:
        if (Client.pClient && Client.pClient->connected())
        {
          State = ClientState::ClientConnected;
        }
        else if (Client.bError == true){
        State = ClientState::Error;
        }
        break;

    case ClientState::ClientConnected:
      if(!Client.pClient->connected()){
        delay(2000);
        State = ClientState::ConnectStart;
      }
      
    case ClientState::RecieveData:
        if (Client.bDataRecieved){
          SendToMQTT.bSend = true;
          Client.bDataRecieved = false;

          if (*RecieveFromMQTT.pCommandRecieved == "write")
          {
            
            SendToMQTT.sWritingDone = ParsingMessage(Client.sMessage);

            State = ClientState::ClientConnected;

          }else if (*RecieveFromMQTT.pCommandRecieved == "read"){
            Serial.println("Sending Message to MQTT to publsih.");
            // SendToMQTT.sensorValue = bMessage;
            SendToMQTT.sSensorValue = ParsingMessage(Client.sMessage);

            Serial.println(SendToMQTT.sSensorValue);
            State = ClientState::ClientConnected;
          }
          
        }
        break;
    
    case ClientState::Error:
        Client.bError = false;
        if (!Client.pClient->connected()){
          Serial.println("Error state : Starting connection again");
          State = ClientState::ConnectStart;
        }
        break;

  }
}


