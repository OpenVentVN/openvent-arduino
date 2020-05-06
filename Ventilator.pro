TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Protocol.cpp \
        main.cpp \
        ovalarm.cpp \
        ovbreather.cpp \
        ovbvmcontrol.cpp \
        ovdisplay.cpp \
        ovflowmeter.cpp \
        ovlogger.cpp \
        ovpressure.cpp \
        ovstring.cpp \
        ovwaveform.cpp \
        tiny_ekf.c

HEADERS += \
    Protocol.h \
    TinyEKF.h \
    ovalarm.h \
    ovbreather.h \
    ovbvmcontrol.h \
    ovconfig.h \
    ovdisplay.h \
    ovflowmeter.h \
    ovlogger.h \
    ovpressure.h \
    ovstring.h \
    ovwaveform.h \
    tiny_ekf.h \
    tiny_ekf_struct.h
