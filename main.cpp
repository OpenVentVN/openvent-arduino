#ifndef ARDUINO
#include <stdio.h>

#include "ovconfig.h"
#include "ovdisplay.h"
#include "ovalarm.h"
#include "ovflowmeter.h"
#include "ovlogger.h"
#include "ovpressure.h"
#include "ovwaveform.h"
#include "ovbvmcontrol.h"

#include "ovstring.h"

int main(int args, char* argv[])
{
    String s = "Hello World";

    printf("%s\n", s.c_str());
    return 0;
}

#endif
