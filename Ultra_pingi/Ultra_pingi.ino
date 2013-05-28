#include <NewPing.h>

#define TRIGGER_PIN  3
#define ECHO_PIN     4
#define MAX_DISTANCE 200
 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
int distance;

 
void setup() {
  Serial.begin(115200);
  pinMode(6,OUTPUT);
}
 
void loop() {
  delay(50);
  int uS = sonar.ping();
  Serial.print("Ping: ");
  distance=uS / US_ROUNDTRIP_CM;
  Serial.print(distance);
  Serial.println("cm");
  if (distance<50){
    digitalWrite(6,HIGH);
  }
  else{
    digitalWrite(6,LOW);
  }
}

void blink(){
  
  
}

