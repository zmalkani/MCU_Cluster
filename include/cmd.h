#pragma once
#include <Arduino.h>

inline void test() {
    digitalWrite(16, HIGH);
    delay(200);
    digitalWrite(16, LOW);
    Serial.println("Test Success");
    

}

inline int ping(int n, int type){
    byte liveNodes = 0b00000000;
    if(n == 0){
        Serial.println("Pinging master (self)...");
        liveNodes = 0b00000001;
        return liveNodes;
    }else if(type == 1){
        //get all node status via CAN bus
        Serial.println("Pinging node "+(String)n+"...");
    }else if(type == 0){
        //get node_n status via CAN bus
        Serial.println("Pinging all nodes..");
    }else{
        liveNodes = 0b10000000;
    }
    return liveNodes;
}

inline void blink(int n, int millis){
    if(n == 0){
        Serial.println("Blinking master node...");
        digitalWrite(16, HIGH);
        delay(millis);
        digitalWrite(16, LOW);
        Serial.println("Blink Success");
    }else{
        Serial.println("Blinking node "+(String)n+"...");
        //send blink command to node_n via CAN bus
        Serial.println("Blink Success");
    }
}