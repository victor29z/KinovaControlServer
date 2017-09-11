#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "iostream"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int i;
    ui->setupUi(this);
    setWindowTitle(tr("Tcp server"));
    pserver = new Server;
    jacoarm = new Jaco();
    sld = new SliderControl();
    //hand = new bhand("/dev/pcan0");
    arm_control_thread = new arm_control(jacoarm);
    //arm_control_thread->start();

    for(i = 0; i < 6; i++){
        forcedata[i] = 0;
        forceadmit[i] = 0;
    }

    // status varibles initialization

    ftzero.force_x = 0;
    ftzero.force_y = 0;
    ftzero.force_z = 0;
    ftzero.torque_x = 0;
    ftzero.torque_y = 0;
    ftzero.torque_z = 0;

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
    admitance_control_enabled = false;
    key_control_enabled = false;
    move_position_angle = REMOTE_MOVE_NONE;

    slider_speed = 0;
    arm_control_mode = 0;

    timer = new QTimer(this);
    //timer->start(5);
   //connect(pserver,SIGNAL(pserver->data_recieved(pserver->ser)),this,SLOT(data_react()));
    //connect(pserver,SIGNAL(data_recieved(float*)),this,SLOT(data_react(float*)));//parameter should not be ignored
    connect(pserver,SIGNAL(data_recieved(CONTROL_PACK_DEF *)),this,SLOT(data_react(CONTROL_PACK_DEF *)));//parameter should not be ignored
    connect(pserver,SIGNAL(force_data_recieved(float*)),this,SLOT(force_data_react(float*)));//parameter should not be ignored

    connect(timer,SIGNAL(timeout()),this,SLOT(timer_out()));
    connect(ui->pb_home,SIGNAL(clicked(bool)),arm_control_thread,SLOT(gohome()));
    connect(this,SIGNAL(SendPos(CartesianPosition)),arm_control_thread,SLOT(GetPos(CartesianPosition)));
    connect(this,SIGNAL(SendTrajectoryPoint(CartesianPosition)),arm_control_thread,SLOT(AddTrajectoryPoint(CartesianPosition)));
    connect(this,SIGNAL(SendForce(int*)),this->pserver,SLOT(get_forcedata_slot(int*)));
    TrajectoryFile = new QFile("TrajectoryFile.log");
    TrajectoryFile->open(QIODevice::ReadWrite | QIODevice::Text);

    ui_init();
}

MainWindow::~MainWindow()
{
    TrajectoryFile->close();
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

    //arm_control_thread->start();

}

void MainWindow::on_close_pushButton_clicked()//there is no reaction when we click close
{
    jacoarm->MyCloseAPI();
}

void MainWindow::on_exit_pushButton_clicked()
{
    exit(0);
}




void MainWindow::data_react(CONTROL_PACK_DEF *data)
{
    TrajectoryPoint pointToSend;

    //qDebug()<<"recv pose data";
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
            RemotePoseCmd.Position.CartesianPosition.X = RemotePoseHome.Position.CartesianPosition.X - data->pos_data[1] / 2 ;
            RemotePoseCmd.Position.CartesianPosition.Y = RemotePoseHome.Position.CartesianPosition.Y + (data->pos_data[0]-0.14) / 2;
            RemotePoseCmd.Position.CartesianPosition.Z = RemotePoseHome.Position.CartesianPosition.Z + data->pos_data[2] / 2 ;
            RemotePoseCmd.Position.CartesianPosition.ThetaX = RemotePoseHome.Position.CartesianPosition.ThetaX + (data->pos_data[4] + 1.3)  ;
            RemotePoseCmd.Position.CartesianPosition.ThetaY = RemotePoseHome.Position.CartesianPosition.ThetaY + (data->pos_data[5] - 2.1) ;
            RemotePoseCmd.Position.CartesianPosition.ThetaZ = RemotePoseHome.Position.CartesianPosition.ThetaZ + (data->pos_data[3] + 3.8)  ;
        }
        if(remote_mode == REMOTE_MODE_REL){
            RemoteSpeedCmd.Position.CartesianPosition.X = - data->pos_data[1];
            RemoteSpeedCmd.Position.CartesianPosition.Y = data->pos_data[0] - 0.14;
            RemoteSpeedCmd.Position.CartesianPosition.Z = data->pos_data[2];
            RemoteSpeedCmd.Position.CartesianPosition.ThetaX = data->pos_data[4] + 1.3;
            RemoteSpeedCmd.Position.CartesianPosition.ThetaY = data->pos_data[5] - 2.1;
            RemoteSpeedCmd.Position.CartesianPosition.ThetaZ = data->pos_data[3] + 3.8;

        }

        arm_control_mode = data->control_mode;
        slider_speed = data->slider_speed;

        //ui->label_control_mode->setNum(arm_control_mode);

    }


}

