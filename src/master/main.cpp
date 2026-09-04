#include <Arduino.h>
#include <cmd.h>
#include <globals.h>
#include <cstring> 

#include "driver/twai.h"

char nodeID = 0x00;
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
uint8_t lnDelay = 100;


//CAN defs
unsigned long lastTransmitTime = 0;
uint8_t counter = 0;


//terminal print functions with microdelay
void terminalPrintln(String i){
  delay(100);
  Serial.println(i);
}

void terminalPrint(String i){
  delay(100);
  Serial.print(i);
}

void serialSetup(){
  //serial setup output
  Serial.println(ascii);
  Serial.println("Node: " + String(nodeID));
  Serial.println("ESP32-S3");
  Serial.println("----------------------------------------------");
  Serial.println("Core: " + String(ESP.getCoreVersion()));
  Serial.println("Chip: " + String(ESP.getChipModel()));
  Serial.println("Cores: " + String(ESP.getChipCores()));
  Serial.println("Revision: " + String(ESP.getChipRevision()));
  Serial.println("CPU Freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
  Serial.println("Flash Size: " + String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB");
  Serial.println("Running: ");  
  Serial.println("----------------------------------------------");  
}
bool isInt(const String& s) {
  if (s.length() == 0) return false; // int must be existent to return 1
  int i = 0;
  if (s[0] == '-' || s[0] == '+') { //negative/positive
    if (s.length() == 1) return false;
    i = 1;
  }
  for (; i < s.length(); i++) {
    if (!isDigit(s[i])) return false;
  }
  return true;
}

void printByteBinary(byte value) {
  Serial.print("0b");
  for (int i = 7; i >= 0; i--) {
    Serial.print((value >> i) & 0x01);
  }
  terminalPrintln(" ");
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(40000);

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
  serialSetup();
}

void loop() {
  if(!prompted){
    terminalPrint("-> master: ");
    prompted = true;
  }
  if(Serial.available()){
    input = Serial.readStringUntil('\n');
    digitalWrite(35,HIGH);
    delay(100);
    digitalWrite(35,LOW);
    input.trim();

    char buf[256];
    input.toCharArray(buf, sizeof(buf)); //input string to charArray

    //parse input array with string tokenizer and save as char* pointer (return type of strtrok)
    //strtok(<input-array-call0-only>, "<delimiter-char-(split location)>");
    char* p1 = strtok(buf, " ");
    char* p2 = strtok(nullptr, " ");
    char* p3 = strtok(nullptr, " ");
    char* extra = strtok(nullptr, " ");

    //run mullptr checks via string tokenizer return
    if (p1 == nullptr || p2 == nullptr || p3 == nullptr) {
      terminalPrintln("Err: too few arguments. Format: <cmd> <arg> <flag>");
      prompted = false;
      return;
    }

    if (extra != nullptr) {
      terminalPrintln("Err: too many arguments. Format: <cmd> <arg> <flag>");
      prompted = false;
      return;
    }

    //set easily referencable strings to use in command logic
    cmd = String(p1);
    arg = String(p2);
    flag = String(p3);

    if(cmd.equals("ping")){ //ping
      prompted = false;
      if(arg.toInt() == 0 || arg.toInt() == 1 || arg.toInt() == 2 || arg.toInt() == 3 || arg.toInt() == 4 || arg.toInt() == 5 || arg.toInt() == 6){
        printByteBinary(ping(arg.toInt(),0)); //ping node with type 0 (single node ping)
      }else if(arg.equals("all")){
        printByteBinary(ping(0,1));           //ping all nodes with type 1 (all node ping)
      }else if(arg.equals("help")){
        terminalPrintln("Functionality: ping nodes 1-6 or all nodes on CAN bus");
        terminalPrintln("Input Format: ping <n#> <->");
        terminalPrintln("Ex1: ping 1 -");
        terminalPrintln("Ex2: ping all -");
        terminalPrintln("Output: Returns byte with node status: 0 = offline, 1 = online");
        terminalPrintln("Output Ex: \n 0b01111111 = all nodes online \n0b00000001 = master online\n0b00000010 = node1 online \n0b10000000 = input/logic error");
      }else{
        terminalPrintln("Err: argument '"+arg+"' unknown");
      }

    }else if(cmd.equals("blink")){ // blink
      prompted = false;
      if(arg.equals("master")){
        blink(0,flag.toInt());
      }else if(arg.equals("help")){
        terminalPrintln("Functionality: blink nodes 1-6 or master node on CAN bus");
        terminalPrintln("Input Format: blink <n#> <millis>");
        terminalPrintln("Ex1: blink 1 1000");
        terminalPrintln("Ex2: blink master 500");
        terminalPrintln("Output: Blinks node_n or master node for <millis> milliseconds");
      }else if(isInt(arg)){
        blink(arg.toInt(), flag.toInt());
      }else{
        terminalPrintln("Err: argument '"+arg+"' unknown");
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
          terminalPrintln("Err: 0b10000000 \n command fetched master node status or node not live. ping to verify node health");
        }else if(statusArray[1] == 1){
          terminalPrintln("Node "+arg+" currently idle.");
        }else{
          terminalPrint(task1+": ");
          if(statusArray[7]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task2+": ");
          if(statusArray[6]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task3+": ");
          if(statusArray[5]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task4+": ");
          if(statusArray[4]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task5+": ");
          if(statusArray[3]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task6+": ");
          if(statusArray[2]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}

        }

      }else if(arg.equals("all")){
        for (int i = 1; i <= 7; i++){
          int statusArray [8]; // 8 index array
          statusByte = status(i);

          for(int i = 0; i < 8; i++){ //convert statusByte into array
          statusArray[i] = (statusByte >> (7 - i) & 0x01); //
          }

          if(statusArray[0] == 1){
          terminalPrintln("Err: 0b10000000 \n command fetched master node status or node not live. ping to verify node health");
          }else if(statusArray[1] == 1){
          terminalPrintln("Node "+(String)i+" currently idle.");
          }else{
          terminalPrint(task1+": ");
          if(statusArray[7]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task2+": ");
          if(statusArray[6]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task3+": ");
          if(statusArray[5]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task4+": ");
          if(statusArray[4]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task5+": ");
          if(statusArray[3]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          terminalPrint(task6+": ");
          if(statusArray[2]==1){terminalPrintln("live");}else{terminalPrintln("inactive");}
          }
        }
      }  
    }else{
      prompted = false;
      terminalPrintln("Err: command '"+cmd+"' unknown");
    }   
  }
}
