#pragma once //compile once only
#include <Arduino.h>
#include <globals.h>
#include "driver/twai.h"

inline void test() {
    digitalWrite(16, HIGH);
    digitalWrite(35, HIGH);
    delay(200);
    digitalWrite(16, LOW);
    digitalWrite(35, HIGH);
    Serial.println("Test Success");
}

inline uint8_t ping(int n, int type){
    uint8_t liveNode = 0xFF;
    uint8_t targID = n;
    if(n == 0){
        Serial.println("Pinging master (self)...");
        liveNode = 0x01;
        return liveNode;
    }/*else if (type == 1){
        // get all node status via CAN bus

        twai_message_t msg;
        msg.identifier = 0x00; // Universal Master Command ID
        msg.extd = 0;
        msg.data_length_code = 4; // 4 bytes of data
        msg.data[0] = (uint8_t)n; // Byte 0 specifies destination (e.g., 3)
        msg.data[1] = 1;          // Byte 1 specifies command
        msg.data[2] = 0;
        msg.data[3] = 0;
        delay(50);
        twai_transmit(&msg, pdMS_TO_TICKS(100));
        //MESSAGE SENT
        //NOW WAIT FOR RESPONSE
        Serial.println("Pinging node " + (String)n + "...");
        twai_message_t rx_msg;
        if (twai_receive(&rx_msg, pdMS_TO_TICKS(50)) == ESP_OK){

            // check if this message was sent to this node via data[0] (the first byte of the message)
            uint8_t returnedID = rx_msg.data[1];
            liveNode = returnedID;
        }
            
    }*/else if (type == 0){
        //get node_n status via CAN bus
        twai_message_t msg;
        msg.identifier = 0x00;        // Universal Master Command ID
        msg.extd = 0;
        msg.data_length_code = 4; // 4 bytes of data
        msg.data[0] = targID;    // Byte 0 specifies destination (e.g., 3)
        msg.data[1] = 1;         // Byte 1 specifies command
        msg.data[2] = 0;  
        msg.data[3] = 0;  

        twai_transmit(&msg, pdMS_TO_TICKS(100));
        Serial.println("Pinging node " + (String)n + "...");
        //MESSAGE SENT
        //NOW WAIT FOR RESPONSE
        twai_message_t rx_msg;
        if (twai_receive(&rx_msg, pdMS_TO_TICKS(50)) == ESP_OK){

            // check if this message was sent to this node via data[0] (the first byte of the message)
            uint8_t returnedID = rx_msg.data[1];
            liveNode = returnedID;
        }
    }

    return liveNode;
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

        //convert to seconds to fit in byte format
        millis = millis/1000;

        twai_message_t msg;
        msg.identifier = 0x00;        // Universal Master Command ID
        msg.extd = 0;
        msg.data_length_code = 4; // 4 bytes of data
        msg.data[0] = (uint8_t)n;    // Byte 0 specifies destination (e.g., 3)
        msg.data[1] = 2;         // Byte 1 specifies command
        msg.data[2] = (uint8_t)millis;  
        msg.data[3] = 0;  

        twai_transmit(&msg, pdMS_TO_TICKS(100));


        Serial.println("Blink Sent to Node"+(String)n);
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

