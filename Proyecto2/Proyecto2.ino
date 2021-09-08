#include <EEPROM.h>
#include <TimerOne.h>

//buttons
int S0 = 2;
int S1 = 3;
float S0_value = 0;
float S1_value = 0;

//leds and its eeprom values, dejando dos espacios entre cada uno para no sobre escribir valores sin querer
int L1 = 5;
int L2 = 6;

int L1_digital = 0;
int L1_digital_rom = 10;
int L1_analog = 0;
int L1_analog_rom = 12;

int L2_digital = 0;
int L2_digital_rom = 14;
int L2_analog = 0;
int L2_analog_rom = 16;

//Led Interrup counters
bool L1_state = false;
bool L2_state = false;

int L1_counter = 0;
int L1_top = 0;
int L1_top_rom = 18;

int L2_counter = 0;
int L2_top = 0;
int L2_top_rom = 20;

//Extra vars
char data;
bool valid_instruction = false;
String instruction = "";
String target = "";
String targetvalue = "";
int inttargetvalue = 0;
int separatorcounter = 0;

unsigned long clock_timer;

void setup() {
    Timer1.initialize(1000);
    Timer1.attachInterrupt(ChangeLedState);
  
    //LoadEEPROM Data
    LoadData();

    //Serial init
    Serial.begin(9600);
    Serial.setTimeout(1);

    //Button init
    pinMode(S0, INPUT);
    pinMode(S1, INPUT);

    //Led init
    pinMode(L1, OUTPUT);
    pinMode(L2, OUTPUT);
}

void loop() {
    S0_value = digitalRead(S0);
    S1_value = digitalRead(S1);

    noInterrupts();
    if (Serial.available() > 0){
        data = Serial.read();
        InstructionParse();
        if (data == 10){
            ExecuteInstruction();
        }
    }
    interrupts();
}

//Control the leds
void L1Control(){
    if (L1_digital > 0){
            digitalWrite(L1, L1_digital);
    }
    else if(L1_analog > 0){
        analogWrite(L1, L1_analog);
    }
    else{
        digitalWrite(L1, 0);
        analogWrite(L1, 0);
    }
}

void L2Control(){
    if (L2_digital > 0){
        digitalWrite(L2, L2_digital);
    }
    else if(L2_analog > 0){
        analogWrite(L2, L2_analog);
    }
    else{
        digitalWrite(L2, 0);
        analogWrite(L2, 0);
    }
}

//Led Interrupt handler
void ChangeLedState(){
  if (L1_state){
    L1Control();
  }
  else{
    digitalWrite(L1, 0);
  }

  if (L2_state){
    L2Control();
  }
  else{
    digitalWrite(L2, 0);
  }
  
  if (L1_counter > L1_top*10){
    L1_counter = 0;
    if (L1_state){
      L1_state = false;
    }
    else{
      L1_state = true;
    }
  }
  L1_counter ++;

  if (L2_counter > L2_top*10){
    L2_counter = 0;
    if (L2_state){
      L2_state = false;
    }
    else{
      L2_state = true;
    }
  }
  L2_counter ++;
}

//Write EEPROM
int PutEEPROM(int dir, int value){
    if (dir >= 0 && dir <= 255){
        EEPROM.put(dir, value);
        return value;
    }
    return 0;
}

//Read EEPROM
int GetEEPROM(int dir){
    int value;
    if (dir >= 0 && dir <= 255){
        EEPROM.get(dir, value);
        return value;
    }
    return 0;
}

//Loads all values from the EEPROM
void LoadData(){
    L1_digital = GetEEPROM(L1_digital_rom);
    L1_analog = GetEEPROM(L1_analog_rom);

    L2_digital = GetEEPROM(L2_digital_rom);
    L2_analog = GetEEPROM(L2_analog_rom);

    L1_top = GetEEPROM(L1_top_rom);
    L2_top = GetEEPROM(L2_top_rom);
}

//Writes all current values to the EEPROM
void UpdateAllData(){
    PutEEPROM(L1_digital_rom, L1_digital);
    PutEEPROM(L1_analog_rom, L1_analog);

    PutEEPROM(L2_digital_rom, L2_digital);
    PutEEPROM(L2_analog_rom, L2_analog);

    PutEEPROM(L1_top_rom, L1_top);
    PutEEPROM(L2_top_rom, L2_top);
}

