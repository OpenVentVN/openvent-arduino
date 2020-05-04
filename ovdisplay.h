#ifndef OVDISPLAY_H
#define OVDISPLAY_H

#include "LiquidCrystal_I2C.h"

/**
 * @brief The OVDisplay class
 */

class OVDisplay
{
public:
    OVDisplay(LiquidCrystal_I2C& _lcd);

    void init();
    virtual void display();
    void displayLCD(unsigned short a, unsigned short b, unsigned short c, unsigned short d, unsigned short e, unsigned short f);


private:
    LiquidCrystal_I2C& lcd;
};

#endif // OVDISPLAY_H
