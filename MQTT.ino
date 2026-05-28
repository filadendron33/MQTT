#include "WifiConfig.h"
#include "MQTTClient.h"
#include <Arduino.h>
#include "TCPClient.h"





WifiConfig wifi;
PubSubClient mqtt_client(wifi.espClient);
MQTTClient mqtt(mqtt_client,"192.168.1.15");
TCPClientClass TCPClient;





//Setup and main
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  TCPClient.RecieveFromMQTT.pCommandRecieved = &mqtt.SendToTCP.sCommand;
  TCPClient.RecieveFromMQTT.pValueRecieved = &mqtt.SendToTCP.sInputValue;
  mqtt.RecieveFromTCP.pSensorValueRecieved = &TCPClient.SendToMQTT.sSensorValue;
  mqtt.RecieveFromTCP.pWritingDone = &TCPClient.SendToMQTT.sWritingDone;
  mqtt.RecieveFromTCP.pConnectionStateRecieved = &TCPClient.SendToMQTT.sConnectionState;
  

  wifi.set_wifi_credentials("ZTE_H168N939DEB", "ffakbx5y");

  // wifi.set_wifi_credentials("AndroidAP5C32", "12345678");

  wifi.start_connect();
  mqtt_client.setServer(mqtt.Client.pIPAddres,1884);
  mqtt_client.setCallback(mqtt.Callback);

 

 
}

void loop() {
  // put your main code here, to run repeatedly:
  mqtt.CyclicLogic();


  if (mqtt.Client.bConnected){
    TCPClient.CyclicLogic();
  }

  if (mqtt.SendToTCP.bSend){
    mqtt.SendToTCP.bSend = false;
    TCPClient.RecieveFromMQTT.bRecieved= true;
    TCPClient.Send();
  }


  if(TCPClient.SendToMQTT.bSend){
    TCPClient.SendToMQTT.bSend = false;

    if (*TCPClient.RecieveFromMQTT.pCommandRecieved == "read"){
        mqtt.Publish("plc/sensorValue", *mqtt.RecieveFromTCP.pSensorValueRecieved);
    }else if (*TCPClient.RecieveFromMQTT.pCommandRecieved == "write") {
       
       mqtt.Publish("plc/writing", *mqtt.RecieveFromTCP.pWritingDone);
    }
    
  
    mqtt.RecieveFromTCP.bRecieved = true;
   
  }
}