void MainWindow::force_data_react(float *data){


    FTData.force_x = data[0];
    FTData.force_y = data[1];
    FTData.force_z = data[2];
    FTData.torque_x = data[3];
    FTData.torque_y = data[4];
    FTData.torque_z = data[5];


    QString tmpstr;

    tmpstr.setNum(((FTData.force_x - ftzero.force_x) * 1.00));
    ui->forceX_show->setText(tmpstr);

    tmpstr.setNum(((FTData.force_y - ftzero.force_y) * 1.00));
    ui->forceY_show->setText(tmpstr);

    tmpstr.setNum(((FTData.force_z - ftzero.force_z)* 1.00));
    ui->forceZ_show->setText(tmpstr);

    tmpstr.setNum(((FTData.torque_x -ftzero.torque_x)* 1.00));
    ui->torqueX_show->setText(tmpstr);

    tmpstr.setNum(((FTData.torque_y -ftzero.torque_y)* 1.00));
    ui->torqueY_show->setText(tmpstr);

    tmpstr.setNum(((FTData.torque_z -ftzero.torque_z) * 1.00));
    ui->torqueZ_show->setText(tmpstr);


    forcedata[0] = (int)(-(FTData.force_z - ftzero.force_z)* 100);
    forcedata[1] = (int)((FTData.force_y - ftzero.force_y)* 100);
    forcedata[2] = (int)((FTData.force_x - ftzero.force_x)* 100);

    forceadmit[1] = (int)((FTData.force_z - ftzero.force_z)* 100);
    forceadmit[0] = (int)((FTData.force_y - ftzero.force_y)* 100);
    forceadmit[2] = (int)((FTData.force_x - ftzero.force_x)* 100);

}



