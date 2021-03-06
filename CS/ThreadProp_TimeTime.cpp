#include "stdafx.h"
#include "CS.h"
#include "AdvCanThread.h"
#include "CANDlg.h"
#include <ATLComTime.h>

#include "ThreadProp_TempTemp.h"
#include "ThreadProp.h"
#include "ThreadProp_TimeTemp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _ConnectionPtr m_pCon;
extern _RecordsetPtr m_pRs_TimeTime;
extern _RecordsetPtr m_pRs_KuNum;		//筛选某个库
extern _RecordsetPtr m_pRs_KuProp;
extern _RecordsetPtr m_pRs_AddKuProp;
extern _RecordsetPtr m_pRs_SelectKu;	//筛选几个库

extern CString aSetFunc[NumSetFunc];
extern CString aKuWorkMode[NumKuWorkMode];

extern CCANDlg *g_pCanDlg;

extern BYTE g_byTimeCtrlSum_St;
extern BYTE g_byKuCtrlSum_St;

extern Prop_t g_StProp;
extern KuCtrl_t *g_pStKuCtrl;
extern TimeCtrl_t *g_pStTimeCtrl;
extern YSJ_t *g_pStYSJ;


extern BOOL g_bFlagInSpan;
extern BOOL g_bStopKuCtrl;
extern BOOL g_bStopProp;
extern BOOL g_bFlagAllYSJOff;

extern BYTE g_byKuOnMax_SetKu;			//同开最多库房数
extern BYTE g_byKuSum_SetKu;			//库房总数
extern BYTE g_byTempSum_SetKu;			//温度节点总数
extern BYTE g_byYSJSum_SetKu;			//压缩机总数

extern COleDateTime g_mTimeNow;

extern int g_nCtrlMode;

extern CString g_strNowTimeKu;

void YSJSomeku(CString strFunc, CEdit *pEdit, int nKuItemNum, int nKuNum)
{	
	CString aSqlYSJItem[6] = {"SB", "YSJ", "XQ", "PQ", "AF", "AB"};
	CString aYSJProp[6]={"水泵", "压缩机", "吸气", "排气", "氨阀", "氨泵"};

	CString strSql = "";
	strSql.Format("SELECT * FROM SetYSJ WHERE YSJ = %d", nKuItemNum );
	m_pRs_KuProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//72.1

	if(!m_pRs_KuProp->BOF)
	{
		m_pRs_KuProp->raw_Close();	//72.2.1

		for(int k=0; k<6; k++)
		{
			strSql.Format("SELECT * FROM SetYSJ WHERE YSJ = %d", nKuItemNum );
			m_pRs_KuProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//73.1
			CString strYSJItemNum = (LPCSTR)_bstr_t(m_pRs_KuProp->GetCollect((_bstr_t)aSqlYSJItem[k]));
			int nYSJItemNum = atoi(strYSJItemNum);  
			m_pRs_KuProp->raw_Close();	//73.2

			strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' AND Item = '%s' AND Num = %d"
				, strFunc ,aYSJProp[k] ,nYSJItemNum );
			m_pRs_KuProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//74.1
			if(!m_pRs_KuProp->BOF)
			{
				CString strYSJItemZu = (LPCSTR)_bstr_t(m_pRs_KuProp->GetCollect("ZuItem"));
				CString strYSJItemZuNum = (LPCSTR)_bstr_t(m_pRs_KuProp->GetCollect("ZuNum"));
				int nYSJItemZuNum = atoi(strYSJItemZuNum);
				m_pRs_KuProp->raw_Close();	//74.2.1

				//CString strZu = "";
				//strZu.Format("库房%d-压缩机%d 的%s%d 在%s的分组为 %s%s"
				//	, nKuNum, nKuItemNum, aYSJProp[k], nYSJItemNum, strFunc, strYSJItemZu, strYSJItemZuNum);
				//AfxMessageBox(strZu);

				strSql.Format("SELECT * FROM PropSomeKu WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
					, strFunc, strYSJItemZu, nYSJItemZuNum);
				m_pRs_KuProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//75.1
				if(m_pRs_KuProp->BOF)
				{
					strSql.Format("INSERT INTO PropSomeKu\
								  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
								  FROM SetProp WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
								  , strFunc, strYSJItemZu, nYSJItemZuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);										
				}
				m_pRs_KuProp->raw_Close();	//75.2							
			}
			else
			{
				m_pRs_KuProp->raw_Close();	//74.2.2
				//CString strNone = "";
				//strNone.Format("库房%d-压缩机%d 的%s%d 在%s中不存在"
				//	,nKuNum, nKuItemNum, aYSJProp[k], nYSJItemNum, strFunc);
				//AfxMessageBox(strNone);
			}
		}
	}
	else
	{
		m_pRs_KuProp->raw_Close();	//72.2.2
		CString strNoneYSJ = "";
		strNoneYSJ.Format("库房%d的压缩机%d 不存在",nKuNum, nKuItemNum);
		//AfxMessageBox(strNoneYSJ);						
	}
}

