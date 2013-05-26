#include <Servo.h>
#define Length 10

typedef struct{
  unsigned long int atTime;
  unsigned long int endTime;
  int mmToMove;
  int dir;
  int shotsToTake;
  int movePattern;
  /*
  int servoID;
  int servoSpeed; //turha
  int servoDir; 
  long runTime;  //turha (naa kaks pitas olla kuinka monta mm pitaa liikkua)
  long idleTime; //turha
  int repeats;
  */
}
commandAction;

commandAction actionlist[Length];

typedef struct{
  int repeatCount;
  int repeats;
  unsigned long int nextEvent;
  int eventType; //turha?
  float distanceToMove;
  int commandIndexRef;
  unsigned long interval;
}
servoTask;

servoTask servo1Task={
  0,0,0,2,0,0,0};
//servoTask servo2Task={
//  0,0,2,0};
//servoTask servo3Task={
//  0,0,2,0};
servoTask servoShutterTask={
  0,0,0,2,0,0,0};

Servo servo1; //turha
Servo servo2; //turha
Servo servo3; //turha
const int ledPin=3;
const int ledPin2=5;
const int ledPinPower=2;
const int ledPinProgram=4;
const int ledPinManualForward=7;
const int ledPinManualBackward=8;
const int resetButton=6; //vois muuttaa "start program buttoniksi, esim 2s viiveella"
const int manualDirButton=12;
const int potPin=2;
const int DIR_PIN=10; //stepper
const int STEP_PIN=11; //stepper
const int STEPPER_SLEEP=13; //stepper sleep pin
const float constSpeed=1;
const int stabilizeTime=300;
const int defaultRemoteTime=300;
const float OneRoundinMM=float(1.25);
const int stepperResolution=800;


String sanoma="";
boolean sanomaReady=false;
boolean sanomaOngoing=false;
int actionlistIndex=0;
int actionlistNext=0;

boolean programRunning=false;
int eventsReceived=0;
//true for forward
boolean manualDir=true;

//this is for controlling the leds, only if the state changes then it is changed
boolean manualDirOld=true;

//potentiometer value is max 1023, this is the initial value
int manualSpeed=1500; //turha

void setup() {

  pinMode(1,OUTPUT);

  //these are for the camera shutter & focus)
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  servo1.attach(9); //analog pin 0
  //servo2.attach(10);
  //servo3.attach(11);
  //servo1.setMaximumPulse(2000);
  //servo1.setMinimumPulse(700);
  
  //stepper pinnit
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(STEPPER_SLEEP, OUTPUT);
  digitalWrite(STEPPER_SLEEP,LOW);
  

  //power light on
  pinMode(ledPinPower, OUTPUT);
  digitalWrite(ledPinPower, HIGH);

  //program running led
  pinMode(ledPinProgram, OUTPUT);


  //direction leds
  pinMode(ledPinManualForward, OUTPUT);
  pinMode(ledPinManualBackward, OUTPUT);
  //digitalWrite(ledPinManualForward, HIGH);

  //digitalWrite(ledPinManualForward, HIGH);
  //digitalWrite(ledPinManualBackward, HIGH);


  //reset button
  pinMode(resetButton, INPUT_PULLUP);  

  // Change manually direction button
  pinMode(manualDirButton, INPUT_PULLUP);
  //servo2.attach(3); //analog pin 1
  Serial.begin(19200);
  Serial.println("Ready");

}