void MainWindow::on_pb_home_clicked()
{
    cout << "Send the robot to HOME position" << endl;
    //jacoarm->MyMoveHome();
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
    AngularPosition angPos;
    jacoarm->MyGetCartesianPosition(dataPosition);
    jacoarm->MyGetAngularPosition(angPos);
    static int i;
    static unsigned int cnt;
    /*
    hand->can_handle.read_loop(false);
    if(cnt++ % 10 == 0)
        on_pb_getFT_clicked();
    */
    emit(SendPos(dataPosition));
    emit(SendForce(forcedata));
    pointToSend.InitStruct();

    //We specify that this point will be used an angular(joint by joint) velocity vector.
    pointToSend.Position.Type = CARTESIAN_VELOCITY;

    switch(arm_control_mode){
    case 1:
        move_position_angle = REMOTE_MOVE_POS;
        break;
    case 2:
        move_position_angle = REMOTE_MOVE_ANG;
        break;
    case 3:
        move_position_angle = REMOTE_MOVE_ALL;
        break;
    case 0:
        move_position_angle = REMOTE_MOVE_NONE;
        break;

    }

    switch(slider_speed){
    case 1:
        sld->move_left();
        break;
    case 2:
        sld->move_left_step();
        break;
    case 3:
        sld->move_right_step();
        break;
    case 4:
        sld->move_right();
        break;

    }

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

        }

        if(key_state.txn){
            pointToSend.Position.CartesianPosition.ThetaX = -0.5;

        }


        if(key_state.typ)
            pointToSend.Position.CartesianPosition.ThetaY = 0.5;
        if(key_state.tyn)
            pointToSend.Position.CartesianPosition.ThetaY = -0.5;

        if(key_state.tzp)
            pointToSend.Position.CartesianPosition.ThetaZ = 0.5;
        if(key_state.tzn)
            pointToSend.Position.CartesianPosition.ThetaZ = -0.5;

        if(key_control_enabled)jacoarm->MySendBasicTrajectory(pointToSend);

        if(admitance_control_enabled){
            if(forceadmit[0] > 550)
                pointToSend.Position.CartesianPosition.X = 0.5;
            if(forceadmit[0] < -550)
                pointToSend.Position.CartesianPosition.X = -0.5;

            if(forceadmit[1] > 550)
                pointToSend.Position.CartesianPosition.Y = 0.5;
            if(forceadmit[1] < -550)
                pointToSend.Position.CartesianPosition.Y = -0.5;

            if(forceadmit[2] > 550)
                pointToSend.Position.CartesianPosition.Z = 0.5;
            if(forceadmit[2] < -550)
                pointToSend.Position.CartesianPosition.Z = -0.5;

            jacoarm->MySendBasicTrajectory(pointToSend);
        }
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
/*
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
*/
        }
        if(remote_mode == REMOTE_MODE_REL)
        {
            if(move_position_angle == REMOTE_MOVE_POS){
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

                pointToSend.Position.CartesianPosition.ThetaX = 0;
                pointToSend.Position.CartesianPosition.ThetaY = 0;
                pointToSend.Position.CartesianPosition.ThetaZ = 0;
            }

            if(move_position_angle == REMOTE_MOVE_ANG){
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

                pointToSend.Position.CartesianPosition.X = 0;
                pointToSend.Position.CartesianPosition.Y = 0;
                pointToSend.Position.CartesianPosition.Z = 0;
            }


        }
        jacoarm->MySendBasicTrajectory(pointToSend);
    }



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

        ui->laba1->setNum(angPos.Actuators.Actuator1);
        ui->laba2->setNum(angPos.Actuators.Actuator2);
        ui->laba3->setNum(angPos.Actuators.Actuator3);
        ui->laba4->setNum(angPos.Actuators.Actuator4);
        ui->laba5->setNum(angPos.Actuators.Actuator5);
        ui->laba6->setNum(angPos.Actuators.Actuator6);

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
        ui->pb_rmt_rel->setChecked(true);
        remote_mode = REMOTE_MODE_REL;

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
        move_position_angle = REMOTE_MOVE_POS;
        ui->pb_movpos->setChecked(true);
        ui->pb_movang->setChecked(false);
    }


}

void MainWindow::on_pb_p1_clicked()
{
    CartesianPosition dataPosition;
    AngularPosition angPos;
    jacoarm->MyGetCartesianPosition(dataPosition);
    jacoarm->MyGetAngularPosition(angPos);

    QTextStream out(TrajectoryFile);

    out << dataPosition.Coordinates.X << ","
        << dataPosition.Coordinates.Y << ","
        << dataPosition.Coordinates.Z << ","
        << dataPosition.Coordinates.ThetaX << ","
        << dataPosition.Coordinates.ThetaY << ","
        << dataPosition.Coordinates.ThetaZ << endl;
/*
    out << angPos.Actuators.Actuator1 << ","
        << angPos.Actuators.Actuator2 << ","
        << angPos.Actuators.Actuator3 << ","
        << angPos.Actuators.Actuator4 << ","
        << angPos.Actuators.Actuator5 << ","
        << angPos.Actuators.Actuator6 << endl;
*/

    qDebug()<<"trajectory point saved";
}

