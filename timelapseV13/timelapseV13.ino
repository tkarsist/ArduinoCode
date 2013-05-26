#include <SuperStepper.h>
#include <avr/interrupt.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"
#define Length 15


typedef struct {
  unsigned long int atTime;
  unsigned long int endTime;
  int mmToMove;
  int dir;
  int shotsToTake;
  int movePattern;
} 
commandAction;

commandAction actionlist[Length];

typedef struct {
  int repeatCount;
  int repeats;
  unsigned long int nextEvent;
  int eventType; //turha
  float distanceToMove;
  int commandIndexRef;  
  unsigned long interval;
} 
servoTask;

servoTask servo1Task = { 
  0, 0, 0, 0, 0, 0, 0 };

servoTask servoShutterTask = { 
  0, 0, 0, 0, 0, 0, 0 };

servoTask blank = { 
  0, 0, 0, 0, 0, 0, 0 };

SuperStepper stepper(9, 10, 11);

volatile static unsigned long last_interrupt_time = 0;

//consts
const int cameraPin1 = 13; //camera
const int cameraPin2 = 12; //camera
const int ledPinPower = 8;
const int ledPinProgram = 7;
const int resetButton = 3; //reset button
const int backwardButton=4;
const int forwardButton=5;
const float constSpeed = 200;
const int stabilizeTime = 700;
const int defaultRemoteTime = 100;
const int debounceDelay = 20; // milliseconds to wait until stable
//for linear accelearation and deceleration
const float startAccFraction=0.30;
const float stopDecFraction=0.25;

//linear acceleration variables, should be moved to the loops itself?
int startAccSteps=0;
int stopDecSteps=0;
float Vavg=0;
float Vmax=0;
float startAccStaticMM=0;
float stopDecStaticMM=0;
float returnToHomeCounterLinearMM=0;


unsigned long time=0;
unsigned long timeOffset = 0;
boolean returnHome = false;
boolean returnedHome = false;

String sanoma = "";
boolean sanomaReady = false;
boolean sanomaOngoing = false;
int actionlistIndex = 0;
int actionlistNext = 0;

volatile int state=LOW;
volatile boolean programRunning = false;
volatile boolean motorManualDrive=false;
int eventsReceived = 0;//true for forward


void setup() {

  pinMode(1, OUTPUT);

  //these are for the camera shutter & focus)
  pinMode(cameraPin1, OUTPUT);
  pinMode(cameraPin2, OUTPUT);

  //stepper
  stepper.setOneRoundInMM(1.25);
  stepper.setResolution(800);
  //stepper.setMinUsDelay(150);

  //power light on
  pinMode(ledPinPower, OUTPUT);
  digitalWrite(ledPinPower, HIGH);

  //program running led
  pinMode(ledPinProgram, OUTPUT);

  pinMode(backwardButton, INPUT_PULLUP);
  pinMode(forwardButton, INPUT_PULLUP);
  //direction leds
  // pinMode(ledPinManualForward, OUTPUT);
  // pinMode(ledPinManualBackward, OUTPUT);

  //enables interrupt and pullut
  digitalWrite(3,HIGH);  
  attachInterrupt(1, stateChange, LOW);  
  //interrupts();

  Serial.begin(19200);
  Serial.println("-----");
  Serial.println("Ready");
  //stepper.setGracefulStop(true);
  readProgramFromEEPROM();

}

void loop() {

  time = millis();
  /*if(time%5000==0){
  Serial.print("available memory: ");
  Serial.println(availableMemory());
  }*/

  //checks if program is still running. If all shutters done it will reset the program (and return to initial position if defined)
  checkIfProgramRunning();
  //stepper taskit
  checkStepperTask();
  //shutter taskit
  checkShutterTask();
  //stepper ja shutter taskien teko
  generateServoTasks();
  //parsing the message
  readSerialMessage();
  checkIfForwardButtonPressed();
  checkIfBackwardButtonPressed();
}


