######################################################################
# Automatically generated by qmake (2.01a) Wed Jan 28 21:10:37 2015
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lrt

# Input
HEADERS += bcm2835.h \
           lcd.h \
           raspilcd.h \
           std_c.h \
           font_terminal_6x8.inc \
           font_terminal_12x16.inc \
           font_fixedsys_8x15.inc \
           font_lucida_10x16.inc \
           bmp_raspi.inc \
           bmp_men.inc \
    lcdscreen.h \
    lcdscreenmain.h \
    lcdscreencpu.h \
    lcdscreenabout.h \
    screenids.h \
    lcdscreentimer.h \
    lcdscreenselect.h \
    lcdscreenmessages.h \
    lcdscreenedit.h
SOURCES += bcm2835.c lcd.c raspilcd.c \
           main.cpp \
    lcdscreen.cpp \
    lcdscreenmain.cpp \
    lcdscreencpu.cpp \
    lcdscreenabout.cpp \
    lcdscreentimer.cpp \
    lcdscreenselect.cpp \
    lcdscreenmessages.cpp \
    lcdscreenedit.cpp

macx|win32{
    DEFINES += QT_EMULATION
    LIBS -= -lrt

    SOURCES -= bcm2835.c

    SOURCES += QtEmulation.cpp
    HEADERS += QtEmulation.h
    MOCS += QtEmulation.h
}
