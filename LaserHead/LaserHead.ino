// Bluetooth libraries
#include <BLEDevice.h>

// Laser library
#include "TFMini.h"

// Stepper motor library
#include "BasicStepperDriver.h"

// Bluetooth configuration
// Create unique service, receive and transmission IDs for BLE
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define RX_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define TX_UUID "a812aeed-78d0-474a-b9b1-20a8a1f95463"

BLECharacteristic *TX_Characteristic = NULL;

// setup communication with laser modules
#define LaserV_RX 13 // defines the RXPIN of Vertical Laser
#define LaserV_TX 12 // defines the TXPIN of Vertical Laser
#define LaserH_RX 14 // defines the RXPIN of Horizontal Laser
#define LaserH_TX 27 // defines the TXPIN of Horizontal Laser

TFMini laserV;
TFMini laserH;

// Setup stepper motor control
// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define MOTOR_RPM 60
#define MOTOR_MICROSTEPS 16

#define STEPPER_DIR 32
#define STEPPER_STEP 33
#define STEPPER_MS 25
#define STEPPER_ENABLE 26

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
      char msg[11];
      // make a measurement of the vertical laser
      if (message[0] == 'V')
      {
        while (Serial1.available())
        {
          Serial1.read();
        }
        int16_t distance = laserV.getDistance();
        float range;
        if (distance > 0)
        {
          range = distance / 100.0;
        }
        Serial.print("vertical: ");
        Serial.println(range);

        sprintf(msg, "%2.2f", range);
      }
      // make a measurement of the horizontal laser
      else if (message[0] == 'H')
      {
        while (Serial2.available())
        {
          Serial2.read();
        }
        int16_t distance = laserH.getDistance();
        float range;
        if (distance > 0)
        {
          range = distance / 100.0;
        }
        Serial.print("horizontal: ");
        Serial.println(range);

        sprintf(msg, "%2.2f", range);
      }
      // disable the motor to save power
      else if (message[0] == 'D')
      {
        stepper.disable();
      }
      // enable the motor to make a measurement
      else if (message[0] == 'E')
      {
        stepper.enable();
        angle = 0;
      }
      // rotate the stepper motor to a given angle,
      // and then make a measurement of the horizontal laser
      else if (message[0] == 'R')
      {
        // These commands are the form R90, or R-30, to rotate to +90 degrees or -30 degrees
        // we constrain ourselves to stay within +/-180 degrees
        int new_angle = String(message.c_str()).substring(1).toInt();
        Serial.print("new angle: ");
        Serial.println(new_angle);
        stepper.rotate(new_angle - angle);
        angle = new_angle;
        Serial.print("angle: ");
        Serial.print(angle);
        while (Serial2.available())
        {
          Serial2.read();
        }
        int16_t distance = laserH.getDistance();
        float range;
        if (distance > 0)
        {
          range = distance / 100.0;
        }

        Serial.print("  range: ");
        Serial.println(range);
        sprintf(msg, "%d:%2.2f", angle, range);
        Serial.println(msg);
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
  // setup stepper motor
  // enable microsteps
  pinMode(STEPPER_MS, OUTPUT);
  digitalWrite(STEPPER_MS, HIGH);
  // setup the stepper motor
  stepper.begin(MOTOR_RPM, MOTOR_MICROSTEPS);
  stepper.disable();

  // setup lasers
  // start serial ports
  Serial1.begin(TFMINI_BAUDRATE, SERIAL_8N1, LaserV_RX, LaserV_TX);
  Serial2.begin(TFMINI_BAUDRATE, SERIAL_8N1, LaserH_RX, LaserH_TX);
  laserV.begin(&Serial1);
  delay(100);
  laserH.begin(&Serial2);
  delay(100);

  // setup bluetooth
  setupBLE();

  // begin serial communication (only needed for debugging))
  Serial.begin(115200);
  Serial.println("Starting Volaser");
}

void loop()
{
  while (Serial1.available())
  {
    Serial1.read();
  }
  while (Serial2.available())
  {
    Serial2.read();
  }
  delay(1000);
}