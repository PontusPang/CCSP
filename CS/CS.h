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
/////////////////////////////����//////////////////////////////////
//�ⷿ�¶Ⱥţ�2
//�ⷿ��Һ����2
//���Žṹ�壺4��22��
////////////////////////////////////////////////////������� �� ///////////////////////////////////////////////////////////////
//****����****
#define CS_SUCCESS 15
#define KU_TEMP	 2
#define KU_YSJ	 4
#define KU_FY	 2
#define COM_TEMP 1
#define COM_MSG	 2
#define COM_ANS  5

//****ģ��****
#define SIM_STATUS TRUE
#define LOGIN_EN FALSE

//****����****
//#define SIM_STATUS FALSE
//#define LOGIN_EN FALSE

////****��ʽ****
//#define SIM_STATUS FALSE
//#define LOGIN_EN TRUE

typedef struct CS	//���ṹ��
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

typedef struct Prop	//���ػ����̺������̶߳��������
{
	CString Func;		//�������̻��ǹػ�����
	CEdit *pEdit;		//����ִ�й����е���ʾ�ؼ�
	BYTE byKuNum;		//�Ǹ��ⷿ�Ŵ���������ִ��
	CString strSqlProp;
	CString strKuNum;
}Prop_t;

typedef struct KuCtrl	//�¶ȿ��ƺ�������ö�ⷿ��š�������Ӳ�����¶������ޡ��ⷿ���ػ�״̬
{
	BYTE byKuNum;
	BYTE aTempNum[KU_TEMP];		//�¶ȼƱ�ţ�Ĭ��Ϊ2���¶Ƚڵ�
	BYTE byTempSum;
	CString strYSJNum;			//151116
	BYTE aYSJNum[KU_YSJ];		//151029
	BYTE aFYNum[KU_FY];			//˫��Һ
	BYTE byHQNum;
	BYTE byFJNum;
	float fTempUp;
	float fTempDn;
	float fTemp;			//��ǰ�¶�	
	BOOL bFlagOn;
	BOOL bOnlyFJ;			//ֻ���
	BOOL bDelay3;			//��ʱ�ر�
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

typedef struct TimeCtrl	//ʱ����ƽṹ��
{
	COleDateTime TimeStart;
	COleDateTime TimeStop;
	CString strTimeKuNum;
}TimeCtrl_t;

typedef struct CXMsg	//���Žṹ��
{
	BOOL YSJ[KU_YSJ];
	float Ku[22];
}CXMsg_t;

typedef struct AnS		//����ṹ��
{
	BYTE byNum;			//Ѳ��·��
	BYTE byStatusNum;	//״̬��
	CString strStatus;	//״̬���
	BYTE byCon;			//Ũ��
}AnS_t;

typedef struct TempSor	//�¶�̽ͷ
{
	BYTE byTempIndex;		//̽ͷ���	
	BYTE byKuNum;			//�����ⷿ
	BOOL bKuStatus;			//�ⷿ����:TRUE
	float fKuTempDn;		//�ⷿ�¶�����	//150908�Զ��ж��¶����������ޱ����Ƿ�̫����
	float fTempCom;			//�������¶�
	float fTempJZ;			//У׼ֵ
	float fTemp;			//�¶�����ֵ
	float fTempDnS;			//���ޱ���ֵ
	BOOL bTempDnS;			//���ޱ�����־��������TRUE��ֻ����FALSEʱ���ܱ���
	COleDateTime mTimeLast;	//�ϴλ�ȡʱ��ֵ
	BOOL bTimeS;			//��ȡʱ�䳬��5���ӱ�����TRUE
}TempSor_t;

typedef struct Cid			//��λ���ڵ�
{
	int nID;				//�ڵ��ţ�1~N��	
	BYTE byDQ;				//������0Ϊ�ǵ�����1Ϊ����1��2Ϊ����2��
	BOOL bSnd;				//�ѷ���:TRUE
	BOOL bRcv;				//�ѽ���:TRUE
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


#define TIMER_ID_TEMP 1			//�����¶�
#define TIMER_MS_TEMP 501
#define TIMER_ID_ANS 2			//�����Զ�
#define TIMER_MS_ANS 5499
#define TIMER_ID_TIME_CTRL 3	//���ƽ���ʱ����ʾ
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
