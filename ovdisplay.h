#ifndef OVDISPLAY_H
#define OVDISPLAY_H

#if defined(ARDUINO)
#include "LiquidCrystal_I2C.h"
#endif

/**
 * @brief The OVDisplay class
 */

class OVDisplay
{
public:
#if defined(ARDUINO)
    OVDisplay(LiquidCrystal_I2C& _lcd);
#endif

    void init();
    virtual void display();
    void displayLCD(unsigned short a, unsigned short b, unsigned short c, unsigned short d, unsigned short e, unsigned short f);


private:
#if defined(ARDUINO)
    LiquidCrystal_I2C& lcd;
#endif
};

#endif // OVDISPLAY_H
