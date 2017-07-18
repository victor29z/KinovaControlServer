#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include"iostream"
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Tcp server"));
    pserver = new Server;
    jacoarm = new Jaco();

    key_state.xp = false;
    key_state.xn = false;
    key_state.yp = false;
    key_state.yn = false;
    key_state.zp = false;
    key_state.zn = false;

    key_state.txp = false;
    key_state.txn = false;
    key_state.typ = false;
    key_state.tyn = false;
    key_state.tzp = false;
    key_state.tzn = false;

    remote_enbaled = false;

    timer = new QTimer(this);
    timer->start(5);
   //connect(pserver,SIGNAL(pserver->data_recieved(pserver->ser)),this,SLOT(data_react()));
    connect(pserver,SIGNAL(data_recieved(float*)),this,SLOT(data_react(float*)));//parameter should not be ignored
    connect(pserver,SIGNAL(data_send()),this,SLOT(data_sendback()));
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_out()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_init_pushButton_clicked()
{
    int result;
    CartesianPosition dataPosition;
    result = jacoarm->MyInitAPI();
    cout << "Initialization's result :" << result << endl;

    KinovaDevice list[MAX_KINOVA_DEVICE];

    int devicesCount = jacoarm->MyGetDevices(list, result);

    for(int i = 0; i < devicesCount; i++)
    {
        cout << "Found a robot on the USB bus (" << list[i].SerialNumber << ")" << endl;

        //Setting the current device as the active device.
        jacoarm->MySetActiveDevice(list[i]);

        //cout << "Send the robot to HOME position" << endl;
        //jacoarm->MyMoveHome();1.5

        //cout << "Initializing the fingers" << endl;
        //jacoarm->MyInitFingers();
     }

    jacoarm->MyGetCartesianPosition(dataPosition);
    RemotePoseCmd.Position.CartesianPosition.X = dataPosition.Coordinates.X;
    RemotePoseCmd.Position.CartesianPosition.Y = dataPosition.Coordinates.Y;
    RemotePoseCmd.Position.CartesianPosition.Z = dataPosition.Coordinates.Z;
    RemotePoseCmd.Position.CartesianPosition.ThetaX = dataPosition.Coordinates.ThetaX;
    RemotePoseCmd.Position.CartesianPosition.ThetaY = dataPosition.Coordinates.ThetaY;
    RemotePoseCmd.Position.CartesianPosition.ThetaZ = dataPosition.Coordinates.ThetaZ;
}

void MainWindow::on_close_pushButton_clicked()//there is no reaction when we click close
{
    jacoarm->MyCloseAPI();
}

void MainWindow::on_exit_pushButton_clicked()
{
    exit(0);
}


/*void MainWindow::on_confirm_pushButton_clicked()
{
    AngularPosition currentCommand;
    if(jacoarm->InitState)//see details in jaco.cpp
    {
        cout << "* * *  E R R O R   D U R I N G   I N I T I A L I Z A T I O N  * * *" << endl;
    }
    else
    {
        cout << "I N I T I A L I Z A T I O N   C O M P L E T E D" << endl << endl;
        TrajectoryPoint pointToSend;
        pointToSend.InitStruct();

        pointToSend.Position.Type = ANGULAR_POSITION;

        //We get the actual angular command of the robot.
        jacoarm->MyGetAngularCommand(currentCommand);

        pointToSend.Position.Actuators.Actuator1 = currentCommand.Actuators.Actuator1 +  pserver->ser[0];;
        pointToSend.Position.Actuators.Actuator2 = currentCommand.Actuators.Actuator2 +  pserver->ser[1];;
        pointToSend.Position.Actuators.Actuator3 = currentCommand.Actuators.Actuator3 +  pserver->ser[2];;
        pointToSend.Position.Actuators.Actuator4 = currentCommand.Actuators.Actuator4 +  pserver->ser[3];;
        pointToSend.Position.Actuators.Actuator5 = currentCommand.Actuators.Actuator5 +  pserver->ser[4];;
        pointToSend.Position.Actuators.Actuator6 = currentCommand.Actuators.Actuator6 +  pserver->ser[5];;
        //when controlling angular,we'd better not set its value directly,but use the adding way.(+)
        //in order to prevent Kinova from being damaged.
        cout << "*********************************" << endl;
        cout << "Sending the first point to the robot." << endl;
        jacoarm->MySendBasicTrajectory(pointToSend);


        cout << "*********************************" << endl << endl << endl;


        cout << endl << "WARNING: Your robot is now set to angular control. If you use the joystick, it will be a joint by joint movement." << endl;
        cout << endl << "C L O S I N G   A P I" << endl;

    }


}*/

