#ifndef _TCL_CLIENT_H_
#define _TCL_CLIENT_H_


#include <stdint.h>
#include "IPAddress.h"
#include <AsyncTCP.h>
#include "Arduino.h"
#include <ETH.h>



  

class TCPClientClass {


  
  public:
    TCPClientClass();
    ~TCPClientClass();
    uint8_t* recv;
    bool dataRecieved;
    bool error;
    String message;
    

    enum ClientState {

      ConnectStart,
      WaitingConnection,
      ClientConnected,
      SendToServer,
      RecieveData,
      Error

    }eState;

    struct fromMQTT {
    
      String* commandRecieved;
      String* valueRecieved;
      bool recieved;

    }stFromMQTT;

    struct toTCP{
      String sensorValue;
      String writingDone;
      String connectionState;
      bool send;
    }stToMQTT;



  


    bool setup_ethernet();
    bool connectToServer();
    void send();
    uint8_t* getRecv();
    void cyclicLogic();
    String parsingMessage(String message);




   static void onConnect(void* arg, AsyncClient* c);
   static void onDisconnect(void* arg, AsyncClient* c);
   static void onError(void* arg, AsyncClient* c, int8_t error);
   static void onData(void* arg, AsyncClient* c, void* data, size_t len);


  //Interface 

    

  private:
    bool hasAClient;
    unsigned long connectStartTime;  
    AsyncClient* client;
    const IPAddress clientIP{192,168,201,2};
    const IPAddress subnet{255,255,255,0};
    const IPAddress serverIP{192,168,201,1};
    uint16_t port = 5000;
    static TCPClientClass* pSelf;


};


#endif





