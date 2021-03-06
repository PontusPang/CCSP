#include "stdafx.h"
#include "CS.h"

#include "AdvCanThread.h"
#include "CANDlg.h"
#include <ATLComTime.h>

#include "ThreadProp_TempTemp.h"
#include "ThreadProp_TimeTime.h"
#include "ThreadProp.h"
#include "ThreadProp_TimeTemp.h"

#include "Security.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _ConnectionPtr m_pCon;
extern _RecordsetPtr m_pRs_TimeTemp;
extern _RecordsetPtr m_pRs_KuOffTemp;
extern _RecordsetPtr m_pRs_TimeMax;
extern _RecordsetPtr m_pRs_KuStatus;	//库房开关机状态

extern CString aSetFunc[NumSetFunc];
extern CString aKuWorkMode[NumKuWorkMode];
extern CString aCtrlTimeTemp[NumCtrlTimeTemp];

extern CCANDlg *g_pCanDlg;

extern HANDLE g_hDelay3;

extern Prop_t g_StProp;
extern KuCtrl_t *g_pStKuCtrl;
extern TimeCtrl_t *g_pStTimeCtrl;
extern BYTE g_byKuCtrlSum_St;

extern BYTE g_byKuOnMax_SetKu;
extern BYTE g_byKuSum_SetKu;		//库房总数
extern BYTE g_byTempSum_SetKu;		//温度节点总数
extern BOOL g_bTimeSt_SetKu;		//时开温关，始时开机=TRUE
extern BOOL g_bAllOffJump_SetKu;	//时开温关，全关跳过=TRUE
extern BOOL g_bAllOffDo_SetKu;		//时开温关，全关即停=TRUE
extern BOOL g_bTimeOut_SetKu;		//时开温关，超时即关=TRUE
extern BOOL g_bOnProp_MaxOn;
extern BYTE g_byYSJOnSum;

extern BOOL g_bStopKuCtrl;
extern BOOL g_bStopProp;
extern BOOL g_bStopAuto;		//根据条件自动跳出当前控制，继续下一个控制
extern BOOL g_bOneKeyStart;		//手动开机，不看时间

extern BOOL g_bFlagInSpan;
extern BOOL g_bFlagAllKuOff;
extern BOOL g_bAllKuDoOnce;
extern BOOL g_bKuOnMaxWhile;
extern BOOL g_bOffAfterCtrl;
extern BOOL g_bFlagAllYSJOff;

extern COleDateTime g_mTimeNow;

extern BYTE g_byTimeTempSum;		//多控制，时开温关库房总数150807
extern BYTE *g_aKuNumTimeTemp;		//存储，时开温关，库房数
extern int g_nCtrlMode;

extern CString g_strNowTimeKu;

