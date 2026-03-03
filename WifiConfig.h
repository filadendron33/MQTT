#include WiFi.h



class wifiSetup{


    public:
        wifiSetup();
        ~wifiSetup();
        WiFiClient espClient;
        void start_connect();
        void set_wifi_credentials(const char* ssid, const char* pass);



    private:
        const char* WifiName;
        const char* password;

};