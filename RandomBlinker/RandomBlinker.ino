// Moodi-ideat:
//Moodi 1: sopivan keltainen valo 
//1. ekalla potikalla saataa vain kirkkautta. 
//2. potikalla saadetaan paljonko liekki elaa. 
//3. potikalla saadetaan varilampotilaa
//Moodi 2: Jokainen potikka saataa varin kirkkautta
//Moodi 3: Random RGB-vilkunta. 
//1. potikka saataa maksimikirkkautta
//2. potikka voisi saataa random ajan pituutta
//3. potikka voisi saataa kuinka paljon valot himmenee
//Moodi 4:
//Taysin randomimoodi: arvottu aika kuinka usein muuttuu ohjelma, ja sitten aina random ohjelma, ehka myos random alin kirkkaus ja maksimikirkkaus


typedef struct{
  int ledPin;
  int potPin;
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
  double randomFactor;

  //state 0=start, 1=off, 2=raise, 3=on, 4 lower 
  int state;
  int running;
  int maxBrightness;
}
ledSeries;

void generateRandomTimes(ledSeries *ledSeriesX);
void changeState(ledSeries *ledSeriesX);
void executeLedJob(ledSeries *ledSeriesX);
void setRandomFactors(ledSeries *ledSeriesX);

ledSeries ledSeries1;
ledSeries ledSeries2;
ledSeries ledSeries3;

//mode 0=normal light with fading,1=random blinking
int programMode=1;
//testipinni potentiometrille
int potPin=0;
int potCheckInterval=100;
long potCheckTime=0;


void setup() {
  // set the digital pin as output:
ledSeries1.ledPin=9;
ledSeries1.potPin=0;
ledSeries1.startTime=0;
ledSeries1.offInterval=0;
ledSeries1.raiseInterval=0;
ledSeries1.onInterval=0;
ledSeries1.lowerInterval=0;

ledSeries1.offMin=50;
ledSeries1.offMax=1000;
ledSeries1.raiseMin=50;
ledSeries1.raiseMax=4000;
ledSeries1.onMin=50;
ledSeries1.onMax=1000;
ledSeries1.lowerMin=50;
ledSeries1.lowerMax=4000;
ledSeries1.randomFactor=1.0;

//state 0=start, 1=off, 2=raise, 3=on, 4 lower 
ledSeries1.state=0;
ledSeries1.running=0;
ledSeries1.maxBrightness=255;


//Series 2
ledSeries2.ledPin=10;
ledSeries2.potPin=0;
ledSeries2.startTime=0;
ledSeries2.offInterval=0;
ledSeries2.raiseInterval=0;
ledSeries2.onInterval=0;
ledSeries2.lowerInterval=0;

ledSeries2.offMin=50;
ledSeries2.offMax=1000;
ledSeries2.raiseMin=50;
ledSeries2.raiseMax=4000;
ledSeries2.onMin=50;
ledSeries2.onMax=1000;
ledSeries2.lowerMin=50;
ledSeries2.lowerMax=4000;
ledSeries2.randomFactor=1.0;

//state 0=start, 1=off, 2=raise, 3=on, 4 lower 
ledSeries2.state=0;
ledSeries2.running=0;
ledSeries2.maxBrightness=255;

//Series 3
ledSeries3.ledPin=11;
ledSeries3.potPin=0;
ledSeries3.startTime=0;
ledSeries3.offInterval=0;
ledSeries3.raiseInterval=0;
ledSeries3.onInterval=0;
ledSeries3.lowerInterval=0;

ledSeries3.offMin=50;
ledSeries3.offMax=1000;
ledSeries3.raiseMin=50;
ledSeries3.raiseMax=4000;
ledSeries3.onMin=50;
ledSeries3.onMax=1000;
ledSeries3.lowerMin=50;
ledSeries3.lowerMax=4000;
ledSeries3.randomFactor=1.0;

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
  
  //normal fading in use
  if(programMode==0){
    if(millis()-potCheckTime>potCheckInterval){
      potCheckTime=millis();
      int val = analogRead(potPin);            // reads the value of the potentiometer (value between 0 and 1023) 
      Serial.println(val);
      val = map(val, 0, 1023, 0, 255);
      delay(20);
      analogWrite(ledSeries1.ledPin,val);
      analogWrite(ledSeries2.ledPin,val);
      analogWrite(ledSeries3.ledPin,val);
      Serial.print("Potentioarvo:");
      Serial.println(val);
    }

  }

  //random mode in use  
  if(programMode==1){
    if(millis()-potCheckTime>potCheckInterval){
      potCheckTime=millis();
      setRandomFactors(&ledSeries1);
      setRandomFactors(&ledSeries2);
      setRandomFactors(&ledSeries3);
    }  
    executeLedJob(&ledSeries1);
    executeLedJob(&ledSeries2);
    executeLedJob(&ledSeries3);
  }

}

void generateRandomTimes(ledSeries *ledSeriesX){
    ledSeriesX->offInterval=random(ledSeriesX->randomFactor*ledSeriesX->offMin,ledSeriesX->randomFactor*ledSeriesX->offMax);
    ledSeriesX->raiseInterval=random(ledSeriesX->randomFactor*ledSeriesX->raiseMin,ledSeriesX->randomFactor*ledSeriesX->raiseMax);
    ledSeriesX->onInterval=random(ledSeriesX->randomFactor*ledSeriesX->onMin,ledSeriesX->randomFactor*ledSeriesX->onMax);
    ledSeriesX->lowerInterval=random(ledSeriesX->randomFactor*ledSeriesX->lowerMin,ledSeriesX->randomFactor*ledSeriesX->lowerMax);
    Serial.println("randomit generoitu");
   
}

void changeState(ledSeries *ledSeriesX){
  if(ledSeriesX->state < 4){
    ledSeriesX->state=ledSeriesX->state+1;
  }
  else
    ledSeriesX->state=0;
    ledSeriesX->running=0;  
  
}

void setRandomFactors(ledSeries *ledSeriesX){
    int val = analogRead(ledSeriesX->potPin);            // reads the value of the potentiometer (value between 0 and 1023) 
    val = map(val, 0, 1023, 0, 500);
    double tmpRandFactor=1.0*val/100.0;
    if(abs(ledSeriesX->randomFactor-tmpRandFactor)>0.1){
      ledSeriesX->randomFactor=1.0*tmpRandFactor;
      if(ledSeriesX->randomFactor<0.05)
        ledSeriesX->randomFactor=0.05;
      //ledSeriesX->randomFactor=1.0;
      Serial.println("FActorit randomit generoitu");

    }
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
    if(millis()-ledSeriesX->startTime<(ledSeriesX->offInterval)){
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
    if(millis()-ledSeriesX->startTime<(ledSeriesX->raiseInterval)){
      long diff=ledSeriesX->raiseInterval-(millis()-ledSeriesX->startTime);
      long pinVal=(1.0-1.0*diff/ledSeriesX->raiseInterval)*ledSeriesX->maxBrightness;

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
    if(millis()-ledSeriesX->startTime<(ledSeriesX->onInterval)){
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
    if(millis()-ledSeriesX->startTime<(ledSeriesX->lowerInterval)){
      long diff=ledSeriesX->lowerInterval-(millis()-ledSeriesX->startTime);
      long pinVal=(1.0*diff/ledSeriesX->lowerInterval*ledSeriesX->maxBrightness);
      //Serial.println(pinVal);

      analogWrite(ledSeriesX->ledPin,pinVal);

    }
    else{
      changeState(ledSeriesX);
    }
  }   


}

