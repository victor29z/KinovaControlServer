#ifndef SLIDERCONTROL_H
#define SLIDERCONTROL_H

#include <modbus/modbus.h>
#include <QString>
#include <QByteArray>

#define MB_SLAVE_ADDR 0x01
class SliderControl
{
public:
    SliderControl();
    QByteArray serial_device;
    unsigned int serial_baudrate;
    void modbus_init(void);
    void set_pos(int pos);
    void set_pose_zero(void);
    int get_pose(void);
    void move_left_step(void);
    void move_right_step(void);
    void move_left(void);
    void move_right(void);
    void stopsld(void);

private:
    modbus_t *mb;

};

#endif // SLIDERCONTROL_H
