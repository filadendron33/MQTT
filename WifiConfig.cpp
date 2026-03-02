#include "WifiConfig.h"
#incldue <Arduino.h>


wifiSetup::wifiSetup(){
    WifiName = nullptr;
    password = nullptr;
}

wifiSetup::~wifiSetup(){

}


void wifiSetup::start_connect()
{
    delay(10);

    Serial.println();
    Serial.println("Connecting to");
    Serial.println(WifiName);


    WiFi.mode(WIFI_STA);
    WiFi.begin(WifiName,password);

     while(WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }

    Serial.println("");
    Serial.println("WiFi Connetcted");
    Serial.println("IP Address:");
    Serial.println(WiFi.localIP());
}


wifiSetup::set_wifi_credentials(const char* ssid, const char* pass)
{   
    WifiName = ssid;
    password = pass;

}