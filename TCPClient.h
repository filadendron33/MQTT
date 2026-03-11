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
  bool mqtt_state;
  bool error;
  String message;
  
  enum ClientState {
    
    ConnectStart,
    WaitingConnection,
    SendToServer,
    RecieveData,
    Error

  }state;
  
  
 


  void setup_ethernet();
  void connectToServer();
  void send();
  uint8_t* getRecv();
  void setMqttState(bool state);

  

  static void onConnect(void* arg, AsyncClient* c);
  static void onDisconnect(void* arg, AsyncClient* c);
  static void onError(void* arg, AsyncClient* c, int8_t error);
  static void onData(void* arg, AsyncClient* c, void* data, size_t len);

  void cyclicLogic();

  private:
   bool hasAClient;
   bool sent;
    unsigned long connectStartTime;  
    AsyncClient* client;
    const IPAddress clientIP{192,168,201,2};
    const IPAddress subnet{255,255,255,0};
    const IPAddress serverIP{192,168,201,1};
    uint16_t port = 5000;


};


#endif





