#ifndef OVPRESSURE_H
#define OVPRESSURE_H

#include <stdint.h>

#ifdef ARDUINO
#include "Adafruit_BMP085.h"
#endif

// TODO: apply EKF
// These must be defined before including TinyEKF.h
// FIXME: include before TinyEKF.h
#define Nsta 2     // Two state values: pressure, temperature
#define Mobs 3     // Three measurements: baro pressure, baro temperature, LM35 temperature
#include "TinyEKF.h"

class OVPressure : public TinyEKF
{
public:
    #define NUM     5

    OVPressure();
    bool init();

    uint16_t getPressure(int samples = NUM);
    virtual void model(double fx[Nsta], double F[Nsta][Nsta], double hx[Mobs], double H[Mobs][Nsta]);

private:
#ifdef ARDUINO
    Adafruit_BMP085 bmp;
#endif
    long            g_arr[NUM];
};

#endif // OVPRESSURE_H
