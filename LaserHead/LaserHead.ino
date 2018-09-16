/* Dieses Programm soll über das Handy ausgelöst die einzelnen Schritte des Programms auslösen, baut auf dem Funktionalen Schlussprogramm auf*/
//funktioniert perfekt

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

HardwareSerial Serial1(1);                                            // Macht einen HardwareSerial für Laser 1
HardwareSerial Serial2(2);                                            // Macht einen HardwareSerial für Laser 2
#define SERIAL1_RXPIN 32                                              // definiert den RXPIN von Laser 1
#define SERIAL1_TXPIN 25                                              // definiert den TXPIN von Laser 1
#define SERIAL2_RXPIN 27                                              // definiert den RXPIN von Laser 2
#define SERIAL2_TXPIN 13                                              // definiert den TXPIN von Laser 1
#define BLYNK_PRINT Serial

//Achtung, bei unserem Motorentreiber sind Step und Dir Anschlüsse vertauscht.
const int stepPin = 14;                                               // definiert den stepPin des Stepper - Motorentreiber
const int dirPin = 26;                                                // definiert den dirPin des Stepper - Motorentreiber
const int enPin = 12;                                                 // definiert den enPin des Stepper - Motorentreiber
float schritt = 1.8;                                                  // deg/step of motor
float anzahl = 10;                                                    // Anzal Messungen, die ich will
float s = 360 / anzahl;                                               // Grad, die zwischen Messungen gefahren werden müssen
float aschritte = s / schritt;                                        // Anzal Schritte, die zwischen den Messungen gemacht werden müssen
int y = 0;                                                            // Variable um die Messungen von Laser 1 im Array messungen an die richtige Stelle zu schreiben
int z = 0;                                                            // Variable um die Messungen von Laser 2 im Array messungenn an die richtige Stelle zu schreiben
int ort = 100;
int zaehler = 10;
const float pi = 3.141;
static float messungen[40] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40};         // Generieren des Arrays um die Messungen einzutragen
static float messungenn[40] = {1, 2, 3};
String sensorRead;                                                    // Variable um zu überprüfen, dass alle Daten vom Laser übertragen wurden
String sensorReadn;
char incomingByte = 0;                                                // Variable um die einzelnen Schritte auszulösen
int falsecount = 0;                                                   // Zur Minimumsuche, sobald Falsecount den Wert 2 erreicht hat, ist das Programm beendet
int drehrichtung = 0;                                                 // Zur Bestimmung Drehrichtung bei der Minimumssuche
int programmzaehler = 0;                                              // Um durch das Programm zu führen
int entscheidungsvariable = 0;
int printnr = 0;                                                      // Um zu definieren, welchen Schritt man printen möchte
int counter = 0;                                                      // Um zu definieren, dass das Printen nur einmal geschieht
int Startpositionbestaetigung = 0;                                    // Um die Startposition zu bestätigen
int Startpositionbestaetigung2 = 0;                                   // Um die zweite Startposition zu bestätigen
int Startmessungsbestaetigung = 0;                                     // Um den Start der Messung auszulösen
float differenz = 0;
float tiefeTank = 0;
float tiefeSonde = 0;
float tiefeSludge = 0;
float flaecheTank = 0;
float volumenTank = 0;
float volumenSludge = 0;
float fuellstand = 0;
float eingabe = 0;
float ergebniss = 0;


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "a619eecff4f54be7b975ff76b23f81b4";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "qkn-11345"; //"Ammandips iPhone Neu";
char pass[] = "8z2m-lygk-zhfg-i00y";//"guggi113";

BLYNK_WRITE(V1)
{
  entscheidungsvariable = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V1 Slider value is: ");
  Serial.println(entscheidungsvariable);
}//WriteV1end


BLYNK_WRITE(V2)
{
  float latitude = param[0].asFloat();
  float longitude = param[1].asFloat();
  float altitude = param[2].asFloat();
  float geschwindigkeit = param[3].asFloat();
}//WriteV2end


//Attach WidgetLCD to Virtual Pin V3
WidgetLCD lcd(V3);

