/* 
 *  Created By Devbrat Raghuvanshi 
 */
int led = 0; //Led pin

void setup() {
  pinMode(led,OUTPUT);

}

void loop() {
  digitalWrite(led,HIGH);
  delay(1000);
  digitalWrite(led,LOW);
  delay(1000);
}
