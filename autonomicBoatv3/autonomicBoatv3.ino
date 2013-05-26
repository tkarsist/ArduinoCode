#include <Servo.h> 
#include <EEPROM.h>
#include <Arduino.h>
#include "EEPROMAnything.h"

//magnetometri start

#include <Wire.h>
#define MAG_ADDR   0x0E // 7-bit address for MAG3110

// magnetometrin viimeisin suuntima asteina 1-360 
int magn_currentBearing=0;

// magnetometrin viimeisimmat raakamittaukset
int magn_xval=0;
int magn_yval=0;
int magn_zval=0;

// magnetometrin min-max arvot akseleille jotka maarittyvat kalibroinnissa
int magn_xmax=-32000;
int magn_xmin=32000;
int magn_ymax=-32000;
int magn_ymin=32000;
int magn_zmax=-32000;
int magn_zmin=32000;


//magnoto stop
 
Servo servo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 

Servo motor;
int servoPos = 0;    // variable to store the servo position 
int servoPin=13;
int motorPin=9;
unsigned long int curTime;
unsigned long int lastTime=0;
int updateBearingInterval=200;
int showCurrentBearing=2000;
unsigned long int showBearingLast=0;
int targetBearing=0;

//backup muuttuja, jos magnetometri ei toimaa ollenkaan
int measuredBearing=0;
//int currentBearing=0;
int bearingThreshold=15;
float servoTurnCorFactor=1.0;
String sanoma = "";
boolean sanomaReady = false;
boolean sanomaOngoing = false;
boolean correctionOngoing=false;

//simulate behaviour
unsigned long lastSimUpdate=0;
int simBearing=0;
 
void setup() 
{ 
  
    
  // magnetometrille Wire kirjasto I2C:
  Wire.begin();
  
  // magnetometrin alustus
  magn_config();

  //magneto stop 
  
  servo.attach(servoPin);  // attaches the servo on pin 9 to the servo object 
  motor.attach(motorPin);
  motor.write(0);
    Serial.begin(57600);
  Serial.println("-----");
  Serial.println("Ready");
  Serial.println("Syntax:");
  Serial.println("MO000=motor+speed , SE000=servo+angle, SS000=servo+angle, BE000=targetbearing+value, MB000=measured bearing + value, CA000=calibrate magnetometer, GX=gps latitude, GY=gps longitude");
    EEPROMReadConf();

} 
 
 
void loop() { 
  curTime=millis();
  
  //paivitetaan currentBearing
  int currentBearing=getCurrentBearing();
  
  //printaaa mika on nykyinen suuntima
  if((curTime-showBearingLast)>showCurrentBearing){
    showBearingLast=curTime;
    Serial.print("Current bearing: ");
    Serial.println(currentBearing);
  } 

  //pitaako tehda korjauksia
  if(targetBearing!=0){
  if((curTime-lastTime)>updateBearingInterval){
    lastTime=curTime;
  checkAndRotateServo(currentBearing);
  }
  //korjaus kaynnissa, pitaako se lopettaa
  if(correctionOngoing){
    checkWhetherToEndCor(currentBearing);
  
  }
  }
  //sarjaliikenteen lukeminen
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
   magn_read();

  // palauttaa asteina nykyisen suunnan magnetometrista
  // arvot 1-360
  
  int corryval=0;
  int corrxval=0;
  
  
  
  corryval=magn_yval-((magn_ymax+magn_ymin)/2);
  corrxval=magn_xval-((magn_xmax+magn_xmin)/2);
  
  float heading=atan2(corryval,corrxval);
  
  if (heading<0) heading+=2*PI;
  
  heading=heading*180/M_PI;  //radiaanit asteina
  
  // jos tulee hasardiarvoja , kaytetaan vanhaa arvoa
  if (heading>360) heading=magn_currentBearing;
  if (heading<1) heading=magn_currentBearing;
  

  return (int)heading;

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

//MO000=motor+speed , SE000=servo+angle, SS000=servo+angle, BE000=targetbearing+value, MB000=measured bearing + value, CA000=calibrate magnetometer, GX=gps latitude, GY=gps longitude
  
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
  /*int adjustedSpeed=100;
  if(speed!=0){
    adjustedSpeed=110+(speed/8);
     }
  motor.write(adjustedSpeed);
  Serial.println(adjustedSpeed);
*/
motor.write(speed);
}

void setServoAngle(int angle){
  Serial.print("Setting servo angle to: ");
  if(angle>130){
    angle=130;
  }
  if(angle<50){
    angle=50;
  }
  Serial.println(angle);
servo.write(angle);
}

void calibrateCompass(void) {
  // 30 sekunnin kalibrointi, pyorittele laitetta ympariinsa sen aikaa
  // jotta loytaa min max arvot kaikille akseleille
  
  long calibrDuration=30000; // kalibroidaan 30 sek
  long startTime=millis();
  
  Serial.println("Kompassi:Kalibroidaan 30 sek..");
  
  while (millis()-startTime<calibrDuration) {
      
     //etsitaan max min arvoja 30 sek
      magn_read();
      if (magn_xval>magn_xmax) { magn_xmax=magn_xval; }
      if (magn_xval<magn_xmin) { magn_xmin=magn_xval; }
      
      if (magn_yval>magn_ymax) { magn_ymax=magn_yval; }
      if (magn_yval<magn_ymin) { magn_ymin=magn_yval; }
      
      if (magn_zval>magn_zmax) { magn_zmax=magn_zval; }
      if (magn_zval<magn_zmin) { magn_zmin=magn_zval; }
  
      magn_printValues();           
      Serial.print("Kompassi:Kalibrointia jaljella:");          
      Serial.println(calibrDuration-(millis()-startTime));
      delay(20);
  }
  
  Serial.println("Kompassi: Kalibrointi valmis!");
  EEPROMWriteConf();
  Serial.println("Configuration written to EEPROM");
}