// Attach WidgetTermnial to Virtual Pin V4
WidgetTerminal terminal(V4);

BLYNK_WRITE(V4)
{
  tiefeSonde = param.asFloat();
  terminal.flush();

}

// This function will be called every time Button Widget
// in Blynk app writes values to the Virtual Pin V5
BLYNK_WRITE(V5)
{
  Startpositionbestaetigung = param.asInt(); // assigning incoming value from pin V5 to a variable

}

// This function will be called every time Butten Widget
// in Blynk app writes values to the Virtual Pin V6
BLYNK_WRITE(V6)
{
  Startpositionbestaetigung2 = param.asInt(); // assigning incoming value from pin V6 to a variable

}

// This function will be called every time Butten Widget
// in Blynk app writes values to the Virtual Pin V7
BLYNK_WRITE(V7)
{
  Startmessungsbestaetigung = param.asInt(); // assigning incoming value from pin V6 to a variable

}

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
  Blynk.begin(auth, ssid, pass);                                      // Beginnen der seriellen Kommunikation mit der Blynk App
  int entscheidungsvariable = 0;
  lcd.clear();
  lcd.print(0, 0, "Start Messung");
  terminal.println("Start Messung");
  terminal.flush();
  Blynk.virtualWrite(V5, LOW);
  Blynk.virtualWrite(V6, LOW);
  Blynk.virtualWrite(V7, LOW);
  Blynk.virtualWrite(V1, 0);

  delay(50);
  Serial2.write("O");


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

void printen(float messergebnisse[], int laenge, int start) {     // Funktion nimmt einen String der Länge laenge und printet die Einträge von start bis laenge
  for (int i = start; i < laenge; i++) {
    Serial.print(messergebnisse[i]);
    Serial.print(' ');
  }//forend
}//printend

void minimumfinden(float messergebnisse[], int laenge) {          // Funktion sucht das minimum eines Strings mit Länge laenge
  float minimum = _min(messergebnisse[0], messergebnisse[1]);
  for (int i = 2; i < laenge; i++) {
    minimum = _min(minimum, messergebnisse[i]);
  }//forend
  Serial.println(minimum);                                        // Printen des Minimums
  for (int x = 0; x < laenge; x++) {                               // Heraussuchen der Stelle des Minimums im Array
    if (minimum == messergebnisse[x]) {
      Serial.print("Die kürzeste Strecke befindet sich an Position ");
      Serial.print(x);                                            // Gibt die Stelle im Array des Minimums an
      ort = x;
      Serial.print(" des Arrays und beträgt: ");
      Serial.print(messergebnisse[x]);
    }//ifend
  }//forend
}//minimumfindenend

void vergleichen(float messergebnisse[], int laenge) {            // Funktion vergleicht, ob die neue Messung kürzer ist als die Alte
  if (zaehler <= laenge) {                                        // Solange der Zähler kleiner ist, als die Länge des Arrays
    if (messergebnisse[zaehler + 1] <= messergebnisse[zaehler]) { // Wird ausgeführt, wenn die neue Messung kleiner gleich die Alte ist
      Serial.println("Neue Messung kürzer als vorherige");
    }//ifkürzerend
    if (messergebnisse[zaehler + 1] > messergebnisse[zaehler]) {  // Wird ausgeführt, wenn die neue Messung länger als die Alte ist
      Serial.println("Neue Messung länger als vorherige");
      falsecount++;                                               // Hier wird gezählt, wie oft dies der Fall ist
      if (falsecount == 1) {                                      // Beim ersten Mal ausführen der Schlaufe, wird die Drehrichtung des Motors geändert
        drehrichtung = 1;
      }//if=1end
    }//iflängerend
  }//ifzaehlerkleinerlaengeend
}//vergleichenend

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

float volumenberechnung(float messung, float flaeche) {         // Funktion berechnet das Volumen aus einer Längenmessung und einer Fläche
  float volumen = messung * flaeche;
  return volumen;
}

