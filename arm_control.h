#ifndef ARM_CONTROL_H
#define ARM_CONTROL_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "jaco.h"

#define ARM_STATUS_IDLE     0x01
#define ARM_STATUS_MOVE     0x02
#define ARM_STATUS_FINISHED 0x03

#define SIZE_CMDFIFO        100
class arm_control : public QThread
{
    Q_OBJECT
public:
    arm_control(Jaco* arm);
    Jaco *jacoarm;

    CartesianPosition CurrentPos;
    TrajectoryPoint CmdPos;

    TrajectoryPoint CmdFIFO[100];

    unsigned char fifolengh;
    unsigned char CmdFIFO_head,CmdFIFO_tail;
    unsigned char arm_status;


public slots:
    void gohome();
    void movepos();
    void GetPos(CartesianPosition pos);
    void AddTrajectoryPoint(CartesianPosition p);
protected:
    virtual void run();
};

#endif // ARM_CONTROL_H
