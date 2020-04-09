/* Created By Devbrat Raghuvanshi 
//total puls width should be ~20 milSec 
//1500 micro sec high and rest low will move servo to -90° angle
//1000 micro sec high and rest low will move servo to 0° angle
//2000 micro sec high and rest low will move servo to 180° angle
*/


const byte servo0  =    0;         // Servo  0 pin on ATtiny // Digital IO pin
const byte servo1  =    1;         // Servo  1 pin on ATtiny // Digital IO pin
const byte servo2  =    3;         // Servo  1 pin on ATtiny // A3 pin is analog but you can use digitalWrite function on a analog pin
const byte servo3  =    4;         // Servo  1 pin on ATtiny // A2 pin is analog but you can use digitalWrite function on a analog pin
int pt_Val = 0;
int pw = 1000;
void setup() {                
  pinMode(servo0, OUTPUT);     
  pinMode(servo1, OUTPUT);     
  pinMode(servo2, OUTPUT);  
  pinMode(servo3, OUTPUT);  
}

void loop() {
  pt_Val = analogRead(A1); // A1 pin of attiny will be attached to potentiometer
  pw = map(pt_Val, 0, 1023, 400,2400); // depending upon potentiometer try to increase or decrease maping from 100 to 2000 see what works for you
  digitalWrite(servo0, HIGH);
  digitalWrite(servo1, HIGH); // Set pin high to start pulse
  digitalWrite(servo2, HIGH);
  digitalWrite(servo3, HIGH);
  delayMicroseconds(pw);      // High pulse angle data   
  digitalWrite(servo0,LOW);  
  digitalWrite(servo1,LOW);  // Set low for the rest of pulse
  digitalWrite(servo2,LOW); 
  digitalWrite(servo3,LOW); 
  delayMicroseconds(20000-pw);
}
