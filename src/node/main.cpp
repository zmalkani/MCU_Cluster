#include <Arduino.h>
#include <globals.h>
#include <cstring>

#include "driver/twai.h"

char nodeID = '1';

boolean taskRunning = false;

void setup() {
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
  
}

