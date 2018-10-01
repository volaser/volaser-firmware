#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "LaserM703A.h"

#define SERIAL1_RXPIN 32 // defines the RXPIN of Laser 1
#define SERIAL1_TXPIN 25 // defines the TXPIN of Laser 1
#define SERIAL2_RXPIN 27 // defines the RXPIN of Laser 2
#define SERIAL2_TXPIN 13 // defines the TXPIN of Laser 2

LaserM703A laser(&Serial1, SERIAL2_RXPIN, SERIAL2_TXPIN);

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define RX_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define TX_UUID "a812aeed-78d0-474a-b9b1-20a8a1f95463"

BLECharacteristic *TX_Characteristic = NULL;

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0)
    {
      measurement m = laser.measure();
      Serial.print("Range: ");
      Serial.println(m.range);
      char msg[10];
      sprintf(msg, "%2.3f", m.range);
      TX_Characteristic->setValue(msg);
    }
  }
};

void setup()
{
  Serial.begin(115200);
  delay(1000);

  measurement m = laser.measure();
  Serial.print("Starting Volaser");

  BLEDevice::init("Volaser");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *RX_Characteristic = pService->createCharacteristic(
      RX_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  TX_Characteristic = pService->createCharacteristic(
      TX_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  TX_Characteristic->addDescriptor(new BLE2902());

  RX_Characteristic->setCallbacks(new MyCallbacks());
  TX_Characteristic->setValue("hello");

  pService->start();

  pServer->getAdvertising()->start();
}

void loop() {}
