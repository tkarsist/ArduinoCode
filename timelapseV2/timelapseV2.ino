#include <Servo.h>
#define Length 10

typedef struct{
 unsigned long int atTime;
 int servoID;
 int servoSpeed;
 int servoDir; 
}commandAction;

commandAction actionlist[Length];

//struct commandAction actionlist[1000];


Servo servo1; Servo servo2; 
String sanoma="";
boolean sanomaReady=false;
boolean sanomaOngoing=false;
int actionlistIndex=0;
int actionlistNext=0;

//commandAction actionlist[];

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
  
 /* 
  if(actionlistIndex>0){
  Serial.println(actionlist[actionlistNext].atTime);
  Serial.println(time);
  }
 */ 
  
  
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
                    //Serial.println(sanoma.substring(0,10));
                    
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
                   //Serial.println(actionlist[actionlistIndex].atTime);
                    /*
                    commandAction tmpCommandAction;
                    //tmpCommandAction.atTime=19191991;
                    tmpCommandAction.atTime=tmpAtTime;
                    tmpCommandAction.servoID=tmpServoId;
                    tmpCommandAction.servoSpeed=tmpServoSpeed;
                    tmpCommandAction.servoDir=tmpServoDir;
                    //actionlist[0]=tmpCommandAction;
                    */
                    actionlistIndex=actionlistIndex+1;
                    Serial.println(actionlistIndex);
                    
                    sanoma="";
                    sanomaReady=false;
            } 
  //static unsigned long detachS1;
  
  //detachS1=0;
  
  /*
  if(detachS1!=0 && time>detachS1){

      Serial.println("Detaching Servo1");
      servo1.detach();
      detachS1=0;
    
  }
  */

/*



    if(ch == '<') {
      
        i=0;
        Serial.println(Serial.available());
        if(Serial.available()){
        ch=Serial.read();
        Serial.println("hottii shittii");
        }
        //while (!sanomaValmis)
        while (Serial.available() && !sanomaValmis && i<10){            
          ch = Serial.read();
               //Serial.println("osuma1.5");
          
          
          //oliko loppumerkki
          if (ch =='>') { 
                  Serial.println("osuma2");

            sanomaValmis=true;
            Serial.println(sanoma); 
          } 
          else {
                  
            Serial.println(ch);
            sanoma+=ch;
            // strArray[i]=val; //laitetaan merkki bufferiin
          }
          
          i++; 
    
        }
    }
    sanomaValmis=false;
    sanoma="";
    */
  }


//switch logic: direction (-, optional), time (optional: s,m,l), speed: 0..9 (optional), servo: a-x) 

/*
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
        

        servo1.write(v);

        if(v==0){
        servo1.detach();
        }
        v = 0;
        break;
            
      //detach all servos
      case 'd':
        servo1.detach();
        break;
      //attach all servos
      case 'x':
        Serial.print("'x' :");
        Serial.println(v2, DEC);
        servo2.write(v2);
        delay(500);
        v = 0;
        v2 =0;
        break;
    }

*/


  }
  
  

 
