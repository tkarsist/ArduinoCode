#include <Servo.h>
#define Length 100

typedef struct{
  unsigned long int atTime;
  int servoID;
  int servoSpeed;
  int servoDir; 
}
commandAction;

commandAction actionlist[Length];


Servo servo1; 
Servo servo2; 
String sanoma="";
boolean sanomaReady=false;
boolean sanomaOngoing=false;
int actionlistIndex=0;
int actionlistNext=0;

void setup() {

  pinMode(1,OUTPUT);
  servo1.attach(9); //analog pin 0
  //servo1.setMaximumPulse(2000);
  //servo1.setMinimumPulse(700);


  servo2.attach(3); //analog pin 1
  Serial.begin(19200);
  Serial.println("Ready");

}

void loop() {

  //servo write 89 -0 toinen suunta, 90-179 toinen suunta
  static int v = 0;
  static int v2 = 0;
  static int servoID=9;
  static int servoID2=3;
  static int sign=1;
  //static int rotatetime=0;
  static unsigned long time;
  time= millis();


  int i=0;    


  if (actionlistIndex>0 && actionlist[actionlistNext].atTime!=0 && time>actionlist[actionlistNext].atTime){
    Serial.println(actionlist[actionlistNext].atTime);
    int tmpIndex=0;
    if (actionlist[actionlistNext].servoSpeed>0){
      //Serial.println("tuli tanne");

      if (actionlist[actionlistNext].servoID==1){
        v=actionlist[actionlistNext].servoSpeed*10-2;
        sign=actionlist[actionlistNext].servoDir;

        if(sign==1){
          sign=-0;
          v=187 - v;
        }

        servo1.attach(servoID);


        servo1.write(v);
        Serial.println("servo start");

        v=0;
        tmpIndex=1;
      }





    }
    if(actionlist[actionlistNext].servoSpeed==0){
      if(actionlist[actionlistNext].servoID==1){
        Serial.println("servo detach");
        servo1.detach();
        tmpIndex=1;
      }


    }
    if(tmpIndex!=0){
      actionlistNext=actionlistNext+1;
      Serial.println(actionlistNext);

    }

  }





  if ( Serial.available()) {
    char ch = Serial.read();

    //viestin alku
    if(ch == '<'){
      sanomaOngoing=true;

    }
    //viestin loppu
    if (ch == '>'){
      sanomaOngoing=false;
      sanomaReady=true;

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
      unsigned long int tmpAtTime=atol(charAtTime);
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

      actionlist[actionlistIndex].atTime=tmpAtTime;
      actionlist[actionlistIndex].servoID=tmpServoId;
      actionlist[actionlistIndex].servoSpeed=tmpServoSpeed;
      actionlist[actionlistIndex].servoDir=tmpServoDir;

      actionlistIndex=actionlistIndex+1;
      Serial.println(actionlistIndex);

      sanoma="";
      sanomaReady=false;
    } 

  }


}





