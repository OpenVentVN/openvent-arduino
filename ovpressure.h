#ifndef OVPRESSURE_H
#define OVPRESSURE_H

#include <stdint.h>

#ifdef ARDUINO
#include "Adafruit_BMP085.h"
#endif

class OVPressure
{
public:
    #define NUM     5

    OVPressure();
    bool init();

    uint16_t getPressure(int samples = NUM);

private:
#ifdef ARDUINO
    Adafruit_BMP085 bmp;
#endif
    long            g_arr[NUM];
};

#endif // OVPRESSURE_H