void MainWindow::data_react(float *data)
{
    TrajectoryPoint pointToSend;


    if(jacoarm->InitState)//see details in jaco.cpp
    {
        cout << "* * *  E R R O R   D U R I N G   I N I T I A L I Z A T I O N  * * *" << endl;
    }
    else
    {



        pointToSend.InitStruct();

        //pointToSend.Position.Type = ANGULAR_POSITION;
        pointToSend.Position.Type = CARTESIAN_POSITION;
        //We get the actual angular command of the robot.


        //RemotePoseCmd = RemotePoseHome;
        if(remote_mode == REMOTE_MODE_ABS)
        {
            RemotePoseCmd.Position.CartesianPosition.X = RemotePoseHome.Position.CartesianPosition.X - data[1] * 2;
            RemotePoseCmd.Position.CartesianPosition.Y = RemotePoseHome.Position.CartesianPosition.Y + (data[0]-0.14) * 2;
            RemotePoseCmd.Position.CartesianPosition.Z = RemotePoseHome.Position.CartesianPosition.Z + data[2] * 2;
            RemotePoseCmd.Position.CartesianPosition.ThetaX = RemotePoseHome.Position.CartesianPosition.ThetaX + (data[4] + 1.3)  ;
            RemotePoseCmd.Position.CartesianPosition.ThetaY = RemotePoseHome.Position.CartesianPosition.ThetaY + (data[5] - 2.1) ;
            RemotePoseCmd.Position.CartesianPosition.ThetaZ = RemotePoseHome.Position.CartesianPosition.ThetaZ + (data[3] + 3.8)  ;
        }
        if(remote_mode == REMOTE_MODE_REL){
            RemoteSpeedCmd.Position.CartesianPosition.X = - data[1];
            RemoteSpeedCmd.Position.CartesianPosition.Y = data[0] - 0.14;
            RemoteSpeedCmd.Position.CartesianPosition.Z = data[2];
            RemoteSpeedCmd.Position.CartesianPosition.ThetaX = data[4] + 1.3;
            RemoteSpeedCmd.Position.CartesianPosition.ThetaY = data[5] - 2.1;
            RemoteSpeedCmd.Position.CartesianPosition.ThetaZ = data[3] + 3.8;

        }

    }


}


void MainWindow::data_sendback(){
    CartesianPosition dataPosition;
    TCP_PACK_DEF pack_sendback;
    jacoarm->MyGetCartesianPosition(dataPosition);

    pack_sendback.pack_type = PACK_SENDBACK;
    pack_sendback.data[0] = dataPosition.Coordinates.X;
    pack_sendback.data[1] = dataPosition.Coordinates.Y;
    pack_sendback.data[2] = dataPosition.Coordinates.Z;
    pack_sendback.data[3] = dataPosition.Coordinates.ThetaX;
    pack_sendback.data[4] = dataPosition.Coordinates.ThetaY;
    pack_sendback.data[5] = dataPosition.Coordinates.ThetaZ;

    pserver->tcpSocket->write((char*)(&pack_sendback),sizeof(TCP_PACK_DEF));//force change

}

void MainWindow::on_pb_home_clicked()
{
    cout << "Send the robot to HOME position" << endl;
    jacoarm->MyMoveHome();
}

void MainWindow::on_pb_move_clicked()
{
    TrajectoryPoint pointToSend;


    if(jacoarm->InitState)//see details in jaco.cpp
    {
        cout << "* * *  E R R O R   D U R I N G   I N I T I A L I Z A T I O N  * * *" << endl;
    }
    else
    {



        pointToSend.InitStruct();

        //pointToSend.Position.Type = ANGULAR_POSITION;
        pointToSend.Position.Type = CARTESIAN_POSITION;
        //We get the actual angular command of the robot.


        //RemotePoseCmd = RemotePoseHome;

        RemotePoseCmd.Position.CartesianPosition.X = 0.180657;
        RemotePoseCmd.Position.CartesianPosition.Y = 0.420958;
        RemotePoseCmd.Position.CartesianPosition.Z = 0;
        RemotePoseCmd.Position.CartesianPosition.ThetaX = -1.71755  ;
        RemotePoseCmd.Position.CartesianPosition.ThetaY = -0.31522 ;
        RemotePoseCmd.Position.CartesianPosition.ThetaZ = 2.01846  ;

        jacoarm->MySendBasicTrajectory(pointToSend);

    }

}

