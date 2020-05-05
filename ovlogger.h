#ifndef OVLOGGER_H
#define OVLOGGER_H

#ifdef ARDUINO
#include "Arduino.h"
#else
#include "ovstring.h"
#endif

class OVLogger
{
public:
    OVLogger();

    OVLogger& log(String str);
};

#endif // OVLOGGER_H
