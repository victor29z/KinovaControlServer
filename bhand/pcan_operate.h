#ifndef PCAN_HANDLE_H
#define PCAN_HANDLE_H
#include <QLibrary>
#include "funtype.h"
#include <QString>
#include <string>
#include <errno.h>
#include <signal.h>
#include <libpcan.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
class pcan_operate
{
public:
    pcan_operate();
    bool pcan_init(std::string dev_name);
    int read_loop(bool display_on);
    DWORD send_msg(int msgID, unsigned int data[], int len, unsigned int delay_us);
    void check_error(__u32 result, std::string location_of_error);
    void print_message(TPCANMsg *m);
    void print_message_ex(TPCANRdMsg *mr);
    ~pcan_operate();

    bool InitState;

    QLibrary * libpcan;
    funCAN_Init_TYPE        funCAN_Init;
    funLINUX_CAN_Open_TYPE  funLINUX_CAN_Open;
    funCAN_VersionInfo_TYPE funCAN_VersionInfo;
    funCAN_Close_TYPE       funCAN_Close;
    funLINUX_CAN_Read_TYPE  funLINUX_CAN_Read;
    funCAN_Status_TYPE      funCAN_Status;
    funnGetLastError_TYPE   funnGetLastError;
    funCAN_Write_TYPE       funCAN_Write;

    HANDLE pcan_handle =NULL;//void *pcan_handle
};

#endif // PCAN_HANDLE_H
