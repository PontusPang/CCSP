#include "stdafx.h"
#include "CS.h"

#include "ThreadProp.h"
#include "ThreadProp_TempTemp.h"
#include "ThreadProp_TimeTime.h"
#include "ThreadProp_TimeTemp.h"

#include "AdvCanThread.h"
#include "CANDlg.h"
#include <ATLComTime.h>

#include "Security.h"
#include "SetDlgFGYSJ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString aOnItem[NumOnItem];
extern CString aOnDir[NumOnDir];
extern CString aSetFunc[NumSetFunc];
extern CString aSetPropZu[NumSetPropZu];
extern CString aKuCtrlMode[NumKuCtrlMode];
extern CString aKuWorkMode[NumKuWorkMode];
extern CString aCtrlTimeTemp[NumCtrlTimeTemp];

extern HANDLE	g_hDevice_CAN1;
extern DWORD	g_dwCobId;
extern BYTE		g_SendData[8];
extern BYTE		g_RcvData[8];
extern BYTE		g_RcvData_Exe[8];
extern BOOL		g_bOpened_CAN1;
extern BOOL		g_bCxTrue;			//查询成功标志

extern _ConnectionPtr m_pCon;
extern _RecordsetPtr m_pRs;		//ADO记录集对象
extern _RecordsetPtr m_pRs_Timer2;	//Timer2
extern _RecordsetPtr m_pRs_OneOnOneOff;	
extern _RecordsetPtr m_pRs_OneOnOff;
extern _RecordsetPtr m_pRs_TempChange;	//温度递变
extern _RecordsetPtr m_pRs_AllOnAllOff;
extern _RecordsetPtr m_pRs_AllOnAllOff_In;
extern _RecordsetPtr m_pRs_Delay3;	//Delay3专属
extern _RecordsetPtr m_pRs_KuCtrl;
extern _RecordsetPtr m_pRs_CidProp;
extern _RecordsetPtr m_pRs_UrgentProp;	
extern _RecordsetPtr m_pRs_KuStatus;	//库房开关机状态
extern _RecordsetPtr m_pRs_ManualYSJ;

extern CCANDlg *g_pCanDlg;

extern BOOL g_bFlagSimCAN;
extern BOOL g_bFlagSimTEMP;
extern BOOL g_bFlagOnlyOnOff;

extern Prop_t g_StProp;
extern KuCtrl_t *g_pStKuCtrl;
extern TimeCtrl_t *g_pStTimeCtrl;
extern CXMsg_t g_StCXMsg;
extern YSJ_t *g_pStYSJ;
extern KuYSJ_t *g_pStKuYSJ;
extern TempSor_t *g_pStTempSor;

extern Cid_t *g_pStCid;
extern BYTE g_byCidSum_SetKu;
extern CS_t *g_pStCS;
extern int g_nCS_Index;

extern BOOL g_bStopProp;		//终止流程进程
extern BOOL g_bStopKuCtrl;		//终止温控进程
extern BOOL g_bUrgentStop;		//紧急关机标志
extern BOOL g_bUrgentING;		//紧急关机正在执行中
extern BOOL g_bUrgentJump;
extern BOOL g_bOneKeyStart;		//直接开启压缩机
extern BOOL g_bOneKeyStop;		//强制关机
extern BOOL g_bStopAuto;		//时开温关，停止温控
extern BOOL g_bStopDelay3;		//150915
extern BOOL g_bStopRef;
extern BOOL g_bFlagKuCtrlING_Watch;
extern BOOL g_bStopKuYSJ;

extern BYTE g_byKuOnMax_SetKu;			//同开最多库房数
extern BYTE g_byKuSum_SetKu;			//库房总数
extern BYTE g_byTempSum_SetKu;			//温度节点总数
extern BYTE g_byYSJSum_SetKu;			//压缩机总数
extern BYTE g_byPropSMS_SetKu;

extern int g_nCtrlMode;				//控制模式总数150807
extern BYTE g_byKuCtrlSum_St;		//自动控制在用库房总数
extern BYTE g_byTimeTempSum;		//多控制，时开温关库房总数150807
extern BYTE g_byTempTempSum;		//多控制，温开温关库房总数150807
extern BYTE *g_aKuNumTimeTemp;		//存储，时开温关，库房数
extern BYTE *g_aKuNumTempTemp;		//存储，温开温关，库房数

extern BYTE g_byKuOnSum;
extern BYTE g_byYSJOnSum;

extern HANDLE g_hPropING;
extern HANDLE g_hKuCtrl;
extern HANDLE g_hIDRefer;
extern HANDLE g_hMsg;
extern HANDLE g_hTimer2;
extern HANDLE g_hDelay3;
extern HANDLE g_hTimeProtect;
extern HANDLE g_hWatch;
extern HANDLE g_hKuYSJ;

extern BOOL g_bOnProp_MaxOn;		//开机流程-同开（最多）为TRUE，异开为FALSE
extern BOOL g_bTimeSt_SetKu;		//时开温关，时开=TRUE
extern BOOL g_bAllOffJump_SetKu;	//时开温关，全关跳过--TRUE
extern BOOL g_bAllOffDo_SetKu;		//时开温关，全关即止=TRUE;全关等开=FALSE;
extern BOOL g_bTimeOut_SetKu;		//时开温关，超时即关=TRUE
extern int g_nTimeS_SetKu;
extern BYTE g_byKuHour_SetKu;
extern BOOL g_bTimeProtectOUT;
extern CString g_strNowTimeKu;

extern BOOL g_bFlagAllKuOff;		//所有库房已关机
extern BOOL g_bKuOnMaxWhile;		//进入同开最多的内部循环

extern BOOL g_bInTimer2;			//处于延时2温控中
extern BOOL g_bFlagAllYSJOff;		//压缩机开关状态，执行完毕开机流程FALSE，执行完毕关机流程TRUE
extern BOOL g_bInKuTempChange;
extern BOOL g_bInDelay3;			//150915处于延时3温控中
extern int g_nFirstDelay3S;

extern BOOL g_bEnKuYSJ;
extern BOOL g_bKuLinkYSJ;
extern BOOL g_bFlagKuCtrlING_Wait;
extern BOOL g_bFlagPropING_Wait;

extern COleDateTime g_mTimeNow;
extern BOOL g_bAuthorAll;
extern BOOL g_bSysCanRun;

extern int g_nMsgEx;				//短信

extern CString g_strDualCtrlMode_TempTemp;	//161029
extern CString g_strDualCtrlMode_TimeTemp;	//161029

void YSJCmd(int nNum, int nDWKD, int nNC, CString strDir, CEdit *pEdit)
{
	int nTempDir = 0;
	for(int i=1; i<sizeof(aOnDir)/4; i++)
	{
		if(strDir == aOnDir[i])
		{
			nTempDir = i;
			break;
		}
	}

	switch(nTempDir)
	{
	case 1:	
		{
			g_SendData[0] = nDWKD;
			g_SendData[1] = 0xA2;
			CString str = "";
			str.Format("压缩机%d档位到%d", nNum, nDWKD);
			Doing2Text(str);
			pEdit->SetWindowTextA(str);
			break;
		}
	case 4: 
		{
			g_SendData[0] = nNC;
			g_SendData[1] = 0xA1;
			CString str = "";
			nNC? str.Format("压缩机%d开", nNum): str.Format("压缩机%d关", nNum);
			Doing2Text(str);
			pEdit->SetWindowTextA(str);
			break;
		}
	default:
		{
			g_SendData[0] = 0;
			g_SendData[1] = 0xA0;	
			break;
		}
	}

	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;
}

void DQCmd(CString strItem, int nNum, int nDQ, int nNC, CEdit *pEdit)
{		
	(nDQ == 2)? g_SendData[0] = nNC+2: g_SendData[0] = nNC;	//nNC=0/1
	
	CString strDQNC = "默认单动开关";

	CString strSql = "";
	strSql.Format("SELECT * FROM SetDQ WHERE DQItem = '%s' AND DQNum = %d", strItem, nNum);
	m_pRs_CidProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//57.1
	if(!m_pRs_CidProp->BOF)
	{
		strDQNC = (LPCSTR)_bstr_t(m_pRs_CidProp->GetCollect("DQNC"));
		(strcmp(strDQNC, "单动开关") == 0)? g_SendData[1] = 0xA1: g_SendData[1] = 0xA2;
	}
	else	//没有设置该电气，默认单动
	{
		g_SendData[1] = 0xA1;
	}
	m_pRs_CidProp->raw_Close();	//57.2

	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;

	CString str = "";
	nNC? str.Format("%s%d开  %s", strItem, nNum, strDQNC):
		str.Format("%s%d关  %s", strItem, nNum, strDQNC);
	Doing2Text(str);
	pEdit->SetWindowTextA(str);
}

void FMCmd(CString strItem, int nNum, int nDWKD, CEdit *pEdit)
{
	g_SendData[0] = nDWKD;
	g_SendData[1] = 0xA3;
	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;

	CString str = "";
	str.Format("%s%d开度到%d", strItem, nNum, nDWKD);
	Doing2Text(str);
	pEdit->SetWindowTextA(str);
}

void AFCmd(CString strFunc, int nNum, int nDWKD, CEdit *pEdit)
{
	CString strAuto = "默认不自动";

	CString strSql = "";
	strSql.Format("SELECT * FROM SetAF WHERE AFNum = %d", nNum);
	m_pRs_CidProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//59.1
	if(!m_pRs_CidProp->BOF)
	{
		strAuto = (LPCSTR)_bstr_t(m_pRs_CidProp->GetCollect("Auto"));

		if(strcmp(strAuto, "不自动") == 0)
		{
			g_SendData[0] = nDWKD;
		}
		else if(strcmp(strAuto, "开自动") == 0)
		{
			if(strcmp(strFunc, "关机流程") == 0)
			{
				g_SendData[0] = 0x65;
				strAuto = "停止自动控制";
			}
			else
			{
				g_SendData[0] = 0x64;
				strAuto = "开始自动控制";
			}
		}
		else
		{
			//AfxMessageBox("氨阀异常，报警");

			Doing2Text("氨阀异常，报警");
			pEdit->SetWindowTextA("氨阀异常，报警");
			
			// 发送紧急短信
			if(g_hMsg == NULL)
			{
				g_nMsgEx = MSG_EX_LOST;	//节点异常
				CWinThread* pThread;
				pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
				g_hMsg = pThread->m_hThread;
			}
			else
			{
				Doing2Text("节点丢失，短信被占用");
			}
		}
	}
	else	//没有设置该电气，默认单动
	{
		g_SendData[0] = nDWKD;
	}
	m_pRs_CidProp->raw_Close();	//59.2

	g_SendData[1] = 0xA3;
	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;

	CString str = "";
	str.Format("氨阀%d开度到%d  %s", nNum, nDWKD, strAuto);
	Doing2Text(str);
	pEdit->SetWindowTextA(str);
}

