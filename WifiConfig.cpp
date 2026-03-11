#include "WifiConfig.h"
#include <Arduino.h>


WifiConfig::WifiConfig(){
    WifiName = "";
    password = "";
    connected = false;
     
}

WifiConfig::~WifiConfig(){

}


void WifiConfig::start_connect(){
    delay(10);

    Serial.println();
    Serial.println("Connecting to");
    Serial.println(WifiName);


    WiFi.mode(WIFI_STA);
    WiFi.begin(WifiName,password);

     while(WiFi.status() != WL_CONNECTED){
            delay(500);
            Serial.print(".");
        }

    Serial.println("");
    Serial.println("WiFi Connetcted");
    Serial.println("IP Address:");
    Serial.println(WiFi.localIP());
    connected = true;
}


void WifiConfig::set_wifi_credentials(const char* ssid, const char* pass){   
    WifiName = ssid;
    password = pass;

}