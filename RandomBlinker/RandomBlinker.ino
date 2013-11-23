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
void executeLedJob(ledSeries *ledSeriesX);

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
  
  executeLedJob(&ledSeries1);
  executeLedJob(&ledSeries2);
  executeLedJob(&ledSeries3);


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

void executeLedJob(ledSeries *ledSeriesX){

  if(ledSeriesX->state==0){
    generateRandomTimes(ledSeriesX);
    ledSeriesX->state=1;
  }

  //state OFF
  if(ledSeriesX->state==1){
    //Serial.println("off");
    if(ledSeriesX->running==0){
      ledSeriesX->startTime=millis();
      ledSeriesX->running=1;
    }
    if(millis()-ledSeriesX->startTime<ledSeriesX->offInterval){
      analogWrite(ledSeriesX->ledPin,0);
    }
    else{
      changeState(ledSeriesX);
    }
  }
  
  //raising
  if(ledSeriesX->state==2){
    //Serial.println("rising");
    if(ledSeriesX->running==0){
      ledSeriesX->startTime=millis();
      ledSeriesX->running=1;
    }
    if(millis()-ledSeriesX->startTime<ledSeriesX->raiseInterval){
      long diff=ledSeriesX->raiseInterval-(millis()-ledSeriesX->startTime);
      long pinVal=(1-diff/ledSeriesX->raiseInterval)*ledSeriesX->maxBrightness;
      Serial.println(1-diff/ledSeriesX->raiseInterval);

      analogWrite(ledSeriesX->ledPin,pinVal);
    }
    else{
      changeState(ledSeriesX);
    }
  }
  
  //On
  if(ledSeriesX->state==3){
    //Serial.println("on");
    if(ledSeriesX->running==0){
      ledSeriesX->startTime=millis();
      ledSeriesX->running=1;
    }
    if(millis()-ledSeriesX->startTime<ledSeriesX->onInterval){
      //analogWrite(ledPin,255);
    }
    else{
      changeState(ledSeriesX);
    }
  }    
  //Lower
  if(ledSeriesX->state==4){
    //Serial.println("Dimming");
    if(ledSeriesX->running==0){
      ledSeriesX->startTime=millis();
      ledSeriesX->running=1;
    }
    if(millis()-ledSeriesX->startTime<ledSeriesX->lowerInterval){
      int diff=ledSeriesX->lowerInterval-(millis()-ledSeriesX->startTime);
      int pinVal=ledSeriesX->maxBrightness-(diff/ledSeriesX->raiseInterval*ledSeriesX->maxBrightness);
      analogWrite(ledSeriesX->ledPin,pinVal);

    }
    else{
      changeState(ledSeriesX);
    }
  }   


}

