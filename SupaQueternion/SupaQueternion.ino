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

float accZ,accY,accX,gyroX,gyroY,gyroZ,magnX,magnY,magnZ,yawAngle,rollAngle,pitchAngle,myRoll,myPitch,yawXX,heading, magnNorm;

unsigned long int printTimer=0;
int printInterval=20;
unsigned long int loopCounter=0;
float averageLoopTime=0.0;
float Y,X;

boolean printRawVal=true;

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
  
  
  /*
  for (int i=0;i<90;i++){
    servoLeft.write(90+i);
    servoRight.write(90-i);
    Serial.print("Speed: ");
    Serial.println(i);
    delay(1000);
  }
  */
  
  
  /*
  //magnetometrin korjaus
  if(abs(rollAngle)>20){
  magnY=magnY*cos(myPitch)+magnZ*sin(myPitch);
  }
  if(abs(myPitch>20)){
  magnX=magnY*sin(myRoll)*sin(myPitch)+magnX*cos(myRoll)-magnZ*sin(myRoll)*cos(myPitch);
  }
  */

  
  //tarkee
  //if(abs(myPitch>20)&&abs(myRoll)>20){
  //float X=magnX/57.2*cos(myPitch/57.2)+magnY/57.2*sin(myRoll/57.2)*sin(myPitch/57.2)+magnZ/57.2*cos(myRoll/57.2)*sin(myPitch/57.2);
  //float Y=magnY/57.2*cos(myRoll/57.2)-magnZ/57.2*sin(myRoll/57.2);
  
  //poista

  if(abs(myRoll)>0.1 && abs(myPitch)>0.1){

  //Y=magnY*cos(myPitch/180.0*M_PI)+magnZ*sin(myPitch/180.0*M_PI);
  //X=magnX*cos(myRoll/180.0*M_PI)+magnY*sin(myRoll/180.0*M_PI)*sin(myPitch/180.0*M_PI)-magnZ*cos(myPitch/180.0*M_PI)*sin(myRoll/180.0*M_PI);
  //X=magnX-abs(myPitch)/100*15;
  
  //X=magnX-myRoll/180*M_PI*0.8;
  
  //X=magnX*cos(pitchAngle/180.0*M_PI)+magnY*sin(pitchAngle/180.0*M_PI)*sin(rollAngle/180.0*M_PI)+magnZ*sin(pitchAngle/180.0*M_PI)*cos(rollAngle/180.0*M_PI);
  //Y=magnY*cos(rollAngle/180.0*M_PI)-magnZ*sin(rollAngle/180.0*M_PI);
  //X=magnX*cos(rollAngle/180.0*M_PI)+magnY*sin(rollAngle/180.0*M_PI)*sin(pitchAngle/180.0*M_PI)+magnZ*sin(rollAngle/180.0*M_PI)*cos(pitchAngle/180.0*M_PI);
  //Y=magnY*cos(pitchAngle/180.0*M_PI)-magnZ*sin(pitchAngle/180.0*M_PI); 
 
 X=magnX*cos(myPitch/180.0*M_PI)+magnY*sin(myPitch/180.0*M_PI)*sin(myRoll/180.0*M_PI)-magnZ*cos(myRoll/180.0*M_PI)*sin(myPitch/180.0*M_PI); 
 Y=magnY*cos(myRoll/180.0*M_PI)-magnZ*sin(myRoll/180.0*M_PI);
 
 //Serial.println(Y/X);
  
  
  }
  else{
  X=magnX;
  Y=magnY;
  }
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
  heading=heading*180.0/M_PI;
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
  myRoll = atan2 (accY,sqrt(accZ*accZ+1.0*accX*accX))*180/M_PI; //pitas olla -180 ja 180 valilla
  myPitch =atan2 (accX,sqrt(accZ*accZ+accY*accY))*180/M_PI; //pitchin pitas olla -90 ja 90 valilla
 
 //kilpaileva pitch
 //float g=sqrt(accY*accY+accX*accX+accZ*accZ);
 //myPitch=asin(accX/-g);
 
 // myPitch = atan2 (accX,accZ)*180/M_PI;
 // myRoll = atan2 (accY,accZ)*180/M_PI;
 // yawXX = atan2 (Y,X)*180/M_PI;

  //Serial.println(yawXX);

  //float pitchAngle = atan2 (accY,accZ);
  //Serial.println(pitchAngle);
/*

*/
  //rollAngle=kalmanCalculateRoll(myRoll+2000,gyroX,loopTime);
  //pitchAngle=kalmanCalculatePitch(myPitch+2000,gyroY,loopTime);
  rollAngle=kalmanRoll.getAngle(myRoll+2000,gyroX/131.0,(double)(micros()-startT)/1000000);
  pitchAngle=kalmanPitch.getAngle(myPitch+2000, gyroY/131.0, (double)(micros()-startT)/1000000);
  yawAngle=kalmanYaw.getAngle(heading+2000,gyroZ/131.0,(double)(micros()-startT)/1000000);
  //yawAngle=kalmanCalculateYaw(heading+2000,gyroZ,loopTime);
  //yawAngle=kalmanCalculateYaw(yawXX+2000,gyroZ,loopTime);

  //rollS.looptime=micros()-startT;
  //kalmanCalculate(pitchAngle+2000,gyroX,rollS.looptime,rollS.Q_angle,rollS.Q_gyro,rollS.R_angle,rollS.pred_angle,rollS.P_00,rollS.P_01,rollS.P_10,rollS.P_11,rollS.dt,rollS.y,rollS.S,rollS.K_0,rollS.K_1,rollS.x_bias);  



  rollAngle-=2000;
  
  if(rollAngle>180){
    rollAngle=-360.0+rollAngle;
    
  }
  if(rollAngle<-180){
    rollAngle=360+rollAngle;
  }
  
  //printGyro(gyroX);
  //printKalman(rollAngle);

  pitchAngle-=2000;
  
  if(pitchAngle>180){
    pitchAngle=-360.0+pitchAngle;
    
  }
  if(pitchAngle<-180){
    pitchAngle=360+pitchAngle;
  }
  
  //printGyro(gyroX);
  //printKalman(pitchAngle);  
  
    yawAngle-=2000;
  
  if(yawAngle>180){
    yawAngle=-360.0+yawAngle;
    
  }
  if(yawAngle<-180){
    yawAngle=360+yawAngle;
  }