void TimerCmd(CString strFunc, int nTimeNO, int nTimeGoal, CEdit *pEdit)
{
	if(nTimeGoal<0) nTimeGoal = 0;

	if(nTimeNO == 3) g_nFirstDelay3S = nTimeGoal;

	CString strText = "";
	strText.Format("%s 延时%d %d 秒", strFunc, nTimeNO, nTimeGoal);
	Doing2Text(strText);
	
	COleDateTime m_TimeBegin, m_TimeEnd;
	COleDateTimeSpan m_TimeSpan;
	int nTimeReal = 0;

	m_TimeBegin = COleDateTime::GetCurrentTime();
	
	while(nTimeReal < nTimeGoal)
	{
		if(nTimeNO < 2)	 Write_WatchFile();	//喂狗	//延时2、延时3的延时不需要喂狗
		Sleep(1000);
		m_TimeEnd = COleDateTime::GetCurrentTime();
		m_TimeSpan = m_TimeEnd - m_TimeBegin;
		nTimeReal = (int)m_TimeSpan.GetTotalSeconds();

		int nLeaveS = nTimeGoal-nTimeReal;

		if(g_bInDelay3)
		{
			g_nFirstDelay3S = nLeaveS;
		}

		CString str = "";
		str.Format("%s 延时%d执行中：%d 秒", strFunc, nTimeNO, nLeaveS);
		pEdit->SetWindowTextA(str);

		if(g_bStopDelay3 && (nTimeNO == 3))
			break;
		if(g_bUrgentStop || g_bStopProp)
			break;
		if(nTimeNO == 0 && g_bFlagSimCAN)
			break;
	}
}
UINT ThreadProc_Timer2(LPVOID wParam)
{
	SP_CidRefer();
	CEdit *pEdit = (CEdit *)wParam;
	CString strSqlTimer2 = "SELECT * FROM PropTem WHERE Item = '延时' AND Num = 2";
	m_pRs_Timer2 = m_pCon->Execute(_bstr_t(strSqlTimer2),NULL,adCmdText);	//104.1
	if(!m_pRs_Timer2 -> BOF)
	{
		CString strTimer2Func = (LPCSTR)_bstr_t(m_pRs_Timer2->GetCollect("Func"));
		CString strTimer2NO = (LPCSTR)_bstr_t(m_pRs_Timer2->GetCollect("NO"));
		int nTimer2NO = atoi(strTimer2NO);
		m_pRs_Timer2->raw_Close();	//104.2.1

		strSqlTimer2.Format("SELECT * FROM PropTem left JOIN SetCid \
			ON PropTem.Item = SetCid.IdItem AND PropTem.Num = SetCid.IdItemNum \
			WHERE NO >= %d ORDER BY NO ASC", nTimer2NO);
		m_pRs_Timer2 = m_pCon->Execute(_bstr_t(strSqlTimer2),NULL,adCmdText);	//105.1

		if(!m_pRs_Timer2->BOF)
		{
			//流程执行标志
			g_bStopProp = FALSE;

			//执行流程（顺序执行）
			Doing2Text("顺序执行延时2流程");
			pEdit->SetWindowTextA("顺序执行延时2流程");
			Sleep(1000);

			PropExecute(m_pRs_Timer2, strTimer2Func, pEdit);
		}
		m_pRs_Timer2->raw_Close();//关闭PropExecute中数据库	//105.2
	}
	else
	{
		m_pRs_Timer2->raw_Close();	//104.2.2
		Doing2Text("理论上在此不会没有延时2");
	}
	
	pEdit->SetWindowTextA("延时2流程停止");
	Sleep(1000);
	Doing2Text("延时2流程停止");

	LONG lThreadDelay3Exit = 0;
	GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);
	if(STILL_ACTIVE != lThreadDelay3Exit)
	{
		g_pCanDlg->PropListLoad("SetProp", "清空流程");
	}

	g_bInTimer2 = FALSE;

	g_hTimer2 = NULL;
	//151103
	ST_CidRefer();

	
	//151104 允许开关压缩机
	CButton *pCheckYSJManual = (CButton *)g_pCanDlg->GetDlgItem(IDC_CHECK_YSJ_MANUAL);	
	pCheckYSJManual->SetCheck(FALSE);
	pCheckYSJManual->EnableWindow(TRUE);	//手动开关压缩机

	return 0;
}
//
void ItemDisp(CString strPropItem, int nItemNum, int nDWKD, int nNC, int nAFYW, CListCtrl* pList)
{
	//找到列表中对应的那一项--的序号
	int nListIndex = -1;
	for(int i=0; i<pList->GetItemCount(); i++)
	{
		CString strItem = pList->GetItemText(i, 1);
		CString strNum = pList->GetItemText(i, 2);
		int nNum = atoi(strNum);
		if( (strcmp(strItem, strPropItem) == 0) && (nNum == nItemNum) )
		{
			CString strListIndex = pList->GetItemText(i, 0);
			nListIndex = i;
			break;
		}
	}

	if(nListIndex == -1)
	{
		//AfxMessageBox("未找到该显示项目");
	}

	//CString aOnItem[] = {"压缩机", "水泵", "氨阀", "氨泵", "风机", "分液", "回气", "延时", "吸气", "排气", "电磁阀"};
	//找到项目所对应
	int nItem = 0;
	for(int i=0; i<NumOnItem; i++)
	{
		if(strPropItem == aOnItem[i])
		{
			nItem = i;
			break;
		}
	}

	//显示相应状态
	if(nItem != 7)
	{
		switch(nItem)
		{
		case 0:	//压缩机
			{	
				CString strNC = "";
				nNC? strNC = "开": strNC = "关";
				CString strDWNC = "";
				strDWNC.Format("%d+%s", nDWKD, strNC);
				pList->SetItemText(nListIndex, 3, strDWNC);
				break;
			}	
		case 1:	//电气
		case 3:
		case 4:	//风机
		case 10:
			{
				CString strNC = "";
				nNC? strNC = "开": strNC = "关";
				pList->SetItemText(nListIndex, 3, strNC);
				break;
			}
		case 2:	//氨阀
			{
				CString strDWKDYW = "";
				strDWKDYW.Format("%d+%d", nDWKD, nAFYW);
				pList->SetItemText(nListIndex, 3, strDWKDYW);				
				break;					
			}
		case 5: //阀门	
		case 6:
		case 8:
		case 9:
			{
				CString strDWKD = "";
				strDWKD.Format("%d", nDWKD);
				pList->SetItemText(nListIndex, 3, strDWKD);
				break;
			}
		default:
			{
				//AfxMessageBox("ERROR");
				break;			
			}
		}
	}
}
void ItemReferCtrlListDisp(BYTE *aRcv, CListCtrl* pList, CString strItem, int nItemNum, int nCidDQ)	//pList，单元遍历List Ctrl
{
	CListCtrl* pKuCtrlList = (CListCtrl*)g_pCanDlg->GetDlgItem(IDC_LIST_KU_CTRL);
	CListCtrl* pYSJCtrlList = (CListCtrl*)g_pCanDlg->GetDlgItem(IDC_LIST_YSJ_CTRL);

	//根据项目和号码判断在KuList的哪一行
	int nListIndex = -1;
	for(int i=0; i<pList->GetItemCount(); i++)
	{
		CString strListItem = pList->GetItemText(i, 2);
		CString strListItemNum = pList->GetItemText(i, 3);
		int nListItemNum = atoi(strListItemNum);
		
		if( (strcmp(strListItem, strItem) == 0) && (nListItemNum == nItemNum) )
		{
			nListIndex = i;
			break;
		}
	}
	
	int nItem = 0;
	for(int i=0; i<NumOnItem; i++)
	{
		if(strItem == aOnItem[i])
		{
			nItem = i;
			break;
		}
	}

	//显示相应状态
	if(nItem != 7)
	{
		switch(nItem)
		{
		case 0:	//压缩机
			{	
				CString strNC = "";
				(aRcv[1] == 0xA0)? strNC = "关": strNC = "开";
				CString strDWNC = "";
				if(aRcv[0] > 100) aRcv[0] = 0;
				strDWNC.Format("%d+%s", aRcv[0], strNC);
				pList->SetItemText(nListIndex, 4, strDWNC);

				CString strDW = "";
				strDW.Format("%d", aRcv[0]);
				for(int i=0; i<pYSJCtrlList->GetItemCount(); i++)
				{
					CString strYSJItemNum = pYSJCtrlList->GetItemText(i, 0);
					int nYSJItemNum = atoi(strYSJItemNum);
					if(nYSJItemNum == nItemNum)
					{
						pYSJCtrlList->SetItemText(i, 2, strDW);
						pYSJCtrlList->SetItemText(i, 1, strNC);
						pYSJCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						break;
					}
				}

				break;
			}	
		case 1:	//电气
		case 3:
		case 10:
		case 4:	
			{
				CString strNC = "";
				if(nCidDQ == 2)
				{
					(aRcv[0] == 0x02)?  strNC = "关": strNC = "开";
				}
				else
				{
					(aRcv[0] == 0x00)?  strNC = "关": strNC = "开";
				}
				pList->SetItemText(nListIndex, 4, strNC);

				if(nItem == 4)	//风机
				{
					for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
					{
						CString strKuItemNum = pKuCtrlList->GetItemText(i, 13);
						int nKuItemNum = atoi(strKuItemNum);
						if(nKuItemNum == nItemNum)
						{
							pKuCtrlList->SetItemText(i, 14, strNC);
							pKuCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
							break;
						}
					}
				}

				if(nItem == 1) //水泵
				{
					for(int i=0; i<pYSJCtrlList->GetItemCount(); i++)
					{
						CString strYSJItemNum = pYSJCtrlList->GetItemText(i, 12);
						int nYSJItemNum = atoi(strYSJItemNum);
						if(nYSJItemNum == nItemNum)
						{
							pYSJCtrlList->SetItemText(i, 13, strNC);
							pYSJCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
							break;
						}
					}				
				}

				if(nItem == 3) //氨泵
				{
					for(int i=0; i<pYSJCtrlList->GetItemCount(); i++)
					{
						CString strYSJItemNum = pYSJCtrlList->GetItemText(i, 10);
						int nYSJItemNum = atoi(strYSJItemNum);
						if(nYSJItemNum == nItemNum)
						{
							pYSJCtrlList->SetItemText(i, 11, strNC);
							pYSJCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
							break;
						}
					}				
				}

				break;
			}
		case 2:	//氨阀
			{
				CString strKDYW = "";
				if(aRcv[0] > 100) aRcv[0] = 0;
				strKDYW.Format("%d+%d", aRcv[0], aRcv[1]);
				pList->SetItemText(nListIndex, 4, strKDYW);

				CString strKD = "";
				strKD.Format("%d", aRcv[0]);
				CString strYW = "";
				strYW.Format("%d", aRcv[1]);
				for(int i=0; i<pYSJCtrlList->GetItemCount(); i++)
				{
					CString strYSJItemNum = pYSJCtrlList->GetItemText(i, 7);
					int nYSJItemNum = atoi(strYSJItemNum);
					if(nYSJItemNum == nItemNum)
					{
						pYSJCtrlList->SetItemText(i, 8, strKD);
						pYSJCtrlList->SetItemText(i, 9, strYW);
						pYSJCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						break;
					}
				}

				CString strText = "";
				strText.Format("氨阀  开度: %s; 液位: %s", strKD, strYW);
				Doing2Text(strText);

				break;					
			}
		case 5: 
			{
				CString strDWKD = "";
				if(aRcv[0] > 100) aRcv[0] = 0;
				strDWKD.Format("%2d", aRcv[0]);
				pList->SetItemText(nListIndex, 4, strDWKD);

				CString str2FYKD = "--,--";		
				for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
				{
					CString strKuItemNum = pKuCtrlList->GetItemText(i, 9);

					//仅单分液
					int nLength = strKuItemNum.GetLength();
					if(nLength > 2)
					{
						//双分液
						BYTE aKuItemNum[2] = {0,0};
						Str2Dec(strKuItemNum, aKuItemNum);
						if( (aKuItemNum[0] != 0) && (aKuItemNum[0] == nItemNum) )
						{
							str2FYKD = pKuCtrlList->GetItemText(i, 10);
							str2FYKD = strDWKD + str2FYKD.Right(3);
							pKuCtrlList->SetItemText(i, 10, str2FYKD);
							pKuCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						}
						else if( (aKuItemNum[1] != 0) && (aKuItemNum[1] == nItemNum) )
						{
							str2FYKD = pKuCtrlList->GetItemText(i, 10);
							str2FYKD = str2FYKD.Left(3) + strDWKD;
							pKuCtrlList->SetItemText(i, 10, str2FYKD);
							pKuCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);					
						}					
					}
					else
					{
						int nKuItemNum = atoi(strKuItemNum);
						if(nKuItemNum == nItemNum)
						{
							pKuCtrlList->SetItemText(i, 10, strDWKD);
							pKuCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
							break;
						}					
					}
				}

				break;
			}
		case 6:	//除分液以外的阀门	
		case 8:
		case 9:
			{
				CString strDWKD = "";
				if(aRcv[0] > 100) aRcv[0] = 0;
				strDWKD.Format("%d", aRcv[0]);
				pList->SetItemText(nListIndex, 4, strDWKD);

				if(nItem == 6)	//回气
				{
					for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
					{
						CString strKuItemNum = pKuCtrlList->GetItemText(i, 11);
						int nKuItemNum = atoi(strKuItemNum);
						if(nKuItemNum == nItemNum)
						{
							pKuCtrlList->SetItemText(i, 12, strDWKD);
							pKuCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
							break;
						}
					}					
				}

				if(nItem == 8) //吸气
				{
					for(int i=0; i<pYSJCtrlList->GetItemCount(); i++)
					{
						CString strYSJItemNum = pYSJCtrlList->GetItemText(i, 3);
						int nYSJItemNum = atoi(strYSJItemNum);
						if(nYSJItemNum == nItemNum)
						{
							pYSJCtrlList->SetItemText(i, 4, strDWKD);
							pYSJCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
							break;
						}
					}				
				}

				if(nItem == 9) //排气
				{
					for(int i=0; i<pYSJCtrlList->GetItemCount(); i++)
					{
						CString strYSJItemNum = pYSJCtrlList->GetItemText(i, 5);
						int nYSJItemNum = atoi(strYSJItemNum);
						if(nYSJItemNum == nItemNum)
						{
							pYSJCtrlList->SetItemText(i, 6, strDWKD);
							pYSJCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
							break;
						}
					}				
				}

				break;
			}
		default:
			{
				//AfxMessageBox("ERROR");
				break;			
			}
		}
	}

	pList->SetItemState(nListIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
}
void KuWorkStatusListDisp(int nKuNum, BOOL bWorkStatus, BOOL bAllKu)
{
	CListCtrl* pKuCtrlList = (CListCtrl*)g_pCanDlg->GetDlgItem(IDC_LIST_KU_CTRL);
	for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
	{
		CString strKuDispNumStatus = pKuCtrlList->GetItemText(i, 0);
		CString strKuDispNum = strKuDispNumStatus.Left(2);
		CString strKuDispEmpty = strKuDispNum.Right(1);
		(strcmp(strKuDispEmpty, " ") == 0)? 
			strKuDispNum = strKuDispNum.Left(1): strKuDispNum = strKuDispNum;
		CString strKuStatus = "";
		bWorkStatus? strKuStatus = " 开": strKuStatus = " 关";
		CString strKuNumStatus = "";
		if(!bAllKu)
		{
			int nKuDispNum = atoi(strKuDispNum);
			if(nKuNum == nKuDispNum)
			{
				strKuNumStatus = strKuDispNum + strKuStatus;
				Doing2Text(strKuNumStatus);
				pKuCtrlList->SetItemText(i, 0, strKuNumStatus);
				pKuCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
				break;
			}		
		}
		else
		{//所有库房统一修改
			strKuNumStatus = strKuDispNum + strKuStatus;
			Doing2Text(strKuNumStatus);
			pKuCtrlList->SetItemText(i, 0, strKuNumStatus);
			pKuCtrlList->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
		}
	}
}
//
int GetKuTemp(CEdit *pEdit, int nKuIndex)
{
	CListCtrl* pKuCtrlList = (CListCtrl*)g_pCanDlg->GetDlgItem(IDC_LIST_KU_CTRL);
	for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
	{
		pKuCtrlList->SetItemState(i, NULL, LVIS_FOCUSED|LVIS_SELECTED);
	}	

	int nCorrectTemp = 0;	
	float fTemp = 0.0;
	float fTempArray[2] = {285.5, 285.5};
	int ntempNumSum = g_pStKuCtrl[nKuIndex].byTempSum;	//若某温度节点错误，则总数减一，作为分母

	int nNoZeroTempNum = ntempNumSum;
	for(int i=0; i<g_pStKuCtrl[nKuIndex].byTempSum; i++)
	{
		//151114
		if(g_pStKuCtrl[nKuIndex].aTempNum[i])
		{
			fTempArray[i] = g_pCanDlg->TempNum(g_pStKuCtrl[nKuIndex].aTempNum[i], pEdit);
		}
		else
		{
			//Doing2Text("该温度探头屏蔽");
			nNoZeroTempNum -= 1;
			fTempArray[i] = 0.0;	
			ntempNumSum -= 1;
		}

		if(fTempArray[i] < 280.0)		//如果温度节点错误，则不计入
		{
			fTemp += fTempArray[i];
			nCorrectTemp++;

			//KuCtrlDisp
			if(g_pStKuCtrl[nKuIndex].aTempNum[i])
			{
				for(int j=0; j<pKuCtrlList->GetItemCount(); j++)
				{
					CString strKuTempNum1 = pKuCtrlList->GetItemText(j, 1);
					int nKuTempNum1 = atoi(strKuTempNum1);
					if(nKuTempNum1 == g_pStKuCtrl[nKuIndex].aTempNum[i])
					{
						CString strTemp = "";
						strTemp.Format("%.1f", fTempArray[i]);
						pKuCtrlList->SetItemText(j, 2, strTemp);
						pKuCtrlList->SetItemState(j, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						break;
					}

					CString strKuTempNum2 = pKuCtrlList->GetItemText(j, 3);
					int nKuTempNum2 = atoi(strKuTempNum2);
					if(nKuTempNum2 == g_pStKuCtrl[nKuIndex].aTempNum[i])
					{
						CString strTemp = "";
						strTemp.Format("%.1f", fTempArray[i]);
						pKuCtrlList->SetItemText(j, 4, strTemp);
						pKuCtrlList->SetItemState(j, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						break;
					}
				}
			}
		}
		else
		{
			ntempNumSum -= 1;
		}

		if(g_bStopKuCtrl)
		{
			break;
		}
	}

	if((nCorrectTemp > 0) && (ntempNumSum > 0))
	{
		//记录当前库房温度
		fTemp = fTemp/ntempNumSum;
		g_pStKuCtrl[nKuIndex].fTemp = fTemp;	

		//KuCtrlDisp
		for(int j=0; j<pKuCtrlList->GetItemCount(); j++)
		{
			CString strKuNum = pKuCtrlList->GetItemText(j, 0);
			int nKuNum = atoi(strKuNum);
			if(nKuNum == g_pStKuCtrl[nKuIndex].byKuNum)
			{
				CString strTemp = "";
				strTemp.Format("%.1f", fTemp);
				pKuCtrlList->SetItemText(j, 7, strTemp);
				pKuCtrlList->SetItemState(j, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
				break;
			}
		}		

		//短信温度2-2
		g_StCXMsg.Ku[g_pStKuCtrl[nKuIndex].byKuNum-1]=fTemp;

		CString strText = "";
		strText.Format("库房%d温度: %.1f ℃", g_pStKuCtrl[nKuIndex].byKuNum, g_pStKuCtrl[nKuIndex].fTemp);
		Doing2Text(strText);
	}
	else
	{
		if(!nNoZeroTempNum)
		{
			//KuCtrlDisp
			for(int j=0; j<pKuCtrlList->GetItemCount(); j++)
			{
				CString strKuNum = pKuCtrlList->GetItemText(j, 0);
				int nKuNum = atoi(strKuNum);
				if(nKuNum == g_pStKuCtrl[nKuIndex].byKuNum)
				{
					pKuCtrlList->SetItemState(j, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
					break;
				}
			}
			return ntempNumSum;
		}
		
		// 发送紧急短信
		if(!g_bStopKuCtrl)	//如果是关闭库房控制导致的，则不报警
		{
			if(g_hMsg == NULL)
			{
				g_nMsgEx = MSG_EX_T_LOST;	//温度丢失

				CWinThread* pThread;
				pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
				g_hMsg = pThread->m_hThread;
			}
			else
			{
				Doing2Text("温度丢失，短信被占用");
			}

			CString str = "";	//为后续报警做准备，变量i
			str.Format("库房%d错误，无法温控，报警，关闭风机", g_pStKuCtrl[nKuIndex].byKuNum);
			Doing2Text(str);		

			//仅关闭该库房
			TempOrTime_Off_OneOff(pEdit, nKuIndex, str);
			Sleep(1000);
		}

		nCorrectTemp = 0;
	}

	g_pCanDlg->KillTimer(TIMER_ID_TEMP);	//关闭定时器

	return nCorrectTemp;
}
BOOL GetKuStatus(BOOL bBorA, int nKuExeNum, CEdit *pEdit)
{
	CString strSqlAllKu = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";
	m_pRs_KuStatus = m_pCon->Execute(_bstr_t(strSqlAllKu),NULL,adCmdText);	//89.1
	(m_pRs_KuStatus->BOF)? g_bFlagAllKuOff = TRUE: g_bFlagAllKuOff = FALSE;//全部关机
	m_pRs_KuStatus->raw_Close();	//89.2

	CString strSqlKuStatus = "";
	strSqlKuStatus.Format("SELECT * FROM SetKu WHERE KuNum = %d AND KuNum > 0 AND KuStatus = 1", nKuExeNum);
	m_pRs_KuStatus = m_pCon->Execute(_bstr_t(strSqlKuStatus),NULL,adCmdText);	//90.1
	CString strKuWorkFlag = (LPCSTR)_bstr_t(m_pRs_KuStatus->GetCollect("WorkStatus"));
	m_pRs_KuStatus->raw_Close();	//90.2

	BOOL bKuStatus = FALSE;
	(strcmp(strKuWorkFlag, "0") == 0)?
		bKuStatus = FALSE:	//关机状态
		bKuStatus = TRUE;	//开机状态
	
	CString strKuStatus = "";
	if(bBorA)	//True，执行前
	{
		if(bKuStatus)	//True，已开机
		{
			strKuStatus.Format("执行前: 库房%d 已开机", nKuExeNum);
		}
		else
		{
			strKuStatus.Format("执行前: 库房%d 已关机", nKuExeNum);
		}
	}
	else
	{
		if(bKuStatus)	//True，已开机
		{
			strKuStatus.Format("执行后: 库房%d 已开机", nKuExeNum);
		}
		else
		{
			strKuStatus.Format("执行后: 库房%d 已关机", nKuExeNum);
		}	
	}

	pEdit->SetWindowTextA(strKuStatus);
	Doing2Text(strKuStatus);
	Sleep(1500);

	return bKuStatus;
}
//
UINT ThreadProc_KuTempChange(LPVOID wParam)
{
	//到时间修改SetKu数据库,修改结构体上下限
	g_bInKuTempChange = FALSE;	//手动复位，如关闭程序，重新开机等
	while(!g_bStopKuCtrl)
	{	//读取当前时间，直到g_byKuHour_SetKu点且0分，启动变化函数；不是g_byKuHour_SetKu点0分，仅Sleep
		if(!g_bInKuTempChange && (g_mTimeNow.GetHour() == g_byKuHour_SetKu) && (g_mTimeNow.GetMinute() == 0) )
		{
			CString strTimeNow = "";
			strTimeNow.Format("%d-%02d-%02d %02d:00:00", 
				g_mTimeNow.GetYear(), g_mTimeNow.GetMonth(), g_mTimeNow.GetDay(), g_byKuHour_SetKu);
			//需确认是否更改

			CString strKuTempChange = "";
			strKuTempChange = "SELECT * FROM SetKu WHERE (KuNum > 0) AND (ChangeDays > 0) AND (ChangeDays != 255) ORDER BY KuNum ASC";
			m_pRs_TempChange = m_pCon->Execute(_bstr_t(strKuTempChange),NULL,adCmdText);	//134.1

			if(m_pRs_TempChange->BOF)
			{
				m_pRs_TempChange->raw_Close();	//134.2.1
				Doing2Text("无需要温度递变的库房，递变完全结束");
				break;
			}
			
			while(!m_pRs_TempChange->adoEOF)
			{
				//①获取数据库数据
				CString strKuNum = "", strTempUp = "", strTempDn = "", strTempUp2 = "", strTempDn2 = "", strChangDays = "", strChangeST = "";

				strKuNum = (LPCSTR)_bstr_t(m_pRs_TempChange->GetCollect("KuNum"));
				int nKuNum = atoi(strKuNum);
				
				strTempUp = (LPCSTR)_bstr_t(m_pRs_TempChange->GetCollect("TempUp"));
				strTempDn = (LPCSTR)_bstr_t(m_pRs_TempChange->GetCollect("TempDn"));
				strTempUp2 = (LPCSTR)_bstr_t(m_pRs_TempChange->GetCollect("TempUp2"));
				strTempDn2 = (LPCSTR)_bstr_t(m_pRs_TempChange->GetCollect("TempDn2"));
				float fTempUp = (float)atof(strTempUp);
				float fTempDn = (float)atof(strTempDn);
				float fTempUp2 = (float)atof(strTempUp2);
				float fTempDn2 = (float)atof(strTempDn2);
				
				strChangDays = (LPCSTR)_bstr_t(m_pRs_TempChange->GetCollect("ChangeDays"));
				int nChangeDays = atoi(strChangDays);

				strChangeST = (LPCSTR)_bstr_t(m_pRs_TempChange->GetCollect("ChangeST")); 
				COleDateTime TimeST;
				TimeST.ParseDateTime(strChangeST);

				//②处理递变结果
				COleDateTimeSpan TimeSpan = g_mTimeNow - TimeST; 
				int nChangeTimeSpanDay = (int)TimeSpan.GetTotalDays();	//天数+1的8点钟改变
				if(nChangeTimeSpanDay < 1)	//如果是当天8点之前修改，则为0，不改变；如果当前时间小于设置起始时间，如人工调整，则也不改变
				{
					nChangeTimeSpanDay = 0;
				}
				int nChangeTimeLastDays = nChangeDays - nChangeTimeSpanDay;

				float fTempUpResult = fTempUp - (fTempUp - fTempUp2)*nChangeTimeSpanDay/nChangeDays;
				float fTempDnResult = fTempDn - (fTempDn - fTempDn2)*nChangeTimeSpanDay/nChangeDays;
				
				if(nChangeTimeSpanDay > nChangeDays)	//如果已经超过总天数，则直接等于最终值
				{
					fTempUpResult = fTempUp2;
					fTempDnResult = fTempDn2;

					nChangeTimeLastDays = 0;
				}

				if(fTempUp > fTempUp2)	//上限递减
				{
					if(fTempUpResult < fTempUp2)
					{
						fTempUpResult = fTempUp2;
					}
				}
				else
				{
					if(fTempUpResult >= fTempUp2)
					{
						fTempUpResult = fTempUp2;
					}				
				}

				if(fTempDn > fTempDn2)	//下限递减
				{
					if(fTempDnResult < fTempDn2)
					{
						fTempDnResult = fTempDn2;
					}
				}
				else
				{
					if(fTempDnResult >= fTempDn2)
					{
						fTempDnResult = fTempDn2;
					}				
				}

				//③修改数据库-温度现用上下限变为计算结果，总天数减去相应值
				strKuTempChange.Format("UPDATE SetKu SET [TempUp] = %.1f, [TempDn] = %.1f, [ChangeDays] = %d, [ChangeST] = '%s' \
									   WHERE (KuNum = %d) AND (KuNum > 0)", 
									   fTempUpResult, fTempDnResult, nChangeTimeLastDays, strTimeNow, nKuNum);
				m_pCon->Execute(_bstr_t(strKuTempChange),NULL,adCmdText);

				//④修改温控库房结构体数组
				//查找库号所在结构体
				if(g_pStKuCtrl != NULL)	
				{
					int nKuOnIndex = GetKuIndex_St(nKuNum, FALSE);
					if(nKuOnIndex > -1)
					{
						g_pStKuCtrl[nKuOnIndex].fTempUp = fTempUpResult;
						g_pStKuCtrl[nKuOnIndex].fTempDn = fTempDnResult;
					}			
				}

				//⑤更新设置界面库房显示，更新控制界面库房显示
				//仅修改上下限处
				CListCtrl* pKuCtrlList = (CListCtrl*)g_pCanDlg->GetDlgItem(IDC_LIST_KU_CTRL);
				for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
				{
					CString strKuDispNumStatus = pKuCtrlList->GetItemText(i, 0);
					CString strKuDispNum = strKuDispNumStatus.Left(2);
					int nKuDispNum = atoi(strKuDispNum);
					if(nKuDispNum == nKuNum)
					{
						CString strTempUp = "";
						strTempUp.Format("%.1f", fTempUpResult);
						CString strTempDn = "";
						strTempDn.Format("%.1f", fTempDnResult);
						pKuCtrlList->SetItemText( i, 5, LPCTSTR(strTempUp) );
						pKuCtrlList->SetItemText( i, 6, LPCTSTR(strTempDn) );
						break;
					}
				}

				CString strText = "";
				strText.Format("库%d本次递变: 上限%.1f→%.1f; 下限%.1f→%.1f; 剩余天: %d"
					, nKuNum, fTempUp, fTempUpResult, fTempDn, fTempDnResult, nChangeTimeLastDays);
				strText = strTimeNow + strText;
				Doing2Text(strText);
					
				m_pRs_TempChange->MoveNext();
			}
			m_pRs_TempChange->raw_Close();	//134.2.2	

			g_bInKuTempChange = TRUE;	//防止一分钟内多次循环
		}
		
		//延时长一点
		Sleep(2589);

		if( (g_mTimeNow.GetHour() == g_byKuHour_SetKu) && (g_mTimeNow.GetMinute() == 1) )
		{
			//g_byKuHour_SetKu:00置位，自此不再进入循环；g_byKuHour_SetKu:01复位，下次可进入循环；其他时间均不动作该标志
			g_bInKuTempChange = FALSE;	//自动复位
		}
	}

	return 0;
}
//
int GetKuCtrlSt()			//获得全部在用库房结构体数组
{
	//151118
	GetYSJSt();
	
	//获取全部在用库房总数150807
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 ORDER BY KuNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-50.1

	if(m_pRs->BOF)			//未存在该库房数
	{
		AfxMessageBox("无在用库房");
		m_pRs->raw_Close();	//RS-50.2.1
		return -1;
	}

	g_byKuCtrlSum_St = 0;	//全局变量值传递与复位
	while(!m_pRs->adoEOF)
	{
		g_byKuCtrlSum_St ++;
		m_pRs->MoveNext();
	}

	//动态申请结构体数组
	if(g_pStKuCtrl != NULL)
	{
		delete [] g_pStKuCtrl;	//释放内存，清空指针
		g_pStKuCtrl = NULL;
	}
	g_pStKuCtrl = new KuCtrl_t[g_byKuCtrlSum_St];
	for(int i=0; i<g_byKuCtrlSum_St; i++)
	{
		//memset(&g_pStKuCtrl[i], 0, sizeof(KuCtrl_t));
		g_pStKuCtrl[i].byKuNum = 0;
		g_pStKuCtrl[i].byTempSum = 0;
		memset(g_pStKuCtrl[i].aTempNum, 0, KU_TEMP);
		g_pStKuCtrl[i].strYSJNum = "";
		memset(g_pStKuCtrl[i].aYSJNum, 0, KU_YSJ);
		memset(g_pStKuCtrl[i].aFYNum, 0, KU_FY);
		g_pStKuCtrl[i].byHQNum = 0;
		g_pStKuCtrl[i].byFJNum = 0;
		g_pStKuCtrl[i].fTempUp = 0.0;
		g_pStKuCtrl[i].fTempDn = 0.0;
		g_pStKuCtrl[i].fTemp = 0.0;
		g_pStKuCtrl[i].bFlagOn = FALSE;
		g_pStKuCtrl[i].bOnlyFJ = FALSE;
		g_pStKuCtrl[i].bDelay3 = FALSE;
	}

	//填充结构体数组成员
	m_pRs->MoveFirst();
	int nKuIndex = 0;		//最终，实际等于KuSum
	while(!m_pRs->adoEOF)
	{
		//获取库房编号
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
		g_pStKuCtrl[nKuIndex].byKuNum = atoi(strKuNum);

		//获取测温序号
		CString strTempNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempNum"));
		g_pStKuCtrl[nKuIndex].byTempSum = Str2Dec(strTempNum, g_pStKuCtrl[nKuIndex].aTempNum);			//将温度节点序号存入数组，并返回实际节点总数	

		//获取各设备号码
		CString strKuYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuYSJ"));
		g_pStKuCtrl[nKuIndex].strYSJNum = strKuYSJ;
		Str2Dec(strKuYSJ, g_pStKuCtrl[nKuIndex].aYSJNum);
		CString strKuHQ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuHQ"));
		g_pStKuCtrl[nKuIndex].byHQNum = (atoi(strKuHQ));
		CString strKuFJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFJ"));
		g_pStKuCtrl[nKuIndex].byFJNum = (atoi(strKuFJ));

		//双分液
		CString strKuFY = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFY"));
		//g_pStKuCtrl[nKuIndex].byFYNum = (atoi(strKuFY));
		Str2Dec(strKuFY, g_pStKuCtrl[nKuIndex].aFYNum);

		//获取温度上下限
		CString strTempUp = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempUp"));
		CString strTempDn = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempDn"));
		g_pStKuCtrl[nKuIndex].fTempUp = (float)atof(strTempUp);
		g_pStKuCtrl[nKuIndex].fTempDn = (float)atof(strTempDn);

		g_pStKuCtrl[nKuIndex].fTemp = 285.5;	//初始化

		//初始化开关机状态
		CString strKuWorkFlag = (LPCSTR)_bstr_t(m_pRs->GetCollect("WorkStatus"));
		(strcmp(strKuWorkFlag, "0") == 0)?
			g_pStKuCtrl[nKuIndex].bFlagOn = FALSE:	//关机状态
			g_pStKuCtrl[nKuIndex].bFlagOn = TRUE;	//开机状态

		CString strOnlyOffFJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuOnlyOffFJ"));
		( strcmp(strOnlyOffFJ, "0") == 0 )? 
			g_pStKuCtrl[nKuIndex].bOnlyFJ = FALSE: 
			g_pStKuCtrl[nKuIndex].bOnlyFJ = TRUE;

		//Delay3 - 延时关机判断
		if(g_pStKuCtrl[nKuIndex].bOnlyFJ)
		{
			g_pStKuCtrl[nKuIndex].bDelay3 = FALSE;
		}
		else
		{
			strSql = "SELECT * FROM SetProp WHERE Func = '关机流程' AND Item = '延时' AND Num = 3";
			m_pRs_Delay3 = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//145.1
			if(m_pRs_Delay3->BOF)
			{
				g_pStKuCtrl[nKuIndex].bDelay3 = FALSE;
				m_pRs_Delay3->raw_Close();	//145.2.1
			}
			else
			{
				m_pRs_Delay3->raw_Close();	//145.2.2

				strSql = "SELECT * FROM SetProp WHERE Func = '关机流程' AND Item = '延时' AND Num = 3 AND ZuItem = '特殊'";
				m_pRs_Delay3 = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//146.1
				if(!m_pRs_Delay3->BOF)		//特殊分组，则为风机前，对每个库有效
				{
					g_pStKuCtrl[nKuIndex].bDelay3 = TRUE;
					m_pRs_Delay3->raw_Close();	//146.2.1
				}
				else	//库房分组，则为回气前后，延时3时长可不同
				{
					m_pRs_Delay3->raw_Close();	//146.2.2

					strSql.Format("SELECT * FROM SetProp WHERE Func = '关机流程' AND Item = '延时' AND Num = 3 \
								  AND ZuItem = '库房' AND ZuNum = %d", g_pStKuCtrl[nKuIndex].byKuNum);
					m_pRs_Delay3 = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//147.1
					(m_pRs_Delay3->BOF)? g_pStKuCtrl[nKuIndex].bDelay3 = FALSE: g_pStKuCtrl[nKuIndex].bDelay3 = TRUE;
					m_pRs_Delay3->raw_Close();	//147.2
				}
			}		
		}

		if(!g_bKuLinkYSJ && (nKuIndex>0) && (strcmp(g_pStKuCtrl[nKuIndex].strYSJNum, g_pStKuCtrl[nKuIndex-1].strYSJNum) != 0))	//151116
			g_bKuLinkYSJ = TRUE;

		nKuIndex ++;
		m_pRs->MoveNext();
	}

	m_pRs->raw_Close();	//RS-50.2.2

	//151105 检查是否库房数开关压缩机
	strSql = "SELECT * FROM SetKu WHERE KuNum = 0";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-7.1
	CString strEnKuYSJ =  (LPCSTR)_bstr_t(m_pRs->GetCollect("KuOnlyOffFJ"));
	m_pRs->raw_Close();	//151105-7.2
	(strcmp(strEnKuYSJ, "0") == 0)? g_bEnKuYSJ = FALSE: g_bEnKuYSJ = TRUE;
	if(g_bEnKuYSJ)
	{
		BOOL bEnKuYSJ = EnKuYSJ();
		if(!bEnKuYSJ)
			return -1;
		GetKuYSJSt();
	}
	
	g_bEnKuYSJ?	strEnKuYSJ = "将执行库房数开关压缩机": strEnKuYSJ = "不执行库房数开关压缩机";
	g_pCanDlg->GetDlgItem(IDC_EDIT_PROP)->SetWindowTextA(strEnKuYSJ);
	Doing2Text(strEnKuYSJ);
	Sleep(2000);

	return 1;
}

int GetKuIndex_St(BYTE byKuNum, BOOL bUrgentStop)
{
	int nKuIndex = -1;
	for(int i=0; i<g_byKuCtrlSum_St; i++)
	{
		if(g_pStKuCtrl[i].byKuNum == byKuNum)
		{
			nKuIndex = i;
			break;
		}
	}
	if( bUrgentStop && (nKuIndex < 0) )
	{
		Auto_UrgentStop("查库失败");
	}
	return nKuIndex;
}
//
void PropExecute(_RecordsetPtr &m_pRsProp, CString strFunc, CEdit *pEdit)
{
	CString strSql = "";
	g_bStopProp = FALSE;

	int nNo=0;			//在总流程中的顺序
	int nPropNO = 1;	//提取出的这部分流程的顺序
	while(!m_pRsProp->adoEOF)
	{		
		if( (m_pRsProp == m_pRs_AllOnAllOff) || (m_pRsProp == m_pRs_OneOnOff))	//不允许延时2、延时3喂狗
			Write_WatchFile();	//喂狗
		
		CString strFlag = "", strCid = "", strCidDQ = "", strNO = "", strPropItem = "", strItemNum = "";
		CString strDir = "", strDWKD = "", strNC = "", strTime = "";
		strNO = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("NO"));
		nNo = atoi(strNO);
		strFlag = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("Flag"));
		strPropItem = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("Item"));
		strItemNum = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("Num"));
		int nItemNum = atoi(strItemNum);

		//清除选中显示
		CListCtrl * pList = (CListCtrl *)g_pCanDlg->GetDlgItem(IDC_LIST_CID_CTRL);
		for(int i=0; i<pList->GetItemCount(); i++)
		{
			pList->SetItemState(i, NULL, LVIS_FOCUSED|LVIS_SELECTED);
		}
		CListCtrl * pKuCtrlDispList = (CListCtrl *)g_pCanDlg->GetDlgItem(IDC_LIST_KU_CTRL);
		for(int i=0; i<pKuCtrlDispList->GetItemCount(); i++)
		{
			pKuCtrlDispList->SetItemState(i, NULL, LVIS_FOCUSED|LVIS_SELECTED);
		}
		CListCtrl* pYSJCtrlList = (CListCtrl*)g_pCanDlg->GetDlgItem(IDC_LIST_YSJ_CTRL);
		for(int i=0; i<pYSJCtrlList->GetItemCount(); i++)
		{
			pYSJCtrlList->SetItemState(i, NULL, LVIS_FOCUSED|LVIS_SELECTED);
		}

		if(strcmp(strFlag, "0") != 0)
		{
			if(g_bStopProp)
				break;
			if(g_bStopKuYSJ && (m_pRsProp == m_pRs_ManualYSJ))	//单独退出
				break;
			if(g_bStopDelay3 && (m_pRsProp == m_pRs_Delay3))	//单独退出
				break;

			nPropNO++;
			m_pRsProp->MoveNext();			
			continue;
		}	

		int nItem = 0;
		for(int i=0; i<NumOnItem; i++)
		{
			if(strPropItem == aOnItem[i])
			{
				nItem = i;
				break;
			}
		}

		//等待时间分级
		//CString aOnItem[] = {"压缩机", "水泵", "氨阀", "氨泵", "风机", "分液", "回气", "延时", "吸气", "排气", "电磁阀"};
		int nWaitRcvTime_FMAFSB[3] = {45, 55, 65};
		int nWaitRcvTime_DQDW[3] = {5, 10, 15};
		int nWaitRcvTime[3] = {0};

		if(nItem != 7)
		{				
			strCid = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("Cid"));
			g_dwCobId = atoi(strCid);
			BYTE byCidDQ = 0;

			switch(nItem)
			{
			case 0:	//压缩机
				{	
					strDir = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("Dir"));
					strDWKD = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("DWKD"));
					strNC = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("NC"));
					YSJCmd(nItemNum, atoi(strDWKD), atoi(strNC), strDir, pEdit);

					if(strcmp(strDir, "档位到") == 0)
					{
						for(int i=0; i<3; i++)
						{
							nWaitRcvTime[i] = nWaitRcvTime_DQDW[i];
						}
					}
					else
					{
						for(int i=0; i<3; i++)
						{
							nWaitRcvTime[i] = nWaitRcvTime_FMAFSB[i];
						}					
					}
					break;
				}	
			case 1:
			case 3:
			case 4:	//风机
			case 10:
				{
					strNC = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("NC"));
					strCidDQ = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("CidDQ"));
					byCidDQ = (BYTE)atoi(strCidDQ);
					DQCmd(strPropItem, nItemNum, byCidDQ, atoi(strNC), pEdit);

					if(nItem == 1)	//水泵
					{
						if(g_bUrgentJump)
						{
							Doing2Text("紧急关机：压缩机或吸气未关，之后的 水泵和排气保持开");
							nPropNO++;
							m_pRsProp->MoveNext();			
							continue;
						}
						
						for(int i=0; i<3; i++)
						{
							nWaitRcvTime[i] = nWaitRcvTime_FMAFSB[i];
						}						
					}
					else	//AB-FJ-DCf
					{
						for(int i=0; i<3; i++)
						{
							nWaitRcvTime[i] = nWaitRcvTime_DQDW[i];
						}					
					}

					break;
				}
			case 2:	
				{
					strDWKD = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("DWKD"));
					AFCmd(strFunc, nItemNum, atoi(strDWKD), pEdit);

					for(int i=0; i<3; i++)
					{
						nWaitRcvTime[i] = nWaitRcvTime_FMAFSB[i];
					}

					break;					
				}
			case 5: 		
			case 6:
			case 8:
			case 9:
				{					
					strDWKD = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("DWKD"));
					FMCmd(strPropItem, nItemNum, atoi(strDWKD), pEdit);

					if(g_bUrgentJump)
					{
						if(strcmp(strPropItem, "排气") == 0)
						{	
							Doing2Text("紧急关机：压缩机或吸气未关，之后的 水泵和排气保持开");
							nPropNO++;
							m_pRsProp->MoveNext();			
							continue;
						}
					}

					for(int i=0; i<3; i++)
					{
						nWaitRcvTime[i] = nWaitRcvTime_FMAFSB[i];
					}

					break;
				}
			default:
				{
					break;			
				}
			}
			
			//151106
			BYTE aSendData_AllOnAllOff[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			BYTE aSendData_OneOnOneOff[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			BYTE aSendData_Timer2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			BYTE aSendData_Delay3[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			BYTE aSendData_ManualYSJ[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

			
			if(!g_bFlagSimCAN)
			{
				if(m_pRsProp == m_pRs_AllOnAllOff)
				{
					memcpy(aSendData_AllOnAllOff, g_SendData, 8);
				}
				else if(m_pRsProp == m_pRs_OneOnOff)
				{
					memcpy(aSendData_OneOnOneOff, g_SendData, 8);
				}
				else if(m_pRsProp == m_pRs_Timer2)
				{
					memcpy(aSendData_Timer2, g_SendData, 8);
				}
				else if(m_pRsProp == m_pRs_Delay3)
				{
					memcpy(aSendData_Delay3, g_SendData, 8);
				}
				else if(m_pRsProp == m_pRs_ManualYSJ)
				{
					memcpy(aSendData_ManualYSJ, g_SendData, 8);
				}
				
				CAN_Send();
			}
			else
			{
				//4-2-1模拟接收（模拟时注销掉）
				//CAN_Send();

				//4-2-2模拟接收
				if(nItem != 7)
				{
					switch(nItem)
					{
					case 0:	//压缩机
						{	
							g_RcvData[0] = atoi(strDWKD);
							(strcmp(strFunc, "关机流程") == 0)? g_RcvData[1] = 0xA0: g_RcvData[1] = 0xA1;	//151014
							//g_RcvData[1] = 0xA0 + atoi(strNC);
							break;
						}	
					case 1:	//电气
					case 3:
					case 4:	//风机
					case 10:
						{
							(atoi(strCidDQ) == 2)? g_RcvData[0] = atoi(strNC)+2: g_RcvData[0] = atoi(strNC);	
							g_RcvData[1] = 0xA0;					
							break;
						}
					case 2:	//氨阀
						{
							g_RcvData[0] = atoi(strDWKD);	
							g_RcvData[1] = 50;				
							break;					
						}
					case 5: //阀门	
					case 6:
					case 8:
					case 9:
						{
							g_RcvData[0] = atoi(strDWKD);	
							g_RcvData[1] = 0xA0;	
							break;
						}
					default:
						{
							//AfxMessageBox("ERROR");
							break;			
						}
					}
				}
				g_RcvData[2] = 0xA1;
				g_RcvData[3] = (BYTE)g_dwCobId;
				memcpy(g_RcvData_Exe, g_RcvData, 8);	//150930
				Sleep(1000);
				//模拟接收			
			}

			int nCidIndex=0;
			for(nCidIndex=0; nCidIndex<g_byCidSum_SetKu; nCidIndex++)
			{
				if( (g_pStCid[nCidIndex].nID == g_dwCobId) && (g_pStCid[nCidIndex].byDQ == byCidDQ) )
				{
					g_pStCid[nCidIndex].bSnd = TRUE;
					g_pStCid[nCidIndex].bRcv = FALSE;
					break;
				}
			}
			if(nCidIndex == g_byCidSum_SetKu)
			{
				Auto_UrgentStop("流程执行 查找失败");
			}

			//151010
			if(g_bFlagSimCAN)
				g_pStCid[nCidIndex].bRcv = TRUE;

			//等待反馈成功
			int m = 0;	//150805
			int n = 0;
			while(!g_pStCid[nCidIndex].bRcv)
			{
				Sleep(1000);	//释放线程
				n++;

				//150805
				if(!LOGIN_EN)	//仅供测试
				{
					if(m < 3)
					{
						if( (g_RcvData[2] == 0xA6) || (g_RcvData[3] == ERRORID) )
						{					
							Doing2Text("下位机反馈干扰");
							Sleep(1000);

							if(m_pRsProp == m_pRs_AllOnAllOff)
							{
								memcpy(g_SendData, aSendData_AllOnAllOff, 8);
							}
							else if(m_pRsProp == m_pRs_OneOnOff)
							{
								memcpy(g_SendData, aSendData_OneOnOneOff, 8);
							}
							else if(m_pRsProp == m_pRs_Timer2)
							{
								memcpy(g_SendData, aSendData_Timer2, 8);
							}
							else if(m_pRsProp == m_pRs_Delay3)
							{
								memcpy(g_SendData, aSendData_Delay3, 8);
							}
							else if(m_pRsProp == m_pRs_ManualYSJ)
							{
								memcpy(g_SendData, aSendData_ManualYSJ, 8);
							}

							CAN_Send();	//内部有g_RcvData[2]复位

							n=0;
							m++;
						}
					}
				}

				if((n >= nWaitRcvTime[0]) || (m > 2))
				{
					//150805
					if(!LOGIN_EN && (m > 2) )	//仅供测试
					{
						Auto_UrgentStop("单元遍历 反馈超限");
						break;
					}
					//150805
								
					//151201
					if(g_bUrgentStop)	//如果是紧急关机，则等不到也跳过
					{
						CString str = "";
						str.Format("%s%d%s 紧急关机过程中丢失，跳过", strPropItem, nItemNum, strDir);
						Doing2Text(str);
						if(((strcmp(strPropItem, "压缩机") == 0)&&(strcmp(strDir, "选开关") == 0)) || (strcmp(strPropItem, "吸气") == 0) )
						{
							g_bUrgentJump = TRUE;
						}
						break;
					}
					if(g_bStopProp)	//如果停止流程，则终; 
					{	
						Doing2Text("人为停止流程执行");
						break;
					}
					if(g_bStopKuYSJ && (m_pRsProp == m_pRs_ManualYSJ))	//单独退出
						break;

					if(n == nWaitRcvTime[0])
					{
						pEdit->SetWindowTextA("通信超时，尝试重发第1次");
						Doing2Text("通信超时，尝试重发第1次");
												
						if(m_pRsProp == m_pRs_AllOnAllOff)
						{
							memcpy(g_SendData, aSendData_AllOnAllOff, 8);
						}
						else if(m_pRsProp == m_pRs_OneOnOff)
						{
							memcpy(g_SendData, aSendData_OneOnOneOff, 8);
						}
						else if(m_pRsProp == m_pRs_Timer2)
						{
							memcpy(g_SendData, aSendData_Timer2, 8);
						}
						else if(m_pRsProp == m_pRs_Delay3)
						{
							memcpy(g_SendData, aSendData_Delay3, 8);
						}
						else if(m_pRsProp == m_pRs_ManualYSJ)
						{
							memcpy(g_SendData, aSendData_ManualYSJ, 8);
						}
						CAN_Send();					
					}

					if(n >= nWaitRcvTime[1])	
					{
						if(g_bStopProp)	//如果停止流程，则终止
						{	
							break;
						}
						if(n == nWaitRcvTime[1])
						{
							pEdit->SetWindowTextA("通信超时，尝试重发第2次");
							Doing2Text("通信超时，尝试重发第2次");
							
							if(m_pRsProp == m_pRs_AllOnAllOff)
							{
								memcpy(g_SendData, aSendData_AllOnAllOff, 8);
							}
							else if(m_pRsProp == m_pRs_OneOnOff)
							{
								memcpy(g_SendData, aSendData_OneOnOneOff, 8);
							}
							else if(m_pRsProp == m_pRs_Timer2)
							{
								memcpy(g_SendData, aSendData_Timer2, 8);
							}
							else if(m_pRsProp == m_pRs_Delay3)
							{
								memcpy(g_SendData, aSendData_Delay3, 8);
							}
							else if(m_pRsProp == m_pRs_ManualYSJ)
							{
								memcpy(g_SendData, aSendData_ManualYSJ, 8);
							}
							CAN_Send();						
						}

						if(n > nWaitRcvTime[2])
						{
							if(g_bStopProp)	//如果停止流程，则终止
							{	
								break;
							}

							CString str = "";
							str.Format("%02d:%02d  %d通信超时，发送3次失败，退出，报警, 紧急关机"
								, g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), g_dwCobId);
							Doing2Text(str);
							pEdit->SetWindowTextA(str);

							// 发送紧急短信
							if(g_hMsg == NULL)
							{
								g_nMsgEx = MSG_EX_LOST;	//节点异常
								CWinThread* pThread;
								pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
								g_hMsg = pThread->m_hThread;
							}
							else
							{
								Doing2Text("节点丢失，短信被占用");
							}

							Auto_UrgentStop("流程执行 节点丢失");
						}
					}
				}
				if(g_bStopProp)
					break;
				if(g_bStopKuYSJ && (m_pRsProp == m_pRs_ManualYSJ))	//单独退出
					break;
			}

			//复位与置位各条流程标志
			if(!g_bStopProp)
			{
				ItemReferCtrlListDisp(g_RcvData_Exe, pList, strPropItem, nItemNum, atoi(strCidDQ));	
				memset(g_RcvData_Exe, 255, 8);
				
				//置位本条已经执行的流程，nNo为实际的流程序号，而不是该组流程编号;也不能是Item+Num，因为流程中可能重复
				strSql.Format("UPDATE SetProp SET Flag = 1 WHERE Func = '%s' AND NO = %d", strFunc, nNo);	
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

				//将对应流程置为False
				if( strcmp(strFunc, aSetFunc[0]) == 0 )			//开机流程
				{
					strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' AND Item = '%s' AND Num = %d", 
						aSetFunc[1], strPropItem, nItemNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
					
					//风机的特殊处理
					if(strcmp(strPropItem, "风机") == 0)
					{
						strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' AND Item = '%s' AND Num = 0", 
							aSetFunc[1], strPropItem);
						m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);					
					}

					if((strcmp(strPropItem, "压缩机") == 0) && (strcmp(strDir, "选开关") == 0))
					{
						g_byYSJOnSum++;
						g_StCXMsg.YSJ[nItemNum-1] = 1;
						
						if(!g_bStopKuYSJ)
						{
							int nYSJIndex = GetYSJIndex_St(nItemNum);
							if((nYSJIndex > -1) && (nYSJIndex < g_byYSJSum_SetKu))
								g_pStYSJ[nYSJIndex].bNC = TRUE;
						}
					}
				}
				else if( strcmp(strFunc, aSetFunc[1]) == 0 )	//关机流程
				{
					strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' AND Item = '%s' AND Num = %d", 
						aSetFunc[0], strPropItem, nItemNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
					
					//风机的特殊处理
					if(strcmp(strPropItem, "风机") == 0)
					{
						strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' AND Item = '%s' AND Num = 0", 
							aSetFunc[0], strPropItem);
						m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);					
					}

					if((strcmp(strPropItem, "压缩机") == 0) && (strcmp(strDir, "选开关") == 0))
					{
						g_byYSJOnSum--;
						g_StCXMsg.YSJ[nItemNum-1] = 0;
						
						if(!g_bStopKuYSJ)
						{
							int nYSJIndex = GetYSJIndex_St(nItemNum);
							if((nYSJIndex > -1) && (nYSJIndex < g_byYSJSum_SetKu))
								g_pStYSJ[nYSJIndex].bNC = FALSE;
						}
					}
				}

				if(!g_bStopDelay3 && g_bInDelay3 && (m_pRsProp == m_pRs_Delay3) && (strcmp(strFunc, "关机流程") == 0))
				{
					if((strcmp(strPropItem, "风机") == 0) || (strcmp(strPropItem, "回气") == 0))
					{
						CString strDelay3 = "";
						strDelay3.Format("DELETE FROM PropCHSH WHERE NO = %d", nNo);	//风机\回气单独删除，延时3单独删除
						m_pCon->Execute(_bstr_t(strDelay3),NULL,adCmdText);	
						g_pCanDlg->PropListLoad("PropCHSH", strFunc);
					}
				}

				CString strTem = "指令完成提示";
				strTem.Format("%s  第%d条指令完成", strFunc, nNo);
				Doing2Text(strTem);
				pEdit->SetWindowTextA(strTem);
				Sleep(1000);
			}
		}
		else
		{
			strTime = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("DWKD"));
			int nDelayNum = atoi(strItemNum);
			
			if( (nDelayNum == 2) && !g_bInTimer2 && (strcmp(strFunc, aSetFunc[0]) == 0) )	//只有开机流程的延时2进入温控，关机不进入
			{
				g_bInTimer2 = TRUE;

				LONG lThreadTimer2Exit = 0;
				GetExitCodeThread( g_hTimer2, (LPDWORD)&lThreadTimer2Exit);
				if(STILL_ACTIVE != lThreadTimer2Exit)	//线程尚未中断 
				{
					CWinThread* pThread;
					pThread = AfxBeginThread(ThreadProc_Timer2, pEdit, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
					g_hTimer2 = pThread->m_hThread;
				}
			}
			else
			{
				g_bInTimer2 = FALSE;
				
				if( (nDelayNum == 3) && (m_pRsProp != m_pRs_Delay3) )	//150929
				{
					nPropNO++;
					m_pRsProp->MoveNext();
					continue;					
				}

				TimerCmd(strFunc, nDelayNum, atoi(strTime), pEdit);	

				if(strcmp(strItemNum, "0") == 0)	//只有延时0置位该Flag
				{
					strSql.Format("UPDATE SetProp SET Flag = 1 WHERE Func = '%s' AND NO = %d", strFunc, nNo);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	

					//复位对应流程同分组非延时1
					CString strZuItem = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("ZuItem"));
					CString strZuNum = (LPCSTR)_bstr_t(m_pRsProp->GetCollect("ZuNum"));
					CString strFuncAnother = "";
					( strcmp(strFunc, aSetFunc[0]) == 0 )? strFuncAnother = aSetFunc[1]: strFuncAnother = aSetFunc[0];
					strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' AND Item = '延时' AND Num != 1", strFuncAnother);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
				}

				//151109 如果是停止延时3置位，则不删除
				if(!g_bStopDelay3 && (nDelayNum == 3) && (g_nFirstDelay3S < 1))
				{
					CString strDelay3 = "";
					strDelay3.Format("DELETE FROM PropCHSH WHERE NO = %d", nNo);	//延时删除
					m_pCon->Execute(_bstr_t(strDelay3),NULL,adCmdText);
					g_pCanDlg->PropListLoad("PropCHSH", strFunc);
					g_nFirstDelay3S = -12345;
				}

				CString strTem = "指令完成提示";
				strTem.Format("%s  第%d条指令完成    延时", strFunc, nNo);
				Doing2Text(strTem);
				pEdit->SetWindowTextA(strTem);
				Sleep(1000);				
			}
		}

		if(g_bInTimer2 || g_bStopProp)
			break;
		if(g_bStopKuYSJ && (m_pRsProp == m_pRs_ManualYSJ))	//单独退出
			break;
		if(g_bStopDelay3 && (m_pRsProp == m_pRs_Delay3))	//单独退出
			break;

		nPropNO++;
		m_pRsProp->MoveNext();
	}//while

	if(g_bStopProp || g_bStopKuYSJ)
	{	
		g_RcvData[2] = 0xFF;
		Sleep(1000);
		CString strTem = "指令停止提示";
		strTem.Format("%s  第%d条指令 终止", strFunc, nNo);
		Doing2Text(strTem);
		pEdit->SetWindowTextA(strTem);
		Sleep(1000);
	}
}

