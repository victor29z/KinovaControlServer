#include "arm_control.h"
#include <QDebug>
arm_control::arm_control(Jaco *arm)
{
    jacoarm = arm;

    arm_status = ARM_STATUS_IDLE;
    CmdFIFO_head = 0;
    CmdFIFO_tail = 0;
    fifolengh = 0;
}

void arm_control::run(){



    while(true){
        //jacoarm->MyGetCartesianPosition(CurrentPos);
        switch (arm_status){
        case ARM_STATUS_IDLE:
            //take out a new trajectory point and set status to move
            if(CmdFIFO_head != CmdFIFO_tail){
                CmdPos = CmdFIFO[CmdFIFO_tail];
                fifolengh --;


                arm_status = ARM_STATUS_MOVE;
                CartesianPosition p;
                p.Coordinates = CmdPos.Position.CartesianPosition;
                /*qDebug()<<"go to point:"
                       << CmdPos.Position.CartesianPosition.X << ","
                       << CmdPos.Position.CartesianPosition.Y << ","
                       << CmdPos.Position.CartesianPosition.Z << ","
                       << CmdPos.Position.CartesianPosition.ThetaX << ","
                       << CmdPos.Position.CartesianPosition.ThetaY << ","
                       << CmdPos.Position.CartesianPosition.ThetaZ << endl;
                */
                qDebug()<<"go to point:"
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.X << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.Y << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.Z << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.ThetaX << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.ThetaY << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.ThetaZ << endl;
                CmdFIFO_tail = (CmdFIFO_tail + 1) % SIZE_CMDFIFO;
                qDebug()<<"head:"<<CmdFIFO_head<<"tail:"<<CmdFIFO_tail<<"length:"<<fifolengh<<endl;
                }

        break;
        case ARM_STATUS_MOVE:
            movepos();


        break;
        case ARM_STATUS_FINISHED:
            if(CmdFIFO_head == CmdFIFO_tail){
                arm_status = ARM_STATUS_IDLE;
                qDebug()<<"job finished! set to idle"<<endl;
            }

            else{   // take out next trajectory cmd

                CmdPos = CmdFIFO[CmdFIFO_tail];
                fifolengh --;


                arm_status = ARM_STATUS_MOVE;
                CartesianPosition p;
                p.Coordinates = CmdPos.Position.CartesianPosition;
                /*qDebug()<<"go to point:"
                       << CmdPos.Position.CartesianPosition.X << ","
                       << CmdPos.Position.CartesianPosition.Y << ","
                       << CmdPos.Position.CartesianPosition.Z << ","
                       << CmdPos.Position.CartesianPosition.ThetaX << ","
                       << CmdPos.Position.CartesianPosition.ThetaY << ","
                       << CmdPos.Position.CartesianPosition.ThetaZ << endl;
                */
                qDebug()<<"go to next point:"
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.X << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.Y << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.Z << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.ThetaX << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.ThetaY << ","
                       << CmdFIFO[CmdFIFO_tail].Position.CartesianPosition.ThetaZ << endl;
                CmdFIFO_tail = (CmdFIFO_tail + 1) % SIZE_CMDFIFO;
                qDebug()<<"head:"<<CmdFIFO_head<<"tail:"<<CmdFIFO_tail<<"length:"<<fifolengh<<endl;

            }

        break;


        }
        msleep(100);
    }

}

void arm_control::gohome(){
    jacoarm->MyMoveHome();

}
void arm_control::GetPos(CartesianPosition pos){
    CurrentPos = pos;
}

