#ifndef _TCL_CLIENT_H_
#define _TCL_CLIENT_H_


#include <stdint.h>
#include "IPAddress.h"
#include <AsyncTCP.h>
#include "Arduino.h"
#include <ETH.h>



  

class TCPClientClass {


  
  public:
    enum ClientState {

      ConnectStart,
      WaitingConnection,
      ClientConnected,
      SendToServer,
      RecieveData,
      Error

    };

    struct FromMQTT {
    
      String* pCommandRecieved;
      String* pValueRecieved;
      bool bRecieved;

    };

    struct ToMQTT{

      String sSensorValue;
      String sWritingDone;
      String sConnectionState;
      bool bSend;

    };

    struct ClientData{

      uint8_t* pRecv;
      AsyncClient* pClient;
      bool bHasAClient;
      bool bDataRecieved;
      bool bError;
      String sMessage;

      };

    TCPClientClass();
    ~TCPClientClass();
    // uint8_t* pRecv;
    // bool bDataRecieved;
    // bool bError;
    // String bMessage;
    ToMQTT SendToMQTT;
    FromMQTT RecieveFromMQTT;
    ClientData Client;
    ClientState State;
    

    bool SetupEthernet();
    bool ConnectToServer();
    void Send();
    uint8_t* GetRecv();
    void CyclicLogic();
    String ParsingMessage(String sMessage);

   static void onConnect(void* arg, AsyncClient* c);
   static void onDisconnect(void* arg, AsyncClient* c);
   static void onError(void* arg, AsyncClient* c, int8_t bError);
   static void onData(void* arg, AsyncClient* c, void* data, size_t len);

  private:
    struct TCPServer{
      unsigned long nConnectStartTime;  
      const IPAddress clientIP{192,168,201,2};
      const IPAddress subnet{255,255,255,0};
      const IPAddress serverIP{192,168,201,1};
      uint16_t nPort = 5000;
    };
    // bool hasAClient;
    // unsigned long connectStartTime;  
    // AsyncClient* client;
    // const IPAddress clientIP{192,168,201,2};
    // const IPAddress subnet{255,255,255,0};
    // const IPAddress serverIP{192,168,201,1};
    // uint16_t port = 5000;
    TCPServer Server;
    static TCPClientClass* pSelf;
};


#endif





