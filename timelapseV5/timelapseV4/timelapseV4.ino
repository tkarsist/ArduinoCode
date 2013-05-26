#include <Servo.h>
#define Length 20

typedef struct{
  unsigned long int atTime;
  int servoID;
  int servoSpeed;
  int servoDir; 
  long runTime;
  long idleTime;
  int repeats;
}
commandAction;

commandAction actionlist[Length];

typedef struct{
  int repeatCount;
  unsigned long int nextEvent;
  int eventType;
  int commandIndexRef;
}
servoTask;

servoTask servo1Task={
  0,0,2,0};
servoTask servo2Task={
  0,0,2,0};
servoTask servo3Task={
  0,0,2,0};
servoTask servoShutterTask={
  0,0,2,0};

Servo servo1; 
Servo servo2; 
Servo servo3;
const int ledPin=3;
const int ledPin2=5;
const int ledPinPower=2;
const int ledPinProgram=4;
const int ledPinManualForward=10;
const int ledPinManualBackward=11;
const int resetButton=6;
const int manualDirButton=12;
const int potPin=2;


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
int manualSpeed=0;

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

  //power light on
  pinMode(ledPinPower, OUTPUT);
  digitalWrite(ledPinPower, HIGH);

  //program running led
  pinMode(ledPinProgram, OUTPUT);
  
  //direction leds
  pinMode(ledPinManualForward, OUTPUT);
  pinMode(ledPinManualBackward, OUTPUT);
  digitalWrite(ledPinManualForward, HIGH);
  digitalWrite(ledPinManualBackward, HIGH);


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
  
  //potentiometrin luku
  if(programRunning==false){
    checkSpeedVal();
    checkIfManualDirButtonPressed();
  //Serial.println(analogRead(potPin));
  }

  //monitor reset program button
  checkIfResetButtonPressed();

  //servojen taskit

  if(servo1Task.nextEvent!=0 && servo1Task.nextEvent<time){
    if(servo1Task.repeatCount<actionlist[servo1Task.commandIndexRef].repeats){
      if(servo1Task.eventType==0){

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
    }
  }

  //servo2 tasks
  if(servo2Task.nextEvent!=0 && servo2Task.nextEvent<time){

    if(servo2Task.repeatCount<actionlist[servo2Task.commandIndexRef].repeats){
      if(servo2Task.eventType==0){

        v=getSpeed(actionlist[servo3Task.commandIndexRef].servoSpeed,actionlist[servo3Task.commandIndexRef].servoDir);

        servo2.writeMicroseconds(v);
        Serial.print("servo 2 start: ");
        Serial.print(servo1Task.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(time);

        v=0;

        servo2Task.eventType=1;
        servo2Task.nextEvent=servo2Task.nextEvent+actionlist[servo2Task.commandIndexRef].runTime;


      }
      else{
        Serial.print("servo 2 detach");
        servo2.writeMicroseconds(1500);

        Serial.print(servo1Task.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(time);
        servo2Task.eventType=0;
        servo2Task.nextEvent=servo2Task.nextEvent+actionlist[servo2Task.commandIndexRef].idleTime;

        servo2Task.repeatCount+=1;
      }
    }
  }

  //servo3 tasks

  if(servo3Task.nextEvent!=0 && servo3Task.nextEvent<time){

    if(servo3Task.repeatCount<actionlist[servo3Task.commandIndexRef].repeats){
      if(servo3Task.eventType==0){

        v=getSpeed(actionlist[servo3Task.commandIndexRef].servoSpeed,actionlist[servo3Task.commandIndexRef].servoDir);

        servo3.writeMicroseconds(v);
        //servo3.write(v);
        Serial.print("servo 3 start: ");
        Serial.print(servo3Task.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(time);

        v=0;

        servo3Task.eventType=1;
        servo3Task.nextEvent=servo3Task.nextEvent+actionlist[servo3Task.commandIndexRef].runTime;


      }
      else{
        Serial.print("servo 3 detach: ");
        //servo3.write(90);
        servo3.writeMicroseconds(1500);
        Serial.print(servo3Task.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(time);
        servo3Task.eventType=0;
        servo3Task.nextEvent=servo3Task.nextEvent+actionlist[servo3Task.commandIndexRef].idleTime;

        servo3Task.repeatCount+=1;
      }
    }
  }

  //servoShutter tasks

  if(servoShutterTask.nextEvent!=0 && servoShutterTask.nextEvent<time){

    if(servoShutterTask.repeatCount<actionlist[servoShutterTask.commandIndexRef].repeats){
      if(servoShutterTask.eventType==0){



        Serial.print("Take picture: ");
        Serial.print(servoShutterTask.nextEvent);
        Serial.print(" curr time: ");
        Serial.println(time);
        digitalWrite(ledPin,HIGH);
        digitalWrite(ledPin2,HIGH);


        servoShutterTask.eventType=1;
        servoShutterTask.nextEvent=servoShutterTask.nextEvent+actionlist[servoShutterTask.commandIndexRef].runTime;

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
    }
  }

  //tehdaan servotaskit
  if (actionlistIndex>0 && actionlist[actionlistNext].atTime!=0 && time>actionlist[actionlistNext].atTime){
    //   if (actionlistIndex>0 && actionlist[actionlistNext].atTime!=0){
    //Serial.println(actionlist[actionlistNext].atTime);
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
      if (actionlist[actionlistNext].servoID==2){

        servo2Task.repeatCount=0;
        servo2Task.nextEvent=actionlist[actionlistNext].atTime;
        servo2Task.eventType=0;
        servo2Task.commandIndexRef=actionlistNext;
        Serial.println("Servotaskkurat 2: nextEvent, eventType, repeat count, commandindexref");
        Serial.println(servo2Task.nextEvent);
        Serial.println(servo2Task.eventType);
        Serial.println(servo2Task.repeatCount);
        Serial.println(servo2Task.commandIndexRef);
        tmpIndex=1;
      }      
      if (actionlist[actionlistNext].servoID==3){

        servo3Task.repeatCount=0;
        servo3Task.nextEvent=actionlist[actionlistNext].atTime;
        servo3Task.eventType=0;
        servo3Task.commandIndexRef=actionlistNext;
        Serial.println("Servotaskkurat 3: nextEvent, eventType, repeat count, commandindexref");
        Serial.println(servo3Task.nextEvent);
        Serial.println(servo3Task.eventType);
        Serial.println(servo3Task.repeatCount);
        Serial.println(servo3Task.commandIndexRef);
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

      sanoma="";
      sanomaReady=false;
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
    delay(1000);
    digitalWrite(ledPinProgram, LOW);
    delay(1000);
    digitalWrite(ledPinProgram, HIGH);
    delay(1000);
    digitalWrite(ledPinProgram, LOW);


  }
}

void checkIfManualDirButtonPressed(){
int buttonval=digitalRead(manualDirButton);
if(buttonval==LOW){
  while(digitalRead(manualDirButton)==LOW){
  }
  
  if(manualDir==true){
  manualDir=false;
  }
  else{
  manualDir=true;
  }
  Serial.println(manualDir);
}
}

void checkSpeedVal(){
  int tolerance=10;
  int analogSpeed=analogRead(potPin);
  if(abs(manualSpeed-analogSpeed)>tolerance){
    manualSpeed=analogRead(potPin);
    Serial.println(manualSpeed);
  }
}

void checkManualDirLed(){
  if(programRunning){
    digitalWrite(ledPinManualForward, LOW);
    digitalWrite(ledPinManualForward, HIGH);
  }
  else{
 if(manualDir==true && manualDirOld!=true){
   digitalWrite(ledPinManualForward, HIGH);
   manualDirOld=manualDir;

 }
if(manualDir==false && manualDirOld!=false){
   digitalWrite(ledPinManualBackward, HIGH);
   manualDirOld=manualDir;
}
} 
}









