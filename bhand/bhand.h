#ifndef BHAND_H
#define BHAND_H
#include "pcan_operate.h"
#include <string>

#define FINGER1  11
#define FINGER2  12
#define FINGER3  13
#define SPREAD  14

#define BASE_TYPE  0
#define TIP_TYPE  1
#define SPREAD_TYPE  2
#define BASE_LIMIT  140.0
#define TIP_LIMIT  48.0
#define SPREAD_LIMIT  180.0

#define MAX_ENCODER_TICKS 195000.0
#define MAX_SPREAD_TICKS 36000.0
#define MAX_FINGERTIP_TICKS 78000.0

#define V  44
#define TSTOP  78
#define MODE  8
#define MODE_VEL 4

#define FTSENSORID 8
#define FTDATAPROPERTY 54

#define F1_POSITION   0x166
#define F2_POSITION   0x186
#define F3_POSITION   0x1a6
#define SPREAD_POSITION   0x1c6

#define AxisForceQuery 8
#define AxisTorqueQuery 8
#define AxisForce   0x50a
#define AxisTorque  0x50b

struct axisSensorData
{
    float force_x;
    float force_y;
    float force_z;
    float torque_x;
    float torque_y;
    float torque_z;
};

class bhand
{
public:
    bhand(std::string dev_name);

    DWORD send_msg(int msgID,unsigned int data[],int len,unsigned int delay_us=0);
    void barett_hand_init();

    void setJointPosition(float f1_pos_f,float f2_pos_f,float f3_pos_f,float sp_pos_f);
    //bhand velocity control:function declaration
    int rad_to_enc(float rad,int type=BASE_TYPE);
    float enc_to_rad(uint32_t enc,int type=BASE_TYPE);
    void check_error(DWORD result,std::string location_of_error);

    void set_16(int msgID,int propID,int tmp_value);
    void set_32(int msgID,int propID,int tmp_value);
    void set_property(int msgID,int propID,int value);
    void set_velocity(int puckID,int velocity);
    void setJointVelocity(int finger,float rad_s);
    uint32_t get_position(int msgID,int depth=1);
    //float get_velocity(int msgID);
    //bool wait_act_finish();

    //6 axis force handle
    void axisForce_init();
    int wavePuck(int bus,int id);
    int compile(
                int property        /** The property being compiled (use the enumerations in btcan.h) */,
                long longVal        /** The value to set the property to */,
                unsigned char *data /** A pointer to a character buffer in which to build the data payload */,
                int *dataLen        /** A pointer to the total length of the data payload for this packet */,
                int isForSafety        /** A flag indicating whether this packet is destined for the safety circuit or not */);
        int setPropertySlow(int bus, int id, int property, int verify, long value);
    axisSensorData get_AxisForce(int force_id,int depth=1);
    void displayAxisSensordata (axisSensorData axisData);
    void displayAxisForceData (axisSensorData axisData);
    void displayAxisTorqueData (axisSensorData axisData);
    axisSensorData get_AxisTorque(int torque_id,int depth=1);
public:
    pcan_operate can_handle;
    HANDLE pcan_handle =NULL;//void *pcan_handle
    axisSensorData axisSensor;
};

#endif // BHAND_H
