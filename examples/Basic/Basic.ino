#include <TinkerController.h>

TinkerController controller;
void setup() {
    Serial.begin(115200);
    Serial.println("Begin");
    controller.setup();
}

void loop() {
    TinkerEvent e = controller.getEvent();
    
    // Event Occured?
    if(e.ok) {
        char eventType = e.type;
        int value = e.value;

        Serial.print("Event type: ");
        Serial.println(eventType);
        Serial.print("Value: ");
        Serial.println(value);
    }
}