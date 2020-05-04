#ifndef OVPRESSURE_H
#define OVPRESSURE_H

#include <stdint.h>
#include "Adafruit_BMP085.h"

class OVPressure
{
public:
    #define NUM     5

    OVPressure();
    bool init();

    uint16_t getPressure(int samples = NUM);

private:
    Adafruit_BMP085 bmp;
    long            g_arr[NUM];
};

#endif // OVPRESSURE_H
