#include <WifiConfig.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <TCPClient.h>



// const char* WifiName = "AndroidAP5C32";
// const char* password = "12345678";
// const char* mqtt_server = "10.116.207.225";
bool emptyMessage;



wifiSetup wifi;
MQTTClient mqtt(wifi.espClient);
// PubSubClient mqtt_client(wifi.espClient);
MyClient mojClient;



//MQTT Code
// void setup_wifi()
// {
//   delay(10);

//   Serial.println();
//   Serial.println("Connecting to");
//   Serial.println(WifiName);


//   WiFi.mode(WIFI_STA);
//   WiFi.begin(WifiName,password);

//   while(WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.println("WiFi Connetcted");
//   Serial.println("IP Address:");
//   Serial.println(WiFi.localIP());

// }








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
  if(!mqtt_client.connected())
  {
      connectToMqtt(mojClient.message);
   }
  mqtt_client.loop();
}
  
}
