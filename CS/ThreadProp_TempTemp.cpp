#include "stdafx.h"
#include "CS.h"

#include "AdvCanThread.h"
#include "CANDlg.h"
#include <ATLComTime.h>

#include "ThreadProp_TempTemp.h"
#include "ThreadProp_TimeTime.h"
#include "ThreadProp_TimeTemp.h"
#include "ThreadProp.h"

#include "Security.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _ConnectionPtr m_pCon;
extern _RecordsetPtr m_pRs_KuStatus;	//库房开关机状态
extern _RecordsetPtr m_pRs_TempTemp;
extern _RecordsetPtr m_pRs_TempMax;

extern CString aSetFunc[NumSetFunc];
extern CString aKuWorkMode[NumKuWorkMode];
extern BOOL g_bOnProp_MaxOn;

extern CCANDlg *g_pCanDlg;

extern Prop_t g_StProp;
extern KuCtrl_t *g_pStKuCtrl;
extern YSJ_t *g_pStYSJ;

extern BYTE g_byKuCtrlSum_St;

extern HANDLE g_hDelay3;

extern BOOL g_bStopKuCtrl;
extern BOOL g_bStopProp;
extern BOOL g_bOneKeyStart;

extern BOOL g_bFlagAllKuOff;
extern BOOL g_bInTimer2;
extern BOOL g_bFlagAllYSJOff;

extern BYTE g_byKuOnMax_SetKu;
extern BYTE g_byKuSum_SetKu;		//库房总数
extern BYTE g_byYSJSum_SetKu;		//压缩机总数
extern BYTE g_byYSJOnSum;
extern BYTE g_byKuOnSum;

extern BYTE g_byTempTempSum;		//多控制，温开温关库房总数150807
extern BYTE *g_aKuNumTempTemp;		//存储，温开温关，库房数
extern int g_nCtrlMode;

extern BOOL g_bKuOnMaxWhile;

BOOL g_bAllKuDoOnce_TempTemp = FALSE;

CString g_strDualCtrlMode_TempTemp = "";	//161029
CString g_strDualCtrlMode_TimeTemp = "";	//161029

