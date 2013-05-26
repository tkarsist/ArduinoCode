#include <Servo.h>

Servo servo1; Servo servo2; Servo servo3;


void setup() {

  pinMode(1,OUTPUT);
  servo1.attach(9); //analog pin 0
  //servo1.setMaximumPulse(2000);
  //servo1.setMinimumPulse(700);
    
  servo3.attach(10);

  servo2.attach(11); //analog pin 1
  Serial.begin(19200);
  Serial.println("Ready");
  Serial.println("0-9 to give addition, - sign to change. C to start and S to stop calibration)

}

void loop() {

  //servo write 89 -0 toinen suunta, 90-179 toinen suunta
  static int v = 0;
  static int v2 = 0;
  static int v3= 0;
  static int servoID=9;
  static int servoID2=11;
  static int servoID3=10;
  static int sign=1;
  static int temp;
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

        
        //servo speed for continuous. 0 is fastest and 9 slowest
      case '0'...'9':
        //char merkit on koodilta 48-57
        temp=ch-48;
        temp=temp*sign;
        v = v + temp;
        Serial.print("V: ");
        Serial.println(v);
        break;

        
        case 'c':
        Serial.print("'calibrate' : servos v:");
        Serial.println(v);
        servo1.attach(servoID);
        servo2.attach(servoID2);
        servo3.attach(servoID3);
        servo1.write(v);
        servo2.write(v);
        servo3.write(v);
        

        break;
        case 's':
        servo1.detach();
        servo2.detach();
        servo3.detach();
        break;
    }




  }
  
  

  //Servo::refresh();

} 
