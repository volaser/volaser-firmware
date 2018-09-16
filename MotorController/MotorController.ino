/*     Dieses Programm lässt den Stepper in den richtigen Gradschritten einmal im Kreis drehen und in den richtigen Momenten Messungen mit dem Laser machen
 */

HardwareSerial Serial1(1);
// defines pins numbers
const int stepPin = 14; //Achtung, bei unserem Motorentreiber sind Step und Dir Anschlüsse vertauscht.
const int dirPin = 26;
const int enPin = 12;
#define SERIAL1_RXPIN 32
#define SERIAL1_TXPIN 25

// define variables
int y = 0;
float schritt = 1.8; // deg/step of motor
float n = 10; // Anzal Messungen, die ich will
float s = 360/n; // Grad, die zwischen Messungen gefahren werden müssen
float aschritte = s/schritt; // Anzal Schritte, die zwischen den Messungen gemacht werden müssen


 
void setup() {
  // Sets the three pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);
  Serial.begin(57600);    // Open serial communications and wait for port to open:
  Serial.print("Grad pro Schritt Motor: ");
  Serial.println(schritt);
  Serial.print("Anzahl Messungen: ");
  Serial.println(n);
  Serial.print("Anzahl Grad zwischen Messungen: ");
  Serial.println(s);
  Serial.print("Anzahl Schritte: ");
  Serial.println(aschritte);
  Serial1.begin(19200, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  Serial.println("\nSoftware serial test started");
  Serial.write("\nSESP test");

delay(50);
Serial1.write("O");

}
void loop() {
  
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  for(y; y < n ; y++){
    Serial.print("Messunglaser ");
    Serial.println(y);
    Serial1.write("D");
    delay(2000);
    Serial.print("Drehung ");
    Serial.print(y);
    Serial.println();
    
  // Makes 200 pulses for making one full cycle rotation
    for(int x = 0; x < aschritte; x++) {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(500); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(500); 
      Serial.print(x);
    } //forschritteschleife
    Serial.println();
    delay(1000); // One second delay
  } //formessungsschleife
  
 if (Serial1.available()) {
   Serial.write(Serial1.read());
 }
  
  
}

