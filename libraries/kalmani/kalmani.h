#ifndef Kalmani_h
#define Kalmani_h


typedef struct { 
 float Q_angle;
 float Q_gyro;
 float R_angle; 
 float pred_angle;
 float pred_bias;
 float P_00; 
 float P_01;
 float P_10;
 float P_11;      
 float dt;
 float y;
 float S;
 float K_0;
 float K_1;
 float accAngle;
 float gyroRate;
 float loopTime;
 }
 kalmanStruct;
 
  kalmanStruct rollFilter={0.001,0.003,0.03,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 kalmanStruct pitchFilter={0.001,0.003,0.03,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  void kalmanCalculate2(kalmanStruct * kalmani) {
    kalmani.dt = float(kalmani.loopTime)/1000000;
    //dt=looptime;    
    kalmani.pred_angle += kalmani.dt * (kalmani.gyroRate - kalmani.pred_bias);
    //Serial.print("Gyro moved");
    //Serial.println(dt * (gyroRate - x_bias));
    kalmani.P_00 +=  - kalmani.dt * (kalmani.P_10 + kalmani.P_01) + kalmani.Q_angle * kalmani.dt;
    kalmani.P_01 +=  - kalmani.dt * kalmani.P_11;
    kalmani.P_10 +=  - kalmani.dt * kalmani.P_11;
    kalmani.P_11 +=  + kalmani.Q_gyro * kalmani.dt;
    
    kalmani.y = kalmani.accAngle - kalmani.pred_angle;
    kalmani.S = kalmani.P_00 + kalmani.R_angle;
    kalmani.K_0 = kalmani.P_00 / kalmani.S;
    kalmani.K_1 = kalmani.P_10 / kalmani.S;
    
    kalmani.pred_angle +=  kalmani.K_0 * kalmani.y;
    kalmani.pred_bias  +=  kalmani.K_1 * kalmani.y;
    kalmani.P_00 -= kalmani.K_0 * kalmani.P_00;
    kalmani.P_01 -= kalmani.K_0 * kalmani.P_01;
    kalmani.P_10 -= kalmani.K_1 * kalmani.P_00;
    kalmani.P_11 -= kalmani.K_1 * kalmani.P_01;
    
  }
#endif  	
