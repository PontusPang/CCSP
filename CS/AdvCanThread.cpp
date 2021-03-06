#include "stdafx.h"
#include "CS.h"
#include "AdvCanThread.h"
#include "CANDlg.h"
#include "Security.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCSApp theApp;
extern CCANDlg *g_pCanDlg;

extern int g_nIDEx;
extern int g_nMsgEx;

extern BOOL g_bFlagSimCAN;
extern BOOL g_bFlagSimTEMP;

extern Cid_t *g_pStCid;
extern BYTE g_byCidSum_SetKu;

extern BYTE g_byCAN12ID;			//下位机ID>100，与CAN2相连	151201
extern BOOL g_bCAN2_InUse;			//是否使用CAN2

CWinThread* p_ThreadWrite = NULL;
CWinThread* p_ThreadRead = NULL;
HANDLE g_hThreadRead = NULL;

/*****************************************************************************
 DESCRIPTION: Run flag of Thread 
*****************************************************************************/
BOOL	g_bExitReadThreadFlag = FALSE;

BOOL    g_bOpened_CAN1 = FALSE;
BOOL    g_bOpened_CAN2 = FALSE;

/*****************************************************************************
 DESCRIPTION: Parameter of data transfer 
*****************************************************************************/
int		g_nBaud = 100;				//波特率100k

int     g_nFilterMode = 0;			//双滤波器技术，滤波器2
DWORD	g_dwAcceptCode = 0xAAAA002A;//接收缓冲器，当前ID为1
DWORD   g_dwAcceptMask = 0x00000000;//屏蔽缓冲器

HANDLE	g_hDevice_CAN1 = NULL;
HANDLE	g_hDevice_CAN2 = NULL;

