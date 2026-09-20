#include <Arduino.h>
#include <globals.h>
#include <cstring>

#include <Wire.h>

constexpr uint8_t nodeID = 0x06;
constexpr uint8_t nodeAddress = 0x06;

bool taskRunning = false;

//shared with the i2c ISR callbacks - must be volatile
volatile uint8_t receivedCommand = 0;
volatile uint8_t receivedArg = 0;
volatile uint8_t receivedFlag = 0;
uint8_t response[8];
volatile uint8_t responseLength = 0;
volatile bool commandReady = false;

void receiveCommand(int bytes){
  if (bytes < 1) return;
  receivedCommand = Wire.read();

  if (Wire.available()) receivedArg = Wire.read();
  if (Wire.available()) receivedFlag = Wire.read();

  while(Wire.available()){
    Wire.read(); // discard any extra bytes
  }

  commandReady = true;
}

void sendResponse() {
  Wire.write(response, responseLength);
}

void setup() {
  pinMode(16, OUTPUT); // blink/utility LED
  pinMode(17, OUTPUT); // status LED
  pinMode(18, OUTPUT); // error LED
  pinMode(35, OUTPUT);




  Serial.begin(115200);
  delay(500); //brief window for a serial monitor to catch the print, then boot regardless
  Serial.println("Node ID: " + String(nodeID));

  //init i2c bus as slave with this node's address.
  //i2cSlaveInit() checks the SDA/SCL line levels first and FAILS (returns false)
  //if either line is low: master powered off/unplugged, no pull-ups, crossed
  //wires, or a stuck bus. Retry until the bus comes up so power-up order
  //doesn't silently leave the node off the bus - blink the error LED meanwhile.
  //4-arg overload = slave mode: address, sda, scl, frequency.
  //Wire.begin(13, 14) would match the master overload and the node
  //would never ACK any address - master scan would find nothing.
  while (!Wire.begin(nodeAddress, 13, 14, 100000)) {
    Serial.println("I2C slave init failed - check SDA/SCL wiring, pull-ups, shared GND. Retrying...");
    digitalWrite(18, HIGH);
    delay(150);
    digitalWrite(18, LOW);
    delay(850);
  }
  Serial.println("I2C slave up: addr=0x" + String(nodeAddress, HEX) + " SDA=13 SCL=14");
  //bus clock is set by the master - setClock() in slave mode only logs an error
  Wire.onReceive(receiveCommand);
  Wire.onRequest(sendResponse);

  //startup status blinks - first, so the node shows life on ANY power
  //source (boot is never gated on USB being attached)
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

  
  //pre-stage ping response so the master's first requestFrom always has data,
  //even if it arrives before loop() processes the command
  response[0] = nodeID;
  responseLength = 1;
}

void loop() {

  //process a command when the i2c receive ISR flags one as ready
  Serial.println("Awaiting command...");
  if (commandReady) {
    commandReady = false;

    Serial.println("Got command, cmd=" + String(receivedCommand));

    //step1: Read the command, arg, and flag captured by receiveCommand
    uint8_t cmd = receivedCommand;
    uint8_t arg = receivedArg;
    uint8_t flag = receivedFlag;

    uint16_t blinkDuration = arg * 1000; // Convert seconds to milliseconds

    //step2: repeat what happened when it was typed in on master node!
    switch (cmd) {
      case 1: // ping command
        response[0] = nodeID; // returns this nodesID, meaning this node is in fact live
        responseLength = 1;
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
        response[0] = 0xFF; //error code for unknown command
        responseLength = 1;
        break;
    }
  }
}

