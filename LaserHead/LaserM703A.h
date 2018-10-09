#ifndef _LIB_M703A_
#define _LIB_M703A_

#include "Arduino.h"

struct measurement
{
  float range;
  int quality;
  bool error;
};

class LaserM703A
{
public:
  LaserM703A(HardwareSerial *Serial, int RX, int TX);
  measurement measure(void);

private:
  HardwareSerial *_Serial;
};

#endif