//
void KuOnOffExe(CString strFunc, CEdit *pEdit, int nKuIndex)
{
	CString strSql = "";
	//151110 停止延时3
	BOOL bDelay3Stoped = SP_Delay3(pEdit);

	//单独开关机，非延时3，将分液、回气、风机筛选到PropSomeKu
	CString aKuProp[4]={"分液", "回气", "压缩机", "风机"};

	//将该库房相关的分液、回气、风机筛选入PropSomeKu
	//注意双分液、回气屏蔽等
	strSql = "DELETE FROM PropSomeKu";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	int nPropStartIndex = 0;	//循环起始
	g_pStKuCtrl[nKuIndex].bOnlyFJ? nPropStartIndex = 2: nPropStartIndex = 0;	//151117

	//获得结构体数组序号后，即可获得该设备的编号
	for(int i=nPropStartIndex; i<4; i++)
	{
		if(g_bStopProp)	//如果停止流程，则终止
		{	
			break;
		}

		int nKuItemNum = 0;
		if(strcmp(aKuProp[i], "分液") == 0)
		{
			nKuItemNum = g_pStKuCtrl[nKuIndex].aFYNum[0];	//两个分液在同一个库房分组
			if(nKuItemNum == 0)
			{
				continue;
			}
		}
		else if(strcmp(aKuProp[i], "回气") == 0)
		{
			nKuItemNum = g_pStKuCtrl[nKuIndex].byHQNum;
			if(nKuItemNum == 0)
			{
				continue;
			}
		}
		else if(strcmp(aKuProp[i], "风机") == 0)
		{
			nKuItemNum = 0;		//风机存储为0，在动作时依库房自动判断
			
			if(g_pStKuCtrl[nKuIndex].byFJNum == 0)
			{
				continue;
			}
		}
		else if(strcmp(aKuProp[i], "压缩机") == 0)
		{
			//continue;	//150428,目前为止，库房没与压缩机单独关联，所以单独开关库房的时候，不动作压缩机
			KuLinkYSJ(strFunc, pEdit, nKuIndex);
		}

		if(i == 3)	//开、关机流程风机要确认选开、关
		{
			(strcmp(strFunc, "开机流程") == 0)?
				strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' AND Item = '%s' AND Num = %d AND NC = 1", 
					strFunc, aKuProp[i], nKuItemNum):
				strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' AND Item = '%s' AND Num = %d AND NC = 0", 
					strFunc, aKuProp[i], nKuItemNum);
		}
		else
		{
			strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' AND Item = '%s' AND Num = %d", 
				strFunc, aKuProp[i], nKuItemNum);
		}
		
		m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151028-1.1
		CString strKuItemZu = "";
		CString strKuItemZuNum = "";
		int nKuItemZuNum = 0;
		if(!m_pRs_OneOnOneOff->BOF)
		{	
			strKuItemZu = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("ZuItem"));
			strKuItemZuNum = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("ZuNum"));
			nKuItemZuNum = atoi(strKuItemZuNum);
		}
		m_pRs_OneOnOneOff->raw_Close();	//151028-1.2

		strSql.Format("SELECT * FROM PropSomeKu WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
			, strFunc, strKuItemZu, nKuItemZuNum);
		m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151028-2.1
		if(m_pRs_OneOnOneOff->BOF)
		{
			strSql.Format("INSERT INTO PropSomeKu\
						  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
						  FROM SetProp WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
						  , strFunc, strKuItemZu, nKuItemZuNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		m_pRs_OneOnOneOff->raw_Close();		//151028-2.2

		//双分液特殊处理，如果不是双分液，则删除此分液之前的
		if(i==0)
		{
			if( g_pStKuCtrl[nKuIndex].aFYNum[1] == 0)
			{
				strSql.Format("DELETE FROM PropSomeKu WHERE Item = '分液' AND Num != %d \
					AND Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
					, nKuItemNum, strFunc, strKuItemZu, nKuItemZuNum);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
		}
	}

	//处理PropSomeKu的风机，并执行
	strSql.Format("UPDATE PropSomeKu SET [Num] = %d WHERE Item = '风机'", g_pStKuCtrl[nKuIndex].byKuNum);
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	strSql = "DELETE FROM PropSomeKu WHERE (Item = '延时' AND Num = 2) OR (Item = '延时' AND Num = 3)";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	strSql = "UPDATE PropSomeKu SET [Flag] = 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	if(g_pCanDlg != NULL)
		g_pCanDlg->PropListLoad("PropSomeKu", strFunc);
	
	strSql.Format("SELECT * FROM PropSomeKu left JOIN SetCid \
				  ON PropSomeKu.Item = SetCid.IdItem AND PropSomeKu.Num = SetCid.IdItemNum \
				  WHERE Func = '%s' ORDER BY NO ASC", strFunc);	
	m_pRs_OneOnOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151109-1.1
	if(!m_pRs_OneOnOff->BOF)
	{
		g_bStopProp = FALSE;
		PropExecute(m_pRs_OneOnOff, strFunc, pEdit);
	}
	m_pRs_OneOnOff->raw_Close();	//151109-1.2

	//151110
	//然后重新启动延时3多线程函数-该函数在之前被停止，因为要重新加载数据集
	if(bDelay3Stoped)
	{
		ST_Delay3(pEdit);
	}

	if(g_pCanDlg != NULL)
	{
		g_bInDelay3 ?
			g_pCanDlg->PropListLoad("PropCHSH", "关机流程"):
			g_pCanDlg->PropListLoad("PropSomeKu", "清空流程");
	}	
}
void KuDelay3Exe(CString strFunc, CEdit *pEdit, int nKuIndex)
{
	SP_Delay3(pEdit);
	
	//将该库房相关的分液、回气、风机筛选入PropSomeKu
	//注意双分液、回气屏蔽等
	CString strSql = "";
	strSql = "DELETE FROM PropSomeKu";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	
	CString aSqlPropItem[4] = {"KuFY", "KuHQ", "KuYSJ", "KuFJ"};
	CString aKuProp[4]={"分液", "回气", "压缩机", "风机"};

	//获得结构体数组序号后，即可获得该设备的编号，并添加PropSomeKu
	for(int i=0; i<4; i++)
	{
		if(g_bStopProp)	//如果停止流程，则终止
		{	
			break;
		}

		int nKuItemNum = 0;
		if(strcmp(aKuProp[i], "分液") == 0)
		{
			nKuItemNum = g_pStKuCtrl[nKuIndex].aFYNum[0];	//两个分液在同一个库房分组
			if(nKuItemNum == 0)
			{
				continue;
			}
		}
		else if(strcmp(aKuProp[i], "回气") == 0)
		{
			nKuItemNum = g_pStKuCtrl[nKuIndex].byHQNum;
			if(nKuItemNum == 0)
			{
				continue;
			}
		}
		else if(strcmp(aKuProp[i], "风机") == 0)
		{
			nKuItemNum = 0;		//风机存储为0，在动作时依库房自动判断
			
			//150823
			if(g_pStKuCtrl[nKuIndex].byFJNum == 0)
			{
				continue;
			}
		}
		else if(strcmp(aKuProp[i], "压缩机") == 0)
		{
			//continue;
			KuLinkYSJ(strFunc, pEdit, nKuIndex);
		}

		(i == 3)?	//风机要确认选关
			strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' AND Item = '%s' AND Num = %d AND NC = 0", 
				strFunc, aKuProp[i], nKuItemNum):
			strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' AND Item = '%s' AND Num = %d", 
				strFunc, aKuProp[i], nKuItemNum);
		
		m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//111.1
		CString strKuItemZu = "";
		CString strKuItemZuNum = "";
		int nKuItemZuNum = 0;
		if(!m_pRs_OneOnOneOff->BOF)	//150914，关机流程中仅能有1条风机关
		{	
			strKuItemZu = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("ZuItem"));
			strKuItemZuNum = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("ZuNum"));
			nKuItemZuNum = atoi(strKuItemZuNum);
		}
		m_pRs_OneOnOneOff->raw_Close();	//111.2

		strSql.Format("SELECT * FROM PropSomeKu WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
			, strFunc, strKuItemZu, nKuItemZuNum);
		m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//112.1
		if(m_pRs_OneOnOneOff->BOF)
		{
			strSql.Format("INSERT INTO PropSomeKu\
						  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
						  FROM SetProp WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
						  , strFunc, strKuItemZu, nKuItemZuNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		m_pRs_OneOnOneOff->raw_Close();		//112.2			

		//双分液特殊处理，如果不是双分液，则删除此分液之前的
		if(i==0)
		{
			if( g_pStKuCtrl[nKuIndex].aFYNum[1] == 0)
			{
				strSql.Format("DELETE FROM PropSomeKu WHERE Item = '分液' AND Num != %d \
					AND Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
					, nKuItemNum, strFunc, strKuItemZu, nKuItemZuNum);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
		}		
	}

	//修改ID=时间戳作为后续标志-唯一
	COleDateTime mTimeST;
	mTimeST.SetDateTime(2015, 9, 1, 0, 0, 0);
	COleDateTimeSpan mTimeCAL;
	mTimeCAL = g_mTimeNow - mTimeST;
	int nTotalSenconds = (int)mTimeCAL.GetTotalSeconds();
	strSql = "SELECT * FROM PropSomeKu ORDER BY NO ASC";
	m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//113.1
	int nSomeKuIndex = 0;
	while(!m_pRs_OneOnOneOff->adoEOF)
	{
		CString strNO = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("NO"));
		int nNO = atoi(strNO);	

		strSql.Format("UPDATE PropSomeKu SET [ID] = %d WHERE NO = %d", nTotalSenconds+nSomeKuIndex, nNO);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		nSomeKuIndex++;
		m_pRs_OneOnOneOff->MoveNext();
	}
	m_pRs_OneOnOneOff->raw_Close();	//113.2

	//设置风机号为库房号
	strSql.Format("UPDATE PropSomeKu SET [Num] = %d WHERE Item = '风机'", g_pStKuCtrl[nKuIndex].byKuNum);
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	//获得延时3流程的NO
	int nDelay3NO = 0;
	int nDelay3S = 0;
	strSql = "SELECT * FROM PropSomeKu WHERE Item = '延时' AND Num = 3";
	m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//114.1
	if(!m_pRs_OneOnOneOff->BOF)
	{
		CString strDelay3NO = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("NO"));
		nDelay3NO = atoi(strDelay3NO);
		CString strDelay3S = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("DWKD"));
		nDelay3S = atoi(strDelay3S);
		m_pRs_OneOnOneOff->raw_Close();	//114.2.1
	}
	else
	{
		m_pRs_OneOnOneOff->raw_Close();	//114.2.2
		return;
	}

	//将延时3及以后的流程插入到PropCHSH中
	//启动库控，或执行完开机流程后，删除PropCHSH
	strSql = "SELECT * FROM PropCHSH";
	m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//115.1
	if(m_pRs_OneOnOneOff->BOF)
	{
		m_pRs_OneOnOneOff->raw_Close();	//115.2.1

		//直接插入并删除
		strSql.Format("INSERT INTO PropCHSH\
					  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
					  FROM PropSomeKu WHERE NO >= %d", nDelay3NO);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		strSql.Format("DELETE FROM PropSomeKu WHERE NO >= %d", nDelay3NO);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}
	else
	{
		m_pRs_OneOnOneOff->raw_Close();	//115.2.2

		strSql.Format("SELECT * FROM PropCHSH WHERE \
					  (ZuItem = '库房' AND ZuNum = %d) OR (Item = '风机' AND Num = %d)", 
					  g_pStKuCtrl[nKuIndex].byKuNum, g_pStKuCtrl[nKuIndex].byKuNum);
		m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//116.1
		if(m_pRs_OneOnOneOff->BOF)
		{
			m_pRs_OneOnOneOff->raw_Close();	//116.2.1

			//延时3流程执行过程中，每1s会更新；回气或风机执行，会暂停库控
			strSql = "SELECT * FROM PropCHSH WHERE Item = '延时' AND Num = 3 ORDER BY ID ASC";
			m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//117.1
			int nTotalS = 0;
			while(!m_pRs_OneOnOneOff->adoEOF)
			{
				CString strDelayS = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("DWKD"));
				int nDelayS = atoi(strDelayS);
				nTotalS += nDelayS;
				m_pRs_OneOnOneOff->MoveNext();
			}
			m_pRs_OneOnOneOff->raw_Close();	//117.2
			
			if(nDelay3S < nTotalS)	//为了简单，只允许向后插入
			{
				nDelay3S = nTotalS;
			}

			//获取最后一行流程信息
			strSql = "SELECT * FROM PropCHSH ORDER BY ID ASC";
			m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//118.1
			m_pRs_OneOnOneOff->MoveLast();
			CString strLastNO = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("NO"));
			int nLastNO = atoi(strLastNO);
			CString strLastID = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("ID")); 
			int nLastID = atoi(strLastID);
			m_pRs_OneOnOneOff->raw_Close();	//118.2

			//插入到最后并删除
			strSql.Format("INSERT INTO PropCHSH\
						  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
						  FROM PropSomeKu WHERE NO >= %d", nDelay3NO);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			strSql.Format("DELETE FROM PropSomeKu WHERE NO >= %d", nDelay3NO);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			//插入PropCHSH后，将NO编号
			strSql.Format("SELECT * FROM PropCHSH WHERE ID > %d ORDER BY ID ASC", nLastID);
			m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//119.1
			int nCHSHIndex = 1;
			while(!m_pRs_OneOnOneOff->adoEOF)
			{
				CString strID = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("ID"));
				int nID = atoi(strID);				
				strSql.Format("UPDATE PropCHSH SET [NO] = %d WHERE ID = %d", nLastNO+nCHSHIndex, nID);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
				nCHSHIndex++;
				m_pRs_OneOnOneOff->MoveNext();
			}
			m_pRs_OneOnOneOff->raw_Close();	//119.2

			//将最后插入的延时3秒数修改
			strSql = "SELECT * FROM PropCHSH WHERE Item = '延时' AND Num = 3 ORDER BY ID ASC";
			m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//120.1
			m_pRs_OneOnOneOff->MoveLast();
			CString strLastDelay3ID = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("ID"));
			int nLastDelay3ID = atoi(strLastDelay3ID);
			m_pRs_OneOnOneOff->raw_Close();	//120.2
			strSql.Format("UPDATE PropCHSH SET [DWKD] = %d WHERE ID = %d", nDelay3S-nTotalS, nLastDelay3ID);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		else
		{
			m_pRs_OneOnOneOff->raw_Close();	//116.2.2
		}
	}

	//置位Flag=0
	strSql = "UPDATE PropCHSH SET [Flag] = 0 ";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	strSql = "UPDATE PropSomeKu SET [Flag] = 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	//至此，PropSomeKu 和 PropCHSH分别存储了要执行的流程
	g_bStopDelay3 = FALSE;
	//首先执行完毕PropSomeKu中的流程
	if(g_pCanDlg != NULL)
		g_pCanDlg->PropListLoad("PropSomeKu", strFunc);
	strSql.Format("SELECT * FROM PropSomeKu left JOIN SetCid \
				  ON PropSomeKu.Item = SetCid.IdItem AND PropSomeKu.Num = SetCid.IdItemNum \
				  WHERE Func = '%s' ORDER BY NO ASC", strFunc);	
	m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//121.1
	if(!m_pRs_OneOnOneOff->BOF)
	{
		g_bStopProp = FALSE;
		PropExecute(m_pRs_OneOnOneOff, strFunc, pEdit);
	}
	m_pRs_OneOnOneOff->raw_Close();	//121.2
	//然后重新启动延时3多线程函数-该函数在之前被停止，因为要重新加载数据集
	ST_Delay3(pEdit);
}
UINT ThreadProc_Prop_Delay3(LPVOID wParam)
{
	g_bInDelay3 = TRUE;
	
	Doing2Text("执行延时3线程");

	//参数传递
	Prop_t *pStProp = (Prop_t *) wParam;
	CString strPropFunc = pStProp->Func;
	CEdit *pEdit = pStProp->pEdit;
	//int nKuNum = pStProp->byKuNum;

	g_pCanDlg->PropListLoad("PropCHSH", strPropFunc);
	CString strSql = "";
	strSql.Format("SELECT * FROM PropCHSH left JOIN SetCid \
				  ON PropCHSH.Item = SetCid.IdItem AND PropCHSH.Num = SetCid.IdItemNum \
				  WHERE Func = '%s' ORDER BY NO ASC", strPropFunc);	
	m_pRs_Delay3 = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//148.1
	if(!m_pRs_Delay3->BOF)
	{
		g_bStopDelay3 = FALSE;
		PropExecute(m_pRs_Delay3, strPropFunc, pEdit);
	}
	m_pRs_Delay3->raw_Close();	//148.2

	Doing2Text("终止延时3线程");

	g_bInDelay3 = FALSE;
	g_hDelay3 = NULL;
	//151103
	ST_CidRefer();

	return 0;
}
BOOL SP_Delay3(CEdit *pEdit)
{
	if(g_hDelay3 != NULL)
	{
		g_bStopDelay3 = TRUE;
		//循环等待延时3停止 -- 可能碰到正在执行风机、回气
		CString str = "";
		int nWaitDelay3 = 0;
		do
		{
			str.Format("等待延时操作终止 %d秒", 45-nWaitDelay3);
			pEdit->SetWindowTextA(str);
			Sleep(1000);
			nWaitDelay3++;

			if(g_bStopKuCtrl)	return FALSE;

			if(nWaitDelay3 > 45)
			{
				Auto_UrgentStop("延时操作终止失败");
				return FALSE;
			}
		}
		while(g_hDelay3 != NULL);

		//因为可能有暂停，确保终止
		if(g_nFirstDelay3S != -12345)	//说明延时3正在读秒中
		{
			CString strSql = "";
			strSql = "SELECT * FROM PropCHSH WHERE Item = '延时' AND Num = 3 ORDER BY ID ASC";
			m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//110.1
			if(!m_pRs_OneOnOneOff->BOF)
			{	
				CString strFirstID = (LPCSTR)_bstr_t(m_pRs_OneOnOneOff->GetCollect("ID"));
				int nFirstID = atoi(strFirstID);
				strSql.Format("UPDATE PropCHSH SET [DWKD] = %d WHERE ID = %d", g_nFirstDelay3S, nFirstID);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
			m_pRs_OneOnOneOff->raw_Close();	//110.2
		}

		return TRUE;
	}
	else
		return FALSE;
}
void ST_Delay3(CEdit *pEdit)
{
	g_StProp.Func = "关机流程";
	g_StProp.pEdit = pEdit;
	g_StProp.byKuNum = 0;//那个库房触发的流程执行
	
	CWinThread* pThread;
	int i = 0;
	for(i=0; i<5; i++)
	{
		LONG lThreadDelay3Exit = 0;
		GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);
		if(STILL_ACTIVE != lThreadDelay3Exit)
		{
			pThread = AfxBeginThread(ThreadProc_Prop_Delay3, &g_StProp, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			g_hDelay3 = pThread->m_hThread;
			break;
		}
		Sleep(1000);
	}
	if(i == 5)
	{
		Auto_UrgentStop("延时3 启动失败");
	}
}
//
UINT ThreadProc_Prop_AllOnAllOff(LPVOID wParam)	//同开同关，同开;修改该工作模式下库房的工作状态：开机、关机
{	
	LONG lThreadDelay3Exit = 0;
	GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);
	if(STILL_ACTIVE == lThreadDelay3Exit)
	{
		g_bStopDelay3 = TRUE;	//终止延时3线程
		Sleep(3000);
	}
	
	SP_CidRefer();
	Sleep(500);
	g_pCanDlg->ST_TempRefer();	//开关机流程执行过程中，温度遍历继续

	//参数传递
	Prop_t *pStProp = (Prop_t *) wParam;
	CString strPropFunc = pStProp->Func;
	CEdit *pEdit = pStProp->pEdit;
	int nKuNum = pStProp->byKuNum;	//区分时开or温开
	CString strSqlProp = pStProp->strSqlProp;
	CString strKuNum = pStProp->strKuNum;

	CButton *pCheckYSJManual = (CButton *)g_pCanDlg->GetDlgItem(IDC_CHECK_YSJ_MANUAL);

	g_byKuOnSum = 0;	//开始执行开、关机流程，直接复位
	if(strcmp(strPropFunc, "关机流程") == 0)
	{
		//开始执行关机流程，延时2线程应该停止
		LONG lThreadTimer2Exit = 0;
		GetExitCodeThread( g_hTimer2, (LPDWORD)&lThreadTimer2Exit);
		if(STILL_ACTIVE == lThreadTimer2Exit)	//线程尚未中断
		{
			g_bStopProp = TRUE;
			Sleep(3000);
			pEdit->SetWindowTextA("要关机，停止延时2流程，等待5s");
			Doing2Text("要关机，停止延时2流程，等待5s");
			Sleep(5000);
		}

		if(pCheckYSJManual->GetCheck())
		{
			pCheckYSJManual->SetCheck(FALSE);
			g_pCanDlg->PostMessage( WM_COMMAND, MAKEWPARAM(IDC_CHECK_YSJ_MANUAL, BN_CLICKED), NULL);
		}
		pCheckYSJManual->EnableWindow(FALSE);
	}
	else 
	{
		g_bStopProp = FALSE;
		g_byYSJOnSum = 0;	//151112

		//开机流程查看系统运行时间
		if(!g_bAuthorAll && (strcmp(strPropFunc, "开机流程") == 0) )
		{
			Lock_OnPropAuthorRunning(pEdit);
			if(!g_bSysCanRun)
				return FALSE;
		}

		if(g_byPropSMS_SetKu)
		{
			//发送开机短信
			Msg_OnProp();
		}
	}

	//开始执行流程
	CString strStart = "开始执行：";
	if(!g_bUrgentStop)
	{
		strStart += strPropFunc;

		//开始执行开机流程，启动定时保护
		if(g_nTimeS_SetKu && (strcmp(strPropFunc, "开机流程") == 0) )
		{
			TimeS_OnPropST();
		}
	}
	else
	{
		strStart += "紧急关机流程";
	}	
	pEdit->SetWindowTextA(strStart);
	Doing2Text(strStart);
	Sleep(1000);

	//为防止数据库执行错误导致原先流程设置被修改，为简化恢复原先流程设置，将流程复制到另外一张表上执行
	CString strSql = "";
	strSql.Format("DELETE PropTem \
				  INSERT INTO PropTem SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
				  FROM %s WHERE Func = '%s' ORDER BY NO ASC", strSqlProp, strPropFunc);
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	//添加风机，只添加在用的库房对应的风机 //最多添加的风机数//150807
	if(strcmp(strPropFunc, "开机流程") == 0)
	{
		if(g_nCtrlMode == 1)
		{
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1";
		}
		else if(g_nCtrlMode == 2)
		{
			if(g_bOneKeyStart)
			{
				strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1";
			}
			else
			{
				if(nKuNum == 0)	//时开温关
				{
					strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuCtrlMode = '时开温关'";
				}
				else	//某个库房
				{
					//151203
					//strSql.Format("SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuNum = %d", nKuNum);
					strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuCtrlMode = '温开温关'";
				}
			}
		}
		else 
		{
			Doing2Text("开机流程控制模式错误");
			return FALSE;
		}
	}
	else if(strcmp(strPropFunc, "关机流程") == 0)
	{
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1";
	}
	m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//135.1
	int nKuUseSum = 0;
	while(!m_pRs_AllOnAllOff->adoEOF)
	{
		nKuUseSum ++;
		m_pRs_AllOnAllOff->MoveNext();
	}
	m_pRs_AllOnAllOff->raw_Close();	//135.2

	//151106
	if(strcmp(strKuNum, "全部库房") == 0)
	{
		g_byKuOnSum = nKuUseSum;	//暂时等于在用库房总数
	}
	else
	{
		//151117 - 单独某个库房温度到导致开机
		//(strKuNum.GetLength() > 2)?
		//	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND PartKu = 1 ORDER BY KuNum ASC":
		//	strSql.Format("SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuNum = %d", atoi(strKuNum));

		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND PartKu = 1 ORDER BY KuNum ASC";
		m_pRs_AllOnAllOff_In = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151106-3.1
		int nIndex = 0;
		while(!m_pRs_AllOnAllOff_In->adoEOF)
		{
			nIndex ++;
			m_pRs_AllOnAllOff_In->MoveNext();
		}
		m_pRs_AllOnAllOff_In->raw_Close();	//151106-3.2
		g_byKuOnSum = nIndex;
	}

	//风机处理方法：删除该条风机流程，增加相同模式下的所有风机流程，之后再删除
	//【3-1-风机在特殊分组】开机流程风机开、关机流程风机关，均在特殊分组
	strSql = "SELECT * FROM PropTem WHERE Item = '风机'";	//150825-FJ-11 风机可在任意分组
	m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//136.1	//150825-FJ-0 m_pRs_AllOnAllOff
	BYTE bySpecialFJ = 0;	//风机流程的个数
	while(!m_pRs_AllOnAllOff->adoEOF)	//150825-FJ-2 风机开或关放在不同的特殊分组，分别处理
	{		
		CString strFJPropNO = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("NO"));	//150825-FJ-7
		int nFJPropNO = atoi(strFJPropNO);

		//150825-FJ-10
		if(bySpecialFJ)
		{
			nFJPropNO = nFJPropNO + (nKuUseSum-1)*bySpecialFJ;
		}

		CString strFJPropID = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("ID"));	//150825-FJ-8
		int nFJPropID = atoi(strFJPropID);
		CString strFJNC = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("NC"));	//150825-FJ-9
		BYTE byFJNC = atoi(strFJNC);

		//从后往前修改序号
		strSql.Format("SELECT * FROM PropTem WHERE Func = '%s' ORDER BY NO ASC", strPropFunc);
		m_pRs_AllOnAllOff_In = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//141.1
		m_pRs_AllOnAllOff_In->MoveLast();
		while(!m_pRs_AllOnAllOff_In->BOF)
		{
			CString strMyNO = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff_In->GetCollect("NO"));
			int nMyNO = atoi(strMyNO);
			strSql.Format("UPDATE PropTem SET [NO] = %d WHERE NO = %d AND NO > %d and Func = '%s'"
				,nMyNO+nKuUseSum-1, nMyNO, nFJPropNO, strPropFunc);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			m_pRs_AllOnAllOff_In->MovePrevious();
		}
		m_pRs_AllOnAllOff_In->raw_Close();	//141.2

		if(strcmp(strKuNum, "全部库房") == 0)
		{//如果为全部库房，则增加在用库房号对应的风机 = nKuUseSum //150807
			if(strcmp(strPropFunc, "开机流程") == 0)
			{
				if(g_nCtrlMode == 1)
				{
					strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 ORDER BY KuNum ASC";
				}
				else if(g_nCtrlMode == 2)
				{
					if(g_bOneKeyStart)
					{
						strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 ORDER BY KuNum ASC";
					}
					else
					{
						if(nKuNum == 0)	//时开温关
						{
							strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuCtrlMode = '时开温关' ORDER BY KuNum ASC";
						}
						else	//某个库房
						{
							//strSql.Format("SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuNum = %d", nKuNum);
							//151203
							//strSql.Format("SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuNum = %d", nKuNum);
							strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuCtrlMode = '温开温关' ORDER BY KuNum ASC";
						}
					}
				}
				else 
				{
					Doing2Text("开机流程控制模式错误");
					return FALSE;
				}
			}
			else if(strcmp(strPropFunc, "关机流程") == 0)
			{
				strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 ORDER BY KuNum ASC";
			}				
		}
		else
		{	//如果为部分库房，则增加部分库房中在用库号对应的风机
			//151117 - 单独某个库房温度到导致开机
			//(strKuNum.GetLength() > 2)?
			//	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND PartKu = 1 ORDER BY KuNum ASC":
			//	strSql.Format("SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuNum = %d", atoi(strKuNum));

			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND PartKu = 1 ORDER BY KuNum ASC";
		}
		m_pRs_AllOnAllOff_In = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//142.1
		int nAddFJPropNO = nFJPropNO;
		int nIndex = 1;
		while(!m_pRs_AllOnAllOff_In->adoEOF)
		{
			CString strAddKuNum = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff_In->GetCollect("KuNum"));
			int nAddKuNum = atoi(strAddKuNum);

			strSql.Format("INSERT INTO PropTem VALUES(%d,'%s',%d,'风机',0,'选开关',-1,%d,'库房',%d,%d)"
				,nIndex,strPropFunc,nAddFJPropNO,byFJNC,nAddKuNum,FALSE);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			nIndex ++;
			nAddFJPropNO++;
			m_pRs_AllOnAllOff_In->MoveNext();
		}
		m_pRs_AllOnAllOff_In->raw_Close();	//142.2
	
		//删除原有风机流程
		strSql.Format("DELETE FROM PropTem WHERE ID = %d",nFJPropID);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		m_pRs_AllOnAllOff->MoveNext();	//150825-FJ-5
		bySpecialFJ++;
	}
	m_pRs_AllOnAllOff->raw_Close();	//136.2	//150825-FJ-6	

	//【3-2-经过上述操作后，风机都在库房分组】
	//若风机在库房分组，将其直接置为库房设置的风机编号
	strSql = "SELECT * FROM PropTem left JOIN SetKu ON PropTem.ZuNum = SetKu.KuNum \
				  WHERE ZuItem = '库房' AND Item = '风机'";
	m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//137.1
	while(!m_pRs_AllOnAllOff->adoEOF)
	{
		CString strID = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("ID"));
		int nID = atoi(strID);
		CString strFJNum = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("KuFJ"));
		int nFJNum = atoi(strFJNum);
		strSql.Format("UPDATE PropTem SET Num = %d WHERE ID = %d", nFJNum, nID);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		m_pRs_AllOnAllOff->MoveNext();
	}
	m_pRs_AllOnAllOff->raw_Close();	//137.2
	
	//150823 如果SetKu风机为屏蔽，则删除
	strSql = "DELETE FROM PropTem WHERE Item = '风机' AND Num = 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	//同开最多数--开启相应数字在用库房分组，删除其他--开机流程,在用库房大于同开最多
	//150908 温开温关亦同开最多
	//151027 部分库房模式不支持同开最多，所以只有全部库房的时候需要进行下列判断
	BYTE *aKuOffNum = NULL;
	aKuOffNum = (BYTE *)malloc(g_byKuSum_SetKu * sizeof(BYTE));
	memset(aKuOffNum, 0, g_byKuSum_SetKu * sizeof(BYTE));
	if( (g_byKuOnMax_SetKu < g_byKuSum_SetKu) && (nKuUseSum > g_byKuOnMax_SetKu) 
		&& (strcmp(strPropFunc, "开机流程") == 0) && (strcmp(strKuNum, "全部库房") == 0) )
	{
		if(g_hKuCtrl != NULL)//库房控制过程中，而不是试行开关机流程
		{					
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 ORDER BY KuNum ASC";
			m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//138.1
			int nKuNumIndex = 0;
			int nKuFirstUse = 0;	//150908大于最多数的库房触发的开机流程
			while(!m_pRs_AllOnAllOff->adoEOF)
			{
				nKuNumIndex++;
				CString strKuNumNeed = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("KuNum"));
				int nKuNumNeed = atoi(strKuNumNeed);

				if(nKuNumIndex == 1)
				{
					nKuFirstUse = nKuNumNeed;	//后续如果要删除前一个库，就是第一个库
				}

				if(nKuNumIndex > g_byKuOnMax_SetKu)
				{			
					if((nKuNumIndex > g_byKuOnMax_SetKu) && (nKuNum == nKuNumNeed))
					{
						nKuNumNeed = nKuFirstUse;
					}

					if(nKuNum != nKuNumNeed)
					{
						//151113，不应该看要开机库房的只风机，要看第一个库房的只风机
						int nKuIndex = 0;
						(nKuNumNeed == nKuFirstUse)? nKuIndex = 0: nKuIndex = nKuNumIndex-1;				
						(!g_pStKuCtrl[nKuIndex].bOnlyFJ)?	//不是只动作风机，则后续单独开关机时会动作分液回气
							strSql.Format("DELETE FROM PropTem WHERE ZuItem = '库房' AND ZuNum = %d", nKuNumNeed):
							strSql.Format("DELETE FROM PropTem WHERE Item = '风机' AND ZuItem = '库房' AND ZuNum = %d", nKuNumNeed);
						m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			

						CString strDeleteKuZu = "";
						strDeleteKuZu.Format("同开最多，暂时不开库房：%d", nKuNumNeed);
						Doing2Text(strDeleteKuZu);

						aKuOffNum[nKuNumIndex-g_byKuOnMax_SetKu-1] = nKuNumNeed;
					}
				}

				m_pRs_AllOnAllOff->MoveNext();
			}
			m_pRs_AllOnAllOff->raw_Close();	//138.2		
		}
		g_byKuOnSum = g_byKuOnMax_SetKu;	//暂时等于同开最多数
	}

	//150924 ↓为异开
	if(!g_bOnProp_MaxOn && (nKuNum!=0) && (strcmp(strPropFunc, "开机流程") == 0))	
	{
		for(int i=0; i<g_byKuCtrlSum_St; i++)
		{	
			if(g_pStKuCtrl[i].byKuNum != nKuNum)
			{
				(g_pStKuCtrl[i].bOnlyFJ)? 
				strSql.Format("DELETE FROM PropTem WHERE Item = '风机' AND ZuItem = '库房' AND ZuNum = %d", g_pStKuCtrl[i].byKuNum):
				strSql.Format("DELETE FROM PropTem WHERE ZuItem = '库房' AND ZuNum = %d", g_pStKuCtrl[i].byKuNum);
			}
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		g_byKuOnSum = 1;	//暂时等于1
	}	

	//151104 删除关机流程的压缩机已关
	YSJDelOffProp(strPropFunc);
	//151106 开机流程根据将要开启的库房数确定压缩机
	YSJDelOnProp(strPropFunc);

	//赵县新星专属
	//if( (strcmp(g_pStCS[g_nCS_Index].strLoginName, "赵县新星") == 0) && (strcmp(strPropFunc, "关机流程") == 0) )
	//{
	//	strSql = "DELETE FROM PropTem WHERE Item = '风机' AND Num < 4 AND NC = 1 AND Func = '关机流程'";
	//	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);		
	//}

	//150912 - 关机流程删除延时3
	strSql = "DELETE FROM PropTem WHERE Item = '延时' AND Num = 3";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	//151014
	strSql = "UPDATE PropTem SET Flag = 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	g_pCanDlg->PropListLoad("PropTem", strPropFunc);
	//打开流程，依靠数据库方式自动依次执行
	if(!g_bUrgentStop)
	{
		strSql.Format("SELECT * FROM PropTem left JOIN SetCid \
			ON PropTem.Item = SetCid.IdItem AND PropTem.Num = SetCid.IdItemNum \
			WHERE Func = '%s' ORDER BY NO ASC", strPropFunc);	
	}
	else
	{
		strSql.Format("SELECT * FROM PropTem left JOIN SetCid \
			ON PropTem.Item = SetCid.IdItem AND PropTem.Num = SetCid.IdItemNum \
			WHERE Func = '%s' AND ZuItem != '库房' OR Func = '%s' AND ZuItem = '库房' AND Item = '风机' \
			ORDER BY NO ASC", strPropFunc, strPropFunc);
	}

	m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//139.1

	g_bStopDelay3 = FALSE;	//150915 保证流程执行
	g_bStopProp = FALSE;
	if(strcmp(strPropFunc, "关机流程") == 0)
	{
		if(!m_pRs_AllOnAllOff->BOF)
		{
			Doing2Text("顺序执行关机流程");
			PropExecute(m_pRs_AllOnAllOff, strPropFunc, pEdit);
		}	
	}
	else
	{
		if(g_bStopKuCtrl) g_bStopProp = TRUE;

		if(!m_pRs_AllOnAllOff->BOF && !g_bStopKuCtrl)
		{		
			if(g_pStCS[g_nCS_Index].bFlagEnMusic)
			{
				g_pCanDlg->PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_WMP_ST, BN_CLICKED), NULL);
				Sleep(1000);
			}
			
			Doing2Text("顺序执行开机流程");
			PropExecute(m_pRs_AllOnAllOff, strPropFunc, pEdit);

			g_bStopAuto = FALSE;
		}	
	}

	m_pRs_AllOnAllOff->raw_Close();	//139.2	//关闭PropExecute中数据库

	//执行完毕或终止
	CString strStop = "";
	g_bStopProp? 
		strStop.Format("%02d:%02d 终止执行：", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute()): 
		strStop.Format("%02d:%02d 执行完毕：", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute());
	strStop += strPropFunc;
	pEdit->SetWindowTextA(strStop);	
	Doing2Text(strStop);
	Sleep(1000);

	//关机流程执行结束，结束本次定时保护
	if(strcmp(strPropFunc, "关机流程") == 0)
	{		
		if(g_byPropSMS_SetKu)
		{
			//发送开机短信
			Msg_OffProp();
		}
		
		g_bFlagAllYSJOff = TRUE;	//150807
		g_byYSJOnSum = 0;	//151112	
		g_bTimeProtectOUT = TRUE;

		if(!g_bUrgentStop)	//非紧急关机
		{
			if(g_pStCS[g_nCS_Index].bFlagEnMusic)
			{
				g_pCanDlg->PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_WMP_SP, BN_CLICKED), NULL);
				Sleep(1000);	
			}
		}
	}
	else if(strcmp(strPropFunc, "开机流程") == 0)
	{
		g_bFlagAllYSJOff = FALSE;
		//150914 - PropCHSH用于单独关机
		strSql = "DELETE FROM PropCHSH";
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		//151103测试功能
		if(!g_bInTimer2)
		{
			pCheckYSJManual->SetCheck(FALSE);
			pCheckYSJManual->EnableWindow(TRUE);	//手动开关压缩机
		}
	}

	if(!g_bInTimer2)
		g_pCanDlg->PropListLoad("SetProp", "清空流程");

	//测试
	g_pCanDlg->GetDlgItem(IDC_BUTTON_START_ON)->EnableWindow(TRUE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_START_OFF)->EnableWindow(TRUE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_ON)->EnableWindow(FALSE);

	//控制
	if( g_bUrgentStop || (strcmp(strPropFunc, "关机流程") == 0) )
	{
		g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(FALSE);	//一键关机，执行完关机流程后,就不要再点击了
		g_pCanDlg->GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(FALSE);	//紧急关机
		//151014
		if(g_hKuCtrl == NULL)
		{
			g_pCanDlg->GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(TRUE);
			g_pCanDlg->GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(TRUE);
			g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(FALSE);
			g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(FALSE);
			g_pCanDlg->GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(TRUE);
			g_pCanDlg->GetDlgItem(IDC_BUTTON_SET)->EnableWindow(TRUE);
			g_pCanDlg->GetDlgItem(IDC_BUTTON_CX_TEXT)->EnableWindow(TRUE);
			g_bUrgentING = FALSE;
			g_bUrgentStop = FALSE;
		}
	}
	else
	{
		g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(TRUE);	//一键关机，执行完开机流程后，使能
		g_pCanDlg->GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(TRUE);	//紧急关机，是执行完全部流程后	
	}

	//同开同关的开关机or同开异关的开机，只要执行完流程，就可置位库房状态--根据库房分组个数
	if(!g_bStopProp)
	{
		//150624
		strSql = "SELECT * FROM PropTem left JOIN SetKu \
				ON PropTem.ZuNum = SetKu.KuNum \
				WHERE PropTem.ZuItem = '库房'\
				AND SetKu.KuNum > 0 AND SetKu.KuStatus = 1";
		m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//140.1
		//151116 无库房分组的处理
		if(m_pRs_AllOnAllOff->BOF)
		{
			m_pRs_AllOnAllOff->raw_Close();	//140.2.1
			if(strcmp(strPropFunc, "关机流程") == 0)
			{
				strSql = "UPDATE SetKu SET WorkStatus = 0 WHERE KuNum > 0 AND KuStatus = 1";
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
				strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1";
				m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151116-1.1

				if(g_pStKuCtrl != NULL)
				{
					for(int i=0; i<g_byKuCtrlSum_St; i++)	//151117
					{
						g_pStKuCtrl[i].bFlagOn = FALSE;
					}				
				}

				while(!m_pRs_AllOnAllOff->adoEOF)
				{
					CString strExeKuNum = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("KuNum"));
					int nExeKuNum = atoi(strExeKuNum);
					KuWorkStatusListDisp(nExeKuNum, 0, FALSE);
					m_pRs_AllOnAllOff->MoveNext();
				}
				m_pRs_AllOnAllOff->raw_Close();	//151116-1.2
			}
			else
			{
				if(strcmp(strKuNum, "全部库房") == 0)
				{
					strSql = "UPDATE SetKu SET WorkStatus = 1 WHERE KuNum > 0 AND KuStatus = 1";
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
					strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1";
					m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151116-2.1
					
					if(g_pStKuCtrl != NULL)
					{
						for(int i=0; i<g_byKuCtrlSum_St; i++)	//151117
						{
							g_pStKuCtrl[i].bFlagOn = TRUE;
						}					
					}

					while(!m_pRs_AllOnAllOff->adoEOF)
					{
						CString strExeKuNum = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("KuNum"));
						int nExeKuNum = atoi(strExeKuNum);
						KuWorkStatusListDisp(nExeKuNum, 1, FALSE);
						m_pRs_AllOnAllOff->MoveNext();
					}
					m_pRs_AllOnAllOff->raw_Close();	//151116-2.2
				}
				else
				{
					strSql = "UPDATE SetKu SET WorkStatus = 1 WHERE KuNum > 0 AND KuStatus = 1 AND PartKu = 1";
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
					strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND PartKu = 1";
					m_pRs_AllOnAllOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151116-3.1
					while(!m_pRs_AllOnAllOff->adoEOF)
					{
						CString strExeKuNum = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("KuNum"));
						int nExeKuNum = atoi(strExeKuNum);

						if(g_pStKuCtrl != NULL)	//151125
						{
							int nExeKuIndex = GetKuIndex_St(nExeKuNum, FALSE);	//151117
							g_pStKuCtrl[nExeKuIndex].bFlagOn = TRUE;
						}

						KuWorkStatusListDisp(nExeKuNum, 1, FALSE);
						m_pRs_AllOnAllOff->MoveNext();
					}
					m_pRs_AllOnAllOff->raw_Close();	//151116-3.2
				}
			}
		}
		else
		{
			while(!m_pRs_AllOnAllOff->adoEOF)
			{
				CString strExeFunc = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("Func"));
				CString strExeKuNum = (LPCSTR)_bstr_t(m_pRs_AllOnAllOff->GetCollect("KuNum"));
				int nExeKuNum = atoi(strExeKuNum);
				int nExeKuIndex = 0;
				if(g_pStKuCtrl != NULL)	//151125
					nExeKuIndex = GetKuIndex_St(nExeKuNum, FALSE);	//151117

				if( strcmp(strExeFunc, aSetFunc[0]) == 0 )	//开机流程
				{
					KuWorkStatusListDisp(nExeKuNum, 1, FALSE);
					strSql.Format("UPDATE SetKu SET WorkStatus = 1 WHERE KuNum = %d AND KuNum > 0", nExeKuNum);					
					if(g_pStKuCtrl != NULL)
						g_pStKuCtrl[nExeKuIndex].bFlagOn = TRUE;	//151117
					//150908
					for(int i=0; i<g_byKuSum_SetKu; i++)
					{
						if(aKuOffNum[i] == nExeKuNum)
						{
							KuWorkStatusListDisp(nExeKuNum, 0, FALSE);
							strSql.Format("UPDATE SetKu SET WorkStatus = 0 WHERE KuNum = %d AND KuNum > 0", nExeKuNum);
							if(g_pStKuCtrl != NULL)
								g_pStKuCtrl[nExeKuIndex].bFlagOn = FALSE;	//151117
							break;
						}
					}		
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);						
				}
				else if(strcmp(strExeFunc, aSetFunc[1]) == 0)	//关机流程
				{
					strSql.Format("UPDATE SetKu SET WorkStatus = 0 WHERE KuNum = %d AND KuNum > 0", nExeKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
					if(g_pStKuCtrl != NULL)
						g_pStKuCtrl[nExeKuIndex].bFlagOn = FALSE;	//151117
					KuWorkStatusListDisp(nExeKuNum, 0, FALSE);				
				}

				m_pRs_AllOnAllOff->MoveNext();
			}
			m_pRs_AllOnAllOff->raw_Close();	//140.2.2
		}
	}

	//150908
	if(aKuOffNum != NULL)
		free(aKuOffNum);

	if(g_bUrgentStop)
	{
		strSql = "UPDATE SetKu SET WorkStatus = 0";
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	

		KuWorkStatusListDisp(-1, 0, TRUE);
	}

	ST_KuCtrl();
	
	g_hPropING = NULL;	
	//151103
	ST_CidRefer();
	
	return 0;
}