void TimeOnTempOff_TempOn_KuOnMax(CEdit *pEdit, int nKuIndex, CString str, COleDateTime TimeStop, CString strKuNum, int nKuExeSum, BYTE *aKuNum)
{
	CString strTimeStop = TimeStop.Format("%H:%M:%S");
	
	//判断当前已经开机的库房数
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";
	m_pRs_TimeMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//82.1
	BYTE byKuOnSum = 0;	
	while(!m_pRs_TimeMax->adoEOF)
	{
		byKuOnSum ++;
		m_pRs_TimeMax->MoveNext();
	}
	m_pRs_TimeMax->raw_Close();	//82.2
	//150922
	LONG lThreadDelay3Exit = 0;
	GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);
	if(STILL_ACTIVE == lThreadDelay3Exit)
	{
		strSql = "SELECT * FROM PropCHSH WHERE Item = '风机'";
		m_pRs_TimeMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//83.1
		int nSumDelay3 = 0;
		while(!m_pRs_TimeMax->adoEOF)
		{
			nSumDelay3++;
			m_pRs_TimeMax->MoveNext();
		}
		m_pRs_TimeMax->raw_Close();	//83.2

		byKuOnSum += nSumDelay3;	//未关的库房数 + 未关的风机数
	}

	CString strKuOn = "";
	strKuOn.Format("最大开机库房数: %d, 已开机库房数: %d", g_byKuOnMax_SetKu, byKuOnSum);
	pEdit->SetWindowTextA(strKuOn);
	Doing2Text(strKuOn);
	Sleep(1000);

	if(byKuOnSum < g_byKuOnMax_SetKu)
	{
		if( (byKuOnSum == 0) && g_bFlagAllYSJOff) 
		{
			CString str = "时开温关，时间段内，库房压缩机 关机中，某库触发行开机";
				
			pEdit->SetWindowTextA(str);
			Doing2Text(str);
			Sleep(2000);

			//151117
			Temp_On_AllOnOneOff(pEdit, nKuIndex, str, strKuNum);
		}
		else	//150815
		{
			if(g_pStKuCtrl[nKuIndex].bFlagOn)	//不同开判断条件
			{
				str += "  已开机";
				Doing2Text(str);
				pEdit->SetWindowTextA(str);
				Sleep(2000);
			}
			else
			{
				str += "  执行该库房开机流程";
				pEdit->SetWindowTextA(str);
				Doing2Text(str);
				Sleep(1000);

				g_StProp.Func = aSetFunc[0];	//执行的是开机流程
				g_StProp.pEdit = pEdit;	
				g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;//那个库房触发的流程执行

				ST_OneOnOneOff(g_StProp);
			}			
		}	
	}
	else
	{
		g_bKuOnMaxWhile = TRUE;
		
		CString strKuOnMax = "";
		strKuOnMax.Format("已达到最大开机库房数%d，等待已开机库房降温", g_byKuOnMax_SetKu);
		pEdit->SetWindowTextA(strKuOnMax);
		Doing2Text(strKuOnMax);
		Sleep(2000);	

		//只循环检测已开机的库房温度,并只进行关机
		while(!g_bStopKuCtrl && (byKuOnSum >= g_byKuOnMax_SetKu) && g_bFlagInSpan)
		{
			
			
			if(g_nCtrlMode == 1)
			{
				strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND \
					KuStatus = 1 AND WorkStatus = 1 AND KuCtrlMode = '时开温关' ORDER BY KuNum ASC";
			}
			else if(g_nCtrlMode == 2)
			{
				strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1 ORDER BY KuNum ASC";			
			}

			m_pRs_TimeMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//84.1
			while(!m_pRs_TimeMax->adoEOF)
			{	
				CString strKuOnNum = (LPCSTR)_bstr_t(m_pRs_TimeMax->GetCollect("KuNum")); 
				int nKuOnNum = atoi(strKuOnNum);

				CString strKuOnWait = "";
				strKuOnWait.Format("已开机库房%d, 进行温度控制[时温循环]", nKuOnNum);
				pEdit->SetWindowTextA(strKuOnWait);
				Doing2Text(strKuOnWait);
				Write_WatchFile();	//喂狗
				Sleep(1000);

				//查询库房号在结构体数组中的序号nKuOnIndex
				int nKuOnIndex = GetKuIndex_St(nKuOnNum, TRUE);
				if(nKuOnIndex < 0)	break;
	
				//查找库房开关状态
				strSql.Format("SELECT * FROM SetKu WHERE KuNum = %d AND KuNum > 0 AND KuStatus = 1", g_pStKuCtrl[nKuOnIndex].byKuNum);
				m_pRs_KuStatus = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//92.1
				CString strRuKuING = (LPCSTR)_bstr_t(m_pRs_KuStatus->GetCollect("RuKuING"));
				m_pRs_KuStatus->raw_Close();	//92.2
				//开关机状态
				g_pStKuCtrl[nKuOnIndex].bFlagOn = GetKuStatus(TRUE, g_pStKuCtrl[nKuOnIndex].byKuNum, pEdit);

				//获得结构体数组中序号，再获得温度
				int nCorrectTemp = 0;
				nCorrectTemp = GetKuTemp(pEdit, nKuOnIndex);

				if(strcmp(strRuKuING, "0") != 0) //入库中
				{
					CString strRuKuIn = "";
					strRuKuIn.Format("库房%d正在入库中，保持开机",g_pStKuCtrl[nKuOnIndex].byKuNum);
					pEdit->SetWindowTextA(strRuKuIn);
					Doing2Text(strRuKuIn);
					Sleep(1000);
					m_pRs_TimeMax->MoveNext();
					continue;
				}

				if(g_bStopKuCtrl)
				{
					break;
				}

				if(nCorrectTemp > 0)
				{
					//比较温度,进行控制
					if(g_pStKuCtrl[nKuOnIndex].fTemp > g_pStKuCtrl[nKuOnIndex].fTempUp)
					{	
						CString strOn = "";
						strOn.Format("库房%d: %.1f 温度高于上限 不动作"
							, g_pStKuCtrl[nKuOnIndex].byKuNum, g_pStKuCtrl[nKuOnIndex].fTemp);
						pEdit->SetWindowTextA(strOn);
						Doing2Text(strOn);
						Sleep(1000);
					}
					else if(g_pStKuCtrl[nKuOnIndex].fTemp < g_pStKuCtrl[nKuOnIndex].fTempDn)
					{	
						CString strOn = "";
						strOn.Format("库房%d: %.1f 温度低于下限"
							, g_pStKuCtrl[nKuOnIndex].byKuNum, g_pStKuCtrl[nKuOnIndex].fTemp);
						Doing2Text(strOn);

						//同开异关 异开异关		
						TempOrTime_Off_OneOff(pEdit, nKuOnIndex, strOn);
					}
					else
					{
						CString strOn = "";
						strOn.Format("库房%d: %.1f 温度正常 不动作"
							, g_pStKuCtrl[nKuOnIndex].byKuNum, g_pStKuCtrl[nKuOnIndex].fTemp);
						Doing2Text(strOn);
						pEdit->SetWindowTextA(strOn);
						Sleep(1000);
					}		
				}	

				g_pStKuCtrl[nKuOnIndex].fTemp = 0.0;

				//开关机标志
				g_pStKuCtrl[nKuOnIndex].bFlagOn = GetKuStatus(FALSE, g_pStKuCtrl[nKuOnIndex].byKuNum, pEdit);

				if( g_bTimeOut_SetKu )	//终时即关，温控时关，时间判断放在前面
				{
					CString strTimeNow = g_mTimeNow.Format("%H:%M:%S");
				
					if(g_mTimeNow > TimeStop)
					{
						TimeOnTempOff_TempCtrl_TimeOut(pEdit, strKuNum, strTimeNow, strTimeStop);	
					}
					else
					{
						g_bFlagInSpan = TRUE;
						TimeOnTempOff_TempCtrl_TimeIn(pEdit, strKuNum, strTimeNow, strTimeStop);
					}	

					g_bStopAuto = FALSE;

					if(g_bFlagInSpan)
					{
						m_pRs_TimeMax->MoveNext();
					}
					else
					{
						break;
					}
				}
				else
				{
					m_pRs_TimeMax->MoveNext();
				}
			}
			m_pRs_TimeMax->raw_Close();	//84.2

			Write_WatchFile();	//喂狗	//151024

			//一遍循环后，查看是否有库房关机从而跳出循环
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";				
			m_pRs_TimeMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//85.1
			byKuOnSum = 0;
			while(!m_pRs_TimeMax->adoEOF)
			{
				byKuOnSum ++;
				m_pRs_TimeMax->MoveNext();
			}
			m_pRs_TimeMax->raw_Close();	//85.2
			//KuOnSum  加入延时3
			LONG lThreadDelay3Exit = 0;
			GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);
			if(STILL_ACTIVE == lThreadDelay3Exit)
			{
				strSql = "SELECT * FROM PropCHSH WHERE Item = '风机'";
				m_pRs_TimeMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//86.1
				int nSumDelay3 = 0;
				while(!m_pRs_TimeMax->adoEOF)
				{
					nSumDelay3++;
					m_pRs_TimeMax->MoveNext();
				}
				m_pRs_TimeMax->raw_Close();	//86.2
				byKuOnSum += nSumDelay3;

				if(byKuOnSum >= g_byKuOnMax_SetKu)
				{
					pEdit->SetWindowTextA("同开最多数，等待延时关");
					//Doing2Text("同开最多数，等待延时关");
					Sleep(1000);				
				}
			}

			//在用已开机库房内部循环后，查询在用未开机库房的温度
			if(byKuOnSum >= g_byKuOnMax_SetKu)
				 GetKuTemp_OffKu(pEdit);

			if(g_bStopKuCtrl)
				g_pCanDlg->KillTimer(TIMER_ID_TEMP);
		}

		//跳出循环后，首先执行该库房
		if(!g_bStopKuCtrl && g_bFlagInSpan && !g_bStopAuto) //150922-g_bStopAuto
		{
			//开关机状态
			g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(TRUE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);

			if(g_pStKuCtrl[nKuIndex].bFlagOn)	//不同开判断条件
			{
				str += "  已开机";
				pEdit->SetWindowTextA(str);
				Doing2Text(str);
				Sleep(1000);
			}
			else
			{
				str += "  执行该库房开机流程";
				pEdit->SetWindowTextA(str);
				Doing2Text(str);
				Sleep(1000);

				g_StProp.Func = aSetFunc[0];	//执行的是开机流程
				g_StProp.pEdit = pEdit;	
				g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;//那个库房触发的流程执行

				ST_OneOnOneOff(g_StProp);
			}

			//开关机状态
			g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(FALSE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);		
		}
		//执行完毕该库房后，再遍历后面的库房

		//150921
		g_bKuOnMaxWhile = FALSE;
	}
}
void GetKuTemp_OffKu(CEdit *pEdit)
{
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 0 ORDER BY KuNum ASC";
	m_pRs_KuOffTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//81.1
	while(!g_bStopKuCtrl && !m_pRs_KuOffTemp->adoEOF)
	{
		//查询温度
		CString strKuOnNum = (LPCSTR)_bstr_t(m_pRs_KuOffTemp->GetCollect("KuNum")); 
		int nKuOnNum = atoi(strKuOnNum);

		CString strKuOffTemp = "";
		strKuOffTemp.Format("未开机库房%d, 进行温度查询", nKuOnNum);
		pEdit->SetWindowTextA(strKuOffTemp);
		Sleep(1000);

		//查询库房号在结构体数组中的序号nKuOnIndex
		int nKuOnIndex = GetKuIndex_St(nKuOnNum, FALSE);
		if(nKuOnIndex > -1)
			GetKuTemp(pEdit, nKuOnIndex);

		m_pRs_KuOffTemp->MoveNext();
	}
	m_pRs_KuOffTemp->raw_Close();	//81.2
}
void TimeOnTempOff_TempCtrl_AllOff(CEdit *pEdit, int nKuIndex, CString strKuNum, CString str)
{
	CString strSql = "";
	if(strcmp(strKuNum, "全部库房") == 0)
	{
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1\
				 AND KuCtrlMode = '时开温关' AND KuWorkMode = '同开同关'";
	}
	else
	{	//150907暂时用不到
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1 AND PartKu = 1\
				 AND KuCtrlMode = '时开温关' AND KuWorkMode = '同开同关'";
	}

	m_pRs_TimeTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//166.1
	if(m_pRs_TimeTemp->BOF)				
	{
		str += "  已关机";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TimeTemp->raw_Close();	//166.2.1
	}
	else
	{
		str += "  执行全部关机流程";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TimeTemp->raw_Close();	//166.2.2

		if(strcmp(strKuNum, "全部库房") == 0)
		{
			SelectSqlAllKu(aSetFunc[1], pEdit);
			g_StProp.strKuNum = "全部库房";
		}
		else
		{
			SelectSqlSomeKu(aSetFunc[1], pEdit, strKuNum);
			g_StProp.strKuNum = strKuNum;
		}

		g_StProp.strSqlProp = "PropSomeKu";	

		g_StProp.Func = aSetFunc[1];
		g_StProp.pEdit = pEdit;
		g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;//那个库房触发的流程执行

		ST_AllOnAllOff(g_StProp);
	}
}
void TimeOnTempOff_TempCtrl_AllOn(CEdit *pEdit, int nKuIndex, CString strKuNum, CString str)
{
	CString strSql = "";
	if(strcmp(strKuNum, "全部库房") == 0)
	{
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 0\
				 AND KuCtrlMode = '时开温关' AND KuWorkMode = '同开同关'";
	}
	else
	{
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 0 AND PartKu = 1\
				 AND KuCtrlMode = '时开温关' AND KuWorkMode = '同开同关'";
	}

	m_pRs_TimeTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//165.1
	if(m_pRs_TimeTemp->BOF)	//全部开机
	{
		str += "  已开机";
		pEdit->SetWindowTextA(str);	
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TimeTemp->raw_Close();	//165.2.1
	}
	else
	{
		str += "  执行全部开机流程";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TimeTemp->raw_Close();	//165.2.2

		if(strcmp(strKuNum, "全部库房") == 0)
		{
			SelectSqlAllKu(aSetFunc[0], pEdit);
			g_StProp.strKuNum = "全部库房";
		}
		else
		{
			SelectSqlSomeKu(aSetFunc[0], pEdit, strKuNum);
			g_StProp.strKuNum = strKuNum;
		}

		g_StProp.strSqlProp = "PropSomeKu";	

		g_StProp.Func = aSetFunc[0];	//执行的是开机流程
		g_StProp.pEdit = pEdit;	
		g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;//那个库房触发的流程执行

		ST_AllOnAllOff(g_StProp);		
	}
}
void TimeOnTempOff_TempCtrl_TimeOut(CEdit *pEdit, CString strKuNum, CString strTimeNow, CString strTimeStop)
{//同开最多
	BOOL g_bFlagTempTempAllOff = FALSE;	//150807
	CString strSql = "";
	if(strcmp(strKuNum, "全部库房") == 0)
	{
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";		
		m_pRs_TimeTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//162.1
		(m_pRs_TimeTemp->BOF)? g_bFlagAllKuOff = TRUE: g_bFlagAllKuOff = FALSE;//全部关机
		m_pRs_TimeTemp->raw_Close();	//162.2

		if(g_nCtrlMode == 2)
		{
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1 AND KuCtrlMode = '温开温关'";		
			m_pRs_TimeTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//163.1
			(m_pRs_TimeTemp->BOF)? g_bFlagTempTempAllOff = TRUE: g_bFlagTempTempAllOff = FALSE;//全部关机
			m_pRs_TimeTemp->raw_Close();	//163.2
		}
	}
	else
	{
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";		
		m_pRs_TimeTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//164.1
		(m_pRs_TimeTemp->BOF)? g_bFlagAllKuOff = TRUE: g_bFlagAllKuOff = FALSE;//全部关机
		m_pRs_TimeTemp->raw_Close();	//164.2
	}

	if(g_bFlagAllKuOff)	//全部关机				
	{		
		//151112 库房全部关机，压缩机也关了，就不要再执行关机流程了，直接退出
		if(g_byYSJOnSum)
		{
			CString strTimeTemp = "";
			strTimeTemp.Format("时开温关，%s > %s 时间已到，库房%s 已 全部关机，关闭压缩机"
				, strTimeNow, strTimeStop, strKuNum);	
			pEdit->SetWindowTextA(strTimeTemp);
			Doing2Text(strTimeTemp);
			Sleep(3000);

			if((g_nCtrlMode == 2) && g_bFlagAllYSJOff)	//150807
			{
				pEdit->SetWindowTextA("时开温关，超出时间，关机流程之前已执行");
				Doing2Text("时开温关，超出时间，关机流程之前已执行");
				Sleep(1000);

				g_bFlagInSpan = FALSE;	//已经不在时间段内, 用于跳出温控循环

				return;	
			}	
		
			//会再关闭一遍风机
			if(strcmp(strKuNum, "全部库房") == 0)
			{
				SelectSqlAllKu(aSetFunc[1], pEdit);
				g_StProp.strKuNum = "全部库房";
			}
			else
			{
				SelectSqlSomeKu(aSetFunc[1], pEdit, strKuNum);
				g_StProp.strKuNum = strKuNum;
			}

			g_StProp.strSqlProp = "PropSomeKu";	

			g_StProp.Func = aSetFunc[1];
			g_StProp.pEdit = pEdit;
			g_StProp.byKuNum = 0;//那个库房触发的流程执行

			ST_AllOnAllOff(g_StProp);		
		}
		else
		{
			CString strTimeTemp = "";
			strTimeTemp.Format("时开温关，%s > %s 时间已到，库房压缩机 已 全部关机，进入下一个时间段"
				, strTimeNow, strTimeStop);	
			pEdit->SetWindowTextA(strTimeTemp);
			Doing2Text(strTimeTemp);
			Sleep(3000);		
		}
		
		g_bFlagInSpan = FALSE;	//已经不在时间段内, 用于跳出温控循环
	}
	else
	{
		//库房未全部关机，g_byYSJOnSum肯定为TRUE，所以不用判断 151112
		CString strTimeTemp = "";

		if( !g_bTimeOut_SetKu )	//终时待温
		{
			strTimeTemp.Format("时开温关，%s > %s 时间已到，库房%s 未 全部关机，继续温控"
				, strTimeNow, strTimeStop, strKuNum);
			pEdit->SetWindowTextA(strTimeTemp);
			Doing2Text(strTimeTemp);
			Sleep(3000);

			g_bFlagInSpan = TRUE;
		}		
		else	//温控时关，到时间就关机
		{
			if( (g_nCtrlMode == 2) && !g_bFlagTempTempAllOff )
			{
				pEdit->SetWindowTextA("混合控制，时间已到，纯温控未全部关机，不关闭压缩机");
				Sleep(1000);
				return;
			}
			
			strTimeTemp.Format("时开温关，%s > %s 时间已到，库房%s 未 全部关机，直接执行关机流程"
				, strTimeNow, strTimeStop, strKuNum);
			pEdit->SetWindowTextA(strTimeTemp);
			Doing2Text(strTimeTemp);
			Sleep(3000);

			//会再关闭一遍风机
			if(strcmp(strKuNum, "全部库房") == 0)
			{
				SelectSqlAllKu(aSetFunc[1], pEdit);
				g_StProp.strKuNum = "全部库房";
			}
			else
			{
				SelectSqlSomeKu(aSetFunc[1], pEdit, strKuNum);
				g_StProp.strKuNum = strKuNum;
			}

			g_StProp.strSqlProp = "PropSomeKu";	

			g_StProp.Func = aSetFunc[1];
			g_StProp.pEdit = pEdit;
			g_StProp.byKuNum = 0;//那个库房触发的流程执行

			ST_AllOnAllOff(g_StProp);

			g_bFlagInSpan = FALSE;

			g_bStopAuto = TRUE;	//已自动停止, 用于跳出温控循环
		}//温关or时关
	}//是否已全部关机
}

