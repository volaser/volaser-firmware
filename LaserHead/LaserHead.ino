/* Im Vergleich zu Schlussprogramm 4 habe ich in Schlussprogramm 6 das Minimumsuchen
  richtig gemacht und die Volumenberechnung eingefügt. Dieses Programm funktioniert vollständig, jedoch noch in Einzelschritten.*/
//funktioniert

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
int zaehler = 10;
const float pi = 3.141;
static float messungen[40] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40};         // Generieren des Arrays um die Messungen einzutragen
static float messungenn[3] = {1, 2, 3};
String sensorRead;                                                    // Variable um zu überprüfen, dass alle Daten vom Laser übertragen wurden
String sensorReadn;
char incomingByte = 0;                                                // Variable um die einzelnen Schritte auszulösen
int falsecount = 0;                                                   // Zur Minimumsuche, sobald Falsecount den Wert 2 erreicht hat, ist das Programm beendet
int drehrichtung = 0;                                                 // Zur Bestimmung Drehrichtung bei der Minimumssuche

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


  //void turn_motor(int degrees, float speed) {
  //  // Do Stuff with speed and float
  //  int i, value
  //  return value[];
  //  }

}//setupend

void drehung(int schritte, int richtung) {
  if ( richtung == 0) {
    digitalWrite(dirPin, HIGH);                                     // Enables the motor to move in a particular direction
    for (int x = 0; x < schritte; x++) {                           // For-Schleife für x kleiner Anzahl Schritte
      digitalWrite(stepPin, HIGH);                                  // Senden der Pulse für einen Schritt an den Steppermotor
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      Serial.println(x);
    } //forschritteschleifeend
  }//ifend
  if ( richtung == 1) {
    digitalWrite(dirPin, LOW);                                     // Enables the motor to move in a particular direction
    for (int x = 0; x < schritte; x++) {                           // For-Schleife für x kleiner Anzahl Schritte
      digitalWrite(stepPin, HIGH);                                  // Senden der Pulse für einen Schritt an den Steppermotor
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      Serial.println(x);
    } //forschritteschleifeend
  }//ifend
}//drehungend

void printen(float messergebnisse[], int laenge, int start) {
  for (int i = start; i < laenge; i++) {
    Serial.print(messergebnisse[i]);
    Serial.print(' ');
  }//forend
}//printend

void minimumfinden(float messergebnisse[], int laenge) {
  float minimum = min(messungen[0], messungen[1]);
  for (int i = 2; i < laenge; i++) {
    minimum = min(minimum, messungen[i]);
  }//forend
  Serial.println(minimum);
  for (int x = 0; x < laenge; x++) {
    if (minimum == messungen[x]) {
      Serial.print("Die kürzeste Strecke befindet sich an Position ");
      Serial.print(x);                                            // Gibt die Stelle im Array des Minimums an
      ort = x;
      Serial.print(" des Arrays und beträgt: ");
      Serial.print(messungen[x]);
    }//ifend
  }//forend
}

void vergleichen(float messergebnisse[], int laenge) {
  if (zaehler <= laenge) {
    if (messergebnisse[zaehler + 1] <= messergebnisse[zaehler]) {
      Serial.println("Neue Messung kürzer als vorherige");
    }
    if (messergebnisse[zaehler + 1] > messergebnisse[zaehler]) {
      Serial.println("Neue Messung länger als vorherige");
      falsecount++;
      if (falsecount == 1) {
        drehrichtung = 1;
      }
    }
  }
}

float flaecheberechnen(float messungsarray[]) { //alle diese Werte müssen angepasst werden, wenn der Speicherort im Array sich ändert
  if (messungsarray[36] < messungsarray[37] && messungsarray[38] < messungsarray[37]) {  //Tank hat eine Kreisfläche
    float durchmesser = messungsarray[34] + messungsarray[37];
    float flaeche = pi * (durchmesser / 2) * (durchmesser / 2);
    return flaeche;
  }//ifend
  else {
    float a = messungsarray[34] + messungsarray[37];
    float b = messungsarray[35] + messungsarray[39];
    float flaeche = a * b;
    return flaeche;
  }//elseend
}//flaechenberechnenend

