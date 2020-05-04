TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Adafruit_BMP085.cpp \
        LiquidCrystal_I2C.cpp \
        Protocol.cpp \
        main.cpp \
        ovalarm.cpp \
        ovbreather.cpp \
        ovbvmcontrol.cpp \
        ovdisplay.cpp \
        ovflowmeter.cpp \
        ovlogger.cpp \
        ovpressure.cpp \
        ovwaveform.cpp

HEADERS += \
    Adafruit_BMP085.h \
    LiquidCrystal_I2C.h \
    Protocol.h \
    ovalarm.h \
    ovbreather.h \
    ovbvmcontrol.h \
    ovconfig.h \
    ovdisplay.h \
    ovflowmeter.h \
    ovlogger.h \
    ovpressure.h \
    ovwaveform.h
