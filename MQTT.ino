#include "WifiConfig.h"
#include "MQTTClient.h"
#include <Arduino.h>
#include "TCPClient.h"



// const char* WifiName = "AndroidAP5C32";
// const char* password = "12345678";
// const char* mqtt_server = "10.116.207.225";
bool emptyMessage;



WifiConfig wifi;
MQTTClient mqtt(wifi.espClient,"10.116.207.225");
MyClient mojClient;





//Setup and main
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 
  
  

  wifi.set_wifi_credentials("ArduinoAP532C5", "12345678");

  wifi.start_connect();


  mojClient.setup_ethernet();
  mojClient.connectToServer();

  // mqtt_client.setServer(mqtt_server, 1884);
  // mqtt_client.setCallback(callback);
  

 
}

void loop() {
  // put your main code here, to run repeatedly:
  mojClient.serverLoop();



if (mojClient.dataRecieved){
  if(!mqtt.mqtt_client.connected())
  {
      mqtt.connectToMqtt(mojClient.message);
   }
  mqtt.mqtt_client.loop();
}
  
}