int availableMemory() {
  // Use 1024 with ATmega168
  int size = 2048;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL)
    ;
  free(buf);
  return size;

}

void checkIfProgramRunning() {
  if (programRunning == true && actionlistNext == eventsReceived
    && servo1Task.repeatCount == servo1Task.repeats
    && servoShutterTask.repeatCount == servoShutterTask.repeats) {
    if (returnHome == returnedHome) {
      resetProgram();
    } 
    else {
      returnToHome();
      resetProgram();
    }

  }
}

void returnToHome() {
  float mmToReturn = 0;
  for (int i = 0; i < eventsReceived; i++) {
    if(actionlist[i].movePattern==0){
      if (actionlist[i].dir == 0) {
        mmToReturn = mmToReturn + ((-1) * actionlist[i].mmToMove);
      } 
      else {
        mmToReturn = mmToReturn + actionlist[i].mmToMove;
      }
    }
  }
  mmToReturn+=returnToHomeCounterLinearMM;
  delay(2000);
  stepper.moveMM(mmToReturn, constSpeed);
  returnedHome = true;
  Serial.println("MM returned");
  Serial.println(mmToReturn);
}

void changeProgramLed(boolean state) {
  if (state) {
    digitalWrite(ledPinProgram, HIGH);
  } 
  else {
    digitalWrite(ledPinProgram, LOW);
  }

}

void checkIfResetButtonPressed() {

  //int buttonval = digitalRead(resetButton);

  //if (buttonval == LOW) {
  //  while (!debounce(resetButton)) {
  // }
  if(motorManualDrive){
    stepper.interruptMotor();
    motorManualDrive=false;
    changeProgramLed(false);
    Serial.println("Motor Stopped");
  }
  else{
  if (programRunning) {
    stepper.interruptMotor();
    Serial.println("Stopping");
    digitalWrite(ledPinProgram, HIGH);
    delay(200);
    digitalWrite(ledPinProgram, LOW);      
    delay(100);
    digitalWrite(ledPinProgram, HIGH);
    delay(200);
    digitalWrite(ledPinProgram, LOW);
    delay(100);
    digitalWrite(ledPinProgram, HIGH);
    delay(200);
    digitalWrite(ledPinProgram, LOW);

    resetProgram();
    setActionListOffset();


    //oma softreset
    //asm volatile ("  jmp 0");    
  } 
  else{
    changeProgramLed(true);
    delay(2000);
    setActionListOffset();
    programRunning = true;

  }}


  //}

}

void setActionListOffset() {
  timeOffset = millis();
}

void resetProgram() {

  servo1Task = blank;
  servoShutterTask = blank;
  actionlistIndex = 0;
  actionlistNext = 0;
  returnToHomeCounterLinearMM=0;
  programRunning = false;
  returnedHome = false;

  changeProgramLed(false);

}

void resetActionList() {
  returnHome = false;
  eventsReceived = 0;
  memset(actionlist, 0, sizeof(commandAction) * Length);
}

// debounce returns the stable switch state
boolean debounce(int pin) {
  boolean state;
  boolean previousState;

  previousState = digitalRead(pin); // store switch state
  for (int counter = 0; counter < debounceDelay; counter++) {
    delay(1); // wait for 1 millisecond
    state = digitalRead(pin); // read the pin
    if (state != previousState) {
      counter = 0; // reset the counter if the state changes
      previousState = state; // and save the current state
    }
  }
  return state;
}

void stateChange(){
  my_interrupt_handler();
}

void my_interrupt_handler()
{

  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 200)
  {
    checkIfResetButtonPressed();

  }

  last_interrupt_time = interrupt_time;
}

void clearEEPROM(){
  EEPROM.write(0,0);
}

