#include <Arduino.h>
#include <cmd.h>
#include <cstring> 

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
String cmd = "";
String arg = "";
String flag = "";
String ext = "";

void serialSetup(){
  //serial setup output
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

void setup() {
  Serial.begin(115200);

  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);

  serialSetup();
  
}

void loop() {
  if(Serial.available()){
    input = Serial.readStringUntil('\n');
    input.trim();

    char buf[64];
    input.toCharArray(buf, sizeof(buf)); //input string to charArray

    //parse input array with string tokenizer and save as char* pointer (return type of strtrok)
    //strtok(<input-array-call0-only>, "<delimiter-char-(split location)>");
    char* p1 = strtok(buf, " ");
    char* p2 = strtok(nullptr, " ");
    char* p3 = strtok(nullptr, " ");
    char* extra = strtok(nullptr, " ");

    cmd = String(p1);
    arg = String(p2);
    flag = String(p3);
    ext = String(extra);

    //nullptr checks via string tokenizer return
    if (p1 == nullptr || p2 == nullptr || p3 == nullptr) {
      Serial.println("Err: too few arguments. Format: <cmd> <arg> <flag>");
      return;
    }

    if (extra != nullptr) {
      Serial.println("Err: too many arguments. Format: <cmd> <arg> <flag>");
      return;
    }

    if(input.equals("test")){
      test();
    }
  }

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

