#include <Arduino.h>
#include <cmd.h>

char nodeID = 'M';
String ascii = "     __  ___________  __                    \n"
                "   /  |/  / ____/ / / /                    \n"
                "  / /|_/ / /   / / / /                     \n"
                " / /  / / /___/ /_/ /                      \n"
                "/_/__/_/\\____/\\____/______________________ \n"
                "  / ____/ /   / / / / ___/_  __/ ____/ __ \\ \n"
                " / /   / /   / / / /\\__ \\ / / / __/ / /_/ / \n"
                "/ /___/ /___/ /_/ /___/ // / / /___/ _, _/  \n"
                "\\____/_____/\\____//____//_/ /_____/_/ |_|  ";
                                                          
String input = "";

void setup() {
  Serial.begin(115200);
  Serial.println(ascii);
  Serial.println("Node: " + String(nodeID));
  Serial.println("ESP32-S3");
  Serial.println("-----------------------------");
  Serial.println("Core: " + String(ESP.getCoreVersion()));
  Serial.println("Chip: " + String(ESP.getChipModel()));
  Serial.println("Cores: " + String(ESP.getChipCores()));
  Serial.println("Revision: " + String(ESP.getChipRevision()));
  Serial.println("CPU Freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
  Serial.println("Flash Size: " + String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB");
  Serial.println("Running: ");  
  
}

void loop() {
  if(Serial.available()){
    input = Serial.readStringUntil('\n');
    input.trim();
    if(input.equals("test")){
      test();
    }
  }

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}