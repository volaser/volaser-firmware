
#include "LaserM703A.h"
// #include <Stepper.h>

#define SERIAL1_RXPIN 32 // defines the RXPIN of Laser 1
#define SERIAL1_TXPIN 25 // defines the TXPIN of Laser 1
#define SERIAL2_RXPIN 27 // defines the RXPIN of Laser 2
#define SERIAL2_TXPIN 13 // defines the TXPIN of Laser 2

String sensorRead; // Variable to check that all the data from the laser was transferred
LaserM703A laser(&Serial1, SERIAL2_RXPIN, SERIAL2_TXPIN);

const int stepPin = 14; // defines the stepPin of the stepper - motor driver
const int dirPin = 26;  // defines the dirPin of the stepper - motor driver
const int enPin = 12;   // defines the enPin of the stepper - motor driver

enum rotation_direction
{
  ROTATE_CLOCKWISE,
  ROTATE_COUNTER_CLOCKWISE
};

void setup()
{
  Serial.begin(115200);

  // Serial1.begin(19200, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  delay(1000);
  Serial.println("Ready to Go!");
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
}

void loop()
{
  rotation(45, ROTATE_COUNTER_CLOCKWISE);
  delay(2000);
  rotation(45, ROTATE_CLOCKWISE);
  delay(2000);
}

void rotation(int steps, enum rotation_direction direction)
{
  if (direction == ROTATE_COUNTER_CLOCKWISE)
  {
    // Enables the motor to move in a particular direction
    digitalWrite(dirPin, HIGH);
  }
  else
  {
    digitalWrite(dirPin, LOW);
  }
  for (int x = 0; x < steps; x++)
  {
    // For loop for x small number of steps
    // Sending the pulses for one step to the stepper motor
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
    Serial.println(x);
    delayMicroseconds(5000);
  }
}
