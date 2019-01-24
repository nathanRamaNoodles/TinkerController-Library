/**
   Author: Nathan Ramanathan
   Additional Contributors: Gil Maimon
**/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

enum TinkerMode {
    TinkerMode_Default,
    TinkerMode_Streaming
};

struct TinkerEvent {
    bool ok;
    char type;
    int value;
};

class TinkerController {
public:
    TinkerController(TinkerMode mode = TinkerMode_Default) : _config("ESPTest", "", 4210, mode) {}

    void setup() {
        configureWiFi();
        logConnectionDetails();
        listenOnUdp();
    }

    IPAddress getIP() const {
        return WiFi.softAPIP();
    }

    unsigned getPort() const {
        return _config.udp_port;
    }

    TinkerEvent getEvent() {
        int packetSize = _udp.parsePacket();
        if(!packetSize) {
            TinkerEvent e;
            e.ok = false;
            return e;
        }

        if(_config.mode == TinkerMode_Streaming) {
            for (int i = 0; i < 5; i++) Serial.print((char)_udp.read());
            Serial.println();
            
            TinkerEvent e;
            e.ok = false;
            return e;
        }

        //When we get our message, we need to evaluate the message type(Is it a button? Is it a joystick?), then we need to get its value.
        char inputType = nextAlpha();
        int num = readValue();

        TinkerEvent e;
        e.ok = true;
        e.type = inputType;
        e.value = num;
        return e;
    }

private:
    struct Config {
        String ssid;
        String password;
        unsigned udp_port;

        TinkerMode mode;

        Config(String name, String pass, unsigned port, TinkerMode mode) :
            ssid(name), 
            password(pass), 
            udp_port(port),
            mode(mode) {

            // Empty Body
        }
    };

    WiFiUDP _udp; 
    const Config _config;

    // Init Helpers
    void configureWiFi() {
        ESP.eraseConfig();
        if (WiFi.status() == WL_CONNECTED) WiFi.disconnect();
        WiFi.softAP(_config.ssid.c_str(), _config.password.c_str());
    }
    void logConnectionDetails() {
        IPAddress myIP = getIP();
        Serial.print("AP IP address: ");
        Serial.println(myIP);
        Serial.print("Port: ");
        Serial.println(_config.udp_port);
    }
    void listenOnUdp() {
        _udp.begin(_config.udp_port);
    }

    // Loop Helpers
    char nextAlpha() {
        while(true) {
            char ch = _udp.read();
            if(isAlpha(ch)) return ch;
        }
    }

    char nextDigit() {
        while(true) {
            char ch = _udp.read();
            if(isdigit(ch)) return ch;
        }
    }

    int readValue() {
        char inputValue = nextDigit();
        int num = inputValue - '0';
        while (isdigit(inputValue = _udp.read())) {
            num = (num * 10) + (inputValue - '0');  //some magic
        }
        return num;
    }
};