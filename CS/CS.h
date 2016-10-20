#if !defined(AFX_CS_H__73AB1E66_6C47_4708_A725_80FCFC7329D1__INCLUDED_)
#define AFX_CS_H__73AB1E66_6C47_4708_A725_80FCFC7329D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CCSApp : public CWinApp
{
public:
	CCSApp();

	public:
	virtual BOOL InitInstance();
	int AdoInit(void);

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////限制//////////////////////////////////
//库房温度号：2
//库房分液数：2
//短信结构体：4机22库
////////////////////////////////////////////////////软件发布 ↓ ///////////////////////////////////////////////////////////////
//****公用****
#define CS_SUCCESS 15
#define KU_TEMP	 2
#define KU_YSJ	 4
#define KU_FY	 2
#define COM_TEMP 1
#define COM_MSG	 2
#define COM_ANS  5

//****模拟****
#define SIM_STATUS TRUE
#define LOGIN_EN FALSE

//****测试****
//#define SIM_STATUS FALSE
//#define LOGIN_EN FALSE

////****正式****
//#define SIM_STATUS FALSE
//#define LOGIN_EN TRUE

typedef struct CS	//冷库结构体
{
	DWORD dwNumVolumeSerial;
	CString strHDSerial;
	CString strCS_SQL;
	CString strLoginName;
	CString strLoginPwd;
	BOOL bFlagEnMusic;
	CString strCS_Name;
	CString strCS_Worker;
	CString strCS_Phone;
	CString strCS_SMS;
}CS_t;

typedef struct Prop	//开关机流程函数，线程多参数传递
{
	CString Func;		//开机流程还是关机流程
	CEdit *pEdit;		//流程执行过程中的显示控件
	BYTE byKuNum;		//那个库房号触发的流程执行
	CString strSqlProp;
	CString strKuNum;
}Prop_t;

typedef struct KuCtrl	//温度控制函数，获得多库房编号、关联各硬件、温度上下限、库房开关机状态
{
	BYTE byKuNum;
	BYTE aTempNum[KU_TEMP];		//温度计编号，默认为2个温度节点
	BYTE byTempSum;
	CString strYSJNum;			//151116
	BYTE aYSJNum[KU_YSJ];		//151029
	BYTE aFYNum[KU_FY];			//双分液
	BYTE byHQNum;
	BYTE byFJNum;
	float fTempUp;
	float fTempDn;
	float fTemp;			//当前温度	
	BOOL bFlagOn;
	BOOL bOnlyFJ;			//只风机
	BOOL bDelay3;			//延时关闭
}KuCtrl_t;

typedef struct YSJ
{
	BYTE byYSJ;
	BYTE byDW;
	BYTE byXQ;
	BYTE byPQ;
	BYTE byAF;
	BYTE byAB;
	BYTE bySB;
	BOOL bNC;
	BYTE byPri;
}YSJ_t;

typedef struct KuYSJ
{
	BYTE KuNum;
	BYTE YSJNum;
}KuYSJ_t;

typedef struct TimeCtrl	//时间控制结构体
{
	COleDateTime TimeStart;
	COleDateTime TimeStop;
	CString strTimeKuNum;
}TimeCtrl_t;

typedef struct CXMsg	//短信结构体
{
	BOOL YSJ[KU_YSJ];
	float Ku[22];
}CXMsg_t;

typedef struct AnS		//氨检结构体
{
	BYTE byNum;			//巡检路数
	BYTE byStatusNum;	//状态码
	CString strStatus;	//状态解读
	BYTE byCon;			//浓度
}AnS_t;

typedef struct TempSor	//温度探头
{
	BYTE byTempIndex;		//探头编号	
	BYTE byKuNum;			//所属库房
	BOOL bKuStatus;			//库房在用:TRUE
	float fKuTempDn;		//库房温度下限	//150908自动判断温度下限与下限报警是否太敏感
	float fTempCom;			//传感器温度
	float fTempJZ;			//校准值
	float fTemp;			//温度最终值
	float fTempDnS;			//下限报警值
	BOOL bTempDnS;			//下限报警标志，报警：TRUE，只有在FALSE时才能报警
	COleDateTime mTimeLast;	//上次获取时间值
	BOOL bTimeS;			//获取时间超过5分钟报警：TRUE
}TempSor_t;

typedef struct Cid			//下位机节点
{
	int nID;				//节点编号（1~N）	
	BYTE byDQ;				//电气（0为非电气；1为电气1；2为电气2）
	BOOL bSnd;				//已发送:TRUE
	BOOL bRcv;				//已接收:TRUE
}Cid_t;
/************************************************************************************************************/
char HexChar(char c);
int Str2Hex(CString str, char* data);
int CharDec(char c);
int Str2Dec(CString all, BYTE* data);
int Time2Dec(CString all, BYTE* data);
void Doing2Text(CString strSQLText);
void Sql2Excel(BOOL bVisible);
void KillProcessFromName(LPCTSTR name);
int Priority2Dec(CString all, BYTE* data);
void ResetGlobalCtrlValue();


#define TIMER_ID_TEMP 1			//串口温度
#define TIMER_MS_TEMP 501
#define TIMER_ID_ANS 2			//氨检自动
#define TIMER_MS_ANS 5499
#define TIMER_ID_TIME_CTRL 3	//控制界面时间显示
#define TIMER_MS_TIME_CTRL 1000

#define NumOnItem	    10
#define NumOnDir		5
#define NumSetFunc		2
#define NumCidItem		11
#define NumKuCtrlMode	3
#define NumCtrlTimeTemp	4
#define NumKuWorkMode	3

#define NumSetPropZu	3
#define NumKuTempSor	2

#define MSG_EX_NO		0
#define MSG_EX_WATER	1
#define MSG_EX_YSJ		2
#define MSG_EX_TIMES	3
#define MSG_EX_LOST		4
#define MSG_EX_ANS		5
#define MSG_EX_CHSH		6
#define MSG_AUTHOR_NO	7
#define MSG_AUTHOR_30	8
#define MSG_AUTHOR_FR	9
#define MSG_EX_T_RDS	10
#define MSG_EX_T_LOST	11
#define MSG_EX_T_DNS	12
#define MSG_PROP_ON		13
#define MSG_PROP_OFF	14
#define MSG_EX_WATCH	15
#define MSG_EX_ELECT	16

#endif
