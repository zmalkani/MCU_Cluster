#include <Arduino.h>
#include <cmd.h>
#include <globals.h>
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
boolean taskRunning = false;
boolean prompted = false;

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
bool isInt(const String& s) {
  if (s.length() == 0) return false;
  int i = 0;
  if (s[0] == '-' || s[0] == '+') {
    if (s.length() == 1) return false;
    i = 1;
  }
  for (; i < s.length(); i++) {
    if (!isDigit(s[i])) return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);

  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);

  serialSetup();
  
}

void loop() {
  if(!prompted){
    Serial.println("-> master: ");
    prompted = true;
  }
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

    //run mullptr checks via string tokenizer return
    if (p1 == nullptr || p2 == nullptr || p3 == nullptr) {
      Serial.println("Err: too few arguments. Format: <cmd> <arg> <flag>");
      return;
    }

    if (extra != nullptr) {
      Serial.println("Err: too many arguments. Format: <cmd> <arg> <flag>");
      return;
    }

    //set easily referencable strings to use in command logic
    cmd = String(p1);
    arg = String(p2);
    flag = String(p3);

    if(cmd.equals("ping")){ //ping
      prompted = false;
      if(isInt(arg)){
        Serial.println(""+(String)ping(arg.toInt(),0)); //ping node with type 0 (single node ping)
      }else if(arg.equals("all")){
        Serial.println(""+(String)ping(0,1));           //ping all nodes with type 1 (all node ping)
      }else if(arg.equals("help")){
        Serial.println("Functionality: ping nodes 1-6 or all nodes on CAN bus");
        Serial.println("Input Format: ping <n#> <->");
        Serial.println("Ex1: ping 1 -");
        Serial.println("Ex2: ping all -");
        Serial.println("Output: Returns byte with node status: 0 = offline, 1 = online");
        Serial.println("Output Ex: 0b01111111 = all nodes online \n0b00000001 = master online\n0b00000010 = node1 online \n 0b10000000 = input/logic error");
      }else{
        Serial.println("Err: argument '"+arg+"' unknown");
      }

    }else if(cmd.equals("blink")){ // blink
      prompted = false;
      if(arg.equals("master")){
        blink(0,flag.toInt());
      }else{
        blink(arg.toInt(), flag.toInt());
      }

    }else if(cmd.equals("status")){ //status
      prompted = false;
      byte statusByte = 0b00000000;
      if(arg.equals("master") || arg.equals("0")){ //valid args: master|0, #, all
        statusByte = status(0);
        
      }else if(isInt(arg)){ //confirm int arg
        int statusArray [8]; // 8 index array
        statusByte = status(arg.toInt()); //fetch status with arg value

        for(int i = 0; i < 8; i++){ //convert statusByte into array
          statusArray[i] = (statusByte >> (7 - i) & 0x01); //
        }

        if(statusArray[0] == 1){
          Serial.println("Err: 0b10000000 \n command fetched master node status or node not live. ping to verify node health");
        }else if(statusArray[1] == 1){
          Serial.println("Node "+arg+" currently idle.");
        }else{
          Serial.print(task1+": ");
          if(statusArray[7]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task2+": ");
          if(statusArray[6]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task3+": ");
          if(statusArray[5]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task4+": ");
          if(statusArray[4]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task5+": ");
          if(statusArray[3]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task6+": ");
          if(statusArray[2]==1){Serial.println("live");}else{Serial.println("inactive");}

        }

      }else if(arg.equals("all")){
        for (int i = 1; i <= 7; i++){
          int statusArray [8]; // 8 index array
          statusByte = status(i);

          for(int i = 0; i < 8; i++){ //convert statusByte into array
          statusArray[i] = (statusByte >> (7 - i) & 0x01); //
          }

          if(statusArray[0] == 1){
          Serial.println("Err: 0b10000000 \n command fetched master node status or node not live. ping to verify node health");
          }else if(statusArray[1] == 1){
          Serial.println("Node "+(String)i+" currently idle.");
          }else{
          Serial.print(task1+": ");
          if(statusArray[7]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task2+": ");
          if(statusArray[6]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task3+": ");
          if(statusArray[5]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task4+": ");
          if(statusArray[4]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task5+": ");
          if(statusArray[3]==1){Serial.println("live");}else{Serial.println("inactive");}
          Serial.print(task6+": ");
          if(statusArray[2]==1){Serial.println("live");}else{Serial.println("inactive");}
          }
        }
      }  
    }else{
      prompted = false;
      Serial.println("Err: command '"+cmd+"' unknown");
    }   
  }
}
