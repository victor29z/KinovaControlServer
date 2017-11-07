#ifndef CAM_MOVE_CONTROL_H
#define CAM_MOVE_CONTROL_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

class cam_move_control : public QObject
{
    Q_OBJECT
public:
    explicit cam_move_control(QObject *parent = 0);
    QSerialPort *serial;

    void setport(QSerialPort *portname);
    void sethor(int t);
    void setver(int t);
signals:

public slots:
};

#endif // CAM_MOVE_CONTROL_H
