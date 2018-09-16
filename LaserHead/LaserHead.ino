/*(15.05.18) FSchlussprogramm3 based on FSchlussprogramm2 but attempts are made
 * here to respond to first/following measurements:*/

// select key for first and following measurements: installed on V1 but does not
// execute anything yet -> next program (14.05.18) run-off for nth measurement
// other: works (15.05.18) change terminal input depending on measurement: works
// (16.05.18) accumulation rate may only be output for the following
// measurements: works (16.05.18) check if 10th measurement = 10.00, if yes,
// repeat area measurement, with other angle: not working yet
//!!!!! Attention, see what exactly is going on with deep tank, somehow
//!different values between first and follow-up measurements!!!!!!: checked
//!(16.05.18)
// S comment out serial communication: not done yet

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>

// HardwareSerial Serial1(1); // Makes a HardwareSerial for Laser 1
// HardwareSerial Serial2(2); // Makes a HardwareSerial for Laser 2
#define SERIAL1_RXPIN 32 // defines the RXPIN of Laser 1
#define SERIAL1_TXPIN 25 // defines the TXPIN of Laser 1
#define SERIAL2_RXPIN 27 // defines the RXPIN of Laser 2
#define SERIAL2_TXPIN 13 // defines the TXPIN of Laser 1
#define BLYNK_PRINT Serial

// Attention, with our motor driver Step and you connections are reversed.
const int stepPin = 14; // definiert den stepPin des Stepper - Motorentreiber
const int dirPin = 26;  // definiert den dirPin des Stepper - Motorentreiber
const int enPin = 12;   // definiert den enPin des Stepper - Motorentreiber
float schritt = 1.8;    // deg/step of motor
float anzahl = 10;      // Anzal Messungen, die ich will
float s = 360 / anzahl; // Grad, die zwischen Messungen gefahren werden müssen
float aschritte =
    s /
    schritt; // Anzal Schritte, die zwischen den Messungen gemacht werden müssen
int y = 0;   // Variable um die Messungen von Laser 1 im Array messungen an die
             // richtige Stelle zu schreiben
int z = 0;   // Variable um die Messungen von Laser 2 im Array messungenn an die
             // richtige Stelle zu schreiben
int ort = 100;
int zaehler = 10;
const float pi = 3.141;
static float messungen[40] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 30, 30, 30, 30, 30, 30, 30, 30,
    30, 30, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40}; // Generieren des Arrays um
                                                     // die Messungen
                                                     // einzutragen
static float messungenn[40] = {1, 2, 3};
String sensorRead; // Variable um zu überprüfen, dass alle Daten vom Laser
                   // übertragen wurden w
String sensorReadn;
char incomingByte = 0;   // Variable um die einzelnen Schritte auszulösen
int falsecount = 0;      // Zur Minimumsuche, sobald Falsecount den Wert 2 erreicht
                         // hat, ist das Programm beendet
int drehrichtung = 0;    // Zur Bestimmung Drehrichtung bei der Minimumssuche
int programmzaehler = 0; // Um durch das Programm zu führen
int entscheidungsvariable = 0;
int printnr = 0;                    // Um zu definieren, welchen Schritt man printen möchte
int counter = 0;                    // Um zu definieren, dass das Printen nur einmal geschieht
int Startpositionbestaetigung1 = 0; // Um die Startposition zu bestätigen
int Startpositionbestaetigung2 = 0; // Um die zweite Startposition zu bestätigen
int Startpositionbestaetigung3 = 0; // Um den Start der Messung auszulösen
int nummermessung =
    0; // Ist = 0 bei der ersten Messung und = 1 bei allen folgenden Messungen
int schlussschritte =
    0;               // Gibt vor, wie weit der Motor zurückgedreht werden muss am Schluss
int resetstatus = 0; // Wenn der Wert = 1 ist, soll der ESP neu gestartet werden
float differenz = 0;
float differenzalt = 0;
float tiefeTank = 0;
float tiefeSonde = 0;
float tiefeSludge = 0;
float flaecheTank = 0;
float volumenTank = 0;
float volumenSludge = 0;
float volumenSludgealt = 0;
float accumulationrate = 0;
float fuellstand = 0;
float eingabe = 0; // Eingabe in Terminal bei folgemessung
int eingabezaehler =
    0; // Zähler um die Eingaben den richtigen Variablen einzuweisen
float ergebniss = 0;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "a619eecff4f54be7b975ff76b23f81b4";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "qkn-11345";           //"Ammandips iPhone Neu";
char pass[] = "8z2m-lygk-zhfg-i00y"; //"guggi113";

// Attach WidgetLED to Virtual Pin V0
WidgetLED led1(V0);

// BLYNK_WRITE(V1)
//{
//  entscheidungsvariable = param.asInt(); // assigning incoming value from pin
//  V1 to a variable
//  // You can also use:
//  // String i = param.asStr();
//  // double d = param.asDouble();
//  Serial.print("V1 Slider value is: ");
//  Serial.println(entscheidungsvariable);
//}//WriteV1end

