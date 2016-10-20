#ifndef _Security_H
#define _Security_H
#include "SerialPort.h"

void MsgEx(int nEx, int nID, CSerialPort *m_SerialPort_MSG);
BOOL ExEvent();

UINT ThreadProc_TimeS(LPVOID wParam);
void TimeS_OnPropST();
void ANS_CRC16( char *puchMsg, unsigned char usDataLen);
void ANS_GetStatus();
void TempS_GetTempSorST();
void TempS_GetTempSorTime();
int Lock_GetAuthorRunningDays();
void Lock_GetSMS(CString strRcv);
void Lock_SendSMS();
void Lock_OnPropAuthorRunning(CEdit *pEdit);
void Lock_DayAuthorRunning();
void Lock_SysCanNotRun();
UINT ThreadProc_TIMER_Time(LPVOID wParam);
void Msg_OnProp();
void Msg_OffProp();
UINT ThreadProc_CCSP_Watch(LPVOID wParam);
int Read_WatchFile();
void Write_WatchFile();
DWORD GetVolumeSerial();
CString GetHDSerial();
void InitCS();
int GetCS_Index();
UINT ThreadProc_Msg(LPVOID wParam);
#endif