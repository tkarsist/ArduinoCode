#include <TM1638.h>
#include <NewPing.h>

#define TRIGGER_PIN  6
#define ECHO_PIN     7
#define MAX_DISTANCE 500 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
int distance;
int showDistance=70;
// define a module on data pin 8, clock pin 9 and strobe pin 7
TM1638 module(3, 4, 5);

String sanoma = "";
String nayttosanoma="";
boolean sanomaReady = false;
boolean sanomaOngoing = false;

unsigned long a=1200;
unsigned long b=600;
int c=200;

void setup(){
module.setDisplayToString("SHOOT   ");
Serial.begin(115200);
Serial.println("-----");
Serial.println("Ready");

}

void loop()
{
  delay(50);
  int uS = sonar.ping();
  distance=uS / US_ROUNDTRIP_CM;
  //Serial.println(distance);
  if (distance<showDistance){
      //Serial.println("nyt sanomaa");
      //Serial.println(nayttosanoma);
      lcdMessage();
  }
  else{
    module.clearDisplay();
  }
  if(module.getButtons() != 0b00000000){
    nayttosanoma="";
    module.clearDisplay();
    //sanoma="          ";
    //lcdMessage();
  }

  readSerialMessage();

}

void readSerialMessage(){
 if (Serial.available()) {
  char ch = Serial.read();
  Serial.println(ch);

  if (ch == '!') {
    Serial.println("Program reset");
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
    lcdMessage();
    nayttosanoma=sanoma;
    sanoma = "";
    sanomaReady = false;
  }
 }
}
void lcdMessage(){
  module.setDisplayToString(nayttosanoma);


}