// This function will be called every time Butten Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  nummermessung =
      param.asInt(); // assigning incoming value from pin V1 to a variable
}

// This function will be called every time Button Widget
// in Blynk app writes values to the Virtual Pin V2
BLYNK_WRITE(V2)
{
  Startpositionbestaetigung1 =
      param.asInt(); // assigning incoming value from pin V2 to a variable
}

// This function will be called every time Butten Widget
// in Blynk app writes values to the Virtual Pin V3
BLYNK_WRITE(V3)
{
  Startpositionbestaetigung2 =
      param.asInt(); // assigning incoming value from pin V3 to a variable
}

// This function will be called every time Butten Widget
// in Blynk app writes values to the Virtual Pin V4
BLYNK_WRITE(V4)
{
  Startpositionbestaetigung3 =
      param.asInt(); // assigning incoming value from pin V4 to a variable
}

// This function will be called every time Butten Widget
// in Blynk app writes values to the Virtual Pin V5
BLYNK_WRITE(V5)
{
  resetstatus =
      param.asInt(); // assigning incoming value from pin V5 to a variable
}

BLYNK_WRITE(V6)
{
  float latitude = param[0].asFloat();
  float longitude = param[1].asFloat();
  float altitude = param[2].asFloat();
  float geschwindigkeit = param[3].asFloat();
} // WriteV2end

// Attach WidgetTermnial to Virtual Pin V7
WidgetTerminal terminal(V7);

BLYNK_WRITE(V7)
{
  if (nummermessung == 0)
  {
    tiefeSonde = param.asFloat();
    terminal.flush();
  } // iferstemessungend

  if (nummermessung == 1)
  {
    eingabe = param.asFloat();
    terminal.flush();
  } // iffolgemessungend
}

// Attach WidgetLCD to Virtual Pin V8
WidgetLCD lcd1(V8);

// Attach WidgetLCD to Virtual Pin V9
WidgetLCD lcd2(V9);

// Attach WidgetLCD to Virtual Pin V10
WidgetLCD lcd3(V10);

// Attach WidgetLCD to Virtual Pin V11
WidgetLCD lcd4(V11);

// Attach WidgetLCD to Virtual Pin V12
WidgetLCD lcd5(V12);

void setup()
{

  Serial.begin(57600); // Öffnen der seriellen Kommunikation mit dem Computer
  Serial1.begin(
      19200, SERIAL_8N1, SERIAL1_RXPIN,
      SERIAL1_TXPIN); // Öffnen der seriellen Kommunikation mit Laser 1
  Serial2.begin(
      19200, SERIAL_8N1, SERIAL2_RXPIN,
      SERIAL2_TXPIN);       // Öffnen der seriellen Kommunikation mit Laser 2
  pinMode(stepPin, OUTPUT); // Sets stepPin as Outputs
  pinMode(dirPin, OUTPUT);  // Sets dirPin as Outputs
  pinMode(enPin, OUTPUT);   // Sets enPin as Outputs

  Serial.print("Grad pro Schritt Motor: ");
  Serial.println(schritt); // Printet Anzahl Grad pro Schritt des Motors
  Serial.print("Anzahl Messungen: ");
  Serial.println(anzahl); // Printet Anzahl Messungen
  Serial.print("Anzahl Grad zwischen Messungen: ");
  Serial.println(s); // Printet Anzahl Grad zwischen den Messungen
  Serial.print("Anzahl Schritte: ");
  Serial.println(aschritte); // Printet Anzahl Schritte zwischen den Messungen
  Blynk.begin(auth, ssid,
              pass); // Beginnen der seriellen Kommunikation mit der Blynk App
  int entscheidungsvariable = 0;
  lcd1.clear();
  lcd2.clear();
  lcd3.clear();
  lcd4.clear();
  lcd5.clear();
  terminal.println("");
  terminal.println("");
  terminal.println("Choose number of measurement on #measurement");
  terminal.println("Position Tripod");
  terminal.println("If positioned press SP1");
  terminal.flush();
  Blynk.virtualWrite(V1, LOW);
  Blynk.virtualWrite(V2, LOW);
  Blynk.virtualWrite(V3, LOW);
  Blynk.virtualWrite(V4, LOW);
  Blynk.virtualWrite(V5, LOW);
  led1.off();

  delay(50);
  Serial2.write("O");

} // setupend

void drehung(int schritte, int richtung)
{
  if (richtung == 0)
  {
    digitalWrite(dirPin,
                 HIGH); // Enables the motor to move in a particular direction
    for (int x = 0; x < schritte;
         x++)
    { // For-Schleife für x kleiner Anzahl Schritte
      digitalWrite(
          stepPin,
          HIGH); // Senden der Pulse für einen Schritt an den Steppermotor
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      Serial.println(x);
      schlussschritte++;
    } // forschritteschleifeend
  }   // ifend
  if (richtung == 1)
  {
    digitalWrite(dirPin,
                 LOW); // Enables the motor to move in a particular direction
    for (int x = 0; x < schritte;
         x++)
    { // For-Schleife für x kleiner Anzahl Schritte
      digitalWrite(
          stepPin,
          HIGH); // Senden der Pulse für einen Schritt an den Steppermotor
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      Serial.println(x);
      schlussschritte--;
    } // forschritteschleifeend
  }   // ifend
} // drehungend