/*  
  //printGyro(gyroX);
  //printKalman(yawAngle);
  float qX[4];
  qX[0]=rollAngle/180*M_PI;
  qX[1]=pitchAngle/180*M_PI;
  //qX[2]=yawAngle/180*M_PI;
  qX[2]=1.0;
  qX[3]=0.0;
  serialPrintFloatArr(qX, 4);
  Serial.println("");
  //delay(20);  
  */
  loopTime=micros()-startT;

if((millis()-printTimer)>printInterval){
  
  printTimer=millis();
  
  if(printRawVal){
    printRaw();
  }
  else{
  printNormal();
  }


}
  loopCounter+=1;
  if(averageLoopTime>0){
    averageLoopTime=averageLoopTime*(loopCounter-1)/loopCounter*1.0+loopTime/loopCounter*1.0;
  }
  else
    averageLoopTime=loopTime;
}

void printRaw(){
  float qX[4];
  qX[0]=rollAngle/180*M_PI;
  qX[1]=pitchAngle/180*M_PI;
  qX[2]=yawAngle/180*M_PI;
  //qX[2]=0.0;
  qX[3]=0.0;
  serialPrintFloatArr(qX, 4);
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
  //    Serial.print(" Y2: ");
  //Serial.print(Euleri[2]);
  //Serial.print(" Looptime: ");  
  //Serial.println(averageLoopTime/1000);
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




 //roll
 float Q_angle  =  0.001;
 float Q_gyro   =  0.003;
 float R_angle  =  0.03; 

 float x_angle = 0;
 float x_bias = 0;
 float P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;      
 float dt, y, S;
 float K_0, K_1;

 //pitch
 float Q_angle1  =  0.001;
 float Q_gyro1   =  0.003;
 float R_angle1  =  0.03; 

 float x_angle1 = 0;
 float x_bias1 = 0;
 float P_001 = 0, P_011 = 0, P_101 = 0, P_111 = 0;      
 float dt1, y1, S1;
 float K_01, K_11;
 
 //yaw
 float Q_angle2  =  0.001;
 float Q_gyro2   =  0.003;
 float R_angle2  =  0.03; 

 float x_angle2 = 0;
 float x_bias2 = 0;
 float P_002 = 0, P_012 = 0, P_102 = 0, P_112 = 0;      
 float dt2, y2, S2;
 float K_02, K_12;
   

  float kalmanCalculateRoll(float accAngle, float gyroRate,int looptime) {
    dt = float(looptime)/1000000;  
    x_angle += dt * (gyroRate - x_bias);
    P_00 +=  - dt * (P_10 + P_01) + Q_angle * dt;
    P_01 +=  - dt * P_11;
    P_10 +=  - dt * P_11;
    P_11 +=  + Q_gyro * dt;
    
    y = accAngle - x_angle;
    S = P_00 + R_angle;
    K_0 = P_00 / S;
    K_1 = P_10 / S;
    
    x_angle +=  K_0 * y;
    x_bias  +=  K_1 * y;
    P_00 -= K_0 * P_00;
    P_01 -= K_0 * P_01;
    P_10 -= K_1 * P_00;
    P_11 -= K_1 * P_01;

    
    return x_angle;
  }
  
    float kalmanCalculatePitch(float accAngle, float gyroRate,int looptime) {
    dt1 = float(looptime)/1000000;  
    x_angle1 += dt1 * (gyroRate - x_bias1);
    P_001 +=  - dt1 * (P_101 + P_011) + Q_angle1 * dt1;
    P_011 +=  - dt1 * P_111;
    P_101 +=  - dt1 * P_111;
    P_111 +=  + Q_gyro1 * dt1;
    
    y1 = accAngle - x_angle1;
    S1 = P_001 + R_angle1;
    K_01 = P_001 / S1;
    K_11 = P_101 / S1;
    
    x_angle1 +=  K_01 * y1;
    x_bias1  +=  K_11 * y1;
    P_001 -= K_01 * P_001;
    P_011 -= K_01 * P_011;
    P_101 -= K_11 * P_001;
    P_111 -= K_11 * P_011;

    
    return x_angle1;
  }
  
      float kalmanCalculateYaw(float accAngle, float gyroRate,int looptime) {
    dt2 = float(looptime)/1000000;  
    x_angle2 += 0.5*dt2 * (gyroRate - x_bias2);
    P_002 +=  - dt2 * (P_102 + P_012) + Q_angle2 * dt2;
    P_012 +=  - dt2 * P_112;
    P_102 +=  - dt2 * P_112;
    P_112 +=  + Q_gyro2 * dt2;
    
    y2 = accAngle - x_angle2;
    S2 = P_002 + R_angle2;
    K_02 = P_002 / S2;
    K_12 = P_102 / S2;
    
    x_angle2 +=  K_02 * y2;
    x_bias2  +=  K_12 * y2;
    P_002 -= K_02 * P_002;
    P_012 -= K_02 * P_012;
    P_102 -= K_12 * P_002;
    P_112 -= K_12 * P_012;

    
    return x_angle2;
  }

    
