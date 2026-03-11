#include "WifiConfig.h"
#include "MQTTClient.h"
#include <Arduino.h>
#include "TCPClient.h"



// const char* WifiName = "AndroidAP5C32";
// const char* password = "12345678";
// const char* mqtt_server = "10.116.207.225";



WifiConfig wifi;
PubSubClient mqtt_client(wifi.espClient);
MQTTClient mqtt(mqtt_client,"192.168.1.21");
TCPClientClass TCPClient;





//Setup and main
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 
  
  

  wifi.set_wifi_credentials("ZTE_H168N939DEB", "ffakbx5y");

  // wifi.set_wifi_credentials("AndroidAP5C32", "12345678");

  wifi.start_connect();
  mqtt_client.setServer(mqtt.mqtt_server,1884);
  mqtt_client.setCallback(mqtt.callback);
  mqtt.mqttSetup();

 



  TCPClient.setup_ethernet();


  


  

 
}

void loop() {
  // put your main code here, to run repeatedly:

  TCPClient.setMqttState(mqtt.mqttClientState);
  TCPClient.cyclicLogic();
  mqtt.cyclicLogic();


  if (TCPClient.dataRecieved){
    mqtt.sendToBroker(TCPClient.message);
  }

  
}
