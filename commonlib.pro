#-------------------------------------------------
#
# Project created by QtCreator 2014-01-06T13:17:32
#
#-------------------------------------------------
TARGET = CommonLib
TEMPLATE = lib

QT       += core gui axserver
CONFIG	+= qt dll

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lsetupapi -ladvapi32

RC_FILE  = commonlib.rc
DEF_FILE = commonlib.def

SOURCES += \
    main.cpp \
    ParkingProtocol/cabstractprotocol.cpp \
    ParkingProtocol/cparkingprotocol.cpp \
    ParkingProtocol/cparkingprotocolprivate.cpp \
    SerialPort/cpollingthread.cpp \
    SerialPort/creadthread.cpp \
    SerialPort/cserialpolling.cpp \
    SerialPort/cserialport.cpp \
    SerialPort/cserialportcommon.cpp \
    SerialPort/cserialportprivate.cpp \
    SerialPort/cwritethread.cpp \
    SerialPort/qextserialenumerator.cpp \
    SerialPort/qextserialenumerator_win.cpp

HEADERS += \
    ParkingProtocol/cabstractprotocol.h \
    ParkingProtocol/cparkingprotocol.h \
    ParkingProtocol/cparkingprotocolprivate.h \
    SerialPort/cpollingthread.h \
    SerialPort/creadthread.h \
    SerialPort/cserialpolling.h \
    SerialPort/cserialport.h \
    SerialPort/cserialportcommon.h \
    SerialPort/cserialportprivate.h \
    SerialPort/cticktimer.h \
    SerialPort/cwritethread.h \
    SerialPort/qextserialenumerator.h \
    SerialPort/qextserialenumerator_p.h
