#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"server.h"
#include"jaco.h"
#include <QTimer>
#include "arm_control.h"
#include <QFile>
#include "bhand/bhand.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include "slidercontrol.h"

namespace Ui {
class MainWindow;
}
#define REMOTE_MODE_ABS 1
#define REMOTE_MODE_REL 2

#define REMOTE_MOVE_NONE 0
#define REMOTE_MOVE_POS 1
#define REMOTE_MOVE_ANG 2
#define REMOTE_MOVE_ALL 3
typedef struct{
    bool xp;
    bool xn;
    bool yp;
    bool yn;
    bool zp;
    bool zn;
    bool txp;
    bool txn;
    bool typ;
    bool tyn;
    bool tzp;
    bool tzn;

}KEY_STATE_DEF;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
//public slots:
   // void data_react(int*);

private slots:
    void on_init_pushButton_clicked();

    void on_close_pushButton_clicked();

    void on_exit_pushButton_clicked();

   // void on_confirm_pushButton_clicked();

    void data_react(CONTROL_PACK_DEF * data);
    void force_data_react(float* data);



    void on_pb_home_clicked();

    void on_pb_move_clicked();

    void on_pb_getp_clicked();

    void on_pb_xp_pressed();

    void on_pb_xp_released();

    void timer_out();

    void on_pb_xn_pressed();

    void on_pb_xn_released();

    void on_pb_yp_pressed();

    void on_pb_yn_pressed();

    void on_pb_zp_pressed();

    void on_pb_zp_released();

    void on_pb_zn_pressed();

    void on_pb_zn_released();

    void on_pb_txp_pressed();

    void on_pb_txp_released();

    void on_pb_txn_pressed();

    void on_pb_txn_released();

    void on_pb_typ_pressed();

    void on_pb_typ_released();

    void on_pb_tyn_pressed();

    void on_pb_tyn_released();

    void on_pb_tzp_pressed();

    void on_pb_tzp_released();

    void on_pb_tzn_pressed();

    void on_pb_tzn_released();

    void on_pb_yp_released();

    void on_pb_yn_released();

    void on_pb_rmt_clicked(bool checked);

    void on_pb_rmt_abs_clicked(bool checked);

    void on_pb_rmt_rel_clicked(bool checked);

    void on_pb_p1_clicked();

    void on_pb_p2_clicked();

    void on_pb_p3_clicked();

    void on_pb_p4_clicked();

    void on_pb_kctrl_clicked(bool checked);

    void on_pb_bhand_init_clicked();

    void on_pb_setPos_1_clicked();

    void on_pb_setPos_2_clicked();

    void on_pb_setPos_3_clicked();

    void on_pb_getPos_clicked();

    void on_pb_getFT_clicked();

    void on_pb_setPos_clicked();

    void on_pb_ft_zero_clicked();

    void on_pb_tim_clicked();

    void on_pb_tim_clicked(bool checked);

    void on_pb_kctrl_2_clicked(bool checked);

    void on_pb_movpos_clicked(bool checked);

    void on_pb_movang_clicked(bool checked);

    void on_pushButton_cmopen_clicked();

    void on_pbReadSldPos_clicked();

    void on_pbSldStop_clicked();


    void on_pbBackFast_2_clicked();

    void on_pbBackStep_2_clicked();

    void on_pbFWDStep_2_clicked();

    void on_pbFWDFast_2_clicked();

private:
    Ui::MainWindow *ui;
    Server *pserver;
    Jaco *jacoarm;
    SliderControl *sld;

    QTimer *timer;
    KEY_STATE_DEF key_state;
    bool remote_enbaled;
    bool key_control_enabled;
    int remote_mode;
    TrajectoryPoint RemotePoseCmd;
    TrajectoryPoint RemoteSpeedCmd;
    TrajectoryPoint RemotePoseHome;

    arm_control *arm_control_thread;

    QFile *TrajectoryFile;

    int forcedata[6];
    int forceadmit[6];

    bhand* hand;
    axisSensorData FTData;
    axisSensorData  ftzero;

    int slider_speed;
    int arm_control_mode;

    bool admitance_control_enabled;
    int move_position_angle;
    void ui_init(void);
signals:
    void SendPos(CartesianPosition pos);
    void SendForce(int* forcedata);
    void SendTrajectoryPoint(CartesianPosition p);
};


#endif // MAINWINDOW_H
