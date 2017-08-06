#include "server.h"
#include<iostream>
#include<QDebug>
#include<QByteArray>
using namespace  std;
char hostaddr[20] = "192.168.3.7";

Server::Server()
{

    tcpServer = new QTcpServer();
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any,18888);

    udpforce = new QUdpSocket(this);
    udpforce->bind(QHostAddress::Any,18889);

    //tcpServer->listen(QHostAddress::Any,43578);

    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(connect_slot()));
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(udp_recv()));
    connect(udpforce,SIGNAL(readyRead()),this,SLOT(udp_force_recv()));
    //connect(sendButton,SIGNAL(clicked()),this,SLOT(send_slot()));
    //connect(exitButton,SIGNAL(clicked()),this,SLOT(close()));
    ser[0] = 0;
    ser[1] = 0;
    ser[2] = 0;
    ser[3] = 0;
    ser[4] = 0;
    ser[5] = 0;

}

void Server::connect_slot()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(recv_slot()));
}

/*void Server::send_slot()
{
    QString str = textEdit->toPlainText();
    tcpSocket->write(qPrintable(str));
}*/

void Server::recv_slot()
{
    int i;
    //QByteArray byte;
    //byte = tcpSocket->readAll();
    //textEdit->setPlainText(QString(byte));
    //cout << byte << endl;
    tcpSocket->read((char *)hand_data,sizeof(int) * 6);
    /*
    switch(tcp_pack.pack_type ){
    case PACK_GET_DAT:
        emit data_send();
        break;
    case PACK_SET_DAT:
        emit data_recieved(tcp_pack.data);
        break;

    }*/
    for(i = 0; i < 6; i++)
        ser[i] = hand_data[i] / 1000000.0f;
    emit data_recieved(ser);

    /*for(int i=0;i<6;i++)
    {
        qDebug() << ser[i];
    }*/

}

void Server::udp_recv(){
    int i;
    udpSocket->readDatagram((char *)hand_data,sizeof(int) * 6);
    for(i = 0; i < 6; i++)
        ser[i] = hand_data[i] / 1000000.0f;

    emit data_recieved(ser);



}

void Server::udp_force_recv(){
    int i;
    int force_data[6];
    udpforce->readDatagram((char *)force_data,sizeof(int) * 6);
    for(i = 0; i < 6; i++)
        force_data_trans[i] = force_data[i] / 100.0f;
    //qDebug()<<"forcedata received"<<endl;
    emit force_data_recieved(force_data_trans);



}

void Server::get_forcedata_slot(int * forcedata){
    udpSocket->writeDatagram((char*)forcedata , sizeof(int) * 6, QHostAddress(hostaddr),18888);
/*
    qDebug()<<"send force data:"
           <<forcedata[0]<<","
           <<forcedata[1]<<","
           <<forcedata[2]<<","
           <<forcedata[3]<<","
           <<forcedata[4]<<","
           <<forcedata[5]<<endl;
*/
}
