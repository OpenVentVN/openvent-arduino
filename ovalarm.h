#ifndef OVALARM_H
#define OVALARM_H

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


    OVAlarm();
};

#endif // OVALARM_H
