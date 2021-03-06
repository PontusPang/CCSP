#include "stdafx.h"
#include "CS.h"
#include "LOGINDlg.h"
#include "CANDlg.h"
#include "SETDlg.h"
#include "DISPDlg.h"
#include "AdvCanThread.h"
#include "ThreadProp.h"
#include "ThreadProp_TimeTime.h"
#include "ThreadProp_TempTemp.h"
#include <ATLComTime.h>
#include "Security.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString aOnItem[NumOnItem];
extern CString aOnDir[NumOnDir];
extern CString aSetFunc[NumSetFunc];
extern CString aCidItem[NumCidItem];

extern CWinThread* p_ThreadWrite;
extern CWinThread* p_ThreadRead;
extern HANDLE g_hThreadRead;
extern BOOL	g_bExitReadThreadFlag;
extern BOOL	g_bExitWriteThreadFlag;
extern DWORD	g_dwCobId;		//send
extern BYTE		g_SendData[8];
extern DWORD	g_RcvCobId;
extern DWORD	g_RcvLength;
extern BYTE		g_RcvData[8];
extern BOOL		g_bExeTrue;			//执行成功标志
extern BOOL		g_bCxTrue;			//查询成功标志

extern BYTE g_byCAN12ID;			//下位机ID>100，与CAN2相连	151201
extern BOOL g_bCAN2_InUse;			//是否使用CAN2
extern HANDLE	g_hDevice_CAN1;
extern BOOL		g_bOpened_CAN1;
extern HANDLE	g_hDevice_CAN2;
extern BOOL		g_bOpened_CAN2;

extern _ConnectionPtr m_pCon;	//ADO连接对象
extern _RecordsetPtr m_pRs;		//ADO记录集对象
extern _RecordsetPtr m_pRs_PropList;
extern _RecordsetPtr m_pRs_CtrlHand;
extern _RecordsetPtr m_pRs_Refer;	//Refer

extern HANDLE g_hPropING;
extern HANDLE g_hKuCtrl;
extern HANDLE g_hIDRefer;
extern HANDLE g_hMsg;
extern HANDLE g_hTimerTime;
extern HANDLE g_hDelay3;
extern HANDLE g_hTimer2;
extern HANDLE g_hWatch;
extern HANDLE g_hKuYSJ;

extern CCANDlg *g_pCanDlg;
extern CSetDlgFGYSJ *pSetDlgFGYSJ;

extern int g_nCtrlMode;

extern BOOL g_bStopProp;
extern BOOL g_bStopKuCtrl;
extern BOOL g_bUrgentStop;
extern BOOL g_bUrgentING;
extern BOOL g_bOneKeyStop;
extern BOOL g_bOneKeyStart;
extern BOOL g_bStopKuYSJ;
extern BOOL g_bFlagKuCtrlING_Wait;

extern Prop_t g_StProp;
extern KuCtrl_t *g_pStKuCtrl;
extern AnS_t *g_pStAnS;
extern TempSor_t *g_pStTempSor;
extern Cid_t *g_pStCid;
extern YSJ_t *g_pStYSJ;

extern CS_t *g_pStCS;
extern int g_nCS_Index;

extern BOOL g_bFlagSimCAN;
extern BOOL g_bFlagSimTEMP;
extern BOOL g_bFlagOnlyOnOff;

extern BOOL g_bWaitRef;		//ID遍历线程暂停中
extern BOOL g_bStopRef;		//停止ID遍历-CanDlg

extern BYTE g_byKuOnMax_SetKu;
extern BYTE g_byKuSum_SetKu;		//库房总数
extern BYTE g_byTempSum_SetKu;		//温度节点总数
extern BYTE g_byYSJSum_SetKu;		//压缩机总数
extern BYTE g_byCidSum_SetKu;

extern COleDateTime g_mTimeNow;
extern BOOL g_bSysCanRun;
extern BOOL g_bFlag8_10;

extern BOOL g_bEnKuYSJ;

//COM_TEMP
//波特率2400,无奇偶校验,8数据位1停止位
BOOL g_bFlagTempReferING = FALSE;
unsigned char g_aTempComRcvData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //68,A1,01,00,13,07,00,96--19.7℃
int g_nTempComRcvCount = 0;
BOOL g_bTempComRcvFlag = FALSE;
char g_aTempComSendData[8]={(char)0x42,(char)0xA1,(char)0x01,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x45};
BYTE g_byTempComErrCount = 0;	//错误计数
BOOL g_bTempComErrFlag = FALSE;	//若出错则置1
//COME_MSG
//波特率9600,无奇偶校验,8数据位1停止位
extern int g_nMsgEx;				//短信
extern CXMsg_t g_StCXMsg;
CString g_strMsgRcv = "";
BOOL g_bPhonePSS = FALSE;
//COMANS
//波特率9600,无奇偶校验,8数据位1停止位
extern char g_aAnsComSendData[8];
extern BYTE g_byAnsSum_SetKu;
int g_nAnsComRcvCount = 0;
extern CString g_strAnsRcvUp3;
BYTE *g_aAnsComRcvData = NULL;

//CFile g_mFileWatch;

// CCANDlg 对话框
IMPLEMENT_DYNAMIC(CCANDlg, CDialog)

CCANDlg::CCANDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCANDlg::IDD, pParent)
	, m_strComRcvData(_T(""))
	, m_strProp(_T(""))
	, m_byCidCtrlID(0)
	, m_byCidCtrlItemNum(0)
	, m_byTestTO(0)
	, m_byTempSimZH(0)
	, m_byTempSimX(0)
	, m_byCidCtrlDQ(0)
	, m_fTempUpCtrl(0)
	, m_fTempDnCtrl(0)
{
	m_bSerialPortOpened_TEMP = FALSE;
	m_bSerialPortOpened_MSG = FALSE;
}

CCANDlg::~CCANDlg()
{
}

void CCANDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RCVMSG, m_strComRcvData);
	DDX_Text(pDX, IDC_EDIT_PROP, m_strProp);
	DDX_Control(pDX, IDC_EDIT_PROP, m_ctrlProp);
	DDX_Text(pDX, IDC_EDIT_CID_ID_CTRL, m_byCidCtrlID);
	DDX_Text(pDX, IDC_EDIT_CID_NUM_CTRL, m_byCidCtrlItemNum);
	DDX_Text(pDX, IDC_EDIT_TEST_TO_CTRL, m_byTestTO);
	DDX_Text(pDX, IDC_EDIT_TEMP_SIM_ZH, m_byTempSimZH);
	DDX_Text(pDX, IDC_EDIT_TEMP_SIM_X, m_byTempSimX);
	DDX_Text(pDX, IDC_EDIT_CID_DQ_CTRL, m_byCidCtrlDQ);
	DDV_MinMaxByte(pDX, m_byTempSimZH, 0, 14);
	DDV_MinMaxByte(pDX, m_byTempSimX, 0, 14);
	DDX_Text(pDX, IDC_EDIT_TEMP_UP_CTRL, m_fTempUpCtrl);
	DDX_Text(pDX, IDC_EDIT_TEMP_DN_CTRL, m_fTempDnCtrl);
	DDX_Control(pDX, IDC_OCX_URGENT, m_player);
	DDX_Control(pDX, IDC_LIST_ANS_CTRL, m_AnsList);
	DDX_Control(pDX, IDC_EDIT_RCVMSG, m_cComRcv);
}


BEGIN_MESSAGE_MAP(CCANDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CCANDlg::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_START_ON, &CCANDlg::OnBnClickedButtonStartOn)
	ON_WM_TIMER()
	ON_MESSAGE(WM_COMM_RXCHAR, OnComm)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CCANDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CCANDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_TEMP_TEST, &CCANDlg::OnBnClickedButtonTempTest)
	ON_BN_CLICKED(IDC_BUTTON_OPEN2, &CCANDlg::OnBnClickedButtonOpen2)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE2, &CCANDlg::OnBnClickedButtonClose2)
	ON_BN_CLICKED(IDC_BUTTON_SEND2, &CCANDlg::OnBnClickedButtonSend2)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ON, &CCANDlg::OnBnClickedButtonStopOn)
	ON_BN_CLICKED(IDC_BUTTON_START_TEMP_CTRL, &CCANDlg::OnBnClickedButtonStartTempCtrl)
	ON_BN_CLICKED(IDC_BUTTON_STOP_TEMP_CTRL, &CCANDlg::OnBnClickedButtonStopTempCtrl)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_RST_KU_PROP, &CCANDlg::OnBnClickedButtonRstKuProp)
	ON_BN_CLICKED(IDC_BUTTON_URGENT_STOP, &CCANDlg::OnBnClickedButtonUrgentStop)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CID_CTRL, &CCANDlg::OnNMDblclkListCidCtrl)
	ON_BN_CLICKED(IDC_BUTTON_REFER_ALL_CTRL, &CCANDlg::OnBnClickedButtonReferAllCtrl)
	ON_BN_CLICKED(IDC_BUTTON_STOP_REF_CTRL, &CCANDlg::OnBnClickedButtonStopRefCtrl)
	ON_BN_CLICKED(IDC_BUTTON_TEST_OPEN_CTRL, &CCANDlg::OnBnClickedButtonTestOpenCtrl)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CLOSE_CTRL, &CCANDlg::OnBnClickedButtonTestCloseCtrl)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CX_CTRL, &CCANDlg::OnBnClickedButtonTestCxCtrl)
	ON_BN_CLICKED(IDC_BUTTON_START_ONEKEY, &CCANDlg::OnBnClickedButtonStartOnekey)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ONEKEY, &CCANDlg::OnBnClickedButtonStopOnekey)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TEMP, &CCANDlg::OnNMDblclkListTemp)
	ON_BN_CLICKED(IDC_BUTTON_TEST_TO_CTRL, &CCANDlg::OnBnClickedButtonTestToCtrl)
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_BUTTON_RUKU_CLEAR, &CCANDlg::OnBnClickedButtonRukuClear)
	ON_BN_CLICKED(IDC_BUTTON_CTRL_OPEN, &CCANDlg::OnBnClickedButtonCtrlOpen)
	ON_BN_CLICKED(IDC_BUTTON_CTRL_CLOSE, &CCANDlg::OnBnClickedButtonCtrlClose)
	ON_BN_CLICKED(IDC_CHECK_CAN_SIMULATE, &CCANDlg::OnBnClickedCheckCanSimulate)
	ON_BN_CLICKED(IDC_CHECK_TEMP_SIMULATE, &CCANDlg::OnBnClickedCheckTempSimulate)
	ON_BN_CLICKED(IDC_BUTTON_TEMP_SIM_OK, &CCANDlg::OnBnClickedButtonTempSimOk)
	ON_BN_CLICKED(IDC_BUTTON_START_OFF, &CCANDlg::OnBnClickedButtonStartOff)
	ON_BN_CLICKED(IDC_BUTTON_RESUME_TEMP_CTRL, &CCANDlg::OnBnClickedButtonResumeTempCtrl)
	ON_BN_CLICKED(IDC_BUTTON_SUSPEND_TEMP_CTRL, &CCANDlg::OnBnClickedButtonSuspendTempCtrl)
	ON_WM_PAINT()
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_TEMP, &CCANDlg::OnNMRdblclkListTemp)
	ON_BN_CLICKED(IDC_BUTTON_KU_ON_MANUAL, &CCANDlg::OnBnClickedButtonKuOnManual)
	ON_BN_CLICKED(IDC_BUTTON_KU_OFF_MANUAL, &CCANDlg::OnBnClickedButtonKuOffManual)
	ON_BN_CLICKED(IDC_BUTTON_CX_TEXT, &CCANDlg::OnBnClickedButtonCxText)
	ON_BN_CLICKED(IDC_CHECK_TEMP_UP_CTRL, &CCANDlg::OnBnClickedCheckTempUpCtrl)
	ON_BN_CLICKED(IDC_CHECK_TEMP_DN_CTRL, &CCANDlg::OnBnClickedCheckTempDnCtrl)
	ON_BN_CLICKED(IDC_BUTTON_TEMP_UP_DN_CTRL, &CCANDlg::OnBnClickedButtonTempUpDnCtrl)
	ON_BN_CLICKED(IDC_BUTTON_WMP_ST, &CCANDlg::OnBnClickedButtonWmpSt)
	ON_BN_CLICKED(IDC_BUTTON_WMP_STOP, &CCANDlg::OnBnClickedButtonWmpStop)
	ON_BN_CLICKED(IDC_BUTTON_WMP_SP, &CCANDlg::OnBnClickedButtonWmpSp)
	ON_BN_CLICKED(IDC_BUTTON_WMP_UR, &CCANDlg::OnBnClickedButtonWmpUr)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_ANS, &CCANDlg::OnBnClickedButtonOpenAns)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_ANS, &CCANDlg::OnBnClickedButtonCloseAns)
	ON_BN_CLICKED(IDC_CHECK_ON_OFF_CYCLE, &CCANDlg::OnBnClickedCheckOnOffCycle)
	ON_BN_CLICKED(IDC_CHECK_YSJ_MANUAL, &CCANDlg::OnBnClickedCheckYsjManual)
	ON_BN_CLICKED(IDC_BUTTON_YSJ_OFF_MANUAL, &CCANDlg::OnBnClickedButtonYsjOffManual)
	ON_BN_CLICKED(IDC_BUTTON_MANUAL_YSJ_STOP, &CCANDlg::OnBnClickedButtonManualYsjStop)
	ON_BN_CLICKED(IDC_BUTTON_YSJ_ON_MANUAL, &CCANDlg::OnBnClickedButtonYsjOnManual)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_QUIT, &CCANDlg::OnBnClickedButtonQuit)
