#include <TM1638.h>

// define a module on data pin 8, clock pin 9 and strobe pin 7
TM1638 module(3, 4, 5);

String sanoma = "";
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
  readSerialMessage();
/*  
  for (unsigned long i=0;i<99999999;i++){
    module.setDisplayToDecNumber(i,0,false);
    delayMicroseconds(50);
  }
*/

/*
for (int i=0;i<2;i++){
  module.setDisplayToString("PESTUU  ");
  //module.setLED(TM1638_COLOR_GREEN, 0);
  //module.setLED(TM1638_COLOR_RED, 1);
  //module.setLED(TM1638_COLOR_GREEN + TM1638_COLOR_RED, 2);
  //module.setLED(TM1638_COLOR_RED, 3);


  delay(c); 
  module.setDisplayToString("PESTUU  ");
  delay(c);
  module.setDisplayToString(" PESTUU ");
  delay(c);
  module.setDisplayToString("  PESTUU");
  delay(c);
  module.setDisplayToString("U  PESTU");
  delay(c);
  module.setDisplayToString("UU  PEST");
  delay(c);
  module.setDisplayToString("TUU  PES");
  delay(c);
  module.setDisplayToString("STUU  PE");
  delay(c);
  module.setDisplayToString("ESTUU  P");
  delay(c);
  module.setDisplayToString("PESTUU  ");
  delay(c);
  
}
delay (1000);
int viipe=1500;

for (int i=0;i<80;i++){

    module.setDisplayToString("    2   ");
    delay(viipe);
    module.setDisplayToString("       2");
    delay(viipe);
    module.setDisplayToString("     0  ");
    delay(viipe);
    module.setDisplayToString("      1 ");
    delay(viipe);
    if (viipe>50)
    viipe=viipe/2;
    
}
module.clearDisplay();
delay(2000);
module.setDisplayToString("KOODAUS ");
delay(2000);
module.setDisplayToString(" ALKAA  ");
delay(1000);
module.setDisplayToString(" A KAA  ");
delay(500);
module.setDisplayToString(" A K A  ");
delay(500);
module.setDisplayToString("   K A  ");
delay(500);
module.setDisplayToString("   K    ");
delay(500);
module.setDisplayToString("        ");
delay(1500);


  for (unsigned long i=1;i<10001;i++){
    if(i%1000==0){
    module.setDisplayToString("SOODAA  ");
    delay(500);
    }
    else
    module.setDisplayToDecNumber(i,0,false);
    delayMicroseconds(50);
  }
  
 delay(500);


*/






/* for(int i=a;i>=0;i--){
   a-=1;
   b-=0.5;
   module.setDisplayToString(String(String(a)+ " "+String(b)));
   delay(500);
 }*/
  /*for (a=10000; a<11000; a++)
 {
 module.setDisplayToDecNumber(a,4,false);
 delay(1);
 }
 for (a=10000; a<11000; a++)
 {
 module.setDisplayToDecNumber(a,0,true);
 delay(1);
 }*/
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
    sanoma = "";
    sanomaReady = false;
  }
 }
}
void lcdMessage(){
  module.setDisplayToString(sanoma);


}
