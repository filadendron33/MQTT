#include "WifiConfig.h"
#include "MQTTClient.h"
#include <Arduino.h>
#include "TCPClient.h"





WifiConfig wifi;
PubSubClient mqtt_client(wifi.espClient);
MQTTClient mqtt(mqtt_client,"10.85.49.225");
TCPClientClass TCPClient;





//Setup and main
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  TCPClient.RecieveFromMQTT.pCommandRecieved = &mqtt.stToTCP.command;
  TCPClient.RecieveFromMQTT.pValueRecieved = &mqtt.stToTCP.inputValue;
  mqtt.stFromTCP.sensorValueRecieved = &TCPClient.SendToMQTT.sensorValue;
  mqtt.stFromTCP.writingDone = &TCPClient.SendToMQTT.writingDone;
  mqtt.stFromTCP.connectionStateRecieved = &TCPClient.SendToMQTT.connectionState;
  

  // wifi.set_wifi_credentials("ZTE_H168N939DEB", "ffakbx5y");

  wifi.set_wifi_credentials("AndroidAP5C32", "12345678");

  wifi.start_connect();
  mqtt_client.setServer(mqtt.mqtt_server,1884);
  mqtt_client.setCallback(mqtt.Callback);

 

 
}

void loop() {
  // put your main code here, to run repeatedly:
  mqtt.CyclicLogic();


  if (mqtt.connected){
    TCPClient.CyclicLogic();
  }

  if (mqtt.stToTCP.Send){
    mqtt.stToTCP.Send = false;
    TCPClient.RecieveFromMQTT.recieved = true;
    TCPClient.Send();
  }


  if(TCPClient.SendToMQTT.Send){
    TCPClient.SendToMQTT.Send = false;

    if (*TCPClient.RecieveFromMQTT.pCommandRecieved == "read"){
        mqtt.Publish("plc/sensorValue", *mqtt.stFromTCP.sensorValueRecieved);
    }else if (*TCPClient.RecieveFromMQTT.pCommandRecieved == "write") {
       
       mqtt.Publish("plc/writing", *mqtt.stFromTCP.writingDone);
    }
    
  
    mqtt.stFromTCP.recieved = true;
   
  }
}