END_MESSAGE_MAP()
// CCANDlg 消息处理程序
void CCANDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_ID_TEMP:
	{
		if(!m_bSerialPortOpened_TEMP) 
		{
			MessageBox("未开启测温串口");
			return;
		}

		int nTempCount = g_aTempComSendData[1] - 0xffffffA1;
		
		//150813
		if(strcmp(g_pStCS[g_nCS_Index].strLoginName, "栖霞源鑫") == 0)
		{
			if(nTempCount < 8)
			{
				nTempCount = nTempCount * 2; 
			}
			else
			{
				nTempCount = (nTempCount - 8)*2+1;
			}
		}

		//清除选中显示
		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_TEMP);
		int nCount = pList->GetItemCount();
		for(int i=0; i<nCount; i++)
		{
			pList->SetItemState(i, NULL, LVIS_FOCUSED|LVIS_SELECTED);
		}

		if(g_bTempComRcvFlag && !g_bTempComErrFlag)
		{				
			//150813
			//nTempCount = g_aTempComSendData[1] - 0xffffffA1;
			
			g_pStTempSor[nTempCount].fTempCom = (float)g_aTempComRcvData[4] + (float)0.1*g_aTempComRcvData[5];

			if(g_aTempComRcvData[6] == 1)
			{
				g_pStTempSor[nTempCount].fTempCom  *= -1;
			}

			//温度较准
			g_pStTempSor[nTempCount].fTemp = g_pStTempSor[nTempCount].fTempCom + g_pStTempSor[nTempCount].fTempJZ;
			g_pStTempSor[nTempCount].mTimeLast = g_mTimeNow;

			//温度保护
			if(g_pStTempSor[nTempCount].fTemp < g_pStTempSor[nTempCount].fTempDnS) 
			{
				if(!g_pStTempSor[nTempCount].bTempDnS)
				{
					g_pStTempSor[nTempCount].bTempDnS = TRUE;
					
					if( GetDlgItem(IDC_BUTTON_URGENT_STOP)->IsWindowEnabled() )
					{
						Doing2Text("温度超限 自动-紧急关机");
						PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_URGENT_STOP, BN_CLICKED), NULL);
					}

					//发送温度异常短信
					if(g_hMsg == NULL)
					{
						g_nMsgEx = MSG_EX_T_DNS;	//温度异常

						CWinThread* pThread;
						pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
						g_hMsg = pThread->m_hThread;
					}
					else
					{
						Doing2Text("温度异常，短信被占用");
						g_pStTempSor[nTempCount].bTempDnS = FALSE;
					}

					CString strTempDnS = "";
					strTempDnS.Format("%0.1f<%0.1f, 温度探头%d下限报警", 
						g_pStTempSor[nTempCount].fTemp, g_pStTempSor[nTempCount].fTempDnS, g_pStTempSor[nTempCount].byTempIndex);
					Doing2Text(strTempDnS);				
				}
			}
			else
			{
				g_pStTempSor[nTempCount].bTempDnS = FALSE;
			}
			
			//短信温度2-1
			if(strcmp(g_pStCS[g_nCS_Index].strLoginName, "栖霞源鑫") != 0)
			{
				if(nTempCount/2 < g_byKuSum_SetKu)
					g_StCXMsg.Ku[nTempCount/2] = g_pStTempSor[nTempCount].fTemp;	//号码相邻，温度1-2，如cx[0]=f[0]或f[1]
			}
			else
			{	//此为栖霞源鑫
				if(nTempCount < g_byKuSum_SetKu)
					g_StCXMsg.Ku[nTempCount] = g_pStTempSor[nTempCount].fTemp;	//库1:1-9
			}
			
			TempDisp(nTempCount, g_pStTempSor[nTempCount].fTemp);

			if(g_bFlagTempReferING)//进入下一个温度测试
			{
				g_aTempComSendData[1] += 0x01;
				
				if(g_aTempComSendData[1] >= (char)0xffffffA1+g_byTempSum_SetKu)
				{
					g_aTempComSendData[1] = (char)0xA1;
				}
				m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
				g_bTempComRcvFlag = FALSE;
				g_bTempComErrFlag = FALSE;	
				g_byTempComErrCount = 0;
			}
			else
			{
				KillTimer(TIMER_ID_TEMP);
			}
		}
		else
		{
			g_byTempComErrCount ++;

			if(g_bFlagSimTEMP)
			{
				//4-1 模拟温度
				if(g_byTempComErrCount == 1)
				{
					g_bTempComRcvFlag = TRUE;
					g_bTempComErrFlag = FALSE;		
				}
				//模拟温度			
			}

			if(g_byTempComErrCount > 3)
			{						
				if(g_byTempComErrCount == 3)
				{
					CString strTempErr = "";

					m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
				}
	
				if(g_byTempComErrCount > 6)
				{
					//CString strTempErr = "";
					//strTempErr.Format("Temp %d ERROR, Jump",nTempCount+1);
					//Doing2Text(strTempErr);

					g_bTempComRcvFlag = FALSE;
					g_bTempComErrFlag = TRUE;

					if(g_bFlagTempReferING)//进入下一个温度测试
					{
						g_aTempComSendData[1] += 0x01;
						
						if(g_aTempComSendData[1] == 0xffffffA1+g_byTempSum_SetKu)
						{
							g_aTempComSendData[1] = (char)0xA1;
						}
						m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
						g_bTempComRcvFlag = FALSE;
						g_bTempComErrFlag = FALSE;	
						g_byTempComErrCount = 0;

						KillTimer(TIMER_ID_TEMP);
						Sleep(1000);
						SetTimer(TIMER_ID_TEMP, TIMER_MS_TEMP, NULL);
					}
				}
			}
		}

		break;
	}
	case TIMER_ID_TIME_CTRL:
	{
		//获得时间
		g_mTimeNow = COleDateTime::GetCurrentTime();
		CString strTime = "";
		strTime.Format("%d-%02d-%02d %02d:%02d:%02d", 
			g_mTimeNow.GetYear(), g_mTimeNow.GetMonth(), g_mTimeNow.GetDay(), 
			g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), g_mTimeNow.GetSecond());

		//更新时间显示
		CWnd *pWnd = g_pCanDlg->GetDlgItem(IDC_STATIC_TIME_CTRL);		//CDC是在CWnd中使用的设备上下文
		pWnd->InvalidateRect(NULL);							//清屏、刷屏					
		pWnd->UpdateWindow();
		CDC *pDC = pWnd->GetDC();
		CFont fon;
		fon.CreateFontA(45,0,0,0,FW_BOLD,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Arial");
		pDC->SelectObject(&fon);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
		pDC->TextOutA(0,0,strTime);
		pWnd->ReleaseDC(pDC);

		//更新峰谷用电时间显示
		if(pSetDlgFGYSJ != NULL)
		{
			CEdit* pEdit = (CEdit*)pSetDlgFGYSJ->GetDlgItem(IDC_EDIT_CUR_TIME);
			pEdit->SetWindowTextA(strTime);			
		}
		
		if((g_mTimeNow.GetHour() == 8) && (g_mTimeNow.GetMinute() == 10) && (g_mTimeNow.GetSecond() == 0))
		{		
			g_bFlag8_10 = TRUE;

			Doing2Text("每天8:10定时备份运行记录并清空");
			Sql2Excel(FALSE);
			CString strText = "DELETE Text";
			m_pCon->Execute(_bstr_t(strText),NULL,adCmdText);
		}
		else if((g_mTimeNow.GetHour() == 14) && (g_mTimeNow.GetMinute() == 10) && (g_mTimeNow.GetSecond() == 0))
		{		
			Doing2Text("每天14:10定时备份运行记录并清空");
			Sql2Excel(FALSE);
			CString strText = "DELETE Text";
			m_pCon->Execute(_bstr_t(strText),NULL,adCmdText);
		}
		else if((g_mTimeNow.GetHour() == 20) && (g_mTimeNow.GetMinute() == 10) && (g_mTimeNow.GetSecond() == 0))
		{		
			Doing2Text("每天20:10定时备份运行记录并清空");
			Sql2Excel(FALSE);
			CString strText = "DELETE Text";
			m_pCon->Execute(_bstr_t(strText),NULL,adCmdText);
		}

		if( (!g_bStopKuCtrl) && (g_hTimerTime == NULL) )
		{
			CWinThread* pThread;
			pThread = AfxBeginThread(ThreadProc_TIMER_Time, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			g_hTimerTime = pThread->m_hThread;		
		}

		break;
	}
	case TIMER_ID_ANS:
	{
		if(m_bSerialPortOpened_ANS)
		{
			m_SerialPort_ANS.WriteToPort(g_aAnsComSendData,8);
		}
	
		break;
	}
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CCANDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//全屏
	ShowWindow(SW_SHOWMAXIMIZED);

	//漏出下面任务栏
	/*if(SIM_STATUS  || !LOGIN_EN)	//仅供测试
	{
		int nCx = GetSystemMetrics(SM_CXFULLSCREEN);
		int nCy = GetSystemMetrics(SM_CYFULLSCREEN);
		CRect rt;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);
		nCy = rt.bottom;
		MoveWindow(0, 0, nCx, nCy);	
	}*/

	g_pCanDlg = this;
	
	g_mTimeNow = COleDateTime::GetCurrentTime();	//防止刚生成软件，应用到致错
	SetTimer(TIMER_ID_TIME_CTRL, TIMER_MS_TIME_CTRL, NULL);

	m_playerCtrl = static_cast<CWMPControls>(m_player.get_controls());
	m_playerSet = m_player.get_settings();

	GetDlgItem(IDC_BUTTON_WMP_ST)->EnableWindow(g_pStCS[g_nCS_Index].bFlagEnMusic);
	GetDlgItem(IDC_BUTTON_WMP_SP)->EnableWindow(g_pStCS[g_nCS_Index].bFlagEnMusic);
	GetDlgItem(IDC_BUTTON_WMP_UR)->EnableWindow(g_pStCS[g_nCS_Index].bFlagEnMusic);
	//GetDlgItem(IDC_BUTTON_WMP_STOP)->EnableWindow(g_pStCS[g_nCS_Index].bFlagEnMusic);

	//模拟初始值
	CButton *pCheckCanSim = (CButton *)GetDlgItem(IDC_CHECK_CAN_SIMULATE);
	CButton *pCheckTempSim = (CButton *)GetDlgItem(IDC_CHECK_TEMP_SIMULATE);
	pCheckCanSim->SetCheck(SIM_STATUS);
	pCheckTempSim->SetCheck(SIM_STATUS);
	g_bFlagSimCAN = pCheckCanSim->GetCheck();
	g_bFlagSimTEMP = pCheckTempSim->GetCheck();
	CButton *pCheckTempSimSign = (CButton *)GetDlgItem(IDC_CHECK_TEMP_SIM_SIGN);
	CEdit *pEditTempSimZh = (CEdit *)GetDlgItem(IDC_EDIT_TEMP_SIM_ZH);
	CEdit *pEditTempSimX = (CEdit *)GetDlgItem(IDC_EDIT_TEMP_SIM_X);
	CStatic *pStaticTempSimX = (CStatic *)GetDlgItem(IDC_STATIC_TEMP_SIM_X);
	CStatic *pStaticTempSimL = (CStatic *)GetDlgItem(IDC_STATIC_TEMP_SIM_LOGO);
	CButton *pTempSimOK = (CButton *)GetDlgItem(IDC_BUTTON_TEMP_SIM_OK);

	pCheckTempSimSign->EnableWindow(g_bFlagSimTEMP);
	pEditTempSimZh->EnableWindow(g_bFlagSimTEMP);
	pEditTempSimX->EnableWindow(g_bFlagSimTEMP);
	pStaticTempSimX->EnableWindow(g_bFlagSimTEMP);
	pStaticTempSimL->EnableWindow(g_bFlagSimTEMP);
	pTempSimOK->EnableWindow(g_bFlagSimTEMP);

	TempListInit();
	CidCtrlInit();
	KuCtrlDispInit();
	YSJCtrlDispInit();
	UpdateRuKuComobox();
	UpdateKuManual();
	AnsListInit();

	CBitmap m_bmp;
	m_bmp.LoadBitmapA(IDB_BITMAP_URGENTBTN);
	HBITMAP hBitmap = (HBITMAP)m_bmp.Detach();
	((CButton*)GetDlgItem(IDC_BUTTON_URGENT_STOP))->ModifyStyle(0, BS_BITMAP);
	((CButton*)GetDlgItem(IDC_BUTTON_URGENT_STOP))->SetBitmap(hBitmap);

	CBitmap m_bmp1;
	m_bmp1.LoadBitmapA(IDB_BITMAP_ONEKEYSTOP);
	HBITMAP hBitmap1 = (HBITMAP)m_bmp1.Detach();
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_ONEKEY))->ModifyStyle(0, BS_BITMAP);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_ONEKEY))->SetBitmap(hBitmap1);

	CBitmap m_bmp2;
	m_bmp2.LoadBitmapA(IDB_BITMAP_STOPKUCTRL);
	HBITMAP hBitmap2 = (HBITMAP)m_bmp2.Detach();
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL))->ModifyStyle(0, BS_BITMAP);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL))->SetBitmap(hBitmap2);

	CBitmap m_bmp4;
	m_bmp4.LoadBitmapA(IDB_BITMAP_STARTKUCTRL);
	HBITMAP hBitmap4 = (HBITMAP)m_bmp4.Detach();
	((CButton*)GetDlgItem(IDC_BUTTON_START_TEMP_CTRL))->ModifyStyle(0, BS_BITMAP);
	((CButton*)GetDlgItem(IDC_BUTTON_START_TEMP_CTRL))->SetBitmap(hBitmap4);

	CBitmap m_bmp5;
	m_bmp5.LoadBitmapA(IDB_BITMAP_QUIT);
	HBITMAP hBitmap5 = (HBITMAP)m_bmp5.Detach();
	((CButton*)GetDlgItem(IDC_BUTTON_QUIT))->ModifyStyle(0, BS_BITMAP);
	((CButton*)GetDlgItem(IDC_BUTTON_QUIT))->SetBitmap(hBitmap5);

	OnBnClickedCheckTempUpCtrl();
	OnBnClickedCheckTempDnCtrl();

	//OnBnClickedButtonOpen();
	OnBnClickedButtonOpen2();

	PropListInit();

	SelectPropUrgent();

	m_editFont.CreatePointFont(180, "微软简隶书");
	m_ctrlProp.SetFont(&m_editFont); // 设置新字体

	//g_mFileWatch.Open(_T("D:\\CCSP\\watch.txt"), CFile::modeCreate | CFile::modeReadWrite);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
HBRUSH CCANDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	//if(nCtlColor == CTLCOLOR_EDIT)	//设置全部编辑框
	if(pWnd->GetDlgCtrlID() == IDC_EDIT_PROP)
	{
		pDC->SetTextColor(RGB(0,0,255));
		//pDC->SetBkColor(RGB(199,237,204));
	}
	return hbr;
}
void CCANDlg::OnDestroy()
{
	CDialog::OnDestroy();
		
	/*if(g_mFileWatch.m_hFile!=CFile::hFileNull)
	{
		g_mFileWatch.Close();
	}*/

	g_pCanDlg = NULL;
}


void CCANDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: 在此处添加消息处理程序代码
	/*
	//逄淑松
	CWnd *pWnd = GetDlgItem(IDC_STATIC_PSS);		//CDC是在CWnd中使用的设备上下文
	pWnd->InvalidateRect(NULL);							//清屏、刷屏					
	pWnd->UpdateWindow();
	CDC *pDC = pWnd->GetDC();
	CFont fon;
	fon.CreateFontA(25,0,0,0,FW_BOLD,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Arial");
	pDC->SelectObject(&fon);
	pDC->SetTextColor(RGB(0,255,0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOutA(0,7,"技术咨询: 逄淑松");
	pDC->TextOutA(0,32, "联系电话: 185-6066-8933");
	pDC->TextOutA(0,57, "电子邮箱: qdtfst@qq.com");
	pWnd->ReleaseDC(pDC);								//释放DC，注意是当前控件的DC

	//冷库
	CWnd *pWndCS = GetDlgItem(IDC_STATIC_CS);		//CDC是在CWnd中使用的设备上下文
	pWndCS->InvalidateRect(NULL);							//清屏、刷屏					
	pWndCS->UpdateWindow();
	CDC *pDCCS = pWndCS->GetDC();
	CFont fonCS;
	fonCS.CreateFontA(60,0,0,0,FW_BOLD,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"楷体_GB2312");
	pDCCS->SelectObject(&fonCS);
	pDCCS->SetTextColor(RGB(255,0,0));
	pDCCS->SetBkMode(TRANSPARENT);
	pDCCS->TextOutA(0,0,CS_NAME);
	pWndCS->ReleaseDC(pDCCS);								//释放DC，注意是当前控件的DC

	//冷库负责人
	CWnd *pWndCS2 = GetDlgItem(IDC_STATIC_SC);		//CDC是在CWnd中使用的设备上下文
	pWndCS2->InvalidateRect(NULL);							//清屏、刷屏					
	pWndCS2->UpdateWindow();
	CDC *pDCCS2 = pWndCS2->GetDC();
	CFont fonCS2;
	fonCS2.CreateFontA(25,0,0,0,FW_BOLD,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"楷体_GB2312");
	pDCCS2->SelectObject(&fonCS2);
	pDCCS2->SetTextColor(RGB(255,0,255));
	pDCCS2->SetBkMode(TRANSPARENT);
	CString strWorker = "运作咨询: ";
	strWorker += CS_WORKER;
	CString strPhone = "联系电话: ";
	strPhone += CS_PHONE;
	pDCCS2->TextOutA(0,2,strWorker);
	pDCCS2->TextOutA(0,27,strPhone);
	pWndCS2->ReleaseDC(pDCCS);								//释放DC，注意是当前控件的DC
	*/
}