void MainWindow::on_pb_p2_clicked()
{
    CartesianPosition p;

    TrajectoryFile->seek(0);
    QTextStream in(TrajectoryFile);
    while(!in.atEnd()){
    QStringList list = in.readLine().split(',');
    p.Coordinates.X = list[0].toDouble();
    p.Coordinates.Y = list[1].toDouble();
    p.Coordinates.Z = list[2].toDouble();
    p.Coordinates.ThetaX = list[3].toDouble();
    p.Coordinates.ThetaY = list[4].toDouble();
    p.Coordinates.ThetaZ = list[5].toDouble();
/*
    qDebug()<<"send new point"
            << p.Coordinates.X << ","
            << p.Coordinates.Y << ","
            << p.Coordinates.Z << ","
            << p.Coordinates.ThetaX << ","
            << p.Coordinates.ThetaY << ","
            << p.Coordinates.ThetaZ << endl;*/
    emit(SendTrajectoryPoint(p));
/*
    qDebug()<< list[0] << ","
            << list[1] << ","
            << list[2] << ","
            << list[3] << ","
            << list[4] << ","
            << list[5] << endl;

*/
    }
}

void MainWindow::on_pb_p3_clicked()
{

    TrajectoryPoint pointToSend;
    pointToSend.InitStruct();

    //We specify that this point will be used an angular(joint by joint) velocity vector.
    pointToSend.Position.Type = ANGULAR_POSITION;

    pointToSend.Position.Actuators.Actuator1 = 154.06;
    pointToSend.Position.Actuators.Actuator2 = 249.00;
    pointToSend.Position.Actuators.Actuator3 = 85.48;
    pointToSend.Position.Actuators.Actuator4 = 219.69;
    pointToSend.Position.Actuators.Actuator5 = 90.53;
    pointToSend.Position.Actuators.Actuator6 = 138.00;

    jacoarm->MySendBasicTrajectory(pointToSend);

}

void MainWindow::on_pb_p4_clicked()
{

    int i;
    TrajectoryPoint pointToSend;
    pointToSend.InitStruct();

    //We specify that this point will be used an angular(joint by joint) velocity vector.
    pointToSend.Position.Type = CARTESIAN_POSITION;
    //pointToSend.Position.Type = ANGULAR_POSITION;
    TrajectoryFile->seek(0);
    QTextStream in(TrajectoryFile);
    while(!in.atEnd()){
        QStringList list = in.readLine().split(',');

        pointToSend.Position.CartesianPosition.X = list[0].toDouble();
        pointToSend.Position.CartesianPosition.Y = list[1].toDouble();
        pointToSend.Position.CartesianPosition.Z = list[2].toDouble();
        pointToSend.Position.CartesianPosition.ThetaX = list[3].toDouble();
        pointToSend.Position.CartesianPosition.ThetaY = list[4].toDouble();
        pointToSend.Position.CartesianPosition.ThetaZ = list[5].toDouble();
   /*
        pointToSend.Position.Actuators.Actuator1 = list[0].toDouble();
        pointToSend.Position.Actuators.Actuator2 = list[1].toDouble();
        pointToSend.Position.Actuators.Actuator3 = list[2].toDouble();
        pointToSend.Position.Actuators.Actuator4 = list[3].toDouble();
        pointToSend.Position.Actuators.Actuator5 = list[4].toDouble();
        pointToSend.Position.Actuators.Actuator6 = list[5].toDouble();
*/
        jacoarm->MySendBasicTrajectory(pointToSend);
        qDebug()<<"pos"<< i++ <<endl;
    }

/*
    pointToSend.Position.CartesianPosition.X = 0.212346;
    pointToSend.Position.CartesianPosition.Y = -0.256814; //Move along Y axis at 20 cm per second
    pointToSend.Position.CartesianPosition.Z = 0.541686;
    pointToSend.Position.CartesianPosition.ThetaX = 1.65492;
    pointToSend.Position.CartesianPosition.ThetaY = 1.11717;
    pointToSend.Position.CartesianPosition.ThetaZ = 0.122715;

    jacoarm->MySendBasicTrajectory(pointToSend);
    qDebug()<<"pos1"<<endl;
    pointToSend.Position.CartesianPosition.X = 0.212346;
    pointToSend.Position.CartesianPosition.Y = -0.256814; //Move along Y axis at 20 cm per second
    pointToSend.Position.CartesianPosition.Z = 0.576686;
    pointToSend.Position.CartesianPosition.ThetaX = 1.65492;
    pointToSend.Position.CartesianPosition.ThetaY = 1.11717;
    pointToSend.Position.CartesianPosition.ThetaZ = 0.122715;

    jacoarm->MySendBasicTrajectory(pointToSend);
    qDebug()<<"pos2"<<endl;
    pointToSend.Position.CartesianPosition.X = 0.212346;
    pointToSend.Position.CartesianPosition.Y = -0.257814; //Move along Y axis at 20 cm per second
    pointToSend.Position.CartesianPosition.Z = 0.600686;
    pointToSend.Position.CartesianPosition.ThetaX = 1.656;
    pointToSend.Position.CartesianPosition.ThetaY = 1.117;
    pointToSend.Position.CartesianPosition.ThetaZ = 0.124;

    jacoarm->MySendBasicTrajectory(pointToSend);
    qDebug()<<"pos3"<<endl;

*/
}

