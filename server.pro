#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T16:42:59
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    jaco.cpp \
    arm_control.cpp \
    bhand/pcan_operate.cpp \
    bhand/bhand.cpp

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
    bhand/bhand.h

FORMS    += mainwindow.ui
