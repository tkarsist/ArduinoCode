#include <SuperStepper.h>
#include <avr/interrupt.h>
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
const int ledPin = 13; //camera
const int ledPin2 = 12; //camera
const int ledPinPower = 8;
const int ledPinProgram = 7;
//const int ledPinManualForward = 9;
//const int ledPinManualBackward = 8;
const int resetButton = 3; //vois muuttaa "start program buttoniksi, esim 2s viiveella"
//const int manualDirButton = 12;
//lisaa const int forward=5; const int backward=4
//const int potPin = 2;
const float constSpeed = 200;
const int stabilizeTime = 700;
const int defaultRemoteTime = 100;
const int debounceDelay = 20; // milliseconds to wait until stable
//for linear accelearation and deceleration
const float startAccFraction=0.30;
const float stopDecFraction=0.25;

//linear acceleration variables
int startAccSteps=0;
int stopDecSteps=0;
float Vavg=0;
float Vmax=0;
float startAccStaticMM=0;
float stopDecStaticMM=0;
float returnToHomeCounterLinearMM=0;



unsigned long timeOffset = 0;
boolean returnHome = false;
boolean returnedHome = false;

String sanoma = "";
boolean sanomaReady = false;
boolean sanomaOngoing = false;
int actionlistIndex = 0;
int actionlistNext = 0;

volatile boolean programRunning = false;
int eventsReceived = 0;
//true for forward
boolean manualDir = true;

//this is for controlling the leds, only if the state changes then it is changed
boolean manualDirOld = true;

//potentiometer value is max 1023, this is the initial value
int manualSpeed = 1500; //turha

volatile int state=LOW;

void setup() {

  pinMode(1, OUTPUT);

  //these are for the camera shutter & focus)
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  //stepper
  stepper.setOneRoundInMM(1.25);
  stepper.setResolution(800);
  //stepper.setMinUsDelay(150);

  //power light on
  pinMode(ledPinPower, OUTPUT);
  digitalWrite(ledPinPower, HIGH);

  //program running led
  pinMode(ledPinProgram, OUTPUT);

  //direction leds
 // pinMode(ledPinManualForward, OUTPUT);
 // pinMode(ledPinManualBackward, OUTPUT);

//enables interrupt and pullut
digitalWrite(3,HIGH);  
attachInterrupt(1, stateChange, LOW);  
//interrupts();


  //reset button with interrupt
//  pinMode(resetButton, INPUT_PULLUP);
/*      sei();                    // Enable global interrupts
    EIMSK |= (1 << INT0);     // Enable external interrupt INT0
    EICRA |= (1 << ISC01);
*/

  // Change manually direction button
  //pinMode(manualDirButton, INPUT_PULLUP);

  Serial.begin(19200);
  Serial.println("Ready");

}

