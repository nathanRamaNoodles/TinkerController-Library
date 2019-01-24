/**
   Author: Nathan Ramanathan
   Project link: https://github.com/nathanRamaNoodles/Tinker-Controller

   Directions: Hook up an LED to pin D7 and D8.  Next connect the controller to the app, and move the left joystick.  You should
   notice that the LEDs will fade according to the position of your controller's joystick.
**/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SensorToButton.h>   //https://github.com/nathanRamaNoodles/SensorToButton

#define LED D8
#define LED_2 D7

struct Config {
    String ssid;
    String password;
    unsigned udp_port;
    
    enum Mode {
        Mode_Default,
        Mode_Streaming
    };

    Mode mode;

    Config(String name, String pass, unsigned port, Mode mode = Mode_Default) :
        ssid(name), 
        password(pass), 
        udp_port(port),
        mode(mode) {

        // Empty Body
    }
};

class TinkerController {
public:
    TinkerController(Mode mode = Mode_Default) : _config("ESPTest", "", 4210, mode) {
        configureWiFi();
        logConnectionDetails();
        listenOnUdp();

        // Get ready to use LED as output
        pinMode(LED, OUTPUT);
    }

    IPAddress getIP() const {
        return WiFi.softAPIP();
    }

    unsigned getPort() const {
        return _config.udp_port;
    }

    void loop() {
        int packetSize = _udp.parsePacket();
        if(!packetSize) return;

        if(_config.mode == Config::Mode_Streaming) {
            for (int i = 0; i < 5; i++) Serial.print((char)_udp.read());
            Serial.println();
            return;
        }

        //When we get our message, we need to evaluate the message type(Is it a button? Is it a joystick?), then we need to get its value.
        char inputType = nextAlpha();
        int num = readValue();

        useGameData(inputType, num);
    }

private:
    WiFiUDP _udp;
    config 
    const Config _config;

    // Init Helpers
    void configureWiFi() {
        ESP.eraseConfig();
        if (WiFi.status() == WL_CONNECTED) WiFi.disconnect();
        WiFi.softAP(_config.ssid, _config.password);
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

int main() {
    TinkerController controller;

    Event e = controller.getEvent();
    

    controller.onEvent(Key::A, [](Event e){
        if(e == Event::A_ON) {

        }

        if(e.key == Key::A) {
            e.value
        }
    });
}

/////////////--------------------------------Process Game Controller Data------------------------------------------------------------/////////////////////
/*I know the naming of chars are weird, but I made them up based on the QWERTY system on my keyboard, So i looked at my controller and typed
  Right to left and then top to bottom*/

void useGameData(char type, int value) {
  yield();
  switch (type) {
    //A Button
    case 'a':
      switch (value) {  //use switch statements if you aren't printing to Serial.
        case 1:
          Serial.println("A on");
          digitalWrite(LED, LOW);
          break;
        case 0:
          Serial.println("A off");
          digitalWrite(LED, HIGH);
          break;
      }
      break;
    //B Button
    case 'p':
     switch (value) {  //use switch statements if you aren't printing to Serial.
        case 1:
          Serial.println("B on");
          break;
        case 0:
          Serial.println("B off");
          break;
      }
      break;
    //Y button
    case 'o':
      switch (value) {  //use switch statements if you aren't printing to Serial.
        case 1:
          Serial.println("Y on");
          break;
        case 0:
          Serial.println("Y off");
          break;
      }
      break;

    //You can do this for the other buttons...but I don't have to because its boring :)

    //X Button
    case 'i':
     switch (value) {  //use switch statements if you aren't printing to Serial.
        case 1:
          Serial.println("X on");
          break;
        case 0:
          Serial.println("X off");
          break;
      }
      break;

    //-------------Dpad Buttons------------//
    case 'd':
      switch (value) {
        //Dpad Center Button
        case 0:
          Serial.println("D-pad Center");
          break;
        //Dpad Left Button
        case 1:
          Serial.println("D-pad Left");
          break;
        //Dpad Up Button
        case 2:
          Serial.println("D-pad Up");
          break;
        //Dpad Right Button
        case 3:
          Serial.println("D-pad Right");
          break;
        //Dpad Down Button
        case 4:
          Serial.println("D-pad Down");
          break;
      }
      break;


    //-------------Function Buttons------------//
    //Back Button
    case 'y':
      break;
    //Start Button
    case 'u':
      break;

    //-------------Joystick Buttons------------//
    //Left Joystick Button
    case 't':
      break;
    //Right Joystick Button
    case 'j':
      break;

    //-------------Trigger Buttons------------//
    //Top left trigger Button
    case 'w':
      break;
    //Top right trigger Button
    case 'e':
      break;
    //Bottom left Trigger(Some controllers have this as analog so 355 means on)
    case 'q':
      switch (value) {
        //        case 355:
        //          digitalWrite(LED, LOW);
        //          break;
        //        case 0:
        //          digitalWrite(LED, HIGH);
        //          break;
        default:
          //analog stuff handled here
          break;
      }
      break;
    //Bottom Right Trigger(Some controllers have this as analog so 355 means on)
    case 'r':
      switch (value) {
        //        case 355:
        //          digitalWrite(LED, LOW);
        //          break;
        //        case 0:
        //          digitalWrite(LED, HIGH);
        //          break;
        default:
          //analog stuff handled here
          break;
      }
      break;


    //-------------Analog------------//
    //Analog values(Joysticks) are under construction.  There's an annoying method i have to deal with in Android
    //(Every joystick movement is sent out to the esp8266 which causes too many packets to reset the esp).
    /*The range for these values is 100-355.  I chose these ranges because Android can't sent UDP packets of different length. I
      wanted a range of 0-255, so I simply bumped it up by adding 100. And it works great and smoothly for Android*/

    //left joystick X direction
    case 'k':
      analogWrite(LED, value - 100);  /*Subtract by 100 since the incoming data has a range from 100-355, and Arduino's analogWrite() only accepts 0-255*/
      break;
    //left joystick Y direction
    case 'l':
      analogWrite(LED_2, value - 100);
      break;
    //Right joystick X direction
    case 'z':
      break;
    //Right joystick Y direction
    case 'x':
      break;
  }
  yield();
}