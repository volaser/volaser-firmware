/* Dieses Programm stellt die verschiedenen Modis für das Schlussprogramm dar*/
//funktioniert noch nicht

#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

int ort = 100;
int zaehler = 10;
const float pi = 3.141;
static float messungen[40] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40};         // Generieren des Arrays um die Messungen einzutragen
static float messungenn[3] = {1, 2, 3};
int entscheidungsvariable = 0;


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "a619eecff4f54be7b975ff76b23f81b4";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Ammandips iPhone Neu";
char pass[] = "guggi113";

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

void setup() {

  Serial.begin(57600);                                                // Öffnen der seriellen Kommunikation mit dem Computer

  Blynk.begin(auth, ssid, pass);
  int entscheidungsvariable = 0;

}

void loop() { // run over and over

  Blynk.run();
  switch (entscheidungsvariable) {
    case 1:
      //programm erster schritt, wo der laser 2 eine messung macht und laser 1 noch aus ist
      Serial.println("GPS Location vom Handy");
      delay(1000);
      Serial.println("Laser 1 aus");
      delay(1000);
      Serial.println("Laser 2 ein");
      delay(1000);
      Serial.println("Messung Laser 2, Laser 2 befindet sich zu oberst beim Tripod -> Speichern Wert in Array messungenn an Stelle 1.");
      delay(1000);
      Serial.println("Schritt 1 fertig");
      entscheidungsvariable = 0;
      break;
    case 2:
      //programm zweiter schritt, wenn man das Gerät runterlässt und Laser 2 überprüft, dass Sludge nicht zu nahe
      Serial.println("Laser 2 überprüft während dem runterfahren, dass der Sludge nicht zu nahe kommt");
      delay(1000);
      Serial.println("falls doch, sendet eine Warunung");
      delay(1000);
      Serial.println("Schritt 2 fertig");
      entscheidungsvariable = 0;
      break;
    case 3:
      //programm dritter schritt, wenn der laser 1 angeht um zu positionieren und laser 2 eine messung macht, sobald man Position bestätigt hat
      Serial.println("Laser 1 geht an, um Positionierung zu vereinfachen, macht keine Messungen, nur Laserpunkt zur Positionierung");
      delay(1000);
      Serial.println("Laser 2 überprüft immer noch Abstand zum Sludge");
      delay(1000);
      Serial.println("Bitte Startposition bestätigen");
      delay(1000);
      Serial.println("Sobald die Startposition bestätigt wurde, Laser 2 zweite Messung, welche in Array messungenn an Stelle 2 gespeichert wird");
      delay(1000);
      Serial.println("Schritt 3 fertig");
      entscheidungsvariable = 0;
      break;
    case 4:
      //programm vierter schritt, wo die position für die messung festgelegt wird
      Serial.println("Zur Messposition fahren");
      delay(1000);
      Serial.println("Laser 2 überprüft Abstand zum Sludge");
      delay(1000);
      Serial.println("Laser 1 geht an, um Abstand zu Wand zu zeigen");
      delay(1000);
      Serial.println("Distanz zwischen 2 er Messung und jetziger Messung wird angezeigt, um zu sehen, wie weit man seit dort gefahren ist");
      delay(1000);
      Serial.println("Warten auf Bestätigung Startposition");
      delay(1000);
      Serial.println("Sobald Startposition bestätigt ist, Laser 2 macht Messung, wird in Array Stelle 3 gespeichert und Distanz zwischen 2 und 3 in einer Variable gespeichert");
      delay(1000);
      Serial.println("Laser 1 und Laser 2 hören mit Messungen auf");
      delay(1000);
      Serial.println("Schritt 4 fertig");
      entscheidungsvariable = 0;
      break;
    case 5:
      //programm fünfter Schritt, Messung startet
      Serial.println("Start Messung");
      delay(1000);
      Serial.println("Tankfläche wird ausgegeben");
      delay(1000);
      Serial.println("Schreiben Messung fertig, fortfahren mit Sonde");
      delay(1000);
      Serial.println("Fragen für Messwert von Sonde einfügen");
      delay(1000);
      Serial.println("Schritt 5 fertig");
      entscheidungsvariable = 0;
      break;
    case 6:
      //programm sechster Schritt, Messung Sonde
      Serial.println("Eingabe Messwert Sonde");
      delay(1000);
      Serial.println("Ausgabe Tanktiefe = Messwert Sonde - (Startwert Laser - Startwert Messung)");
      delay(1000);
      Serial.println("Ausgabe Tankvolumen = Tanktiefe * Flaeche von Schritt 5");
      delay(1000);
      Serial.println("Ausgabe Sludgevolumen = Flaeche von Schritt 5 * (Tanktiefe - Startwert Messung)");
      delay(1000);
      Serial.println("Ausgabe Füllstand in Prozent = Sludgevolumen/Tankvolumen * 100");
      delay(1000);
      Serial.println("Schritt 6 fertig");
      entscheidungsvariable = 0;
      break;

    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }//switchend

}//loopend

