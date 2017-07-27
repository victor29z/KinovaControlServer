#include "jaco.h"

Jaco::Jaco()//Here we use QLibrary to load and resolve to map
{
    InitState = false;
    QLibrary mylib("/home/turtlebot/qttest/Example_AngularControl/Kinova.API.USBCommandLayerUbuntu.so");
    //We load the functions from the library
    if(mylib.load())
    {
        MyInitAPI = (FDInitAPI)mylib.resolve("InitAPI");
        MyCloseAPI = (FDCloseAPI)mylib.resolve("CloseAPI");
        MyMoveHome = (FDMoveHome)mylib.resolve("MoveHome");
        MyInitFingers = (FDInitFingers)mylib.resolve("InitFingers");
        MyGetDevices = (FDGetDevices)mylib.resolve("GetDevices");
        MySetActiveDevice = (FDSetActiveDevice)mylib.resolve("SetActiveDevice");
        MySendBasicTrajectory  = (FDSendBasicTrajectory)mylib.resolve("SendBasicTrajectory");
        MyGetAngularCommand  = (FDGetAngularCommand)mylib.resolve("GetAngularCommand");
        MyGetAngularPosition  = (FDGetAngularCommand)mylib.resolve("GetAngularPosition");

        MyGetCartesianCommand = (FDGetCartesianCommand)mylib.resolve("GetCartesianCommand");
        MyGetCartesianPosition = (FDGetCartesianPosition)mylib.resolve("GetCartesianPosition");

    }

    if((MyInitAPI == NULL) || (MyCloseAPI == NULL) || (MySendBasicTrajectory == NULL) ||
       (MySendBasicTrajectory == NULL) || (MyMoveHome == NULL) || (MyInitFingers == NULL) || (MyGetAngularPosition == NULL))
    //if((MyInitAPI == NULL)
    {
        //cout << "* * *  E R R O R   D U R I N G   I N I T I A L I Z A T I O N  * * *" << endl;
        InitState = true;//kinova continues on working when it is true.
    }
    else
        InitState = false;
}



