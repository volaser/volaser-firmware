/*(15.05.18) FSchlussprogramm3 based on FSchlussprogramm2 but attempts are made
 * here to respond to first/following measurements:*/

// select key for first and following measurements: installed on V1 but does not
// execute anything yet -> next program (14.05.18) run-off for nth measurement
// other: works (15.05.18) change terminal input depending on measurement: works
// (16.05.18) accumulation rate may only be output for the following
// measurements: works (16.05.18) check if 10th measurement = 10.00, if yes,
// repeat area measurement, with other angle: not working yet
//!!!!! Attention, see what exactly is going on with deep tank, somehow
//! different values between first and follow-up measurements!!!!!!: checked
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
const int stepPin = 14; // defines the stepPin of the stepper - motor driver
const int dirPin = 26;  // defines the dirPin of the stepper - motor driver
const int enPin = 12;   // defines the enPin of the stepper - motor driver

float deg_step = 1.8;    // deg/step of motor
float rotation_div = 10; // Anzal measurements that I want
float s =
    360 / rotation_div; // Degrees to be driven between measurements
                        // Number of steps to be taken between measurements
float aschritte = s / deg_step;
int y = 0; // Variable to write the measurements of laser 1 in the array
           // measurements to the right place
int z = 0; // Variable to measure the measurements of Laser 2 in the array to
           // the to write the right job
int ort = 100;
int zaehler = 10;

// Generate the array of measurements to be registered
static float messungen[40] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                              20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
                              30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
                              40, 40, 40, 40, 40, 40, 40, 40, 40, 40};
static float messungenn[40] = {1, 2, 3};
String sensorRead; // Variable to check that all the data from the laser was
                   // transferred
String sensorReadn;
char incomingByte = 0; // Variable to trigger the individual steps

// To minimum search, as soon as False account reaches the value 2 has, the
// program is finished
int falsecount = 0;
int drehrichtung =
    0;                   // To determine the direction of rotation for the minimum search
int programmzaehler = 0; // To guide through the program
int state = 0;
int printnr = 0;                    // To define which step you want to print
int counter = 0;                    // To define that the printing is done only once
int Startpositionbestaetigung1 = 0; // To confirm the start position
int Startpositionbestaetigung2 = 0; // To confirm the second start position
int Startpositionbestaetigung3 = 0; // To trigger the start of the measurement

// is =0 for the first measurement and =1 for all subsequent measurements
int not_first_measurement = 0;

// Specifies how far the motor must be turned back at the end.
int schlussschritte = 0;
int resetstatus = 0; // If the value = 1, the ESP should be restarted.
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
float eingabe = 0; // Input into terminal for follow-up measurement

// Counter to instruct the inputs to the correct variables
int eingabezaehler = 0;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "a619eecff4f54be7b975ff76b23f81b4";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "qkn-11345";           //"Ammandips iPhone Neu";
char pass[] = "8z2m-lygk-zhfg-i00y"; //"guggi113";

// Attach WidgetLED to Virtual Pin V0
WidgetLED led1(V0);

// This function will be called every time Butten Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  // assigning incoming value from pin V1 to a variable
  not_first_measurement = param.asInt();
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
}

// Attach WidgetTermnial to Virtual Pin V7
WidgetTerminal terminal(V7);

