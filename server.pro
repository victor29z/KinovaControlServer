#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T16:42:59
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    jaco.cpp \
    arm_control.cpp \
    bhand/pcan_operate.cpp \
    bhand/bhand.cpp \
    slidercontrol.cpp \
    libmodbus/modbus-data.c \
    libmodbus/modbus-rtu.c \
    libmodbus/modbus-tcp.c \
    libmodbus/modbus.c \
    cam_move_control.cpp

HEADERS  += mainwindow.h \
    server.h \
    ../../../qttest/Example_AngularControl/src/Lib_Examples/Kinova.API.CommLayerUbuntu.h \
    ../../../qttest/Example_AngularControl/src/Lib_Examples/KinovaTypes.h \
    ang.h \
    jaco.h \
    arm_control.h \
    ../../../qttest/Example_AngularControl/src/Lib_Examples/ang.h \
    ../../../qttest/Example_AngularControl/src/Lib_Examples/Kinova.API.UsbCommandLayerUbuntu.h \
    bhand/pcan_operate.h \
    bhand/funtype.h \
    bhand/bhand.h \
    slidercontrol.h \
    libmodbus/config.h \
    libmodbus/modbus-private.h \
    libmodbus/modbus-rtu-private.h \
    libmodbus/modbus-rtu.h \
    libmodbus/modbus-tcp-private.h \
    libmodbus/modbus-tcp.h \
    libmodbus/modbus-version.h \
    libmodbus/modbus.h \
    cam_move_control.h

FORMS    += mainwindow.ui
