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

void connectToMqtt(String value){
  

  while (!mqtt_client.connected())
  {
     
    String clientID = "MQTTClient - ";
    clientID += String(random(0,100));

    Serial.println("Trying to connect to MQTT Server with " + clientID);


    if (mqtt_client.connect(clientID.c_str())) {

      Serial.println("Connected to MQTT Server with " + clientID);



       if (mqtt_client.subscribe("SensorTopic"))
       {
          Serial.println("Successfully subscribed to topic");
       }

      if(mqtt_client.publish("SensorTopic", value.c_str())){
          Serial.println ("Message with value " + value + " should be sent. Wait for callback");
      }

      

      

    }
    else {
      Serial.println("Didn't connect. Try again.");
      Serial.println(mqtt_client.state());
      Serial.println("Try again in 5 sec ");

      delay(5000);

    }
  }

}






//Setup and main
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 
  
  

  wifi.set_wifi_credentials("ArduinoAP532C5", "12345678");

  wifi.start_connect();


  mojClient.setup_ethernet();
  mojClient.connectToServer();

  mqtt_client.setServer(mqtt_server, 1884);
  mqtt_client.setCallback(callback);
  

 
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
