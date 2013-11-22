#include <TM1638.h>
#include <NewPing.h>

#define TRIGGER_PIN  6
#define ECHO_PIN     7
#define MAX_DISTANCE 250 
#define Length 25


typedef struct {
  String message;
  int type;
  
} 
messageStruct;

messageStruct lcdMessages[Length];

int lcdMessagesIndex=0;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
int distance=0;
int showDistance=120;
// define a module on data pin 8, clock pin 9 and strobe pin 7
TM1638 module(3, 4, 5);

String sanoma = "";
boolean sanomaReady = false;
boolean sanomaOngoing = false;

unsigned long a=1200;
unsigned long b=600;
int c=200;

unsigned long int breachTime=0;
int lcdMessageDelay=1000;
boolean breachOngoing=false;
unsigned long int sonarTime=0;
int sonarDelay=150;
double ledStepSize= showDistance/8;
unsigned long ledCheckTime=0;
int ledCheckInterval=5;

void setup(){
  module.setDisplayToString("SHOOT   ");
  initLcdMessages();
  Serial.begin(115200);
  Serial.println("-----");
  Serial.println("Ready");

}

void loop()
{
  if((millis()-sonarTime)>sonarDelay){
  //delay(50);
  int uS = sonar.ping(); //sonar.ping();
  distance=uS / US_ROUNDTRIP_CM;
  sonarTime=millis();
  //module.setDisplayToDecNumber(distance,0,false);
  }
  if(breachOngoing||distance<showDistance){
    perimeterBreach();
    //Serial.println(distance);
    
    if(distance<showDistance){
  int leds=distance/ledStepSize;
  setXLeds(leds);
  //Serial.println(leds);
  //Serial.println(distance);
}
  }
  

  
  /*
  if((millis()-ledCheckTime)>ledCheckInterval){
    ledCheckTime=millis();
   if(distance<showDistance){
      clearAllLeds();
      double z=distance/ledStepSize;
      for(int i=0;i<=z;i++){
        module.setLED(TM1638_COLOR_GREEN,i);
   
      Serial.println(distance);

      }
  }*/
  if(distance>=showDistance){
    clearAllLeds();
  }

  

  if(module.getButtons() != 0b00000000){
    breachOngoing=false;
    initLcdMessages();
    module.clearDisplay();
    //sanoma="          ";
    //lcdMessage();
  }

  readSerialMessage();

}

void setXLeds(int leds){
  for(int i=0;i<leds;i++){
    module.setLED(TM1638_COLOR_GREEN,i);
  }
  for(int i=7;i>=leds;i--){
    module.setLED(TM1638_COLOR_NONE,i);
  }

}

void clearAllLeds(){
    for(int i=0;i<8;i++){
      module.setLED(TM1638_COLOR_NONE,i);
   
    }
}

void readSerialMessage(){
 if (Serial.available()) {
  char ch = Serial.read();
  Serial.println(ch);

  if (ch == '!') {
    Serial.println("Program reset");
    initLcdMessages();
    module.clearDisplay();
  }
  //viestin alku
  if (ch == '<') {
    sanomaOngoing = true;
  }
  //viestin loppu
  if (ch == '>') {
    sanomaOngoing = false;
    sanomaReady = true;
  }

  //lisataan merkit sanomaan    
  if (sanomaOngoing && ch != '<') {
    sanoma = sanoma + ch;
  }
  //sanoma valmis
  if (sanomaReady) {
    //nayttosanoma=sanoma;
    //setLcdMessage(nayttosanoma);
    String nayttosanoma=sanoma;
    int mtype=1;
    addLcdMessage(nayttosanoma,mtype);
    sanoma = "";
    sanomaReady = false;
  }
}
}
void setLcdMessage(String lsanoma){
  module.setDisplayToString(lsanoma);


}

void  addLcdMessage(String sanoma, int type){
  if(lcdMessagesIndex==(Length-1)){
    lcdMessagesIndex=0;
  }
  else
  {
    lcdMessagesIndex+=1;
  }
  lcdMessages[lcdMessagesIndex].message=sanoma;
  lcdMessages[lcdMessagesIndex].type=type;

}

void perimeterBreach(){
  if(breachOngoing){
    boolean isShown=false;
    int counter=0;
    for (int i=0;i<Length;i++){
      if(lcdMessages[i].message!=""){
        counter=counter+1;
        if(isShown==false && ((millis()-breachTime)<(counter)*lcdMessageDelay)){
          //module.clearDisplay();
          setLcdMessage(lcdMessages[i].message);
          isShown=true;
        }
      }
    }
    if(isShown==false){
      breachOngoing=false;
      module.clearDisplay();
    }
  }
  else{
    breachOngoing=true;
    breachTime=millis();
  }
}

void initLcdMessages(){
  for (int i=0;i<Length;i++){
    lcdMessages[i].message="";
  }

}



