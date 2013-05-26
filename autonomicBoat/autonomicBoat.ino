#include <Servo.h> 
 
Servo servo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 

Servo motor;
int servoPos = 0;    // variable to store the servo position 
int servoPin=3;
int motorPin=9;
unsigned long int curTime;
unsigned long int lastTime=0;
int updateBearingInterval=100;
int targetBearing=0;

//backup muuttuja, jos magnetometri ei toimaa ollenkaan
int measuredBearing=0;
//int currentBearing=0;
int bearingThreshold=5;
float servoTurnCorFactor=5.0;
String sanoma = "";
boolean sanomaReady = false;
boolean sanomaOngoing = false;
boolean correctionOngoing=false;

//simulate behaviour
unsigned long lastSimUpdate=0;
int simBearing=0;
 
void setup() 
{ 
  servo.attach(servoPin);  // attaches the servo on pin 9 to the servo object 
  motor.attach(motorPin);
  motor.write(100);
    Serial.begin(19200);
  Serial.println("-----");
  Serial.println("Ready");
} 
 
 
void loop() { 
  curTime=millis();
  
  int currentBearing=getCurrentBearing();
  if(targetBearing!=0){
  if((curTime-lastTime)>updateBearingInterval){
    lastTime=curTime;
  checkAndRotateServo(currentBearing);
  }
  if(correctionOngoing){
    checkWhetherToEndCor(currentBearing);
  
  }
  }
  readSerialMessage();
  
  
}

void checkAndRotateServo(int currentBearing){
  if(!correctionOngoing && abs(targetBearing-currentBearing)>bearingThreshold){
      Serial.println("I will rotate servo because going to wrong direction. Targetbearing: ");
     Serial.print(targetBearing);
    Serial.print(" Currentbearing: ");
   Serial.println(currentBearing); 
      int bearingError=targetBearing-currentBearing;
      int servoAngleChange=abs(bearingError)/4*servoTurnCorFactor;
      if(bearingError<0){
        if(servoAngleChange<90){
        servo.write(90-servoAngleChange);
        Serial.print("Correction angle: ");
        Serial.println(90-servoAngleChange);
        }
        else
        servo.write(1);
      }
      if(bearingError>0){
        if(servoAngleChange<90){
        servo.write(90+servoAngleChange);
         Serial.print("Correction angle: ");
        Serial.println(90+servoAngleChange);
        }
        else
        servo.write(179);
        
      }
      
      correctionOngoing=true;
  }
 
}
void checkWhetherToEndCor(int currentBearing){
    if(abs(targetBearing-currentBearing)<=bearingThreshold){
    correctionOngoing=false;
    Serial.println("Going to right direction");
    servo.write(90);
    }
}

int getCurrentBearing(){
  if(measuredBearing!=0){
    return measuredBearing;
  }
  if(correctionOngoing && (millis()-lastSimUpdate)>30){
    if((targetBearing-simBearing)>0){
      simBearing+=1;

    }
    else{
      simBearing-=1;
    }
    lastSimUpdate=millis();
          Serial.print("Target Bearing: ");
      Serial.println(targetBearing);
      Serial.print("Sim bearing: ");
      Serial.print(simBearing);
    return simBearing;
  }
  else{
    //int bearing=200;
    //Serial.print("New target bearing: ");
   // Serial.println(bearing);
   return simBearing;
  }
}

void readSerialMessage(){
 if (Serial.available()) {
  char ch = Serial.read();

  if (ch == '!') {
    Serial.println("Program reset");
    //resetActionList();
    //clearEEPROM();
    //resetProgram();
  }
  //viestin alku
  if (ch == '<') {
    sanomaOngoing = true;
  }
  //viestin loppu
  if (ch == '>') {
    sanomaOngoing = false;
    sanomaReady = true;
    //eventsReceived += 1;
  }

  //lisataan merkit sanomaan    
  if (sanomaOngoing && ch != '<') {
    sanoma = sanoma + ch;
  }
  //sanoma valmis
  if (sanomaReady) {
    processMessage(sanoma);
    sanoma = "";
    sanomaReady = false;
  }
 }
}

void processMessage(String message){
  Serial.print("sanoma: ");
  Serial.println(message);

  //Task start time (7 char in seconds)
  String stringCommand = message.substring(0, 2);
  Serial.print("Command is: ");
  Serial.println(stringCommand);

//MO=motor+speed , SE=servo+angle, SS=servo+angle, BE=targetbearing+value, MB=measured bearing + value, CA=calibrate magnetometer, GX=gps latitude, GY=gps longitude
  
if(stringCommand=="MO"){
     setMotorSpeed(getIntFromString(message));
}
if(stringCommand=="SE"){
     setServoAngle(getIntFromString(message));
}

if(stringCommand=="SS"){
     setServoAngle(getIntFromString(message));
     delay(1000);
     setServoAngle(90);
}

if(stringCommand=="BE"){
     targetBearing=getIntFromString(message);
     Serial.print("Setting targetbearing to: ");
     Serial.println(targetBearing);
     
}
if(stringCommand=="MB"){
  measuredBearing=getIntFromString(message);
  Serial.print("Overriding measured bearing to: ");
  Serial.println(measuredBearing);
  
}

if(stringCommand=="CA"){
  Serial.print("Calibrating compass ... ");
  calibrateCompass();
}

if(stringCommand=="GX"){
  getDoubleFromString(message);

}

if(stringCommand="GY"){
  getDoubleFromString(message);
}

  
}

double getDoubleFromString(String message){
  
  double eka=0;
  double toka=0;
    char charValue[3];
  String stringValue = message.substring(2, 4);
  Serial.println(stringValue);
  stringValue.toCharArray(charValue, sizeof(charValue));
    eka=atol(charValue)+0.0;
    eka=eka/1.0;
  Serial.println(eka);

    
        char charValue2[7];
  String stringValue2 = message.substring(5, 11);
  Serial.println(stringValue);
  stringValue2.toCharArray(charValue2, sizeof(charValue2));
    toka=atol(charValue2)+0.0;
    Serial.println(toka);
    Serial.println(1.0*toka/1000000,7);
    double kolmas=1.0*toka/1000000;
    
    double tmpValue = eka+kolmas;
    
  //unsigned long int tmpEndTime=atol(charEndTime)*1000+timeOffset;
  
  Serial.print("floatVal: ");
  Serial.println(tmpValue,7);
  return (double)tmpValue;
  
}

int getIntFromString(String message){
    //Task end time (7 char in seconds)
  char charValue[4];
  String stringValue = message.substring(2, 5);
  stringValue.toCharArray(charValue, sizeof(charValue));
  //unsigned long int tmpEndTime=atol(charEndTime)*1000+timeOffset;
  int tmpValue = atoi(charValue);
  Serial.print("Integer value: ");
  Serial.println(tmpValue);
  return tmpValue;
  
}

void setMotorSpeed(int speed){
  Serial.print("Setting motor speed to");
  int adjustedSpeed=100;
  if(speed!=0){
    adjustedSpeed=110+(speed/8);
     }
  motor.write(adjustedSpeed);
  Serial.println(adjustedSpeed);

}

void setServoAngle(int angle){
  Serial.print("Setting servo angle to: ");
  Serial.println(angle);
servo.write(angle);
}

void calibrateCompass(){
 for (int i=0; i<30;i++){
   Serial.println("Vittu mä kalibroin servoa, heiluta sitä lapaa");
   delay(500);
 } 
}



