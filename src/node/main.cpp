#include <Arduino.h>
#include <globals.h>
#include <cstring>

#include "driver/twai.h"

char nodeID = 0x01;

boolean taskRunning = false;

void setup() {

  // NOTE: GPIO 43/44 are UART0 (serial monitor + flashing) - do NOT use for TWAI
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_4, GPIO_NUM_5, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS(); // Set bus speed to 125 kbps
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  
  // Install and start TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    Serial.println("TWAI Driver installed successfully.");
  } else {
    Serial.println("Failed to install TWAI driver.");
    return;
  }

  if (twai_start() == ESP_OK) {
    Serial.println("TWAI Driver started successfully.");
  } else {
    Serial.println("Failed to start TWAI driver.");
    return;
  }




  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(35, OUTPUT);
  //startup status blinks
  for (int i =0; i < 3; i++){
    digitalWrite(16,HIGH);
    delay(100);
    digitalWrite(17,HIGH);
    delay(100);
    digitalWrite(18,HIGH);
    delay(100);
    digitalWrite(35,HIGH);
    delay(100);
    //turn all off
    digitalWrite(16,LOW);
    digitalWrite(17,LOW);
    digitalWrite(18,LOW);
    digitalWrite(35,LOW);
  }
}

void loop() {
  twai_message_t rx_msg;

  //scan for messages with a 50ms timeout
  if (twai_receive(&rx_msg, pdMS_TO_TICKS(50)) == ESP_OK) {

    //step1: Check if this message was sent to this node via data[0] (the first byte of the message)
    if (rx_msg.data[0] == nodeID) {
      
      //step2: Read the command stored in data[1]
      uint8_t cmd = rx_msg.data[1];
      uint8_t arg = rx_msg.data[2]; 
      uint8_t flag = rx_msg.data[3]; 

      /*
     NOTE FOR FUTURE ZACH: Remember that this is setup right now
      to expect 4 bytes in each msg. HOWEVER, if not all 4 bytes
      are sent, the rest will be shitty noise or something. So 
      if you are getting issues with that, switch to a case by 
      case reading basis instead of this current solutiom.
      */
      //step3: repeat what happened when it was typed in on master node!
      twai_message_t msg; //declare msg once and use for all cases
      switch (cmd) {
        case 1:
          
          
          msg.identifier = nodeID;       
          msg.extd = 0;
          msg.data_length_code = 4; // 4 bytes of data
          msg.data[0] = 0x00;    //return to sender!
          msg.data[1] = nodeID;         // returns this nodesID, meaning this node is in fact live
          msg.data[2] = 0;
          msg.data[3] = 0;
          twai_transmit(&msg, pdMS_TO_TICKS(100));
          digitalWrite(17,HIGH);
          delay(100);
          digitalWrite(17,LOW);
          break;
        /*
        case 2:
          Serial.println("Command Received: Turning LED ON");
          digitalWrite(LED_BUILTIN, HIGH);
          break;
        
        case 3:
          Serial.println("Command Received: Turning LED OFF");
          digitalWrite(LED_BUILTIN, LOW);
          break;

        case 4:
          Serial.println("Command Received: Rebooting...");
          ESP.restart();
          break;
        */
        default:
          digitalWrite(18, HIGH);
          delay(5000);
          digitalWrite(18, LOW);
          msg.identifier = nodeID;   
          msg.extd = 0;
          msg.data_length_code = 4; // 4 bytes of data
          msg.data[0] = 0x00;    //return to sender!
          msg.data[1] = 0xFF;    //error code for unknown command
          msg.data[2] = nodeID;  //returns this nodesID, meaning this node is in fact live
          msg.data[3] = 0;
          twai_transmit(&msg, pdMS_TO_TICKS(100));
          break;
      }
    }
  }
}

