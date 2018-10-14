/******************************************************************************
TB6612.cpp
TB6612FNG H-Bridge Motor Driver
Modified from the Sparkfun library to be compatible with ESP32
Michelle @ SparkFun Electronics
8/20/16
https://github.com/sparkfun/SparkFun_TB6612FNG_Arduino_Library
******************************************************************************/

#include "TB6612.h"
#include <Arduino.h>

#define FREQ 490
#define RESOLUTION 8
#define CHANNEL 0

Motor::Motor(int In1pin, int In2pin, int PWMpin, int offset, int STBYpin)
{
    In1 = In1pin;
    In2 = In2pin;
    PWM = PWMpin;
    Standby = STBYpin;
    Offset = offset;

    pinMode(In1, OUTPUT);
    pinMode(In2, OUTPUT);
    pinMode(PWM, OUTPUT);

    ledcSetup(CHANNEL, FREQ, RESOLUTION);
    ledcAttachPin(PWM, CHANNEL);
    pinMode(Standby, OUTPUT);
}

void Motor::drive(int speed)
{
    digitalWrite(Standby, HIGH);
    speed = speed * Offset;
    if (speed >= 0)
        fwd(speed);
    else
        rev(-speed);
}
void Motor::drive(int speed, int duration)
{
    drive(speed);
    delay(duration);
}

void Motor::fwd(int speed)
{
    digitalWrite(In1, HIGH);
    digitalWrite(In2, LOW);
    ledcWrite(CHANNEL, speed);
}

void Motor::rev(int speed)
{
    digitalWrite(In1, LOW);
    digitalWrite(In2, HIGH);
    ledcWrite(CHANNEL, speed);
}

void Motor::brake()
{
    digitalWrite(In1, HIGH);
    digitalWrite(In2, HIGH);
    ledcWrite(CHANNEL, 0);
}

void Motor::standby()
{
    digitalWrite(Standby, LOW);
}