void KuSomeKu(CString strFunc, CEdit *pEdit, int nKuNum)
{
	CString aSqlPropItem[4] = {"KuFY", "KuHQ", "KuYSJ", "KuFJ"};
	CString aKuProp[4]={"分液", "回气", "压缩机", "风机"};

	CString strSql = "";
	strSql.Format("SELECT * FROM SetKu WHERE KuNum = %d", nKuNum);
	m_pRs_KuNum = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//69.1

	if(!m_pRs_KuNum->BOF)
	{
		CString strKuItemNum = "";
		for(int j=0; j<4; j++)
		{
			//风机的特殊处理
			int nKuItemNum = 0;
			if(j != 3)
			{
				strKuItemNum = (LPCSTR)_bstr_t(m_pRs_KuNum->GetCollect((_bstr_t)aSqlPropItem[j]));
				nKuItemNum = atoi(strKuItemNum);

				//压缩机各项目查找
				if(j == 2)
				{
					BYTE byYSJNum[4] = {0,0,0,0};
					Str2Dec(strKuItemNum, byYSJNum);
					
					for(int i=0; i<4; i++)
					{		
						if(byYSJNum[i] == 0)
						{
							break;
						}
						else
						{
							YSJSomeku(strFunc, pEdit, byYSJNum[i], nKuNum);
						}
					}
				}
			}
			else
			{
				nKuItemNum = 0;
			}

			strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' AND Item = '%s' AND Num = %d"
				, strFunc ,aKuProp[j] ,nKuItemNum );
			m_pRs_KuProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//70.1
			//150825-FJ-12
			CString strKuItemZu = "";
			CString strKuItemZuNum = "";
			int nKuItemZuNum = 0;
			while(!m_pRs_KuProp->adoEOF)
			{	
				strKuItemZu = (LPCSTR)_bstr_t(m_pRs_KuProp->GetCollect("ZuItem"));
				strKuItemZuNum = (LPCSTR)_bstr_t(m_pRs_KuProp->GetCollect("ZuNum"));
				nKuItemZuNum = atoi(strKuItemZuNum);

				strSql.Format("SELECT * FROM PropSomeKu WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
					, strFunc, strKuItemZu, nKuItemZuNum);
				m_pRs_AddKuProp = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//71.1
				if(m_pRs_AddKuProp->BOF)	//150825 若流程已存在，则不添加
				{
					strSql.Format("INSERT INTO PropSomeKu\
								  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
								  FROM SetProp WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
								  , strFunc, strKuItemZu, nKuItemZuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);					
				}
				m_pRs_AddKuProp->raw_Close();	//71.2			

				//150827 双分液特殊处理，如果不是双分液，则删除此分液之前的
				if(j==0)
				{
					if( strKuItemNum.GetLength() < 3)
					{
						strSql.Format("DELETE FROM PropSomeKu WHERE Item = '分液' AND Num != %d \
							AND Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
							, nKuItemNum, strFunc, strKuItemZu, nKuItemZuNum);
						m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
					}
				}
				
				m_pRs_KuProp->MoveNext();
			}
			m_pRs_KuProp->raw_Close();	//70.2

			//150908 若SetKu的回气为0--一般会单独不装回气，所以单独特殊处理
			if((j == 1) && (nKuItemNum==0))
			{
				strSql.Format("DELETE FROM PropSomeKu WHERE Item = '回气'\
					AND Func = '%s' AND ZuItem = '库房' AND ZuNum = %d", strFunc, nKuNum);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);					
			}
		}//依次查找比较库房项目
	}//某库房的查找
	else
	{
		CString strNoneKu = "";
		strNoneKu.Format("库房%d 不存在",nKuNum);	
	}//某库房可能不存在
	m_pRs_KuNum->raw_Close();	//69.2
}

void SelectSqlSomeKu(CString strFunc, CEdit *pEdit, CString strKuNum)
{
	CString strSql = "";
	strSql = "DELETE PropSomeKu";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	//151117 - 单独某个库房温度到导致开机TempOnAllOnOneOff
	(strKuNum.GetLength() > 2)?
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND PartKu = 1 ORDER BY KuNum ASC":
		strSql.Format("SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuNum = %d", atoi(strKuNum));

	//筛选在用库房的流程
	//strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND PartKu = 1 ORDER BY KuNum ASC";
	m_pRs_SelectKu = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//66.1
	while(!m_pRs_SelectKu->adoEOF)
	{
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs_SelectKu->GetCollect("KuNum"));
		int nKuNum = atoi(strKuNum);

		KuSomeKu(strFunc, pEdit, nKuNum);

		m_pRs_SelectKu->MoveNext();
	}
	m_pRs_SelectKu->raw_Close();	//66.2

	//150828-确认添加延时2,仅能有1个 开机流程 延时2
	strSql = "SELECT * FROM SetProp WHERE Func = '开机流程' AND Item = '延时' AND Num = 2";
	m_pRs_SelectKu = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//67.1
	if(!m_pRs_SelectKu->BOF)
	{
		CString strKuItemZu = (LPCSTR)_bstr_t(m_pRs_SelectKu->GetCollect("ZuItem"));
		CString strKuItemZuNum = (LPCSTR)_bstr_t(m_pRs_SelectKu->GetCollect("ZuNum"));
		int nKuItemZuNum = atoi(strKuItemZuNum);
		m_pRs_SelectKu->raw_Close();	//67.2.1

		strSql.Format("SELECT * FROM PropSomeKu WHERE Func = '开机流程' AND ZuItem = '%s' AND ZuNum = %d"
			, strKuItemZu, nKuItemZuNum);
		m_pRs_SelectKu = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//68.1
		if(m_pRs_SelectKu->BOF)	//150825 若流程已存在，则不添加
		{
			strSql.Format("INSERT INTO PropSomeKu\
						  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
						  FROM SetProp WHERE Func = '开机流程' AND ZuItem = '%s' AND ZuNum = %d"
						  , strKuItemZu, nKuItemZuNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);					
		}
		m_pRs_SelectKu->raw_Close();	//68.2		
	}
	else
	{
		m_pRs_SelectKu->raw_Close();	//67.2.2
	}

	CString strSomeKu = "";
	strSomeKu.Format("库房%s %s 筛选完毕", strKuNum, strFunc);
	pEdit->SetWindowTextA(strSomeKu);
	Sleep(1000);
}