void InstructionParse(){
    if (data == 44){
        separatorcounter ++;
    }
    else if (separatorcounter == 0){
        instruction = instruction + (char)data;
    }
    else if (separatorcounter == 1){
      if (data != 101){
        target = target + (char)data;
      }
    }
    else if (separatorcounter == 2){
        targetvalue = targetvalue + (char)data;
    }
}

void ExecuteInstruction(){
    if (separatorcounter == 0){
        Serial.print("Instruction: ");
        Serial.print(instruction[0]);
        Serial.print(instruction[1]);
        if (AllOff()){
          valid_instruction = true;
        }
        else if(UpdateEEPROM()){
          valid_instruction = true;
        }
    }
    else if (separatorcounter == 1){
        Serial.print("Instruction: ");
        Serial.print(instruction);
        Serial.print(" --> ");
        Serial.print("Target: ");
        Serial.print(target);
        
        if (SwitchState()){
            valid_instruction = true;
        }
    }
    else if (separatorcounter == 2){
        Serial.print("Instruction: ");
        Serial.print(instruction);
        Serial.print(" --> ");
        Serial.print("Target: ");
        Serial.print(target);
        Serial.print(" --> ");
        Serial.print("Target value: ");
        inttargetvalue = targetvalue.toInt();
        Serial.print(inttargetvalue);
        
        if (LedDWrite()){
            valid_instruction = true;
        }
        else if (LedAWrite()){
            valid_instruction = true;
        }
        else if (LedFrecuency()){
            valid_instruction = true;
        }
    }

    if (valid_instruction == false){
        Serial.print(" Error: ");
        Serial.print("Invalid Instruction, please check");
    }
    valid_instruction = false;
    separatorcounter = 0;
    instruction = "";
    target = "";
    targetvalue = "";
}

//Catches the R command and its values
bool SwitchState(){
    Serial.print(" --> ");
    if (instruction == "R" && target == "S0\n"){
        if (S0_value > 0){
            Serial.print("Button 0 Presed");
        }
        else{
            Serial.print("Button 0 Not Pressed");
        }
        return true;
    }
    else if (instruction == "R" && target == "S1\n"){
        if (S1_value > 0){
            Serial.print("Button 1 Presed");
        }
        else{
            Serial.print("Button 1 Not Pressed");
        }
        return true;
    }
    return false;
}

//Catches the W command and its values
bool LedDWrite(){
    if (instruction == "W" && target == "L1"){
        if (inttargetvalue >= 0 && inttargetvalue <= 1){
            L1_digital = inttargetvalue;
            L1_analog = 0;
            return true;
        }
    }
    else if (instruction == "W" && target == "L2"){
        if (inttargetvalue >= 0 && inttargetvalue <= 1){
            L2_digital = inttargetvalue;
            L2_analog = 0;
            return true;
        }
    }
    return false;
}

//Catches the A command and its values
bool LedAWrite(){
    if (instruction == "A" && target == "L1"){
        if (inttargetvalue >= 0 && inttargetvalue <= 255){
            L1_digital = 0;
            L1_analog = inttargetvalue;
            return true;
        }
    }
    else if (instruction == "A" && target == "L2"){
        if (inttargetvalue >= 0 && inttargetvalue <= 255){
            L2_digital = 0;
            L2_analog = inttargetvalue;
            return true;
        }
    }
    return false;
}

//Catches the B command and its values
bool LedFrecuency(){
    if (instruction == "B" && target == "L1"){
        if (inttargetvalue >= 0 && inttargetvalue <= 100){
            L1_top = inttargetvalue;
            L1_counter = 0;
            return true;
        }
    }
    else if (instruction == "B" && target == "L2"){
        if (inttargetvalue >= 0 && inttargetvalue <= 100){
            L2_top = inttargetvalue;
            L2_counter = 0;
            return true;
        }
    }
    return false;
}

bool UpdateEEPROM(){
    if (instruction == "E\n"){
        Serial.print(" --> ");
        Serial.print("Data Uploaded To EEPROM");
        UpdateAllData();
        return true;
    }
    return false;
}

//Set all values to 0
bool AllOff(){
    if (instruction == "O\n"){
        Serial.print(" --> ");
        Serial.print("Shuting down all leds");
        L1_digital = 0;
        L1_analog = 0;
        L2_digital = 0;
        L2_analog = 0;
        L1_top = 0;
        L2_top = 0;
        return true;
    }
    return false;
}