void TimeOnTempOff_TempCtrl_TimeIn(CEdit *pEdit, CString strKuNum, CString strTimeNow, CString strTimeStop)
{
	CString strTimeTemp = "";
	strTimeTemp.Format("%s < %s 时间未到，继续温控", strTimeNow, strTimeStop);
	pEdit->SetWindowTextA(strTimeTemp);
	//Doing2Text(strTimeTemp);	//150912
	Sleep(2000);
	CString strSql = "";

	if(g_bFlagInSpan)
	{
		//150907
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";	
		m_pRs_TimeTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//161.1
		(m_pRs_TimeTemp->BOF)? g_bFlagAllKuOff = TRUE: g_bFlagAllKuOff = FALSE;//全部关机
		m_pRs_TimeTemp->raw_Close();	//161.2

		//全关跳过，则全关无动作；
		if(!g_bAllOffJump_SetKu)
		{
			//全关不跳过，则关机
			if(g_bAllKuDoOnce && g_bFlagAllKuOff && !g_bKuOnMaxWhile && !g_bOffAfterCtrl)
			{
				//151112
				if(g_byYSJOnSum)
				{
					pEdit->SetWindowTextA("时开温关，所有库房已遍历，无等待开机库房，且已全部关机，则执行关机流程");
					Doing2Text("时开温关，所有库房已遍历，无等待开机库房，且已全部关机，则执行关机流程");
					Sleep(3000);

					if((g_nCtrlMode == 2) && g_bFlagAllYSJOff)	//150807
					{
						pEdit->SetWindowTextA("时开温关，时间段内，关机流程之前已执行");
						Doing2Text("时开温关，时间段内，关机流程之前已执行");
						Sleep(1000);

						//全关即关，则停止本时间段；全关等开，则继续温控
						(g_bAllOffDo_SetKu)? g_bStopAuto = TRUE: g_bStopAuto = FALSE;
						g_bOffAfterCtrl = TRUE;		//执行完一遍关机流程了

						return;	
					}	

					//会再关闭一遍风机
					if(strcmp(strKuNum, "全部库房") == 0)
					{
						SelectSqlAllKu(aSetFunc[1], pEdit);
						g_StProp.strKuNum = "全部库房";
					}
					else
					{
						SelectSqlSomeKu(aSetFunc[1], pEdit, strKuNum);
						g_StProp.strKuNum = strKuNum;
					}

					g_StProp.strSqlProp = "PropSomeKu";	

					g_StProp.Func = aSetFunc[1];
					g_StProp.pEdit = pEdit;
					g_StProp.byKuNum = 0;//那个库房触发的流程执行

					ST_AllOnAllOff(g_StProp);

					//全关即关，则停止本时间段；全关等开，则继续温控
					(g_bAllOffDo_SetKu)? g_bStopAuto = TRUE: g_bStopAuto = FALSE;
					g_bOffAfterCtrl = TRUE;		//执行完一遍关机流程了
				}
				else
				{
				
				}
			}	
		}
	}
}
void KuTempCtrl_TimeTemp(int nKuExeIndex, CEdit *pEdit, CString strWorkMode, CString strKuNum, int nKuExeSum, BYTE *aKuNum, COleDateTime TimeStop, CString strTimeStop)
{
	//150924
	(strcmp(strWorkMode, aKuWorkMode[2]) == 0)? g_bOnProp_MaxOn = FALSE: g_bOnProp_MaxOn = TRUE;
	
	if(!g_bStopKuCtrl &&  g_bTimeOut_SetKu )	//终时即止，时间判断放在前面
	{
		CString strTimeNow = g_mTimeNow.Format("%H:%M:%S");
	
		if(g_mTimeNow > TimeStop)
		{
			if(g_bFlagInSpan)	//如果在KuOnMax中已置位，则不再继续关机
			{
				TimeOnTempOff_TempCtrl_TimeOut(pEdit, strKuNum, strTimeNow, strTimeStop);
			}	
		}
		else
		{
			g_bFlagInSpan = TRUE;
			TimeOnTempOff_TempCtrl_TimeIn(pEdit, strKuNum, strTimeNow, strTimeStop);	
		}		
	}

	if(!g_bFlagInSpan || g_bStopAuto)	//TimeOut会复位该标志
	{
		return;
	}
		
	int nKuIndex = -1;
	if(strcmp(strKuNum, "全部库房") == 0)
	{
		nKuIndex = nKuExeIndex;
	}
	else
	{
		//查找引发流程执行的库房在结构体数组中的序号
		nKuIndex = GetKuIndex_St(aKuNum[nKuExeIndex], FALSE);
		if(nKuIndex == -1)
		{
			//150907
			CString strText = "";
			strText.Format("库房%d未用或无效", aKuNum[nKuExeIndex]);
			pEdit->SetWindowTextA(strText);
			Doing2Text(strText);
			Sleep(1000);
			return;
		}
	}

	//更新库房开关状态
	CString strSql = "";
	strSql.Format("SELECT * FROM SetKu WHERE KuNum = %d AND KuNum > 0 AND KuStatus = 1", g_pStKuCtrl[nKuIndex].byKuNum);
	m_pRs_KuStatus = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//93.1
	CString strRuKuING = (LPCSTR)_bstr_t(m_pRs_KuStatus->GetCollect("RuKuING"));
	m_pRs_KuStatus->raw_Close();	//93.2
	//开关机状态
	g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(TRUE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);

	if(g_bStopKuCtrl)	//退出库房监控与流程执行
	{
		g_bStopProp = TRUE;
		return;
	}

	//获得结构体数组中序号，再获得温度
	int nCorrectTemp = 0;
	nCorrectTemp = GetKuTemp(pEdit, nKuIndex);

	if(strcmp(strRuKuING, "0") != 0) //入库中
	{
		CString strRuKu = "";
		strRuKu.Format("库房%d正在入库中，保持开机",g_pStKuCtrl[nKuIndex].byKuNum);
		pEdit->SetWindowTextA(strRuKu);
		Doing2Text(strRuKu);
		Sleep(1000);
		return;
	}

	if(nCorrectTemp > 0)
	{
		//比较温度,进行控制
		if(g_pStKuCtrl[nKuIndex].fTemp > g_pStKuCtrl[nKuIndex].fTempUp)
		{	
			CString str = "";
			str.Format("库房%d: %.1f 温度高于上限"
				, g_pStKuCtrl[nKuIndex].byKuNum, g_pStKuCtrl[nKuIndex].fTemp);
			Doing2Text(str);

			//高于上限，未开机的库房才进行动作
			if(!g_pStKuCtrl[nKuIndex].bFlagOn)
			{
				pEdit->SetWindowTextA("默认，进入同开最多控制模式");
				Doing2Text("默认，进入同开最多控制模式");
				Sleep(1000);
				TimeOnTempOff_TempOn_KuOnMax(pEdit, nKuIndex, str, TimeStop, strKuNum, nKuExeSum, aKuNum);
			}
			else
			{
				str += "  已开机";
				pEdit->SetWindowTextA(str);
				Doing2Text(str);
				Sleep(1000);			
			}
			
			g_bOffAfterCtrl = FALSE;	//执行开机流程了，复位
		}
		else if(g_pStKuCtrl[nKuIndex].fTemp < g_pStKuCtrl[nKuIndex].fTempDn)
		{	
			CString str = "";
			str.Format("库房%d: %.1f 温度低于下限"
				, g_pStKuCtrl[nKuIndex].byKuNum, g_pStKuCtrl[nKuIndex].fTemp);
			Doing2Text(str);

			if(g_pStKuCtrl[nKuIndex].bFlagOn)
			{
				//150924 默认单独关
				TempOrTime_Off_OneOff(pEdit, nKuIndex, str);					//同开异关 异开异关
			}
			else
			{
				str += "  已关机";
				pEdit->SetWindowTextA(str);
				Doing2Text(str);
				Sleep(1000);			
			}
		}
		else
		{
			CString str = "";
			str.Format("库房%d: %.1f 温度正常"
				, g_pStKuCtrl[nKuIndex].byKuNum, g_pStKuCtrl[nKuIndex].fTemp);
			pEdit->SetWindowTextA(str);
			Doing2Text(str);
			Sleep(1000);
		}		
	}	

	g_pStKuCtrl[nKuIndex].fTemp = 0.0;

	//开关机状态
	g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(FALSE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);

	if(g_bStopKuCtrl)	//退出库房监控与流程执行
	{
		g_bStopProp = TRUE;
		return;
	}
}
void TimeOnTempOff_TempCtrl(CEdit *pEdit, CString strWorkMode, CString strKuNum, COleDateTime TimeStop)
{
	CString strTimeStop = "";
	g_bOneKeyStart?	strTimeStop = "--:--:--": strTimeStop = TimeStop.Format("%H:%M:%S");

	int nKuExeSum = 0;	//温度循环遍历总数
	BYTE *aKuNum = NULL;
	BOOL bPartKu = FALSE;
	if(strcmp(strKuNum, "全部库房") == 0)
	{
		nKuExeSum = g_byKuCtrlSum_St;
		CString strAllKu = "开始执行温度控制 全部库房";
		pEdit->SetWindowTextA(strAllKu);
		Doing2Text(strAllKu);
		Sleep(1000);

		bPartKu = FALSE;
	}
	else
	{
		CString strSomeKu = "开始执行温度控制 部分库房";
		strSomeKu += strKuNum;
		pEdit->SetWindowTextA(strSomeKu);
		Doing2Text(strSomeKu);
		Sleep(1000);

		//部分库房，字符串转化为数组，数组最大为库房总数
		aKuNum = (BYTE *)malloc(g_byKuSum_SetKu * sizeof(BYTE));
		nKuExeSum = Str2Dec(strKuNum, aKuNum);

		bPartKu = TRUE;
	}

	//时开温关，一直进行温度控制
	while(!g_bStopKuCtrl && g_bFlagInSpan && !g_bStopAuto)
	{
		if(g_nCtrlMode == 1)
		{
			//150911-部分库房，也关心其他在用库房
			if(bPartKu && !g_bStopKuCtrl && !g_bStopProp && g_bFlagInSpan && !g_bStopAuto)
				GetKuTemp_OffKu(pEdit);

			for(int nKuExeIndex = 0; nKuExeIndex < nKuExeSum; nKuExeIndex++)
			{
				Write_WatchFile();	//喂狗
				KuTempCtrl_TimeTemp(nKuExeIndex, pEdit, strWorkMode, strKuNum, nKuExeSum, aKuNum, TimeStop, strTimeStop);
				if(g_bStopKuCtrl || g_bStopProp || !g_bFlagInSpan || g_bStopAuto)
				{
					break;
				}
			}//某个库房，判断
		}
		else if(g_nCtrlMode == 2)
		{
			//150807仅有一种控制模式，则循环即可；多种控制模式，则要【筛选】
			for(int nIndexTimeTemp = 0; nIndexTimeTemp < g_byTimeTempSum; nIndexTimeTemp++)	//时开温关库房数内循环
			{
				//查找温开温关库房数在全库结构体中的序号
				Write_WatchFile();	//喂狗
				int nKuExeIndex = GetKuIndex_St(g_aKuNumTimeTemp[nIndexTimeTemp], TRUE);
				if(nKuExeIndex < 0)	break;

				KuTempCtrl_TimeTemp(nKuExeIndex, pEdit, strWorkMode, strKuNum, nKuExeSum, aKuNum, TimeStop, strTimeStop);
				if(g_bStopKuCtrl || g_bStopProp || !g_bFlagInSpan || g_bStopAuto)
				{
					break;
				}
			}

			//150807 4-2时间段内温控中
			if((g_nCtrlMode == 2) && !g_bStopKuCtrl && g_bFlagInSpan)
			{
				TempOnTempOff(pEdit, strWorkMode);
			}
		}
		else
		{
			Auto_UrgentStop("时开温关 模式错误");
			break;
		}

		//150815 始时开机，执行完一遍温控，即可置位；始时不开，必须执行完一遍开机流程才可
		if( g_bTimeSt_SetKu )
		{
			g_bAllKuDoOnce = TRUE;
		}
		else
		{
			(g_bFlagAllYSJOff)? g_bAllKuDoOnce = FALSE: g_bAllKuDoOnce = TRUE;
		}

		//判断时间
		if(!g_bStopKuCtrl && !g_bTimeOut_SetKu )	//终时待温，时间判断放在后面
		{
			CString strTimeNow = g_mTimeNow.Format("%H:%M:%S");
		
			if(g_mTimeNow > TimeStop)
			{
				TimeOnTempOff_TempCtrl_TimeOut(pEdit, strKuNum, strTimeNow, strTimeStop);	
			}
			else
			{
				g_bFlagInSpan = TRUE;
				TimeOnTempOff_TempCtrl_TimeIn(pEdit, strKuNum, strTimeNow, strTimeStop);	
			}		
		}

		if(g_bStopKuCtrl)	//退出库房监控与流程执行
		{
			g_bStopProp = TRUE;
			break;
		}
	}//某些库房，循环

	if(strcmp(strKuNum, "全部库房") != 0)
	{
		free(aKuNum);
	}
}
void TimeOnTempOff(CEdit *pEdit, CString strWorkMode)
{
	Doing2Text("时开温关");
	//库房设置结构体数组在ThreadProc_KuCtrl中 g_pStKuCtrl

	if(!g_bOneKeyStart)	
	{
		Doing2Text("自动开机，读取峰谷用电设置");
		//峰谷用电设置
		int nTimeCtrlStatus = GetTimeCtrlSt();
		if(nTimeCtrlStatus == -1)
		{
			pEdit->SetWindowTextA("时温控制失败");
			Doing2Text("时温控制失败");
			g_bStopKuCtrl = TRUE;
		}	
	}
	else	//人工一键开机，不看时间
	{
		g_bFlagInSpan = TRUE;
		pEdit->SetWindowTextA("手动开机");
		Doing2Text("手动开机，不读取峰谷用电设置");
		Sleep(1500);

		//开启温度遍历
		g_pCanDlg->ST_TempRefer();

		//直接开机，为全部库房，而不是部分库房
		Time_On_AllOnAllOff(pEdit, "全部库房");

		if(!g_bStopKuCtrl)
		{
			//关闭温度遍历
			g_pCanDlg->SP_TempRefer();

			g_bAllKuDoOnce = FALSE;
			g_bKuOnMaxWhile = FALSE;
			g_bOffAfterCtrl = FALSE;
			COleDateTime TimeStopNone;
			TimeStopNone.SetDateTime(2200,1,1,0,0,0);
			TimeOnTempOff_TempCtrl(pEdit, strWorkMode, "全部库房", TimeStopNone);

			if(g_bStopAuto)	//已全部关机，等待至本段时间结束
			{
				CString strWaitForTimeOut = "";
				strWaitForTimeOut = "已全部关机，等待停止库控";
				pEdit->SetWindowTextA(strWaitForTimeOut);
				Doing2Text("已全部关机，等待停止库控");	

				while(!g_bStopKuCtrl)	//退出库房监控与流程执行
				{
					//开启温度遍历
					g_pCanDlg->ST_TempRefer();

					Write_WatchFile();
					Sleep(1000);
				}
			}			
		}
	}

	while(!g_bOneKeyStart && !g_bStopKuCtrl)
	{
		Write_WatchFile();	//喂狗
		int nNowTimeSpan=0;	//当前时间所在时间段
		nNowTimeSpan = NowTimeSpan(pEdit, strWorkMode);	//置位g_bFlagInSpan	

		while(g_bFlagInSpan)	//执行关闭库房指令后，退出此循环
		{
			Write_WatchFile();	//喂狗
			if(g_bStopKuCtrl)	//退出库房监控与流程执行
			{
				g_bStopProp = TRUE;
				g_bFlagInSpan = FALSE;
				break;
			}			

			//获取当前时间
			CString strTimeNow = g_mTimeNow.Format("%H:%M:%S");

			GetTimeStartStop(g_mTimeNow, nNowTimeSpan);

			CString strTimeStart = "";
			strTimeStart = g_pStTimeCtrl[nNowTimeSpan].TimeStart.Format("%H:%M:%S");
			CString strTimeStop = "";
			strTimeStop = g_pStTimeCtrl[nNowTimeSpan].TimeStop.Format("%H:%M:%S");			

			//查询与比较时间结构体数组，如果到达起始时间，则打开库房，如果到达终止时间，则关闭库房	
			if( (g_mTimeNow >= g_pStTimeCtrl[nNowTimeSpan].TimeStart) && (g_mTimeNow <= g_pStTimeCtrl[nNowTimeSpan].TimeStop) )
			{					
				g_bFlagInSpan = TRUE;

				//开启温度遍历
				g_pCanDlg->ST_TempRefer();

				CString strStartTimeKu = "";
				if( g_bTimeSt_SetKu )	//始时开机-到达时间，开启压缩机或风机
				{
					strStartTimeKu.Format("%s < %s < %s  时间到达，同时 开启库房 %s"
						,strTimeStart, strTimeNow, strTimeStop, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);
					pEdit->SetWindowTextA(strStartTimeKu);
					Doing2Text(strStartTimeKu);
					Sleep(2000);	

					Time_On_AllOnAllOff(pEdit, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);			
				}
				else	//始时不开-到达时间，不开启压缩机或风机，进入温度控制
				{
					//150815
					strStartTimeKu.Format("%s < %s < %s  始时不开，进入温控",strTimeStart, strTimeNow, strTimeStop);
					pEdit->SetWindowTextA(strStartTimeKu);
					Doing2Text(strStartTimeKu);
					Sleep(2000);

					//复位
					g_bStopAuto = FALSE;	//151112 新的时间段，复位
				}

				if(g_bStopKuCtrl)	//退出库房监控与流程执行
				{
					g_bStopProp = TRUE;
					g_bFlagInSpan = FALSE;
					break;
				}	

				//关闭温度遍历
				g_pCanDlg->SP_TempRefer();

				g_bAllKuDoOnce = FALSE;
				g_bKuOnMaxWhile = FALSE;
				g_bOffAfterCtrl = FALSE;
				TimeOnTempOff_TempCtrl(pEdit, strWorkMode, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum, g_pStTimeCtrl[nNowTimeSpan].TimeStop);
				
				if(g_bStopAuto)	//已全部关机，等待至本段时间结束
				{
					Doing2Text("已全部关机，等待本段时间结束");
					do
					{
						//g_mTimeNow = COleDateTime::GetCurrentTime();
						strTimeNow = g_mTimeNow.Format("%H:%M:%S");

						CString strWaitForTimeOut = "";
						strWaitForTimeOut.Format("%s < %s 已全部关机，等待本段时间结束", strTimeNow, strTimeStop);
						pEdit->SetWindowTextA(strWaitForTimeOut);
						
						Write_WatchFile();	//喂狗
						Sleep(1000);	

						if(g_bStopKuCtrl)	//退出库房监控与流程执行
						{
							g_bFlagInSpan = FALSE;
							break;
						}

						//开启温度遍历
						g_pCanDlg->ST_TempRefer();

						//150807 4-3全部关机未到终止时间
						if((g_nCtrlMode == 2) && !g_bStopKuCtrl && g_bFlagInSpan)
						{
							TempOnTempOff(pEdit, strWorkMode);
							GetKuTemp_OffKu(pEdit);
						}
					}
					while(g_mTimeNow <= g_pStTimeCtrl[nNowTimeSpan].TimeStop);
				}
			}
			else if(g_mTimeNow < g_pStTimeCtrl[nNowTimeSpan].TimeStart)
			{
				g_bFlagInSpan = TRUE;

				//开启温度遍历
				g_pCanDlg->ST_TempRefer();

				CString strNormalTimeKu = "";
				strNormalTimeKu.Format("%s < %s 时间未到，不开启库房", strTimeNow,strTimeStart);
				pEdit->SetWindowTextA(strNormalTimeKu);
				Sleep(1000);

				//150807 4-1未到起始时间
				if((g_nCtrlMode == 2) && !g_bStopKuCtrl && g_bFlagInSpan)
				{
					TempOnTempOff(pEdit, strWorkMode);
					GetKuTemp_OffKu(pEdit);
				}
			}
			else if(g_mTimeNow > g_pStTimeCtrl[nNowTimeSpan].TimeStop)
			{
				Doing2Text("正常：时间判断更新不及时");
				g_bFlagInSpan = FALSE;
			}//比较起止时间
		}//循环当前峰谷用电时间段内比较
	}//循环查询当前时间所在时间段

	if(g_pStTimeCtrl != NULL)
		delete [] g_pStTimeCtrl;	//释放内存，清空指针
	g_pStTimeCtrl = NULL;
}