#pragma once //compile once only
#include <Arduino.h>
#include <globals.h>

inline void test() {
    digitalWrite(16, HIGH);
    digitalWrite(35, HIGH);
    delay(200);
    digitalWrite(16, LOW);
    digitalWrite(35, HIGH);
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
        digitalWrite(35, HIGH);
        delay(millis);
        digitalWrite(16, LOW);
        digitalWrite(35, LOW);
        Serial.println("Blink Success");
    }else{
        Serial.println("Blinking node "+(String)n+"...");
        //send blink command to node_n via CAN bus
        Serial.println("Blink Success");
    }
}

inline byte status(int n){
    byte statusByte = 0b10000000;
    /*
    status return type breakdown:
    0b00000000
    0b      0      0       0       0       0       0       0       0
           err    idle  task6  task5    task4   task3   task2   task1
    */
    if(n == 0){
        return 0b01000000;
    }else{
        //send status request to node "n" ID via CAN bus and return status byte
        return statusByte;
    }
    
}