BLYNK_WRITE(V7)
{
  if (not_first_measurement == 0)
  {
    tiefeSonde = param.asFloat();
    terminal.flush();
  }

  if (not_first_measurement == 1)
  {
    eingabe = param.asFloat();
    terminal.flush();
  }
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

  Serial.begin(57600); // Open serial communication with the computer

  // Opening serial communication with laser 1
  Serial1.begin(19200, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  // Opening serial communication with laser 2
  Serial2.begin(19200, SERIAL_8N1, SERIAL2_RXPIN, SERIAL2_TXPIN);

  // Set the motor controller pints to outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);

  Serial.print("Degrees per step Motor: ");
  Serial.println(deg_step);
  Serial.print("Number of measurements: ");
  Serial.println(rotation_div);
  Serial.print("Number of degrees between measurements: ");
  Serial.println(s);
  Serial.print("Number of steps: ");
  Serial.println(aschritte);
  // Start serial communication with the Blynk App
  Blynk.begin(auth, ssid, pass);
  int state = 0;
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
}

void rotation(int steps, int direction)
{
  if (direction == 0)
  {
    // Enables the motor to move in a particular direction
    digitalWrite(dirPin, HIGH);
    for (int x = 0; x < steps; x++)
    {
      // For loop for x small number of steps
      // Sending the pulses for one step to the stepper motor
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      Serial.println(x);
      schlussschritte++;
    }
  }
  if (direction == 1)
  {
    // Enables the motor to move in a particular direction
    digitalWrite(dirPin, LOW);
    for (int x = 0; x < steps; x++)
    {
      // for loop for x small number of steps
      // Sending the pulses for one step to the stepper motor
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      Serial.println(x);
      schlussschritte--;
    }
  }
}

void print_measurement(float measurement[], int length, int start)
{
  // Function takes a string of length and length
  // prints the entries from start to length
  for (int i = start; i < length; i++)
  {
    Serial.print(measurement[i]);
    Serial.print(' ');
  }
}

void find_minimum(float measurements[], int length)
{
  // Function searches for the minimum of a string with length length
  float minimum = _min(measurements[0], measurements[1]);
  for (int i = 2; i < length; i++)
  {
    minimum = _min(minimum, measurements[i]);
  }
  Serial.println(minimum);
  for (int x = 0; x < length;
       x++)
  { // Find the location of the minimum in the array
    if (minimum == measurements[x])
    {
      Serial.print("Die kürzeste Strecke befindet sich an Position ");
      Serial.print(x); // Specifies the position in the minimum array
      ort = x;
      Serial.print(" des Arrays und beträgt: ");
      Serial.print(measurements[x]);
    }
  }
}

void compare_measurements(float messergebnisse[], int laenge)
{
  // Function compares whether the new measurement
  // is shorter than the old one
  if (zaehler <= laenge)
  {
    // As long as the counter is smaller than the length of the array
    if (messergebnisse[zaehler + 1] <= messergebnisse[zaehler])
    {
      // Is executed when the new measurement is less than previous one
      Serial.println("New measurement shorter than previous one");
    }
    if (messergebnisse[zaehler + 1] > messergebnisse[zaehler])
    {
      // Is executed when the new measurement is longer
      Serial.println("New measurement longer than previous measurement");
      falsecount++; // This counts how often this is the case.
      if (falsecount == 1)
      {
        // The first time the loop is executed, the
        // Direction of motor rotation changed
        drehrichtung = 1;
      }
    }
  }
}

float calculate_area(float measurements[])
{
  // all these values must be adjusted if
  // the location in the array changes
  if (measurements[36] < measurements[37] &&
      measurements[38] < measurements[37])
  {
    // Tank has a circular area
    float radius = measurements[34] + measurements[37];
    float area = M_PI * (radius / 2) * (radius / 2);
    return area;
  }
  else
  {
    // Tank has a rectangular area
    float a = measurements[34] + measurements[37];
    float b = measurements[35] + measurements[39];
    float area = a * b;
    return area;
  }
}

float volume(float height, float area) { return height * area; }

void loop()
{
  Blynk.run();

  while (Serial1.available())
  {
    // Wait until serial input of laser 1 is available
    // Writes the existing information of the variable input to
    char input = Serial1.read();
    // As long as no newline character is sent by the laser
    if (input != '\n')
    {
      sensorRead += input; // the string sensorRead filled with the inputs
    }
    else
    {
      break;
    }
  }

  while (Serial2.available())
  {
    // Wait until serial input of laser 1 is available
    char inputn = Serial2.read();
    // Writes the existing information of the
    // Variable input to
    if (inputn != '\n')
    {
      // As long as no newline character is sent by the laser
      sensorReadn += inputn; // the String sensorRead filled with the inputs
    }
    else
    {
      break;
    }
  }

  char seperator1 = ' '; // separator1 corresponds to the honor sign
  char seperator2 = 'm'; // separator2 corresponds to m after measurement

  // index1 indicates the position of separator1 in the string
  int index1 = sensorRead.indexOf(seperator1);
  // index2 indicates the position of separator2 in the string
  int index2 = sensorRead.indexOf(seperator2);
  String firstString = sensorRead.substring(index1 + 1, index2); // generates a new string starting with firstString
                                                                 // from index1 + 1 to index2
  char buf[20] = "";                                             // generates an empty char array of length 20
  firstString.toCharArray(buf, 20);                              // the string firstString is transformed to a charstring
                                                                 // and written in buf
  float distance = atof(buf);                                    // the char string buf will be converted into a float and stored in
                                                                 // of the variable distance is stored
  if (distance != 0)
  { // If the value of distance is not equal to 0,
    Serial.println(sensorRead);
    messungen[y] = distance; // it is written at the position y in the array measurements
    Serial.println(messungen[y]);
    y++;
  }
  sensorRead = ""; // The String sensorRead is set to zero again.

  int index1n = sensorReadn.indexOf(seperator1);                     // index1n indicates the position of separator1 in the string
  int index2n = sensorReadn.indexOf(seperator2);                     // index2n indicates the position of separator2 in the string
  String firstStringn = sensorReadn.substring(index1n + 1, index2n); // generates a new string firstString
                                                                     // starting from index1n + 1 to index2n
  char bufn[20] = "";                                                // generates an empty char array of length 20
  firstStringn.toCharArray(bufn, 20);                                // the string firstStringn becomes a charstring
                                                                     // transformed and written in buf
  float distancen = atof(bufn);                                      // the char string bufn will be converted to a float and stored in
                                                                     // of the variable distance is stored
  if (distancen != 0)
  {                            // If the value of distances is not equal to 0,
    messungenn[z] = distancen; // it will be placed at the position z in the array
                               // measurements written
    Serial.println(messungenn[z]);
    z++;
  }

  sensorReadn = ""; // The string sensorReadn is set to zero again

  switch (state)
  {
  case 1:
    // Step 1 Program, Laser 1 off, Laser 2 on as soon as SP1 is confirmed,
    // Laser 2 makes a measurement -> stored in array measurements[0]
    if (not_first_measurement == 0)
    {
      z = 0;
      printnr = 1;
      counter = 0;
      Serial2.write("D");
      delay(1000);
      state = 0;
    }

    if (not_first_measurement == 1)
    {
      printnr = 5;
      terminal.println("Insert distance from last measurement");
      terminal.print("D: ");
      terminal.flush();
      state = 0;
    } // iffolgemessung
    break;
  case 2:
    // Step 2 Program:Laser 1 approaches to position the
    // simplify, Laser 2 checks distance to sludge, warns in case of emergency
    // wait until SP2 confirms
    Serial.println("Laser 2 checks while shutting down that the Sludge doesn't come too close");
    z = 1;
    printnr = 2;
    counter = 0;
    Serial1.write("O");
    Serial2.write("D");
    delay(1000);
    state = 0;
    break;
  case 3:
    // Step 3 Program: Laser 1 starts to make measurements to get the
    // Tank geometry looks and Laser 2 continues to measure to the distance travelled
    // Distance to release wait until SP3 confirms
    // Serial.println("Laser 1 approaches to simplify positioning,
    // makes no measurements, only laser point for positioning");
    // delay(1000);
    // Serial.println("Laser 2 still checks distance to sludge");
    // delay(1000);
    // Serial.println("Please confirm start position");
    // delay(1000);
    // Serial.println("As soon as the start position was confirmed, Laser 2
    // second measurement stored in array measurements at position 2
    // will"); delay(1000); Serial.println("Step 3 ready");
    y = 0;
    z = 2;
    printnr = 3;
    counter = 0;
    Serial2.write("D");
    if (not_first_measurement == 0)
    {
      Serial1.write("D");
      delay(1000);
    }
    if (not_first_measurement == 1)
    {
      Serial1.write("C");
    }
    state = 0;
    break;
  case 4:
    // Step 4: Measurement probe
    Serial.println("Input Probe measurement value");
    terminal.println("Insert measurement of stick");
    terminal.print("Measurement stick: ");
    terminal.flush();
    printnr = 4;
    state = 0;
    break;
  case 5:
    // Step 1 if follow-up measurement
    if (eingabezaehler == 1)
    {
      printnr = 5;
      eingabe = 0;
      terminal.println("Insert area of tank");
      terminal.print("TS: ");
      terminal.flush();
      state = 0;
    }
    if (eingabezaehler == 2)
    {
      printnr = 5;
      eingabe = 0;
      terminal.println("Insert Tank volume");
      terminal.print("TV: ");
      terminal.flush();
      state = 0;
    }
    if (eingabezaehler == 3)
    {
      printnr = 5;
      eingabe = 0;
      terminal.println("Insert Sludge volume measured last time");
      terminal.print("Old SV: ");
      terminal.flush();
      state = 0;
    }
    if (eingabezaehler == 4)
    {
      // at the very end of the measured value inputs should come that
      printnr = 1;
      counter = 0;
      eingabe = 0;
      state = 0;
    }
    break;
  default:
    // if nothing else matches, do the default
    // default is optional
    break;
  }

  if (resetstatus == 1)
  {
    terminal.println("Wait till Device has restarted.");
    ESP.restart();
    resetstatus = 0;
  }

  if (Startpositionbestaetigung1 == 1)
  {
    state = 1;
    Startpositionbestaetigung1 = 0;
  }

  if (printnr == 1 && counter == 10)
  {
    lcd1.print(0, 1, "L2:");
    lcd1.print(4, 1, messungenn[0]);
    lcd1.print(10, 1, "m");
    terminal.println("Move device to SP2");
    terminal.println("If positioned, press SP2");
    terminal.flush();
    printnr = 0;
    state = 2;
  }

  if (printnr == 2 && counter == 10)
  {
    lcd1.print(0, 1, "L2:");
    lcd1.print(4, 1, messungenn[1]);
    lcd1.print(10, 1, "m");
    if (messungenn[1] <= 0.2 && messungenn[1] > 0.1)
    {
      Blynk.setProperty(V0, "color", "#ED9D00");
      led1.on();
    }
    if (messungenn[1] <= 0.1)
    {
      Blynk.setProperty(V0, "color", "#D3435C");
      led1.on();
    }
    if (messungenn[1] > 0.2)
    {
      led1.off();
    }
    if (Startpositionbestaetigung2 == 1)
    {
      terminal.println("Move to desired SP3");
      terminal.println("If desired SP3 reached, press SP3");
      terminal.flush();
      state = 3;
      printnr = 0;
    }
    if (Startpositionbestaetigung2 == 0)
    {
      state = 2;
    }
  }

  if (printnr == 3 && counter == 10)
  {
    if (not_first_measurement == 0)
    {
      lcd1.clear();
      lcd1.print(0, 0, "L1:");
      lcd1.print(3, 0, messungen[0]);
      lcd1.print(9, 0, "m");
    }
    if (not_first_measurement == 1)
    {
      lcd1.print(0, 0, "Dtg:");
      lcd1.print(4, 0, differenzalt);
      lcd1.print(10, 0, "m");
    }
    differenz = messungenn[1] - messungenn[2];
    lcd1.print(0, 1, "D:");
    lcd1.print(2, 1, differenz);
    lcd1.print(8, 1, "m");
    if (messungenn[2] <= 0.2 && messungenn[2] > 0.1)
    {
      Blynk.setProperty(V0, "color", "#ED9D00");
      led1.on();
    }
    if (messungenn[2] <= 0.1)
    {
      Blynk.setProperty(V0, "color", "#D3435C");
      led1.on();
    }
    if (messungenn[2] > 0.2)
    {
      led1.off();
    }
    if (Startpositionbestaetigung3 == 1)
    {
      if (not_first_measurement == 0)
      {
        terminal.println("Wait till measurement finished");
        terminal.flush();
        printnr = 0;
        incomingByte = 'T';
        counter = 0;
        y = 0;
        state = 0;
      }
      if (not_first_measurement == 1)
      {
        terminal.println("Check Results in Tab 3. Results");
        terminal.flush();
        printnr = 6;
        state = 0;
      }
    }
    if (Startpositionbestaetigung3 == 0)
    {
      state = 3;
    }
  }

  if (printnr == 4 && tiefeSonde != 0)
  {
    terminal.print(tiefeSonde);
    terminal.println(" m");
    terminal.println("Check Results in Tab 3. Results");
    terminal.flush();
    tiefeTank = tiefeSonde - (messungenn[0] - messungenn[2]);
    tiefeSludge = tiefeTank - (messungenn[2]);
    volumenTank = volume(tiefeTank, flaecheTank);
    volumenSludge = volume(tiefeSludge, flaecheTank);
    fuellstand = (volumenSludge / volumenTank) * 100;
    accumulationrate = volumenSludge - volumenSludgealt;
    Serial.println("Output distance");
    Serial.println(differenz);
    lcd2.print(0, 0, "D:");
    lcd2.print(2, 0, differenz);
    lcd2.print(8, 0, "m");
    Serial.println("Output tank area");
    Serial.println(flaecheTank);
    lcd2.print(0, 1, "TS:");
    lcd2.print(3, 1, flaecheTank);
    lcd2.print(9, 1, "m^2");
    Serial.println("Output tank depth = measured value probe - (start value laser - Start value measurement)");
    Serial.println(tiefeTank);
    lcd3.print(0, 0, "TD:");
    lcd3.print(3, 0, tiefeTank);
    lcd3.print(9, 0, "m");
    Serial.println("Output Sludget Depth = Tank Depth - Start Value Measurement");
    Serial.println(tiefeSludge);
    lcd4.print(0, 0, "SD:");
    lcd4.print(3, 0, tiefeSludge);
    lcd4.print(9, 0, "m");
    Serial.println("Output tank volume = tank depth * area of step 5");
    Serial.println(volumenTank);
    lcd3.print(0, 1, "TV:");
    lcd3.print(3, 1, volumenTank);
    lcd3.print(9, 1, "m^3");
    Serial.println("Sludge volume output = area of step 5 * (tank depth - Measurement start value)");
    Serial.println(volumenSludge);
    lcd4.print(0, 1, "SV:");
    lcd4.print(3, 1, volumenSludge);
    lcd4.print(9, 1, "m^3");
    Serial.println("Output filling level in percent = Sludge volume/tank volume * 100");
    Serial.println(fuellstand);
    lcd5.print(0, 0, "FS:");
    lcd5.print(3, 0, fuellstand);
    lcd5.print(9, 0, "%");
    Serial.println("Step 5 finished");
    printnr = 0;
    state = 0;
  }

  if (printnr == 5 && eingabe != 0)
  {

    if (eingabezaehler == 0)
    {
      differenzalt = eingabe;
    }
    if (eingabezaehler == 1)
    {
      flaecheTank = eingabe;
    }
    if (eingabezaehler == 2)
    {
      volumenTank = eingabe;
    }
    if (eingabezaehler == 3)
    {
      volumenSludgealt = eingabe;
    }
    eingabezaehler++;
    state = 5;
  }
  if (printnr == 6)
  {
    tiefeTank = volumenTank / flaecheTank;
    tiefeSludge = tiefeTank - (messungenn[2]);
    volumenSludge = volume(tiefeSludge, flaecheTank);
    fuellstand = (volumenSludge / volumenTank) * 100;
    accumulationrate = volumenSludge - volumenSludgealt;
    Serial.println("Output distance");
    Serial.println(differenz);
    lcd2.print(0, 0, "D:");
    lcd2.print(2, 0, differenz);
    lcd2.print(8, 0, "m");
    Serial.println("Output tank area");
    Serial.println(flaecheTank);
    lcd2.print(0, 1, "TS:");
    lcd2.print(3, 1, flaecheTank);
    lcd2.print(9, 1, "m^2");
    Serial.println("Output tank depth = measured value probe - (start value laser - Start value measurement)");
    Serial.println(tiefeTank);
    lcd3.print(0, 0, "TD:");
    lcd3.print(3, 0, tiefeTank);
    lcd3.print(9, 0, "m");
    Serial.println("Output Sludget Depth = Tank Depth - Start Value Measurement");
    Serial.println(tiefeSludge);
    lcd4.print(0, 0, "SD:");
    lcd4.print(3, 0, tiefeSludge);
    lcd4.print(9, 0, "m");
    Serial.println("Output tank volume = tank depth * area of step 5");
    Serial.println(volumenTank);
    lcd3.print(0, 1, "TV:");
    lcd3.print(3, 1, volumenTank);
    lcd3.print(9, 1, "m^3");
    Serial.println("Sludge volume output = area of step 5 * (tank depth - Measurement start value)");
    Serial.println(volumenSludge);
    lcd4.print(0, 1, "SV:");
    lcd4.print(3, 1, volumenSludge);
    lcd4.print(9, 1, "m^3");
    Serial.println("Output level in percent = Sludge volume/tank volume * 100");
    Serial.println(fuellstand);
    lcd5.print(0, 0, "FS:");
    lcd5.print(3, 0, fuellstand);
    lcd5.print(9, 0, "%");
    // these only print if measurement N
    Serial.println("Accumulation Rate = New Sludge Volume - Old Sludge Volume");
    Serial.println(accumulationrate);
    lcd5.print(0, 1, "AR:");
    lcd5.print(3, 1, accumulationrate);
    lcd5.print(9, 1, "m^3");
    Serial.println("Schritt 5 fertig");
    printnr = 0;
    state = 0;
  }

  if (incomingByte == 'T' && counter == 0)
  {                     // In this section the first 10 measurements are made
    Serial1.write("D"); // Send measurement command to laser 1
    delay(1000);
    for (int i = 0; i < 9; i++)
    {
      rotation(aschritte, 0); // Making the 9 turns
      Serial1.write("D");     // Send measurement command to laser 1
      delay(1000);
    }
  }

  if (incomingByte == 'T' &&
      counter == 20)
  { // In this part, the shortest place of the first 10
    // Measurements rotated
    print_measurement(messungen, 10, 0);
    find_minimum(messungen, 10);
    Serial.print(ort);
    if (ort <= 9)
    { // Will only be executed if the location is within the 10
      // Measurements located
      if (ort <= 4)
      { // If the location is in the first 5 entries of the array,
        // the motor continues to rotate in the same direction
        for (int i = 0; i <= ort; i++)
        {
          rotation(aschritte, 0);
        }
      }

      if (ort > 4)
      { // If the location is in the last 5 entries of the array,
        // the motor turns back in the other direction
        int ortlinks = 9 - ort;
        for (int j = 0; j < ortlinks; j++)
        {
          rotation(aschritte, 1);
        }
      }
    }
  }

  if (incomingByte == 'T' && counter == 21)
  {
    delay(500);
    rotation(2, 0);
    delay(500);
    Serial1.write("D"); // Send measurement command to laser 1
    delay(1000);
    rotation(2, 0);
    Serial1.write("D"); // Send measurement command to laser 1
    delay(1000);
  }

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
    compare_measurements(messungen, 33);
    zaehler++;
  }
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
      { // Restricts that you can only get up to the right
        // place in array reads
        if (falsecount <
            2)
        { // Ensures that as long as the new measurement is shorter, in
          // is rotated in the same direction
          rotation(2, drehrichtung);
          Serial1.write("D"); // Send measurement command to laser 1
          delay(1000);
        }
        if (falsecount == 2)
        { // As soon as the new measurement is longer for the 2nd time
          // is, a piece is turned back
          rotation(4, 0);
          falsecount = 3; // The wrong counter is set to 3, so that this counter
                          // step will surely not be executed anymore
          counter = 110;  // The program counter is then set to 110,
                          // so that the rest of the program can be executed,
        }
        // even if the minimum is reached before reaching the end position of
        // the array has been found
      }
    }
  }

  if (incomingByte == 'T' && counter == 112)
  {
    // In this part the measurements in the 90 degrees step made
    y = 34; // Determines the start location of the measurements for the last
            // measurements
    //    Serial2.write("D"); //Send measurement command to Laser 2
    if (ort <= 4)
    {
      Serial1.write("D"); // Measurement at the shortest point
      delay(1000);
      rotation(50, 1);    // 90 degree rotation
      Serial1.write("D"); // Measurement at 90 degrees point
      delay(1000);
      rotation(35, 1);    // 63 degree rotation
      Serial1.write("D"); // 1 measurement for recognition of the geometry
      delay(1000);
      rotation(15, 1);    // 180 degree rotation
      Serial1.write("D"); // Measurement at 180 degrees point
      delay(1000);
      rotation(15, 1);    // 27 degree rotation
      Serial1.write("D"); // 2 Measurement for recognition of geometry
      delay(1000);
      rotation(35, 1);    // Rotation to 270 degrees
      Serial1.write("D"); // Measurement at 270 degrees point
      delay(1000);
    } // ifort1end

    if (ort > 4)
    {
      Serial1.write("D"); // Measurement at the shortest point
      delay(1000);
      rotation(50, 0);    // 90 degree rotation
      Serial1.write("D"); // Measurement at 90 degrees point
      delay(1000);
      rotation(35, 0);    // 63 degree rotation
      Serial1.write("D"); // 1 measurement for recognition of the geometry
      delay(1000);
      rotation(15, 0);    // 180 degree rotation
      Serial1.write("D"); // Measurement at 180 degrees point
      delay(1000);
      rotation(15, 0);    // 27 degree rotation
      Serial1.write("D"); // 2 Measurement for recognition of geometry
      delay(1000);
      rotation(35, 0);    // Rotation to 270 degrees
      Serial1.write("D"); // Measurement at 270 degrees point
      delay(1000);
    }
  }

  if (incomingByte == 'T' && counter == 134)
  {
    // Calculates the area and the volume
    // print_measurement(messungen, 40, 0);
    Serial.println(schlussschritte);
    print_measurement(messungenn, 3, 0);
    rotation(schlussschritte, 1);
    flaecheTank = calculate_area(messungen);
    Serial.println(' ');
    Serial.println("Fläche: ");
    Serial.println(flaecheTank);
    terminal.println("Measurement finished");
    terminal.flush();
    incomingByte = 0; // incomingByte back to 0
    state = 4;
  }

  counter++;
}
