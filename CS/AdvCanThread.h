#ifndef _AdvCanThread_H
#define _AdvCanThread_H

#include "AdvCANIO.h"

typedef struct CAN_Send	//¿‰ø‚Ω·ππÃÂ
{
	DWORD	dwAimId;
	int		nFlagStandard;
	int		nSendLength;
	BYTE	aSendData[8];
}CAN_Send_t;

/*
UINT ThreadWrite(LPVOID param);
UINT ThreadRead(void *param);
void CAN1_OpenClose();
void CAN_Send();
void CanRsv();
*/
UINT CAN_ThreadWrite(LPVOID param);
UINT CAN1_ThreadRead(void *param);
UINT CAN2_ThreadRead(void *param);
void CAN_Open(CString strCan, HANDLE &hCan, BOOL &bCanOpened);
void CAN_Close();
void CAN_Send();
void CAN1_Rcv();
void CAN2_Rcv();

#endif
