#include "ovdisplay.h"
#include "ovconfig.h"

#if defined(ARDUINO)
OVDisplay::OVDisplay(LiquidCrystal_I2C& _lcd)
    : lcd(_lcd)
{

}
#else
#endif

void OVDisplay::init()
{
#if defined(ARDUINO)
    // init lcd
    lcd.init();

    // Print a message to the LCD.
    lcd.backlight();
#endif
}

void OVDisplay::display()
{
    // do nothing
}


void OVDisplay::displayLCD(unsigned short a, unsigned short b, unsigned short c, unsigned short d, unsigned short e, unsigned short f)
{
#if defined(ARDUINO)
    unsigned short ng, tr, ch, dv, temp;

    lcd.setCursor(0,0);   lcd.print("Vt   Ti    RR    Mode");
    lcd.setCursor(0,2);   lcd.print("Peep Pip   Sup");

    if      (st_data_set.g_Mode == A_VC) {lcd.setCursor(16,1); lcd.print("AC/ "); lcd.setCursor(16,2); lcd.print("  VC");}
    else if (st_data_set.g_Mode == SIMV) {lcd.setCursor(16,1); lcd.print("SIMV"); lcd.setCursor(16,2); lcd.print(" +PS");}
    else if (st_data_set.g_Mode == PRVC) {lcd.setCursor(16,1); lcd.print("PRVC"); lcd.setCursor(16,2); lcd.print("    ");}
    else if (st_data_set.g_Mode == SET)  {lcd.setCursor(16,1); lcd.print("SET "); lcd.setCursor(16,2); lcd.print("    ");}

    ng = a/1000;
    temp = a%1000;
    tr = temp/100;
    temp = a%100; ch = temp/10;
    dv = a%10;
    lcd.setCursor(0,1);   lcd.write(tr+0x30); lcd.write(ch+0x30); lcd.write(dv+0x30);
    ng = b/1000;
    temp = b%1000;
    tr = temp/100;
    temp = b%100; ch = temp/10;
    //dv = b%10;
    lcd.setCursor(5,1);   lcd.write(ng+0x30); lcd.write('.');     lcd.write(tr+0x30);   lcd.write(ch+0x30);
    tr = c/100;
    temp = c%100; ch = temp/10;
    dv = c%10;
    lcd.setCursor(11,1);  lcd.write(ch+0x30); lcd.write(dv+0x30);

    ch = d/10;
    dv = d%10;
    lcd.setCursor(0,3);   lcd.write(ch+0x30); lcd.write(dv+0x30);
    ch = e/10;
    dv = e%10;
    lcd.setCursor(5,3);   lcd.write(ch+0x30); lcd.write(dv+0x30);
    f *= 10;
    tr = f/100;
    ch = f%100; ch = ch/10;
    dv = f%10;
    lcd.setCursor(11,3);  lcd.write(tr+0x30); lcd.write(ch+0x30); lcd.write(dv+0x30); lcd.print("%");
#endif
}
