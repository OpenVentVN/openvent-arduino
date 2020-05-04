#ifndef OVBVMCONTROL_H
#define OVBVMCONTROL_H

#include <Arduino.h>

class OVBVMControl
{
public:
    OVBVMControl();

    inline void delay_us(long us);
    inline long calculatePulse(long Vt);

    void Compress(long Vt, long Ti);
    void Home();
};


inline void OVBVMControl::delay_us(long us)
{
    us = (us*4)/5;
    while(us--) {
      delayMicroseconds(2);
    }
}

inline long OVBVMControl::calculatePulse(long Vt)
{
    float Vt_p = Vt/800.0;
    Vt_p = sqrt(Vt_p)*450;
                                //Max 600 pulse
    return (long)(Vt_p) + 70;  //offset 150*2 pulse
}

#endif // OVBVMCONTROL_H
