/*
  SuperStepper.cpp - Library for driving stepper motor.
 Created by Timo Karsisto, July 31, 2012.
 Released into the public domain.
 */

#include "Arduino.h"
#include "SuperStepper.h"

SuperStepper::SuperStepper(byte DIR_PIN, byte STEP_PIN, byte STEPPER_SLEEP_PIN){
  _DIR_PIN=DIR_PIN;
  _STEP_PIN=STEP_PIN;
  _STEPPER_SLEEP_PIN=STEPPER_SLEEP_PIN;
  _oneRoundInMM=float(1);
   _stepperResolution=800;
   _motorInterrupt=false;
   _gracefulStop=false;
 
  _accFactor=8;
  _decFactor=10;

  pinMode(_DIR_PIN, OUTPUT);
  pinMode(_STEP_PIN, OUTPUT);
  pinMode(_STEPPER_SLEEP_PIN, OUTPUT);
  digitalWrite(_STEPPER_SLEEP_PIN,LOW);
}

SuperStepper::SuperStepper(byte DIR_PIN, byte STEP_PIN){
  _DIR_PIN=DIR_PIN;
  _STEP_PIN=STEP_PIN;
  _STEPPER_SLEEP_PIN=0;
  _oneRoundInMM=float(1);
   _stepperResolution=800;
   _motorInterrupt=false;
   _gracefulStop=false;
 
  _accFactor=8;
  _decFactor=10;

  pinMode(_DIR_PIN, OUTPUT);
  pinMode(_STEP_PIN, OUTPUT);
  pinMode(_STEPPER_SLEEP_PIN, OUTPUT);
  digitalWrite(_STEPPER_SLEEP_PIN,LOW);
}


void SuperStepper::setResolution(int stepperResolution){
  _stepperResolution=stepperResolution; 
}

void SuperStepper::setOneRoundInMM(float oneRoundInMM){
  _oneRoundInMM=oneRoundInMM;
}



void SuperStepper::sleepMotor(){
	digitalWrite(_STEPPER_SLEEP_PIN, LOW);
  	delay(2);

}

void SuperStepper::awakeMotor(){
  digitalWrite(_STEPPER_SLEEP_PIN, HIGH);
  delay(2);	
}

void SuperStepper::setAccelerationFactor(int accFactor){
_accFactor=accFactor;
}

void SuperStepper::setDecelerationFactor(int decFactor){
_decFactor=decFactor;
}


void SuperStepper::rotateSteps(long int steps, float speed){
  //negative number for other direction

if(_STEPPER_SLEEP_PIN!=0){
	awakeMotor();}

  int dir = (steps > 0)? HIGH:LOW;
  steps = abs(steps);

  digitalWrite(_DIR_PIN,dir); 
  
  float usDelay=calculateUsDelay(speed);
  	

  //Acceleration and deceleration method variables
  float step_delay=1500;
  int accel_cout=1;
  float rest=0;
  boolean maxSpeedReached=false;
  _motorInterrupt=false;

  for(long int i=0; i < steps; i++){
	if(_motorInterrupt){
	i=steps;
	if(_gracefulStop)
		graciousStop(step_delay,accel_cout,rest);
	}
	if(!_motorInterrupt){
    digitalWrite(_STEP_PIN, HIGH);
    delayMicroseconds(step_delay); 

    digitalWrite(_STEP_PIN, LOW);
    delayMicroseconds(step_delay);
    if(!maxSpeedReached && accel_cout<(steps/2)){
      float step_delay_tmp=step_delay-(2*step_delay+rest)/(_accFactor*accel_cout+1);
      if(step_delay_tmp>usDelay){
        step_delay=step_delay_tmp;
        rest=int(int(2*step_delay+rest)%int(4*accel_cout+1));
        accel_cout+=1;

      }
      else{
        maxSpeedReached=true;
      }
    }
    if(accel_cout>=(steps-i)){ //litte bit different slowing functions         
      float step_delay_tmp=step_delay+(_decFactor*step_delay+rest)/(8*accel_cout+1);
      if(step_delay_tmp<1000){ //slowest speed
        step_delay=step_delay_tmp;
        rest=int(int(2*step_delay+rest)%int(4*accel_cout+1));
        accel_cout-=1;

      }

    }
  	}
  }

  
  if(_STEPPER_SLEEP_PIN!=0){
sleepMotor();
  }
  _motorInterrupt=false;

} 

void SuperStepper::interruptMotor(){
_motorInterrupt=true;
}

void SuperStepper::rotateDeg(float deg, float speed){

  long int steps = abs(deg)*float(_stepperResolution/360.0);
	if(deg<0){
		steps=steps*(-1);
	}  
  rotateSteps(steps,speed);

}

void SuperStepper::moveMM(float millimeters, float speed){
  rotateDeg(float(millimeters/(_oneRoundInMM)*360),speed);
}

void SuperStepper::runContinuous(int direction, float speed){

	long int steps=direction*2000000000;
	rotateSteps(steps,speed);
	
}

void SuperStepper::graciousStop(float step_delay, int accel_cout, float rest){
	
	boolean running=true;
	while(running){
	
	  float step_delay_tmp=step_delay+(_decFactor*step_delay+rest)/(8*accel_cout+1);
      if(step_delay_tmp<1000){ //slowest speed
        step_delay=step_delay_tmp;
        rest=int(int(2*step_delay+rest)%int(4*accel_cout+1));
        accel_cout-=1;

      }
      else
      running=false;
      
      digitalWrite(_STEP_PIN, HIGH);
      delayMicroseconds(step_delay); 
      digitalWrite(_STEP_PIN, LOW);
      delayMicroseconds(step_delay);
	}

}

float SuperStepper::calculateUsDelay(float speed){
	return float((60/(speed*_stepperResolution)-0.00003)*500000);
	
}
void SuperStepper::setGracefulStop(boolean gracefulStop){
	_gracefulStop=gracefulStop;
}

