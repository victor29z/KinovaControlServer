#include "slidercontrol.h"
#include <QDebug>
#include <QTime>
SliderControl::SliderControl()
{

}


void SliderControl::modbus_init(void){
    mb = modbus_new_rtu(serial_device,serial_baudrate,'N',8,2);
    qDebug()<<"com open:"<<serial_device<<",baud rate:"<<serial_baudrate;
    modbus_set_slave(mb,MB_SLAVE_ADDR);
    modbus_connect(mb);

    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 1500000;
    modbus_set_response_timeout(mb,&t);



}
void SliderControl::set_pos(int pos){

}
void SliderControl::set_pose_zero(void){

}
int SliderControl::get_pose(void){
    uint16_t tab_reg[32]={0};
    int ret = modbus_read_registers(mb, 0x0520, 2, tab_reg);

    int pos = tab_reg[1]<<16 | tab_reg[0];
    QTime time = QTime::currentTime();
    qDebug() << time.toString("H:m:s a")<< "read status" << ret;
    qDebug() << "pos" << pos;
    //ui->lcdpos->display(pos);
    return pos;
}
void SliderControl::move_left_step(void){
    uint16_t sendbuf[2];
    sendbuf[0] = 6;
    sendbuf[1] = 0;
    int ret = modbus_write_registers(mb,0x050e,2,sendbuf);
    QTime time = QTime::currentTime();
    qDebug() << time.toString("H:m:s a")<< "read status" << ret;
}
void SliderControl::move_right_step(void){
    uint16_t sendbuf[2];
    sendbuf[0] = 5;
    sendbuf[1] = 0;
    int ret = modbus_write_registers(mb,0x050e,2,sendbuf);
    QTime time = QTime::currentTime();
    qDebug() << time.toString("H:m:s a")<< "read status" << ret;
}
void SliderControl::move_left(void){
    uint16_t sendbuf[2];
    sendbuf[0] = 4;
    sendbuf[1] = 0;
    modbus_write_registers(mb,0x050e,2,sendbuf);
}
void SliderControl::move_right(void){
    uint16_t sendbuf[2];
    sendbuf[0] = 3;
    sendbuf[1] = 0;
    modbus_write_registers(mb,0x050e,2,sendbuf);
}
void SliderControl::stopsld(void){
    uint16_t sendbuf[2];
    sendbuf[0] = 1000;
    sendbuf[1] = 0;
    modbus_write_registers(mb,0x050e,2,sendbuf);

}