void TempOnTempOff_TempOn_KuOnMax(CEdit *pEdit, int nKuIndex, CString str)
{
	//判断当前已经开机的库房数
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";
	m_pRs_TempMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//76.1
	BYTE byKuOnSum = 0;	
	while(!m_pRs_TempMax->adoEOF)
	{
		byKuOnSum ++;
		m_pRs_TempMax->MoveNext();
	}
	m_pRs_TempMax->raw_Close();	//76.2
	//150922
	LONG lThreadDelay3Exit = 0;
	GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);
	if(STILL_ACTIVE == lThreadDelay3Exit)
	{
		strSql = "SELECT * FROM PropCHSH WHERE Item = '风机'";
		m_pRs_TempMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//77.1
		int nSumDelay3 = 0;
		while(!m_pRs_TempMax->adoEOF)
		{
			nSumDelay3++;
			m_pRs_TempMax->MoveNext();
		}
		m_pRs_TempMax->raw_Close();	//77.2

		byKuOnSum += nSumDelay3;	//未关的库房数 + 未关的风机数
	}

	CString strKuOn = "";
	strKuOn.Format("最大开机库房数: %d, 已开机库房数: %d", g_byKuOnMax_SetKu, byKuOnSum);
	pEdit->SetWindowTextA(strKuOn);
	Doing2Text(strKuOn);
	Sleep(1000);

	if(byKuOnSum < g_byKuOnMax_SetKu)
	{	
		if(!g_bStopKuCtrl)
		{
			if(g_pStKuCtrl[nKuIndex].bFlagOn)	//不同开判断条件
			{
				str += "  已开机";
				pEdit->SetWindowTextA(str);
				Doing2Text(str);
				Sleep(2000);
			}
			else
			{
				if(byKuOnSum == 0)
				{
					//151117
					Temp_On_AllOnOneOff(pEdit, nKuIndex, str, "全部库房");
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
		while((!g_bStopKuCtrl) && (byKuOnSum >= g_byKuOnMax_SetKu))
		{			
			if(g_nCtrlMode == 1)
			{	
				//150924
				strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND \
					KuStatus = 1 AND WorkStatus = 1 AND KuCtrlMode = '温开温关' ORDER BY KuNum ASC";
			}
			else if(g_nCtrlMode == 2)
			{
				strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1 ORDER BY KuNum ASC";			
			}

			m_pRs_TempMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//78.1
			while(!m_pRs_TempMax->adoEOF)
			{	
				CString strKuOnNum = (LPCSTR)_bstr_t(m_pRs_TempMax->GetCollect("KuNum")); 
				int nKuOnNum = atoi(strKuOnNum);

				CString strKuOnWait = "";
				strKuOnWait.Format("已开机库房%d, 进行温度控制[温温循环]", nKuOnNum);
				pEdit->SetWindowTextA(strKuOnWait);
				Doing2Text(strKuOnWait);
				Write_WatchFile();	//喂狗
				Sleep(1000);

				//查询库房号在结构体数组中的序号nKuOnIndex
				int nKuOnIndex = GetKuIndex_St(nKuOnNum, TRUE);
				if(nKuOnIndex < 0)	break;
				
				//查找开关状态
				strSql.Format("SELECT * FROM SetKu WHERE KuNum = %d AND KuNum > 0 AND KuStatus = 1", g_pStKuCtrl[nKuOnIndex].byKuNum);
				m_pRs_KuStatus = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//91.1
				CString strRuKuING = (LPCSTR)_bstr_t(m_pRs_KuStatus->GetCollect("RuKuING"));
				m_pRs_KuStatus->raw_Close();	//91.2
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
					m_pRs_TempMax->MoveNext();
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
						pEdit->SetWindowTextA(strOn);
						Doing2Text(strOn);
						Sleep(1000);
					}		
				}	

				g_pStKuCtrl[nKuOnIndex].fTemp = 0.0;

				//开关机标志
				g_pStKuCtrl[nKuOnIndex].bFlagOn = GetKuStatus(FALSE, g_pStKuCtrl[nKuOnIndex].byKuNum, pEdit);

				m_pRs_TempMax->MoveNext();
			}
			m_pRs_TempMax->raw_Close();	//78.2

			Write_WatchFile();	//喂狗	//151024
			
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";
			m_pRs_TempMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//79.1
			byKuOnSum = 0;
			while(!m_pRs_TempMax->adoEOF)
			{
				byKuOnSum ++;
				m_pRs_TempMax->MoveNext();
			}
			m_pRs_TempMax->raw_Close();	//79.2

			//KuOnSum  加入FJ
			LONG lThreadDelay3Exit = 0;
			GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);
			if(STILL_ACTIVE == lThreadDelay3Exit)
			{
				strSql = "SELECT * FROM PropCHSH WHERE Item = '风机'";
				m_pRs_TempMax = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//80.1
				int nSumDelay3 = 0;
				while(!m_pRs_TempMax->adoEOF)
				{
					nSumDelay3++;
					m_pRs_TempMax->MoveNext();
				}
				m_pRs_TempMax->raw_Close();	//80.2
				byKuOnSum += nSumDelay3;

				if(byKuOnSum >= g_byKuOnMax_SetKu)
				{
					//pEdit->SetWindowTextA("同开最多数，等待延时关");
					Doing2Text("同开最多数，等待延时关");
					Sleep(1000);				
				}
			}

			if(byKuOnSum >= g_byKuOnMax_SetKu)
				GetKuTemp_OffKu(pEdit);

			if(g_bStopKuCtrl)
				g_pCanDlg->KillTimer(TIMER_ID_TEMP);
		}

		//跳出循环后，首先执行该库房
		if(!g_bStopKuCtrl)
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
		g_bKuOnMaxWhile = FALSE;
	}
}
void Temp_On_AllOnOneOff(CEdit *pEdit, int nKuIndex, CString str, CString strKuNum)
{
	CString strSql = "";

	(strcmp(strKuNum, "全部库房") == 0)?	//为查看是否全部关机中
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1":
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1 AND PartKu = 1";
	
	m_pRs_TempTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//160.1
	if(m_pRs_TempTemp->BOF && !g_bInTimer2)	//为空，即全部关机中
	{
		str += "  执行开机流程";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TempTemp->raw_Close();	//160.2.1

		//开启温度遍历
		g_pCanDlg->ST_TempRefer();

		if(strcmp(strKuNum, "全部库房") == 0)
		{
			if(g_bOnProp_MaxOn)	//151117 同开异关
			{
				//SelectSqlAllKu(aSetFunc[0], pEdit);	


				//161029 - 多控制模式，温开温关导致的开机，只开温开温关的库房
				if (g_nCtrlMode == 1)
				{
					//151117
					SelectSqlAllKu(aSetFunc[0], pEdit);
				}
				else
				{
					CString strSql = "";
					strSql = "update SetKu set PartKu = 1 where KuNum > 0 and KuStatus = 1 and KuCtrlMode = '温开温关'";
					m_pCon->Execute(_bstr_t(strSql), NULL, adCmdText);
					strSql = "update SetKu set PartKu = 0 where KuNum > 0 and KuStatus = 1 and KuCtrlMode != '温开温关'";
					m_pCon->Execute(_bstr_t(strSql), NULL, adCmdText);
					SelectSqlSomeKu(aSetFunc[0], pEdit, g_strDualCtrlMode_TempTemp);
				}
			}
			else
			{
				CString strKuExeNum = "";
				strKuExeNum.Format("%d", g_pStKuCtrl[nKuIndex].byKuNum);
				SelectSqlSomeKu(aSetFunc[0], pEdit, strKuExeNum);
			}
			g_StProp.strKuNum = "全部库房";
		}
		else
		{
			if(g_bOnProp_MaxOn)	//151117 同开异关
			{
				SelectSqlSomeKu(aSetFunc[0], pEdit, strKuNum);
				g_StProp.strKuNum = strKuNum;
			}
			else
			{
				CString strKuExeNum = "";
				strKuExeNum.Format("%d", g_pStKuCtrl[nKuIndex].byKuNum);
				SelectSqlSomeKu(aSetFunc[0], pEdit, strKuExeNum);
				g_StProp.strKuNum = strKuExeNum;				
			}
		}

		g_StProp.strSqlProp = "PropSomeKu";	
		g_StProp.Func = aSetFunc[0];	//执行的是开机流程
		g_StProp.pEdit = pEdit;	
		g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;	//那个库房触发的流程执行

		ST_AllOnAllOff(g_StProp);			

		g_bFlagAllYSJOff = FALSE;

		//关闭温度遍历
		g_pCanDlg->SP_TempRefer();
	}
	else
	{
		m_pRs_TempTemp->raw_Close();	//160.2.2
		
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
			//g_StProp.strSqlProp = "SetProp";
			//OneOnOneOff不需要strKuNum

			ST_OneOnOneOff(g_StProp);					
		}					
	}
}
void Temp_On_AllOnAllOff(CEdit *pEdit, int nKuIndex, CString str)
{
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuStatus = 1 AND WorkStatus = 0 AND KuCtrlMode = '温开温关' AND KuWorkMode = '同开同关'";
	m_pRs_TempTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//159.1
	if(m_pRs_TempTemp->BOF)	//全部开机
	{
		str += "  已开机";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TempTemp->raw_Close();	//159.2.1
	}
	else
	{
		str += "  执行全部开机流程";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TempTemp->raw_Close();	//159.2.2

		g_StProp.Func = aSetFunc[0];	//执行的是开机流程
		g_StProp.pEdit = pEdit;	
		g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;//那个库房触发的流程执行

		SelectSqlAllKu(aSetFunc[0], pEdit);
		g_StProp.strKuNum = "全部库房";
		g_StProp.strSqlProp = "PropSomeKu";	

		ST_AllOnAllOff(g_StProp);					
	}
}
void TempOrTime_On_OneOnOneOff(CEdit *pEdit, int nKuIndex, CString str)
{//Time_On_OneOnOneOff中亦调用
	if(g_pStKuCtrl[nKuIndex].bFlagOn)	//不同开判断条件
	{
		str += "  已开机";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(2000);
	}
	else
	{
		str += "  执行该库房开机流程";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(2000);

		g_StProp.Func = aSetFunc[0];	//执行的是开机流程
		g_StProp.pEdit = pEdit;	
		g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;//那个库房触发的流程执行

		ST_OneOnOneOff(g_StProp);			
	}
}
void Temp_Off_AllOff(CEdit *pEdit, int nKuIndex, CString str)
{
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuStatus = 1 AND WorkStatus = 1 AND KuWorkMode = '同开同关'";
	m_pRs_TempTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//158.1
	if(m_pRs_TempTemp->BOF)				
	{
		str += "  已关机";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TempTemp->raw_Close();	//158.2.1
	}
	else
	{
		str += "  执行全部关机流程";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TempTemp->raw_Close();	//158.2.2

		g_StProp.Func = aSetFunc[1];
		g_StProp.pEdit = pEdit;
		g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;//那个库房触发的流程执行

		SelectSqlAllKu(aSetFunc[1], pEdit);
		g_StProp.strKuNum = "全部库房";
		g_StProp.strSqlProp = "PropSomeKu";	

		ST_AllOnAllOff(g_StProp);					
	}
}
void TempOrTime_Off_OneOff(CEdit *pEdit, int nKuIndex, CString str)
{//Time_Off_OneOnOneOff中亦调用
	if(!g_pStKuCtrl[nKuIndex].bFlagOn)	//异关判断条件
	{
		str += "  已关机";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
	}
	else
	{
		str += "  执行该库房关机流程";
		pEdit->SetWindowTextA(_T(str));
		Doing2Text(str);
		Sleep(1000);

		g_StProp.Func = aSetFunc[1];
		g_StProp.pEdit = pEdit;
		g_StProp.byKuNum = g_pStKuCtrl[nKuIndex].byKuNum;//那个库房触发的流程执行

		ST_OneOnOneOff(g_StProp);			
	}
}
void KuTempCtrl_TempTemp(int nKuIndex, CEdit *pEdit, CString strWorkMode)
{
	//150924
	(strcmp(strWorkMode, aKuWorkMode[2]) == 0)? g_bOnProp_MaxOn = FALSE: g_bOnProp_MaxOn = TRUE;
	//开关机状态
	g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(TRUE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);

	if(g_bStopKuCtrl)	//退出库房监控与流程执行
	{
		g_bStopProp = TRUE;
		return;
	}

	//获取温度
	int nCorrectTemp = 0;
	nCorrectTemp = GetKuTemp(pEdit, nKuIndex);
	if(nCorrectTemp > 0)
	{
		//比较温度
		if(g_pStKuCtrl[nKuIndex].fTemp > g_pStKuCtrl[nKuIndex].fTempUp)
		{	
			CString str = "";
			str.Format("库房%d: %.1f 温度高于上限"
				, g_pStKuCtrl[nKuIndex].byKuNum, g_pStKuCtrl[nKuIndex].fTemp);
			Doing2Text(str);
			
			if(!g_pStKuCtrl[nKuIndex].bFlagOn)
			{
				//150908
				pEdit->SetWindowTextA("默认，进入同开最多控制模式");
				Doing2Text("默认，进入同开最多控制模式");
				Sleep(1000);
				TempOnTempOff_TempOn_KuOnMax(pEdit, nKuIndex, str);	
			}
			else
			{
				str += "  已开机";
				pEdit->SetWindowTextA(str);
				Doing2Text(str);
				Sleep(1000);			
			}
		}
		else if(g_pStKuCtrl[nKuIndex].fTemp < g_pStKuCtrl[nKuIndex].fTempDn)
		{	
			CString str = "";
			str.Format("库房%d: %.1f 温度低于下限"
				, g_pStKuCtrl[nKuIndex].byKuNum, g_pStKuCtrl[nKuIndex].fTemp);
			Doing2Text(str);

			if(g_pStKuCtrl[nKuIndex].bFlagOn)
			{
				//150924-默认进入异关
				TempOrTime_Off_OneOff(pEdit, nKuIndex, str);		//同开异关 异开异关		
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

	if(g_bStopKuCtrl)
	{
		g_bStopProp = TRUE;
		return;
	}

	if(!g_bStopKuCtrl && g_bFlagAllKuOff && !g_bFlagAllYSJOff && g_bAllKuDoOnce_TempTemp)
	{
		pEdit->SetWindowTextA("温开温关，库房已遍历一遍，且已全部关机，则执行关机流程");
		Doing2Text("温开温关，库房已遍历一遍，且已全部关机，则执行关机流程");
		Sleep(3000);

		//会再关闭一遍风机
		SelectSqlAllKu(aSetFunc[1], pEdit);
		g_StProp.strKuNum = "全部库房";
		g_StProp.strSqlProp = "PropSomeKu";	

		g_StProp.Func = aSetFunc[1];
		g_StProp.pEdit = pEdit;
		g_StProp.byKuNum = 0;

		//开启温度遍历
		g_pCanDlg->ST_TempRefer();

		ST_AllOnAllOff(g_StProp);	

		g_bFlagAllYSJOff = TRUE;
		g_bAllKuDoOnce_TempTemp = FALSE;

		//关闭温度遍历
		g_pCanDlg->SP_TempRefer();
	}
}
void TempOnTempOff(CEdit *pEdit, CString strWorkMode)
{
	//151203
	if(g_nCtrlMode == 1)
	{
		g_bFlagAllYSJOff = TRUE;	//150907复位
		g_bAllKuDoOnce_TempTemp = FALSE;	//150929
		g_bKuOnMaxWhile = FALSE;
	}
	if(g_bOneKeyStart)
	{
		if(g_nCtrlMode == 2)
		{
			if(!g_bFlagAllYSJOff)
			{
				pEdit->SetWindowTextA("混合控制，手动开机，已开机");
				Doing2Text("混合控制，手动开机，已开机");
				Sleep(1500);
			}
			else
			{
				pEdit->SetWindowTextA("混合控制，已执行一遍，等待停止库控");
				Doing2Text("混合控制，已执行一遍，等待停止库控");
				Sleep(1500);
				return;
			}
		}
		else
		{			
			pEdit->SetWindowTextA("手动直接开机，将开启最多在用风机");
			Doing2Text("手动直接开机，将开启最多在用风机");
			Sleep(1500);
			
			if(!g_byKuOnSum)
			{
				g_StProp.Func = aSetFunc[0];	//执行的是开机流程
				g_StProp.pEdit = pEdit;	
				g_StProp.byKuNum = 0;//那个库房触发的流程执行

				SelectSqlAllKu(aSetFunc[0], pEdit);
				g_StProp.strKuNum = "全部库房";
				g_StProp.strSqlProp = "PropSomeKu";	
				
				//开启温度遍历
				g_pCanDlg->ST_TempRefer();
				ST_AllOnAllOff(g_StProp);
			}

			g_bFlagAllYSJOff = FALSE;

			g_pCanDlg->SP_TempRefer();

			if(!g_bStopKuCtrl)
			{
				pEdit->SetWindowTextA("手动直接开机结束，进入温控");
				Doing2Text("手动直接开机结束，进入温控");
				Sleep(1500);			
			}
		}
	}
	else
	{
		//151203
		if(g_nCtrlMode == 1)
		{
			//151111 不是直接开机，而是一键开机
			CString strSql = "";
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";
			m_pRs_TempTemp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151111-3.1
			if(!m_pRs_TempTemp->BOF)
			{
				pEdit->SetWindowTextA("已存在库房开机，认为压缩机全部开启");
				Doing2Text("已存在库房开机，认为压缩机全部开启");
				Sleep(1000);

				g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(TRUE);	//一键关机，执行完开机流程后，使能
				g_pCanDlg->GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(TRUE);	//紧急关机，是执行完全部流程后	
				//151102
				g_pCanDlg->GetDlgItem(IDC_CHECK_YSJ_MANUAL)->EnableWindow(TRUE);	//手动开关压缩机
				for(int i=0; i<g_byYSJSum_SetKu; i++)
				{
					g_pStYSJ[i].bNC = TRUE;
				}
				g_byYSJOnSum = g_byYSJSum_SetKu;
			}
			m_pRs_TempTemp->raw_Close();	//151111-3.2
		}
	}
	
	if(g_nCtrlMode == 1)
	{
		while(!g_bStopKuCtrl)
		{	
			for(int nKuIndex = 0; nKuIndex<g_byKuCtrlSum_St; nKuIndex++)
			{		
				Write_WatchFile();	//喂狗
				KuTempCtrl_TempTemp(nKuIndex, pEdit, strWorkMode);
				if(g_bStopKuCtrl || g_bStopProp)
				{
					break;
				}
			}//库房号循环
			g_bAllKuDoOnce_TempTemp = TRUE;
		}//正常情况下无限循环
	}	
	else if(g_nCtrlMode == 2)	//150807
	{
		//150807仅有一种控制模式，则循环即可；多种控制模式，则要【筛选】
		for(int nIndexTempTemp = 0; nIndexTempTemp < g_byTempTempSum; nIndexTempTemp++)	//温开温关库房数内循环
		{
			Write_WatchFile();	//喂狗
			//查找温开温关库房数在全库结构体中的序号
			int nKuIndex = GetKuIndex_St(g_aKuNumTempTemp[nIndexTempTemp], TRUE);
			if(nKuIndex < 0)	break;

			KuTempCtrl_TempTemp(nKuIndex, pEdit, strWorkMode);
			if(g_bStopKuCtrl || g_bStopProp)
			{
				break;
			}
		}	
		g_bAllKuDoOnce_TempTemp = TRUE;
	} 
	else
	{
		Auto_UrgentStop("温开温关 模式错误");
	}
}