/*****************************************************************************
 DESCRIPTION: Send msg info 
*****************************************************************************/
DWORD	g_dwCobId = 0;				//目标节点ID
int		g_SendFlag = 0;				//标准数据格式
int		g_SendLength = 4;			//指令长度
BYTE	g_SendData[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
BOOL	g_bExeTrue = FALSE;			//执行成功标志
BOOL	g_bCxTrue = FALSE;			//执行成功标志

/*****************************************************************************
 DESCRIPTION: Rcv msg info 
*****************************************************************************/
DWORD	g_RcvFlag = 0;
DWORD   g_RcvCobId = 0;				//接收到ID
DWORD	g_RcvLength = 4;
BYTE	g_RcvData[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
BYTE	g_RcvData_Exe[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

//151201
UINT CAN1_ThreadRead(void *param)
{
	g_RcvFlag = 0;
	g_RcvCobId = 0;
	g_RcvLength = 0;
	memset(g_RcvData, 0xFF, sizeof(g_RcvData));
	
	DWORD	dwRead	=0;
	int nRcvTrue = 0;
	BYTE	byBuf[100];
	OVERLAPPED ov;

	DWORD	dwError = 0;

	while(g_bExitReadThreadFlag)
	{
		ZeroMemory(byBuf, 100);
		CanMsg_t *pBuf = (CanMsg_t *)byBuf;
		memset(&ov, 0, sizeof(OVERLAPPED));
		ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//,手动复位,无信号,[自动复位时间，waitforsingleobject不返回]

		nRcvTrue = acCanRead(g_hDevice_CAN1, pBuf, 1, &dwRead, &ov);
		CloseHandle(ov.hEvent);

		if(nRcvTrue == SUCCESS || nRcvTrue == TIME_OUT)
		{
			if( dwRead > 0 )
			{
				g_RcvFlag		= pBuf->flags;
				g_RcvCobId		= pBuf->id;
				g_RcvLength		= pBuf->length;
				memcpy(g_RcvData, pBuf->data, 8);

				dwRead = 0;
				pBuf->data[2] = 0xFF;
				pBuf->flags = 0;

				if(g_RcvCobId == ERRORID)
				{
					continue;
				}

				CString str = "";
				str.Format("%d-%d-%d-%d", g_RcvData[0], g_RcvData[1], g_RcvData[2], g_RcvData[3]);

				switch(g_RcvData[2])
				{
				case 0xA0:	g_bCxTrue = TRUE;	break;
				case 0xA1:	
					{
						memcpy(g_RcvData_Exe, g_RcvData, 8);
						
						int nCidIndex=0;
						for(nCidIndex=0; nCidIndex<g_byCidSum_SetKu; nCidIndex++)
						{
							if( g_pStCid[nCidIndex].nID == g_RcvData[3])
							{
								if(g_pStCid[nCidIndex].byDQ > 0)
								{
									if(g_RcvData[0] < 2)
									{
										for(int j=0; j<g_byCidSum_SetKu; j++)
										{
											if( (g_pStCid[j].nID == g_RcvData[3]) && (g_pStCid[j].byDQ == 1) )
											{
												g_pStCid[j].bRcv = TRUE;
												g_pStCid[j].bSnd = FALSE;
												break;
											}
										}
									}
									else
									{
										for(int j=0; j<g_byCidSum_SetKu; j++)
										{
											if( (g_pStCid[j].nID == g_RcvData[3]) && (g_pStCid[j].byDQ == 2) )
											{
												g_pStCid[j].bRcv = TRUE;
												g_pStCid[j].bSnd = FALSE;
												break;
											}
										}									
									}
								}
								else
								{
									g_pStCid[nCidIndex].bRcv = TRUE;
									g_pStCid[nCidIndex].bSnd = FALSE;
								}
								break;
							}
						}
						
						g_bExeTrue = TRUE;	
						break;
					}
				case 0xA2:	
					{
						g_nIDEx = (int)g_RcvData[3];
						g_nMsgEx = MSG_EX_WATER;
						break;
					}
				case 0xA3:	g_nMsgEx = MSG_EX_YSJ;	break;
				case 0xA4:	g_nMsgEx = MSG_EX_ANS;	break;
				case 0xA5:	g_nMsgEx = MSG_EX_CHSH;	break;
				case 0xA6:	break;
				case 0xA7:	g_nMsgEx = MSG_EX_ELECT;break;
				default:	break;
				}
									
				if(!LOGIN_EN)	//仅供测试
				{
					CString strRcv = "";
					strRcv.Format("%d-%d-%d-%d", g_RcvData[0], g_RcvData[1], g_RcvData[2], g_RcvData[3]);
					/*if(g_pCanDlg != NULL)
					{
						g_pCanDlg->GetDlgItem(IDC_EDIT_CHSH)->SetWindowTextA(strRcv);
					}*/

					(g_RcvFlag & MSG_EXT)?
						strRcv += " EFF":	strRcv += " SFF";
					if(g_RcvFlag & MSG_RTR)
						strRcv += " |RTR";	
					if(g_RcvFlag & MSG_SELF)
						strRcv += " |SELF";
					if(g_RcvFlag & MSG_BOVR)
						strRcv += " Buffer overrun error";
					if(g_RcvFlag & MSG_OVR)
						strRcv += " Fifo overrun error";
					if(g_RcvFlag & MSG_BUSOFF)
						strRcv += " Bus off error";
					if(g_RcvFlag & MSG_PASSIVE)
						strRcv += " Passive error";

					if(g_RcvCobId == ERRORID)
					{
						strRcv += " ID error";
						Doing2Text(strRcv);
					}
				}
			}
			else
			{
				continue;
			}

			ExEvent();
		}
		else
		{
			continue;
		}
	}

	return 0;
}


UINT CAN2_ThreadRead(void *param)
{
	g_RcvFlag = 0;
	g_RcvCobId = 0;
	g_RcvLength = 0;
	memset(g_RcvData, 0xFF, sizeof(g_RcvData));
	
	DWORD	dwRead	=0;
	int nRcvTrue = 0;
	BYTE	byBuf[100];
	OVERLAPPED ov;

	DWORD	dwError = 0;

	while(g_bExitReadThreadFlag)
	{
		ZeroMemory(byBuf, 100);
		CanMsg_t *pBuf = (CanMsg_t *)byBuf;
		memset(&ov, 0, sizeof(OVERLAPPED));
		ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//,手动复位,无信号,[自动复位时间，waitforsingleobject不返回]

		nRcvTrue = acCanRead(g_hDevice_CAN2, pBuf, 1, &dwRead, &ov);
		CloseHandle(ov.hEvent);

		if(nRcvTrue == SUCCESS || nRcvTrue == TIME_OUT)
		{
			if( dwRead > 0 )
			{
				g_RcvFlag		= pBuf->flags;
				g_RcvCobId		= pBuf->id;
				g_RcvLength		= pBuf->length;
				memcpy(g_RcvData, pBuf->data, 8);

				dwRead = 0;
				pBuf->data[2] = 0xFF;
				pBuf->flags = 0;

				if(g_RcvCobId == ERRORID)
				{
					continue;
				}

				CString str = "";
				str.Format("%d-%d-%d-%d", g_RcvData[0], g_RcvData[1], g_RcvData[2], g_RcvData[3]);

				switch(g_RcvData[2])
				{
				case 0xA0:	g_bCxTrue = TRUE;	break;
				case 0xA1:	
					{
						memcpy(g_RcvData_Exe, g_RcvData, 8);
						
						int nCidIndex=0;
						for(nCidIndex=0; nCidIndex<g_byCidSum_SetKu; nCidIndex++)
						{
							if( g_pStCid[nCidIndex].nID == g_RcvData[3])
							{
								if(g_pStCid[nCidIndex].byDQ > 0)
								{
									if(g_RcvData[0] < 2)
									{
										for(int j=0; j<g_byCidSum_SetKu; j++)
										{
											if( (g_pStCid[j].nID == g_RcvData[3]) && (g_pStCid[j].byDQ == 1) )
											{
												g_pStCid[j].bRcv = TRUE;
												g_pStCid[j].bSnd = FALSE;
												break;
											}
										}
									}
									else
									{
										for(int j=0; j<g_byCidSum_SetKu; j++)
										{
											if( (g_pStCid[j].nID == g_RcvData[3]) && (g_pStCid[j].byDQ == 2) )
											{
												g_pStCid[j].bRcv = TRUE;
												g_pStCid[j].bSnd = FALSE;
												break;
											}
										}									
									}
								}
								else
								{
									g_pStCid[nCidIndex].bRcv = TRUE;
									g_pStCid[nCidIndex].bSnd = FALSE;
								}
								break;
							}
						}
						
						g_bExeTrue = TRUE;	
						break;
					}
				case 0xA2:	
					{
						g_nIDEx = (int)g_RcvData[3];
						g_nMsgEx = MSG_EX_WATER;
						break;
					}
				case 0xA3:	g_nMsgEx = MSG_EX_YSJ;	break;
				case 0xA4:	g_nMsgEx = MSG_EX_ANS;	break;
				case 0xA5:	g_nMsgEx = MSG_EX_CHSH;	break;
				case 0xA6:	break;
				case 0xA7:	g_nMsgEx = MSG_EX_ELECT;break;
				default:	break;
				}
									
				if(!LOGIN_EN)	//仅供测试
				{
					CString strRcv = "";
					strRcv.Format("%d-%d-%d-%d", g_RcvData[0], g_RcvData[1], g_RcvData[2], g_RcvData[3]);
					/*if(g_pCanDlg != NULL)
					{
						g_pCanDlg->GetDlgItem(IDC_EDIT_CHSH)->SetWindowTextA(strRcv);
					}*/

					(g_RcvFlag & MSG_EXT)?
						strRcv += " EFF":	strRcv += " SFF";
					if(g_RcvFlag & MSG_RTR)
						strRcv += " |RTR";	
					if(g_RcvFlag & MSG_SELF)
						strRcv += " |SELF";
					if(g_RcvFlag & MSG_BOVR)
						strRcv += " Buffer overrun error";
					if(g_RcvFlag & MSG_OVR)
						strRcv += " Fifo overrun error";
					if(g_RcvFlag & MSG_BUSOFF)
						strRcv += " Bus off error";
					if(g_RcvFlag & MSG_PASSIVE)
						strRcv += " Passive error";

					if(g_RcvCobId == ERRORID)
					{
						strRcv += " ID error";
						Doing2Text(strRcv);
					}
				}
			}
			else
			{
				continue;
			}

			ExEvent();
		}
		else
		{
			continue;
		}
	}

	return 0;
}


UINT CAN_ThreadWrite(LPVOID param)
{
	OVERLAPPED ov;
	DWORD dwWritten;

	//150805 必须这样
	acEnterResetMode(g_hDevice_CAN1);
	acClearRxFifo(g_hDevice_CAN1);
	acEnterWorkMode(g_hDevice_CAN1);

	if(g_bCAN2_InUse)
	{
		acEnterResetMode(g_hDevice_CAN2);
		acClearRxFifo(g_hDevice_CAN2);
		acEnterWorkMode(g_hDevice_CAN2);	
	}

	CanMsg_t tx[1];
	{
		ZeroMemory(tx[0].data, 8);
		memcpy(tx[0].data, g_SendData, 8);
		tx[0].flags	    = g_SendFlag;
		tx[0].id	    = g_dwCobId;
		tx[0].length    = g_SendLength;
	}
	
	memset(&ov, 0, sizeof(OVERLAPPED));
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if(!g_bCAN2_InUse)
	{
		acCanWrite(g_hDevice_CAN1, &tx[0], 1, &dwWritten, &ov);
	}
	else
	{
		(g_dwCobId <= g_byCAN12ID)?
			acCanWrite(g_hDevice_CAN1, &tx[0], 1, &dwWritten, &ov):
			acCanWrite(g_hDevice_CAN2, &tx[0], 1, &dwWritten, &ov);
	}
	
	CloseHandle(ov.hEvent);

	return 0;
}

void CAN_Open(CString strCan, HANDLE &hCan, BOOL &bCanOpened)
{
	char TempBuf[50];
	ZeroMemory(TempBuf, 50);
	strcpy(TempBuf, strCan);		//打开CAN
	hCan = acCanOpen(TempBuf, FALSE);

	if (hCan == INVALID_HANDLE_VALUE || hCan == NULL)
	{	
		if(g_bFlagSimCAN)
		{
			bCanOpened = TRUE;
			return;
		}
		
		acCanClose(hCan);
		hCan = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"控制通道打开错误，请关闭或断电重启",NULL,MB_OK);
		bCanOpened = FALSE;
		return;
	}
	
	int Ret;
	Ret = acEnterResetMode(hCan);
	if (Ret < 0)
	{
		acCanClose(hCan);
		hCan = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Enter reset mode Error",NULL,MB_OK);
		return;
	}

	//set param
	Ret = acSetBaud(hCan, g_nBaud);	//标准波特率，100K
	if (Ret < 0)
	{
		acCanClose(hCan);
		hCan = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Set Baud Error",NULL,MB_OK);
		return;
	}

	Ret = acSetAcceptanceFilter(hCan, g_dwAcceptCode, g_dwAcceptMask);
	if (Ret < 0)
	{
		acCanClose(hCan);
		hCan = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Set AccetpanceCode and AcceptanceMask Error",NULL,MB_OK);
		return;
	}

	Ret = acSetAcceptanceFilterMode(hCan, g_nFilterMode);
	if(Ret < 0)
	{
		acCanClose(hCan);
		hCan = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Set Accetpance filter mode Error",NULL,MB_OK);
		return;
	}

	Ret = acEnterWorkMode(hCan);
	if (Ret < 0)
	{
		acCanClose(hCan);
		hCan = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Enter work mode Error",NULL,MB_OK);
		return;
	}

	bCanOpened = TRUE;

	//打开CAN后即可接收
	(hCan == g_hDevice_CAN1)? CAN1_Rcv(): CAN2_Rcv();
}
void CAN_Close()
{
	g_bExitReadThreadFlag  = FALSE;

	acCanClose(g_hDevice_CAN1);
	g_hDevice_CAN1 = INVALID_HANDLE_VALUE;
	g_bOpened_CAN1 = FALSE;

	acCanClose(g_hDevice_CAN2);
	g_hDevice_CAN2 = INVALID_HANDLE_VALUE;
	g_bOpened_CAN2 = FALSE;

	p_ThreadWrite = NULL;
	p_ThreadRead = NULL;
	g_hThreadRead = NULL;
}
void CAN_Send()
{
	g_RcvData[2] = 0xFF;
	g_bExeTrue = FALSE;			//执行成功标志复位
	g_bCxTrue = FALSE;			//执行成功标志复位
	
	DWORD ThreadIDWrite;
	p_ThreadWrite = AfxBeginThread(CAN_ThreadWrite,&ThreadIDWrite);
}
void CAN1_Rcv()
{
	g_bExitReadThreadFlag = TRUE;

	DWORD ThreadIDRead;
	p_ThreadRead = AfxBeginThread(CAN1_ThreadRead, &ThreadIDRead, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	g_hThreadRead = p_ThreadRead->m_hThread;
}

void CAN2_Rcv()
{
	g_bExitReadThreadFlag = TRUE;

	DWORD ThreadIDRead;
	p_ThreadRead = AfxBeginThread(CAN2_ThreadRead, &ThreadIDRead, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	g_hThreadRead = p_ThreadRead->m_hThread;
}
/*
UINT ThreadWrite(LPVOID param)
{
	OVERLAPPED ov;
	DWORD dwWritten;

	//150805 必须这样
	acEnterResetMode(g_hDevice_CAN1);
	acClearRxFifo(g_hDevice_CAN1);
	acEnterWorkMode(g_hDevice_CAN1);

	CanMsg_t tx[1];
	{
		ZeroMemory(tx[0].data, 8);
		memcpy(tx[0].data, g_SendData, 8);
		tx[0].flags	    = g_SendFlag;
		tx[0].id	    = g_dwCobId;
		tx[0].length    = g_SendLength;
	}
	
	memset(&ov, 0, sizeof(OVERLAPPED));
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	acCanWrite(g_hDevice_CAN1, &tx[0], 1, &dwWritten, &ov);
	
	CloseHandle(ov.hEvent);

	return 0;
}

UINT ThreadRead(void *param)
{
	g_RcvFlag = 0;
	g_RcvCobId = 0;
	g_RcvLength = 0;
	memset(g_RcvData, 0xFF, sizeof(g_RcvData));
	
	DWORD	dwRead	=0;
	int nRcvTrue = 0;
	BYTE	byBuf[100];
	OVERLAPPED ov;

	DWORD	dwError = 0;

	while(g_bExitReadThreadFlag)
	{
		ZeroMemory(byBuf, 100);
		CanMsg_t *pBuf = (CanMsg_t *)byBuf;
		memset(&ov, 0, sizeof(OVERLAPPED));
		ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//,手动复位,无信号,[自动复位时间，waitforsingleobject不返回]

		nRcvTrue = acCanRead(g_hDevice_CAN1, pBuf, 1, &dwRead, &ov);
		CloseHandle(ov.hEvent);

		if(nRcvTrue == SUCCESS || nRcvTrue == TIME_OUT)
		{
			if( dwRead > 0 )
			{
				g_RcvFlag		= pBuf->flags;
				g_RcvCobId		= pBuf->id;
				g_RcvLength		= pBuf->length;
				memcpy(g_RcvData, pBuf->data, 8);

				dwRead = 0;
				pBuf->data[2] = 0xFF;
				pBuf->flags = 0;

				if(g_RcvCobId == ERRORID)
				{
					continue;
				}

				CString str = "";
				str.Format("%d-%d-%d-%d", g_RcvData[0], g_RcvData[1], g_RcvData[2], g_RcvData[3]);

				switch(g_RcvData[2])
				{
				case 0xA0:	g_bCxTrue = TRUE;	break;
				case 0xA1:	
					{
						memcpy(g_RcvData_Exe, g_RcvData, 8);
						
						int nCidIndex=0;
						for(nCidIndex=0; nCidIndex<g_byCidSum_SetKu; nCidIndex++)
						{
							if( g_pStCid[nCidIndex].nID == g_RcvData[3])
							{
								if(g_pStCid[nCidIndex].byDQ > 0)
								{
									if(g_RcvData[0] < 2)
									{
										for(int j=0; j<g_byCidSum_SetKu; j++)
										{
											if( (g_pStCid[j].nID == g_RcvData[3]) && (g_pStCid[j].byDQ == 1) )
											{
												g_pStCid[j].bRcv = TRUE;
												g_pStCid[j].bSnd = FALSE;
												break;
											}
										}
									}
									else
									{
										for(int j=0; j<g_byCidSum_SetKu; j++)
										{
											if( (g_pStCid[j].nID == g_RcvData[3]) && (g_pStCid[j].byDQ == 2) )
											{
												g_pStCid[j].bRcv = TRUE;
												g_pStCid[j].bSnd = FALSE;
												break;
											}
										}									
									}
								}
								else
								{
									g_pStCid[nCidIndex].bRcv = TRUE;
									g_pStCid[nCidIndex].bSnd = FALSE;
								}
								break;
							}
						}
						
						g_bExeTrue = TRUE;	
						break;
					}
				case 0xA2:	
					{
						g_nIDEx = (int)g_RcvData[3];
						g_nMsgEx = MSG_EX_WATER;
						break;
					}
				case 0xA3:	g_nMsgEx = MSG_EX_YSJ;	break;
				case 0xA4:	g_nMsgEx = MSG_EX_ANS;	break;
				case 0xA5:	g_nMsgEx = MSG_EX_CHSH;	break;
				case 0xA6:	break;
				default:	break;
				}
									
				if(!LOGIN_EN)	//仅供测试
				{
					CString strRcv = "";
					strRcv.Format("%d-%d-%d-%d", g_RcvData[0], g_RcvData[1], g_RcvData[2], g_RcvData[3]);
					//if(g_pCanDlg != NULL)
					//{
					//	g_pCanDlg->GetDlgItem(IDC_EDIT_CHSH)->SetWindowTextA(strRcv);
					//}

					(g_RcvFlag & MSG_EXT)?
						strRcv += " EFF":	strRcv += " SFF";
					if(g_RcvFlag & MSG_RTR)
						strRcv += " |RTR";	
					if(g_RcvFlag & MSG_SELF)
						strRcv += " |SELF";
					if(g_RcvFlag & MSG_BOVR)
						strRcv += " Buffer overrun error";
					if(g_RcvFlag & MSG_OVR)
						strRcv += " Fifo overrun error";
					if(g_RcvFlag & MSG_BUSOFF)
						strRcv += " Bus off error";
					if(g_RcvFlag & MSG_PASSIVE)
						strRcv += " Passive error";

					if(g_RcvCobId == ERRORID)
					{
						strRcv += " ID error";
						Doing2Text(strRcv);
					}
				}
			}
			else
			{
				continue;
			}

			ExEvent();
		}
		else
		{
			continue;
		}
	}

	return 0;
}


void CAN1_OpenClose()
{
	char TempBuf[50];
	ZeroMemory(TempBuf, 50);
	strcpy(TempBuf, "CAN1");		//打开CAN1
	g_hDevice_CAN1 = acCanOpen(TempBuf, FALSE);

	if (g_hDevice_CAN1 == INVALID_HANDLE_VALUE || g_hDevice_CAN1 == NULL)
	{	
		if(g_bFlagSimCAN)
		{
			g_bOpened_CAN1 = TRUE;
			return;
		}
		
		acCanClose(g_hDevice_CAN1);
		g_hDevice_CAN1 = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"控制通道打开错误，请关闭或断电重启",NULL,MB_OK);
		g_bOpened_CAN1 = FALSE;
		return;
	}
	
	int Ret;
	Ret = acEnterResetMode(g_hDevice_CAN1);
	if (Ret < 0)
	{
		acCanClose(g_hDevice_CAN1);
		g_hDevice_CAN1 = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Enter reset mode Error",NULL,MB_OK);
		return;
	}

	//set param
	Ret = acSetBaud(g_hDevice_CAN1, g_nBaud);	//标准波特率，100K
	if (Ret < 0)
	{
		acCanClose(g_hDevice_CAN1);
		g_hDevice_CAN1 = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Set Baud Error",NULL,MB_OK);
		return;
	}

	Ret = acSetAcceptanceFilter(g_hDevice_CAN1, g_dwAcceptCode, g_dwAcceptMask);
	if (Ret < 0)
	{
		acCanClose(g_hDevice_CAN1);
		g_hDevice_CAN1 = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Set AccetpanceCode and AcceptanceMask Error",NULL,MB_OK);
		return;
	}

	Ret = acSetAcceptanceFilterMode(g_hDevice_CAN1, g_nFilterMode);
	if(Ret < 0)
	{
		acCanClose(g_hDevice_CAN1);
		g_hDevice_CAN1 = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Set Accetpance filter mode Error",NULL,MB_OK);
		return;
	}

	Ret = acEnterWorkMode(g_hDevice_CAN1);
	if (Ret < 0)
	{
		acCanClose(g_hDevice_CAN1);
		g_hDevice_CAN1 = INVALID_HANDLE_VALUE;
		MessageBoxA(NULL,"Enter work mode Error",NULL,MB_OK);
		return;
	}

	g_bOpened_CAN1 = TRUE;

	CanRsv();		//打开CAN后即可接收
}

void CAN1_Send()
{
	g_RcvData[2] = 0xFF;
	g_bExeTrue = FALSE;			//执行成功标志复位
	g_bCxTrue = FALSE;			//执行成功标志复位
	
	DWORD ThreadIDWrite;
	p_ThreadWrite = AfxBeginThread(ThreadWrite,&ThreadIDWrite);
}

void CanRsv()
{
	g_bExitReadThreadFlag = TRUE;

	DWORD ThreadIDRead;
	p_ThreadRead = AfxBeginThread(ThreadRead, &ThreadIDRead, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	g_hThreadRead = p_ThreadRead->m_hThread;
}

*/