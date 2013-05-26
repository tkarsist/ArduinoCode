/*
  SuperStepper.cpp - Library for driving stepper motor. 
  Tested with Sparkfun Big Easy Driver.
  Built-in acceleration and deceleration for the stepper
  Created by Timo Karsisto, July 31, 2012.
  Released into the public domain.
 */
#ifndef SuperStepper_h
#define SuperStepper_h

#include "Arduino.h"

class SuperStepper
{
public:
  //constructor with sleep mode enabled
  SuperStepper(byte DIR_PIN, byte STEP_PIN, byte STEPPER_SLEEP_PIN);
  //constructor without sleep mode
  SuperStepper(byte DIR_PIN, byte STEP_PIN);
  //stepper resolution in steps/round
  void setResolution(int stepperResolution);
  //if moving object, configure how many MM is 360 on the stepper
  void setOneRoundInMM(float oneRoundInMM);
  //tries to do graceful stop if interrupt received
  void setGracefulStop(boolean gracefulStop);
  //rotates given amount of steps, speed is in max RPM
  void rotateSteps(long int steps, float speed);
  //rotates given amount of degrees, speed is in max RPM
  void rotateDeg(float deg, float speed);
  //moves given MM, speed is in RPM
  void moveMM(float millimeters, float speed);
  //internal but can be used, it will set the motor to sleep
  void sleepMotor();
  //awakes the motor from sleep
  void awakeMotor();
  //Should not be touched. Default value is 8 (smaller value is quicker acceleration). Value around greater than 3 is ok
  void setAccelerationFactor(int accFactor);
  //Should not be touched. Default value is 10 (smaller value is quicker deceleration). Value greater than 3 is ok
  void setDecelerationFactor(int decFactor);
  //interrupt motor at anya time
  void interruptMotor();
  //calculates microseconds by max RPM
  float calculateUsDelay(float speed);
  //stops motor gracious (not hard stop)
  void graciousStop(float step_delay, int accel_cout, float rest);
  //runs motor continuous. Direction 1/-1 , speed in RPM
  void runContinuous(int direction, float speed);
  
private:
  byte _DIR_PIN;
  byte _STEP_PIN;
  byte _STEPPER_SLEEP_PIN;
  float _oneRoundInMM;
  int _stepperResolution;
  int _accFactor;
  int _decFactor;
  boolean _gracefulStop;
  volatile boolean _motorInterrupt;
};

#endif