void MainWindow::on_pb_kctrl_clicked(bool checked)
{
    if(checked){

        key_control_enabled = true;

    }

    else{
        key_control_enabled = false;
    }


}

void MainWindow::on_pb_bhand_init_clicked()
{

    //hand->barett_hand_init();
    //hand->axisForce_init();

}
void MainWindow::on_pb_setPos_1_clicked()
{
    //hand->setJointPosition(1.068,1.068,1.068,3.126);
}

void MainWindow::on_pb_setPos_2_clicked()
{
    //hand->setJointPosition(0,0,0,3.126);
}

void MainWindow::on_pb_setPos_3_clicked()
{
    //hand->setJointPosition(2.442,2.442,2.442,3.126);
}

void MainWindow::on_pb_getPos_clicked()
{
    float pos;
    QString tmpstr;
/*

    pos = hand->enc_to_rad(hand->get_position(FINGER1));
    tmpstr.setNum(pos);
    ui->f1_pos->setText(tmpstr);
    pos = hand->enc_to_rad(hand->get_position(FINGER2));
    tmpstr.setNum(pos);
    ui->f2_pos->setText(tmpstr);
    pos = hand->enc_to_rad(hand->get_position(FINGER3));
    tmpstr.setNum(pos);
    ui->f3_pos->setText(tmpstr);
    pos = hand->enc_to_rad(hand->get_position(SPREAD),SPREAD_TYPE);
    tmpstr.setNum(pos);
    ui->fs_pos->setText(tmpstr);
    */
}

void MainWindow::on_pb_setPos_clicked()
{
    float pos1,pos2,pos3,pos4;
    QString tmpstr;
/*
    pos1 = ui->f1_pos->text().toFloat();
    pos2 = ui->f2_pos->text().toFloat();
    pos3 = ui->f3_pos->text().toFloat();
    pos4 = ui->fs_pos->text().toFloat();
    hand->setJointPosition(pos1,pos2,pos3,pos4);
*/
}

