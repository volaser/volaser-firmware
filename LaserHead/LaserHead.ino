/* In diesem Programm kann man individuell alle Komponenten ansteueren */

HardwareSerial Serial1(1);                                            // Macht einen HardwareSerial für Laser 1
HardwareSerial Serial2(2);                                            // Macht einen HardwareSerial für Laser 2
#define SERIAL1_RXPIN 32                                              // definiert den RXPIN von Laser 1
#define SERIAL1_TXPIN 25                                              // definiert den TXPIN von Laser 1
#define SERIAL2_RXPIN 27                                              // definiert den RXPIN von Laser 2
#define SERIAL2_TXPIN 13                                              // definiert den TXPIN von Laser 1

//Achtung, bei unserem Motorentreiber sind Step und Dir Anschlüsse vertauscht.
const int stepPin = 14;                                               // definiert den stepPin des Stepper - Motorentreiber
const int dirPin = 26;                                                // definiert den dirPin des Stepper - Motorentreiber
const int enPin = 12;                                                 // definiert den enPin des Stepper - Motorentreiber
float schritt = 1.8;                                                  // deg/step of motor
float anzahl = 10;                                                    // Anzal Messungen, die ich will
float s = 360 / anzahl;                                               // Grad, die zwischen Messungen gefahren werden müssen
float aschritte = s / schritt;                                        // Anzal Schritte, die zwischen den Messungen gemacht werden müssen
int y = 0;                                                            // Variable um die Messungen im Array an die richtige Stelle zu schreiben
int z = 0;
int n = 2;                                                            // Im Moment nicht nötig: Für For-Schleife um das Minimum zu finden
int m = 0;                                                            // Im Moment nicht nötig: Für For-Schleife um das Minimum zu printen
int ort = 100;
static float messungen[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};         // Generieren des Arrays um die Messungen einzutragen
static float messungenn[3] = {1, 2, 3};
String sensorRead;                                                    // Variable um zu überprüfen, dass alle Daten vom Laser übertragen wurden
String sensorReadn;
char incomingByte = 0;                                                // Variable um die einzelnen Schritte auszulösen

void setup() {

  Serial.begin(57600);                                                // Öffnen der seriellen Kommunikation mit dem Computer
  Serial1.begin(19200, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);     // Öffnen der seriellen Kommunikation mit Laser 1
  Serial2.begin(19200, SERIAL_8N1, SERIAL2_RXPIN, SERIAL2_TXPIN);     // Öffnen der seriellen Kommunikation mit Laser 2
  pinMode(stepPin, OUTPUT);                                           // Sets stepPin as Outputs
  pinMode(dirPin, OUTPUT);                                            // Sets dirPin as Outputs
  pinMode(enPin, OUTPUT);                                             // Sets enPin as Outputs

  Serial.print("Grad pro Schritt Motor: ");
  Serial.println(schritt);                                            // Printet Anzahl Grad pro Schritt des Motors
  Serial.print("Anzahl Messungen: ");
  Serial.println(anzahl);                                             // Printet Anzahl Messungen
  Serial.print("Anzahl Grad zwischen Messungen: ");
  Serial.println(s);                                                  // Printet Anzahl Grad zwischen den Messungen
  Serial.print("Anzahl Schritte: ");
  Serial.println(aschritte);                                          // Printet Anzahl Schritte zwischen den Messungen
  Serial.println("\nSoftware serial test started");
  Serial.print("\nSESP test");

  delay(50);
  Serial1.write("O");                                                 // Schaltet Laser 1 ein
  Serial2.write("O");                                                 // Schaltet Laser 2 ein
}//setupend