void MainWindow::on_pb_getp_clicked()
{
    CartesianPosition dataPosition;
    jacoarm->MyGetCartesianPosition(dataPosition);
    ui->labx->setNum(dataPosition.Coordinates.X);
    ui->laby->setNum(dataPosition.Coordinates.Y);
    ui->labz->setNum(dataPosition.Coordinates.Z);
    ui->labtx->setNum(dataPosition.Coordinates.ThetaX);
    ui->labty->setNum(dataPosition.Coordinates.ThetaY);
    ui->labtz->setNum(dataPosition.Coordinates.ThetaZ);
}


void MainWindow::timer_out(){

    TrajectoryPoint pointToSend;
    CartesianPosition dataPosition;
    static int i;
    jacoarm->MyGetCartesianPosition(dataPosition);
    pointToSend.InitStruct();

    //We specify that this point will be used an angular(joint by joint) velocity vector.
    pointToSend.Position.Type = CARTESIAN_VELOCITY;


    if(remote_enbaled == false){
        if(key_state.xp)
            pointToSend.Position.CartesianPosition.X = 0.5;
        if(key_state.xn)
            pointToSend.Position.CartesianPosition.X = -0.5;

        if(key_state.yp)
            pointToSend.Position.CartesianPosition.Y = 0.5;
        if(key_state.yn)
            pointToSend.Position.CartesianPosition.Y = -0.5;

        if(key_state.zp)
            pointToSend.Position.CartesianPosition.Z = 0.5;
        if(key_state.zn)
            pointToSend.Position.CartesianPosition.Z = -0.5;

        if(key_state.txp){
            pointToSend.Position.CartesianPosition.ThetaX = 0.5;
            pointToSend.Position.CartesianPosition.ThetaY = 0.5;
        }

        if(key_state.txn){
            pointToSend.Position.CartesianPosition.ThetaX = -0.5;
            pointToSend.Position.CartesianPosition.ThetaY = -0.5;
        }


        if(key_state.typ)
            pointToSend.Position.CartesianPosition.ThetaY = 0.5;
        if(key_state.tyn)
            pointToSend.Position.CartesianPosition.ThetaY = -0.5;

        if(key_state.tzp)
            pointToSend.Position.CartesianPosition.ThetaZ = 0.5;
        if(key_state.tzn)
            pointToSend.Position.CartesianPosition.ThetaZ = -0.5;
    }
    else{
        if(remote_mode == REMOTE_MODE_ABS)
        {
            // handle position control
            if((dataPosition.Coordinates.X - RemotePoseCmd.Position.CartesianPosition.X) > 0.01)
                pointToSend.Position.CartesianPosition.X = -0.1;
            else if((dataPosition.Coordinates.X - RemotePoseCmd.Position.CartesianPosition.X) < -0.01)
                pointToSend.Position.CartesianPosition.X = 0.1;

            if((dataPosition.Coordinates.Y - RemotePoseCmd.Position.CartesianPosition.Y) > 0.01)
                pointToSend.Position.CartesianPosition.Y = -0.1;
            else if((dataPosition.Coordinates.Y - RemotePoseCmd.Position.CartesianPosition.Y) < -0.01)
                pointToSend.Position.CartesianPosition.Y = 0.1;

            if((dataPosition.Coordinates.Z - RemotePoseCmd.Position.CartesianPosition.Z) > 0.01)
                pointToSend.Position.CartesianPosition.Z = -0.1;
            else if((dataPosition.Coordinates.Z - RemotePoseCmd.Position.CartesianPosition.Z) < -0.01)
                pointToSend.Position.CartesianPosition.Z = 0.1;

            if((dataPosition.Coordinates.ThetaX - RemotePoseCmd.Position.CartesianPosition.ThetaX) > 0.01)
                pointToSend.Position.CartesianPosition.ThetaX = -0.1;
            else if((dataPosition.Coordinates.ThetaX - RemotePoseCmd.Position.CartesianPosition.ThetaX) < -0.01)
                pointToSend.Position.CartesianPosition.ThetaX = 0.1;

            if((dataPosition.Coordinates.ThetaY - RemotePoseCmd.Position.CartesianPosition.ThetaY) > 0.01)
                pointToSend.Position.CartesianPosition.ThetaY = -0.1;
            else if((dataPosition.Coordinates.ThetaY - RemotePoseCmd.Position.CartesianPosition.ThetaY) < -0.01)
                pointToSend.Position.CartesianPosition.ThetaY = 0.1;

            if((dataPosition.Coordinates.ThetaZ - RemotePoseCmd.Position.CartesianPosition.ThetaZ) > 0.01)
                pointToSend.Position.CartesianPosition.ThetaZ = -0.1;
            else if((dataPosition.Coordinates.ThetaZ - RemotePoseCmd.Position.CartesianPosition.ThetaZ) < -0.01)
                pointToSend.Position.CartesianPosition.ThetaZ = 0.1;
        }
        if(remote_mode == REMOTE_MODE_REL)
        {
            if(RemoteSpeedCmd.Position.CartesianPosition.X > 0.04 || RemoteSpeedCmd.Position.CartesianPosition.X < -0.04)
                pointToSend.Position.CartesianPosition.X = RemoteSpeedCmd.Position.CartesianPosition.X;
            else
                pointToSend.Position.CartesianPosition.X = 0;

            if(RemoteSpeedCmd.Position.CartesianPosition.Y > 0.04 || RemoteSpeedCmd.Position.CartesianPosition.Y < -0.04)
                pointToSend.Position.CartesianPosition.Y = RemoteSpeedCmd.Position.CartesianPosition.Y;
            else
                pointToSend.Position.CartesianPosition.Y = 0;

            if(RemoteSpeedCmd.Position.CartesianPosition.Z > 0.04 || RemoteSpeedCmd.Position.CartesianPosition.Z < -0.04)
                pointToSend.Position.CartesianPosition.Z = RemoteSpeedCmd.Position.CartesianPosition.Z;
            else
                pointToSend.Position.CartesianPosition.Z = 0;

            if(RemoteSpeedCmd.Position.CartesianPosition.ThetaX > 0.7 || RemoteSpeedCmd.Position.CartesianPosition.ThetaX < -0.7)
                pointToSend.Position.CartesianPosition.ThetaX = RemoteSpeedCmd.Position.CartesianPosition.ThetaX / 3;
            else
                pointToSend.Position.CartesianPosition.ThetaX = 0;

            if(RemoteSpeedCmd.Position.CartesianPosition.ThetaY > 0.7 || RemoteSpeedCmd.Position.CartesianPosition.ThetaY < -0.7)
                pointToSend.Position.CartesianPosition.ThetaY = RemoteSpeedCmd.Position.CartesianPosition.ThetaY / 3;
            else
                pointToSend.Position.CartesianPosition.ThetaY = 0;

            if(RemoteSpeedCmd.Position.CartesianPosition.ThetaZ > 1 || RemoteSpeedCmd.Position.CartesianPosition.ThetaZ < -1)
                pointToSend.Position.CartesianPosition.ThetaZ = RemoteSpeedCmd.Position.CartesianPosition.ThetaZ / 3;
            else
                pointToSend.Position.CartesianPosition.ThetaZ = 0;
        }

    }

    jacoarm->MySendBasicTrajectory(pointToSend);

    if(i < 10)
        i++;
    else{
        i = 0;

        ui->labx->setNum(dataPosition.Coordinates.X);
        ui->laby->setNum(dataPosition.Coordinates.Y);
        ui->labz->setNum(dataPosition.Coordinates.Z);
        ui->labtx->setNum(dataPosition.Coordinates.ThetaX);
        ui->labty->setNum(dataPosition.Coordinates.ThetaY);
        ui->labtz->setNum(dataPosition.Coordinates.ThetaZ);

    }

}