void MainWindow::on_pb_getFT_clicked()
{

    axisSensorData force_data; //= hand->get_AxisForce(AxisForceQuery);
    axisSensorData torque_data; //= hand->get_AxisTorque(AxisTorqueQuery);
    FTData.force_x = force_data.force_x;
    FTData.force_y = force_data.force_y;
    FTData.force_z = force_data.force_z;
    FTData.torque_x = torque_data.torque_x ;
    FTData.torque_y = torque_data.torque_y ;
    FTData.torque_z = torque_data.torque_z ;


    QString tmpstr;

    tmpstr.setNum(((force_data.force_x - ftzero.force_x) * 1.00));
    ui->forceX_show->setText(tmpstr);

    tmpstr.setNum(((force_data.force_y - ftzero.force_y) * 1.00));
    ui->forceY_show->setText(tmpstr);

    tmpstr.setNum(((force_data.force_z - ftzero.force_z)* 1.00));
    ui->forceZ_show->setText(tmpstr);

    tmpstr.setNum(((torque_data.torque_x -ftzero.torque_x)* 1.00));
    ui->torqueX_show->setText(tmpstr);

    tmpstr.setNum(((torque_data.torque_y -ftzero.torque_y)* 1.00));
    ui->torqueY_show->setText(tmpstr);

    tmpstr.setNum(((torque_data.torque_z -ftzero.torque_z) * 1.00));
    ui->torqueZ_show->setText(tmpstr);


    forcedata[0] = (int)(-(force_data.force_z - ftzero.force_z)* 100);
    forcedata[1] = (int)((force_data.force_y - ftzero.force_y)* 100);
    forcedata[2] = (int)((force_data.force_x - ftzero.force_x)* 100);

    forceadmit[1] = (int)((force_data.force_z - ftzero.force_z)* 100);
    forceadmit[0] = (int)((force_data.force_y - ftzero.force_y)* 100);
    forceadmit[2] = (int)((force_data.force_x - ftzero.force_x)* 100);
}


void MainWindow::on_pb_ft_zero_clicked()
{
    ftzero = FTData;
}

void MainWindow::on_pb_tim_clicked()
{

}

void MainWindow::on_pb_tim_clicked(bool checked)
{
    if(checked){
       timer->start(5);
    }
    else
        timer->stop();

}

void MainWindow::on_pb_kctrl_2_clicked(bool checked)
{
    if(checked){
       admitance_control_enabled = true;
    }
    else
        admitance_control_enabled = false;
}

void MainWindow::on_pb_movpos_clicked(bool checked)
{

    if(checked){
       move_position_angle = REMOTE_MOVE_POS;
       ui->pb_movang->setChecked(false);
    }


}

void MainWindow::on_pb_movang_clicked(bool checked)
{
    if(checked){
        move_position_angle = REMOTE_MOVE_ANG;
        ui->pb_movpos->setChecked(false);
    }


}

void MainWindow::ui_init(void)
{
    // Example use QSerialPortInfo

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();

        // Example use QSerialPort
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            ui->comboBox->addItem(info.portName());
            serial.close();
        }
    }
}

void MainWindow::on_pushButton_cmopen_clicked()
{
    QString rawname = ui->comboBox->currentText();
    rawname = "/dev/" + rawname;
    qDebug()<<rawname;
    //QByteArray dev = ui->comboBox->currentText().toLatin1();

    unsigned int baud = ui->lineEdit->text().toInt();
    sld->serial_device = rawname.toLatin1();
    sld->serial_baudrate = baud;
    sld->modbus_init();
    ui->pushButton_comclose->setEnabled(true);
    ui->pushButton_cmopen->setEnabled(false);
    ui->pbReadSldPos->setEnabled(true);
    ui->pbSldStop->setEnabled(true);
    ui->pbBackFast_2->setEnabled(true);
    ui->pbBackStep_2->setEnabled(true);
    ui->pbFWDFast_2->setEnabled(true);
    ui->pbFWDStep_2->setEnabled(true);
}

void MainWindow::on_pbReadSldPos_clicked()
{
    sld->get_pose();
}

void MainWindow::on_pbSldStop_clicked()
{
    sld->stopsld();
}

void MainWindow::on_pbBackFast_2_clicked()
{
    sld->move_left();
}

void MainWindow::on_pbBackStep_2_clicked()
{
    sld->move_left_step();
}

void MainWindow::on_pbFWDStep_2_clicked()
{
    sld->move_right_step();
}

void MainWindow::on_pbFWDFast_2_clicked()
{
    sld->move_right();
}