BOOL CCANDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
			break;
		case VK_ESCAPE:
			return TRUE;
			break;
		default:
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
LRESULT CCANDlg::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    UINT nFlags= CDialog::OnNcHitTest(point);   //调用基类的OnNcHitTest获得返回值  
    if(nFlags==HTCAPTION||nFlags==HTSYSMENU)	//如果鼠标消息是在 标题栏上或者系统菜单栏  我们就返回FALSE 不进行处理
    {
		//return FALSE;
    }  
    else if(nFlags==HTCLIENT)					//可以通过客户区域移动窗体但是不能通过标题栏移动 
		//return  HTCAPTION ;
		return FALSE;	//也不允许
    else
		return nFlags;

	return CDialog::OnNcHitTest(point);
}
void CCANDlg::OnBnClickedButtonQuit()
{
	if(g_hKuCtrl != NULL)
	{
		CString str = "自动控制正在进行中，退出可能导致问题\n\n      确定要强制退出程序吗？";
		int nMSGB = AfxMessageBox(str, MB_YESNO);
		if(nMSGB == IDNO)
		{
			return;
		}
		else
		{
			Doing2Text("控制过程中强制退出");
		}
	}
	
	KillTimer(TIMER_ID_TIME_CTRL);
	KillTimer(TIMER_ID_TEMP);
	KillTimer(TIMER_ID_ANS);
	//151202
	OnBnClickedButtonStopRefCtrl();
	Sleep(200);
	OnBnClickedButtonCtrlClose();
	Sleep(200);
	OnBnClickedButtonClose();
	Sleep(200);
	OnBnClickedButtonClose2();
	Sleep(200);
	OnBnClickedButtonCloseAns();
	Sleep(200);

	if(g_aAnsComRcvData != NULL)
	{
		free(g_aAnsComRcvData);
		g_aAnsComRcvData = NULL;
	}
	if(g_pStAnS != NULL)
	{
		delete [] g_pStAnS;	//释放内存，清空指针
		g_pStAnS = NULL;	
	}
	if(g_pStCid != NULL)
	{
		delete [] g_pStCid;
		g_pStCid = NULL;
	}
	if(g_pStKuCtrl != NULL)					//结构体数组
	{
		delete [] g_pStKuCtrl;
		g_pStKuCtrl = NULL;
	}
	if(g_pStCS != NULL)					//结构体数组
	{
		delete [] g_pStCS;
		g_pStCS = NULL;
	}
	if(g_pStTempSor != NULL)
	{
		delete [] g_pStTempSor;
		g_pStTempSor = NULL;
	}
	if(g_pStYSJ != NULL)
	{
		delete [] g_pStYSJ;
		g_pStYSJ = NULL;
	}
	
	PostMessage(WM_QUIT);
}

//CAN开关
void CCANDlg::OnBnClickedButtonCtrlOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_bSysCanRun)
	{
		return;
	}

	//CAN1_OpenClose();
	CAN_Open("CAN1", g_hDevice_CAN1, g_bOpened_CAN1);
	if(g_bCAN2_InUse)
		CAN_Open("CAN2", g_hDevice_CAN2, g_bOpened_CAN2);
	
	GetDlgItem(IDC_BUTTON_CTRL_OPEN)->EnableWindow(!g_bOpened_CAN1);
	GetDlgItem(IDC_BUTTON_CTRL_CLOSE)->EnableWindow(g_bOpened_CAN1);
}
void CCANDlg::OnBnClickedButtonCtrlClose()
{
	CAN_Close();

	GetDlgItem(IDC_BUTTON_CTRL_OPEN)->EnableWindow(!g_bOpened_CAN1);
	GetDlgItem(IDC_BUTTON_CTRL_CLOSE)->EnableWindow(g_bOpened_CAN1);
}
void CCANDlg::OnBnClickedButtonSet()
{
	if(g_hKuCtrl != NULL)
	{
		MessageBox("请先停止库控");
		return;
	}
	
	CSETDlg dlg;
	dlg.DoModal();	
}

void CCANDlg::OnBnClickedButtonCxText()
{
	if(g_hKuCtrl != NULL)
	{
		MessageBox("请先停止库控");
		return;
	}

	Sql2Excel(TRUE);

	//CDISPDlg dlg;
	//dlg.DoModal();
}

void CCANDlg::OnBnClickedButtonStartOn()
{
	//确认CAN打开
	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请打开控制通道");
		m_ctrlProp.SetWindowTextA("控制失败");

		Doing2Text("请打开控制通道，温度控制失败");
		return;
	}
	
	Doing2Text("手动·试行开机");

	if(g_hKuCtrl != NULL)
	{
		MessageBox("请先停止库控");
		return;
	}
	g_bStopKuCtrl = FALSE;
	g_StProp.strKuNum = "全部库房";
	g_StProp.strSqlProp = "SetProp";	
	g_StProp.Func = aSetFunc[0];
	g_StProp.pEdit = &m_ctrlProp;
	g_StProp.byKuNum = 0;	//默认，那个库房触发的流程执行

	//150827
	g_nCtrlMode = 1;
	g_bFlagKuCtrlING_Wait = FALSE;

	ST_AllOnAllOff(g_StProp);

	GetDlgItem(IDC_BUTTON_START_ON)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_OFF)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_ON)->EnableWindow(TRUE);
}
void CCANDlg::OnBnClickedButtonStartOff()
{
	//确认CAN打开
	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请打开控制通道");
		m_ctrlProp.SetWindowTextA("控制失败");

		Doing2Text("请打开控制通道，温度控制失败");
		return;
	}

	Doing2Text("手动·试行关机");

	if(g_hKuCtrl != NULL)
	{
		MessageBox("请先停止库控");
		return;
	}
	g_bStopKuCtrl = FALSE;
	g_StProp.strKuNum = "全部库房";
	g_StProp.strSqlProp = "SetProp";	
	g_StProp.Func = aSetFunc[1];
	g_StProp.pEdit = &m_ctrlProp;
	g_StProp.byKuNum = 0;	//默认，那个库房触发的流程执行

	g_bFlagKuCtrlING_Wait = FALSE;

	ST_AllOnAllOff(g_StProp);

	GetDlgItem(IDC_BUTTON_START_ON)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_OFF)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_ON)->EnableWindow(TRUE);
}

void CCANDlg::OnBnClickedButtonStopOn()
{
	// TODO: 在此添加控件通知处理程序代码
	g_bStopProp = TRUE;
	Doing2Text("手动·停止流程");

	GetDlgItem(IDC_BUTTON_START_ON)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_START_OFF)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP_ON)->EnableWindow(FALSE);
}

//串口
void CCANDlg::ST_TempRefer(void)
{
	if(m_bSerialPortOpened_TEMP && !g_bFlagTempReferING) 
	{
		m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
		SetTimer(TIMER_ID_TEMP, TIMER_MS_TEMP, NULL);
		GetDlgItem(IDC_BUTTON_TEMP_TEST)->SetWindowTextA("关测温");
		Doing2Text("开启温度遍历");
		g_bFlagTempReferING = TRUE;
	}
}
void CCANDlg::SP_TempRefer(void)
{
	if(m_bSerialPortOpened_TEMP && g_bFlagTempReferING)
	{
		KillTimer(TIMER_ID_TEMP);
		Doing2Text("关闭温度遍历");
		g_bFlagTempReferING = FALSE;
		GetDlgItem(IDC_BUTTON_TEMP_TEST)->SetWindowTextA("开测温");
	}
}
LONG CCANDlg::OnComm(WPARAM ch, LPARAM port)
{
	if(port == COM_TEMP)
	{
		unsigned char chTemp = (unsigned char)ch;

		if(chTemp == 0x68)
		{
			g_nTempComRcvCount = 0;
		}

		g_aTempComRcvData[g_nTempComRcvCount] = chTemp;
		g_nTempComRcvCount++;
		g_bTempComRcvFlag = FALSE;

		if( (chTemp == 0x96) && (g_nTempComRcvCount == 8))
		{
			//MessageBox("本次接收完成");
			g_bTempComRcvFlag = TRUE;
			g_bTempComErrFlag = FALSE;
		}
		
		CString strTemp = "";
		strTemp.Format("%02X", chTemp);	//十六进制显示
		m_strComRcvData += strTemp;
		if(strcmp(strTemp,"96") == 0)
		{
			if(!LOGIN_EN)	//仅供测试
			{
				//m_strComRcvData += "\r\n";
				CEdit *pEditRcv = (CEdit *)GetDlgItem(IDC_EDIT_RCVMSG);
				pEditRcv->SetWindowTextA(m_strComRcvData);
				pEditRcv->LineScroll(pEditRcv ->GetLineCount());			
			}
		}

		if(g_nTempComRcvCount == 8)
		{
			g_nTempComRcvCount = 0;
			m_strComRcvData.Empty();
		}		
	}
	
	if(port == COM_MSG)
	{
		unsigned char chTemp = (unsigned char)ch;
		CString strTemp = "";
		strTemp.Format("%c", chTemp);	//十六进制显示
		g_strMsgRcv += strTemp;

		if(!LOGIN_EN)	//仅供测试
		{
			SetDlgItemTextA(IDC_EDIT_RCVMSG, g_strMsgRcv);
		}

		CString strRcv = g_strMsgRcv.Right(8);
		CString strRcvPhone = g_strMsgRcv.Right(12);
		if(!g_bPhonePSS && strcmp(strRcvPhone, "8165608639F3") == 0)
		{
			g_strMsgRcv.Empty();
			g_bPhonePSS = TRUE;
		}

		if(strcmp(strRcv, "67E58BE2") == 0)
		{
			g_nMsgEx = MSG_EX_NO;	//正常查询

			if(g_hMsg == NULL)
			{
				CWinThread* pThread;
				pThread = AfxBeginThread(ThreadProc_Msg, &m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
				g_hMsg = pThread->m_hThread;
			}
			else
			{
				Doing2Text(strRcv);
				Doing2Text("查询状态，短信被占用");
			}

			g_strMsgRcv.Empty();
			g_bPhonePSS = FALSE;
		}
		else
		{
			// 仅18560668933, 电话号码后面剩余28字节，非查询
			if( g_bPhonePSS && (g_strMsgRcv.GetLength() == 28) && (strcmp(strRcv, "67E58BE2") != 0)) 
			{
				Lock_GetSMS(strRcv);

				if(!LOGIN_EN)	//仅供测试
				{
					SetDlgItemTextA(IDC_EDIT_RCVMSG, g_strMsgRcv);
				}

				g_strMsgRcv.Empty();
				g_bPhonePSS = FALSE;
			}
		}
	}

	if(port == COM_ANS)
	{
		int nAnsComRcvDn = g_byAnsSum_SetKu*4+2;
		
		unsigned char chTemp = (unsigned char)ch;
		CString strTemp = "";

		strTemp.Format("%02X", chTemp);	//十六进制显示
		m_strComRcvData += strTemp;

		if(strcmp(m_strComRcvData, g_strAnsRcvUp3) == 0)
		{
			g_nAnsComRcvCount = -1;

			m_AnsList.DeleteAllItems();
		}
		
		if( (g_aAnsComRcvData != NULL) && (g_nAnsComRcvCount < nAnsComRcvDn) && (g_nAnsComRcvCount > -1) )
		{
			g_aAnsComRcvData[g_nAnsComRcvCount] = chTemp;
		}
		
		if(g_nAnsComRcvCount == nAnsComRcvDn-1 )
		{
			for(int nAnsIndex=0; nAnsIndex<g_byAnsSum_SetKu; nAnsIndex++)
			{
				g_pStAnS[nAnsIndex].byNum = nAnsIndex+1;
				int nAnsNumIndex = nAnsIndex*4;
				g_pStAnS[nAnsIndex].byStatusNum = g_aAnsComRcvData[nAnsNumIndex+1];
				g_pStAnS[nAnsIndex].byCon = g_aAnsComRcvData[nAnsNumIndex+3];

				switch(g_pStAnS[nAnsIndex].byStatusNum)
				{
				case 0: 
				case 4:
					{
						g_pStAnS[nAnsIndex].strStatus = "正常";
						break;
					}
				case 1: 
				case 5:
					{
						g_pStAnS[nAnsIndex].strStatus = "故障";
						break;
					}
				case 2: 
				case 6:
					{
						g_pStAnS[nAnsIndex].strStatus = "报警";
						break;
					}
				default:	break;
				}

				//CString str = "";
				//str.Format("氨检巡路%d，状态%d（%s），浓度%d", 
				//	g_pStAnS[nAnsIndex].byNum, g_pStAnS[nAnsIndex].byStatusNum, 
				//	g_pStAnS[nAnsIndex].strStatus, g_pStAnS[nAnsIndex].byCon);
				//Doing2Text(str);

				CString strAnsNum = "";
				strAnsNum.Format("%d", g_pStAnS[nAnsIndex].byNum);
				m_AnsList.InsertItem( nAnsIndex, LPCTSTR( strAnsNum ), 0 );
				CString strAnsCon = "";
				strAnsCon.Format("%d", g_pStAnS[nAnsIndex].byCon);
				m_AnsList.SetItemText( nAnsIndex, 1, LPCTSTR( strAnsCon ) );
				m_AnsList.SetItemText( nAnsIndex, 2, LPCTSTR( g_pStAnS[nAnsIndex].strStatus ) );

				m_AnsList.SetItemState(nAnsIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
				Sleep(100);
			}
				
			if(!LOGIN_EN)	//仅供测试
			{
				SetDlgItemTextA(IDC_EDIT_RCVMSG, m_strComRcvData);
			}

			m_strComRcvData.Empty();
			g_nAnsComRcvCount = -1;
		}

		g_nAnsComRcvCount++;
	}

	return 0;
}

void CCANDlg::OnBnClickedButtonTempTest()	//遍历温度
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_hKuCtrl != NULL)
	{
		MessageBox("请先停止库控");
		return;
	}

	if(m_bSerialPortOpened_TEMP)
	{	
		g_bFlagTempReferING = TRUE;
		CString strTem = "";
		GetDlgItemText(IDC_BUTTON_TEMP_TEST, strTem);
		if(strTem == "开测温")
		{
			SetDlgItemTextA(IDC_BUTTON_TEMP_TEST, "关测温");
			g_bTempComRcvFlag = FALSE;
			g_bTempComErrFlag = FALSE;
			g_byTempComErrCount = 0;
			m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
			SetTimer(TIMER_ID_TEMP, TIMER_MS_TEMP, NULL);
		}
		else
		{
			g_bFlagTempReferING = FALSE;
			SetDlgItemTextA(IDC_BUTTON_TEMP_TEST, "开测温");
			KillTimer(TIMER_ID_TEMP);
		}	
	}

	Doing2Text("手动·温度遍历");
}

float CCANDlg::TempNum(int Num, CEdit *pEdit)	//查询某一个温度节点
{//该函数只能在多线程中使用
	int nTempIndex = -1;
	if(m_bSerialPortOpened_TEMP)
	{
		g_bFlagTempReferING = FALSE;

		for(int i=0; i<g_byTempSum_SetKu; i++)
		{
			if(g_pStTempSor[i].byTempIndex == Num)
			{
				nTempIndex = i;
				break;
			}
		}
		if(nTempIndex == -1)
		{
			Doing2Text("未找到温度探头编号");
			return 0.0;
		}

		g_pStTempSor[nTempIndex].fTemp = 285.5;
		g_aTempComSendData[1] = 0xffffffA1 + g_pStTempSor[nTempIndex].byTempIndex - 1;
		m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
		g_bTempComRcvFlag = FALSE;
		g_bTempComErrFlag = FALSE;
		g_byTempComErrCount = 0;
		SetTimer(TIMER_ID_TEMP, TIMER_MS_TEMP, NULL);
	}

	int n = 0;
	//由于Sleep, 该函数只能在多线程中使用
	while(g_pStTempSor[nTempIndex].fTemp == 285.5 && !g_bStopKuCtrl && (g_hKuCtrl != NULL))
	{	
		if(g_bStopKuCtrl)	//退出库房监控与流程执行
		{
			break;
		}
		n++;
		Sleep(500);
		
		if(n >= 2*10)	//10s
		{
			if(n == 2*10)
			{
				CString str = "";
				str.Format("温度%d超时，重发第1次", Num);		
				pEdit->SetWindowTextA(str);
				Doing2Text(str);
				m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
			}

			if(g_bStopKuCtrl)	//退出库房监控与流程执行
			{
				break;
			}

			if(n >= 2*20)	//20s
			{
				if(n == 2*20)
				{
					CString str = "";
					str.Format("温度%d超时，重发第2次", Num);
					pEdit->SetWindowTextA(str);
					Doing2Text(str);
					m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);				
				}

				if(g_bStopKuCtrl)	//退出库房监控与流程执行
				{
					break;
				}

				if(n > 2*30)	//30s
				{
					CString str = "";
					str.Format("温度%d超时，节点错误", Num);
					pEdit->SetWindowTextA(str);
					Doing2Text(str);
					g_pStTempSor[nTempIndex].fTemp = 285.5;
					break;
				}
			}
		}
	}	

	return g_pStTempSor[nTempIndex].fTemp;
}