void loop() {

  //current serial logic: 
  //< 0000015000  1         1         0             01500      02000       0003> : 
  //  time[10],   servo[1], speed[1], direction[1], runtime[5] idletime[5] repeats[4


  static int v = 0;
  static unsigned long time;
  //static unsigned long timeOffset=0;
  time = millis();

  //ledi kontrolli

  checkIfProgramRunning();
  //checkManualDirLed();

  //monitor reset program button
  //checkIfResetButtonPressed();

/*
  //potentiometrin luku
  if (programRunning == false) {
    checkSpeedVal();
    checkIfManualDirButtonPressed();
    //Serial.println(analogRead(potPin));
  }
*/
  //servojen taskit

  if (servo1Task.nextEvent != 0 && servo1Task.interval != 0 && servo1Task.nextEvent < time) {
    
    if (servo1Task.repeatCount < servo1Task.repeats) {
      if(servo1Task.eventType==0){

      //Serial.println("Distance to move");
      //Serial.println(servo1Task.distanceToMove);

      stepper.moveMM(servo1Task.distanceToMove, constSpeed);
      servoShutterTask.nextEvent = millis() + stabilizeTime;
      //Serial.println("Current time");
      //Serial.println(millis());
      //Serial.println("Next camera shutter");
      //Serial.println(servoShutterTask.nextEvent);

      //Serial.println("Motor moved");
      servo1Task.repeatCount += 1;
      servo1Task.nextEvent = servo1Task.nextEvent + servo1Task.interval;
      //Serial.print("next motor event");
      //Serial.println(servo1Task.nextEvent);
      //Serial.println(availableMemory());
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

  //servoShutter tasks

  if (servoShutterTask.nextEvent != 0 && servoShutterTask.nextEvent < time) {

    if (servoShutterTask.repeatCount < servoShutterTask.repeats) {
      //      if(servoShutterTask.eventType==0){


      //Serial.print("Take picture: ");
      //Serial.print(servoShutterTask.nextEvent);
      //Serial.print(" curr time: ");
      //Serial.println(millis());
      digitalWrite(ledPin, HIGH);
      digitalWrite(ledPin2, HIGH);
      delay(defaultRemoteTime);
      //Serial.print("Take picture end ");
      //Serial.print(" curr time: ");
      //Serial.println(millis());
      digitalWrite(ledPin, LOW);
      digitalWrite(ledPin2, LOW);
      servoShutterTask.repeatCount += 1;
      servoShutterTask.nextEvent = 0;
      Serial.print("Shutters done:");
      Serial.println(servoShutterTask.repeatCount);

    }

  }


  //tehdaan servotaskit
  if (programRunning && actionlistNext < eventsReceived && time > long(timeOffset + actionlist[actionlistNext].atTime)) {
    Serial.println("ahaa:");


    

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
    /*
		 Serial.println("Servotaskkurat: nextEvent, repeats, repeat count, interval, distanceToMove");
     		 Serial.println(servo1Task.nextEvent);
     		 Serial.println(servo1Task.repeats);
     		 Serial.println(servo1Task.repeatCount);
     		 Serial.println("Interval:");
     		 Serial.println(servo1Task.interval);
     		 Serial.println(servo1Task.distanceToMove);
     		 */
    actionlistNext = actionlistNext + 1;

  }

  //parsing the message
  if (Serial.available()) {
    char ch = Serial.read();

    if (ch == '!') {
      Serial.println("Program reset");
      resetActionList();
      resetProgram();
      //timeOffset=time;
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
      //programRunning=true;
      //changeProgramLed();

    }

    //lisataan merkit sanomaan    
    if (sanomaOngoing && ch != '<') {

      sanoma = sanoma + ch;

    }
    //sanoma valmis
    if (sanomaReady) {
      Serial.print("sanoma: ");
      Serial.println(sanoma);

      //Task start time (7 char in seconds)
      char charAtTime[8];
      String stringAtTime = sanoma.substring(0, 7);
      stringAtTime.toCharArray(charAtTime, sizeof(charAtTime));
      //unsigned long int tmpAtTime=atol(charAtTime)*1000+timeOffset;
      unsigned long int tmpAtTime = long(atol(charAtTime) * 1000);
      Serial.print("attime: ");
      Serial.println(tmpAtTime);

      //Task end time (7 char in seconds)
      char charEndTime[8];
      String stringEndTime = sanoma.substring(7, 14);
      stringEndTime.toCharArray(charEndTime, sizeof(charEndTime));
      //unsigned long int tmpEndTime=atol(charEndTime)*1000+timeOffset;
      unsigned long int tmpEndTime = atol(charEndTime) * 1000;
      Serial.print("endtime: ");
      Serial.println(tmpEndTime);

      //How many mm to move (max 999mm)
      char charMMToMove[4];
      String stringMMToMove = sanoma.substring(14, 17);
      stringMMToMove.toCharArray(charMMToMove, sizeof(charMMToMove));
      int tmpMMToMove = atol(charMMToMove);
      Serial.print("MMToMove: ");
      Serial.println(tmpMMToMove);

      //Direction (0 or 1)
      String stringDir = sanoma.substring(17, 18);
      int tmpDir = stringDir[0] - 48;
      Serial.print("Direction: ");
      Serial.println(tmpDir);

      //How many shots to take (max 9999)
      char charShotsToTake[5];
      String stringShotsToTake = sanoma.substring(18, 22);
      stringShotsToTake.toCharArray(charShotsToTake,
      sizeof(charShotsToTake));
      int tmpShotsToTake = atol(charShotsToTake);
      Serial.print("ShotsToTake: ");
      Serial.println(tmpShotsToTake);

      //Return home (0 not, 1 yeah)
      String stringReturnHome = sanoma.substring(22, 23);
      int tmpReturnHome = stringReturnHome[0] - 48;
      Serial.print("ReturnHome: ");
      Serial.println(tmpReturnHome);

      //Move pattern 
      String stringMovePattern = sanoma.substring(23, 24);
      int tmpMovePattern = stringMovePattern[0] - 48;
      Serial.print("MovePattern: ");
      Serial.println(tmpMovePattern);

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

      sanoma = "";
      sanomaReady = false;

    }

  }

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
      //eventsReceived=0;

      //oma softreset
      //asm volatile ("  jmp 0");    
    } 
    else {
      changeProgramLed(true);
      delay(2000);
      setActionListOffset();
      programRunning = true;

    }

  //}

}

void setActionListOffset() {
  timeOffset = millis();
}

/*
void checkIfManualDirButtonPressed() {
  int buttonval = digitalRead(manualDirButton);
  if (buttonval == LOW) {
    while (!debounce(manualDirButton)) {
    }

    //while(digitalRead(manualDirButton)==LOW){
    //}

    if (manualDir == true) {
      manualDir = false;
      manualSpeed = 0;
    } 
    else {
      manualDir = true;
      manualSpeed = 0;

    }
    Serial.println(manualDir);

  }
}

void checkSpeedVal() {
  int tolerance = 100;
  int analogSpeed = analogRead(potPin);
  if (manualSpeed == 1500) {
    manualSpeed = analogRead(potPin);
  } 
  else {
    if (abs(manualSpeed - analogSpeed) > tolerance) {
      manualSpeed = analogRead(potPin);
      int speedou = manualSpeed / 113;
      int tmpDir = 0;
      //Serial.println(manualSpeed);
      //Serial.println("speed : ");
      //Serial.println(speedou);
      if (manualDir == true)
        tmpDir = 1;

      if (speedou == 0) {
        //servo1.writeMicroseconds(1500);
        //Serial.println("Manual drive: Stop Servo");

      } 
      else {
        //int v= getSpeed(speedou,tmpDir);
        //  servo1.writeMicroseconds(v);
        //Serial.println("Manual drive: run servo");
      }
    }
  }
}

void checkManualDirLed() {
  if (programRunning) {
    digitalWrite(ledPinManualForward, LOW);
    digitalWrite(ledPinManualBackward, LOW);

  } 
  else {
    if (manualDir == true) {
      //if(manualDir==true && manualDirOld!=true){
      digitalWrite(ledPinManualForward, HIGH);
      digitalWrite(ledPinManualBackward, LOW);
      //manualSpeed=0;
      //manualDirOld=manualDir;


    }
    if (manualDir == false) {
      //if(manualDir==false && manualDirOld!=false){
      digitalWrite(ledPinManualBackward, HIGH);
      digitalWrite(ledPinManualForward, LOW);
      //manualSpeed=0;
      //manualDirOld=manualDir;

    }

  }

}
*/
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
/*
ISR(EXT_INT0_vect)
{
  digitalWrite(4, HIGH);
  //my_interrupt_handler();
}
*/
void my_interrupt_handler()
{
  
  unsigned long interrupt_time = millis();
  //Serial.println(interrupt_time);
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
checkIfResetButtonPressed();

//      programRunning = true;
//Serial.println("osumaa");

  }
  //Serial.println(last_interrupt_time);
  last_interrupt_time = interrupt_time;
} 



