#ifndef JACO_H
#define JACO_H
#include <iostream>
#include <dlfcn.h>
#include <vector>
#include "Kinova.API.CommLayerUbuntu.h"
#include "KinovaTypes.h"
#include <stdio.h>
#include <unistd.h>
#include <ang.h>
#include <QLibrary>

class Jaco//Here we define a class which contains operating functions of kinova
{
public:
    Jaco();

    bool InitState;

    FDInitAPI MyInitAPI;
    FDCloseAPI MyCloseAPI;
    FDMoveHome MyMoveHome;
    FDInitFingers MyInitFingers;
    FDGetDevices MyGetDevices;
    FDSetActiveDevice MySetActiveDevice;
    FDSendBasicTrajectory MySendBasicTrajectory;
    FDGetAngularCommand MyGetAngularCommand;
    FDGetCartesianCommand MyGetCartesianCommand;
    FDGetCartesianPosition MyGetCartesianPosition;
    FDSendJoystickCommand MySendJoystickCommand;

};

#endif // JACO_H

