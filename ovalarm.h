#ifndef OVALARM_H
#define OVALARM_H

#ifdef ARDUINO
#include "Arduino.h"
#else
#include "ovstring.h"
#endif
#include "ovdisplay.h"

/**
 * @brief The OVAlarm class
 */
class OVAlarm
{
public:
    enum AlarmLevel {
      NO_ALARM,
      NOTIFY,
      EMERGENCY,
      OFF_LEVEL,
      NUM_LEVELS
    };


    OVAlarm(OVDisplay *disp);
    void alarm(AlarmLevel level);
    void alarm(AlarmLevel level, String alarmString);

private:
    OVDisplay *_disp;
};

#endif // OVALARM_H
