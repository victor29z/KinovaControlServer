#ifndef SERVER_H
#define SERVER_H
#include<QDialog>
#include<QPushButton>
#include<QTextEdit>
#include<QGridLayout>
#include<QTcpSocket>
#include<QTcpServer>
#include<QHostAddress>
#include <QUdpSocket>

#define PACK_SET_POSE    0xf1
#define PACK_SET_MODE    0xf2
#define PACK_SET_CAM     0xf3

typedef struct{

    float pos_data[6];
    int slider_speed;
    int control_mode;

}CONTROL_PACK_DEF;

class Server : public QDialog
{
    Q_OBJECT
public:
    Server();

    float ser[6];
    float handle_position[6];
    float force_data_trans[6];
    int hand_data[8];
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QUdpSocket * udpSocket;
    QUdpSocket * udpforce;
    CONTROL_PACK_DEF control_pack;
public slots:

    void recv_slot();
    void udp_recv();
    void udp_force_recv();
    void connect_slot();
    void get_forcedata_slot(int * forcedata);
signals:
    void data_recieved(float *);
    void data_recieved(int *);
    void data_recieved(CONTROL_PACK_DEF *);
    void force_data_recieved(float *);
    void data_send();
};


#endif // SERVER_H