void loop() {

  //current serial logic: 
  //< 0000015000  1         1         0             01500      02000       0003> : 
  //  time[10],   servo[1], speed[1], direction[1], runtime[5] idletime[5] repeats[4


  static int v = 0;
  static unsigned long time;
  static unsigned long timeOffset=0;
  time= millis();

  //ledi kontrolli
  checkIfProgramRunning();
  checkManualDirLed();

  //potentiometrin luku
  if(programRunning==false){
    checkSpeedVal();
    checkIfManualDirButtonPressed();
    //Serial.println(analogRead(potPin));
  }

  //monitor reset program button
  checkIfResetButtonPressed();

  //servojen taskit

  if(servo1Task.nextEvent!=0 && servo1Task.interval!=0 && servo1Task.nextEvent<time){
    if(servo1Task.repeatCount<servo1Task.repeats){
        Serial.println("Distance to move");
        Serial.println(servo1Task.distanceToMove);
        rotateMM(servo1Task.distanceToMove,constSpeed);
        servoShutterTask.nextEvent=millis()+stabilizeTime;
        Serial.println("Motor moved");
        servo1Task.repeatCount+=1;
        servo1Task.nextEvent=servo1Task.nextEvent+servo1Task.interval;
        Serial.print("next event");
        Serial.println(servo1Task.interval);


/*      if(servo1Task.eventType==0){

        v=getSpeed(actionlist[servo1Task.commandIndexRef].servoSpeed,actionlist[servo1Task.commandIndexRef].servoDir);
        servo1.writeMicroseconds(v);
        Serial.print("servo 1 start: ");
        Serial.print(servo1Task.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(time);
        v=0;
        servo1Task.eventType=1;
        servo1Task.nextEvent=servo1Task.nextEvent+actionlist[servo1Task.commandIndexRef].runTime;

      }
      else{
        Serial.print("servo 1 detach");
        servo1.writeMicroseconds(1500);
        Serial.print(servo1Task.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(time);
        servo1Task.eventType=0;
        servo1Task.nextEvent=servo1Task.nextEvent+actionlist[servo1Task.commandIndexRef].idleTime;
        servo1Task.repeatCount+=1;
        if(servo1Task.repeatCount==actionlist[servo1Task.commandIndexRef].repeats){
          servo1Task.eventType=2;
        }
      }
      */
    }
  }


  //servoShutter tasks

  if(servoShutterTask.nextEvent!=0 && servoShutterTask.nextEvent<time){

    if(servoShutterTask.repeatCount<servoShutterTask.repeats){
//      if(servoShutterTask.eventType==0){



        Serial.print("Take picture: ");
        Serial.print(servoShutterTask.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(millis());
        digitalWrite(ledPin,HIGH);
        digitalWrite(ledPin2,HIGH);
        delay (defaultRemoteTime);
        Serial.print("Take picture end ");
        Serial.print(" curr time: ");
        Serial.println(millis());
        digitalWrite(ledPin,LOW);
        digitalWrite(ledPin2,LOW);
        servoShutterTask.repeatCount+=1;
        servoShutterTask.nextEvent=0;

/*
      }
      else{
        //runtime is the time the shutter is connected
        Serial.print("Take picture end ");
        Serial.print(servoShutterTask.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(time);
        digitalWrite(ledPin,LOW);
        digitalWrite(ledPin2,LOW);

        servoShutterTask.eventType=0;
        servoShutterTask.nextEvent=servoShutterTask.nextEvent+actionlist[servoShutterTask.commandIndexRef].idleTime;

        servoShutterTask.repeatCount+=1;
        if(servoShutterTask.repeatCount==actionlist[servoShutterTask.commandIndexRef].repeats){
          servoShutterTask.eventType=2;
        }
      }
      */
    }
    
  }

  //tehdaan servotaskit
  if (actionlistIndex>0 && actionlist[actionlistNext].atTime!=0 && time>actionlist[actionlistNext].atTime){
    //   if (actionlistIndex>0 && actionlist[actionlistNext].atTime!=0){
    //Serial.println(actionlist[actionlistNext].atTime);
    //Serial.println("mmToMove & shotsToTake");
    //Serial.println(actionlist[actionlistNext].mmToMove);
    //Serial.println(actionlist[actionlistNext].shotsToTake);
    Serial.println("ahaa:");
    Serial.println((actionlist[actionlistNext].mmToMove)/(actionlist[actionlistNext].shotsToTake));

    servo1Task.distanceToMove=float(actionlist[actionlistNext].mmToMove)/(actionlist[actionlistNext].shotsToTake);
    servo1Task.repeats=actionlist[actionlistNext].shotsToTake;
    servo1Task.nextEvent=actionlist[actionlistNext].atTime; 
    servo1Task.repeatCount=0;
    servo1Task.interval=(actionlist[actionlistNext].endTime-actionlist[actionlistNext].atTime)/actionlist[actionlistNext].shotsToTake;
    
    
    if(actionlist[actionlistNext].dir==1){
    servo1Task.distanceToMove=servo1Task.distanceToMove*(-1);
    }
  
    servoShutterTask.repeats=actionlist[actionlistNext].shotsToTake;
    servoShutterTask.nextEvent=0;
    servoShutterTask.repeatCount=0;  
    servoShutterTask.interval=(actionlist[actionlistNext].endTime-actionlist[actionlistNext].atTime)/actionlist[actionlistNext].shotsToTake;
    
            Serial.println("Servotaskkurat: nextEvent, repeats, repeat count, interval, distanceToMove");
        Serial.println(servo1Task.nextEvent);
        Serial.println(servo1Task.repeats);
        Serial.println(servo1Task.repeatCount);
        Serial.println("Interval:");
        Serial.println(servo1Task.interval);
        Serial.println(servo1Task.distanceToMove);
        
              actionlistNext=actionlistNext+1;


    /*
    int tmpIndex=0;
    if (actionlist[actionlistNext].servoSpeed>0){


      if (actionlist[actionlistNext].servoID==1){

        servo1Task.repeatCount=0;
        servo1Task.nextEvent=actionlist[actionlistNext].atTime;
        servo1Task.eventType=0;
        servo1Task.commandIndexRef=actionlistNext;
        Serial.println("Servotaskkurat: nextEvent, eventType, repeat count, commandindexref");
        Serial.println(servo1Task.nextEvent);
        Serial.println(servo1Task.eventType);
        Serial.println(servo1Task.repeatCount);
        Serial.println(servo1Task.commandIndexRef);
        tmpIndex=1;
      }
     



    }
    if (actionlist[actionlistNext].servoID==9){

      servoShutterTask.repeatCount=0;
      servoShutterTask.nextEvent=actionlist[actionlistNext].atTime;
      servoShutterTask.eventType=0;
      servoShutterTask.commandIndexRef=actionlistNext;
      Serial.println("Servo Shutter: nextEvent, eventType, repeat count, commandindexref");
      Serial.println(servoShutterTask.nextEvent);
      Serial.println(servoShutterTask.eventType);
      Serial.println(servoShutterTask.repeatCount);
      Serial.println(servoShutterTask.commandIndexRef);
      tmpIndex=1;
    }      
    if(tmpIndex!=0){
      actionlistNext=actionlistNext+1;
      Serial.print("Available memory: ");
      Serial.println(availableMemory());
    }
    if(tmpIndex==0){
      Serial.println("Task dropped, invalid");
      actionlistNext=actionlistNext+1;
    }
    */
  }





  //parsing the message
  if ( Serial.available()) {
    char ch = Serial.read();

    if (ch == '!'){
      Serial.println("New time offset is set");
      timeOffset=time;
    }
    //viestin alku
    if(ch == '<'){
      sanomaOngoing=true;

    }
    //viestin loppu
    if (ch == '>'){
      sanomaOngoing=false;
      sanomaReady=true;
      eventsReceived+=1;
      programRunning=true;
      changeProgramLed();

    }

    //lisataan merkit sanomaan    
    if(sanomaOngoing && ch!='<'){


      sanoma=sanoma + ch;


    }
    //sanoma valmis
    if(sanomaReady){
      Serial.print("sanoma: ");
      Serial.println(sanoma);

      //Task start time (7 char in seconds)
      char charAtTime[8];
      String stringAtTime=sanoma.substring(0,7);
      stringAtTime.toCharArray(charAtTime, sizeof(charAtTime));
      unsigned long int tmpAtTime=atol(charAtTime)*1000+timeOffset;
      Serial.print("attime: ");
      Serial.println(tmpAtTime);
      
      //Task end time (7 char in seconds)
      char charEndTime[8];
      String stringEndTime=sanoma.substring(7,14);
      stringEndTime.toCharArray(charEndTime, sizeof(charEndTime));
      unsigned long int tmpEndTime=atol(charEndTime)*1000+timeOffset;
      Serial.print("endtime: ");
      Serial.println(tmpEndTime);
      
      //How many mm to move (max 999mm)
      char charMMToMove[4];
      String stringMMToMove=sanoma.substring(14,17);
      stringMMToMove.toCharArray(charMMToMove, sizeof(charMMToMove));
      int tmpMMToMove=atol(charMMToMove);
      Serial.print("MMToMove: ");
      Serial.println(tmpMMToMove);
      
      //Direction (0 or 1)
      String stringDir=sanoma.substring(17,18); 
      int tmpDir=stringDir[0]-48;                  
      Serial.print("Direction: ");
      Serial.println(tmpDir);

      //How many shots to take (max 9999)
      char charShotsToTake[5];
      String stringShotsToTake=sanoma.substring(18,22);
      stringShotsToTake.toCharArray(charShotsToTake, sizeof(charShotsToTake));
      int tmpShotsToTake=atol(charShotsToTake);
      Serial.print("ShotsToTake: ");
      Serial.println(tmpShotsToTake); 
 
      //Return home (0 not, 1 yeah)
      String stringReturnHome=sanoma.substring(22,23); 
      int tmpReturnHome=stringReturnHome[0]-48;                  
      Serial.print("ReturnHome: ");
      Serial.println(tmpReturnHome);
     
      //Move pattern 
      String stringMovePattern=sanoma.substring(23,24); 
      int tmpMovePattern=stringMovePattern[0]-48;                  
      Serial.print("MovePattern: ");
      Serial.println(tmpMovePattern);

      actionlist[actionlistIndex].atTime=tmpAtTime;
      actionlist[actionlistIndex].endTime=tmpEndTime;
      actionlist[actionlistIndex].mmToMove=tmpMMToMove;
      actionlist[actionlistIndex].dir=tmpDir;
      actionlist[actionlistIndex].shotsToTake=tmpShotsToTake;
      actionlist[actionlistIndex].movePattern=tmpMovePattern;


      actionlistIndex=actionlistIndex+1;
      Serial.println("Actionlist index: ");
      Serial.println(actionlistIndex);

      sanoma="";
      sanomaReady=false;
            
      /*
      //At time extract
      char charAtTime[11];
      String stringAtTime=sanoma.substring(0,10);
      stringAtTime.toCharArray(charAtTime, sizeof(charAtTime));
      unsigned long int tmpAtTime=atol(charAtTime)+timeOffset;
      Serial.print("attime: ");
      Serial.println(tmpAtTime);

      //servoid extract
      String stringServoId=sanoma.substring(10,11); 
      int tmpServoId=stringServoId[0]-48;                  
      Serial.print("servoid: ");
      Serial.println(tmpServoId);


      //servospeed extract
      String stringServoSpeed=sanoma.substring(11,12);
      int tmpServoSpeed=stringServoSpeed[0]-48;
      Serial.print("servospeed: ");
      Serial.println(tmpServoSpeed);

      //servodirection extract
      String stringServoDir=sanoma.substring(12,13);
      int tmpServoDir=stringServoDir[0]-48;
      Serial.print("servodirection: ");
      Serial.println(tmpServoDir);

      //runtime extract
      char charRunTime[6];
      String stringRunTime=sanoma.substring(13,18);
      stringRunTime.toCharArray(charRunTime, sizeof(charRunTime));
      long tmpRunTime=atol(charRunTime);
      Serial.print("runtime: ");
      Serial.println(tmpRunTime);

      //idletime extract
      char charIdleTime[6];
      String stringIdleTime=sanoma.substring(18,23);
      stringIdleTime.toCharArray(charIdleTime, sizeof(charIdleTime));
      long tmpIdleTime=atol(charIdleTime);
      Serial.print("idletime: ");
      Serial.println(tmpIdleTime);

      //repeats extract      
      char charRepeats[5];
      String stringRepeats=sanoma.substring(23,27);
      stringRepeats.toCharArray(charRepeats, sizeof(charRepeats));
      int tmpRepeats=atoi(charRepeats);
      Serial.print("repeats: ");
      Serial.println(tmpRepeats);


      actionlist[actionlistIndex].atTime=tmpAtTime;
      actionlist[actionlistIndex].servoID=tmpServoId;
      actionlist[actionlistIndex].servoSpeed=tmpServoSpeed;
      actionlist[actionlistIndex].servoDir=tmpServoDir;
      actionlist[actionlistIndex].runTime=tmpRunTime;
      actionlist[actionlistIndex].idleTime=tmpIdleTime;
      actionlist[actionlistIndex].repeats=tmpRepeats;

      actionlistIndex=actionlistIndex+1;
      Serial.println("Actionlist index: ");
      Serial.println(actionlistIndex);
      */

      
    } 

  }




}

int availableMemory(){
  // Use 1024 with ATmega168
  int size = 2048;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;

}

/*
int getSpeed2(int servoSpeed, int servoDirection){
  int factor=0;
  int staticSpeed=90;
  int vspeed=0;

  if(servoSpeed<5){
    factor=5;
  }
  else
    factor=9;
  if (servoDirection==1){
    vspeed=90 +5+servoSpeed*factor;
  }
  else
    vspeed=90-servoSpeed*factor;

  return (vspeed);
  //0:88, 1:83, 2:78, 3:73, 4:68, 5:43, 6:34, 7:25, 8:16, 7
  //0:98, 1:103, 2:105, 3:108, 4:113, 5: 143, 6: 152, 7: 161, 8: 170, 9: 179  


}
*/

int getSpeed(int servoSpeed, int servoDirection){
  int factor=50;
  int staticSpeed=1500;
  int vspeed=0;

  if (servoDirection==1){
    vspeed=staticSpeed +50 + servoSpeed*factor;
  }
  else
    vspeed=staticSpeed-50-servoSpeed*factor;

  return (vspeed);
  //0:88, 1:83, 2:78, 3:73, 4:68, 5:43, 6:34, 7:25, 8:16, 7
  //0:98, 1:103, 2:105, 3:108, 4:113, 5: 143, 6: 152, 7: 161, 8: 170, 9: 179  


}

void checkIfProgramRunning(){
  if(programRunning==true && actionlistNext>=eventsReceived && servo1Task.eventType==2 && servoShutterTask.eventType==2){
    programRunning=false;
    manualSpeed=1500;
    changeProgramLed();
  }
}

void changeProgramLed(){
  if(programRunning==true){
    digitalWrite(ledPinProgram, HIGH);
  }
  else{
    digitalWrite(ledPinProgram, LOW);
  }

}

void checkIfResetButtonPressed(){
  int buttonval=digitalRead(resetButton);
  if(buttonval==LOW){
    while(digitalRead(resetButton)==LOW){
    }
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
    //oma softreset
    asm volatile ("  jmp 0");



  }
}

void checkIfManualDirButtonPressed(){
  int buttonval=digitalRead(manualDirButton);
  if(buttonval==LOW){
    while(digitalRead(manualDirButton)==LOW){
    }
    
    if(manualDir==true){
      manualDir=false;
      manualSpeed=0;
    }
    else{
      manualDir=true;
      manualSpeed=0;

    }
    Serial.println(manualDir);
    //rotate(6400, 1);
    
    rotateDeg(360, 1);
    //rotateMM(-50,0.5);
    delay(200);
    //rotateMM(-800,1);
    //delay(200);
    //rotateDeg(-36000, 1);
    //delay(200);
   /*
    rotateDeg(90,1);
    delay(200);
   rotateDeg(90,1);
   delay(200);
   rotateDeg(-180,1);
   */
  }
}

void checkSpeedVal(){
  int tolerance=100;
  int analogSpeed=analogRead(potPin);
  if(manualSpeed==1500){
  manualSpeed=analogRead(potPin);
  }
  else{
  if(abs(manualSpeed-analogSpeed)>tolerance){
    manualSpeed=analogRead(potPin);
    int speedou=manualSpeed/113;
    int tmpDir=0;
    //Serial.println(manualSpeed);
    //Serial.println("speed : ");
    //Serial.println(speedou);
    if(manualDir==true)
    tmpDir=1;
    
    if(speedou==0){
            servo1.writeMicroseconds(1500);
            //Serial.println("Manual drive: Stop Servo");
    
    }
    else{
    int v= getSpeed(speedou,tmpDir);
            servo1.writeMicroseconds(v);
        //Serial.println("Manual drive: run servo");
    }
  }
  }
}

void checkManualDirLed(){
  if(programRunning){
    digitalWrite(ledPinManualForward, LOW);
    digitalWrite(ledPinManualBackward, LOW);

  }
  else{
    if(manualDir==true){
    //if(manualDir==true && manualDirOld!=true){
      digitalWrite(ledPinManualForward, HIGH);
      digitalWrite(ledPinManualBackward, LOW);
      //manualSpeed=0;
      //manualDirOld=manualDir;
      

    }
    if(manualDir==false){
    //if(manualDir==false && manualDirOld!=false){
      digitalWrite(ledPinManualBackward, HIGH);
      digitalWrite(ledPinManualForward, LOW);
      //manualSpeed=0;
      //manualDirOld=manualDir;
      
    }
    
  }

}

void rotate(int steps, float speed){
  //rotate a specific number of microsteps (8 microsteps per step) - (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  Serial.println("Tuli rotateen");
  int dir = (steps > 0)? HIGH:LOW;
  steps = abs(steps);

  digitalWrite(DIR_PIN,dir); 

  float usDelay = (1/speed) * 150;

  for(int i=0; i < steps; i++){
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(usDelay);
  }
} 

void rotateDeg(float deg, float speed){
  
  //virran saasto ei toimi
  digitalWrite(STEPPER_SLEEP, HIGH);
  delay(2);
  
  //rotate a specific number of degrees (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (deg > 0)? HIGH:LOW;
  digitalWrite(DIR_PIN,dir); 
  
  //16 micro steps
  //long int steps = abs(deg)*(1/0.05625);
  
  //half step
  //long int steps = abs(deg)*(1/0.45);
  
  long int steps = abs(deg)*float(stepperResolution/360.0);
  
  float usDelay = (1/speed) * 150;

  //uus kiihdytysmetodi
  float step_delay=1500;
  int accel_cout=1;
  float rest=0;
  boolean maxSpeedReached=false;


  for(long int i=0; i < steps; i++){
    //Serial.print("Step_delay: ");
    //Serial.println(step_delay);
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(step_delay); 

    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(step_delay);
    if(!maxSpeedReached && accel_cout<(steps/2)){
      float step_delay_tmp=step_delay-(2*step_delay+rest)/(8*accel_cout+1);
      if(step_delay_tmp>usDelay){
            step_delay=step_delay_tmp;
            rest=int(int(2*step_delay+rest)%int(4*accel_cout+1));
            accel_cout+=1;

      }
      else{
        maxSpeedReached=true;
      }
    }
   if(accel_cout>=(steps-i)){ //ei ole ihan samanlainen kuin nousu
          //Serial.println("Hidastuu...");           
           float step_delay_tmp=step_delay+(4*step_delay+rest)/(8*accel_cout+1);
           if(step_delay_tmp<1000){ //hitain vauhti
            step_delay=step_delay_tmp;
            rest=int(int(2*step_delay+rest)%int(4*accel_cout+1));
            accel_cout-=1;
            
         }
        
   }
  }


/*
  for(long int i=0; i < steps; i++){
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(usDelay);
  }
*/  
  digitalWrite(STEPPER_SLEEP, LOW);
}

void rotateMM(float millimeters, float speed){
  //rotateDeg((millimeters/OneRoundinMM*360),speed);
    rotateDeg((millimeters/(1.25)*360),speed);
}










