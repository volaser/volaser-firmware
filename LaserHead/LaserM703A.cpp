#include "Arduino.h"
#include "LaserM703A.h"

// use a timeout of 4s (datasheet says max is 3s)
#define TIMEOUT 4000

LaserM703A::LaserM703A(HardwareSerial *Serial, int RX, int TX)
{
    _Serial = Serial;
    _Serial->begin(19200, SERIAL_8N1, RX, TX);
    _Serial->setTimeout(TIMEOUT);
}

measurement LaserM703A::measure(void)
{
    // Serial command 'D' performs a single measurement in auto measurement mode
    // which echos the initial character 'D' and returns a string of the form
    // ':12.345m,0079\n', where the first number is the range in meters and the
    // next number is the signal quality, where a smaller number indicates better
    // signal quality.
    measurement result;

    // wait for any outgoing data to finish
    _Serial->flush();
    // clear out the serial buffer
    while (_Serial->available())
    {
        _Serial->read();
    }

    _Serial->write('D');
    String result_str = _Serial->readStringUntil('\n');
    if (result_str[2] == 'E')
    {
        result.range = 0;
        result.quality = 9999;
        result.error = true;
    }
    else
    {
        String range_str = result_str.substring(2, 8);
        String quality_str = result_str.substring(10, 14);
        result.range = range_str.toFloat();
        result.quality = quality_str.toInt();
        result.error = false;
    }
    return result;
}
