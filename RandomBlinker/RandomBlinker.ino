//
//hubber bubber
typedef struct{
  int ledPin;
  long startTime;
  long offInterval;
  long raiseInterval;
  long onInterval;
  long lowerInterval;

  int offMin;
  int offMax;
  int raiseMin;
  int raiseMax;
  int onMin;
  int onMax;
  int lowerMin;
  int lowerMax;

  //state 0=start, 1=off, 2=raise, 3=on, 4 lower 
  int state;
  int running;
  int maxBrightness;
}
ledSeries;

void generateRandomTimes(ledSeries *ledSeriesX);
void changeState(ledSeries *ledSeriesX);

ledSeries ledSeries1;
ledSeries ledSeries2;
ledSeries ledSeries3;


void setup() {
  // set the digital pin as output:
ledSeries1.ledPin=9;
ledSeries1.startTime=0;
ledSeries1.offInterval=0;
ledSeries1.raiseInterval=0;
ledSeries1.onInterval=0;
ledSeries1.lowerInterval=0;

ledSeries1.offMin=500;
ledSeries1.offMax=1000;
ledSeries1.raiseMin=1000;
ledSeries1.raiseMax=4000;
ledSeries1.onMin=500;
ledSeries1.onMax=1000;
ledSeries1.lowerMin=1000;
ledSeries1.lowerMax=4000;

//state 0=start, 1=off, 2=raise, 3=on, 4 lower 
ledSeries1.state=0;
ledSeries1.running=0;
ledSeries1.maxBrightness=255;


//Series 2
ledSeries2.ledPin=10;
ledSeries2.startTime=0;
ledSeries2.offInterval=0;
ledSeries2.raiseInterval=0;
ledSeries2.onInterval=0;
ledSeries2.lowerInterval=0;

ledSeries2.offMin=500;
ledSeries2.offMax=1000;
ledSeries2.raiseMin=1000;
ledSeries2.raiseMax=4000;
ledSeries2.onMin=500;
ledSeries2.onMax=1000;
ledSeries2.lowerMin=1000;
ledSeries2.lowerMax=4000;

//state 0=start, 1=off, 2=raise, 3=on, 4 lower 
ledSeries2.state=0;
ledSeries2.running=0;
ledSeries2.maxBrightness=255;

//Series 3
ledSeries3.ledPin=11;
ledSeries3.startTime=0;
ledSeries3.offInterval=0;
ledSeries3.raiseInterval=0;
ledSeries3.onInterval=0;
ledSeries3.lowerInterval=0;

ledSeries3.offMin=500;
ledSeries3.offMax=1000;
ledSeries3.raiseMin=1000;
ledSeries3.raiseMax=4000;
ledSeries3.onMin=500;
ledSeries3.onMax=1000;
ledSeries3.lowerMin=1000;
ledSeries3.lowerMax=4000;

//state 0=start, 1=off, 2=raise, 3=on, 4 lower 
ledSeries3.state=0;
ledSeries3.running=0;
ledSeries3.maxBrightness=255;
  //
  pinMode(ledSeries1.ledPin, OUTPUT);
  pinMode(ledSeries2.ledPin, OUTPUT);
  pinMode(ledSeries3.ledPin, OUTPUT);

  Serial.begin(57600);
  Serial.println("Ready");  
}

