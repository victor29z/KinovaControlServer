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
    arm_control.cpp

HEADERS  += mainwindow.h \
    server.h \
    ../../../qttest/Example_AngularControl/src/Lib_Examples/Kinova.API.CommLayerUbuntu.h \
    ../../../qttest/Example_AngularControl/src/Lib_Examples/KinovaTypes.h \
    ang.h \
    jaco.h \
    arm_control.h

FORMS    += mainwindow.ui
