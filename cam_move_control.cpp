#include "cam_move_control.h"

cam_move_control::cam_move_control(QObject *parent) : QObject(parent)
{


}
void cam_move_control::setport(QSerialPort *portname){

    serial = portname;
}
void cam_move_control::sethor(int t){
    QString tmp;
    tmp.sprintf("#1P%dT1000\r\n",t);
    qDebug()<<tmp;
    serial->write(tmp.toLatin1());
}
void cam_move_control::setver(int t){
    QString tmp;
    tmp.sprintf("#2P%dT1000\r\n",t);
    serial->write(tmp.toLatin1());
    qDebug()<<tmp;
}
