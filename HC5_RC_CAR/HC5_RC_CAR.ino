#define Mtr1S1 12
#define Mtr1S2 11
#define Mtr2S1 10
#define Mtr2S2 9

void setup() {
  pinMode(Mtr1S1, OUTPUT);
  pinMode(Mtr1S2, OUTPUT);
  pinMode(Mtr2S1, OUTPUT);
  pinMode(Mtr2S2, OUTPUT);
  
  Serial.begin(9600); // Default communication rate of the Bluetooth module
}
void loop() {
  char direc ;
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
    direc = Serial.read(); // Reads the data from the serial port
    delay(10);
    Serial.print("text: ");
    Serial.println(direc);
 }
  if (direc == 's' || direc == 'S') {
  Stop();
  Serial.println("Stoped");
 }
  if (direc == 'f' || direc == 'F') {
  MoveForward();
  Serial.println("Going Forward");
 }
  if (direc == 'b' || direc == 'B') {
  MoveBack();
  Serial.println("Going Back");
 }
  if (direc == 'l' || direc == 'L') {
  TurnLeft();
  Serial.println("Turn Left");
 }
  if (direc == 'r' || direc == 'R') {
  TurnRight();
  Serial.println("Turn Right");
 }
 delay(100);
}

void Stop(){
    LeftMotorStop();
    RightMotorStop();
}
void MoveForward(){
    LeftMotorForward();
    RightMotorForward();
}
void MoveBack(){
    LeftMotorBackward();
    RightMotorBackward();  
}
void TurnLeft(){
   LeftMotorBackward();
   RightMotorForward();
}
void TurnRight(){
  RightMotorBackward(); 
  LeftMotorForward();
}

void LeftMotorStop(){
      digitalWrite(Mtr1S1,LOW);
    digitalWrite(Mtr1S2,LOW);
}
void LeftMotorForward(){
      digitalWrite(Mtr1S1,HIGH);
    digitalWrite(Mtr1S2,LOW);
}
void LeftMotorBackward(){
  digitalWrite(13, HIGH);
      digitalWrite(Mtr1S1,LOW);
    digitalWrite(Mtr1S2,HIGH);
}
void RightMotorStop(){
      digitalWrite(Mtr2S1,LOW);
    digitalWrite(Mtr2S2,LOW);
}
void RightMotorForward(){
      digitalWrite(Mtr2S1,HIGH);
    digitalWrite(Mtr2S2,LOW);
}
void RightMotorBackward(){
      digitalWrite(Mtr2S1,LOW);
    digitalWrite(Mtr2S2,HIGH);
}