void CCANDlg::TempDisp(int nTempCount, float fTemp)
{	
	//nTempCount = 实际的温度号-1
	CString strTempCountOld = "";
	strTempCountOld.Format("%d", nTempCount+1);
	CString strTempCount = "";
	CString strTemp = "";

	//150813 通用
	int nListKuIndex = nTempCount / 2;
	int nListTempColumn = nTempCount % 2;

	CListCtrl* pTempList = (CListCtrl*)GetDlgItem(IDC_LIST_TEMP);

	strTempCount.Format("%d", nTempCount+1);		
	strTemp.Format("%.1f", fTemp);
	if((fTemp < 0.0)&&(fTemp > -1.0))
	{
		strTemp.Format("%2.1f", fTemp);
	}
	
	if(nListTempColumn == 0)
	{
		pTempList->SetItemText( nListKuIndex, 1, LPCTSTR( strTemp ) );
	}
	else if(nListTempColumn == 1)
	{
		pTempList->SetItemText( nListKuIndex, 2, LPCTSTR( strTemp ) );
	}

	pTempList->SetItemState(nListKuIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);	
}

void CCANDlg::OnNMDblclkListTemp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(!m_bSerialPortOpened_TEMP) return;

	g_bFlagTempReferING = FALSE;
	
	CListCtrl* pTemp = (CListCtrl*) GetDlgItem(IDC_LIST_TEMP);
	int nTempIndex = pTemp->GetNextItem(-1, LVNI_SELECTED);

	if( (m_bSerialPortOpened_TEMP)&&(nTempIndex > -1) )
	{
		CString strCurTempNum = pTemp->GetItemText(nTempIndex, 0);
		BYTE byTestTempNum = atoi(strCurTempNum);
		
		byTestTempNum = byTestTempNum*2-1;

		if(m_bSerialPortOpened_TEMP)	//直接发送，而不是调用TempNum
		{
			g_bFlagTempReferING = FALSE;
			g_pStTempSor[byTestTempNum - 1].fTemp = 285.5;
			g_aTempComSendData[1] = 0xffffffA1 + byTestTempNum - 1;
			m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
			g_bTempComRcvFlag = FALSE;
			g_bTempComErrFlag = FALSE;
			g_byTempComErrCount = 0;
			SetTimer(TIMER_ID_TEMP, TIMER_MS_TEMP, NULL);
		}
	}
}
void CCANDlg::OnNMRdblclkListTemp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(!m_bSerialPortOpened_TEMP) return;

	g_bFlagTempReferING = FALSE;
	
	CListCtrl* pTemp = (CListCtrl*) GetDlgItem(IDC_LIST_TEMP);
	int nTempIndex = pTemp->GetNextItem(-1, LVNI_SELECTED);

	if( (m_bSerialPortOpened_TEMP)&&(nTempIndex > -1) )
	{
		CString strCurTempNum = pTemp->GetItemText(nTempIndex, 0);
		BYTE byTestTempNum = atoi(strCurTempNum);
		
		byTestTempNum = byTestTempNum*2;

		if(m_bSerialPortOpened_TEMP)	//直接发送，而不是调用TempNum
		{
			g_bFlagTempReferING = FALSE;
			g_pStTempSor[byTestTempNum - 1].fTemp = 285.5;
			g_aTempComSendData[1] = 0xffffffA1 + byTestTempNum - 1;
			m_SerialPort_TEMP.WriteToPort(g_aTempComSendData,8);
			g_bTempComRcvFlag = FALSE;
			g_bTempComErrFlag = FALSE;
			g_byTempComErrCount = 0;
			SetTimer(TIMER_ID_TEMP, TIMER_MS_TEMP, NULL);
		}
	}
}

void CCANDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_bSysCanRun)
	{
		m_bSerialPortOpened_TEMP=FALSE;
		return;
	}

	TempS_GetTempSorST();

	int nPort = COM_TEMP;
	if(m_SerialPort_TEMP.InitPort(this, nPort, 2400,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))	//波特率2400,无奇偶校验,8数据位1停止位
	{
		m_SerialPort_TEMP.StartMonitoring();
		m_bSerialPortOpened_TEMP=TRUE;

	}
	else
	{
		if(g_bFlagSimTEMP)
		{
			m_bSerialPortOpened_TEMP=TRUE;
		}
		else
		{
			AfxMessageBox("没有发现此串口或被占用");
			m_bSerialPortOpened_TEMP=FALSE;		
		}
	}
	
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened_TEMP);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened_TEMP);
}

void CCANDlg::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SerialPort_TEMP.ClosePort();//关闭串口
    m_bSerialPortOpened_TEMP=FALSE;
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened_TEMP);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened_TEMP);
	
	SetDlgItemTextA(IDC_BUTTON_TEMP_TEST, "开测温");
	
	KillTimer(TIMER_ID_TEMP);	//若正在遍历温度，也关闭

	if(g_pStTempSor != NULL)
	{
		delete [] g_pStTempSor;
		g_pStTempSor = NULL;
	}
}
void CCANDlg::TempListInit(void)
{
	CListCtrl* pTEMPList = (CListCtrl*)GetDlgItem(IDC_LIST_TEMP);
	pTEMPList->SetExtendedStyle( pTEMPList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pTEMPList->InsertColumn( 0, LPCTSTR("库号"), LVCFMT_CENTER, 45 );
	pTEMPList->InsertColumn( 1, LPCTSTR("温度1"), LVCFMT_CENTER, 60 );
	pTEMPList->InsertColumn( 2, LPCTSTR("温度2"), LVCFMT_CENTER, 60 );

	TempListLoad();
}
void CCANDlg::TempListLoad(void)
{
	CListCtrl* pTEMPList = (CListCtrl*)GetDlgItem(IDC_LIST_TEMP);

	pTEMPList->DeleteAllItems();

	for(int i=0; i<g_byTempSum_SetKu/2; i++)
	{
		CString str = "";
		str.Format("%d", i+1);
		pTEMPList->InsertItem( i, LPCTSTR( str ), 0 );
	}
}
void CCANDlg::OnBnClickedButtonOpen2()
{
	// TODO: 在此添加控件通知处理程序代码
	int nPort = COM_MSG; //串口号

	if(m_SerialPort_MSG.InitPort(this, nPort, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))	//波特率9600,无奇偶校验,8数据位1停止位
	{
		m_SerialPort_MSG.StartMonitoring();
		m_bSerialPortOpened_MSG=TRUE;

		//接收自动发串口
		m_SerialPort_MSG.WriteToPort("AT+CNMI=2,2,0,0,0\r\n");
	}
	else
	{
		if(g_bFlagSimTEMP)
		{
			m_bSerialPortOpened_MSG=TRUE;
		}
		else
		{
			AfxMessageBox("没有发现此串口或被占用");
			m_bSerialPortOpened_MSG=FALSE;		
		}
	}
}

void CCANDlg::OnBnClickedButtonClose2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SerialPort_MSG.ClosePort();//关闭串口
    m_bSerialPortOpened_MSG=FALSE;
}

void CCANDlg::OnBnClickedButtonSend2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_bSerialPortOpened_MSG) return; //检查串口是否打开
	
	if(g_hMsg == NULL)
	{
		g_nMsgEx = MSG_EX_NO;	//正常查询
		
		CWinThread* pThread;
		pThread = AfxBeginThread(ThreadProc_Msg, &m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		g_hMsg = pThread->m_hThread;
	}
	else
	{
		Doing2Text("查询状态，短信被占用");
	}

	Doing2Text("手动·发送短信");
}

void CCANDlg::OnBnClickedButtonOpenAns()
{
	m_AnsList.DeleteAllItems();

	if(!g_byAnsSum_SetKu) return;

	int nPort = COM_ANS; //串口号

	if(m_SerialPort_ANS.InitPort(this, nPort, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))	//波特率9600,无奇偶校验,8数据位1停止位
	{		
		m_SerialPort_ANS.StartMonitoring();
		m_bSerialPortOpened_ANS=TRUE;
		
		ANS_GetStatus();
		
		if(g_aAnsComRcvData == NULL)
		{
			g_aAnsComRcvData = (BYTE *)malloc((g_byAnsSum_SetKu*4+2) * sizeof(BYTE));
		}

		if(m_bSerialPortOpened_ANS)
		{
			m_SerialPort_ANS.WriteToPort(g_aAnsComSendData,8);
		}
		SetTimer(TIMER_ID_ANS, TIMER_MS_ANS, NULL);
	}
	else
	{
		AfxMessageBox("没有发现此串口或被占用");
		m_bSerialPortOpened_ANS=FALSE;		
	}
	GetDlgItem(IDC_BUTTON_OPEN_ANS)->EnableWindow(!m_bSerialPortOpened_ANS);
	GetDlgItem(IDC_BUTTON_CLOSE_ANS)->EnableWindow(m_bSerialPortOpened_ANS);
}
void CCANDlg::OnBnClickedButtonCloseAns()
{
	m_SerialPort_ANS.ClosePort();//关闭串口
    m_bSerialPortOpened_ANS=FALSE;
	GetDlgItem(IDC_BUTTON_OPEN_ANS)->EnableWindow(!m_bSerialPortOpened_ANS);
	GetDlgItem(IDC_BUTTON_CLOSE_ANS)->EnableWindow(m_bSerialPortOpened_ANS);

	KillTimer(TIMER_ID_ANS);

	if(g_aAnsComRcvData != NULL)
	{
		free(g_aAnsComRcvData);
		g_aAnsComRcvData = NULL;
	}
}

void CCANDlg::AnsListInit(void)
{
	CListCtrl* pAnsList = (CListCtrl*)GetDlgItem(IDC_LIST_ANS_CTRL);
	pAnsList->SetExtendedStyle( pAnsList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pAnsList->InsertColumn( 0, LPCTSTR("氨检巡路"), LVCFMT_CENTER, 80 );
	pAnsList->InsertColumn( 1, LPCTSTR("浓度"), LVCFMT_CENTER, 50 );
	pAnsList->InsertColumn( 2, LPCTSTR("状态"), LVCFMT_CENTER, 50 );

	pAnsList->DeleteAllItems();
}
//遍历
void ST_CidRefer()
{
	HANDLE aHandle[4] = {g_hPropING, g_hTimer2, g_hDelay3, g_hKuYSJ};	//全局变量，值也在变化
	BOOL bThreadING = FALSE;	//要在线程结束之后再调用，所以为FALSE, 所以可以共用初值
	for(int i=0; i<4; i++)
	{
		if(NULL != aHandle[i])
		{
			bThreadING = TRUE;
			break;
		}
	}

	if(g_bWaitRef && !bThreadING)
	{
		ResumeThread(g_hIDRefer);
		g_bWaitRef = FALSE;
		Doing2Text("恢复单元遍历");	
	}
}
void SP_CidRefer()
{
	if(!g_bWaitRef)
	{
		SuspendThread(g_hIDRefer);
		g_bWaitRef = TRUE;
		Doing2Text("暂停单元遍历");
	}
}
UINT ThreadProc_CidRefer(LPVOID wParam)
{
	CListCtrl* pCidList=(CListCtrl*)wParam;
	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请开控制");
		return -1;
	}

	g_bStopRef = FALSE;
	g_bWaitRef = FALSE;
	
	CString strSqlIDRefer = "";

	//动态提取长度
	int nListEntry_Init = 0;	
	strSqlIDRefer = "SELECT * FROM SetCid WHERE Cid < 3 ORDER BY Cid, CidDQ ASC";	//延时与主机		
	m_pRs_Refer = m_pCon->Execute(_bstr_t(strSqlIDRefer),NULL,adCmdText);	//102.1
	while(!m_pRs_Refer->adoEOF)
	{
		nListEntry_Init++;
		m_pRs_Refer->MoveNext();
	}
	m_pRs_Refer->raw_Close();	//102.2

	strSqlIDRefer = "SELECT * FROM SetCid WHERE Cid > 2 ORDER BY Cid, CidDQ ASC";	//全部节点都遍历		
	m_pRs_Refer = m_pCon->Execute(_bstr_t(strSqlIDRefer),NULL,adCmdText);	//103.1
	while( !g_bStopRef )
	{	
		m_pRs_Refer->MoveFirst();	//游标移动
		int nListEntry = nListEntry_Init;
		while( !m_pRs_Refer->adoEOF )
		{
			//清除选中显示
			int nCount = pCidList->GetItemCount();
			for(int i=0; i<nCount; i++)
			{
				pCidList->SetItemState(i, NULL, LVIS_FOCUSED|LVIS_SELECTED);
			}
			
			CString strCurCid = (LPCSTR)_bstr_t(m_pRs_Refer->GetCollect("Cid"));
			g_dwCobId = atoi(strCurCid);
			CString strCurCidDQ = (LPCSTR)_bstr_t(m_pRs_Refer->GetCollect("CidDQ"));
			int nCidDQ = atoi(strCurCidDQ);
			CString strItem = (LPCSTR)_bstr_t(m_pRs_Refer->GetCollect("IdItem"));

			(nCidDQ == 2)? g_SendData[0] = 0x01: g_SendData[0] = 0x00;
			g_SendData[1] = 0xA0;
			g_SendData[2] = 0xA0;
			g_SendData[3] = 0x01;
			
			if(!g_bFlagSimCAN)
			{
				CAN_Send();
			}
			else
			{
				//4-3-1 模拟1
				//CAN_Send();

				//4-3-2 模拟2
				if(strcmp(strItem, aCidItem[10]) != 0)
				{
					(nCidDQ == 2)? g_RcvData[0] = 3: g_RcvData[0] = 0;
					g_RcvData[1] = 0xA0;
					g_RcvData[2] = 0xA0;
					Sleep(1000);
				}
				else
				{
					g_RcvData[2] = 0x00;
				}
				//模拟
			}

			//等待反馈成功
			int n = 0;
			BOOL bReferError = FALSE;
			while(!g_bFlagSimCAN && !g_bStopRef && !g_bCxTrue && g_RcvData[2] != 0xA0)
			{
				Sleep(1000);
				n++;

				if(n >= 5)
				{
					//151010 暂时屏蔽遍历过程短信报警
					bReferError = TRUE;
					if(!SIM_STATUS)
					{
						CString str = "";
						str.Format("%d-%d CX TimeOut", g_dwCobId, nCidDQ);
						Doing2Text(str);
					}

					break;
					
					/*
					//其他节点遍历一次不成功可跳过
					if(strcmp(strItem, aCidItem[10]) != 0)
					{
						bReferError = TRUE;
						if(!SIM_STATUS)
						{
							CString str = "";
							str.Format("%d-%d CX TimeOut", g_dwCobId, nCidDQ);
							Doing2Text(str);
						}

						break;
					}
					else	//氨检节点确认在线
					{
						//150827 确认流程执行
						LONG lThreadPropExit = 0;
						GetExitCodeThread( g_hPropING, (LPDWORD)&lThreadPropExit);
						if( STILL_ACTIVE == lThreadPropExit )
						{
							Doing2Text("流程执行中，氨检遍历跳过");
							//退出遍历等待
							bReferError = TRUE;
							break;
						}
						else
						{
							if(n == 5)
							{
								CAN_Send();
							}

							if(n >= 10)
							{
								CString str = "";
								str.Format("%02d:%02d  %d通信超时，重发2次失败，退出，报警"
									, g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), g_dwCobId);
								Doing2Text(str);

								// 发送紧急短信
								if(g_hMsg == NULL)
								{
									g_nMsgEx = 4;	//节点丢失
									CWinThread* pThread;
									pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
									g_hMsg = pThread->m_hThread;
								}
								else
								{
									Doing2Text("节点丢失，短信被占用");
								}

								//退出遍历等待
								bReferError = TRUE;
								break;
							}
						}
					}
					*/

				}
			}

			if(g_bStopRef)
			{
				break;
			}

			if(!g_bStopRef)
			{
				if(!bReferError)
				{
					//找到项目所对应
					//CString aCidItem[] = {"压缩机", "氨阀", "分液", "回气", "吸气", "排气", "风机", "水泵", "氨泵", "电磁阀", "氨检"};	//ID选项框
					int nItem = 0;
					for(int i=0; i<NumCidItem; i++)
					{
						if(strItem == aCidItem[i])
						{
							nItem = i;
							break;
						}
					}

					//显示相应状态
					switch(nItem)
					{
					case 0:	//压缩机
						{	
							CString strNC = "";
							(g_RcvData[1] == 161)? strNC = "开": strNC = "关";
							CString strDWNC = "";
							strDWNC.Format("%d+%s", g_RcvData[0], strNC);
							pCidList->SetItemText( nListEntry, 4 , LPCTSTR( strDWNC ));
							break;
						}	
					case 7:	//电气
					case 8:
					case 9:
					case 6://风机
						{
							CString strNC = "";
							if(nCidDQ == 2)
							{
								(g_RcvData[0] == 3)? strNC = "开": strNC = "关";
							}
							else 
							{
								g_RcvData[0]? strNC = "开": strNC = "关";
							}
							pCidList->SetItemText( nListEntry, 4 , LPCTSTR( strNC ));
							break;
						}
					case 1:	//氨阀
						{
							CString strKDYW = "";
							strKDYW.Format("%d+%d", g_RcvData[0], g_RcvData[1]);
							pCidList->SetItemText( nListEntry, 4 , LPCTSTR( strKDYW ));
							
							//150827
							if(g_mTimeNow.GetMinute()%2)
							{
								CString strTextKDYW = "";
								strTextKDYW.Format("【氨阀】开度：%d , 液位：%d", g_RcvData[0], g_RcvData[1]);
								Doing2Text(strTextKDYW);
							}

							break;					
						}
					case 2: //阀门	
					case 3:
					case 4:
					case 5:
					case 10://氨检
						{
							CString strDWKD = "";
							strDWKD.Format("%d", g_RcvData[0]);
							pCidList->SetItemText( nListEntry, 4 , LPCTSTR( strDWKD ));
							break;
						}
					default:
						{
							//AfxMessageBox("ERROR");
							break;			
						}
					}
				}
				else
				{
					CString strStatus = "--";
					pCidList->SetItemText( nListEntry, 4 , LPCTSTR( strStatus ));				
				}			

				pCidList->SetItemState(nListEntry, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);

				nListEntry++;
				m_pRs_Refer->MoveNext();
				Sleep(1001);
			}
		}
	}
	m_pRs_Refer->raw_Close();	//103.2

	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->GetDlgItem(IDC_BUTTON_REFER_ALL_CTRL)->EnableWindow(TRUE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_REF_CTRL)->EnableWindow(FALSE);
	}

	g_hIDRefer = NULL;

	return 0;
}

