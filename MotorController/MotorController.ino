// Bluetooth Libraries
#include <BLEDevice.h>

// winch motor controller
#include "TB6612.h"

// Bluetooth configuration
// Create unique service, receive and transmission IDs for BLE
#define SERVICE_UUID "1c228e6c-0907-4fa6-9fef-34103904da21"
#define RX_UUID "1dd9685e-bd8f-416b-bca2-0b0591709e64"

int pinValueup = 0;
int pinValuedown = 0;

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
#define STBY 13
#define IN1 12
#define IN2 14
#define PWM 27
#define OFFSET 1

#define LED 5
#define LED_CHANNEL 1

// Initializing motor controller from SparkFun_TB6612
Motor motor = Motor(IN1, IN2, PWM, OFFSET, STBY);

// implement a simple callback to parse BLE communications
class ParseReceive : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string message = pCharacteristic->getValue();
    if (message.length() > 0)
    {
      // move the laser up
      if (message[0] == 'U')
      {
        motor.drive(255);
        ledcWrite(LED_CHANNEL, 32);
        Serial.println("Up");
      }
      // move the laser down
      else if (message[0] == 'D')
      {
        motor.drive(-255);
        ledcWrite(LED_CHANNEL, 32);
        Serial.println("Down");
      }
      // otherwise brake the motor
      else
      {
        motor.brake();
        ledcWrite(LED_CHANNEL, 0);
        Serial.println("Brake");
      }
    }
  }
};

void setupBLE()
{
  // setup bluetooth device with name Volaser
  BLEDevice::init("Volaser Winch");
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

  // start the service
  pService->start();
  // advertise the service so clients can find it
  pServer->getAdvertising()->start();
}

void setup()
{
  // setup bluetooth
  setupBLE();

  // begin serial communication (only needed for debugging))
  Serial.begin(115200);
  Serial.println("Starting Volaser Winch");

  pinMode(LED, OUTPUT);
  ledcAttachPin(LED, LED_CHANNEL);
  for (int i = 0; i < 5; i++)
  {
    ledcWrite(LED_CHANNEL, 255);
    delay(25);
    ledcWrite(LED_CHANNEL, 0);
    delay(25);
  }
}

int counter = 0;
void loop()
{
  Serial.println(counter++);
  delay(2000);
}
