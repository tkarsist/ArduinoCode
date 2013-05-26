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
int printRawVal=3;

void loop() { 

  //m3IMU.getRawValues(XXX);
  
  
  startT=micros();
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
  
  
  

  
  
 
  
  //if(abs(myRoll)>0.3 && abs(myPitch)>0.3){

  //Y=magnY*cos(myPitch/180.0*M_PI)+magnZ*sin(myPitch/180.0*M_PI);
  //X=magnX*cos(myRoll/180.0*M_PI)+magnY*sin(myRoll/180.0*M_PI)*sin(myPitch/180.0*M_PI)-magnZ*cos(myPitch/180.0*M_PI)*sin(myRoll/180.0*M_PI);
  //X=magnX-abs(myPitch)/100*15;
  
  //X=magnX-myRoll/180*M_PI*0.8;
  
  //X=magnX*cos(pitchAngle/180.0*M_PI)+magnY*sin(pitchAngle/180.0*M_PI)*sin(rollAngle/180.0*M_PI)+magnZ*sin(pitchAngle/180.0*M_PI)*cos(rollAngle/180.0*M_PI);
  //Y=magnY*cos(rollAngle/180.0*M_PI)-magnZ*sin(rollAngle/180.0*M_PI);
  //X=magnX*cos(rollAngle/180.0*M_PI)+magnY*sin(rollAngle/180.0*M_PI)*sin(pitchAngle/180.0*M_PI)+magnZ*sin(rollAngle/180.0*M_PI)*cos(pitchAngle/180.0*M_PI);
  //Y=magnY*cos(pitchAngle/180.0*M_PI)-magnZ*sin(pitchAngle/180.0*M_PI); 
 cosPitch=cos(pitchAngle);
 sinPitch=sin(pitchAngle);
 cosRoll=cos(rollAngle);
 sinRoll=sin(rollAngle);
 
 //X=magnX*cosPitch+magnY*sinPitch*sinRoll+magnZ*cosRoll*sinPitch;
 //Y=magnY*cosRoll-magnZ*sinRoll;
 
 X=magnX*cosPitch-magnZ*sinPitch;
 Y=magnX*sinRoll*sinPitch+magnY*cosRoll-magnZ*sinRoll*cosPitch;
 
 //oma hakki
 //Y=magnY-rollAngle/M_PI*180/100;
 //X=magnX-pitchAngle/M_PI*180/100;

 
 //X=magnX*cos(myPitch)+magnY*sin(myPitch)*sin(myRoll)+magnZ*cos(myRoll)*sin(myPitch); 
 //Y=magnY*cos(myRoll)-magnZ*sin(myRoll);
 
 //Y=magnY*cos(myRoll)+magnX*sin(myPitch)*sin(myRoll)+magnZ*cos(myPitch)*sin(myRoll); 
 //X=magnX*cos(myPitch)-magnZ*sin(myPitch);
 
 
 //Serial.println(Y/X);
  
  
  //}
  //else{
  //X=magnX;
  //Y=magnY;
  //}
  //uusin 20.9.2012
  //X=magnX*cos(myPitch/180.0*M_PI)+magnY*sin(myRoll/180.0*M_PI)*sin(myPitch/180.0*M_PI)+magnZ*cos(myRoll/180.0*M_PI)*sin(myPitch/180.0*M_PI);
  //Xh = bx * cos(theta) + by * sin(phi) * sin(theta) + bz * cos(phi) * sin(theta)
  
  
  //uusin 20.9.2012
  //Y=magnY*cos(myRoll/180.0*M_PI)-magnZ*sin(myRoll/180.0*M_PI);
  
  //Y=magnY-myPitch/180*M_PI*0.8;
  /*
  Serial.print(magnX);
  Serial.print(" ");
  Serial.print(X);
  Serial.print(" ");
  Serial.print(magnY); 
  Serial.print(" ");
  Serial.print(Y);  
  Serial.println(" ");
  */
  /*
  
  
  Serial.print(myPitch/180*M_PI);
  Serial.print(" ");
    Serial.print(myRoll/180*M_PI);
  Serial.println("");
  */
  
  
  //Yh = by * cos(phi) - bz * sin(phi)
  //}
  //else{
  //X=magnX;
  //Y=magnY;
  //}
  heading=atan2(-Y,X);
 
  
  /*
  if(X<0){
  heading=heading-M_PI;
  }
  if(X>0 && Y<0){
    heading*=(-1);
  
  }
  if(X>0 && Y>0){
    heading=2*M_PI-heading;
  }
  */
  
 
  
  /*
  if (heading > M_PI)

        heading -= (2*M_PI);

    if (heading < -M_PI)

        heading += (2*M_PI);

    if (heading < 0)
        heading += 2*M_PI;
    */  
 /*       
  if(heading<0){
    heading+=2*M_PI;
  }
  */
  /*
    Serial.print("Y:");
    Serial.print(Y);
    Serial.print(" X: ");
    Serial.println(X);
    */
  //Serial.println(cos(myRoll/180.0*M_PI));
  //Serial.println(magnX*magnX+magnZ*magnZ+magnY*magnY);
  heading=heading;
  //Serial.println(heading);
  //}

  //magnX=magnX*cos(myPitch)+magnY*sin(myRoll)*sin(myPitch)+magnZ*cos(myRoll)*sin(myPitch);
  //magnY=magnY*cos(myRoll)-magnZ*sin(myRoll);
  //   Xh = bx * cos(theta) + by * sin(phi) * sin(theta) + bz * cos(phi) * sin(theta)
  //  Yh = by * cos(phi) - bz * sin(phi)
 
  //Serial.println(gyroX);
  ///pitch on y-akselin ympari, pitch on myPitch
  //roll on x-akselin ymapari, roll on myRoll
  
  //joku oikea tapa: Tuossa on vakio kerroin, koska jos Gz on 0, niin tulee joku jakotulos
  
  myRoll = atan2 (accY,sqrt(accZ*accZ+1.0*accX*accX)); //pitas olla -180 ja 180 valilla
  myPitch =atan2 (accX,sqrt(accZ*accZ+accY*accY)); //pitchin pitas olla -90 ja 90 valilla
 
 //kilpaileva pitch
 //float g=sqrt(accY*accY+accX*accX+accZ*accZ);
 //myPitch=asin(accX/-g);
 //myPitch = atan2 (accX,accZ);
// myRoll = atan2 (accY,accZ);
 // yawXX = atan2 (Y,X)*180/M_PI;

  //Serial.println(yawXX);

  //float pitchAngle = atan2 (accY,accZ);
  //Serial.println(pitchAngle);
/*

*/
  //rollAngle=kalmanCalculateRoll(myRoll+2000,gyroX,loopTime);
  //pitchAngle=kalmanCalculatePitch(myPitch+2000,gyroY,loopTime);
  rollAngle=kalmanRoll.getAngle(myRoll,gyroX/180.0*M_PI,(double)(micros()-startT)/1000000);
  pitchAngle=kalmanPitch.getAngle(myPitch, gyroY/180.0*M_PI, (double)(micros()-startT)/1000000);
  yawAngle=kalmanYaw.getAngle(heading,gyroZ/180.0*M_PI,(double)(micros()-startT)/1000000);
  
  //Serial.println(gyroX);
  //yawAngle=kalmanCalculateYaw(heading+2000,gyroZ,loopTime);
  //yawAngle=kalmanCalculateYaw(yawXX+2000,gyroZ,loopTime);

  //rollS.looptime=micros()-startT;
  //kalmanCalculate(pitchAngle+2000,gyroX,rollS.looptime,rollS.Q_angle,rollS.Q_gyro,rollS.R_angle,rollS.pred_angle,rollS.P_00,rollS.P_01,rollS.P_10,rollS.P_11,rollS.dt,rollS.y,rollS.S,rollS.K_0,rollS.K_1,rollS.x_bias);  



  //rollAngle-=2000;
  
  /*
  if(rollAngle>180){
    rollAngle=-360.0+rollAngle;
    
  }
  if(rollAngle<-180){
    rollAngle=360+rollAngle;
  }

  pitchAngle-=2000;
  
  if(pitchAngle>180){
    pitchAngle=-360.0+pitchAngle;
    
  }
  if(pitchAngle<-180){
    pitchAngle=360+pitchAngle;
  }
  
    yawAngle-=2000;
  
  if(yawAngle>180){
    yawAngle=-360.0+yawAngle;
    
  }
  if(yawAngle<-180){
    yawAngle=360+yawAngle;
  }
  */

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
  Serial.print(Euleri[0]);
  Serial.print(" ");
  Serial.print(pitchAngle);
  //    Serial.print(" P2: ");
  //Serial.print(Euleri[1]);
  //Serial.print(" Yaw: ");
  Serial.print(" ");
  Serial.print(Euleri[1]);
Serial.print(" ");


  //Serial.print("Roll: ");
  Serial.print(rollAngle);
  //  Serial.print(" R2: ");
  //Serial.print(Euleri[0]);
  //Serial.print(" Pitch: ");
    Serial.print(" ");
    Serial.print(Euleri[2]);
Serial.print(" ");

Serial.print(" MesHeading: ");
Serial.print(heading);

Serial.print(" PitchDif: ");
Serial.print(pitchAngle-Euleri[1]);

Serial.print(" RollDif: ");
Serial.print(rollAngle-Euleri[2]);




  Serial.println("");

}

