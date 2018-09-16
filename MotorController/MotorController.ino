/*     Dieses Programm lässt den Stepper in den richtigen Gradschritten einmal im Kreis drehen.
 */


// defines pins numbers
const int stepPin = 14; //Achtung, bei unserem Motorentreiber sind Step und Dir Anschlüsse vertauscht.
const int dirPin = 26;
const int enPin = 12;
// define variables
int y = 0;
int z = 0;
float schritt = 1.8; // deg/step of motor
float n = 10; // Anzal Messungen, die ich will
float s = 360/n; // Grad, die zwischen Messungen gefahren werden müssen
float aschritte = s/schritt; // Anzal Schritte, die zwischen den Messungen gemacht werden müssen

 
void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);
  Serial.begin(9600);
  Serial.print("Grad pro Schritt Motor: ");
  Serial.println(schritt);
  Serial.print("Anzahl Messungen: ");
  Serial.println(n);
  Serial.print("Anzahl Grad zwischen Messungen: ");
  Serial.println(s);
  Serial.print("Anzahl Schritte: ");
  Serial.println(aschritte);
}
void loop() {

  Serial.println("Stepper fährt zur kürzesten Distanz von der ersten Messung");
  digitalWrite(dirPin, LOW); // Drehen des Steppers um einige Grad nach links von der zuvor kürzesten Distanz
  for(z; z < 10; z++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }
  delay(1000);
  Serial.println("Stepper befindet sich nun am rechten Ende der kleinsten Messung");
    
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  for(y; y < 10 ; y++){
    Serial.print("Messunglaser ");
    Serial.println(y);
    delay(1000);
    Serial.print("Drehung ");
    Serial.print(y);
    Serial.println();
  // Makes 200 pulses for making one full cycle rotation
    for(int x = 0; x < 2; x++) {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(500); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(500); 
      Serial.print(x);
    }
    Serial.println();
    delay(1000); // One second delay
    
  }

  Serial.println("Stepper befindet sich nun am linken Ende der kleinsten Messung");
  
  
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(y; y < 400; y++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }
  delay(1000);
}
