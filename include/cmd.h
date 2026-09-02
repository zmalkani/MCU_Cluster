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
    if(type == 1){
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