void readProgramFromEEPROM(){
  int events=0;
  events=EEPROM.read(0);
  eventsReceived=events;
  Serial.println("Events in total: ");
  Serial.println(events);
  char charSanoma[25];
//  EEPROM_readAnything(0,events);
  if(events!=0){
  for (int i=1;i<=events;i++){
      String sanomaY="";
      int index=2+(i-1)*24;
      for(int x=0;x<sizeof(charSanoma);x++){
        charSanoma[x]=EEPROM.read(index);
        index+=1;
      }
      sanomaY=charSanoma;
      String sanomaP=sanomaY.substring(0,24);
      Serial.println(sanomaP);
      processMessage(sanomaP);
  }
  }
  
}
void writeMessageToEEPROM(String sanomaX, int events){
 
  Serial.println("Event nro: ");
  EEPROM.write(0,events);
  int G=EEPROM.read(0);
  Serial.println(G);
  
  Serial.println("Sanoma sisaan: ");
  char charSanoma[25];
  String sanomaP=sanomaX.substring(0,24);
  sanomaP.toCharArray(charSanoma, sizeof(charSanoma));
  for(int i=0;i<sizeof(charSanoma);i++){
  Serial.print(charSanoma[i]);
  }
   int indexToWrite=2+(events-1)*24;
   for (int x=0; x<sizeof(charSanoma);x++){
     EEPROM.write(indexToWrite,charSanoma[x]);
     indexToWrite+=1;
   }
   Serial.println("");
   int indexFromRead=2+(events-1)*24;
   char charResult[25];
   for (int z=0;z<sizeof(charResult);z++){
     charResult[z]=EEPROM.read(indexFromRead);
     indexFromRead+=1;
   }
   Serial.println("Sanoma ulos: ");
     for(int i=0;i<sizeof(charResult);i++){
  Serial.print(charResult[i]);
  }
  Serial.println("");
}

void processMessage(String message){
  Serial.print("sanoma: ");
  Serial.println(message);

  //Task start time (7 char in seconds)
  char charAtTime[8];
  String stringAtTime = message.substring(0, 7);
  stringAtTime.toCharArray(charAtTime, sizeof(charAtTime));
  //unsigned long int tmpAtTime=atol(charAtTime)*1000+timeOffset;
  unsigned long int tmpAtTime = long(atol(charAtTime) * 1000);
  Serial.print("attime: ");
  Serial.println(tmpAtTime);

  //Task end time (7 char in seconds)
  char charEndTime[8];
  String stringEndTime = message.substring(7, 14);
  stringEndTime.toCharArray(charEndTime, sizeof(charEndTime));
  //unsigned long int tmpEndTime=atol(charEndTime)*1000+timeOffset;
  unsigned long int tmpEndTime = atol(charEndTime) * 1000;
  Serial.print("endtime: ");
  Serial.println(tmpEndTime);

  //How many mm to move (max 999mm)
  char charMMToMove[4];
  String stringMMToMove = message.substring(14, 17);
  stringMMToMove.toCharArray(charMMToMove, sizeof(charMMToMove));
  int tmpMMToMove = atol(charMMToMove);
  Serial.print("MMToMove: ");
  Serial.println(tmpMMToMove);

  //Direction (0 or 1)
  String stringDir = message.substring(17, 18);
  int tmpDir = stringDir[0] - 48;
  Serial.print("Direction: ");
  Serial.println(tmpDir);

  //How many shots to take (max 9999)
  char charShotsToTake[5];
  String stringShotsToTake = message.substring(18, 22);
  stringShotsToTake.toCharArray(charShotsToTake,
  sizeof(charShotsToTake));
  int tmpShotsToTake = atol(charShotsToTake);
  Serial.print("ShotsToTake: ");
  Serial.println(tmpShotsToTake);

  //Return home (0 not, 1 yeah)
  String stringReturnHome = message.substring(22, 23);
  int tmpReturnHome = stringReturnHome[0] - 48;
  Serial.print("ReturnHome: ");
  Serial.println(tmpReturnHome);

  //Move pattern 
  String stringMovePattern = message.substring(23, 24);
  int tmpMovePattern = stringMovePattern[0] - 48;
  Serial.print("MovePattern: ");
  Serial.println(tmpMovePattern);

  if(actionlistIndex<Length){
  actionlist[actionlistIndex].atTime = tmpAtTime;
  actionlist[actionlistIndex].endTime = tmpEndTime;
  actionlist[actionlistIndex].mmToMove = tmpMMToMove;
  actionlist[actionlistIndex].dir = tmpDir;
  actionlist[actionlistIndex].shotsToTake = tmpShotsToTake;
  actionlist[actionlistIndex].movePattern = tmpMovePattern;
  //Serial.println(returnHome);
  if (tmpReturnHome == 1)
    returnHome = true;
  //Serial.println(returnHome);

  actionlistIndex = actionlistIndex + 1;
  Serial.print("Actionlist index: ");
  Serial.println(actionlistIndex);
  }
}

