#include <ADXL345.h>
#include <HMC58X3.h>
#include <ITG3200.h>
#include <bma180.h>
#include <MS561101BA.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Math.h>
#include "Arduino.h"
#include "Kalman.h"

#define DEBUG
#ifdef DEBUG
#include "DebugUtils.h"
#endif

#include "CommunicationUtils.h"
#include "FreeIMU.h"

#include <Wire.h>
#include <Servo.h> 

Kalman kalmanPitch;
Kalman kalmanRoll;
Kalman kalmanYaw;

 
Servo servoLeft;
Servo servoRight;
int servoUpdateInterval=20;
unsigned long int lastServoUpdate=0;
int servoLeftSpeed=90;
int servoRightSpeed=90;
float pitchTolerance=2.0;



float q[4];




//Oma kalmani
float XXX[9];
unsigned long int startT=0;
unsigned long int loopTime=20;

float Euleri[3];
int RaakaArvot[10];

///
// Set the FreeIMU object
FreeIMU my3IMU = FreeIMU();

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(5);
  my3IMU.init();
  delay(5);
  servoLeft.attach(10);
  servoRight.attach(9);
  kalmanPitch.setAngle(0);
  servoLeft.write(90);
  delay(50);
  servoRight.write(90);
  delay(100);
  servoLeft.write(65);
  delay(50);
  servoRight.write(65);
  delay(100);
    servoLeft.write(90);
  delay(50);
  servoRight.write(90);
  delay(100);
  servoLeft.detach();
  servoRight.detach();

}

float minRoll=32000.0;
float marollAngle=-32000.0;

float minRollKalman=32000.0;
float marollAngleKalman=-32000.0;

float minGyro=32000.0;
float maxGyro=-32000.0;

float accZ,accY,accX,gyroX,gyroY,gyroZ,magnX,magnY,magnZ,yawAngle,rollAngle,pitchAngle,myRoll,myPitch,yawXX,heading, magnNorm,cosPitch,sinPitch,cosRoll,sinRoll;



unsigned long int printTimer=0;
int printInterval=50;
unsigned long int loopCounter=0;
float averageLoopTime=0.0;
float Y,X;

//0 for normal serial, 1 for all yaw pitch roll, 2 for cube yaw pitch roll, 3 for loop stats
int printRawVal=0;

//kumpaa kaytetaan ypr:aan. 0=freeimu, 1=oma kalmani
byte yprFilter=1;

void loop() { 

  //m3IMU.getRawValues(XXX);
  
  
  startT=micros();
  if(yprFilter==1){
  //my3IMU.getQ(q);
  my3IMU.getValues(XXX);
  //my3IMU.getEuler(Euleri);
  
  //laita paalle jos haluat vertailun
  //my3IMU.getYawPitchRoll(Euleri);
  
  accX=XXX[0];
  accY=XXX[1];
  accZ=XXX[2];
  magnX=XXX[6];
  magnY=XXX[7];
  magnZ=XXX[8];
  magnNorm = invSqrt(magnX * magnX + magnY * magnY + magnZ * magnZ);

  gyroX=float(XXX[3]);
  gyroY=float(XXX[4]);
  gyroZ=float(XXX[5]);
  
  
  magnX/=magnNorm;
  magnY/=magnNorm;
  magnZ/=magnNorm;
  

 cosPitch=cos(pitchAngle);
 sinPitch=sin(pitchAngle);
 cosRoll=cos(rollAngle);
 sinRoll=sin(rollAngle);
 
 X=magnX*cosPitch-magnZ*sinPitch;
 Y=magnX*sinRoll*sinPitch+magnY*cosRoll-magnZ*sinRoll*cosPitch;
 
 heading=atan2(-Y,X);

  //joku oikea tapa: Tuossa on vakio kerroin, koska jos Gz on 0, niin tulee joku jakotulos
  
  myRoll = atan2 (accY,sqrt(accZ*accZ+1.0*accX*accX)); //pitas olla -180 ja 180 valilla
  myPitch =atan2 (accX,sqrt(accZ*accZ+accY*accY)); //pitchin pitas olla -90 ja 90 valilla
 
  rollAngle=kalmanRoll.getAngle(myRoll*180.0/M_PI,gyroX,(double)(micros()-startT)/1000000);
  pitchAngle=kalmanPitch.getAngle(myPitch*180.0/M_PI, gyroY, (double)(micros()-startT)/1000000);
  yawAngle=kalmanYaw.getAngle(heading*180.0/M_PI,gyroZ,(double)(micros()-startT)/1000000);
  }
  if(yprFilter==0){
    
      //laita paalle jos haluat vertailun
  my3IMU.getYawPitchRoll(Euleri);
  yawAngle=Euleri[0];
  pitchAngle=Euleri[1];
  rollAngle=Euleri[2];
  }
loopTime=micros()-startT;

if((millis()-printTimer)>printInterval){
  
  printTimer=millis();
  
  if(printRawVal==1){
    printRaw();
  }
  if(printRawVal==0){
  printNormal();
  }
  if(printRawVal==2){
  printRawYPR();
  }


}
  loopCounter+=1;
  if(averageLoopTime>0){
    averageLoopTime=averageLoopTime*1.0*(loopCounter-1)/loopCounter*1.0+1.0*loopTime/loopCounter*1.0;
  }
  else
    averageLoopTime=loopTime;
  if(printRawVal==3){
    printLoopStats();
  }

}

void printLoopStats(){
Serial.println(averageLoopTime);
}

void printRaw(){
  float qX[12];
  qX[0]=rollAngle;
  qX[1]=pitchAngle;
  qX[2]=yawAngle;
  //qX[2]=0.0;
  qX[3]=accX;
  qX[4]=accY;
  qX[5]=accZ;
  qX[6]=magnX;
  qX[7]=magnY;
  qX[8]=magnZ;
  qX[9]=gyroX;
  qX[10]=gyroY;
  qX[11]=gyroZ;
  
  serialPrintFloatArr(qX, 12);
  Serial.println("");

}

void printRawYPR(){
  float qX[4];
  qX[0]=rollAngle;
  qX[1]=pitchAngle;
  qX[2]=yawAngle;
  qX[3]=0.0;
  
  serialPrintFloatArr(qX,4);
  Serial.println("");
}

void printNormal(){

  Serial.print(yawAngle); //yawAngle
Serial.print(" ");  
  Serial.print(pitchAngle);
  Serial.print(" ");

  Serial.print(rollAngle);

    Serial.print(" ");




  Serial.println("");

}

