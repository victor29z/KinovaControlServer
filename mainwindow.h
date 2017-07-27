#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"server.h"
#include"jaco.h"
#include <QTimer>
#include "arm_control.h"
#include <QFile>

namespace Ui {
class MainWindow;
}
#define REMOTE_MODE_ABS 1
#define REMOTE_MODE_REL 2
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

    void data_react(float* data);

    void data_sendback();

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

private:
    Ui::MainWindow *ui;
    Server *pserver;
    Jaco *jacoarm;
    QTimer *timer;
    KEY_STATE_DEF key_state;
    bool remote_enbaled;
    int remote_mode;
    TrajectoryPoint RemotePoseCmd;
    TrajectoryPoint RemoteSpeedCmd;
    TrajectoryPoint RemotePoseHome;

    arm_control *arm_control_thread;

    QFile *TrajectoryFile;

signals:
    void SendPos(CartesianPosition pos);
    void SendTrajectoryPoint(CartesianPosition p);
};


#endif // MAINWINDOW_H
