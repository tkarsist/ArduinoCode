#include <Servo.h>

Servo servo1; Servo servo2; 


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
  static int rotatetime=0;
  static unsigned long time;
  time= millis();
  
  
  
  static unsigned long detachS1;
  
  //detachS1=0;
  
  if(detachS1!=0 && time>detachS1){

      Serial.println("Detaching Servo1");
      servo1.detach();
      detachS1=0;
    
  }
  
  
  //      static char numstr[21];
  //      static String minus;
  //      static String result;

  if ( Serial.available()) {
    char ch = Serial.read();

//switch logic: direction (-, optional), time (optional: s,m,l), speed: 0..9 (optional), servo: a-x) 

    switch(ch) {

      //direction to rotate
      case '-':
        sign=-1*sign;
        Serial.println(sign, DEC);

        break;

      //small time to rotate
      case 's':
      //115
      //Serial.println(ch, DEC);
      rotatetime=500;
      detachS1=time+500;
      break;
      
      //medium time to rotate
      case 'm':
      //109
      //Serial.println(ch, DEC);
      rotatetime=5000;
      detachS1=time+5000;

      break;
      
      //large time to rotate
      case 'l':
      //108
      //Serial.println(ch, DEC);
      detachS1=time+20000;
      rotatetime=20000;
      break;

        
        //servo speed for continuous. 0 is fastest and 9 slowest
      case '0'...'9':
        //char merkit on koodilta 48-57
        //v = v * 100 + ch - '0';
        //v=ch-40;
        if(ch==48){
          v=1;
        }
        else{
        v=ch-48;
        }
        v=v*10-2;
        
        v2=ch-48;
        v2=v2*40-1;
        
        
        v2=v2*sign;
        if(sign==-1){
        sign=-1*sign;
        v=187 - v;
        }
        Serial.println("");
        Serial.println (" ---------- ");
        
        Serial.print("'0-9' :");
        Serial.print(" CH:");
        Serial.print(ch, DEC);
        Serial.print(" V:");
        Serial.print(v, DEC);
        Serial.print( " V2:");
        Serial.print(v2, DEC);
        Serial.println(" . Case 0-9 --END");
        //v=v+ch -'0';
        break;

      // servo a (there are servos a,b,c,d,e,f,g)
      case 'a':
        Serial.print("'A' :");
        Serial.print(" V:");
        Serial.println(v, DEC);
        servo1.attach(servoID);
        
        /*if(rotatetime!=0){
          Serial.println("rotatetime ei nolla");
        servo1.writeMicroseconds(rotatetime);
        delay(500);
        servo1.detach();
        rotatetime=0;
        }
        else{
        */
        servo1.write(v);

/* toimii
        delay(rotatetime);
        if(rotatetime!=0){
        servo1.detach();
        rotatetime=0;
        }
*/        
        // if no speed then detach
        if(v==0){
        servo1.detach();
        }
        v = 0;
        //servo1.detach();
        break;
      

      
      case 'w':
        servo2.write(v);
        delay(15);
        v = 0;
        break;
      
      //detach all servos
      case 'd':
        servo1.detach();
        break;
      //attach all servos
      case 'y':
        servo1.detach();
        servo1.attach(servoID);
        break;
      case 't':
        servo2.write(1);
        delay(500);
        servo2.write(90);
        delay(500);
        servo2.write(170);
        delay(500);
        v = 0;
        break;
      case 'x':
        Serial.print("'x' :");
        Serial.println(v2, DEC);
        servo2.write(v2);
        delay(500);
        v = 0;
        v2 =0;
        break;
    }




  }
  
  

  //Servo::refresh();

} 
