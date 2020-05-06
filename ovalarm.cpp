#include "ovalarm.h"

OVAlarm::OVAlarm(OVDisplay *disp)
    : _disp(disp)
{

}

void OVAlarm::alarm(OVAlarm::AlarmLevel level)
{
    // simple function
}

void OVAlarm::alarm(OVAlarm::AlarmLevel level, String alarmString)
{
    // display string
}
