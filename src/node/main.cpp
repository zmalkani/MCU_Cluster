#include <Arduino.h>
#include <globals.h>
#include <cstring>

#include <Wire.h>

constexpr uint8_t nodeID = 0x06;
constexpr uint8_t nodeAddress = 0x06;

bool taskRunning = false;

uint8_t receivedCommand = 0; 
uint8_t response[8];
uint8_t responseLength = 0;
bool commandReady = false;

void receiveCommand(int bytes){
  if (bytes < 1) return;
  receivedCommand = Wire.read();

  while(Wire.available()){
    Wire.read(); // discard any extra bytes
  }

  commandReady = true;
}

void sendResponse() {
  Wire.write(response, responseLength);
}

void setup() {

  Serial.begin(115200);
  Serial.println("Node ID: " + String(nodeID));
  Wire.begin(13,14); // init i2c bus
  Wire.onRequest(sendResponse);

  pinMode(16, OUTPUT); // blink/utility LED
  pinMode(17, OUTPUT); // status LED
  pinMode(18, OUTPUT); // error LED
  pinMode(35, OUTPUT);
  //startup status blinks
  for (int i =0; i < 3; i++){
    digitalWrite(18, HIGH);
    delay(100);
    digitalWrite(18, LOW);
    
    digitalWrite(17, HIGH);
    delay(100);
    digitalWrite(17, LOW);

    digitalWrite(16,HIGH);
    delay(100);
    digitalWrite(16, LOW);
 
  }
}

void loop() {
 

  //scan for messages with a 50ms timeout
  if (twai_receive(&rx_msg, pdMS_TO_TICKS(50)) == ESP_OK) {

    //step1: Check if this message was sent to this node via data[0] (the first byte of the message)
    if (rx_msg.data[0] == nodeID) {
      Serial.println("Got frame, dest=" + String(rx_msg.data[0]) + " cmd=" + String(rx_msg.data[1]));
      
      //step2: Read the command stored in data[1]
      uint8_t cmd = rx_msg.data[1];
      uint8_t arg = rx_msg.data[2]; 
      uint8_t flag = rx_msg.data[3]; 

      uint16_t blinkDuration = arg * 1000; // Convert seconds to milliseconds

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
        case 1: // ping command
          
          
          msg.identifier = nodeID;       
          msg.extd = 0;
          msg.data_length_code = 4; // 4 bytes of data
          msg.data[0] = 0x00;    //return to sender!
          msg.data[1] = nodeID;  // returns this nodesID, meaning this node is in fact live
          msg.data[2] = 0;
          msg.data[3] = 0;
          twai_transmit(&msg, pdMS_TO_TICKS(100));
          digitalWrite(17,HIGH);
          delay(100);
          digitalWrite(17,LOW);
          break;
        
        case 2: //blink command
          digitalWrite(16, HIGH);
          delay(blinkDuration);
          digitalWrite(16, LOW);
          break;
        /*
        case 3:
          ------("Command Received: Turning LED OFF");
          digitalWrite(LED_BUILTIN, LOW);
          break;

        case 4:
          ------("Command Received: Rebooting...");
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