void MainWindow::on_pb_xp_pressed()
{
    key_state.xp = true;
}

void MainWindow::on_pb_xp_released()
{
    key_state.xp = false;
}

void MainWindow::on_pb_xn_pressed()
{
    key_state.xn = true;
}

void MainWindow::on_pb_xn_released()
{
    key_state.xn = false;
}


void MainWindow::on_pb_yp_pressed()
{
    key_state.yp = true;
}

void MainWindow::on_pb_yp_released()
{
    key_state.yp = false;
}

void MainWindow::on_pb_yn_pressed()
{
    key_state.yn = true;
}

void MainWindow::on_pb_yn_released()
{
    key_state.yn = false;
}


void MainWindow::on_pb_zp_pressed()
{
    key_state.zp = true;
}

void MainWindow::on_pb_zp_released()
{
    key_state.zp = false;
}

void MainWindow::on_pb_zn_pressed()
{
    key_state.zn = true;
}

void MainWindow::on_pb_zn_released()
{
    key_state.zn = false;
}


void MainWindow::on_pb_txp_pressed()
{
    key_state.txp = true;
}

void MainWindow::on_pb_txp_released()
{
    key_state.txp = false;
}

void MainWindow::on_pb_txn_pressed()
{
    key_state.txn = true;
}

void MainWindow::on_pb_txn_released()
{
    key_state.txn = false;
}