float volumenberechnung(float messungenn[], float flaeche) {
  float volumen = messungenn[1] * flaeche;
  return volumen;
}

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
    Serial.println(sensorRead);
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

    if (incomingByte == 'W') {
      drehung(2, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(2, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      incomingByte = 0;                                               // incomingByte wieder auf 0
    }//ifWend

    if (incomingByte == 'X') {
      Serial.println(falsecount);
      if (zaehler <= 33) {
        if (falsecount < 2) {
          drehung(2, drehrichtung);
          Serial1.write("D");                                             //Senden Messbefehl an Laser 1
          delay(1000);
        }//falsecountkleinerend
        if (falsecount == 2) {
          drehung(4, 0);
          falsecount = 3;
        }//falsecount2end
        incomingByte = 0;                                               // incomingByte wieder auf 0
      }//zahelerend
    }//ifXend

    if (incomingByte == 'B') {
      //printen(messungen, 40, 0);
      printen(messungenn, 3, 0);
      float flaeche = flaecheberechnen(messungen);
      Serial.println(' ');
      Serial.println("Fläche: ");
      Serial.println(flaeche);
      float volumen = volumenberechnung(messungenn, flaeche);
      Serial.println(' ');
      Serial.println("Volumen: ");
      Serial.println(volumen);
      incomingByte = 0;                                               // incomingByte wieder auf 0
    }//ifendB


    if (incomingByte == 'E') { //In diesem Teil werden die Messungen im 90 Grad Schritt gemacht
      y = 34; //Bestimmt den Startort der Messungen für die letzten Messungen
      Serial2.write("D");                                             //Senden Messbefehl an Laser 2
      if (ort <= 4) {
        Serial1.write("D");                               // Messung an kürzester Stelle
        delay(1000);
        drehung(50, 1);                                               // Drehung um 90 Grad
        Serial1.write("D");                             // Messung an 90 Grad Stelle
        delay(1000);
        drehung(35, 1);                                               // Drehung um 63 Grad
        Serial1.write("D");                              // 1 Messung zum Erkennen der Geometrie
        delay(1000);
        drehung(15, 1);                                               // Drehung zu 180 Grad
        Serial1.write("D");                              // Messung an 180 Grad Stelle
        delay(1000);
        drehung(15, 1);                                               // Drehung um 27 Grad
        Serial1.write("D");                            // 2 Messung zum Erkennen der Geometrie
        delay(1000);
        drehung(15, 0);
        delay(1000);
        drehung(50, 1);                                               // Drehung zu 270 Grad
        Serial1.write("D");                              // Messung an 270 Grad Stelle
        delay(1000);
      }//ifort1end

      if (ort > 4) {
        Serial1.write("D");                               // Messung an kürzester Stelle
        delay(1000);
        drehung(50, 0);                                               // Drehung um 90 Grad
        Serial1.write("D");                              // Messung an 90 Grad Stelle
        delay(1000);
        drehung(35, 0);                                               // Drehung um 63 Grad
        Serial1.write("D");                               // 1 Messung zum Erkennen der Geometrie
        delay(1000);
        drehung(15, 0);                                               // Drehung zu 180 Grad
        Serial1.write("D");                              // Messung an 180 Grad Stelle
        delay(1000);
        drehung(15, 0);                                               // Drehung um 27 Grad
        Serial1.write("D");                              // 2 Messung zum Erkennen der Geometrie
        delay(1000);
        drehung(15, 1);
        delay(1000);
        drehung(50, 0);                                               // Drehung zu 270 Grad
        Serial1.write("D");                             // Messung an 270 Grad Stelle
        delay(1000);
      }//ifort2end
      incomingByte = 0;                                               // incomingByte wieder auf 0
    }//ifEend

    if (incomingByte == 'A') {  //In diesem Teil wird zur kürzesten Stelle der ersten 10 Messungen gedreht
      printen(messungen, 10, 0);
      minimumfinden(messungen, 10);
      Serial.print(ort);
      if (ort <= 9) {
        if (ort <= 4) {
          for (int i = 0; i <= ort; i++) {
            drehung(aschritte, 0);
          }//forortschleife
        }//ifort1end

        if (ort > 4) {
          int ortlinks = 9 - ort;
          for (int j = 0; j < ortlinks; j++) {
            drehung(aschritte, 1);
          }//forortschleife
        }//ifort2end
        incomingByte = 0;                                               // incomingByte wieder auf 0
      }//ifendD
    }//ifAend

    if (incomingByte == 'R') { //In diesem Abschnitt werden die 10 ersten Messungen gemacht
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      drehung(aschritte, 0);
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
      delay(1000);
      incomingByte = 0;
    }//ifendR

    if (incomingByte == 'M') {
      Serial1.write("D");                                             //Senden Messbefehl an Laser 1
    }//ifendM

    if (incomingByte == 'N') {
      Serial2.write("D");                                             //Senden Messbefehl an Laser 2
    }//ifendN

    if (incomingByte == 'D') {
      drehung(aschritte, 0);
      incomingByte = 0;                                               // incomingByte wieder auf 0
    }//ifendD

    if (incomingByte == 'P') {                                        // Printen des Arrays messungen
      printen(messungen, 40, 0);
      incomingByte = 0;
    }//ifendP

    if (incomingByte == 'Q') {                                        // Printen des Arrays messungen
      printen(messungenn, 3, 0);
      incomingByte = 0;
    }//ifendP

    if (incomingByte == 'F') {                                        // Suchen des Minimums des Arrays messungen
      minimumfinden(messungen, 10);
      Serial.print(ort);
      incomingByte = 0;
    }//ifendF

    if (incomingByte == 'G') {                                        // Suchen des Minimums des Arrays messungen
      minimumfinden(messungenn, 3);
      incomingByte = 0;
    }//ifendF


    if (incomingByte == 'K') {
      drehung (2, 0);
      incomingByte = 0;                                               // incomingByte wieder auf 0
    }//ifKend

    if (incomingByte == 'L') {
      drehung (2, 1);
      incomingByte = 0;                                               // incomingByte wieder auf 0
    }//ifLend

    if (incomingByte == 'V') {
      vergleichen(messungen, 33);
      zaehler++;
      incomingByte = 0;
    }//ifVend


  }//ifendserialavailable
}//loopend