void CCANDlg::CidCtrlInit(void)
{
	CListBox* pCidItem = (CListBox*)GetDlgItem(IDC_LIST_CID_ITEM_CTRL);
	CListCtrl* pCidList = (CListCtrl*)GetDlgItem(IDC_LIST_CID_CTRL);

	for(int i=0; i<sizeof(aCidItem)/4; i++)
	{
		pCidItem->AddString(*(aCidItem+i));
	}

	pCidList->SetExtendedStyle( pCidList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pCidList->InsertColumn( 0, LPCTSTR("编"), LVCFMT_CENTER, 35 );
	pCidList->InsertColumn( 1, LPCTSTR("DQ"), LVCFMT_CENTER, 35 );
	pCidList->InsertColumn( 2, LPCTSTR("项目"), LVCFMT_CENTER, 60 );
	pCidList->InsertColumn( 3, LPCTSTR("号"), LVCFMT_CENTER, 35 );
	pCidList->InsertColumn( 4, LPCTSTR("状态"), LVCFMT_CENTER, 60 );

	CidCtrlLoad();
}

void CCANDlg::CidCtrlLoad(void)
{
	CListCtrl* pCidList = (CListCtrl*)GetDlgItem(IDC_LIST_CID_CTRL);
	pCidList->DeleteAllItems();

	CString strSql = "";
	strSql = "SELECT * FROM SetCid ORDER BY Cid, CidDQ, IdItemNum  ASC";
	m_pRs->Open((_variant_t)strSql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//RS-1.1

	int nListEntry = 0;
	while(!m_pRs->adoEOF)
	{
		CString strTem = "";
		strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)1)->Value;

		pCidList->InsertItem( nListEntry, LPCTSTR( strTem ), 0 );
		
		for (int i=2; i<m_pRs->GetFields()->Count; i++)
		{
			strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)i)->Value;
			pCidList->SetItemText( nListEntry, i-1, LPCTSTR( strTem ) );
		}
		
		nListEntry ++;
		m_pRs->MoveNext();
	}	
	m_pRs->raw_Close();	//Rs-1.2

	g_byCidSum_SetKu = nListEntry;
	
	//下位机节点结构体
	if(g_pStCid != NULL)
	{
		delete [] g_pStCid;
		g_pStCid = NULL;
	}
	g_pStCid = new Cid_t[g_byCidSum_SetKu];
	for(int i=0; i<g_byCidSum_SetKu; i++)
	{
		memset(&g_pStCid[i], 0, sizeof(Cid_t));
	}

	strSql = "SELECT * FROM SetCid ORDER BY Cid,CidDQ,IdItemNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-2.1
	int nCidIndex = 0;
	while( !m_pRs->adoEOF )
	{
		CString strCid = (LPCSTR)_bstr_t(m_pRs->GetCollect("Cid"));
		g_pStCid[nCidIndex].nID = atoi(strCid);
		CString strCidDQ = (LPCSTR)_bstr_t(m_pRs->GetCollect("CidDQ"));
		g_pStCid[nCidIndex].byDQ = atoi(strCidDQ);

		g_pStCid[nCidIndex].bSnd = FALSE;
		g_pStCid[nCidIndex].bRcv = FALSE;

		nCidIndex ++;
		m_pRs->MoveNext();
	}	
	m_pRs->raw_Close();	//RS-2.2

	//151201 - 查看是否使用CAN2
	g_bCAN2_InUse = FALSE;
	/*strSql.Format("SELECT * FROM SetCid WHERE Cid > %d", g_byCAN12ID);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151201-1.1
	(m_pRs->BOF)?	g_bCAN2_InUse = FALSE:	g_bCAN2_InUse = TRUE; 
	m_pRs->raw_Close();	//151201-1.2
	PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CTRL_CLOSE, BN_CLICKED), NULL);*/

	//显示最后一行
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_CID_CTRL);
	int nCount = pList->GetItemCount();
	if(nCount > 0)
	{
		pList->EnsureVisible(nCount-1, 0);
	}
}

void CCANDlg::OnNMDblclkListCidCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CListCtrl* pCid = (CListCtrl*) GetDlgItem(IDC_LIST_CID_CTRL);
	int nCidIndex = pCid->GetNextItem(-1, LVNI_SELECTED);

	if(nCidIndex > -1)
	{
		//将ID设置和开关机流程设置分别填充
		CListBox* pCidItem = (CListBox*) GetDlgItem(IDC_LIST_CID_ITEM_CTRL);

		CString strCurCid = pCid->GetItemText(nCidIndex, 0);
		m_byCidCtrlID = atoi(strCurCid);
		CString strCurCidDQ = pCid->GetItemText(nCidIndex, 1);
		CString strCurItem = pCid->GetItemText(nCidIndex, 2);
		CString strCurItemNum = pCid->GetItemText(nCidIndex, 3);
		m_byCidCtrlDQ = atoi(strCurCidDQ);
		m_byCidCtrlItemNum = atoi(strCurItemNum);

		int nCurSelCidItem = 0;
		for(int i=0; i<NumCidItem; i++)
		{
			if(strcmp(strCurItem, aCidItem[i]) == 0)
			{
				nCurSelCidItem = i;
				break;
			}
		}


		pCidItem->SetCurSel(nCurSelCidItem);
		CString strCurSel = "";
		pCidItem->GetText(nCurSelCidItem, strCurSel);
		if(strcmp(strCurSel, strCurItem) != 0)
		{
			pCidItem->SetCurSel(-1);
		}

		UpdateData(FALSE);
	}	
}

void CCANDlg::OnBnClickedButtonReferAllCtrl()
{
	if(!g_bOpened_CAN1 )
	{
		AfxMessageBox("请开控制");
		return;
	}

	LONG lThreadIDReferExit = 0;
	GetExitCodeThread( g_hIDRefer, (LPDWORD)&lThreadIDReferExit);

	if(STILL_ACTIVE != lThreadIDReferExit)
	{
		CWinThread* pThread;
		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_CID_CTRL);
		pThread = AfxBeginThread(ThreadProc_CidRefer, pList, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		g_hIDRefer = pThread->m_hThread;

		GetDlgItem(IDC_BUTTON_REFER_ALL_CTRL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP_REF_CTRL)->EnableWindow(TRUE);
	}
}

void CCANDlg::OnBnClickedButtonStopRefCtrl()
{
	//151103
	ST_CidRefer();
	g_bStopRef = TRUE;
}

void CCANDlg::OnBnClickedButtonTestOpenCtrl()
{
	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请开控制");
		return;
	}

	UpdateData(TRUE);
	if(m_byCidCtrlID<3)
	{
		MessageBox("项目号码错误");
		return;
	}

	CButton* pOPEN = (CButton*)GetDlgItem(IDC_BUTTON_TEST_OPEN_CTRL);
	pOPEN->EnableWindow(FALSE);

	g_dwCobId = m_byCidCtrlID;

	CListBox* pCidItem = (CListBox*) GetDlgItem(IDC_LIST_CID_ITEM_CTRL);
	int nTempCidItem = pCidItem->GetCurSel();
	CString strCidItem = "";
	if(nTempCidItem != -1)
	{
		int nTempCidItem = pCidItem->GetCurSel();
		strCidItem = aCidItem[nTempCidItem];
	}

	if(strcmp(strCidItem, "压缩机") == 0)
	{
		g_SendData[0] = 0x01;
		g_SendData[1] = 0xA1;
	}
	else if(strcmp(strCidItem, "氨阀") == 0)
	{
		MessageBox("会影响氨阀是否自动控制");
		g_SendData[0] = 0x64;
		g_SendData[1] = 0xA3;
	}
	else
	{
		(m_byCidCtrlDQ == 2)? g_SendData[0] = 0x03: g_SendData[0] = 0x01;

		CString strSql = "";
		strSql.Format("SELECT * FROM SetDQ WHERE DQItem = '%s' AND DQNum = %d", strCidItem, m_byCidCtrlItemNum);
		m_pRs_CtrlHand = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//52.1
		if(!m_pRs_CtrlHand->BOF)
		{
			CString strDQNC = (LPCSTR)_bstr_t(m_pRs_CtrlHand->GetCollect("DQNC"));
			(strcmp(strDQNC, "单动开关") == 0)? g_SendData[1] = 0xA1: g_SendData[1] = 0xA2;
		}
		else	//没有设置该电气，默认单动
		{
			g_SendData[1] = 0xA1;
		}
		m_pRs_CtrlHand->raw_Close();	//52.2

	}

	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;

	CAN_Send();

	int n=0;
	while(!g_bExeTrue && g_RcvData[2] != 0xA1)
	{
		Sleep(1000);
		n++;
		//ExEvent();

		if(n >= 20)
		{
			CString str = "";
			str.Format("%02d:%02d  %d通信超时，报警！", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), g_dwCobId);
			AfxMessageBox(str);
			pOPEN->EnableWindow(TRUE);
			break;
		}
	}

	CString strStatus = "";
	if(strcmp(strCidItem, "压缩机") == 0)
	{
		strStatus.Format("%d+%d", g_RcvData[0], g_RcvData[1]-160);
	}
	else if(strcmp(strCidItem, "氨阀") == 0)
	{
		strStatus.Format("%d+%d", g_RcvData[0], g_RcvData[1]);
	}
	else
	{
		strStatus.Format("%d", g_RcvData[0]);
	}

	CListCtrl* pCidList = (CListCtrl*)GetDlgItem(IDC_LIST_CID_CTRL);
	int nCidIndex = pCidList->GetNextItem(-1, LVNI_SELECTED);
	pCidList->SetItemText( nCidIndex, 4 , LPCTSTR( strStatus ));

	pOPEN->EnableWindow(TRUE);
}

void CCANDlg::OnBnClickedButtonTestCloseCtrl()
{
	// TODO: 在此添加控件通知处理程序代码

	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请开控制");
		return;
	}

	UpdateData(TRUE);
	if(m_byCidCtrlID<3)
	{
		MessageBox("项目号码错误");
		return;
	}

	CButton* pCLOSE = (CButton*)GetDlgItem(IDC_BUTTON_TEST_CLOSE_CTRL);
	pCLOSE->EnableWindow(FALSE);

	g_dwCobId = m_byCidCtrlID;

	CListBox* pCidItem = (CListBox*) GetDlgItem(IDC_LIST_CID_ITEM_CTRL);
	int nTempCidItem = pCidItem->GetCurSel();
	CString strCidItem = "";
	if(nTempCidItem != -1)
	{
		int nTempCidItem = pCidItem->GetCurSel();
		strCidItem = aCidItem[nTempCidItem];
	}

	if(strcmp(strCidItem, "压缩机") == 0)
	{
		g_SendData[0] = 0x00;
		g_SendData[1] = 0xA1;
	}
	else if(strcmp(strCidItem, "氨阀") == 0)
	{
		g_SendData[0] = 0x65;
		g_SendData[1] = 0xA3;
	}
	else
	{
		(m_byCidCtrlDQ == 2)? g_SendData[0] = 0x02: g_SendData[0] = 0x00;	

		CString strSql = "";
		strSql.Format("SELECT * FROM SetDQ WHERE DQItem = '%s' AND DQNum = %d", strCidItem, m_byCidCtrlItemNum);
		m_pRs_CtrlHand = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//53.1
		if(!m_pRs_CtrlHand->BOF)
		{
			CString strDQNC = (LPCSTR)_bstr_t(m_pRs_CtrlHand->GetCollect("DQNC"));
			(strcmp(strDQNC, "单动开关") == 0)? g_SendData[1] = 0xA1: g_SendData[1] = 0xA2;
		}
		else	//没有设置该电气，默认单动
		{
			g_SendData[1] = 0xA1;
		}
		m_pRs_CtrlHand->raw_Close();	//53.2

	}
	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;

	Sleep(1000);

	CAN_Send();

	int n=0;
	while(!g_bExeTrue && g_RcvData[2] != 0xA1)
	{
		Sleep(1000);
		n++;
		//ExEvent();
		if(n >= 20)
		{
			CString str = "";
			str.Format("%02d:%02d  %d通信超时，报警！", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), g_dwCobId);
			AfxMessageBox(str);
			pCLOSE->EnableWindow(TRUE);
			break;
		}
	}

	CString strStatus = "";
	if(strcmp(strCidItem, "压缩机") == 0)
	{
		strStatus.Format("%d+%d", g_RcvData[0], g_RcvData[1]-160);
	}
	else if(strcmp(strCidItem, "氨阀") == 0)
	{
		strStatus.Format("%d+%d", g_RcvData[0], g_RcvData[1]);
	}
	else
	{
		strStatus.Format("%d", g_RcvData[0]);
	}

	CListCtrl* pCidList = (CListCtrl*)GetDlgItem(IDC_LIST_CID_CTRL);
	int nCidIndex = pCidList->GetNextItem(-1, LVNI_SELECTED);
	pCidList->SetItemText( nCidIndex, 4 , LPCTSTR( strStatus ));

	pCLOSE->EnableWindow(TRUE);
}