void printen(float messergebnisse[], int laenge,
             int start)
{ // Funktion nimmt einen String der Länge laenge und
  // printet die Einträge von start bis laenge
  for (int i = start; i < laenge; i++)
  {
    Serial.print(messergebnisse[i]);
    Serial.print(' ');
  } // forend
} // printend

void minimumfinden(
    float messergebnisse[],
    int laenge)
{ // Funktion sucht das minimum eines Strings mit Länge laenge
  float minimum = _min(messergebnisse[0], messergebnisse[1]);
  for (int i = 2; i < laenge; i++)
  {
    minimum = _min(minimum, messergebnisse[i]);
  }                        // forend
  Serial.println(minimum); // Printen des Minimums
  for (int x = 0; x < laenge;
       x++)
  { // Heraussuchen der Stelle des Minimums im Array
    if (minimum == messergebnisse[x])
    {
      Serial.print("Die kürzeste Strecke befindet sich an Position ");
      Serial.print(x); // Gibt die Stelle im Array des Minimums an
      ort = x;
      Serial.print(" des Arrays und beträgt: ");
      Serial.print(messergebnisse[x]);
    } // ifend
  }   // forend
} // minimumfindenend

void vergleichen(float messergebnisse[],
                 int laenge)
{ // Funktion vergleicht, ob die neue Messung
  // kürzer ist als die Alte
  if (zaehler <=
      laenge)
  { // Solange der Zähler kleiner ist, als die Länge des Arrays
    if (messergebnisse[zaehler + 1] <=
        messergebnisse[zaehler])
    { // Wird ausgeführt, wenn die neue Messung
      // kleiner gleich die Alte ist
      Serial.println("Neue Messung kürzer als vorherige");
    } // ifkürzerend
    if (messergebnisse[zaehler + 1] >
        messergebnisse[zaehler])
    { // Wird ausgeführt, wenn die neue Messung
      // länger als die Alte ist
      Serial.println("Neue Messung länger als vorherige");
      falsecount++; // Hier wird gezählt, wie oft dies der Fall ist
      if (falsecount == 1)
      { // Beim ersten Mal ausführen der Schlaufe, wird die
        // Drehrichtung des Motors geändert
        drehrichtung = 1;
      } // if=1end
    }   // iflängerend
  }     // ifzaehlerkleinerlaengeend
} // vergleichenend

float flaecheberechnen(
    float messungsarray[])
{ // alle diese Werte müssen angepasst werden, wenn
  // der Speicherort im Array sich ändert
  if (messungsarray[36] < messungsarray[37] &&
      messungsarray[38] < messungsarray[37])
  { // Tank hat eine Kreisfläche
    float durchmesser = messungsarray[34] + messungsarray[37];
    float flaeche = pi * (durchmesser / 2) * (durchmesser / 2);
    return flaeche;
  } // ifend
  else
  {
    float a = messungsarray[34] + messungsarray[37];
    float b = messungsarray[35] + messungsarray[39];
    float flaeche = a * b;
    return flaeche;
  } // elseend
} // flaechenberechnenend

float volumenberechnung(float messung,
                        float flaeche)
{ // Funktion berechnet das Volumen aus
  // einer Längenmessung und einer Fläche
  float volumen = messung * flaeche;
  return volumen;
}

