#include "TCPClient.h"


MyClient::MyClient()
{
  client = nullptr;
  hasAClient = false;
  connecting = false;
  recv = nullptr;
  sent = false;
  dataRecieved = false;

  connectStart = 0;
}

MyClient::~MyClient()
{
  if(client)
  {
    delete client;
    client = nullptr;
  }
}


void MyClient::setup_ethernet()
{
Serial.println("Starting Ethernet...");


      bool start = ETH.begin();

      if (!start)
      {
        Serial.println("ETH.begin() returned false. Need diffrent arguments");
      }

 
      unsigned long waitTime = millis();
      Serial.println("Waiting for ethernet link");
      while(!ETH.linkUp() && (millis()-waitTime < 10000))
      {
        Serial.print(".");
        delay(200);
      }

      ETH.config(clientIP, subnet);

      if(ETH.linkUp())
      {
        Serial.print("Link is up, IP address is: ");
        Serial.println(ETH.localIP());
      } else
      {
        Serial.println("Bad link.");
      }

}

void MyClient::onConnect(void* arg, AsyncClient* c)
{
  Serial.println("TCP CONNECTED");
}

void MyClient::onDisconnect(void* arg, AsyncClient* c)
{
  Serial.println("TCP DISCONNECTED");
}

void MyClient::onError(void* arg, AsyncClient* c, int8_t error)
{
  Serial.printf("TCP ERROR: %d\n", error);

}

void MyClient::onData(void* arg, AsyncClient* c, void* data, size_t len)
{
    MyClient* self = static_cast<MyClient*>(arg);

    self->dataRecieved = false;

    self->recv = (uint8_t*)data;


    for (size_t i = 0; i < len; i++)
    {
      if (self->recv[i] != '\r')
      {
          self->message.concat((char)self->recv[i]);
      }
     
    }

   
  Serial.println(self->message);

  self->dataRecieved = true;

}



void MyClient::connectToServer()
{

    Serial.println("Connnecting to Server");

    sent = false;
    
    if (hasAClient)
    {
      delete client; 
      client = nullptr;
      hasAClient = false;
    }
    client = new AsyncClient();
    


    client->onConnect(onConnect, nullptr);
    client->onDisconnect(onDisconnect, nullptr);
    client->onError(onError, nullptr);
    client->onData(onData, this);


    if (!client->connect(serverIP,port))
    {
      connectStart = millis();
      Serial.println("Failed connection to Server");
      delete client;
      client = nullptr;
    }
    else
    {
      Serial.println("Connect sent to Server");
      hasAClient = true;
      connecting = true;
    }

}

void MyClient::sendToServer()
{
  if (client->connected())
  {
    Serial.println("Sending read to Server");
    String message = "DEW1\r";

    if (client->canSend())
    {
      client->write(message.c_str(),message.length());
    }
  }
  else
  {
    Serial.println("Client is not connected.");
  }
}



uint8_t* MyClient::getRecv(){

  return recv;
}


void MyClient::serverLoop()
{
  // if (client->disconnected() && !client->connecting())
  // {
     
  //   connectToServer();
  // }
  // else 
  if (!sent && client->connected())
  {
    sendToServer();
    sent = true;
  }
  else 
  {
    return;
  }
}