void MainWindow::on_pb_typ_pressed()
{
    key_state.typ = true;
}

void MainWindow::on_pb_typ_released()
{
    key_state.typ = false;
}

void MainWindow::on_pb_tyn_pressed()
{
    key_state.tyn = true;
}

void MainWindow::on_pb_tyn_released()
{
    key_state.tyn = false;
}


void MainWindow::on_pb_tzp_pressed()
{
    key_state.tzp = true;
}

void MainWindow::on_pb_tzp_released()
{
    key_state.tzp = false;
}

void MainWindow::on_pb_tzn_pressed()
{
    key_state.tzn = true;
}

void MainWindow::on_pb_tzn_released()
{
    key_state.tzn = false;
}




void MainWindow::on_pb_rmt_clicked(bool checked)
{

    if(checked){
        CartesianPosition dataPosition;
        jacoarm->MyGetCartesianPosition(dataPosition);
        RemotePoseHome.Position.CartesianPosition.X = dataPosition.Coordinates.X;
        RemotePoseHome.Position.CartesianPosition.Y = dataPosition.Coordinates.Y;
        RemotePoseHome.Position.CartesianPosition.Z = dataPosition.Coordinates.Z;
        RemotePoseHome.Position.CartesianPosition.ThetaX = dataPosition.Coordinates.ThetaX;
        RemotePoseHome.Position.CartesianPosition.ThetaY = dataPosition.Coordinates.ThetaY;
        RemotePoseHome.Position.CartesianPosition.ThetaZ = dataPosition.Coordinates.ThetaZ;
        remote_enbaled = true;

        ui->labx_2->setNum(dataPosition.Coordinates.X);
        ui->laby_2->setNum(dataPosition.Coordinates.Y);
        ui->labz_2->setNum(dataPosition.Coordinates.Z);
        ui->labtx_2->setNum(dataPosition.Coordinates.ThetaX);
        ui->labty_2->setNum(dataPosition.Coordinates.ThetaY);
        ui->labtz_2->setNum(dataPosition.Coordinates.ThetaZ);

        ui->pb_rmt_abs->setEnabled(true);
        ui->pb_rmt_rel->setEnabled(true);
        ui->pb_rmt_abs->setChecked(true);
        remote_mode = REMOTE_MODE_ABS;

    }

    else{
        ui->labx_2->clear();
        ui->laby_2->clear();
        ui->labz_2->clear();
        ui->labtx_2->clear();
        ui->labty_2->clear();
        ui->labtz_2->clear();
        remote_enbaled = false;
        ui->pb_rmt_abs->setEnabled(false);
        ui->pb_rmt_rel->setEnabled(false);
    }


}

void MainWindow::on_pb_rmt_abs_clicked(bool checked)
{
    if(checked){
        remote_mode = REMOTE_MODE_ABS;
        ui->pb_rmt_rel->setChecked(false);
    }


}

void MainWindow::on_pb_rmt_rel_clicked(bool checked)
{
    if(checked){
        remote_mode = REMOTE_MODE_REL;
        ui->pb_rmt_abs->setChecked(false);
    }


}