void CCANDlg::OnBnClickedButtonTestCxCtrl()
{
	// TODO: 在此添加控件通知处理程序代码

	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请开控制");
		return;
	}

	UpdateData(TRUE);
	if(m_byCidCtrlID<3)
	{
		MessageBox("项目号码错误");
		return;
	}

	CButton* pCX = (CButton*)GetDlgItem(IDC_BUTTON_TEST_CX_CTRL);
	pCX->EnableWindow(FALSE);

	g_dwCobId = m_byCidCtrlID;

	(m_byCidCtrlDQ == 2)? g_SendData[0] = 0x01: g_SendData[0] = 0x00;
	g_SendData[1] = 0xA0;
	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;

	CAN_Send();

	//等待反馈成功
	int n=0;
	while(!g_bCxTrue && g_RcvData[2] != 0xA0)
	{
		Sleep(1000);
		n++;
		//ExEvent();
		if(n >= 10)
		{
			CString str = "";
			str.Format("%02d:%02d  %d通信超时，报警！", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), g_dwCobId);
			AfxMessageBox(str);
			pCX->EnableWindow(TRUE);
			break;
		}
	}

	CListBox* pCidItem = (CListBox*) GetDlgItem(IDC_LIST_CID_ITEM_CTRL);
	int nTempCidItem = pCidItem->GetCurSel();
	CString strCidItem = "";
	if(nTempCidItem != -1)
	{
		int nTempCidItem = pCidItem->GetCurSel();
		strCidItem = aCidItem[nTempCidItem];
	}

	CString strStatus = "";
	if(strcmp(strCidItem, "压缩机") == 0)
	{
		strStatus.Format("%d+%d", g_RcvData[0], g_RcvData[1]-160);
	}
	else if(strcmp(strCidItem, "氨阀") == 0)
	{
		strStatus.Format("%d+%d", g_RcvData[0], g_RcvData[1]);
	}
	else
	{
		strStatus.Format("%d", g_RcvData[0]);
	}

	CListCtrl* pCidList = (CListCtrl*)GetDlgItem(IDC_LIST_CID_CTRL);
	int nCidIndex = pCidList->GetNextItem(-1, LVNI_SELECTED);
	pCidList->SetItemText( nCidIndex, 4 , LPCTSTR( strStatus ));

	pCX->EnableWindow(TRUE);
}

void CCANDlg::OnBnClickedButtonTestToCtrl()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_bOpened_CAN1)
	{
		MessageBox("请开控制");
		return;
	}

	UpdateData(TRUE);

	if(g_pCanDlg->m_byCidCtrlID<3)
	{
		AfxMessageBox("项目号码错误");
		return;
	}

	g_dwCobId = g_pCanDlg->m_byCidCtrlID;

	CListBox* pCidItem = (CListBox*) g_pCanDlg->GetDlgItem(IDC_LIST_CID_ITEM_CTRL);
	int nTempCidItem = pCidItem->GetCurSel();
	CString strCidItem = "";
	if(nTempCidItem != -1)
	{
		int nTempCidItem = pCidItem->GetCurSel();
		strCidItem = aCidItem[nTempCidItem];
	}

	if(strcmp(strCidItem, "压缩机") == 0)
	{
		g_SendData[0] = g_pCanDlg->m_byTestTO;
		
		CString strDW = "0";
		CString strSql = "";
		strSql.Format("SELECT * FROM SetYSJ WHERE YSJ = %d", g_pCanDlg->m_byCidCtrlItemNum);
		m_pRs_CtrlHand = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//54.1
		if(!m_pRs_CtrlHand->BOF)
		{
			strDW = (LPCSTR)_bstr_t(m_pRs_CtrlHand->GetCollect("DW"));
		}
		else	//没有设置该电气，默认单动
		{
			CString str = "";
			str.Format("未设置压缩机%d档位数", g_pCanDlg->m_byCidCtrlItemNum);
			AfxMessageBox(str);
		}
		m_pRs_CtrlHand->raw_Close();	//54.2

		if(g_SendData[0] > atoi(strDW))
		{
			g_SendData[0] = atoi(strDW);
		}	

		g_SendData[1] = 0xA2;
	}
	else
	{
		g_SendData[0] = g_pCanDlg->m_byTestTO;	
		if(g_SendData[0] > 99)
		{
			g_SendData[0] = 99;
		}
		g_SendData[1] = 0xA3;
	}
	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;

	CAN_Send();
}
//在用库房显示
void CCANDlg::KuCtrlDispInit(void)
{
	CListCtrl* pKuCtrlList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_CTRL);
	pKuCtrlList->SetExtendedStyle( pKuCtrlList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	pKuCtrlList->InsertColumn( 0, LPCTSTR("库号"), LVCFMT_CENTER, 50 );
	pKuCtrlList->InsertColumn( 1, LPCTSTR("温1"), LVCFMT_CENTER, 40 );
	pKuCtrlList->InsertColumn( 2, LPCTSTR("℃"), LVCFMT_CENTER, 70 );
	pKuCtrlList->InsertColumn( 3, LPCTSTR("温2"), LVCFMT_CENTER, 40 );
	pKuCtrlList->InsertColumn( 4, LPCTSTR("℃"), LVCFMT_CENTER, 70 );
	pKuCtrlList->InsertColumn( 5, LPCTSTR("上限"), LVCFMT_CENTER, 50 );
	pKuCtrlList->InsertColumn( 6, LPCTSTR("下限"), LVCFMT_CENTER, 50 );
	pKuCtrlList->InsertColumn( 7, LPCTSTR("库温"), LVCFMT_CENTER, 70 );
	pKuCtrlList->InsertColumn( 8, LPCTSTR("压缩机"), LVCFMT_CENTER, 65 );
	pKuCtrlList->InsertColumn( 9, LPCTSTR("分液"), LVCFMT_CENTER, 60 );
	pKuCtrlList->InsertColumn( 10, LPCTSTR("KD"), LVCFMT_CENTER, 60 );
	pKuCtrlList->InsertColumn( 11, LPCTSTR("回气"), LVCFMT_CENTER, 45 );
	pKuCtrlList->InsertColumn( 12, LPCTSTR("KD"), LVCFMT_CENTER, 40 );
	pKuCtrlList->InsertColumn( 13, LPCTSTR("风机"), LVCFMT_CENTER, 45 );
	pKuCtrlList->InsertColumn( 14, LPCTSTR("NC"), LVCFMT_CENTER, 40 );
	pKuCtrlList->InsertColumn( 15, LPCTSTR("控制"), LVCFMT_CENTER, 45 );

	KuCtrlDispLoad();
}

void CCANDlg::KuCtrlDispLoad(void)
{
	CListCtrl* pKuCtrlList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_CTRL);
	pKuCtrlList->DeleteAllItems();

	CString strSqlKuCtrlDisp = "";
	strSqlKuCtrlDisp = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 ORDER BY KuNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSqlKuCtrlDisp),NULL,adCmdText);	//87.1
	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{
		CString strKuNum = "", strTempNum = "", strTempUp = "", strTempDn = "";
		CString strKuYSJ = "", strKuFY = "", strKuHQ = "", strKuFJ = "";
		CString strKuWorkStatus = "";

		strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum")); 
		strKuWorkStatus = (LPCSTR)_bstr_t(m_pRs->GetCollect("WorkStatus"));
		CString strStatus = "";
		(strcmp(strKuWorkStatus, "0") == 0)? strStatus = " 关": strStatus = " 开";
		strKuNum += strStatus;
		pKuCtrlList->InsertItem( nListEntry, LPCTSTR( strKuNum ), 0 );
		
		strTempNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempNum"));
		BYTE aTempNum[2] = {0, 0};
		Str2Dec(strTempNum, aTempNum);
		CString strTempNum1 = "";
		strTempNum1.Format("%d", aTempNum[0]);
		pKuCtrlList->SetItemText( nListEntry, 1, LPCTSTR(strTempNum1) );
		CString strTempNum2 = "";
		strTempNum2.Format("%d", aTempNum[1]);
		pKuCtrlList->SetItemText( nListEntry, 3, LPCTSTR(strTempNum2) );

		strTempUp = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempUp"));
		strTempDn = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempDn"));	
		float fTempUp=(float)atof(strTempUp);
		if( (fTempUp > 0.0) && (fTempUp < 1.0) )
		{
			strTempUp.Format("%.1f",fTempUp);		
		}
		if( (fTempUp < 0.0) && (fTempUp > -1.0) )
		{
			strTempUp.Format("%2.1f",fTempUp);
		}
		float fTempDn=(float)atof(strTempDn);
		if( (fTempDn > 0.0) && (fTempDn < 1.0) )
		{
			strTempDn.Format("%.1f",fTempDn);		
		}
		if( (fTempDn < 0.0) && (fTempDn > -1.0) )
		{
			strTempDn.Format("%2.1f",fTempDn);
		}
		pKuCtrlList->SetItemText( nListEntry, 5, LPCTSTR(strTempUp) );
		pKuCtrlList->SetItemText( nListEntry, 6, LPCTSTR(strTempDn) );

		strKuYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuYSJ"));
		strKuFY = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFY"));
		strKuHQ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuHQ"));
		strKuFJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFJ"));
		pKuCtrlList->SetItemText( nListEntry, 8, LPCTSTR(strKuYSJ) );
		pKuCtrlList->SetItemText( nListEntry, 9, LPCTSTR(strKuFY) );
		pKuCtrlList->SetItemText( nListEntry, 11, LPCTSTR(strKuHQ) );
		pKuCtrlList->SetItemText( nListEntry, 13, LPCTSTR(strKuFJ) );

		CString strNone = "--";
		pKuCtrlList->SetItemText( nListEntry, 2, LPCTSTR(strNone) );
		pKuCtrlList->SetItemText( nListEntry, 4, LPCTSTR(strNone) );
		pKuCtrlList->SetItemText( nListEntry, 7, LPCTSTR(strNone) );
		pKuCtrlList->SetItemText( nListEntry, 10, LPCTSTR("--,--") );
		pKuCtrlList->SetItemText( nListEntry, 12, LPCTSTR(strNone) );
		pKuCtrlList->SetItemText( nListEntry, 14, LPCTSTR(strNone) );

		//150807
		CString strCtrlMode = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuCtrlMode"));
		(strcmp(strCtrlMode, "时开温关") == 0)? strCtrlMode = "时温":strCtrlMode = "温温";
		pKuCtrlList->SetItemText( nListEntry, 15, LPCTSTR(strCtrlMode) );

		nListEntry ++;
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//87.2
}

void CCANDlg::YSJCtrlDispInit(void)
{
	CListCtrl* pYSJCtrlList = (CListCtrl*)GetDlgItem(IDC_LIST_YSJ_CTRL);
	pYSJCtrlList->SetExtendedStyle( pYSJCtrlList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	pYSJCtrlList->InsertColumn( 0, LPCTSTR("压缩机"), LVCFMT_CENTER, 65 );
	pYSJCtrlList->InsertColumn( 1, LPCTSTR("开关"), LVCFMT_CENTER, 45 );
	pYSJCtrlList->InsertColumn( 2, LPCTSTR("档位"), LVCFMT_CENTER, 45 );
	pYSJCtrlList->InsertColumn( 3, LPCTSTR("吸气"), LVCFMT_CENTER, 45 );
	pYSJCtrlList->InsertColumn( 4, LPCTSTR("开度"), LVCFMT_CENTER, 55 );
	pYSJCtrlList->InsertColumn( 5, LPCTSTR("排气"), LVCFMT_CENTER, 45 );
	pYSJCtrlList->InsertColumn( 6, LPCTSTR("开度"), LVCFMT_CENTER, 55 );
	pYSJCtrlList->InsertColumn( 7, LPCTSTR("氨阀"), LVCFMT_CENTER, 45 );
	pYSJCtrlList->InsertColumn( 8, LPCTSTR("开度"), LVCFMT_CENTER, 55 );
	pYSJCtrlList->InsertColumn( 9, LPCTSTR("液位"), LVCFMT_CENTER, 55 );
	pYSJCtrlList->InsertColumn( 10, LPCTSTR("氨泵"), LVCFMT_CENTER, 45 );
	pYSJCtrlList->InsertColumn( 11, LPCTSTR("开关"), LVCFMT_CENTER, 55 );
	pYSJCtrlList->InsertColumn( 12, LPCTSTR("水泵"), LVCFMT_CENTER, 45 );
	pYSJCtrlList->InsertColumn( 13, LPCTSTR("开关"), LVCFMT_CENTER, 55 );
	pYSJCtrlList->InsertColumn( 14, LPCTSTR("优先级"), LVCFMT_CENTER, 75 );

	YSJCtrlDispLoad();
}

void CCANDlg::YSJCtrlDispLoad(void)
{
	CListCtrl* pYSJCtrlList = (CListCtrl*)GetDlgItem(IDC_LIST_YSJ_CTRL);
	pYSJCtrlList->DeleteAllItems();

	CString strSqlYSJCtrlDisp = "";
	strSqlYSJCtrlDisp = "SELECT * FROM SetYSJ ORDER BY YSJ ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSqlYSJCtrlDisp),NULL,adCmdText);	//88.1
	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{
		CString strYSJ = "", strXQ = "", strPQ = "", strAF = "", strAB="", strSB = "", strPRI = "";

		strYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("YSJ"));
		pYSJCtrlList->InsertItem( nListEntry, LPCTSTR( strYSJ ), 0 );
		strXQ = (LPCSTR)_bstr_t(m_pRs->GetCollect("XQ"));
		pYSJCtrlList ->SetItemText(nListEntry, 3, LPCTSTR(strXQ) );
		strPQ = (LPCSTR)_bstr_t(m_pRs->GetCollect("PQ"));
		pYSJCtrlList ->SetItemText(nListEntry, 5, LPCTSTR(strPQ) );
		strAF = (LPCSTR)_bstr_t(m_pRs->GetCollect("AF"));
		pYSJCtrlList ->SetItemText(nListEntry, 7, LPCTSTR(strAF) );
		strAB = (LPCSTR)_bstr_t(m_pRs->GetCollect("AB"));
		pYSJCtrlList ->SetItemText(nListEntry, 10, LPCTSTR(strAB) );
		strSB = (LPCSTR)_bstr_t(m_pRs->GetCollect("SB"));
		pYSJCtrlList ->SetItemText(nListEntry, 12, LPCTSTR(strSB) );
		strPRI = (LPCSTR)_bstr_t(m_pRs->GetCollect("PRI"));
		pYSJCtrlList ->SetItemText(nListEntry, 14, LPCTSTR(strPRI) );

		CString strNone = "--";
		pYSJCtrlList->SetItemText( nListEntry, 1, LPCTSTR(strNone) );
		pYSJCtrlList->SetItemText( nListEntry, 2, LPCTSTR(strNone) );
		pYSJCtrlList->SetItemText( nListEntry, 4, LPCTSTR(strNone) );
		pYSJCtrlList->SetItemText( nListEntry, 6, LPCTSTR(strNone) );
		pYSJCtrlList->SetItemText( nListEntry, 8, LPCTSTR(strNone) );
		pYSJCtrlList->SetItemText( nListEntry, 9, LPCTSTR(strNone) );
		pYSJCtrlList->SetItemText( nListEntry, 11, LPCTSTR(strNone) );
		pYSJCtrlList->SetItemText( nListEntry, 13, LPCTSTR(strNone) );

		nListEntry ++;
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//88.2

	//151118
	GetYSJSt();
}

//库房控制
void CCANDlg::OnBnClickedButtonStartTempCtrl()
{
	Doing2Text("手动·一键开机");
	if( GetDlgItem(IDC_BUTTON_OPEN)->IsWindowEnabled() )
		PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_OPEN, BN_CLICKED), NULL);
	if( GetDlgItem(IDC_BUTTON_CTRL_OPEN)->IsWindowEnabled() )
		PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CTRL_OPEN, BN_CLICKED), NULL);
	if( GetDlgItem(IDC_BUTTON_REFER_ALL_CTRL)->IsWindowEnabled() )
		PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_REFER_ALL_CTRL, BN_CLICKED), NULL);

	if(g_byAnsSum_SetKu)
	{
		if( GetDlgItem(IDC_BUTTON_OPEN_ANS)->IsWindowEnabled() )
			PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_OPEN_ANS, BN_CLICKED), NULL);
	}

	g_bOneKeyStart = FALSE;	//不直接开机

	if(g_hKuCtrl == NULL)
	{
		CWinThread* pThread;
		pThread = AfxBeginThread(ThreadProc_KuCtrl, &m_ctrlProp, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		g_hKuCtrl = pThread->m_hThread;		
	}
}