void loop()
{ // run over and over

  Blynk.run();

  while (
      Serial1
          .available())
  {                              // Wartet bis Serial input von Laser 1 vorhanden ist
    char input = Serial1.read(); // Schreibt die vorhandene Information der
                                 // Variable input zu
    if (input !=
        '\n')
    {                      // Solange kein newline charakter vom Laser gesendet wird
      sensorRead += input; // der String sensorRead mit den inputs gefüllt
    }
    else
    {
      break;
    } // elseend
  }   // whileend

  while (
      Serial2
          .available())
  {                               // Wartet bis Serial input von Laser 1 vorhanden ist
    char inputn = Serial2.read(); // Schreibt die vorhandene Information der
                                  // Variable input zu
    if (inputn !=
        '\n')
    {                        // Solange kein newline charakter vom Laser gesendet wird
      sensorReadn += inputn; // der String sensorRead mit den inputs gefüllt
    }
    else
    {
      break;
    } // elseend
  }   // whileend

  char seperator1 = ' '; // separator1 entspricht dem Leehrzeichen
  char seperator2 = 'm'; // separator2 entspricht dem m nach der Messung
  int index1 = sensorRead.indexOf(
      seperator1); // index1 gibt die Stelle des separator1 im String an
  int index2 = sensorRead.indexOf(
      seperator2); // index2 gibt die Stelle des separator2 im String an
  String firstString = sensorRead.substring(
      index1 + 1, index2); // generiert einen neuen String firstString beginnend
                           // von index1 + 1 bis index2
  char buf[20] = "";       // generiert einen leeren char array von Länge 20
  firstString.toCharArray(
      buf, 20); // der String firstString wird zu einem Charstring transformiert
                // und in buf geschrieben
  float distance =
      atof(buf); // der char string buf wird in einen float umgewandelt und in
                 // der Variable distance gespeichert
  if (distance != 0)
  { // Wenn der Wert von distance nicht gleich 0 ist,
    Serial.println(sensorRead);
    messungen[y] =
        distance; // wird sie an der Stelle y in den array messungen geschrieben
    Serial.println(messungen[y]);
    y++;
  }
  sensorRead = ""; // Der String sensorRead wird wieder auf Null gesetzt

  int index1n = sensorReadn.indexOf(
      seperator1); // index1n gibt die Stelle des separator1 im String an
  int index2n = sensorReadn.indexOf(
      seperator2); // index2n gibt die Stelle des separator2 im String an
  String firstStringn = sensorReadn.substring(
      index1n + 1, index2n); // generiert einen neuen String firstString
                             // beginnend von index1n + 1 bis index2n
  char bufn[20] = "";        // generiert einen leeren char array von Länge 20
  firstStringn.toCharArray(
      bufn, 20); // der String firstStringn wird zu einem Charstring
                 // transformiert und in buf geschrieben
  float distancen =
      atof(bufn); // der char string bufn wird in einen float umgewandelt und in
                  // der Variable distance gespeichert
  if (distancen != 0)
  {                            // Wenn der Wert von distancen nicht gleich 0 ist,
    messungenn[z] = distancen; // wird sie an der Stelle z in den array
                               // messungenn geschrieben
    Serial.println(messungenn[z]);
    z++;
  }

  sensorReadn = ""; // Der String sensorReadn wird wieder auf Null gesetzt

  switch (entscheidungsvariable)
  {
  case 1:
    // 1. Schritt Programm, Laser 1 aus, Laser 2 ein, sobald SP1 bestätigt wird,
    // macht Laser 2 eine Messung -> gespeichert in Array messungenn[0]
    if (nummermessung == 0)
    {
      z = 0;
      printnr = 1;
      counter = 0;
      //      Serial.println("Laser 1 aus");
      //      delay(1000);
      //      Serial.println("Laser 2 ein");
      //      Serial.println("Messung Laser 2, Laser 2 befindet sich zu oberst
      //      beim Tripod -> Speichern Wert in Array messungenn an Stelle 1.");
      Serial2.write("D");
      delay(1000);
      entscheidungsvariable = 0;
    } // iferstemessungend

    if (nummermessung == 1)
    {
      printnr = 5;
      terminal.println("Insert distance from last measurement");
      terminal.print("D: ");
      terminal.flush();
      entscheidungsvariable = 0;
    } // iffolgemessung
    break;
  case 2:
    // 2. Schritt Programm:Laser 1 geht an, um die Positionierung zu
    // vereinfachen, Laser 2 überprüft Abstand zum Sludge, warnt im Notfall
    // warten bis SP2 bestätigt
    Serial.println("Laser 2 überprüft während dem runterfahren, dass der "
                   "Sludge nicht zu nahe kommt");
    z = 1;
    printnr = 2;
    counter = 0;
    Serial1.write("O");
    Serial2.write("D");
    delay(1000);
    //      Serial.println("falls doch, sendet eine Warunung");
    //      delay(1000);
    //      Serial.println("Schritt 2 fertig");
    entscheidungsvariable = 0;
    break;
  case 3:
    // 3. Schritt Programm: Laser 1 fängt an Messungen zu machen, damit man die
    // Tankgeometrie sieht und Laser 2 weiter misst, um die zurückgelegte Distanz
    // herauszugeben warten bis SP3 bestätigt
    //      Serial.println("Laser 1 geht an, um Positionierung zu vereinfachen,
    //      macht keine Messungen, nur Laserpunkt zur Positionierung");
    //      delay(1000);
    //      Serial.println("Laser 2 überprüft immer noch Abstand zum Sludge");
    //      delay(1000);
    //      Serial.println("Bitte Startposition bestätigen");
    //      delay(1000);
    //      Serial.println("Sobald die Startposition bestätigt wurde, Laser 2
    //      zweite Messung, welche in Array messungenn an Stelle 2 gespeichert
    //      wird"); delay(1000); Serial.println("Schritt 3 fertig");
    y = 0;
    z = 2;
    printnr = 3;
    counter = 0;
    Serial2.write("D");
    if (nummermessung == 0)
    {
      Serial1.write("D");
      delay(1000);
    } // iferstemessungend
    if (nummermessung == 1)
    {
      Serial1.write("C");
    } // iffolgemessungend
    entscheidungsvariable = 0;
    break;
  case 4:
    // Schritt 4: Messung Sonde
    Serial.println("Eingabe Messwert Sonde");
    //      delay(1000);
    terminal.println("Insert measurement of stick");
    terminal.print("Measurement stick: ");
    terminal.flush();
    printnr = 4;
    entscheidungsvariable = 0;
    break;
  case 5:
    // Schritt 1 wenn folgemessung
    if (eingabezaehler == 1)
    {
      printnr = 5;
      eingabe = 0;
      terminal.println("Insert Squarearea of tank");
      terminal.print("TS: ");
      terminal.flush();
      entscheidungsvariable = 0;
    } // ifeingabe=1end
    if (eingabezaehler == 2)
    {
      printnr = 5;
      eingabe = 0;
      terminal.println("Insert Tankvolume");
      terminal.print("TV: ");
      terminal.flush();
      entscheidungsvariable = 0;
    } // ifeingabe=2end
    if (eingabezaehler == 3)
    {
      printnr = 5;
      eingabe = 0;
      terminal.println("Insert Sludgevolume measured last time");
      terminal.print("Old SV: ");
      terminal.flush();
      entscheidungsvariable = 0;
    } // ifeingabe=3end
    if (eingabezaehler == 4)
    {
      // ganz am Schluss von den Messwerteingaben soll das kommen
      printnr = 1;
      counter = 0;
      eingabe = 0;
      entscheidungsvariable = 0;
    } // ifeingabe=4end
    break;
  default:
    // if nothing else matches, do the default
    // default is optional
    break;
  } // switchend

  if (resetstatus == 1)
  {
    terminal.println("Wait till Device has restarted.");
    ESP.restart();
    resetstatus = 0;
  } // ifresetknopfend

  if (Startpositionbestaetigung1 == 1)
  {
    entscheidungsvariable = 1;
    Startpositionbestaetigung1 = 0;
  } // ifknopfgedrücktend

  if (printnr == 1 && counter == 10)
  {
    lcd1.print(0, 1, "L2:");
    lcd1.print(4, 1, messungenn[0]);
    lcd1.print(10, 1, "m");
    terminal.println("Move device to SP2");
    terminal.println("If positioned, press SP2");
    terminal.flush();
    printnr = 0;
    entscheidungsvariable = 2;
  } // printnr1counter10end

  if (printnr == 2 && counter == 10)
  {
    lcd1.print(0, 1, "L2:");
    lcd1.print(4, 1, messungenn[1]);
    lcd1.print(10, 1, "m");
    if (messungenn[1] <= 0.2 && messungenn[1] > 0.1)
    {
      Blynk.setProperty(V0, "color", "#ED9D00");
      led1.on();
    } // ifmessungenzunahe0.2end
    if (messungenn[1] <= 0.1)
    {
      Blynk.setProperty(V0, "color", "#D3435C");
      led1.on();
    } // ifmessungenzunahe0.1end
    if (messungenn[1] > 0.2)
    {
      led1.off();
    } // ifmessungennichtzunahe
    if (Startpositionbestaetigung2 == 1)
    {
      terminal.println("Move to desired SP3");
      terminal.println("If desired SP3 reached, press SP3");
      terminal.flush();
      entscheidungsvariable = 3;
      printnr = 0;
    } // ifknopfgedrücktend
    if (Startpositionbestaetigung2 == 0)
    {
      entscheidungsvariable = 2;
    } // ifknopfnichtgedrücktend
  }   // printnr2counter10end

  if (printnr == 3 && counter == 10)
  {
    if (nummermessung == 0)
    {
      lcd1.clear();
      lcd1.print(0, 0, "L1:");
      lcd1.print(3, 0, messungen[0]);
      lcd1.print(9, 0, "m");
    } // iferstemessungend
    if (nummermessung == 1)
    {
      //      lcd1.clear();
      lcd1.print(0, 0, "Dtg:");
      lcd1.print(4, 0, differenzalt);
      lcd1.print(10, 0, "m");
    } // iffolgemessungend
    differenz = messungenn[1] - messungenn[2];
    lcd1.print(0, 1, "D:");
    lcd1.print(2, 1, differenz);
    lcd1.print(8, 1, "m");
    if (messungenn[2] <= 0.2 && messungenn[2] > 0.1)
    {
      Blynk.setProperty(V0, "color", "#ED9D00");
      led1.on();
    } // ifmessungenzunahe0.2end
    if (messungenn[2] <= 0.1)
    {
      Blynk.setProperty(V0, "color", "#D3435C");
      led1.on();
    } // ifmessungenzunahe0.1end
    if (messungenn[2] > 0.2)
    {
      led1.off();
    } // ifmessungennichtzunahe
    if (Startpositionbestaetigung3 == 1)
    {
      if (nummermessung == 0)
      {
        terminal.println("Wait till measurement finished");
        terminal.flush();
        printnr = 0;
        incomingByte = 'T';
        counter = 0;
        y = 0;
        entscheidungsvariable = 0;
      } // iferstemessungend
      if (nummermessung == 1)
      {
        terminal.println("Check Results in Tab 3. Results");
        terminal.flush();
        printnr = 6;
        entscheidungsvariable = 0;
      } // iferstemessungend
    }   // ifknopfgedrücktend
    if (Startpositionbestaetigung3 == 0)
    {
      entscheidungsvariable = 3;
    } // ifknopfnichtgedrücktend
  }   // printnr2counter10end

  if (printnr == 4 && tiefeSonde != 0)
  {
    terminal.print(tiefeSonde);
    terminal.println(" m");
    terminal.println("Check Results in Tab 3. Results");
    terminal.flush();
    tiefeTank = tiefeSonde - (messungenn[0] - messungenn[2]);
    tiefeSludge = tiefeTank - (messungenn[2]);
    volumenTank = volumenberechnung(tiefeTank, flaecheTank);
    volumenSludge = volumenberechnung(tiefeSludge, flaecheTank);
    fuellstand = (volumenSludge / volumenTank) * 100;
    accumulationrate = volumenSludge - volumenSludgealt;
    Serial.println("Ausgabe Distanz");
    Serial.println(differenz);
    lcd2.print(0, 0, "D:");
    lcd2.print(2, 0, differenz);
    lcd2.print(8, 0, "m");
    //    delay(1000);
    Serial.println("Ausgabe Tankfläche");
    Serial.println(flaecheTank);
    lcd2.print(0, 1, "TS:");
    lcd2.print(3, 1, flaecheTank);
    lcd2.print(9, 1, "m^2");
    //    delay(1000);
    Serial.println("Ausgabe Tanktiefe = Messwert Sonde - (Startwert Laser - "
                   "Startwert Messung)");
    Serial.println(tiefeTank);
    lcd3.print(0, 0, "TD:");
    lcd3.print(3, 0, tiefeTank);
    lcd3.print(9, 0, "m");
    //    delay(1000);
    Serial.println("Ausgabe Sludgetiefe = Tanktiefe - Startwert Messung");
    Serial.println(tiefeSludge);
    lcd4.print(0, 0, "SD:");
    lcd4.print(3, 0, tiefeSludge);
    lcd4.print(9, 0, "m");
    //    delay(1000);
    Serial.println("Ausgabe Tankvolumen = Tanktiefe * Flaeche von Schritt 5");
    Serial.println(volumenTank);
    lcd3.print(0, 1, "TV:");
    lcd3.print(3, 1, volumenTank);
    lcd3.print(9, 1, "m^3");
    //    delay(1000);
    Serial.println("Ausgabe Sludgevolumen = Flaeche von Schritt 5 * (Tanktiefe "
                   "- Startwert Messung)");
    Serial.println(volumenSludge);
    lcd4.print(0, 1, "SV:");
    lcd4.print(3, 1, volumenSludge);
    lcd4.print(9, 1, "m^3");
    //    delay(1000);
    Serial.println(
        "Ausgabe Füllstand in Prozent = Sludgevolumen/Tankvolumen * 100");
    Serial.println(fuellstand);
    lcd5.print(0, 0, "FS:");
    lcd5.print(3, 0, fuellstand);
    lcd5.print(9, 0, "%");
    //    delay(1000);
    Serial.println("Schritt 5 fertig");
    printnr = 0;
    entscheidungsvariable = 0;
  } // printnr4end

  if (printnr == 5 && eingabe != 0)
  {
    //    terminal.println(eingabe);
    //    terminal.flush();
    if (eingabezaehler == 0)
    {
      differenzalt = eingabe;
    } // ifeingabe=0end
    if (eingabezaehler == 1)
    {
      flaecheTank = eingabe;
    } // ifeingabe=1end
    if (eingabezaehler == 2)
    {
      volumenTank = eingabe;
    } // ifeingabe=2end
    if (eingabezaehler == 3)
    {
      volumenSludgealt = eingabe;
    } // ifeingabe=3end
    eingabezaehler++;
    entscheidungsvariable = 5;
  } // printnr5end

  if (printnr == 6)
  {
    tiefeTank = volumenTank / flaecheTank;
    tiefeSludge = tiefeTank - (messungenn[2]);
    volumenSludge = volumenberechnung(tiefeSludge, flaecheTank);
    fuellstand = (volumenSludge / volumenTank) * 100;
    accumulationrate = volumenSludge - volumenSludgealt;
    Serial.println("Ausgabe Distanz");
    Serial.println(differenz);
    lcd2.print(0, 0, "D:");
    lcd2.print(2, 0, differenz);
    lcd2.print(8, 0, "m");
    //    delay(1000);
    Serial.println("Ausgabe Tankfläche");
    Serial.println(flaecheTank);
    lcd2.print(0, 1, "TS:");
    lcd2.print(3, 1, flaecheTank);
    lcd2.print(9, 1, "m^2");
    //    delay(1000);
    Serial.println("Ausgabe Tanktiefe = Messwert Sonde - (Startwert Laser - "
                   "Startwert Messung)");
    Serial.println(tiefeTank);
    lcd3.print(0, 0, "TD:");
    lcd3.print(3, 0, tiefeTank);
    lcd3.print(9, 0, "m");
    //    delay(1000);
    Serial.println("Ausgabe Sludgetiefe = Tanktiefe - Startwert Messung");
    Serial.println(tiefeSludge);
    lcd4.print(0, 0, "SD:");
    lcd4.print(3, 0, tiefeSludge);
    lcd4.print(9, 0, "m");
    //    delay(1000);
    Serial.println("Ausgabe Tankvolumen = Tanktiefe * Flaeche von Schritt 5");
    Serial.println(volumenTank);
    lcd3.print(0, 1, "TV:");
    lcd3.print(3, 1, volumenTank);
    lcd3.print(9, 1, "m^3");
    //    delay(1000);
    Serial.println("Ausgabe Sludgevolumen = Flaeche von Schritt 5 * (Tanktiefe "
                   "- Startwert Messung)");
    Serial.println(volumenSludge);
    lcd4.print(0, 1, "SV:");
    lcd4.print(3, 1, volumenSludge);
    lcd4.print(9, 1, "m^3");
    //    delay(1000);
    Serial.println(
        "Ausgabe Füllstand in Prozent = Sludgevolumen/Tankvolumen * 100");
    Serial.println(fuellstand);
    lcd5.print(0, 0, "FS:");
    lcd5.print(3, 0, fuellstand);
    lcd5.print(9, 0, "%");
    //    delay(1000);
    Serial.println(
        "Accumulation Rate = Sludgevolumenneu - Sludgevolumenalt"); // Diese nur
                                                                    // Printen,
                                                                    // wenn
                                                                    // Messung N
    Serial.println(accumulationrate);
    lcd5.print(0, 1, "AR:");
    lcd5.print(3, 1, accumulationrate);
    lcd5.print(9, 1, "m^3");
    Serial.println("Schritt 5 fertig");
    printnr = 0;
    entscheidungsvariable = 0;
  } // printnr6end

  if (incomingByte == 'T' &&
      counter ==
          0)
  {                     // In diesem Abschnitt werden die 10 ersten Messungen gemacht
    Serial1.write("D"); // Senden Messbefehl an Laser 1
    delay(1000);
    for (int i = 0; i < 9; i++)
    {
      drehung(aschritte, 0); // Machen der 9 Drehungen
      Serial1.write("D");    // Senden Messbefehl an Laser 1
      delay(1000);
    } // forend
  }   // ifendR

  if (incomingByte == 'T' &&
      counter == 20)
  { // In diesem Teil wird zur kürzesten Stelle der ersten 10
    // Messungen gedreht
    printen(messungen, 10, 0);
    minimumfinden(messungen, 10);
    Serial.print(ort);
    if (ort <= 9)
    { // Wird nur ausgeführt, wenn sich der Ort innerhalb der 10
      // Messungen befindet
      if (ort <= 4)
      { // Wenn der Ort in den ersten 5 Einträgen des Arrays ist,
        // dreht der Motor in die gleiche Richtung weiter
        for (int i = 0; i <= ort; i++)
        {
          drehung(aschritte, 0);
        } // forortschleife
      }   // ifort1end

      if (ort > 4)
      { // Wenn der Ort in den letzten 5 Einträgen des Arrays ist,
        // dreht der Motor in die andere Richtung zurück
        int ortlinks = 9 - ort;
        for (int j = 0; j < ortlinks; j++)
        {
          drehung(aschritte, 1);
        } // forortschleife
      }   // ifort2end
    }     // ifendD
  }       // ifAend

  if (incomingByte == 'T' && counter == 21)
  {
    delay(500);
    drehung(2, 0);
    delay(500);
    Serial1.write("D"); // Senden Messbefehl an Laser 1
    delay(1000);
    drehung(2, 0);
    Serial1.write("D"); // Senden Messbefehl an Laser 1
    delay(1000);
  } // ifWend

  if (incomingByte == 'T' && counter == 24 ||
      incomingByte == 'T' && counter == 28 ||
      incomingByte == 'T' && counter == 32 ||
      incomingByte == 'T' && counter == 36 ||
      incomingByte == 'T' && counter == 40 ||
      incomingByte == 'T' && counter == 44 ||
      incomingByte == 'T' && counter == 48 ||
      incomingByte == 'T' && counter == 52 ||
      incomingByte == 'T' && counter == 56 ||
      incomingByte == 'T' && counter == 60 ||
      incomingByte == 'T' && counter == 64 ||
      incomingByte == 'T' && counter == 68 ||
      incomingByte == 'T' && counter == 72 ||
      incomingByte == 'T' && counter == 76 ||
      incomingByte == 'T' && counter == 80 ||
      incomingByte == 'T' && counter == 84 ||
      incomingByte == 'T' && counter == 88 ||
      incomingByte == 'T' && counter == 92 ||
      incomingByte == 'T' && counter == 96 ||
      incomingByte == 'T' && counter == 100 ||
      incomingByte == 'T' && counter == 104)
  {
    vergleichen(messungen, 33);
    zaehler++;
    //}//ifcounternd
  } // ifVend

  if (incomingByte == 'T' && counter == 25 ||
      incomingByte == 'T' && counter == 29 ||
      incomingByte == 'T' && counter == 33 ||
      incomingByte == 'T' && counter == 37 ||
      incomingByte == 'T' && counter == 41 ||
      incomingByte == 'T' && counter == 45 ||
      incomingByte == 'T' && counter == 49 ||
      incomingByte == 'T' && counter == 53 ||
      incomingByte == 'T' && counter == 57 ||
      incomingByte == 'T' && counter == 61 ||
      incomingByte == 'T' && counter == 65 ||
      incomingByte == 'T' && counter == 69 ||
      incomingByte == 'T' && counter == 73 ||
      incomingByte == 'T' && counter == 77 ||
      incomingByte == 'T' && counter == 81 ||
      incomingByte == 'T' && counter == 85 ||
      incomingByte == 'T' && counter == 89 ||
      incomingByte == 'T' && counter == 93 ||
      incomingByte == 'T' && counter == 97 ||
      incomingByte == 'T' && counter == 101 ||
      incomingByte == 'T' && counter == 105)
  {
    if (falsecount < 3)
    {
      Serial.print("falsecount = ");
      Serial.println(falsecount);
      if (zaehler <= 33)
      { // Beschränkt, dass man nur bis zu der richtigen
        // Stelle im Array liest
        if (falsecount <
            2)
        { // Sorgt dafür, dass solange die neue Messung kürzer ist, in
          // die gleiche Richtung gedreht wird
          drehung(2, drehrichtung);
          Serial1.write("D"); // Senden Messbefehl an Laser 1
          delay(1000);
        } // falsecountkleinerend
        if (falsecount == 2)
        { // Sobald die neue Messung zum 2ten Mal länger
          // ist, wird ein Stück zurückgedreht
          drehung(4, 0);
          falsecount = 3; // Der Falschzähler wird auf 3 gesetzt, damit dieser
                          // Schritt sicher nicht mehr ausgeführt wird
          counter = 110;  // Danach wird der Programmzähler auf 110 gesetzt,
                          // damit der Rest des Programms ausgeführt werden kann,
        }                 // falsecount2end                                                 //
                          // auch wenn das Minimum vor Erreichen der Endposition des Arrays
                          // gefunden worden ist
      }                   // zahelerend
    }                     // falsecountend
    //}//counterend
  } // ifXend

  if (incomingByte == 'T' &&
      counter == 112)
  {         // In diesem Teil werden die Messungen im 90 Grad
            // Schritt gemacht
    y = 34; // Bestimmt den Startort der Messungen für die letzten Messungen
    //    Serial2.write("D"); //Senden Messbefehl an Laser 2
    if (ort <= 4)
    {
      Serial1.write("D"); // Messung an kürzester Stelle
      delay(1000);
      drehung(50, 1);     // Drehung um 90 Grad
      Serial1.write("D"); // Messung an 90 Grad Stelle
      delay(1000);
      drehung(35, 1);     // Drehung um 63 Grad
      Serial1.write("D"); // 1 Messung zum Erkennen der Geometrie
      delay(1000);
      drehung(15, 1);     // Drehung zu 180 Grad
      Serial1.write("D"); // Messung an 180 Grad Stelle
      delay(1000);
      drehung(15, 1);     // Drehung um 27 Grad
      Serial1.write("D"); // 2 Messung zum Erkennen der Geometrie
      delay(1000);
      drehung(35, 1);     // Drehung zu 270 Grad
      Serial1.write("D"); // Messung an 270 Grad Stelle
      delay(1000);
    } // ifort1end

    if (ort > 4)
    {
      Serial1.write("D"); // Messung an kürzester Stelle
      delay(1000);
      drehung(50, 0);     // Drehung um 90 Grad
      Serial1.write("D"); // Messung an 90 Grad Stelle
      delay(1000);
      drehung(35, 0);     // Drehung um 63 Grad
      Serial1.write("D"); // 1 Messung zum Erkennen der Geometrie
      delay(1000);
      drehung(15, 0);     // Drehung zu 180 Grad
      Serial1.write("D"); // Messung an 180 Grad Stelle
      delay(1000);
      drehung(15, 0);     // Drehung um 27 Grad
      Serial1.write("D"); // 2 Messung zum Erkennen der Geometrie
      delay(1000);
      drehung(35, 0);     // Drehung zu 270 Grad
      Serial1.write("D"); // Messung an 270 Grad Stelle
      delay(1000);
    } // ifort2end
  }   // ifEend

  if (incomingByte == 'T' &&
      counter == 134)
  { // Berechnet die Fläche und das Volumen
    // printen(messungen, 40, 0);
    Serial.println(schlussschritte);
    printen(messungenn, 3, 0);
    drehung(schlussschritte, 1);
    flaecheTank = flaecheberechnen(messungen);
    Serial.println(' ');
    Serial.println("Fläche: ");
    Serial.println(flaecheTank);
    terminal.println("Measurement finished");
    terminal.flush();
    incomingByte = 0; // incomingByte wieder auf 0
    entscheidungsvariable = 4;
  } // ifendB

  counter++;
} // loopend
