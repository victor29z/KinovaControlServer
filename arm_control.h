#ifndef ARM_CONTROL_H
#define ARM_CONTROL_H

#include <QObject>
#include <QThread>
#include "mainwindow.h"
class arm_control : public QThread
{
    Q_OBJECT
public:
    arm_control();
protected:
    virtual void run();
};

#endif // ARM_CONTROL_H