void generateServoTasks(){
  if (programRunning && actionlistNext < eventsReceived && time > long(timeOffset + actionlist[actionlistNext].atTime)) {
    servo1Task.distanceToMove = float(actionlist[actionlistNext].mmToMove)/(actionlist[actionlistNext].shotsToTake);
    servo1Task.interval = (actionlist[actionlistNext].endTime-actionlist[actionlistNext].atTime)/actionlist[actionlistNext].shotsToTake;
    servo1Task.repeats = actionlist[actionlistNext].shotsToTake;
    servo1Task.nextEvent = actionlist[actionlistNext].atTime + timeOffset;
    servo1Task.repeatCount = 0;

    if (actionlist[actionlistNext].dir == 1) {
      servo1Task.distanceToMove = servo1Task.distanceToMove * (-1);
    }

    if(actionlist[actionlistNext].movePattern==0){
      servo1Task.eventType=0;
      Serial.println("Constant Speed Movement");
    }

    if(actionlist[actionlistNext].movePattern==1){
      servo1Task.eventType=1;
      startAccSteps=float(startAccFraction*actionlist[actionlistNext].shotsToTake);
      stopDecSteps=float(stopDecFraction*actionlist[actionlistNext].shotsToTake);
      Vavg=float((actionlist[actionlistNext].mmToMove+0.0)/(actionlist[actionlistNext].shotsToTake+0.0));
      Vmax=float(Vavg/(0.5*startAccFraction+(1.0-startAccFraction-stopDecFraction)+0.5*stopDecFraction));
      startAccStaticMM=float(Vmax/(startAccSteps));
      stopDecStaticMM=float(Vmax/(stopDecSteps));
      Serial.println("Linear Acc/Dec Movement");
      Serial.print("Vavg: ");
      Serial.println(Vavg);
      Serial.print("StartFraction: ");
      Serial.println(startAccFraction);
      Serial.print("StopFraction: ");
      Serial.println(stopDecFraction);
      Serial.print("Vmax: ");
      Serial.println(Vmax);
      Serial.print("AccSteps: ");
      Serial.println(startAccSteps);
      Serial.print("DecSteps: ");
      Serial.println(stopDecSteps); 
      Serial.print("StartAccStaticMM: ");
      Serial.println(startAccStaticMM);
      Serial.print("StopDecStaticMM: ");        
      Serial.println(stopDecStaticMM);
    }

    servoShutterTask.repeats = actionlist[actionlistNext].shotsToTake;
    servoShutterTask.nextEvent = 0;
    servoShutterTask.repeatCount = 0;
    servoShutterTask.interval = (actionlist[actionlistNext].endTime-actionlist[actionlistNext].atTime)/actionlist[actionlistNext].shotsToTake;
    actionlistNext = actionlistNext + 1;
  }
}