void SelectSqlAllKu(CString strFunc, CEdit *pEdit)
{
	CString strSql = "";
	strSql = "DELETE PropSomeKu";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	//筛选在用库房的流程
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 ORDER BY KuNum ASC";
	m_pRs_SelectKu = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//63.1

	while(!m_pRs_SelectKu->adoEOF)
	{
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs_SelectKu->GetCollect("KuNum"));
		int nKuNum = atoi(strKuNum);

		KuSomeKu(strFunc, pEdit, nKuNum);

		m_pRs_SelectKu->MoveNext();
	}
	m_pRs_SelectKu->raw_Close();	//63.2

	//150828-确认添加延时2,仅能有1个 开机流程 延时2
	strSql = "SELECT * FROM SetProp WHERE Func = '开机流程' AND Item = '延时' AND Num = 2";
	m_pRs_SelectKu = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//64.1
	if(!m_pRs_SelectKu->BOF)
	{
		CString strKuItemZu = (LPCSTR)_bstr_t(m_pRs_SelectKu->GetCollect("ZuItem"));
		CString strKuItemZuNum = (LPCSTR)_bstr_t(m_pRs_SelectKu->GetCollect("ZuNum"));
		int nKuItemZuNum = atoi(strKuItemZuNum);
		m_pRs_SelectKu->raw_Close();	//64.2.1

		strSql.Format("SELECT * FROM PropSomeKu WHERE Func = '开机流程' AND ZuItem = '%s' AND ZuNum = %d"
			, strKuItemZu, nKuItemZuNum);
		m_pRs_SelectKu = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//65.1
		if(m_pRs_SelectKu->BOF)	//150825 若流程已存在，则不添加
		{
			strSql.Format("INSERT INTO PropSomeKu\
						  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
						  FROM SetProp WHERE Func = '开机流程' AND ZuItem = '%s' AND ZuNum = %d"
						  , strKuItemZu, nKuItemZuNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);					
		}
		m_pRs_SelectKu->raw_Close();	//65.2				

	}
	else
	{
		m_pRs_SelectKu->raw_Close();	//64.2.2
	}

	CString strAllKu = "";
	strAllKu.Format("全部库房 %s 筛选完毕", strFunc);
	pEdit->SetWindowTextA(strAllKu);
	Sleep(1000);
}