UINT ThreadProc_Prop_OneOnOneOff(LPVOID wParam)
{
	LONG lThreadDelay3Exit = 0;
	GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);
	if(STILL_ACTIVE != lThreadDelay3Exit)
	{
		SP_CidRefer();
	}
	
	g_bStopProp = FALSE; //复位标志
	g_bFlagPropING_Wait = TRUE;

	//参数传递
	Prop_t *pStProp = (Prop_t *) wParam;
	CString strPropFunc = pStProp->Func;
	CEdit *pEdit = pStProp->pEdit;
	int nKuNum = pStProp->byKuNum;

	//执行前，↓，风机流程是否执行标志的特殊处理
	CString strSql = "";
	if(strcmp(strPropFunc, aSetFunc[0]) == 0)	//开机流程
	{
		//风机的特殊处理
		//异开异关模式下，开机，如果还有未开机，则将风机流程执行状态改为0，在库房分组内除外
		strSql = "SELECT * FROM SetKu WHERE KuStatus = 1 AND WorkStatus = 0 AND KuWorkMode Like '%异关'";
		m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//106.1		
		if(!m_pRs_OneOnOneOff->BOF)
		{
			strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' AND Item = '风机' AND Num = 0 AND ZuItem != '库房'", strPropFunc);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		m_pRs_OneOnOneOff->raw_Close();	//106.2
	}
	else	//关机流程
	{
		//风机的特殊处理
		//异开异关模式下，关机，如果还有未关机，则将风机流程执行状态改为0，在库房分组内除外
		strSql = "SELECT * FROM SetKu WHERE KuStatus = 1 AND WorkStatus = 1 AND KuWorkMode Like '%异关'";
		m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//107.1			
		if(!m_pRs_OneOnOneOff->BOF)
		{
			//141221
			strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' \
						  AND Item = '风机' AND Num = 0 AND NC = 0 AND ZuItem != '库房'", strPropFunc);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		m_pRs_OneOnOneOff->raw_Close();	//107.2
	}	

	//开始执行流程
	CString strStart = "";
	(strcmp(strPropFunc, aSetFunc[0]) == 0)? 
		strStart.Format("开始库房%d开机", nKuNum): strStart.Format("开始库房%d关机", nKuNum);
	pEdit->SetWindowTextA(strStart);
	Doing2Text(strStart);
	Sleep(1000);

	//查找引发流程执行的库房在结构体数组中的序号
	int nKuIndex = GetKuIndex_St(nKuNum, TRUE);

	//执行库房控制
	if(!g_bStopKuCtrl)
	{
		if( (strcmp(strPropFunc, "关机流程") == 0) && g_pStKuCtrl[nKuIndex].bDelay3 )
		{
			KuDelay3Exe(strPropFunc, pEdit, nKuIndex);
		}
		else
		{
			KuOnOffExe(strPropFunc, pEdit, nKuIndex);
		}
	}

	//执行完毕
	CString strStop = "";
	g_bStopProp? strStop.Format("%02d:%02d 终止执行：库房%d ", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), nKuNum):
				 strStop.Format("%02d:%02d 执行完毕：库房%d ", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), nKuNum);
	strStop += strPropFunc;
	pEdit->SetWindowTextA(strStop);
	Doing2Text(strStop);
	Sleep(1000);

	//执行完毕流程后，相应库房运行状态
	if(!g_bStopProp)
	{
		if(strcmp(strPropFunc, aSetFunc[0]) == 0)	//开机流程
		{
			strSql.Format("UPDATE SetKu SET WorkStatus = 1 WHERE KuNum = %d AND KuNum > 0", nKuNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			KuWorkStatusListDisp(nKuNum, 1, FALSE);

			//执行后，↑，风机的特殊处理
			//异开异关模式下，开机，如果还有未开机，则将风机流程执行状态改为0，在库房分组内除外
			strSql = "SELECT * FROM SetKu WHERE KuStatus = 1 AND WorkStatus = 0 AND KuWorkMode Like '%异关'";
			m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//108.1			
			if(!m_pRs_OneOnOneOff->BOF)
			{
				strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' AND Item = '风机' AND Num = 0 AND ZuItem != '库房'", strPropFunc);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
			m_pRs_OneOnOneOff->raw_Close();	//108.2
		}
		else	//关机流程
		{
			strSql.Format("UPDATE SetKu SET WorkStatus = 0 WHERE KuNum = %d", nKuNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			KuWorkStatusListDisp(nKuNum, 0, FALSE);

			//风机的特殊处理
			//异开异关模式下，关机，如果还有未关机，则将风机流程执行状态改为0，在库房分组内除外
			strSql = "SELECT * FROM SetKu WHERE KuStatus = 1 AND WorkStatus = 1 AND KuWorkMode Like '%异关'";
			m_pRs_OneOnOneOff = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//109.1			
			if(!m_pRs_OneOnOneOff->BOF)
			{
				strSql.Format("UPDATE SetProp SET Flag = 0 WHERE Func = '%s' \
							  AND Item = '风机' AND Num = 0 AND NC = 0 AND ZuItem != '库房'", strPropFunc);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
			m_pRs_OneOnOneOff->raw_Close();	//109.2
		}	

		//修改库房开机数
		(strcmp(strPropFunc, "开机流程") == 0)?	g_byKuOnSum++:	g_byKuOnSum--;
		KuYSJ(pEdit);
	}

	//151107
	Sleep(1000);
	if(g_hKuYSJ == NULL)
	{
		ST_KuCtrl();
	}

	g_hPropING = NULL;
	g_bFlagPropING_Wait = FALSE;
	//151103
	ST_CidRefer();

	return 0;
}

//
void AddUrgentStopProp(CString strItem, int nItemNum)
{
	if(strcmp(strItem, "风机") == 0)
	{
		nItemNum = 0;
	}

	CString strSql = "";
	strSql.Format("SELECT * FROM SetProp WHERE Func = '关机流程' AND Item = '%s' AND Num = %d"
		, strItem, nItemNum );
	if(strcmp(strItem, "风机") == 0)
	{
		strSql = "SELECT * FROM SetProp WHERE Func = '关机流程' AND Item = '风机' AND Num = 0 AND NC = 0";
	}
	m_pRs_UrgentProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//60.1

	if(!m_pRs_UrgentProp->BOF)
	{
		CString strKuItemZu = (LPCSTR)_bstr_t(m_pRs_UrgentProp->GetCollect("ZuItem"));
		CString strKuItemZuNum = (LPCSTR)_bstr_t(m_pRs_UrgentProp->GetCollect("ZuNum"));
		int nKuItemZuNum = atoi(strKuItemZuNum);
		m_pRs_UrgentProp->raw_Close();	//60.2.1

		strSql.Format("SELECT * FROM PropUrgent WHERE Func = '关机流程' AND ZuItem = '%s' AND ZuNum = %d"
			, strKuItemZu, nKuItemZuNum);
		m_pRs_UrgentProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//61.1
		if(m_pRs_UrgentProp->BOF)
		{
			m_pRs_UrgentProp->raw_Close();	//61.2.1
			strSql.Format("INSERT INTO PropUrgent\
						  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
						  FROM SetProp WHERE Func = '关机流程' AND ZuItem = '%s' AND ZuNum = %d"
						  , strKuItemZu, nKuItemZuNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			//删除所有的延时
			strSql = "DELETE FROM PropUrgent WHERE Func = '关机流程' AND Item = '延时'";
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			//在add之后现将PropUrgent中流程Flag=1,然后执行开机流程的当前项，会将PropUrgent中的Flag=0
			strSql.Format("UPDATE PropUrgent SET Flag = 1 WHERE Func = '关机流程' AND ZuItem = '%s' AND ZuNum = %d"
				, strKuItemZu, nKuItemZuNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		else
		{
			m_pRs_UrgentProp->raw_Close();	//61.2.2					
		}

		//例如吸气、排气、档位、氨阀等，只保留DWKD值最小的那一个
		strSql.Format("SELECT * FROM PropUrgent WHERE Func = '关机流程' AND Item = '%s' AND Num = %d AND DWKD != -1 ORDER BY NO DESC"
			, strItem, nItemNum );
		m_pRs_UrgentProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//62.1
		int i=0;
		while(!m_pRs_UrgentProp->adoEOF)
		{
			if(i>0)//如果只有一条，不删；如果有多条，保留第一条
			{
				CString strDWKD = (LPCSTR)_bstr_t(m_pRs_UrgentProp->GetCollect("DWKD"));
				int nDWKD = atoi(strDWKD);
				strSql.Format("DELETE FROM PropUrgent WHERE Func = '关机流程' AND Item = '%s' AND Num = %d AND DWKD = %d"
					, strItem, nItemNum, nDWKD);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
			i++;
			m_pRs_UrgentProp->MoveNext();
		}
		m_pRs_UrgentProp->raw_Close();	//62.2
	}
	else
	{
		m_pRs_UrgentProp->raw_Close();	//60.2.2
	}//库房项目可能不存在	

	strSql = "DELETE FROM PropUrgent WHERE Item = '风机' AND NC = 1";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
}
void SelectPropUrgent()
{
	CString strSql = "";
	strSql = "DELETE PropUrgent";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	strSql = "SELECT * FROM SetProp WHERE Func = '关机流程' AND Item != '延时' ORDER BY NO ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151014-1-1
	while(!m_pRs->adoEOF)
	{
		CString strItem = "", strNum = "";
		strItem = (LPCSTR)_bstr_t(m_pRs->GetCollect("Item"));		
		strNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("Num"));
		AddUrgentStopProp(strItem, atoi(strNum));//内部使用m_pRs_Urgent..
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//151014-1-2
}
void UrgentStopExecute(CEdit *pEdit)
{
	g_bUrgentING = TRUE;
	g_bUrgentJump = FALSE;

	if(g_pStCS[g_nCS_Index].bFlagEnMusic)
	{
		g_pCanDlg->PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_WMP_UR, BN_CLICKED), NULL);
		Sleep(1000);	
	}

	CString strUrgentStop = "开始紧急关机";
	pEdit->SetWindowTextA(strUrgentStop);
	Doing2Text(strUrgentStop);
	Sleep(2000);

	g_StProp.Func = aSetFunc[1];			//执行的是关机流程
	g_StProp.pEdit = pEdit;	
	g_StProp.byKuNum = 0;					//那个库房触发的流程执行
	g_StProp.strSqlProp = "PropUrgent";
	g_StProp.strKuNum = "全部库房";			//默认值
	
	//151118
	GetYSJSt();

	ST_AllOnAllOff(g_StProp);
}

void Auto_UrgentStop(CString str)
{	
	g_bStopProp = TRUE;	
	g_bStopKuCtrl = TRUE;
	g_bOneKeyStop = FALSE;
	g_bUrgentStop = TRUE;

	str += " 自动-紧急关机";
	Doing2Text(str);
}
void OneKeyStopExecute(CEdit *pEdit)
{
	CString strOneKeyStop = "开始强制关机，执行全部在用库房关机流程";
	pEdit->SetWindowTextA(strOneKeyStop);
	Doing2Text(strOneKeyStop);
	Sleep(1000);

	g_StProp.Func = aSetFunc[1];			//执行的是关机流程
	g_StProp.pEdit = pEdit;	
	g_StProp.byKuNum = 0;					//那个库房触发的流程执行

	if(strcmp(g_strNowTimeKu, "全部库房") == 0)
	{
		SelectSqlAllKu(aSetFunc[1], &g_pCanDlg->m_ctrlProp);	
	}
	else
	{
		SelectSqlSomeKu(aSetFunc[1], &g_pCanDlg->m_ctrlProp, g_strNowTimeKu);
	}

	g_StProp.strKuNum = g_strNowTimeKu;
	g_StProp.strSqlProp = "PropSomeKu";

	ST_AllOnAllOff(g_StProp);
}
//
void SP_PropING()
{
	if(g_bFlagPropING_Wait && (g_hPropING != NULL))
	{
		g_bFlagPropING_Wait = FALSE;
		Doing2Text("暂停流程执行");
		SuspendThread(g_hPropING);
	}	
}
void ST_PropING()
{
	if(!g_bFlagPropING_Wait && (g_hPropING != NULL))
	{
		ResumeThread(g_hPropING);
		g_bFlagPropING_Wait = TRUE;
		Doing2Text("恢复流程执行");
	}	
}
void SP_KuCtrl()
{
	if(g_bFlagKuCtrlING_Wait && (g_hKuCtrl != NULL))
	{
		g_bFlagKuCtrlING_Wait = FALSE;
		Doing2Text("暂停库房控制");
		SuspendThread(g_hKuCtrl);
	}	
}
void ST_KuCtrl()
{
	if(!g_bFlagKuCtrlING_Wait && (g_hKuCtrl != NULL))
	{
		ResumeThread(g_hKuCtrl);
		g_bFlagKuCtrlING_Wait = TRUE;
		Doing2Text("恢复库房控制");
	}	
}
void ST_AllOnAllOff(Prop_t &g_StProp)
{
	CWinThread* pThread;
	pThread = AfxBeginThread(ThreadProc_Prop_AllOnAllOff, &g_StProp, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	g_hPropING = pThread->m_hThread;

	SP_KuCtrl();
}
void ST_OneOnOneOff(Prop_t &g_StProp)
{
	CWinThread* pThread;
	pThread = AfxBeginThread(ThreadProc_Prop_OneOnOneOff, &g_StProp, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	g_hPropING = pThread->m_hThread;
	
	SP_KuCtrl();
}
void Recovery(CEdit *pEdit)
{
	pEdit->SetWindowTextA("资源回收中，请勿退出程序，最多等待10秒……");
	Sleep(1000);	
	
	g_bFlagKuCtrlING_Watch = FALSE;
	g_bFlagKuCtrlING_Wait = FALSE;
	
	//内存泄露-关闭程序死机保护
	LONG lThreadWatchExit = 0;
	GetExitCodeThread( g_hWatch, (LPDWORD)&lThreadWatchExit);
	if(STILL_ACTIVE == lThreadWatchExit)
	{
		Sleep(2000);
	}
	
	//内存泄露-关闭运行时间保护
	LONG lThreadTimeSExit = 0;
	GetExitCodeThread( g_hTimeProtect, (LPDWORD)&lThreadTimeSExit);
	if(STILL_ACTIVE == lThreadTimeSExit)
	{
		g_bTimeProtectOUT = TRUE;
		Sleep(2000);
	}

	//内存泄露-关闭单元遍历
	LONG lThreadIDReferExit = 0;
	GetExitCodeThread( g_hIDRefer, (LPDWORD)&lThreadIDReferExit);
	if(STILL_ACTIVE == lThreadIDReferExit)
	{	
		ST_CidRefer();
		g_bStopRef = TRUE;
		Sleep(1000);
		Doing2Text("停止单元遍历");
	}

	//关闭温度遍历
	g_pCanDlg->SP_TempRefer();
	g_pCanDlg->PropListLoad("SetProp", "清空流程");

	//关闭各端口
	if( g_pCanDlg->GetDlgItem(IDC_BUTTON_CTRL_CLOSE)->IsWindowEnabled() )
		g_pCanDlg->PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CTRL_CLOSE, BN_CLICKED), NULL);
	//内存泄露 - TempS_GetTempSorTime
	if( g_pCanDlg->GetDlgItem(IDC_BUTTON_CLOSE)->IsWindowEnabled() )
		g_pCanDlg->PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CLOSE, BN_CLICKED), NULL);
	if( g_pCanDlg->GetDlgItem(IDC_BUTTON_CLOSE_ANS)->IsWindowEnabled() )
		g_pCanDlg->PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CLOSE_ANS, BN_CLICKED), NULL);
	Sleep(1000);

	//内存泄露-内存释放
	if(g_pStKuYSJ != NULL)
	{
		delete [] g_pStKuYSJ;
		g_pStKuYSJ = NULL;
	}
	if(g_pStYSJ != NULL)
	{
		delete [] g_pStYSJ;
		g_pStYSJ = NULL;
	}
	if(g_pStKuCtrl != NULL)
	{
		delete [] g_pStKuCtrl;	//释放内存，清空指针
		g_pStKuCtrl = NULL;
	}
	if(g_aKuNumTimeTemp != NULL)
	{
		delete []g_aKuNumTimeTemp;
		g_aKuNumTimeTemp = NULL;
	}
	if(g_aKuNumTempTemp != NULL)
	{
		delete []g_aKuNumTempTemp;
		g_aKuNumTempTemp = NULL;
	}
	if(g_pStTempSor != NULL)
	{
		delete [] g_pStTempSor;
		g_pStTempSor = NULL;
	}

	if(!g_bSysCanRun)
	{
		AfxMessageBox("超过使用期限，请联系售后");
	}

	g_pCanDlg->GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(TRUE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(TRUE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(FALSE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_SUSPEND_TEMP_CTRL)->EnableWindow(FALSE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_RESUME_TEMP_CTRL)->EnableWindow(FALSE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(FALSE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(TRUE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_SET)->EnableWindow(TRUE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_CX_TEXT)->EnableWindow(TRUE);

	//151116
	CString strSql = "UPDATE SetKu SET [PartKu] = 0 WHERE KuNum > 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	g_hKuCtrl = NULL;	//全局变量手动复位

	CString strStopCtrl = "";
	strStopCtrl.Format("%02d:%02d 终止库房控制  可安全退出", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute());
	pEdit->SetWindowTextA(strStopCtrl);
	Doing2Text(strStopCtrl);
	AfxMessageBox(strStopCtrl);	
}
void STKuCtrl_KuOnAlready(CEdit *pEdit)
{
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";
	m_pRs_KuCtrl = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151113-1.1
	if(!m_pRs_KuCtrl->BOF)
	{
		pEdit->SetWindowTextA("已存在库房开机");
		Doing2Text("已存在库房开机");
		Sleep(2000);	

		//151113
		//if(LOGIN_EN)
			Auto_UrgentStop("已存在库房开机,建议紧急关机");	

		g_byKuOnSum = 0;
		while(!m_pRs_KuCtrl->adoEOF)
		{
			g_byKuOnSum++;
			m_pRs_KuCtrl->MoveNext();
		}

		g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(TRUE);	//一键关机，执行完开机流程后，使能
		g_pCanDlg->GetDlgItem(IDC_CHECK_YSJ_MANUAL)->EnableWindow(TRUE);	//手动开关压缩机
		for(int i=0; i<g_byYSJSum_SetKu; i++)
		{
			g_pStYSJ[i].bNC = TRUE;
		}
		g_byYSJOnSum = g_byYSJSum_SetKu;
		g_bFlagAllYSJOff = FALSE;	//151116
		g_bFlagAllKuOff = FALSE;	
	}
	m_pRs_KuCtrl->raw_Close();	//151113-1.2
}
UINT ThreadProc_KuCtrl(LPVOID wParam)		//温开温关，同开同关
{
	Sleep(1000);	//勿删
	CEdit *pEdit=(CEdit*)wParam;

	//确认CAN打开
	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请打开控制通道");
		pEdit->SetWindowTextA("温度控制失败");

		Doing2Text("请打开控制通道，温度控制失败");
		return -1;
	}

	//获取串口状态
	if(!g_pCanDlg->m_bSerialPortOpened_TEMP)
	{
		AfxMessageBox("请打开温度通道");
		pEdit->SetWindowTextA("温度控制失败");

		Doing2Text("请打开温度通道，温度控制失败");
		return -1;
	}

	if(!g_pCanDlg->m_bSerialPortOpened_MSG) 
	{
		AfxMessageBox("请打开短信通道");
		pEdit->SetWindowTextA("温度控制失败");

		Doing2Text("请打开短信通道，温度控制失败");
		return -1;
	}

	//先遍历、后库控
	LONG lThreadIDReferExit = 0;
	GetExitCodeThread( g_hIDRefer, (LPDWORD)&lThreadIDReferExit);
	if(STILL_ACTIVE != lThreadIDReferExit)
	{	
		pEdit->SetWindowTextA("单元遍历开启");
		Doing2Text("单元遍历开启");
		Sleep(1000);
	}

	g_pCanDlg->GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(FALSE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(FALSE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(TRUE);
	if(!LOGIN_EN)	//151117
	{
		g_pCanDlg->GetDlgItem(IDC_BUTTON_SUSPEND_TEMP_CTRL)->EnableWindow(TRUE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_RESUME_TEMP_CTRL)->EnableWindow(TRUE);
	}
	g_pCanDlg->GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(TRUE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_SET)->EnableWindow(FALSE);
	g_pCanDlg->GetDlgItem(IDC_BUTTON_CX_TEXT)->EnableWindow(FALSE);

	ResetGlobalCtrlValue();

	if(g_bStopKuCtrl) return -1;
	g_bFlagKuCtrlING_Wait = TRUE;
	
	//151015程序死机保护
	Write_WatchFile();	//喂狗
	g_bFlagKuCtrlING_Watch = TRUE;
	CWinThread* pThreadWatch;
	pThreadWatch = AfxBeginThread(ThreadProc_CCSP_Watch, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	g_hWatch = pThreadWatch->m_hThread;

	Sql2Excel(FALSE);
	CString strText = "DELETE Text";
	m_pCon->Execute(_bstr_t(strText),NULL,adCmdText);
	Sleep(1000);
	pEdit->SetWindowTextA("开始执行库房控制");
	Doing2Text("开始执行库房控制");
	Sleep(1000);
	CString strSql = "";

	//150914 - PropCHSH用于单独关机
	strSql = "DELETE FROM PropCHSH";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	//防止过程中修改风机编号导致流程执行错误
	strSql = "UPDATE SetProp SET Num = 0 WHERE Item = '风机'";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	int nTempCtrlStatus = GetKuCtrlSt();	//150807单独控制模式无影响
	if(nTempCtrlStatus != 1)
	{	
		pEdit->SetWindowTextA("库控数据错误，温度控制失败，可退出");
		Doing2Text("库控数据错误，温度控制失败，可退出");

		Recovery(pEdit);
		return -1;
	}

	//150805
	if(g_bFlagOnlyOnOff)	//循环流程
	{
		int nMSGB = AfxMessageBox("将循环执行开关机流程", MB_YESNO);
		if(nMSGB == IDYES)
		{
			g_nCtrlMode = 1;
			while(!g_bStopKuCtrl)
			{
				g_bStopRef = TRUE;
				if(!g_bStopKuCtrl)
				{
					SelectSqlAllKu(aSetFunc[0], pEdit);
					g_StProp.strKuNum = "全部库房";	
					g_StProp.strSqlProp = "PropSomeKu";	
					g_StProp.Func = aSetFunc[0];	//执行的是开机流程
					g_StProp.pEdit = pEdit;	
					g_StProp.byKuNum = 0;//那个库房触发的流程执行
					ST_AllOnAllOff(g_StProp);
				}
				
				if(!g_bStopKuCtrl)
				{
					SelectSqlAllKu(aSetFunc[1], pEdit);
					g_StProp.strKuNum = "全部库房";
					g_StProp.strSqlProp = "PropSomeKu";	
					g_StProp.Func = aSetFunc[1];
					g_StProp.pEdit = pEdit;
					g_StProp.byKuNum = 0;//那个库房触发的流程执行
					ST_AllOnAllOff(g_StProp);
				}
			}		
		}
		else if(nMSGB == IDNO)
		{
			CButton *pCheckOnlyOnOff = (CButton *)g_pCanDlg->GetDlgItem(IDC_CHECK_ON_OFF_CYCLE);
			g_bFlagOnlyOnOff = FALSE;
			pCheckOnlyOnOff->SetCheck(g_bFlagOnlyOnOff);
		}
	}

	//时开温关，温开温关：同开异关
	CString strCtrlMode="时开温关", strWorkMode="同开异关";
	CString strKuMode = "读取库房工作模式…";
	strSql = "SELECT * FROM SetKu AS KuMode \
			 WHERE KuID = (SELECT TOP 1 KuID FROM SetKu Where KuStatus = 1 AND \
			 KuCtrlMode = KuMode.KuCtrlMode AND KuWorkMode = KuMode.KuWorkMode)";
	m_pRs_KuCtrl = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//153.1
	g_nCtrlMode = 0;
	while(!m_pRs_KuCtrl->adoEOF)
	{
		g_nCtrlMode++;
		m_pRs_KuCtrl->MoveNext();
	}
	m_pRs_KuCtrl->MoveFirst();
	if(g_nCtrlMode > 2)
	{
		pEdit->SetWindowTextA("控制模式选择错误");
		Doing2Text("控制模式选择错误");	
		m_pRs_KuCtrl->raw_Close();	//153.2.1

		Recovery(pEdit);	
		return -1;
	}
	if(g_nCtrlMode == 1)
	{
		strCtrlMode = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("KuCtrlMode"));
		strWorkMode = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("KuWorkMode"));

		if(strcmp(strCtrlMode, "时开温关") == 0)
		{
			CString strTimeSt = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("TimeSt"));
			CString strAllOff = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("AllOff"));
			CString strTimeOut = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("TimeOut"));
			(strcmp(strTimeSt, "始时开机") == 0)? g_bTimeSt_SetKu = TRUE: g_bTimeSt_SetKu = FALSE;
			if(strcmp(strAllOff, "全关跳过") == 0)
			{
				g_bAllOffJump_SetKu = TRUE;	//全部关机，不动作
			}
			else
			{
				g_bAllOffJump_SetKu = FALSE;
				(strcmp(strAllOff, "全关即止") == 0)? g_bAllOffDo_SetKu = TRUE: g_bAllOffDo_SetKu = FALSE;			
			}
			(strcmp(strTimeOut, "终时即关") == 0)? g_bTimeOut_SetKu = TRUE: g_bTimeOut_SetKu = FALSE;
			strKuMode.Format("库房控制模式为: %s(%s %s %s) %s", 
				strCtrlMode, strTimeSt, strAllOff, strTimeOut, strWorkMode);
		}
		else
		{
			strKuMode.Format("库房控制模式为: %s %s", strCtrlMode, strWorkMode);
		}	

		m_pRs_KuCtrl->raw_Close();	//153.2.2
	}
	else if(g_nCtrlMode == 2)
	{
		m_pRs_KuCtrl->raw_Close();	//153.2.3

		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuCtrlMode = '时开温关' ORDER BY KuNum ASC";
		m_pRs_KuCtrl = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//154.1
		if(m_pRs_KuCtrl->BOF)			//未存在该库房数
		{
			AfxMessageBox("无在用时开温关库房");
			m_pRs_KuCtrl->raw_Close();	//154.2.1
			
			Recovery(pEdit);
			return -1;
		}
		strWorkMode = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("KuWorkMode"));
		if(strcmp(strWorkMode, "同开异关") != 0)
		{
			AfxMessageBox("多控制模式暂时仅支持同开异关");
			m_pRs_KuCtrl->raw_Close();	//154.2.3
			//151203
			Recovery(pEdit);
			return -1;		
		}
		g_byTimeTempSum = 0;
		while(!m_pRs_KuCtrl->adoEOF)
		{
			g_byTimeTempSum ++;
			m_pRs_KuCtrl->MoveNext();
		}
		m_pRs_KuCtrl->MoveFirst();

		if (g_aKuNumTimeTemp != NULL)
		{
			delete [] g_aKuNumTimeTemp;
			g_aKuNumTimeTemp = NULL;
		}
		g_aKuNumTimeTemp = new BYTE[g_byTimeTempSum];
		CString strTimeTemp = ":";
		int nKuIndex = 0;
		while(!m_pRs_KuCtrl->adoEOF)
		{
			//获取库房编号
			CString strKuNum = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("KuNum"));
			strTimeTemp += strKuNum;
			strTimeTemp += ",";
			g_aKuNumTimeTemp[nKuIndex] = atoi(strKuNum);

			CString strTimeSt = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("TimeSt"));
			CString strAllOff = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("AllOff"));
			CString strTimeOut = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("TimeOut"));
			(strcmp(strTimeSt, "始时开机") == 0)? g_bTimeSt_SetKu = TRUE: g_bTimeSt_SetKu = FALSE;
			if(strcmp(strAllOff, "全关跳过") == 0)
			{
				g_bAllOffJump_SetKu = TRUE;	//全部关机，不动作
			}
			else
			{
				g_bAllOffJump_SetKu = FALSE;
				(strcmp(strAllOff, "全关即止") == 0)? g_bAllOffDo_SetKu = TRUE: g_bAllOffDo_SetKu = FALSE;			
			}
			(strcmp(strTimeOut, "终时即关") == 0)? g_bTimeOut_SetKu = TRUE: g_bTimeOut_SetKu = FALSE;

			nKuIndex ++;
			m_pRs_KuCtrl->MoveNext();
		}		
		m_pRs_KuCtrl->raw_Close();	//154.2.2

		//151203
		if(!g_bTimeSt_SetKu)
		{
			AfxMessageBox("多控制模式暂时仅支持始时开机");
			//151203
			Recovery(pEdit);
			return -1;				
		}

		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuCtrlMode = '温开温关' ORDER BY KuNum ASC";
		m_pRs_KuCtrl = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//155.1
		if(m_pRs_KuCtrl->BOF)			//未存在该库房数
		{
			AfxMessageBox("无在用温开温关库房");
			m_pRs_KuCtrl->raw_Close();	//155.2.1

			Recovery(pEdit);
			return -1;
		}
		strWorkMode = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("KuWorkMode"));
		if(strcmp(strWorkMode, "同开异关") != 0)
		{
			AfxMessageBox("多控制模式暂时仅支持同开异关");
			m_pRs_KuCtrl->raw_Close();	//155.2.3

			Recovery(pEdit);
			return -1;		
		}
		g_byTempTempSum = 0;
		while(!m_pRs_KuCtrl->adoEOF)
		{
			g_byTempTempSum ++;
			m_pRs_KuCtrl->MoveNext();
		}
		m_pRs_KuCtrl->MoveFirst();

		if(g_aKuNumTempTemp != NULL)
		{
			delete []g_aKuNumTempTemp;
			g_aKuNumTempTemp = NULL;
		}
		g_aKuNumTempTemp = new BYTE[g_byTempTempSum];
		CString strTempTemp = ":";
		nKuIndex = 0;
		while(!m_pRs_KuCtrl->adoEOF)
		{
			//获取库房编号
			CString strKuNum = (LPCSTR)_bstr_t(m_pRs_KuCtrl->GetCollect("KuNum"));
			strTempTemp += strKuNum;
			strTempTemp += ",";
			g_aKuNumTempTemp[nKuIndex] = atoi(strKuNum);
			nKuIndex ++;
			m_pRs_KuCtrl->MoveNext();
		}
		m_pRs_KuCtrl->raw_Close();	//155.2.2

		strCtrlMode="时开温关";
		strWorkMode="同开异关";
		
		strKuMode.Format("库房控制模式为：时开温关%s 与 温开温关%s 混合！", strTimeTemp, strTempTemp);

		g_strDualCtrlMode_TempTemp = strTempTemp;	//161029
		g_strDualCtrlMode_TimeTemp = strTimeTemp;	//161029
	}
	
	pEdit->SetWindowTextA(strKuMode);
	Doing2Text(strKuMode);
	Sleep(2000);

	//需要温度递变才开启线程
	strSql = "SELECT * FROM SetKu WHERE (KuNum > 0) AND (ChangeDays > 0) AND (ChangeDays != 255) ORDER BY KuNum ASC";
	m_pRs_KuCtrl = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//156.1
	if(!m_pRs_KuCtrl->BOF)
	{
		AfxBeginThread(ThreadProc_KuTempChange, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);	
	}
	m_pRs_KuCtrl->raw_Close();	//156.2

	//151113
	STKuCtrl_KuOnAlready(pEdit);

	//库房控制
	if(!g_bStopKuCtrl && (g_nCtrlMode == 1))	//150807单独控制模式无影响
	{
		if(strcmp(strCtrlMode, aKuCtrlMode[1]) == 0)		//时开温关
		{
			TimeOnTempOff(pEdit, strWorkMode);
		}
		else if(strcmp(strCtrlMode, aKuCtrlMode[0]) == 0)	//温开温关
		{
			TempOnTempOff(pEdit, strWorkMode);
		}
		else if(strcmp(strCtrlMode, aKuCtrlMode[2]) == 0)	//时开时关
		{
			TimeOnTimeOff(pEdit, strWorkMode);
		}	
	}
	else if(!g_bStopKuCtrl && (g_nCtrlMode == 2))
	{
		TimeOnTempOff(pEdit, strWorkMode);	//进入时开温关
	}

	if(g_bOneKeyStop)	//先判断一键关机，一键关机中可能出现后续的紧急关机
	{
		OneKeyStopExecute(pEdit);
	}

	if(g_bUrgentStop)	//紧急关机后不会出现一键关机
	{
		if(!g_bUrgentING)
			UrgentStopExecute(pEdit);
		
		//执行完毕151014
		g_bUrgentING = FALSE;
		g_bUrgentJump = FALSE;
	}

	Recovery(pEdit);	//内部g_hKuCtrl = NULL;
	return 0;
}