void loop(){
  //alkutila
  if(ledSeries1.state==0){
    generateRandomTimes(&ledSeries1);
    ledSeries1.state=1;
  }

  //state OFF
  if(ledSeries1.state==1){
    //Serial.println("off");
    if(ledSeries1.running==0){
      ledSeries1.startTime=millis();
      ledSeries1.running=1;
    }
    if(millis()-ledSeries1.startTime<ledSeries1.offInterval){
      analogWrite(ledSeries1.ledPin,0);
    }
    else{
      changeState(&ledSeries1);
    }
  }
  
  //raising
  if(ledSeries1.state==2){
    //Serial.println("rising");
    if(ledSeries1.running==0){
      ledSeries1.startTime=millis();
      ledSeries1.running=1;
    }
    if(millis()-ledSeries1.startTime<ledSeries1.raiseInterval){
      long diff=ledSeries1.raiseInterval-(millis()-ledSeries1.startTime);
      long pinVal=(1-diff/ledSeries1.raiseInterval)*ledSeries1.maxBrightness;
      Serial.println(1-diff/ledSeries1.raiseInterval);

      analogWrite(ledSeries1.ledPin,pinVal);
    }
    else{
      changeState(&ledSeries1);
    }
  }
  
  //On
  if(ledSeries1.state==3){
    //Serial.println("on");
    if(ledSeries1.running==0){
      ledSeries1.startTime=millis();
      ledSeries1.running=1;
    }
    if(millis()-ledSeries1.startTime<ledSeries1.onInterval){
      //analogWrite(ledPin,255);
    }
    else{
      changeState(&ledSeries1);
    }
  }    
  //Lower
  if(ledSeries1.state==4){
    //Serial.println("Dimming");
    if(ledSeries1.running==0){
      ledSeries1.startTime=millis();
      ledSeries1.running=1;
    }
    if(millis()-ledSeries1.startTime<ledSeries1.lowerInterval){
      int diff=ledSeries1.lowerInterval-(millis()-ledSeries1.startTime);
      int pinVal=ledSeries1.maxBrightness-(diff/ledSeries1.raiseInterval*ledSeries1.maxBrightness);
      analogWrite(ledSeries1.ledPin,pinVal);

    }
    else{
      changeState(&ledSeries1);
    }
  }      

//Series2

    //alkutila
  if(ledSeries2.state==0){
    generateRandomTimes(&ledSeries2);
    ledSeries2.state=1;
  }

  //state OFF
  if(ledSeries2.state==1){
    //Serial.println("off");
    if(ledSeries2.running==0){
      ledSeries2.startTime=millis();
      ledSeries2.running=1;
    }
    if(millis()-ledSeries2.startTime<ledSeries2.offInterval){
      analogWrite(ledSeries2.ledPin,0);
    }
    else{
      changeState(&ledSeries2);
    }
  }
  
  //raising
  if(ledSeries2.state==2){
    //Serial.println("rising");
    if(ledSeries2.running==0){
      ledSeries2.startTime=millis();
      ledSeries2.running=1;
    }
    if(millis()-ledSeries2.startTime<ledSeries2.raiseInterval){
      long diff=ledSeries2.raiseInterval-(millis()-ledSeries2.startTime);
      long pinVal=(1-diff/ledSeries2.raiseInterval)*ledSeries2.maxBrightness;
      Serial.println(1-diff/ledSeries2.raiseInterval);

      analogWrite(ledSeries2.ledPin,pinVal);
    }
    else{
      changeState(&ledSeries2);
    }
  }
  
  //On
  if(ledSeries2.state==3){
    //Serial.println("on");
    if(ledSeries2.running==0){
      ledSeries2.startTime=millis();
      ledSeries2.running=1;
    }
    if(millis()-ledSeries2.startTime<ledSeries2.onInterval){
      //analogWrite(ledPin,255);
    }
    else{
      changeState(&ledSeries2);
    }
  }    
  //Lower
  if(ledSeries2.state==4){
    //Serial.println("Dimming");
    if(ledSeries2.running==0){
      ledSeries2.startTime=millis();
      ledSeries2.running=1;
    }
    if(millis()-ledSeries2.startTime<ledSeries2.lowerInterval){
      int diff=ledSeries2.lowerInterval-(millis()-ledSeries2.startTime);
      int pinVal=ledSeries2.maxBrightness-(diff/ledSeries2.raiseInterval*ledSeries2.maxBrightness);
      analogWrite(ledSeries2.ledPin,pinVal);

    }
    else{
      changeState(&ledSeries2);
    }
  } 
//Series 3
  //alkutila
  if(ledSeries3.state==0){
    generateRandomTimes(&ledSeries3);
    ledSeries3.state=1;
  }

  //state OFF
  if(ledSeries3.state==1){
    //Serial.println("off");
    if(ledSeries3.running==0){
      ledSeries3.startTime=millis();
      ledSeries3.running=1;
    }
    if(millis()-ledSeries3.startTime<ledSeries3.offInterval){
      analogWrite(ledSeries3.ledPin,0);
    }
    else{
      changeState(&ledSeries3);
    }
  }
  
  //raising
  if(ledSeries3.state==2){
    //Serial.println("rising");
    if(ledSeries3.running==0){
      ledSeries3.startTime=millis();
      ledSeries3.running=1;
    }
    if(millis()-ledSeries3.startTime<ledSeries3.raiseInterval){
      long diff=ledSeries3.raiseInterval-(millis()-ledSeries3.startTime);
      long pinVal=(1-diff/ledSeries3.raiseInterval)*ledSeries3.maxBrightness;
      Serial.println(1-diff/ledSeries3.raiseInterval);

      analogWrite(ledSeries3.ledPin,pinVal);
    }
    else{
      changeState(&ledSeries3);
    }
  }
  
  //On
  if(ledSeries3.state==3){
    //Serial.println("on");
    if(ledSeries3.running==0){
      ledSeries3.startTime=millis();
      ledSeries3.running=1;
    }
    if(millis()-ledSeries3.startTime<ledSeries3.onInterval){
      //analogWrite(ledPin,255);
    }
    else{
      changeState(&ledSeries3);
    }
  }    
  //Lower
  if(ledSeries3.state==4){
    //Serial.println("Dimming");
    if(ledSeries3.running==0){
      ledSeries3.startTime=millis();
      ledSeries3.running=1;
    }
    if(millis()-ledSeries3.startTime<ledSeries3.lowerInterval){
      int diff=ledSeries3.lowerInterval-(millis()-ledSeries3.startTime);
      int pinVal=ledSeries3.maxBrightness-(diff/ledSeries3.raiseInterval*ledSeries3.maxBrightness);
      analogWrite(ledSeries3.ledPin,pinVal);

    }
    else{
      changeState(&ledSeries3);
    }
  }      

}

void generateRandomTimes(ledSeries *ledSeriesX){
    ledSeriesX->offInterval=random(ledSeriesX->offMin,ledSeriesX->offMax);
    ledSeriesX->raiseInterval=random(ledSeriesX->raiseMin,ledSeriesX->raiseMax);
    ledSeriesX->onInterval=random(ledSeriesX->onMin,ledSeriesX->onMax);
    ledSeriesX->lowerInterval=random(ledSeriesX->lowerMin,ledSeriesX->lowerMax);
   
}

void changeState(ledSeries *ledSeriesX){
  if(ledSeriesX->state < 4){
    ledSeriesX->state=ledSeriesX->state+1;
  }
  else
    ledSeriesX->state=0;
    ledSeriesX->running=0;  
  
}

