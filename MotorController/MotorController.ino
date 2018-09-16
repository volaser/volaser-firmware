/* (26.05.18) Dieses Programm steuert den MKR1000 in unserem Projekt, welcher den Motor hoch und runter bewegt */
//hoch runter: (02.06.18) Die Befehle für den SparkFun Treiber hinzugefügt: funktioniert nocht nicht
//Wert von Sludgeabstand übergeben sodass Runter blockiert wird, sobald sludge zu nahe: funktioniert noch nicht

#define BLYNK_PRINT SerialUSB


#include <SPI.h>
#include <WiFi101.h>
#include <BlynkSimpleWiFiShield101.h>
#include <SparkFun_TB6612.h>

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// the default pins listed are the ones used on the Redbot (ROB-12097) with
// the exception of STBY which the Redbot controls with a physical switch
#define AIN1 4
#define BIN1 7
#define AIN2 3
#define BIN2 8
#define PWMA 2
#define PWMB 6
#define STBY 5

int pinValueup = 0;
int pinValuedown = 0;

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "f0f810652e194433a96d4e4a81e96ab0";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Ammandips iPhone Neu"; //"qkn-11345"
char pass[] = "guggi113"; //"8z2m-lygk-zhfg-i00y"

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V0
BLYNK_WRITE(V0)
{
  pinValueup = param.asInt(); // assigning incoming value from pin V0 to a variable

  // process received value
}//BLYNK_WRITE(V0)end

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  pinValuedown = param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
}//BLYNK_WRITE(V1)end

void setup()
{
  Blynk.begin(auth, ssid, pass);
}//setupend

void loop()
{
  Blynk.run();
  if (pinValueup == 1) {
    motor1.drive(255,1);
  }//ifpinValueup=1end
  if (pinValuedown == 1) {
    motor1.drive(-255);
  }//ifpinValuedown=1end
  else {
    motor1.brake();
  }//elseend
}//loopend

