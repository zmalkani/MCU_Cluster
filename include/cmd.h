#pragma once
#include <Arduino.h>

inline void test() {
    digitalWrite(16, HIGH);
    delay(200);
    digitalWrite(16, LOW);
    Serial.println("Test Success");
    

}