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
  TCPClient.stFromMQTT.commandRecieved = &mqtt.stToTCP.command;
  TCPClient.stFromMQTT.valueRecieved = &mqtt.stToTCP.inputValue;
  mqtt.stFromTCP.sensorValueRecieved = &TCPClient.stToMQTT.sensorValue;
  mqtt.stFromTCP.writingDone = &TCPClient.stToMQTT.writingDone;
  mqtt.stFromTCP.connectionStateRecieved = &TCPClient.stToMQTT.connectionState;
  

  // wifi.set_wifi_credentials("ZTE_H168N939DEB", "ffakbx5y");

  wifi.set_wifi_credentials("AndroidAP5C32", "12345678");

  wifi.start_connect();
  mqtt_client.setServer(mqtt.mqtt_server,1884);
  mqtt_client.setCallback(mqtt.callback);

 

 
}

void loop() {
  // put your main code here, to run repeatedly:
  mqtt.cyclicLogic();


  if (mqtt.connected){
    TCPClient.cyclicLogic();
  }

  if (mqtt.stToTCP.send){
    mqtt.stToTCP.send = false;
    TCPClient.stFromMQTT.recieved = true;
    TCPClient.send();
  }


  if(TCPClient.stToMQTT.send){
    TCPClient.stToMQTT.send = false;

    if (*TCPClient.stFromMQTT.commandRecieved == "read"){
        mqtt.publish("plc/sensorValue", *mqtt.stFromTCP.sensorValueRecieved);
    }else if (*TCPClient.stFromMQTT.commandRecieved == "write") {
       
       mqtt.publish("plc/writing", *mqtt.stFromTCP.writingDone);
    }
    
  
    mqtt.stFromTCP.recieved = true;
   
  }
}