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

#define PACK_GET_DAT    0x1
#define PACK_SET_DAT    0x2
#define PACK_SENDBACK   0x3
typedef struct{
    int pack_type;
    float data[6];

}TCP_PACK_DEF;

class Server : public QDialog
{
    Q_OBJECT
public:
    Server();

    float ser[6];
    int hand_data[6];
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QUdpSocket * udpSocket;
    TCP_PACK_DEF tcp_pack;
public slots:

    void recv_slot();
    void udp_recv();
    void connect_slot();
    void get_forcedata_slot(int * forcedata);
signals:
    void data_recieved(float *);
    void data_recieved(int *);
    void data_send();
};


#endif // SERVER_H
