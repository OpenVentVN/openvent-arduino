#ifndef OVFLOWMETER_H
#define OVFLOWMETER_H

#include <stdint.h>

class OVFlowMeter
{
public:
    OVFlowMeter();
    virtual uint16_t getFlow();
};

#endif // OVFLOWMETER_H