void checkStepperTask(){
  if (servo1Task.nextEvent != 0 && servo1Task.interval != 0 && servo1Task.nextEvent < time) {

    if (servo1Task.repeatCount < servo1Task.repeats) {
      if(servo1Task.eventType==0){

        stepper.moveMM(servo1Task.distanceToMove, constSpeed);
        servoShutterTask.nextEvent = millis() + stabilizeTime;
        servo1Task.repeatCount += 1;

        Serial.print("Repeats done:");
        Serial.println(servo1Task.repeatCount);
      }
      if(servo1Task.eventType==1){
        int sign=-1;

        if(servo1Task.distanceToMove<0){
          sign=1;
        }    

        servo1Task.repeatCount += 1;

        if(servo1Task.repeatCount<=startAccSteps){
          Serial.println("Speeding up");
          stepper.moveMM(servo1Task.repeatCount*startAccStaticMM, constSpeed);
          returnToHomeCounterLinearMM+=float((servo1Task.repeatCount)*startAccStaticMM*sign);

        }

        if(servo1Task.repeatCount>startAccSteps && servo1Task.repeatCount<=(servo1Task.repeats-stopDecSteps)){
          Serial.println("Running const speed");
          stepper.moveMM(Vmax, constSpeed);
          returnToHomeCounterLinearMM+=float(Vmax*sign);
        }
        if(servo1Task.repeatCount>(servo1Task.repeats-stopDecSteps)){
          Serial.println("Slowing down");
          stepper.moveMM((servo1Task.repeats-(servo1Task.repeatCount))*stopDecStaticMM, constSpeed);
          returnToHomeCounterLinearMM+=float((servo1Task.repeats-(servo1Task.repeatCount))*stopDecStaticMM*sign);
        }  

        Serial.print("returnToHomeCounterLinearMM: ");
        Serial.println(returnToHomeCounterLinearMM);
        servoShutterTask.nextEvent = millis() + stabilizeTime;
        servo1Task.nextEvent = servo1Task.nextEvent + servo1Task.interval;
      }
    }
  }
}

void checkShutterTask(){
  if (servoShutterTask.nextEvent != 0 && servoShutterTask.nextEvent < time) {
    if (servoShutterTask.repeatCount < servoShutterTask.repeats) {
      digitalWrite(cameraPin1, HIGH);
      digitalWrite(cameraPin2, HIGH);
      delay(defaultRemoteTime);
      digitalWrite(cameraPin1, LOW);
      digitalWrite(cameraPin2, LOW);
      servoShutterTask.repeatCount += 1;
      servoShutterTask.nextEvent = 0;
      Serial.print("Shutters done:");
      Serial.println(servoShutterTask.repeatCount);
    }
  } 
}

void readSerialMessage(){
 if (Serial.available()) {
  char ch = Serial.read();

  if (ch == '!') {
    Serial.println("Program reset");
    resetActionList();
    clearEEPROM();
    resetProgram();
  }
  //viestin alku
  if (ch == '<') {
    sanomaOngoing = true;
  }
  //viestin loppu
  if (ch == '>') {
    sanomaOngoing = false;
    sanomaReady = true;
    eventsReceived += 1;
  }

  //lisataan merkit sanomaan    
  if (sanomaOngoing && ch != '<') {
    sanoma = sanoma + ch;
  }
  //sanoma valmis
  if (sanomaReady) {
    writeMessageToEEPROM(sanoma, eventsReceived);
    processMessage(sanoma);
    sanoma = "";
    sanomaReady = false;
  }
 }
}

void checkIfForwardButtonPressed(){
if(!programRunning && !motorManualDrive){
  int buttonval = digitalRead(forwardButton);

  if (buttonval == LOW) {
    while (!debounce(forwardButton)) {
    }
    Serial.println("Forward button pressed");
    changeProgramLed(true);
    motorManualDrive=true;
    stepper.runContinuous(1,constSpeed);

  }
}
}
void checkIfBackwardButtonPressed(){
 if(!programRunning && !motorManualDrive){
  int buttonval = digitalRead(backwardButton);

  if (buttonval == LOW) {
    while (!debounce(backwardButton)) {
    }
    Serial.println("backward button pressed");
    changeProgramLed(true);
    motorManualDrive=true;
    stepper.runContinuous(-1,constSpeed);
  }
 }

}






