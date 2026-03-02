#include <stdint.h>
#include "IPAddress.h"
#include <AsyncTCP.h>
#include "Arduino.h"
#include <ETH.h>



class MyClient {

  public:
  MyClient();
  ~MyClient();
  uint8_t* recv;
  bool dataRecieved;
 


  void setup_ethernet();
  void connectToServer();
  void sendToServer();
  uint8_t* getRecv();

  String message;

  static void onConnect(void* arg, AsyncClient* c);
  static void onDisconnect(void* arg, AsyncClient* c);
  static void onError(void* arg, AsyncClient* c, int8_t error);
  static void onData(void* arg, AsyncClient* c, void* data, size_t len);

  void serverLoop();

  private:
   bool hasAClient;
   bool sent;
  unsigned long connectStart;
  bool connecting ;  
  AsyncClient* client;
  
  

  const IPAddress clientIP{192,168,201,2};
  const IPAddress subnet{255,255,255,0};
  const IPAddress serverIP{192,168,201,1};
  uint16_t port = 5000;


};