void arm_control::AddTrajectoryPoint(CartesianPosition p){
    if(fifolengh < SIZE_CMDFIFO){
        fifolengh ++;
        CmdFIFO[CmdFIFO_head].Position.CartesianPosition.X = p.Coordinates.X;
        CmdFIFO[CmdFIFO_head].Position.CartesianPosition.Y = p.Coordinates.Y;
        CmdFIFO[CmdFIFO_head].Position.CartesianPosition.Z = p.Coordinates.Z;
        CmdFIFO[CmdFIFO_head].Position.CartesianPosition.ThetaX = p.Coordinates.ThetaX;
        CmdFIFO[CmdFIFO_head].Position.CartesianPosition.ThetaY = p.Coordinates.ThetaY;
        CmdFIFO[CmdFIFO_head].Position.CartesianPosition.ThetaZ = p.Coordinates.ThetaZ;

        /**/
        qDebug()<<"Add new fifo point :"
               << CmdFIFO[CmdFIFO_head].Position.CartesianPosition.X << ","
               << CmdFIFO[CmdFIFO_head].Position.CartesianPosition.Y << ","
               << CmdFIFO[CmdFIFO_head].Position.CartesianPosition.Z << ","
               << CmdFIFO[CmdFIFO_head].Position.CartesianPosition.ThetaX << ","
               << CmdFIFO[CmdFIFO_head].Position.CartesianPosition.ThetaY << ","
               << CmdFIFO[CmdFIFO_head].Position.CartesianPosition.ThetaZ << endl;


        CmdFIFO_head = (CmdFIFO_head + 1) % SIZE_CMDFIFO;
        qDebug()<<"head:"<<CmdFIFO_head<<"tail:"<<CmdFIFO_tail<<"length:"<<fifolengh<<endl;

    }
    else{
        qDebug()<<"FIFO overlap, trajectory point lost!";

    }

}
void arm_control::movepos(){
    TrajectoryPoint pointToSend;
    int i = 0;
    pointToSend.InitStruct();

    //We specify that this point will be used an angular(joint by joint) velocity vector.
    //pointToSend.Position.Type = ANGULAR_POSITION;
    pointToSend.Position.Type = CARTESIAN_VELOCITY;
    // handle position control

    if((CurrentPos.Coordinates.X - CmdPos.Position.CartesianPosition.X) > 0.01)
        pointToSend.Position.CartesianPosition.X = -0.5;
    else if((CurrentPos.Coordinates.X - CmdPos.Position.CartesianPosition.X) < -0.01)
        pointToSend.Position.CartesianPosition.X = 0.5;
    else
        i++;
    if((CurrentPos.Coordinates.Y - CmdPos.Position.CartesianPosition.Y) > 0.01)
        pointToSend.Position.CartesianPosition.Y = -0.5;
    else if((CurrentPos.Coordinates.Y - CmdPos.Position.CartesianPosition.Y) < -0.01)
        pointToSend.Position.CartesianPosition.Y = 0.5;
    else
        i++;
    if((CurrentPos.Coordinates.Z - CmdPos.Position.CartesianPosition.Z) > 0.01)
        pointToSend.Position.CartesianPosition.Z = -0.5;
    else if((CurrentPos.Coordinates.Z - CmdPos.Position.CartesianPosition.Z) < -0.01)
        pointToSend.Position.CartesianPosition.Z = 0.5;
    else
        i++;

    if((CurrentPos.Coordinates.ThetaX - CmdPos.Position.CartesianPosition.ThetaX) > 0.1)
        pointToSend.Position.CartesianPosition.ThetaX = -0.5;
    else if((CurrentPos.Coordinates.ThetaX - CmdPos.Position.CartesianPosition.ThetaX) < -0.1)
        pointToSend.Position.CartesianPosition.ThetaX = 0.5;
    else
        i++;
    if((CurrentPos.Coordinates.ThetaY - CmdPos.Position.CartesianPosition.ThetaY) > 0.1)
        pointToSend.Position.CartesianPosition.ThetaY = -0.5;
    else if((CurrentPos.Coordinates.ThetaY - CmdPos.Position.CartesianPosition.ThetaY) < -0.1)
        pointToSend.Position.CartesianPosition.ThetaY = 0.5;
    else
        i++;
    if((CurrentPos.Coordinates.ThetaZ - CmdPos.Position.CartesianPosition.ThetaZ) > 0.1)
        pointToSend.Position.CartesianPosition.ThetaZ = -0.5;
    else if((CurrentPos.Coordinates.ThetaZ - CmdPos.Position.CartesianPosition.ThetaZ) < -0.1)
        pointToSend.Position.CartesianPosition.ThetaZ = 0.5;
    else
        i++;


    if(i == 6)
        arm_status = ARM_STATUS_FINISHED;
    else{
        arm_status = ARM_STATUS_MOVE;
        jacoarm->MySendBasicTrajectory(pointToSend);
    }

}