void loop() { // run over and over

  while (Serial1.available()) {                                       // Wartet bis Serial input von Laser 1 vorhanden ist
    char input = Serial1.read();                                      // Schreibt die vorhandene Information der Variable input zu
    if (input != '\n') {                                              // Solange kein newline charakter vom Laser gesendet wird
      sensorRead += input;                                            // der String sensorRead mit den inputs gefüllt
    } else {
      break;
    } //elseend
  }//whileend

  while (Serial2.available()) {                                        // Wartet bis Serial input von Laser 1 vorhanden ist
    char inputn = Serial2.read();                                      // Schreibt die vorhandene Information der Variable input zu
    if (inputn != '\n') {                                              // Solange kein newline charakter vom Laser gesendet wird
      sensorReadn += inputn;                                           // der String sensorRead mit den inputs gefüllt
    } else {
      break;
    } //elseend
  }//whileend


  char seperator1 = ' ';                                              // separator1 entspricht dem Leehrzeichen
  char seperator2 = 'm';                                              // separator2 entspricht dem m nach der Messung
  int index1 = sensorRead.indexOf(seperator1);                        // index1 gibt die Stelle des separator1 im String an
  int index2 = sensorRead.indexOf(seperator2);                        // index2 gibt die Stelle des separator2 im String an
  String firstString = sensorRead.substring(index1 + 1, index2);      // generiert einen neuen String firstString beginnend von index1 + 1 bis index2
  char buf[20] = "";                                                  // generiert einen leeren char array von Länge 20
  firstString.toCharArray(buf, 20);                                   // der String firstString wird zu einem Charstring transformiert und in buf geschrieben
  float distance = atof(buf);                                         // der char string buf wird in einen float umgewandelt und in der Variable distance gespeichert
  if (distance != 0) {                                                // Wenn der Wert von distance nicht gleich 0 ist,
    messungen[y] = distance;                                          // wird sie an der Stelle y in den array messungen geschrieben
    Serial.println(messungen[y]);
    y++;
  }
  sensorRead = "";                                                    // Der String sensorRead wird wieder auf Null gesetzt


  int index1n = sensorReadn.indexOf(seperator1);                      // index1n gibt die Stelle des separator1 im String an
  int index2n = sensorReadn.indexOf(seperator2);                      // index2n gibt die Stelle des separator2 im String an
  String firstStringn = sensorReadn.substring(index1n + 1, index2n);  // generiert einen neuen String firstString beginnend von index1n + 1 bis index2n
  char bufn[20] = "";                                                 // generiert einen leeren char array von Länge 20
  firstStringn.toCharArray(bufn, 20);                                 // der String firstStringn wird zu einem Charstring transformiert und in buf geschrieben
  float distancen = atof(bufn);                                       // der char string bufn wird in einen float umgewandelt und in der Variable distance gespeichert
  if (distancen != 0) {                                               // Wenn der Wert von distancen nicht gleich 0 ist,
    messungenn[z] = distancen;                                        // wird sie an der Stelle z in den array messungenn geschrieben
    Serial.println(messungenn[z]);
    z++;
  }

  sensorReadn = "";                                                   // Der String sensorReadn wird wieder auf Null gesetzt

  if (Serial.available() > 0) {                                       // Abfragen ob eine Eingabe über Serial
    incomingByte = Serial.read();                                     // Wenn ja, wird der Wert in die Variable incomingByte geschrieben

    if (incomingByte == 'M') {
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
    }//ifendM

    if (incomingByte == 'N') {
      Serial2.write("D");                                             //Senden Messbefehl an Laser 2
    }//ifendN

    if (incomingByte == 'D') {
      digitalWrite(dirPin, HIGH);                                     // Enables the motor to move in a particular direction
      for (int x = 0; x < aschritte; x++) {                           // For-Schleife für x kleiner Anzahl Schritte
        digitalWrite(stepPin, HIGH);                                  // Senden der Pulse für einen Schritt an den Steppermotor
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
        Serial.println(x);
      } //forschritteschleifeend
      incomingByte = 0;                                               // incomingByte wieder auf 0
    }//ifendD

    if (incomingByte == 'P') {                                        // Printen des Arrays messungen
      for (int i = 0; i <= 9; i++) {
        Serial.print(messungen[i]);
        Serial.print(' ');
      }//forend
      incomingByte = 0;
    }//ifendP

    if (incomingByte == 'Q') {                                        // Printen des Arrays messungen
      Serial.println(messungenn[0]);
      Serial.print(' ');
      Serial.print(messungenn[1]);
      Serial.print(' ');
      Serial.print(messungenn[2]);
      Serial.println(' ');
      incomingByte = 0;
    }//ifendP

    if (incomingByte == 'F') {                                        // Suchen des Minimums des Arrays messungen
      float minimum = min(messungen[0], messungen[1]);
      for (int i = 2; i <= 9; i++) {
        minimum = min(minimum, messungen[i]);
      }//forend
      Serial.println(minimum);
      for (int x = 0; x <= 9; x++) {
        if (minimum == messungen[x]) {
          Serial.print("Die kürzeste Strecke befindet sich an Position ");
          Serial.print(x);                                            // Gibt die Stelle im Array des Minimums an
          ort = x;
          Serial.print(" des Arrays und beträgt: ");
          Serial.print(messungen[x]);
        }//ifend
      }//forend
      incomingByte = 0;
    }//ifendF

    if (incomingByte == 'S' && ort <= 9) {
      if (ort <= 4) {
        digitalWrite(dirPin, HIGH);                                     // Enables the motor to move in a particular direction
        for (int i = 0; i <= ort - 1; i++) {
          for (int x = 0; x < aschritte; x++) {                           // For-Schleife für x kleiner Anzahl Schritte
            digitalWrite(stepPin, HIGH);                                  // Senden der Pulse für einen Schritt an den Steppermotor
            delayMicroseconds(500);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(500);
            Serial.println(x);
          } //forschritteschleifeend
        }//forortschleife
        int halbschritt = aschritte / 2;
        for (int x = 0; x < halbschritt ; x++) {                         // For-Schleife für x kleiner Anzahl Schritte
          digitalWrite(stepPin, HIGH);                                  // Senden der Pulse für einen Schritt an den Steppermotor
          delayMicroseconds(500);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(500);
          Serial.println(x);
        } //forschritteschleifeend
      }//ifort1end

      if (ort > 4) {
        int ortlinks = 8 - ort;
        digitalWrite(dirPin, LOW);                                     // Enables the motor to move in a particular direction
        for (int j = 0; j < ortlinks; j++) {
          for (int y = 0; y < aschritte; y++) {                           // For-Schleife für x kleiner Anzahl Schritte
            digitalWrite(stepPin, HIGH);                                  // Senden der Pulse für einen Schritt an den Steppermotor
            delayMicroseconds(500);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(500);
            Serial.println(y);
          } //forschritteschleifeend
        }//forortschleife
        int halbschritt = aschritte / 2;
        for (int x = 0; x < halbschritt ; x++) {                         // For-Schleife für x kleiner Anzahl Schritte
          digitalWrite(stepPin, HIGH);                                  // Senden der Pulse für einen Schritt an den Steppermotor
          delayMicroseconds(500);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(500);
          Serial.println(x);
        } //forschritteschleifeend
      }//ifort2end
      incomingByte = 0;                                               // incomingByte wieder auf 0
    }//ifSend

  }//ifendserialavailable
}//loopend

