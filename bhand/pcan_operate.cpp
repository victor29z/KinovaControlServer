#include "pcan_operate.h"
#include <QDebug>


#define DEFAULT_NODE "/dev/pcan32"

pcan_operate::pcan_operate()
{
    InitState = false;

    libpcan = new QLibrary("libpcan.so");
    //We load the functions from the library
    if(libpcan->load())
    {
        qDebug()<<"libpcan.so load succeed!";

        funCAN_Init         =(funCAN_Init_TYPE)libpcan->resolve("CAN_Init");
        funLINUX_CAN_Open   =(funLINUX_CAN_Open_TYPE)libpcan->resolve("LINUX_CAN_Open");
        funCAN_Close        =(funCAN_Close_TYPE)libpcan->resolve("CAN_Close");
        funCAN_VersionInfo  =(funCAN_VersionInfo_TYPE)libpcan->resolve("CAN_VersionInfo");
        funLINUX_CAN_Read   =(funLINUX_CAN_Read_TYPE)libpcan->resolve("LINUX_CAN_Read");
        funCAN_Status       =(funCAN_Status_TYPE)libpcan->resolve("CAN_Status");
        funnGetLastError    =(funnGetLastError_TYPE)libpcan->resolve("nGetLastError");
        funCAN_Write        =(funCAN_Write_TYPE)libpcan->resolve("CAN_Write");
    }

    if((funCAN_Init == NULL) || (funLINUX_CAN_Open == NULL) || (funCAN_Close == NULL) ||
       (funCAN_VersionInfo == NULL) || (funLINUX_CAN_Read == NULL) || (funCAN_Status == NULL) ||
            (funnGetLastError == NULL) || (funCAN_Write == NULL))
    //if((MyInitAPI == NULL)
    {
        qDebug()<<"function resolve falied!";
        InitState = true;
    }
    else
        InitState = false;
}
/*
Notes : Get handle of USB-pcan,and set some parameter of CAN bus.
Input : the device name,you can get it use commands "ls /dev/pcan*".the default name:"/dev/pcan0".
Output: None.
*/
bool pcan_operate::pcan_init(std::string dev_name)
{
    char txt[VERSIONSTRING_LEN];            //store information of can version
    unsigned short wBTR0BTR1 = CAN_BAUD_1M; //set the communicate baud rate of can bus
    int nExtended = CAN_INIT_TYPE_ST;       //set can message int standard model
    const char  *szDevNode = DEFAULT_NODE;  //define const pointer point to device name

    if(dev_name.c_str() !="")
        pcan_handle =funLINUX_CAN_Open(dev_name.c_str(), O_RDWR | O_NONBLOCK);
    else
    {
        pcan_handle =funLINUX_CAN_Open(szDevNode, O_RDWR | O_NONBLOCK);
        dev_name=DEFAULT_NODE;
    }

    //judge whether the call is success.if pcan_handle=null,the call would be failed
    if(pcan_handle){
        printf("CAN Bus test: %s have been opened\n", szDevNode);
        errno = funCAN_VersionInfo(pcan_handle, txt);
        if (!errno)
            printf("CAN Bus test: driver version = %s\n", txt);
        else {
            perror("CAN Bus test: CAN_VersionInfo()");
        }
        if (wBTR0BTR1) {
                errno = funCAN_Init(pcan_handle, wBTR0BTR1, nExtended);
                if (errno) {
                    perror("CAN Bus test: CAN_Init()");
                }
                else
                    printf("Device Info: %s; CAN_BAUD_1M; CAN_INIT_TYPE_ST\n", szDevNode);
            }
    }
    else
        printf("CAN Bus test: can't open %s\n", szDevNode);
}

/*
Notes : read data of can bus, you can add extra data monitor process in this function.
      : and for some special unclear reason, it need to be added to the main loop.
Input : when display_on=true, this function will print all the data read on can bus.
Output: none.
*/
int pcan_operate::read_loop(bool display_on)
{

    TPCANRdMsg m;
    __u32 status;

    if (funLINUX_CAN_Read(pcan_handle, &m)) {
        //perror("receivetest: LINUX_CAN_Read()");
        return errno;
    }

    // check if a CAN status is pending
    if (m.Msg.MSGTYPE & MSGTYPE_STATUS) {
        status = funCAN_Status(pcan_handle);
        if ((int)status < 0) {
            errno = funnGetLastError();
            perror("receivetest: CAN_Status()");
            return errno;
        }

        printf("receivetest: pending CAN status 0x%04x read.\n",
               (__u16)status);
    }

    if(display_on)
    {
        print_message_ex(&m);
    }
    //do extra processing if you need. such as monitor other data on can bus.

}

/*
 Notes : use CAN to send a message
 Input : CAN id, data array, length of the data, delay some micro-seconds when data is sended.
Output : the state of CAN write.
Example: unsigned int data[]={0x30};
       : send_msg(msgID+0xc0,data,sizeof(data)/sizeof(data[0]),9000);
*/
DWORD pcan_operate::send_msg(int msgID, unsigned int data[], int len, unsigned int delay_us)
{
    TPCANMsg msg;
    msg.LEN =len;
    msg.MSGTYPE =MSGTYPE_STANDARD;
    for(int i=0;i<len;i++)
    {
        msg.DATA[i]=data[i];
    }
    msg.ID = msgID;
    DWORD stat=funCAN_Write(pcan_handle,&msg);
    check_error(stat,"write");
    //print_message(&msg);
    usleep(delay_us);
    return stat;
}

/*
Notes : check whether the can send/write/...  is success or not.
Input : state of can handle,description.
Output: none.
*/
void pcan_operate::check_error(__u32 result, std::string location_of_error)
{
    if(result==CAN_ERR_OK)
        return;
    else
    {
        printf("CAN Error Number:%x, while attempting to %s ",result,location_of_error.c_str());
    }
}

/*
Notes : print a can format message.
Input : meaasge.
Output: none.
*/
void pcan_operate::print_message(TPCANMsg *m)
{
    int i;

    //print RTR, 11 or 29, CAN-Id and datalength
    printf("receivetest: %c %c 0x%08x %1d ",
            ((m->MSGTYPE & MSGTYPE_RTR) ? 'r' : 'm') -
                ((m->MSGTYPE ) ? 0x20 : 0),
            (m->MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
             m->ID,
             m->LEN);

    //don't print any telegram contents for remote frames
    if (!(m->MSGTYPE & MSGTYPE_RTR))
        for (i = 0; i < m->LEN; i++)
            printf("%02x ", m->DATA[i]);
          //printf("%3d ", m->DATA[i]);//decimal format print.
    printf("\n");
}

/*
Notes : print a can message read on can bus.
Input : can message read on can bus.
Output: none.
*/
void pcan_operate::print_message_ex(TPCANRdMsg *mr)
{
    printf("%u.%3u ", mr->dwTime, mr->wUsec);
    print_message(&mr->Msg);
}

pcan_operate::~pcan_operate()
{
    funCAN_Close(pcan_handle);


}