void printKalman(float &rollAngle){
  if(millis()>10000){  
  if(minRollKalman>rollAngle)
    minRollKalman=rollAngle;
  if(marollAngleKalman<rollAngle)
    marollAngleKalman=rollAngle;
  }
  Serial.print(rollAngle);
  Serial.print(" maxKalman: ");
  Serial.print(marollAngleKalman);
  Serial.print(" minKalman: ");
  Serial.print(minRollKalman);
  Serial.println(" ");
}

void printGyro(float &xGyro){
    if(minGyro>xGyro)
    minGyro=xGyro;
  if(maxGyro<xGyro)
    maxGyro=xGyro;
  Serial.print(xGyro);
  Serial.print(" maxGyro: ");
  Serial.print(maxGyro);
  Serial.print(" minGyro: ");
  Serial.print(minGyro);
  Serial.println(" ");
}

void printAcc(float &pitchAngle){
/*
  if(minRoll>pitchAngle)
    minRoll=pitchAngle;
  if(marollAngle<pitchAngle)
    marollAngle=;
  Serial.print(mpitchAngle);
  Serial.print(" max: ");
  Serial.print(marollAngle);
  Serial.print(" min: ");
  Serial.print(minRoll);
    Serial.println(" ");
*/
}

void roska(int roska){
   float servoPitchAngle=pitchAngle;//pitchAngle;//Euleri[1];
  
  if(startT>5000000 && startT/1000>(lastServoUpdate+servoUpdateInterval)){
    //Serial.println(lastServoUpdate);
    lastServoUpdate=startT/1000;
    //Serial.println("Updating Servo");
    if(abs(servoPitchAngle)>pitchTolerance){
      int tmpSpeed=abs(int(servoPitchAngle));
      if(tmpSpeed>90)
      tmpSpeed=90;
      if(servoPitchAngle>0){
        
        //servoLeftSpeed=90+30;
        //servoRightSpeed=90-30;
        
        servoLeftSpeed=100+tmpSpeed;
        servoRightSpeed=80-tmpSpeed;
        
        /*
        if(servoLeftSpeed<179)
        servoLeftSpeed+=1;
        if(servoRightSpeed>0)
        servoRightSpeed-=1;
        */
        
      }
      else{
        //servoLeftSpeed=90-30;
        //servoRightSpeed=90+30;
        
        servoLeftSpeed=80-tmpSpeed;
        servoRightSpeed=100+tmpSpeed;
        
        /*
        if(servoLeftSpeed>0)
        servoLeftSpeed-=1;
        if(servoRightSpeed<179)
        servoRightSpeed+=1;
        */
        
      }
      servoLeft.write(servoLeftSpeed);
      servoRight.write(servoRightSpeed);
      //Serial.println(servoLeftSpeed);
      //Serial.println(servoRightSpeed);
      
    }
    else{
      servoLeftSpeed=90;
      servoLeft.write(servoLeftSpeed);
      servoRightSpeed=90;      
      servoRight.write(servoRightSpeed);
    }
  } 
}