void CCANDlg::OnBnClickedButtonSuspendTempCtrl()
{	
	LONG lThreadKuCtrlExit = 0;
	LONG lThreadPropExit = 0;
	LONG lThreadDelay3Exit = 0;
	GetExitCodeThread( g_hKuCtrl, (LPDWORD)&lThreadKuCtrlExit);
	GetExitCodeThread( g_hPropING, (LPDWORD)&lThreadPropExit);
	GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);

	LONG lThreadTimer2Exit = 0;
	GetExitCodeThread( g_hTimer2, (LPDWORD)&lThreadTimer2Exit);
	LONG lThreadWatchExit = 0;
	GetExitCodeThread( g_hWatch, (LPDWORD)&lThreadWatchExit);
	 
	if(STILL_ACTIVE == lThreadPropExit)
	{
		SuspendThread(g_hPropING);
		SuspendThread(g_hKuCtrl);
	}
	else
	{
		if(STILL_ACTIVE == lThreadKuCtrlExit) 
			SuspendThread(g_hKuCtrl);
	}

	if(STILL_ACTIVE == lThreadDelay3Exit)
		SuspendThread(g_hDelay3);

	if(STILL_ACTIVE == lThreadTimer2Exit)
		SuspendThread(g_hTimer2);
	if(STILL_ACTIVE == lThreadWatchExit)
		SuspendThread(g_hWatch);
	
	Doing2Text("手动·暂停库控");

	GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SUSPEND_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RESUME_TEMP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(FALSE);
}

void CCANDlg::OnBnClickedButtonResumeTempCtrl()
{
	 LONG lThreadKuCtrlExit = 0;
	 LONG lThreadPropExit = 0;
	 LONG lThreadDelay3Exit = 0;
	 GetExitCodeThread( g_hKuCtrl, (LPDWORD)&lThreadKuCtrlExit);
	 GetExitCodeThread( g_hPropING, (LPDWORD)&lThreadPropExit);
	 GetExitCodeThread( g_hDelay3, (LPDWORD)&lThreadDelay3Exit);

 	LONG lThreadTimer2Exit = 0;
	GetExitCodeThread( g_hTimer2, (LPDWORD)&lThreadTimer2Exit);
	LONG lThreadWatchExit = 0;
	GetExitCodeThread( g_hWatch, (LPDWORD)&lThreadWatchExit);
	 
	if(STILL_ACTIVE == lThreadPropExit)
	{
		ResumeThread(g_hPropING);
		ResumeThread(g_hKuCtrl);
	}
	else
	{
		if(STILL_ACTIVE == lThreadKuCtrlExit) 
			ResumeThread(g_hKuCtrl);
	}

	if(STILL_ACTIVE == lThreadDelay3Exit)
		ResumeThread(g_hDelay3);

	if(STILL_ACTIVE == lThreadTimer2Exit)
		ResumeThread(g_hTimer2);
	if(STILL_ACTIVE == lThreadWatchExit)
		ResumeThread(g_hWatch);

	Doing2Text("手动·恢复库控");

	GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SUSPEND_TEMP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RESUME_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(TRUE);

	if(g_bOneKeyStop)
	{
		GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SUSPEND_TEMP_CTRL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RESUME_TEMP_CTRL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(FALSE);	
	}
}

void CCANDlg::OnBnClickedButtonStopTempCtrl()
{
	// TODO: 在此添加控件通知处理程序代码
	g_bStopKuCtrl = TRUE;
	g_bStopProp = TRUE;
	Doing2Text("手动·停止库控");

	//控件状态修改在KuCtrl最后
}
void CCANDlg::OnBnClickedButtonStartOnekey()
{
	Doing2Text("手动·直接开机");
	if( GetDlgItem(IDC_BUTTON_OPEN)->IsWindowEnabled() )
		PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_OPEN, BN_CLICKED), NULL);
	if( GetDlgItem(IDC_BUTTON_CTRL_OPEN)->IsWindowEnabled() )
		PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CTRL_OPEN, BN_CLICKED), NULL);
	if( GetDlgItem(IDC_BUTTON_REFER_ALL_CTRL)->IsWindowEnabled() )
		PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_REFER_ALL_CTRL, BN_CLICKED), NULL);

	if(g_byAnsSum_SetKu)
	{
		if( GetDlgItem(IDC_BUTTON_OPEN_ANS)->IsWindowEnabled() )
			PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_OPEN_ANS, BN_CLICKED), NULL);
	}

	g_bOneKeyStart = TRUE;	//直接开机

	if(g_hKuCtrl == NULL)
	{
		CWinThread* pThread;
		pThread = AfxBeginThread(ThreadProc_KuCtrl, &m_ctrlProp, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		g_hKuCtrl = pThread->m_hThread;		
	}
}


void CCANDlg::OnBnClickedButtonStopOnekey()
{	//强制关机
	//执行全部在用库房的关机流程
	g_bStopProp = TRUE;
	g_bStopKuCtrl = TRUE;
	g_bOneKeyStop = TRUE;
	g_bOneKeyStart = FALSE;

	Doing2Text("手动·一键关机");

	//关闭各端口--在这里关闭会导致内存泄露，在线程中关闭
	//if( GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->IsWindowEnabled() )
	//	PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_STOP_TEMP_CTRL, BN_CLICKED), NULL);
	//if( GetDlgItem(IDC_BUTTON_CTRL_CLOSE)->IsWindowEnabled() )
	//	PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CTRL_CLOSE, BN_CLICKED), NULL);
	//if( GetDlgItem(IDC_BUTTON_CLOSE)->IsWindowEnabled() )
	//	PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CLOSE, BN_CLICKED), NULL);
	//if( GetDlgItem(IDC_BUTTON_CLOSE2)->IsWindowEnabled() )
	//	PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CLOSE2, BN_CLICKED), NULL);

	GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SUSPEND_TEMP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RESUME_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(FALSE);
	//150827
	GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(TRUE);
}

void CCANDlg::OnBnClickedButtonRstKuProp()
{
	//151012
	if(g_hKuCtrl != NULL)
	{
		MessageBox("库房控制过程中，无法复位状态");	
		return;
	}	
	
	ResetGlobalCtrlValue();

	Doing2Text("手动·复位状态");
	Sql2Excel(FALSE);
	Sleep(50);
	
	// TODO: 在此添加控件通知处理程序代码
	CString strSqlRst = "UPDATE SetKu SET WorkStatus = 0 WHERE KuStatus = 1 \
			 UPDATE SetProp SET Flag = 0 \
			 DELETE Text";
	m_pCon->Execute(_bstr_t(strSqlRst),NULL,adCmdText);	//150908

	KuCtrlDispLoad();
	YSJCtrlDispLoad();
}
void CCANDlg::OnBnClickedButtonUrgentStop()
{
	if( GetDlgItem(IDC_BUTTON_CTRL_OPEN)->IsWindowEnabled() )
		PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CTRL_OPEN, BN_CLICKED), NULL);
	if( GetDlgItem(IDC_BUTTON_STOP_REF_CTRL)->IsWindowEnabled() )
		PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_STOP_REF_CTRL, BN_CLICKED), NULL);
	Sleep(1000);

	if(g_hKuCtrl == NULL)
	{
		g_pCanDlg->GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(FALSE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(FALSE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(FALSE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(TRUE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(FALSE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_SET)->EnableWindow(FALSE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_CX_TEXT)->EnableWindow(FALSE);
		
		g_bUrgentStop = TRUE;
		g_bStopProp = FALSE;

		Doing2Text("手动点击 - 直接-紧急关机");
		if(!g_bUrgentING)
			UrgentStopExecute(&m_ctrlProp);	
	}
	else
	{
		Auto_UrgentStop("手动点击");
	}

	GetDlgItem(IDC_BUTTON_START_ONEKEY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_TEMP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SUSPEND_TEMP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RESUME_TEMP_CTRL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_ONEKEY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_URGENT_STOP)->EnableWindow(FALSE);
}
void CCANDlg::UpdateRuKuComobox(void)
{
	//更新入库中库房
	CComboBox* pRuKu = (CComboBox*)GetDlgItem(IDC_COMBO_RUKU);
	pRuKu->ResetContent();

	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND RuKuING = 1 ORDER BY KuNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-3.1
	if(!m_pRs->BOF)
	{
		pRuKu->AddString("全部库房");
	}
	while( !m_pRs->adoEOF )
	{
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
		pRuKu->AddString(strKuNum);
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//RS-3.2

	pRuKu->SetCurSel(0);
}
void CCANDlg::OnBnClickedButtonRukuClear()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strRuKuNum = "";
	CComboBox* pRuKu = (CComboBox*)GetDlgItem(IDC_COMBO_RUKU);
	pRuKu->GetWindowTextA(strRuKuNum);

	CString strSqlRuKuClear = "";
	if(strcmp(strRuKuNum, "全部库房") == 0)
	{
		strSqlRuKuClear = "UPDATE SetKu SET RuKuING = 0 WHERE KuStatus = 1";
	}
	else
	{
		int nRuKuNum = atoi(strRuKuNum);
		strSqlRuKuClear.Format("UPDATE SetKu SET RuKuING = 0 WHERE KuStatus = 1 AND KuNum = %d", nRuKuNum);
	}

	m_pCon->Execute(_bstr_t(strSqlRuKuClear),NULL,adCmdText);
	
	UpdateRuKuComobox();
}
//模拟
void CCANDlg::OnBnClickedCheckCanSimulate()
{
	CButton *pCheckCanSim = (CButton *)GetDlgItem(IDC_CHECK_CAN_SIMULATE);
	g_bFlagSimCAN = pCheckCanSim->GetCheck();
}

void CCANDlg::OnBnClickedCheckTempSimulate()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton *pCheckTempSim = (CButton *)GetDlgItem(IDC_CHECK_TEMP_SIMULATE);
	g_bFlagSimTEMP = pCheckTempSim->GetCheck();

	CButton *pCheckTempSimSign = (CButton *)GetDlgItem(IDC_CHECK_TEMP_SIM_SIGN);
	CEdit *pEditTempSimZh = (CEdit *)GetDlgItem(IDC_EDIT_TEMP_SIM_ZH);
	CEdit *pEditTempSimX = (CEdit *)GetDlgItem(IDC_EDIT_TEMP_SIM_X);
	CStatic *pStaticTempSimX = (CStatic *)GetDlgItem(IDC_STATIC_TEMP_SIM_X);
	CStatic *pStaticTempSimL = (CStatic *)GetDlgItem(IDC_STATIC_TEMP_SIM_LOGO);
	CButton *pTempSimOK = (CButton *)GetDlgItem(IDC_BUTTON_TEMP_SIM_OK);

	pCheckTempSimSign->EnableWindow(g_bFlagSimTEMP);
	pEditTempSimZh->EnableWindow(g_bFlagSimTEMP);
	pEditTempSimX->EnableWindow(g_bFlagSimTEMP);
	pStaticTempSimX->EnableWindow(g_bFlagSimTEMP);
	pStaticTempSimL->EnableWindow(g_bFlagSimTEMP);
	pTempSimOK->EnableWindow(g_bFlagSimTEMP);
}

void CCANDlg::OnBnClickedButtonTempSimOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CButton *pCheckTempSimSign = (CButton *)GetDlgItem(IDC_CHECK_TEMP_SIM_SIGN);
	( pCheckTempSimSign->GetCheck() )? g_aTempComRcvData[6] = 1: g_aTempComRcvData[6] = 0;

	g_aTempComRcvData[4] = m_byTempSimZH;
	g_aTempComRcvData[5] = m_byTempSimX;
}
void CCANDlg::OnBnClickedCheckOnOffCycle()
{
	CButton *pCheckOnlyOnOff = (CButton *)GetDlgItem(IDC_CHECK_ON_OFF_CYCLE);
	g_bFlagOnlyOnOff = pCheckOnlyOnOff->GetCheck();
}

//手动微调库号状态
void CCANDlg::UpdateKuManual(void)
{
	//手动微调库房
	CComboBox* pKuNum = (CComboBox*)GetDlgItem(IDC_COMBO_KU_NUM_MANUAL);
	CString strSqlManual = "";
	strSqlManual = "SELECT * FROM SetKu WHERE KuStatus = 1 AND KuNum > 0 ORDER BY KuNum ASC";
	m_pRs_CtrlHand = m_pCon->Execute(_bstr_t(strSqlManual),NULL,adCmdText);	//55.1
	if(!m_pRs_CtrlHand->BOF)
	{
		pKuNum->ResetContent();
	}
	while( !m_pRs_CtrlHand->adoEOF)
	{
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs_CtrlHand->GetCollect("KuNum"));
		pKuNum->AddString(strKuNum);

		m_pRs_CtrlHand->MoveNext();
	}
	m_pRs_CtrlHand->raw_Close();	//55.2

	//手动压缩机151029
	CComboBox* pYSJNum = (CComboBox*)GetDlgItem(IDC_COMBO_YSJ_MANUAL);
	pYSJNum->ResetContent();
	for(int i=1; i<=g_byYSJSum_SetKu; i++)
	{
		CString strYSJ = "";
		strYSJ.Format("%d", i);
		pYSJNum->AddString(strYSJ);
	}
	pYSJNum->SelectString(-1, "1");
	
}

void CCANDlg::OnBnClickedButtonKuOnManual()
{
	BYTE byKuNumManual = -1;
	CComboBox* pKuNum = (CComboBox*)GetDlgItem(IDC_COMBO_KU_NUM_MANUAL);
	CString strKuNumManual = "";
	pKuNum->GetWindowTextA(strKuNumManual);
	byKuNumManual = atoi(strKuNumManual);

	if(byKuNumManual > 0)
	{
		CString strStatusOn = "";
		strStatusOn.Format("UPDATE SetKu SET WorkStatus = 1 WHERE KuNum = %d", byKuNumManual);
		m_pCon->Execute(_bstr_t(strStatusOn),NULL,adCmdText);

		KuWorkStatusListDisp(byKuNumManual, 1, FALSE);

		CString strText = "";
		strText.Format("手动·微调库房状态 %d 开", byKuNumManual);
		Doing2Text(strText);	
	}
	else
	{
		MessageBox("请选择库房号");
	}
}
void CCANDlg::OnBnClickedButtonKuOffManual()
{
	BYTE byKuNumManual = -1;
	CComboBox* pKuNum = (CComboBox*)GetDlgItem(IDC_COMBO_KU_NUM_MANUAL);
	//byKuNumManual = pKuNum->GetCurSel() + 1;
	CString strKuNumManual = "";
	pKuNum->GetWindowTextA(strKuNumManual);
	byKuNumManual = atoi(strKuNumManual);
	
	if(byKuNumManual > 0)
	{
		CString strStatusOn = "";
		strStatusOn.Format("UPDATE SetKu SET WorkStatus = 0 WHERE KuNum = %d", byKuNumManual);
		m_pCon->Execute(_bstr_t(strStatusOn),NULL,adCmdText);

		KuWorkStatusListDisp(byKuNumManual, 0, FALSE);
		CString strText = "";
		strText.Format("手动·微调库房状态 %d 关", byKuNumManual);
		Doing2Text(strText);
	}
	else
	{
		MessageBox("请选择库房号");
		//m_playerCtrl.stop();
	}
}

void CCANDlg::OnBnClickedCheckTempUpCtrl()
{
	CButton* pBtTempUpCtrl = (CButton*)GetDlgItem(IDC_CHECK_TEMP_UP_CTRL);
	CButton* pBtTempDnCtrl = (CButton*)GetDlgItem(IDC_CHECK_TEMP_DN_CTRL);
	CEdit* pEditTempUpCtrl = (CEdit*)GetDlgItem(IDC_EDIT_TEMP_UP_CTRL);
	CButton* pBtTempUpDnOK = (CButton*)GetDlgItem(IDC_BUTTON_TEMP_UP_DN_CTRL);

	if(pBtTempUpCtrl->GetCheck())
	{
		pEditTempUpCtrl->EnableWindow(TRUE);
		pBtTempUpDnOK->EnableWindow(TRUE);
	}
	else
	{
		pEditTempUpCtrl->EnableWindow(FALSE);
		(pBtTempDnCtrl->GetCheck())? pBtTempUpDnOK->EnableWindow(TRUE): pBtTempUpDnOK->EnableWindow(FALSE);
	}
}
void CCANDlg::OnBnClickedCheckTempDnCtrl()
{
	CButton* pBtTempUpCtrl = (CButton*)GetDlgItem(IDC_CHECK_TEMP_UP_CTRL);
	CButton* pBtTempDnCtrl = (CButton*)GetDlgItem(IDC_CHECK_TEMP_DN_CTRL);
	CEdit* pEditTempDnCtrl = (CEdit*)GetDlgItem(IDC_EDIT_TEMP_DN_CTRL);
	CButton* pBtTempUpDnOK = (CButton*)GetDlgItem(IDC_BUTTON_TEMP_UP_DN_CTRL);

	if(pBtTempDnCtrl->GetCheck())
	{
		pEditTempDnCtrl->EnableWindow(TRUE);
		pBtTempUpDnOK->EnableWindow(TRUE);
	}
	else
	{
		pEditTempDnCtrl->EnableWindow(FALSE);
		(pBtTempUpCtrl->GetCheck())? pBtTempUpDnOK->EnableWindow(TRUE): pBtTempUpDnOK->EnableWindow(FALSE);
	}
}