void magn_read(void) {
  //lukee magnetometrin raaka-arvot globaaleihin muuttujiin
  // otetaan 5 mittausta joka akselille ja keskiarvo niista
  int magxsum=0;
  int magysum=0;
  int magzsum=0;
  
  for (int i=0;i<5;i++) {
    magxsum=magxsum+magn_readx();
    magysum=magysum+magn_ready();
    magzsum=magzsum+magn_readz();
  }
  magn_xval=magxsum/5;
  magn_yval=magysum/5;
  magn_zval=magzsum/5;
}

void magn_config() {
 // magnetometrin alkukonffaus 
 Wire.beginTransmission(MAG_ADDR);
 Wire.write(0x11);
 Wire.write(0x80);
 Wire.endTransmission();
 delay(15);
 
 Wire.beginTransmission(MAG_ADDR);
 Wire.write(0x10);
 Wire.write(1);
 Wire.endTransmission();
    
}

void magn_printValues(void) {
  // tulostaa magnetometrin arvot serialiin

  
  Serial.print("x=");
  Serial.print(magn_xval);
  Serial.print(",");
  Serial.print("y=");
  Serial.print(magn_yval);
  Serial.print(",");
  Serial.print("z=");
  Serial.println(magn_zval);
  
  Serial.print("maxx=");
  Serial.print(magn_xmax);
  Serial.print(",");
  Serial.print("maxy=");
  Serial.print(magn_ymax);
  Serial.print(",");
  Serial.print("maxz=");
  Serial.println(magn_zmax);

  Serial.print("minx=");
  Serial.print(magn_xmin);
  Serial.print(",");
  Serial.print("miny=");
  Serial.print(magn_ymin);
  Serial.print(",");
  Serial.print("minz=");
  Serial.println(magn_zmin);


  Serial.println(getCurrentBearing());
  
}

int magn_readx(void) {
  int x1,xh;
  
  Wire.beginTransmission(MAG_ADDR);  
  Wire.write(0x01);
  Wire.endTransmission();
  
  delayMicroseconds(2);
 
  Wire.requestFrom(MAG_ADDR,1);
  while (Wire.available()) {
   xh = Wire.read();
   
  }
  
  delayMicroseconds(2);
 
  Wire.beginTransmission(MAG_ADDR);  
  Wire.write(0x02);
  Wire.endTransmission();
  
  delayMicroseconds(2);
  
  Wire.requestFrom(MAG_ADDR,1);
  while (Wire.available()) {
   x1 = Wire.read();
   
  }
  
  int xout = (x1 | (xh <<8));

  
  return xout;
}

int magn_ready(void) {
  int y1,yh;
  
  Wire.beginTransmission(MAG_ADDR);  
  Wire.write(0x03);
  Wire.endTransmission();
  
  delayMicroseconds(2);
 
  Wire.requestFrom(MAG_ADDR,1);
  while (Wire.available()) {
   yh = Wire.read();
   
  }
  
  delayMicroseconds(2);
 
  Wire.beginTransmission(MAG_ADDR);  
  Wire.write(0x04);
  Wire.endTransmission();
  
  delayMicroseconds(2);
  
  Wire.requestFrom(MAG_ADDR,1);
  while (Wire.available()) {
   y1 = Wire.read();
   
  }
  
  int yout = (y1 | (yh <<8));
  return yout;
}


int magn_readz(void) {
  int z1,zh;
  
  Wire.beginTransmission(MAG_ADDR);  
  Wire.write(0x05);
  Wire.endTransmission();
  
  delayMicroseconds(2);
 
  Wire.requestFrom(MAG_ADDR,1);
  while (Wire.available()) {
   zh = Wire.read();
   
  }
  
  delayMicroseconds(2);
 
  Wire.beginTransmission(MAG_ADDR);  
  Wire.write(0x06);
  Wire.endTransmission();
  
  delayMicroseconds(2);
  
  Wire.requestFrom(MAG_ADDR,1);
  while (Wire.available()) {
   z1 = Wire.read();
   
  }
  
  int zout = (z1 | (zh <<8));
  return zout;
}

void EEPROMWriteConf(){
  
  int writed=1;
  EEPROM_writeAnything(0,writed);
  EEPROM_writeAnything(6,magn_xmax);
  EEPROM_writeAnything(14,magn_xmin);
  EEPROM_writeAnything(20,magn_ymax);
  EEPROM_writeAnything(26,magn_ymin);
  EEPROM_writeAnything(32,magn_zmax);
  EEPROM_writeAnything(38,magn_zmin);
  Serial.println("All magnetometer written to eeprom");
  
}

void EEPROMReadConf(){
  int writed=0;
  EEPROM_readAnything(0,writed);
  Serial.println(writed);
  if(writed==1){
  EEPROM_readAnything(6,magn_xmax);
  EEPROM_readAnything(14,magn_xmin);
  EEPROM_readAnything(20,magn_ymax);
  EEPROM_readAnything(26,magn_ymin);
  EEPROM_readAnything(32,magn_zmax);
  EEPROM_readAnything(38,magn_zmin);
  Serial.println("MAgnetometer from EEPROM: Xmax, Xmin, YMax,Ymin,Zmax,Zmin");
  Serial.println(magn_xmax);
  Serial.println(magn_xmin);
  Serial.println(magn_ymax);
  Serial.println(magn_ymin);
  Serial.println(magn_zmax);
  Serial.println(magn_zmin);
  
  }

}
