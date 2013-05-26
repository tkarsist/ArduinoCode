/*
  Physical Pixel
  
  created 2006
  by David A. Mellis
  modified 14 Apr 2009
  by Tom Igoe and Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/PhysicalPixel
  
  CODE CHANGED AND UPDATED FOR PERSONAL USE BY JEREMIAH M DUKE 7-19-2009
  YOU ARE FREE TO USE THIS SOFTWARE AS LONG AS YOU KEEP ALL OF THE ABOVE INTACT.
  I AM NOT THE ORIGINAL CREATOR OF THIS SOFTWARE.
  
  */
 int ledPin13 = 13; // the pin that the green LED is attached to
 int ledPin12 = 12; // the pin that the red LED is attached to
 int incomingByte;      // a variable to read incoming serial data into
 void setup() {
   
   Serial.begin(9600); // initialize serial communication
  
   pinMode(ledPin13, OUTPUT);  // initialize the green LED pin as an output
   pinMode(ledPin12, OUTPUT);  // initialize the red LED pin as an output
 }
 void loop() {
   // see if there's incoming serial data:
   if (Serial.available() > 0) {
     
     incomingByte = Serial.read(); // read the oldest byte in the serial buffer
//Preform the code to switch on or off the leds
    if (incomingByte == '0') {
    digitalWrite(ledPin13, HIGH); //If the serial data is 0 turn red LED on
  } 
   if (incomingByte == '1') {
   digitalWrite(ledPin13, LOW); //If the serial data is 1 turn red LED off
 }
 
     if (incomingByte == '2') {
    digitalWrite(ledPin12, HIGH); //If the serial data is 2 turn green LED on
  } 
   if (incomingByte == '3') {
   digitalWrite(ledPin12, LOW); //If the serial data is 3 turn green LED off
 }

   }
 }
