// Bluetooth Libraries
#include <BLEDevice.h>

// Laser object library
#include "LaserM703A.h"
// Stepper motor library
#include "BasicStepperDriver.h"

// Bluetooth configuration
// Create unique service, receive and transmission IDs for BLE
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define RX_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define TX_UUID "a812aeed-78d0-474a-b9b1-20a8a1f95463"

BLECharacteristic *TX_Characteristic = NULL;

// setup communication with laser modules
#define SERIAL1_RXPIN 32 // defines the RXPIN of Laser 1
#define SERIAL1_TXPIN 25 // defines the TXPIN of Laser 1
#define SERIAL2_RXPIN 27 // defines the RXPIN of Laser 2
#define SERIAL2_TXPIN 13 // defines the TXPIN of Laser 2

LaserM703A laserH(&Serial1, SERIAL1_RXPIN, SERIAL1_TXPIN);
LaserM703A laserV(&Serial2, SERIAL2_RXPIN, SERIAL2_TXPIN);

// Setup stepper motor control
// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define MOTOR_RPM 120
#define MOTOR_MICROSTEPS 1

#define STEPPER_DIR 26
#define STEPPER_STEP 14
#define STEPPER_ENABLE 12
int angle = 0; // keep track of phase of the stepper motor rotation, since there is no encoder.

BasicStepperDriver stepper(MOTOR_STEPS, STEPPER_DIR, STEPPER_STEP, STEPPER_ENABLE);

// implement a simple callback to parse BLE communications
class ParseReceive : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string message = pCharacteristic->getValue();
    if (message.length() > 0)
    {
      char msg[10];
      // make a measurement of the vertical laser
      if (message[0] == 'V')
      {
        measurement m = laserV.measure();
        sprintf(msg, "%2.3f", m.range);
      }
      // make a measurement of the horizontal laser
      else if (message[0] == 'H')
      {
        measurement m = laserH.measure();
        sprintf(msg, "%2.3f", m.range);
      }
      // rotate the stepper motor to a given angle,
      // and then make a measurement of the horizontal laser
      else if (message[0] == 'R')
      {
        // These commands are the form R90, or R-30, to rotate to +90 degrees or -30 degrees
        int new_angle = String(message.c_str()).substring(1).toInt();
        // we constrain ourselves to stay within +/-180 degrees
        if (0 <= new_angle && new_angle <= 360)
        {
          stepper.rotate(new_angle - angle);
          angle = new_angle;
          Serial.println(angle);
          measurement m = laserH.measure();
          sprintf(msg, "%d:%2.3f", angle, m.range);
        }
      }
      TX_Characteristic->setValue(msg);
    }
  }
};

void setupBLE()
{
  // setup bluetooth device with name Volaser
  BLEDevice::init("Volaser");
  // create the bluetooth server
  BLEServer *pServer = BLEDevice::createServer();
  // create the bluetooth service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // setup a characteristic on this service that we will use for receiving communications
  BLECharacteristic *RX_Characteristic = pService->createCharacteristic(
      RX_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  // setup a callback on this receive characteristic that will parse that communication and act
  RX_Characteristic->setCallbacks(new ParseReceive());

  // setup a characteristic to transmit data back
  TX_Characteristic = pService->createCharacteristic(
      TX_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  // start the service
  pService->start();
  // advertise the service so clients can find it
  pServer->getAdvertising()->start();
}

void setup()
{
  // setup the stepper motor
  stepper.begin(MOTOR_RPM, MOTOR_MICROSTEPS);
  stepper.enable();

  // setup bluetooth
  setupBLE();

  // begin serial communication (only needed for debugging))
  Serial.begin(115200);
  Serial.println("Starting Volaser");
}

void loop() {}