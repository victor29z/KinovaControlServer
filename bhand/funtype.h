#ifndef FUNTYPE_H
#define FUNTYPE_H
#include "libpcan.h"
typedef DWORD   (*funCAN_Init_TYPE)(HANDLE hHandle, WORD wBTR0BTR1, int nCANMsgType);
typedef HANDLE  (*funLINUX_CAN_Open_TYPE)(const char *szDeviceName, int nFlag);
typedef DWORD   (*funCAN_VersionInfo_TYPE)(HANDLE hHandle, LPSTR lpszTextBuff);
typedef DWORD   (*funCAN_Close_TYPE)(HANDLE hHandle);
typedef DWORD   (*funLINUX_CAN_Read_TYPE)(HANDLE hHandle, TPCANRdMsg* pMsgBuff);
typedef DWORD   (*funCAN_Status_TYPE)(HANDLE hHandle);
typedef int     (*funnGetLastError_TYPE)(void);
typedef DWORD   (*funCAN_Write_TYPE)(HANDLE hHandle, TPCANMsg* pMsgBuff);

#endif // FUNTYPE_H