void CCANDlg::OnBnClickedButtonTempUpDnCtrl()
{	//控制过程中手动修改温度上下限
	CButton* pBtTempUpCtrl = (CButton*)GetDlgItem(IDC_CHECK_TEMP_UP_CTRL);
	CButton* pBtTempDnCtrl = (CButton*)GetDlgItem(IDC_CHECK_TEMP_DN_CTRL);

	BYTE byKuNumManual = -1;
	CComboBox* pKuNum = (CComboBox*)GetDlgItem(IDC_COMBO_KU_NUM_MANUAL);
	CString strKuNumManual = "";
	pKuNum->GetWindowTextA(strKuNumManual);
	byKuNumManual = atoi(strKuNumManual);

	if(byKuNumManual < 1)
	{
		MessageBox("请选择库房号");
		pBtTempUpCtrl->SetCheck(FALSE);
		OnBnClickedCheckTempUpCtrl();
		pBtTempDnCtrl->SetCheck(FALSE);
		OnBnClickedCheckTempDnCtrl();
		return;
	}

	CListCtrl* pKuCtrlList = (CListCtrl*)g_pCanDlg->GetDlgItem(IDC_LIST_KU_CTRL);
	float fTempUpOld = 0.0;
	float fTempDnOld = 0.0;
	for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
	{
		CString strKuDispNumStatus = pKuCtrlList->GetItemText(i, 0);
		CString strKuDispNum = strKuDispNumStatus.Left(2);
		int nKuDispNum = atoi(strKuDispNum);
		if(nKuDispNum == byKuNumManual)
		{
			CString strTempUpOld = "";
			CString strTempDnOld = "";
			strTempUpOld = pKuCtrlList->GetItemText(i,5);
			strTempDnOld = pKuCtrlList->GetItemText(i,6);
			fTempUpOld = (float)atof(strTempUpOld);
			fTempDnOld = (float)atof(strTempDnOld);
			break;
		}
	}

	UpdateData(TRUE);

	if( pBtTempUpCtrl->GetCheck() )
	{
		if(m_fTempUpCtrl < fTempDnOld)
		{
			MessageBox("上限小于下限，请先修改下限");
			pBtTempUpCtrl->SetCheck(FALSE);
			OnBnClickedCheckTempUpCtrl();
			pBtTempDnCtrl->SetCheck(FALSE);
			OnBnClickedCheckTempDnCtrl();
			return;
		}
			
		CString strTempUp = "";
		strTempUp.Format("UPDATE SetKu SET [TempUp] = %.1f WHERE (KuNum = %d) AND (KuNum > 0)", m_fTempUpCtrl, byKuNumManual);
		m_pCon->Execute(_bstr_t(strTempUp),NULL,adCmdText);	

		pBtTempUpCtrl->SetCheck(FALSE);
		OnBnClickedCheckTempUpCtrl();

		//查找库号所在结构体
		if(g_pStKuCtrl != NULL)	
		{
			int nKuOnIndex = GetKuIndex_St(byKuNumManual, FALSE);
			if(nKuOnIndex > -1)
			{
				g_pStKuCtrl[nKuOnIndex].fTempUp = m_fTempUpCtrl;
			}			
		}

		for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
		{
			CString strKuDispNumStatus = pKuCtrlList->GetItemText(i, 0);
			CString strKuDispNum = strKuDispNumStatus.Left(2);
			int nKuDispNum = atoi(strKuDispNum);
			if(nKuDispNum == byKuNumManual)
			{
				CString strTempUpNew = "";
				strTempUpNew.Format("%.1f", m_fTempUpCtrl);
				pKuCtrlList->SetItemText( i, 5, LPCTSTR(strTempUpNew) );
				break;
			}
		}

		CString strTempUpText = "";
		strTempUpText.Format("手动·调整库房%d上限：%.1f→%.1f", byKuNumManual, fTempUpOld, m_fTempUpCtrl);
		Doing2Text(strTempUpText);
	}

	if( pBtTempDnCtrl->GetCheck() )
	{
		if(m_fTempDnCtrl > fTempUpOld)
		{
			MessageBox("下限大于上限，请先修改上限");
			pBtTempUpCtrl->SetCheck(FALSE);
			OnBnClickedCheckTempUpCtrl();
			pBtTempDnCtrl->SetCheck(FALSE);
			OnBnClickedCheckTempDnCtrl();
			return;
		}

		CString strTempDn = "";
		strTempDn.Format("UPDATE SetKu SET [TempDn] = %.1f WHERE (KuNum = %d) AND (KuNum > 0)", m_fTempDnCtrl, byKuNumManual);
		m_pCon->Execute(_bstr_t(strTempDn),NULL,adCmdText);

		pBtTempDnCtrl->SetCheck(FALSE);
		OnBnClickedCheckTempDnCtrl();

		//查找库号所在结构体
		if(g_pStKuCtrl != NULL)	
		{
			int nKuOnIndex = GetKuIndex_St(byKuNumManual, FALSE);
			if(nKuOnIndex > -1)
			{
				g_pStKuCtrl[nKuOnIndex].fTempDn = m_fTempDnCtrl;
			}			
		}

		for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
		{
			CString strKuDispNumStatus = pKuCtrlList->GetItemText(i, 0);
			CString strKuDispNum = strKuDispNumStatus.Left(2);
			int nKuDispNum = atoi(strKuDispNum);
			if(nKuDispNum == byKuNumManual)
			{
				CString strTempDnNew = "";
				strTempDnNew.Format("%.1f", m_fTempDnCtrl);
				pKuCtrlList->SetItemText( i, 6, LPCTSTR(strTempDnNew) );
				break;
			}
		}

		CString strTempDnText = "";
		strTempDnText.Format("手动·调整库房%d下限：%.1f→%.1f", byKuNumManual, fTempDnOld, m_fTempDnCtrl);
		Doing2Text(strTempDnText);
	}

	float fTempUpNew = 0.0;
	float fTempDnNew = 0.0;
	for(int i=0; i<pKuCtrlList->GetItemCount(); i++)
	{
		CString strKuDispNumStatus = pKuCtrlList->GetItemText(i, 0);
		CString strKuDispNum = strKuDispNumStatus.Left(2);
		int nKuDispNum = atoi(strKuDispNum);
		if(nKuDispNum == byKuNumManual)
		{
			CString strTempUpNew = "";
			CString strTempDnNew = "";
			strTempUpNew = pKuCtrlList->GetItemText(i,5);
			strTempDnNew = pKuCtrlList->GetItemText(i,6);
			fTempUpNew = (float)atof(strTempUpNew);
			fTempDnNew = (float)atof(strTempDnNew);
			break;
		}
	}
	if(fTempUpNew < fTempDnNew)
	{
		MessageBox("上限小于下限，请修改");
		return;
	}
}
void CCANDlg::OnBnClickedCheckYsjManual()
{
	CButton* pBtCheckYSJ = (CButton*)GetDlgItem(IDC_CHECK_YSJ_MANUAL);
	if(g_bEnKuYSJ)
	{
		CString str = "";
		str.Format("人为干预，自动开关压缩机将失效");
		int nMSGB = AfxMessageBox(str, MB_YESNO);
		if(nMSGB == IDYES)
		{
			Doing2Text(str);
			g_bEnKuYSJ = FALSE;
		}
		else
		{
			pBtCheckYSJ->SetCheck(0);
			return;
		}
	}
	
	CButton* pBtYSJOff = (CButton*)GetDlgItem(IDC_BUTTON_YSJ_OFF_MANUAL);
	CButton* pBtYSJOn = (CButton*)GetDlgItem(IDC_BUTTON_YSJ_ON_MANUAL);
	
	pBtYSJOff->EnableWindow(pBtCheckYSJ->GetCheck());
	pBtYSJOn->EnableWindow(pBtCheckYSJ->GetCheck());
}

void CCANDlg::OnBnClickedButtonYsjOffManual()
{
	if(g_pStYSJ != NULL)
	{
		CComboBox* pYSJNum = (CComboBox*)GetDlgItem(IDC_COMBO_YSJ_MANUAL);
		CString strYSJManual = "";
		pYSJNum->GetWindowTextA(strYSJManual);
		BYTE byYSJManual = atoi(strYSJManual);	
		
		int nYSJIndex = GetYSJIndex_St(byYSJManual);
		if(nYSJIndex > -1)
		{	
			if(g_pStYSJ[nYSJIndex].bNC)
			{
				//151104必须保证至少有一个压缩机是开着的
				int nYSJOn = 0;
				for(int i=0; i<g_byYSJSum_SetKu; i++)
				{
					if(g_pStYSJ[i].bNC)
					{
						nYSJOn++;
					}
				}
				
				if(nYSJOn > 1)
				{
					Doing2Text("手动-关闭压缩机");
					YSJOnOffExe("关机流程", &m_ctrlProp, nYSJIndex);
				}else
				{
					MessageBox("请点击 一键关机");
				}
			}
			else
			{
				MessageBox("该压缩机已关闭");
			}
		}	
	}
	else
	{
		MessageBox("自动控制未启动");
	}
}

void CCANDlg::OnBnClickedButtonYsjOnManual()
{
	if(g_pStYSJ != NULL)
	{
		CComboBox* pYSJNum = (CComboBox*)GetDlgItem(IDC_COMBO_YSJ_MANUAL);
		CString strYSJManual = "";
		pYSJNum->GetWindowTextA(strYSJManual);
		BYTE byYSJManual = atoi(strYSJManual);	
		
		int nYSJIndex = GetYSJIndex_St(byYSJManual);
		if(nYSJIndex > -1)
		{	
			if(g_pStYSJ[nYSJIndex].bNC)
			{
				MessageBox("该压缩机已开启");
			}
			else
			{
				Doing2Text("手动-开启压缩机");
				YSJOnOffExe("开机流程", &m_ctrlProp, nYSJIndex);				
			}
		}	
	}
	else
	{
		MessageBox("自动控制未启动");
	}
}
void CCANDlg::OnBnClickedButtonManualYsjStop()
{	
	g_bStopKuYSJ = TRUE;
	Doing2Text("手动-停止压缩机流程");
}

//音效
void CCANDlg::OnBnClickedButtonWmpStop()
{
	m_playerCtrl.stop();
}
void CCANDlg::OnBnClickedButtonWmpSt()
{
	m_player.put_URL(_T("D:\\CCSP\\ST.mp3"));	//打开并播放
	m_playerCtrl.stop();
	m_playerSet.put_playCount(1);
	m_playerCtrl.play();
}

void CCANDlg::OnBnClickedButtonWmpSp()
{
	m_player.put_URL(_T("D:\\CCSP\\SP.mp3"));	//打开并播放
	m_playerCtrl.stop();
	m_playerSet.put_playCount(1);
	m_playerCtrl.play();
}

void CCANDlg::OnBnClickedButtonWmpUr()
{
	m_player.put_URL(_T("D:\\CCSP\\UR.mp3"));	//打开并播放
	m_playerCtrl.stop();
	m_playerSet.put_playCount(20);
	m_playerCtrl.play();
}


//流程显示
void CCANDlg::PropListInit(void)
{
	CListCtrl* pOnList = (CListCtrl*)GetDlgItem(IDC_LIST_DELAY3);
	pOnList->SetExtendedStyle( pOnList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	pOnList->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_CENTER, 45 );
	pOnList->InsertColumn( 1, LPCTSTR("项目"), LVCFMT_CENTER, 65 );
	pOnList->InsertColumn( 2, LPCTSTR("号"), LVCFMT_CENTER, 30 );
	pOnList->InsertColumn( 3, LPCTSTR("特征"), LVCFMT_CENTER, 65 );
	pOnList->InsertColumn( 4, LPCTSTR("数"), LVCFMT_CENTER, 50 );
	pOnList->InsertColumn( 5, LPCTSTR("分组"), LVCFMT_CENTER, 70 );
	PropListLoad("SetProp", "清空流程");
}

void CCANDlg::PropListLoad(CString strProp, CString strFunc)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_DELAY3);
	pList->DeleteAllItems();

	if(strcmp(strFunc, "清空流程") == 0)
		return;
	
	CString strSql = "";
	(!g_bUrgentStop)?
		strSql.Format("SELECT * FROM %s WHERE Func = '%s' ORDER BY NO ASC", strProp, strFunc):
		strSql.Format("SELECT * FROM %s WHERE Func = '%s' AND Item != '分液' AND Item != '回气' ORDER BY NO ASC", strProp, strFunc);
	m_pRs_PropList = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//149.1	

	int nListEntry = 0;

	while( !m_pRs_PropList->adoEOF )
	{	
		CString strNO = "", strItem = "", strNum = "", strDir = "";
		CString strNC = "", strDWKD = "", strZuItem = "", strZuNum = "", strZu = "";
		
		strNO = (LPCSTR)_bstr_t(m_pRs_PropList->GetCollect("NO"));
		pList->InsertItem( nListEntry, LPCTSTR( strNO ), 0 );

		strItem = (LPCSTR)_bstr_t(m_pRs_PropList->GetCollect("Item"));
		pList->SetItemText( nListEntry, 1, LPCTSTR( strItem ) );
		
		strNum = (LPCSTR)_bstr_t(m_pRs_PropList->GetCollect("Num"));
		if( (strcmp(strNum, "0") == 0) && (strcmp(strItem, "风机") == 0) )
		{
			pList->SetItemText( nListEntry, 2, LPCTSTR( NULL ) );	
		}
		else
		{
			pList->SetItemText( nListEntry, 2, LPCTSTR( strNum ) );
		}

		strDir = (LPCSTR)_bstr_t(m_pRs_PropList->GetCollect("Dir"));
		pList->SetItemText( nListEntry, 3, LPCTSTR( strDir ) );
		
		if( ( !strcmp(strItem, *(aOnItem+1) ) ) ||
			( !strcmp(strItem, *(aOnItem+3) ) ) || 
			( !strcmp(strItem, *(aOnItem+4) ) ) ||
			( !strcmp(strItem, *(aOnItem+10) ) )|| 
			( ( !strcmp(strItem, *(aOnItem+0) ) ) && ( !strcmp(strDir, *(aOnDir+4) ) ) ) )
		{
			strNC = (LPCSTR)_bstr_t(m_pRs_PropList->GetCollect("NC"));
			int nNC = atoi(strNC);
			switch(nNC)
			{
				case 1: pList->SetItemText( nListEntry, 4, LPCTSTR( "开" ) ); break;
				case 0:	pList->SetItemText( nListEntry, 4, LPCTSTR( "关" ) ); break;
				default: break;
			}
		}
		else
		{
			strDWKD = (LPCSTR)_bstr_t(m_pRs_PropList->GetCollect("DWKD"));
			pList->SetItemText( nListEntry, 4, LPCTSTR( strDWKD ) );
		}

		strZuItem = (LPCSTR)_bstr_t(m_pRs_PropList->GetCollect("ZuItem"));
		strZuNum = (LPCSTR)_bstr_t(m_pRs_PropList->GetCollect("ZuNum"));
		strZu = strZuItem + strZuNum;
		pList->SetItemText( nListEntry, 5, LPCTSTR( strZu ) );
		
		nListEntry ++;
		m_pRs_PropList->MoveNext();
	}

	m_pRs_PropList->raw_Close();	//149.2
}
