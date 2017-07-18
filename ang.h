#ifndef ANG_H//This Header file is newly-built rather than adding from existing files.
#define ANG_H
#include "Kinova.API.CommLayerUbuntu.h"
#include "KinovaTypes.h"//inlcude these two header files is necessary for defining functions of kinova
int ang_ctrl(void);

/*MyInitAPI = (int (*)()) dlsym(commandLayer_handle,"InitAPI");
MyCloseAPI = (int (*)()) dlsym(commandLayer_handle,"CloseAPI");
MyMoveHome = (int (*)()) dlsym(commandLayer_handle,"MoveHome");
MyInitFingers = (int (*)()) dlsym(commandLayer_handle,"InitFingers");
MyGetDevices = (int (*)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result)) dlsym(commandLayer_handle,"GetDevices");
MySetActiveDevice = (int (*)(KinovaDevice devices)) dlsym(commandLayer_handle,"SetActiveDevice");
MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(commandLayer_handle,"SendBasicTrajectory");
MyGetAngularCommand = (int (*)(AngularPosition &)) dlsym(commandLayer_handle,"GetAngularCommand");
MyGetCartesianCommand = (int (*)(CartesianPosition &)) dlsym(commandLayer_handle,"GetCartesianCommand");
MyGetCartesianPosition = (int (*)(CartesianPosition &)) dlsym(commandLayer_handle,"GetCartesianPosition");
*/

typedef int (* FDInitAPI)();
typedef int (* FDCloseAPI)();
typedef int (* FDMoveHome)();
typedef int (* FDInitFingers)();
typedef int (* FDGetDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);
typedef int (* FDSetActiveDevice)(KinovaDevice devices);
typedef int (* FDSendBasicTrajectory)(TrajectoryPoint);
typedef int (* FDGetAngularCommand)(AngularPosition &);
typedef int (* FDGetCartesianCommand)(CartesianPosition &);
typedef int (* FDGetCartesianPosition)(CartesianPosition &);
typedef int (* FDGetCartesianPosition)(CartesianPosition &);
typedef int (* FDSendJoystickCommand)(JoystickCommand &);
#endif // ANG_H




