/* Blink without Delay
 
 Turns on and off a light emitting diode(LED) connected to a digital  
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.
 
 The circuit:
 * LED attached from pin 13 to ground.
 * Note: on most Arduinos, there is already an LED on the board
 that's attached to pin 13, so no hardware is needed for this example.
 
 
 created 2005
 by David A. Mellis
 modified 8 Feb 2010
 by Paul Stoffregen
 
 This example code is in the public domain.

 
 http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */

// constants won't change. Used here to 
// set pin numbers:
const int ledPin =  9;      // the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long startTime=0;

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 100;           // interval at which to blink (milliseconds)
int i=0;

long offInterval=0;
long raiseInterval=0;
long onInterval=0;
long lowerInterval=0;

int offMin=500;
int offMax=1000;
int raiseMin=1000;
int raiseMax=4000;
int onMin=500;
int onMax=1000;
int lowerMin=1000;
int lowerMax=4000;


//state 0=start, 1=off, 2=raise, 3=on, 4 lower 

int state=0;
int running=0;
int maxBrightness=255;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  Serial.begin(57600);
  Serial.println("Ready");  
}

void loop()
{
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to 
  // blink the LED.
  
  if(state==0){
    generateRandomTimes();
    state=1;
  }

  //state OFF
  if(state==1){
    Serial.println("off");
    if(running==0){
      startTime=millis();
      running=1;
    }
    if(millis()-startTime<offInterval){
      analogWrite(ledPin,0);
    }
    else{
      changeState();
    }
  }
  
  //raising
  if(state==2){
    Serial.println("rising");
    if(running==0){
      startTime=millis();
      running=1;
    }
    if(millis()-startTime<raiseInterval){
      long diff=raiseInterval-(millis()-startTime);
      long pinVal=(1-diff/raiseInterval)*maxBrightness;
      Serial.println(1-diff/raiseInterval);

      analogWrite(ledPin,pinVal);
    }
    else{
      changeState();
    }
  }
  
  //On
  if(state==3){
    Serial.println("on");
    if(running==0){
      startTime=millis();
      running=1;
    }
    if(millis()-startTime<onInterval){
      //analogWrite(ledPin,255);
    }
    else{
      changeState();
    }
  }    
  //Lower
  if(state==4){
    Serial.println("Dimming");
    if(running==0){
      startTime=millis();
      running=1;
    }
    if(millis()-startTime<lowerInterval){
      int diff=lowerInterval-(millis()-startTime);
      int pinVal=maxBrightness-(diff/raiseInterval*maxBrightness);
      analogWrite(ledPin,pinVal);

    }
    else{
      changeState();
    }
  }      

}

void generateRandomTimes(){
    offInterval=random(offMin,offMax);
    raiseInterval=random(raiseMin,raiseMax);
    onInterval=random(onMin,onMax);
    lowerInterval=random(lowerMin,lowerMax);
}

void changeState(){
  if(state < 4){
    state=state+1;
  }
  else
    state=0;
  running=0;  
}

