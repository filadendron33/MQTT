#ifndef _WIFI_CONFIG_H_
#define _WIFI_CONFIG_H_
#include <WiFi.h>



class WifiConfig{


    public:
        WifiConfig();
        ~WifiConfig();
        WiFiClient espClient;
        void start_connect();
        void set_wifi_credentials(const char* ssid, const char* pass);



    private:
        const char* WifiName;
        const char* password;

};

#endif