//
int GetTimeCtrlSt()			//获得结构体数组
{
	//获取该模式下在用库房总数
	CString strSql = "";
	strSql = "SELECT * FROM SetFGYD ORDER BY TimeStartH, TimeStartM ASC";
	m_pRs_TimeTime = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//170.1

	if(m_pRs_TimeTime->BOF)			//未存在该库房数
	{
		//AfxMessageBox("峰谷用电未设置");
		m_pRs_TimeTime->raw_Close();	//170.2.1
		return -1;
	}

	g_byTimeCtrlSum_St = 0;	
	while(!m_pRs_TimeTime->adoEOF)
	{
		g_byTimeCtrlSum_St ++;
		m_pRs_TimeTime->MoveNext();
	}
	//m_pRs_TimeTime->Close();		//将游标返回即可

	//动态申请结构体数组
	if(g_pStTimeCtrl != NULL)
	{
		delete [] g_pStTimeCtrl;
		g_pStTimeCtrl = NULL;
	}
	g_pStTimeCtrl = new TimeCtrl_t[g_byTimeCtrlSum_St];
	//全局变量赋初值
	for(int i=0; i<g_byTimeCtrlSum_St; i++)
	{
		g_pStTimeCtrl[i].TimeStart.ParseDateTime("1990-6-14 8:8:8");
		g_pStTimeCtrl[i].TimeStop.ParseDateTime("1990-6-14 8:8:8");
		g_pStTimeCtrl[i].strTimeKuNum = "全部库房";
	}

	//填充结构体数组成员
	m_pRs_TimeTime->MoveFirst();
	int nTimeIndex = 0;
	while(!m_pRs_TimeTime->adoEOF)
	{
		CString strTimeStartH = "", strTimeStartM = "", strTimeStopH = "", strTimeStopM = "";
		strTimeStartH = (LPCSTR)_bstr_t(m_pRs_TimeTime->GetCollect("TimeStartH"));
		strTimeStartM = (LPCSTR)_bstr_t(m_pRs_TimeTime->GetCollect("TimeStartM"));
		strTimeStopH = (LPCSTR)_bstr_t(m_pRs_TimeTime->GetCollect("TimeStopH"));
		strTimeStopM = (LPCSTR)_bstr_t(m_pRs_TimeTime->GetCollect("TimeStopM"));	

		CString strTimeStartSet = "";
		strTimeStartSet.Format("%s:%s:0 14 6 1990", strTimeStartH, strTimeStartM);
		g_pStTimeCtrl[nTimeIndex].TimeStart.ParseDateTime(strTimeStartSet);

		CString strTimeStopSet = "";
		strTimeStopSet.Format("%s:%s:0 14 6 1990", strTimeStopH, strTimeStopM);
		g_pStTimeCtrl[nTimeIndex].TimeStop.ParseDateTime(strTimeStopSet);

		CString strTimeKuNum = (LPCSTR)_bstr_t(m_pRs_TimeTime->GetCollect("TimeKuNum"));
		g_pStTimeCtrl[nTimeIndex].strTimeKuNum = strTimeKuNum;

		nTimeIndex ++;
		m_pRs_TimeTime->MoveNext();
	}

	m_pRs_TimeTime->raw_Close();	//170.2.2
	return 0;
}
int NowTimeSpan(CEdit *pEdit, CString strWorkMode)
{
	CString strTimeNumSpan = "";
	int i = 0;	//作为外部变量进行值传递，所处时间段
	for(i=0; i<g_byTimeCtrlSum_St; i++)
	{
		//获取当前时间
		int nNowYear = g_mTimeNow.GetYear();
		int nNowMonth = g_mTimeNow.GetMonth();
		int nNowDay = g_mTimeNow.GetDay();
		int nChangeYear = nNowYear;
		int nChangeMonth = nNowMonth;
		int nChangeDay = nNowDay;

		//获取当前起止时间
		if(g_pStTimeCtrl[i].TimeStart.GetHour() <= g_pStTimeCtrl[i].TimeStop.GetHour())
		{//如果当前时间段没有跨越0点，即起始小时不大于终止小时，则直接将终止时间年月日相等当前时间
			g_pStTimeCtrl[i].TimeStop.SetDateTime(nNowYear, nNowMonth, nNowDay, 
				g_pStTimeCtrl[i].TimeStop.GetHour(), g_pStTimeCtrl[i].TimeStop.GetMinute(), 
				g_pStTimeCtrl[i].TimeStop.GetSecond());		
		}
		else
		{//如果当前时间跨越0点，即起始小时大于终止小时
			if(g_mTimeNow.GetHour() >= g_pStTimeCtrl[i].TimeStart.GetHour())
			{//跨越0点，当前小时不小于起始小时，则当前时间还在当天，则要处理终止时间，以达到结果：当前时间小于终止时间
				//不能简单地日期加1，时间会出错
				if( ((nNowYear%4==0)&&(nNowYear%100!=0)) || (nNowYear%400==0) )
				{//闰年，2月有29天
					switch(nNowMonth)
					{
					case 1:
					case 3:
					case 5:
					case 7:
					case 8:
					case 10:
					case 12:
						{
							if(nNowDay == 31)
							{
								if(nNowMonth == 12)
								{
									nChangeYear ++;
									nChangeMonth = 1;
								}
								else
								{
									nChangeMonth ++;
								}
								nChangeDay = 1;
							}
							else
							{
								nChangeDay ++;
							}
							break;
						}
					case 4:
					case 6:
					case 9:
					case 11:
						{
							if(nNowDay == 30)
							{
								nChangeMonth ++;
								nChangeDay = 1;
							}
							else
							{
								nChangeDay ++;
							}	
							break;
						}
					case 2:
						{
							if(nNowDay == 29)
							{
								nChangeMonth ++;
								nChangeDay = 1;
							}
							else
							{
								nChangeDay ++;
							}	
							break;						
						}
					}//switch		
				}
				else
				{//不是闰年，2月28天
					switch(nNowMonth)
					{
					case 1:
					case 3:
					case 5:
					case 7:
					case 8:
					case 10:
					case 12:
						{
							if(nNowDay == 31)
							{
								if(nNowMonth == 12)
								{
									nChangeYear ++;
									nChangeMonth = 1;
								}
								else
								{
									nChangeMonth ++;
								}
								nChangeDay = 1;
							}
							else
							{
								nChangeDay ++;
							}
							break;
						}
					case 4:
					case 6:
					case 9:
					case 11:
						{
							if(nNowDay == 30)
							{
								nChangeMonth ++;
								nChangeDay = 1;
							}
							else
							{
								nChangeDay ++;
							}	
							break;
						}
					case 2:
						{
							if(nNowDay == 28)
							{
								nChangeMonth ++;
								nChangeDay = 1;
							}
							else
							{
								nChangeDay ++;
							}	
							break;						
						}
					}//switch
				}//判断闰年

				g_pStTimeCtrl[i].TimeStop.SetDateTime(nChangeYear, nChangeMonth, nChangeDay, 
					g_pStTimeCtrl[i].TimeStop.GetHour(), g_pStTimeCtrl[i].TimeStop.GetMinute(), 
					g_pStTimeCtrl[i].TimeStop.GetSecond());	
			}
			else
			{//当前时间段跨越0点，但当前小时小于起始小时，即当前小时已经在第二天，则不必处理终止时间
				g_pStTimeCtrl[i].TimeStop.SetDateTime(nNowYear, nNowMonth, nNowDay, 
					g_pStTimeCtrl[i].TimeStop.GetHour(), g_pStTimeCtrl[i].TimeStop.GetMinute(), 
					g_pStTimeCtrl[i].TimeStop.GetSecond());				
			}//已经跨越0点，是否处理终止时间
		}//是否跨越0点

		if(g_mTimeNow <= g_pStTimeCtrl[i].TimeStop)
		{
			g_bFlagInSpan = TRUE;
			break;
		}

		if((i==g_byTimeCtrlSum_St-1) && (g_mTimeNow > g_pStTimeCtrl[i].TimeStop))
		{//如果超过所有终止时间，则位于第0时间段，依然一直循环读取时间段，等待第二天比较
			g_bFlagInSpan = FALSE;
			strTimeNumSpan="时控进行中，本次结束，等待下次";
			strTimeNumSpan += g_mTimeNow.Format("%H:%M:%S");
			pEdit->SetWindowTextA(strTimeNumSpan);
			Sleep(1000);

			//开启温度遍历
			g_pCanDlg->ST_TempRefer();

			//150807 4-4超过所有终止时间
			if((g_nCtrlMode == 2) && !g_bStopKuCtrl)
			{
				TempOnTempOff(pEdit, strWorkMode);
				GetKuTemp_OffKu(pEdit);
			}
		}
	}//for

	if(g_bFlagInSpan)
	{
		strTimeNumSpan.Format("当前时间位于第%d时间段", i+1);
		pEdit->SetWindowTextA(strTimeNumSpan);

		//150907
		if(strcmp(g_pStTimeCtrl[i].strTimeKuNum, "全部库房") == 0)
		{
			g_strNowTimeKu = "全部库房";
		}
		else
		{
			g_strNowTimeKu = g_pStTimeCtrl[i].strTimeKuNum;
			//150907-设置库房PartKu-TRUE
			//部分库房，字符串转化为数组，数组最大为库房总数
			BYTE *aKuNum = (BYTE *)malloc(g_byKuSum_SetKu * sizeof(BYTE));
			int nPartKuSum = Str2Dec(g_pStTimeCtrl[i].strTimeKuNum, aKuNum);
			CString strSql = "";
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0";
			m_pRs_TimeTime = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//169.1
			while(!m_pRs_TimeTime->adoEOF)
			{
				CString strKuNum = (LPCSTR)_bstr_t(m_pRs_TimeTime->GetCollect("KuNum")); 
				int nKuNum = atoi(strKuNum);
				int j = 0;
				for(j=0; j<nPartKuSum; j++)
				{
					if(nKuNum == aKuNum[j])
					{
						strSql.Format("UPDATE SetKu SET [PartKu] = 1 WHERE KuNum > 0 AND KuStatus = 1 AND KuNum = %d", aKuNum[j]);
						m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
						break;
					}
				}
				if(j == nPartKuSum)
				{
					strSql.Format("UPDATE SetKu SET [PartKu] = 0 WHERE KuNum > 0 AND KuNum = %d", nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
				}
				m_pRs_TimeTime->MoveNext();
			}
			m_pRs_TimeTime->raw_Close();	//169.2
			free(aKuNum);
		}

		Sleep(3000);
	}

	return i;
}
void GetTimeStartStop(COleDateTime TimeNow, int i)
{
	int nNowYear = TimeNow.GetYear();
	int nNowMonth = TimeNow.GetMonth();
	int nNowDay = TimeNow.GetDay();
	int nChangeYear = nNowYear;
	int nChangeMonth = nNowMonth;
	int nChangeDay = nNowDay;
	//获取当前起止时间
	if(g_pStTimeCtrl[i].TimeStart.GetHour() <= g_pStTimeCtrl[i].TimeStop.GetHour())
	{//如果当前时间段没有跨越0点，即起始小时不大于终止小时，则直接将起始、终止时间年月日相等当前时间,不必处理
		g_pStTimeCtrl[i].TimeStop.SetDateTime(nNowYear, nNowMonth, nNowDay, 
			g_pStTimeCtrl[i].TimeStop.GetHour(), g_pStTimeCtrl[i].TimeStop.GetMinute(), 
			g_pStTimeCtrl[i].TimeStop.GetSecond());	
		g_pStTimeCtrl[i].TimeStart.SetDateTime(nNowYear, nNowMonth, nNowDay, 
			g_pStTimeCtrl[i].TimeStart.GetHour(), g_pStTimeCtrl[i].TimeStart.GetMinute(), 
			g_pStTimeCtrl[i].TimeStart.GetSecond());
	}
	else
	{//如果当前时间跨越0点，即起始小时大于终止小时
		if(TimeNow.GetHour() >= g_pStTimeCtrl[i].TimeStart.GetHour())
		{//跨越0点，当前小时不小于起始小时，则当前时间还在当天，则直接将起始时间年月日等于当前时间年月日，
			//则要处理终止时间，以达到结果：当前时间小于终止时间
			//不能简单地日期加1，时间会出错
			if( ((nNowYear%4==0)&&(nNowYear%100!=0)) || (nNowYear%400==0) )
			{//闰年，2月有29天
				switch(nNowMonth)
				{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					{
						if(nNowDay == 31)
						{
							if(nNowMonth == 12)
							{
								nChangeYear ++;
								nChangeMonth = 1;
							}
							else
							{
								nChangeMonth ++;
							}
							nChangeDay = 1;
						}
						else
						{
							nChangeDay ++;
						}
						break;
					}
				case 4:
				case 6:
				case 9:
				case 11:
					{
						if(nNowDay == 30)
						{
							nChangeMonth ++;
							nChangeDay = 1;
						}
						else
						{
							nChangeDay ++;
						}	
						break;
					}
				case 2:
					{
						if(nNowDay == 29)
						{
							nChangeMonth ++;
							nChangeDay = 1;
						}
						else
						{
							nChangeDay ++;
						}	
						break;						
					}
				}//switch		
			}
			else
			{//不是闰年，2月28天
				switch(nNowMonth)
				{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					{
						if(nNowDay == 31)
						{
							if(nNowMonth == 12)
							{
								nChangeYear ++;
								nChangeMonth = 1;
							}
							else
							{
								nChangeMonth ++;
							}
							nChangeDay = 1;
						}
						else
						{
							nChangeDay ++;
						}
						break;
					}
				case 4:
				case 6:
				case 9:
				case 11:
					{
						if(nNowDay == 30)
						{
							nChangeMonth ++;
							nChangeDay = 1;
						}
						else
						{
							nChangeDay ++;
						}	
						break;
					}
				case 2:
					{
						if(nNowDay == 28)
						{
							nChangeMonth ++;
							nChangeDay = 1;
						}
						else
						{
							nChangeDay ++;
						}	
						break;						
					}
				}//switch
			}//判断闰年

			g_pStTimeCtrl[i].TimeStop.SetDateTime(nChangeYear, nChangeMonth, nChangeDay, 
				g_pStTimeCtrl[i].TimeStop.GetHour(), g_pStTimeCtrl[i].TimeStop.GetMinute(), 
				g_pStTimeCtrl[i].TimeStop.GetSecond());	

			g_pStTimeCtrl[i].TimeStart.SetDateTime(nNowYear, nNowMonth, nNowDay, 
				g_pStTimeCtrl[i].TimeStart.GetHour(), g_pStTimeCtrl[i].TimeStart.GetMinute(), 
				g_pStTimeCtrl[i].TimeStart.GetSecond());
		}
		else
		{//当前时间段跨越0点，但当前小时小于起始小时，即当前小时已经在第二天，则不必处理终止时间
			//要将起始时间日期减1，又要注意会出BUG
			g_pStTimeCtrl[i].TimeStop.SetDateTime(nNowYear, nNowMonth, nNowDay, 
				g_pStTimeCtrl[i].TimeStop.GetHour(), g_pStTimeCtrl[i].TimeStop.GetMinute(), 
				g_pStTimeCtrl[i].TimeStop.GetSecond());	

			//如果当前时间处于第二天，且处于一个月的第一天（还要注意是否是一年的第一天）
			//不能简单地日期减1，时间会出错
			if( ((nNowYear%4==0)&&(nNowYear%100!=0)) || (nNowYear%400==0) )
			{//闰年，2月有29天
				switch(nNowMonth)
				{
				case 2:
				case 4:
				case 6:
				case 8:
				case 9:
				case 11:
				case 1:
					{
						if(nNowDay == 1)
						{
							if(nNowMonth == 1)
							{
								nChangeYear --;
								nChangeMonth = 12;
							}
							else
							{
								nChangeMonth --;
							}
							nChangeDay = 31;
						}
						else
						{
							nChangeDay --;
						}
						break;
					}
				case 5:
				case 7:
				case 10:
				case 12:
					{
						if(nNowDay == 1)
						{
							nChangeMonth --;
							nChangeDay = 30;
						}
						else
						{
							nChangeDay --;
						}	
						break;
					}
				case 3:
					{
						if(nNowDay == 1)
						{
							nChangeMonth --;
							nChangeDay = 29;
						}
						else
						{
							nChangeDay --;
						}	
						break;						
					}
				}//switch		
			}
			else
			{//不是闰年，2月28天
				switch(nNowMonth)
				{
				case 2:
				case 4:
				case 6:
				case 8:
				case 9:
				case 11:
				case 1:
					{
						if(nNowDay == 1)
						{
							if(nNowMonth == 1)
							{
								nChangeYear --;
								nChangeMonth = 12;
							}
							else
							{
								nChangeMonth --;
							}
							nChangeDay = 31;
						}
						else
						{
							nChangeDay --;
						}
						break;
					}
				case 5:
				case 7:
				case 10:
				case 12:
					{
						if(nNowDay == 1)
						{
							nChangeMonth --;
							nChangeDay = 30;
						}
						else
						{
							nChangeDay --;
						}	
						break;
					}
				case 3:
					{
						if(nNowDay == 1)
						{
							nChangeMonth --;
							nChangeDay = 28;
						}
						else
						{
							nChangeDay --;
						}	
						break;						
					}
				}//switch		
			}//判断闰年

			g_pStTimeCtrl[i].TimeStart.SetDateTime(nChangeYear, nChangeMonth, nChangeDay, 
				g_pStTimeCtrl[i].TimeStart.GetHour(), g_pStTimeCtrl[i].TimeStart.GetMinute(), 
				g_pStTimeCtrl[i].TimeStart.GetSecond());
		}//已经跨越0点，是否处理终止时间
	}//是否跨越0点
}
//
void Time_On_OneOnOneOff(CEdit *pEdit, CString strKuNum)
{
	int nKuExeSum = 0;
	BYTE *aKuNum;
	if(strcmp(strKuNum, "全部库房") == 0)
	{
		nKuExeSum = g_byKuCtrlSum_St;
	}
	else
	{
		aKuNum = (BYTE *)malloc(g_byKuSum_SetKu * sizeof(BYTE));

		nKuExeSum = Str2Dec(strKuNum, aKuNum);		
	}	

	for(int j=0; j<nKuExeSum; j++)
	{
		if(g_bStopKuCtrl)	//退出库房监控与流程执行
		{
			g_bStopProp = TRUE;
			break;
		}

		int nKuIndex = -1;
		if(strcmp(strKuNum, "全部库房") == 0)
		{
			nKuIndex = j;
		}
		else
		{
			//查找库号所在结构体
			nKuIndex = GetKuIndex_St(aKuNum[j], FALSE);
			if(nKuIndex == -1)
			{
				return;
			}
		}

		//开关机状态
		g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(TRUE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);

		//执行库房开机
		CString str = " ";
		str.Format("库房%d: 时间到", g_pStKuCtrl[nKuIndex].byKuNum);
		TempOrTime_On_OneOnOneOff(pEdit, nKuIndex, str);

		//开关机状态
		g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(FALSE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);
	}

	if(strcmp(strKuNum, "全部库房") != 0)
	{
		free(aKuNum);
	}
}

void Time_Off_OneOnOneOff(CEdit *pEdit, CString strKuNum)
{
	int nKuExeSum = 0;
	BYTE *aKuNum;
	if(strcmp(strKuNum, "全部库房") == 0)
	{
		nKuExeSum = g_byKuCtrlSum_St;
	}
	else
	{
		aKuNum = (BYTE *)malloc(g_byKuSum_SetKu * sizeof(BYTE));
		nKuExeSum = Str2Dec(strKuNum, aKuNum);		
	}

	for(int j=0; j<nKuExeSum; j++)
	{
		if(g_bStopKuCtrl)	//退出库房监控与流程执行
		{
			g_bStopProp = TRUE;
			break;
		}

		int nKuIndex = -1;
		if(strcmp(strKuNum, "全部库房") == 0)
		{
			nKuIndex = j;
		}
		else
		{
			//查找库号所在结构体
			nKuIndex = GetKuIndex_St(aKuNum[j], FALSE);
			if(nKuIndex == -1)
			{
				return;
			}
		}

		//开关机状态
		g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(TRUE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);

		//执行库房关机
		CString str = " ";
		str.Format("库房%d: 时间到", g_pStKuCtrl[nKuIndex].byKuNum);
		TempOrTime_Off_OneOff(pEdit, nKuIndex, str);

		//开关机状态
		g_pStKuCtrl[nKuIndex].bFlagOn = GetKuStatus(FALSE, g_pStKuCtrl[nKuIndex].byKuNum, pEdit);
	}

	if(strcmp(strKuNum, "全部库房") != 0)
	{
		free(aKuNum);
	}
}
void Time_On_AllOnAllOff(CEdit *pEdit, CString strKuNum)
{
	CString str = "";
	str = "库房" + strKuNum;
	CString strSql = "";

	if(strcmp(strKuNum, "全部库房") == 0)
	{
		//150807
		if(g_nCtrlMode == 1)
		{//150807
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1 AND KuCtrlMode LIKE '时开%'";
		}
		else if(g_nCtrlMode == 2)
		{
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND WorkStatus = 1";		
		}
		else
		{
			Doing2Text("时开执行开机模式错误");
			g_bStopKuCtrl = TRUE;
			return;
		}
	}
	else
	{
		//150907
		if(g_nCtrlMode == 1)
		{
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 \
				AND WorkStatus = 1 AND KuCtrlMode LIKE '时开%' AND PartKu = 1";
		}
		else
		{
			AfxMessageBox("峰谷用电-部分库房-暂时不适用-多控制模式");
			Doing2Text("峰谷用电-部分库房-暂时不适用-多控制模式");
			g_bStopKuCtrl = TRUE;
			return;
		}
	}

	m_pRs_TimeTime = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//168.1
	if(m_pRs_TimeTime->BOF)	//全部开机150807
	{
		str += "  时间到达，执行指定开机流程";
		pEdit->SetWindowTextA(str);
		Doing2Text(str);
		Sleep(1000);
		m_pRs_TimeTime->raw_Close();	//168.2.2

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
		g_StProp.byKuNum = 0;	//那个库房触发的流程执行

		ST_AllOnAllOff(g_StProp);			
	}
	
	//150915
	g_bFlagAllYSJOff = FALSE;
}
void Time_Off_AllOnAllOff(CEdit *pEdit, CString strKuNum)
{
	CString str = "";
	str = "库房" + strKuNum;
	CString strSql = "";

	if(strcmp(strKuNum, "全部库房") == 0)
	{
		strSql = "SELECT * FROM SetKu WHERE KuStatus = 1 AND WorkStatus = 1 AND KuCtrlMode = '时开时关' AND KuWorkMode = '同开同关'";
	}
	else
	{
		//部分库房，字符串转化为数组，数组最大为库房总数
		BYTE *aKuNum = (BYTE *)malloc(g_byKuSum_SetKu * sizeof(BYTE));
		int nKuExeSum = Str2Dec(strKuNum, aKuNum);

		strSql.Format("SELECT * FROM SetKu WHERE KuStatus = 1 AND WorkStatus = 1 AND KuNum = %d", aKuNum[0]);	//查询其中一个即可

		free(aKuNum);
	}

	m_pRs_TimeTime = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//167.1
	if(m_pRs_TimeTime->BOF)	//全部关机			
	{
		str += "  已关机";
		pEdit->SetWindowTextA(str);	
		Sleep(1000);
		m_pRs_TimeTime->raw_Close();	//167.2.1
	}
	else
	{
		str += "  时间到达，执行指定关机流程";
		pEdit->SetWindowTextA(str);
		Sleep(1000);
		m_pRs_TimeTime->raw_Close();	//167.2.2

		//if(strcmp(strKuNum, "全部库房") == 0)
		//{
		//	g_StProp.strSqlProp = "SetProp";
		//	g_StProp.strKuNum = "全部库房";
		//}
		//else
		//{
		//	SelectSqlSomeKu(aSetFunc[1], pEdit, strKuNum);
		//	g_StProp.strSqlProp = "PropSomeKu";
		//	g_StProp.strKuNum = strKuNum;
		//}

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
}
void TimeOnTimeOff(CEdit *pEdit, CString strWorkMode)
{
	int nTimeCtrlStatus = GetTimeCtrlSt();
	if(nTimeCtrlStatus == -1)
	{
		pEdit->SetWindowTextA("时间控制失败");
		return;
	}

	while(!g_bStopKuCtrl)
	{
		int nNowTimeSpan=0;	//当前时间所在时间段
		nNowTimeSpan = NowTimeSpan(pEdit, strWorkMode);

		while(g_bFlagInSpan)	//执行关闭库房指令后，退出此循环
		{
			if(g_bStopKuCtrl)	//退出库房监控与流程执行
			{
				g_bStopProp = TRUE;
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

				CString strStartTimeKu = "";
				if(strcmp(strWorkMode, aKuWorkMode[2]) == 0)		//异开异关
				{
					strStartTimeKu.Format("%s < %s < %s  时间到达，分别 开启库房 %s"
						,strTimeStart, strTimeNow, strTimeStop, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);
					pEdit->SetWindowTextA(strStartTimeKu);
					Sleep(3000);

					Time_On_OneOnOneOff(pEdit, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);
				}
				else if(strcmp(strWorkMode, aKuWorkMode[0]) == 0)	//同开同关
				{
					strStartTimeKu.Format("%s < %s < %s  时间到达，同时 开启库房 %s"
						,strTimeStart, strTimeNow, strTimeStop, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);
					pEdit->SetWindowTextA(strStartTimeKu);
					Sleep(3000);

					Time_On_AllOnAllOff(pEdit, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);				
				}			
			}
			else if(g_mTimeNow < g_pStTimeCtrl[nNowTimeSpan].TimeStart)
			{
				g_bFlagInSpan = TRUE;

				CString strNormalTimeKu = "";
				strNormalTimeKu.Format("%s < %s 时间未到，不开启库房", strTimeNow,strTimeStart);
				pEdit->SetWindowTextA(strNormalTimeKu);
				Sleep(1000);
			}
			else if(g_mTimeNow > g_pStTimeCtrl[nNowTimeSpan].TimeStop)
			{
				CString strStopTimeKu = "";

				if(strcmp(strWorkMode, aKuWorkMode[2]) == 0)		//异开异关
				{
					strStopTimeKu.Format("%s > %s 时间已过，分别关闭库房 %s"
						, strTimeNow, strTimeStop, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);
					pEdit->SetWindowTextA(strStopTimeKu);
					Sleep(3000);

					Time_Off_OneOnOneOff(pEdit, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);			
				}
				else if(strcmp(strWorkMode, aKuWorkMode[0]) == 0)	//同开同关
				{
					strStopTimeKu.Format("%s > %s 时间已过，同时关闭库房 %s"
						, strTimeNow, strTimeStop, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);
					pEdit->SetWindowTextA(strStopTimeKu);
					Sleep(3000);

					Time_Off_AllOnAllOff(pEdit, g_pStTimeCtrl[nNowTimeSpan].strTimeKuNum);			
				}

				g_bFlagInSpan = FALSE;
			}//比较起止时间
		}//循环当前峰谷用电时间段内比较
	}//循环查询当前时间所在时间段

	delete [] g_pStTimeCtrl;	//释放内存，清空指针
	g_pStTimeCtrl = NULL;
}