void loop() { // run over and over

  Blynk.run();

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
    //    lcd.print(0, 1, "L2:");
    //    lcd.print(4, 1, messungenn[z]);
    //    lcd.print(10, 1, "m");
    z++;
  }

  sensorReadn = "";                                                   // Der String sensorReadn wird wieder auf Null gesetzt

  switch (entscheidungsvariable) {
    case 1:
      //programm erster schritt, wo der laser 2 eine messung macht und laser 1 noch aus ist
      printnr = 1;
      counter = 0;
      lcd.clear();
      lcd.print(0, 0, "Schritt 1");
      terminal.println("Schritt 1");
      terminal.flush();
      Serial.println("Laser 1 aus");
      delay(1000);
      Serial.println("Laser 2 ein");
      Serial.println("Messung Laser 2, Laser 2 befindet sich zu oberst beim Tripod -> Speichern Wert in Array messungenn an Stelle 1.");
      z = 0;
      Serial2.write("D");
      delay(1000);
      entscheidungsvariable = 0;
      break;
    case 2:
      //programm zweiter schritt, wenn man das Gerät runterlässt und Laser 2 überprüft, dass Sludge nicht zu nahe und Laser 1 angeht für Positionierung
      Serial.println("Laser 2 überprüft während dem runterfahren, dass der Sludge nicht zu nahe kommt");
      z = 1;
      printnr = 2;
      counter = 0;
      Serial1.write("O");
      Serial2.write("D");
      delay(1000);
      Serial.println("falls doch, sendet eine Warunung");
      delay(1000);
      Serial.println("Schritt 2 fertig");
      entscheidungsvariable = 0;
      break;
    case 3:
      //Programm dritter Schritt, wo der Laser 1 anfängt Messungen zu machen, damit man die Tankgeometrie sieht und Laser 2 weiter misst, um die
      //zurückgelegte Distanz herauszugeben
      Serial.println("Laser 1 geht an, um Positionierung zu vereinfachen, macht keine Messungen, nur Laserpunkt zur Positionierung");
      delay(1000);
      Serial.println("Laser 2 überprüft immer noch Abstand zum Sludge");
      delay(1000);
      Serial.println("Bitte Startposition bestätigen");
      delay(1000);
      Serial.println("Sobald die Startposition bestätigt wurde, Laser 2 zweite Messung, welche in Array messungenn an Stelle 2 gespeichert wird");
      delay(1000);
      Serial.println("Schritt 3 fertig");
      y = 0;
      z = 2;
      printnr = 3;
      counter = 0;
      Serial2.write("D");
      delay(1000);
      Serial1.write("D");
      delay(1000);
      entscheidungsvariable = 0;
      break;
    case 4:
      //programm vierter Schritt, Messung startet
      //      Serial.println("Start Messung");
      //      delay(1000);
      //      Serial.println("Tankfläche wird ausgegeben");
      //      delay(1000);
      //      Serial.println("Schreiben Messung fertig, fortfahren mit Sonde");
      //      delay(1000);
      //      Serial.println("Fragen für Messwert von Sonde einfügen");
      //      delay(1000);
      //      Serial.println("Schritt 5 fertig");

      if (Startmessungsbestaetigung == 1) {
        incomingByte = 'T';
        counter = 0;
        y = 0;
        entscheidungsvariable = 0;
      }
      break;
    case 5:
      //programm fünfter Schritt, Messung Sonde
      Serial.println("Eingabe Messwert Sonde");
      delay(1000);
      terminal.println("Bitte Messwert der Sonde eingeben.");
      terminal.print("Messwert Sonde: ");
      terminal.flush();
      printnr = 5;
      entscheidungsvariable = 0;
      break;

    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }//switchend

  if (printnr == 1 && counter == 10) {
    lcd.print(0, 1, "L2:");
    lcd.print(4, 1, messungenn[0]);
    lcd.print(10, 1, "m");
    printnr = 0;
  }//printnr1counter10end

  if (printnr == 2 && counter == 10) {
    lcd.print(0, 1, "L2:");
    lcd.print(4, 1, messungenn[1]);
    lcd.print(10, 1, "m");
    if (messungenn[1] <= 0.1) {
      lcd.clear();
      lcd.print(0, 0, "Achtung!");
      terminal.println("Achtung Sludge zu nahe!");
      terminal.flush();
    }//ifmessungenzunaheend
    if (Startpositionbestaetigung == 1) {
      entscheidungsvariable = 0;
    }//ifknopfgedrücktend
    if (Startpositionbestaetigung == 0) {
      entscheidungsvariable = 2;
    }//ifknopfnichtgedrücktend
  }//printnr2counter10end

  if (printnr == 3 && counter == 10) {
    lcd.clear();
    lcd.print(0, 0, "L1:");
    lcd.print(3, 0, messungen[0]);
    lcd.print(9, 0, "m");
    differenz = messungenn[1] - messungenn[2];
    lcd.print(0, 1, "D: ");
    lcd.print(2, 1, differenz);
    lcd.print(8, 1, "m");
    if (messungenn[2] <= 0.1) {
      terminal.println("Achtung Sludge zu nahe!");
      terminal.flush();
    }//ifmessungenzunaheend
    if (Startpositionbestaetigung2 == 1) {
      entscheidungsvariable = 0;
    }//ifknopfgedrücktend
    if (Startpositionbestaetigung2 == 0) {
      entscheidungsvariable = 3;
    }//ifknopfnichtgedrücktend
  }//printnr2counter10end

  if (printnr == 5 && tiefeSonde != 0) {
    terminal.print(tiefeSonde);
    terminal.println(" m");
    terminal.flush();
    tiefeTank = tiefeSonde - (messungenn[0] - messungenn[2]);
    tiefeSludge = tiefeTank - (messungenn[2]);
    volumenTank = volumenberechnung(tiefeTank, flaecheTank);
    volumenSludge = volumenberechnung(tiefeSludge, flaecheTank);
    fuellstand = (volumenSludge / volumenTank) * 100;
    Serial.println("Ausgabe Tanktiefe = Messwert Sonde - (Startwert Laser - Startwert Messung)");
    Serial.println(tiefeTank);
    terminal.print("Tanktiefe: ");
    terminal.print(tiefeTank);
    terminal.println(" m");
    terminal.flush();
    delay(1000);
    Serial.println("Ausgabe Sludgetiefe = Tanktiefe - Startwert Messung");
    Serial.println(tiefeSludge);
    terminal.print("Sludgetiefe: ");
    terminal.print(tiefeSludge);
    terminal.println(" m");
    terminal.flush();
    delay(1000);
    Serial.println("Ausgabe Tankvolumen = Tanktiefe * Flaeche von Schritt 5");
    Serial.println(volumenTank);
    terminal.print("Tankvolmen: ");
    terminal.print(volumenTank);
    terminal.println(" m^3");
    terminal.flush();
    delay(1000);
    Serial.println("Ausgabe Sludgevolumen = Flaeche von Schritt 5 * (Tanktiefe - Startwert Messung)");
    Serial.println(volumenSludge);
    terminal.print("Sludgevolmen: ");
    terminal.print(volumenSludge);
    terminal.println(" m^3");
    terminal.flush();
    delay(1000);
    Serial.println("Ausgabe Füllstand in Prozent = Sludgevolumen/Tankvolumen * 100");
    Serial.println(fuellstand);
    terminal.print("Füllstand: ");
    terminal.print(fuellstand);
    terminal.println(" %");
    terminal.flush();
    delay(1000);
    lcd.clear();
    lcd.print(0, 0, "TV:");
    lcd.print(3, 0, volumenTank);
    lcd.print(9, 0, "m^3");
    lcd.print(0, 1, "SV:");
    lcd.print(3, 1, volumenSludge);
    lcd.print(9, 1, "m^3");
    Serial.println("Schritt 5 fertig");
    printnr = 0;
  }//printnr1counter10end

  if (incomingByte == 'T' && counter == 0) {                //In diesem Abschnitt werden die 10 ersten Messungen gemacht
    Serial1.write("D");                                             //Senden Messbefehl an Laser 1
    delay(1000);
    for (int i = 0; i < 9; i++) {
      drehung(aschritte, 0);                                        // Machen der 9 Drehungen
      Serial1.write("D");                                           //Senden Messbefehl an Laser 1
      delay(1000);
    }//forend
  }//ifendR

  if (incomingByte == 'T' && counter == 20) {               //In diesem Teil wird zur kürzesten Stelle der ersten 10 Messungen gedreht
    printen(messungen, 10, 0);
    minimumfinden(messungen, 10);
    Serial.print(ort);
    if (ort <= 9) {                                                 // Wird nur ausgeführt, wenn sich der Ort innerhalb der 10 Messungen befindet
      if (ort <= 4) {                                               // Wenn der Ort in den ersten 5 Einträgen des Arrays ist, dreht der Motor in die gleiche Richtung weiter
        for (int i = 0; i <= ort; i++) {
          drehung(aschritte, 0);
        }//forortschleife
      }//ifort1end

      if (ort > 4) {                                                // Wenn der Ort in den letzten 5 Einträgen des Arrays ist, dreht der Motor in die andere Richtung zurück
        int ortlinks = 9 - ort;
        for (int j = 0; j < ortlinks; j++) {
          drehung(aschritte, 1);
        }//forortschleife
      }//ifort2end
    }//ifendD
  }//ifAend

  if (incomingByte == 'T' && counter == 21) {
    delay(500);
    drehung(2, 0);
    delay(500);
    Serial1.write("D");                                             //Senden Messbefehl an Laser 1
    delay(1000);
    drehung(2, 0);
    Serial1.write("D");                                             //Senden Messbefehl an Laser 1
    delay(1000);
  }//ifWend

  if (incomingByte == 'T' && counter == 24 || incomingByte == 'T' && counter == 28 || incomingByte == 'T' && counter == 32 || incomingByte == 'T' && counter == 36
      || incomingByte == 'T' && counter == 40 || incomingByte == 'T' && counter == 44 || incomingByte == 'T' && counter == 48 || incomingByte == 'T' && counter == 52
      || incomingByte == 'T' && counter == 56 || incomingByte == 'T' && counter == 60 || incomingByte == 'T' && counter == 64 || incomingByte == 'T' && counter == 68
      || incomingByte == 'T' && counter == 72 || incomingByte == 'T' && counter == 76 || incomingByte == 'T' && counter == 80 || incomingByte == 'T' && counter == 84
      || incomingByte == 'T' && counter == 88 || incomingByte == 'T' && counter == 92 || incomingByte == 'T' && counter == 96 || incomingByte == 'T' && counter == 100
      || incomingByte == 'T' && counter == 104) {
    vergleichen(messungen, 33);
    zaehler++;
    //}//ifcounternd
  }//ifVend

  if (incomingByte == 'T' && counter == 25 || incomingByte == 'T' && counter == 29 || incomingByte == 'T' && counter == 33 || incomingByte == 'T' && counter == 37
      || incomingByte == 'T' && counter == 41 || incomingByte == 'T' && counter == 45 || incomingByte == 'T' && counter == 49 || incomingByte == 'T' && counter == 53
      || incomingByte == 'T' && counter == 57 || incomingByte == 'T' && counter == 61 || incomingByte == 'T' && counter == 65 || incomingByte == 'T' && counter == 69
      || incomingByte == 'T' && counter == 73 || incomingByte == 'T' && counter == 77 || incomingByte == 'T' && counter == 81 || incomingByte == 'T' && counter == 85
      || incomingByte == 'T' && counter == 89 || incomingByte == 'T' && counter == 93 || incomingByte == 'T' && counter == 97 || incomingByte == 'T' && counter == 101
      || incomingByte == 'T' && counter == 105) {
    if (falsecount < 3) {
      Serial.print("falsecount = ");
      Serial.println(falsecount);
      if (zaehler <= 33) {                                                // Beschränkt, dass man nur bis zu der richtigen Stelle im Array liest
        if (falsecount < 2) {                                             // Sorgt dafür, dass solange die neue Messung kürzer ist, in die gleiche Richtung gedreht wird
          drehung(2, drehrichtung);
          Serial1.write("D");                                             // Senden Messbefehl an Laser 1
          delay(1000);
        }//falsecountkleinerend
        if (falsecount == 2) {                                            // Sobald die neue Messung zum 2ten Mal länger ist, wird ein Stück zurückgedreht
          drehung(4, 0);
          falsecount = 3;                                                 // Der Falschzähler wird auf 3 gesetzt, damit dieser Schritt sicher nicht mehr ausgeführt wird
          counter = 110;                                          // Danach wird der Programmzähler auf 110 gesetzt, damit der Rest des Programms ausgeführt werden kann,
        }//falsecount2end                                                 // auch wenn das Minimum vor Erreichen der Endposition des Arrays gefunden worden ist
      }//zahelerend
    }//falsecountend
    //}//counterend
  }//ifXend



  if (incomingByte == 'T' && counter == 112) {                    //In diesem Teil werden die Messungen im 90 Grad Schritt gemacht
    y = 34;                                                               //Bestimmt den Startort der Messungen für die letzten Messungen
    //    Serial2.write("D");                                                   //Senden Messbefehl an Laser 2
    if (ort <= 4) {
      Serial1.write("D");                                                 // Messung an kürzester Stelle
      delay(1000);
      drehung(50, 1);                                                     // Drehung um 90 Grad
      Serial1.write("D");                                                 // Messung an 90 Grad Stelle
      delay(1000);
      drehung(35, 1);                                                     // Drehung um 63 Grad
      Serial1.write("D");                                                 // 1 Messung zum Erkennen der Geometrie
      delay(1000);
      drehung(15, 1);                                                     // Drehung zu 180 Grad
      Serial1.write("D");                                                 // Messung an 180 Grad Stelle
      delay(1000);
      drehung(15, 1);                                                     // Drehung um 27 Grad
      Serial1.write("D");                                                 // 2 Messung zum Erkennen der Geometrie
      delay(1000);
      drehung(35, 1);                                                     // Drehung zu 270 Grad
      Serial1.write("D");                                                 // Messung an 270 Grad Stelle
      delay(1000);
    }//ifort1end

    if (ort > 4) {
      Serial1.write("D");                                                 // Messung an kürzester Stelle
      delay(1000);
      drehung(50, 0);                                                     // Drehung um 90 Grad
      Serial1.write("D");                                                 // Messung an 90 Grad Stelle
      delay(1000);
      drehung(35, 0);                                                     // Drehung um 63 Grad
      Serial1.write("D");                                                 // 1 Messung zum Erkennen der Geometrie
      delay(1000);
      drehung(15, 0);                                                     // Drehung zu 180 Grad
      Serial1.write("D");                                                 // Messung an 180 Grad Stelle
      delay(1000);
      drehung(15, 0);                                                     // Drehung um 27 Grad
      Serial1.write("D");                                                 // 2 Messung zum Erkennen der Geometrie
      delay(1000);
      drehung(35, 0);                                                     // Drehung zu 270 Grad
      Serial1.write("D");                                                 // Messung an 270 Grad Stelle
      delay(1000);
    }//ifort2end
  }//ifEend


  if (incomingByte == 'T' && counter == 134) {                    // Berechnet die Fläche und das Volumen
    //printen(messungen, 40, 0);
    printen(messungenn, 3, 0);
    flaecheTank = flaecheberechnen(messungen);
    Serial.println(' ');
    Serial.println("Fläche: ");
    Serial.println(flaecheTank);
    //    float volumen = volumenberechnung(messungenn, flaeche);
    //    Serial.println(' ');
    //    Serial.println("Volumen: ");
    //    Serial.println(volumen);
    incomingByte = 0;                                                     // incomingByte wieder auf 0
    Startmessungsbestaetigung = 0;
  }//ifendB

  counter++;
}//loopend


