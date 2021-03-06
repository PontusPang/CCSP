#include "stdafx.h"
#include "CS.h"
#include "DISPDlg.h"
#include "SETDlg.h"
#include "CANDlg.h"
#include "LOGINDlg.h"
#include "CWMPPlayer4.h"
#include "Security.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CCSApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CCSApp::CCSApp()
{
}

/////////////////////////////////////////////////////////////////////////////
//A - CCSP
HANDLE g_hCCSPEvent = NULL;			//程序实例句柄
CCSApp theApp;
CCANDlg *g_pCanDlg = NULL;

//B - 数据库
_ConnectionPtr m_pCon;
_RecordsetPtr m_pRs;				//设置与初始化
_RecordsetPtr m_pRs_KuOffTemp;		//关机库房测温专用
_RecordsetPtr m_pRs_CidProp;		//单个单元流程执行
_RecordsetPtr m_pRs_UrgentProp;		//添加紧急关机流程使用
_RecordsetPtr m_pRs_KuNum;			//筛选某个库
_RecordsetPtr m_pRs_KuProp;			//筛选某个库某个项目的流程，YSJ
_RecordsetPtr m_pRs_AddKuProp;		//添加某个库某个项目的流程
_RecordsetPtr m_pRs_SelectKu;		//筛选几个库
_RecordsetPtr m_pRs_KuStatus;		//库房开关机状态
_RecordsetPtr m_pRs_Msg;			//Msg
_RecordsetPtr m_pRs_Lock;			//Lock
_RecordsetPtr m_pRs_Refer;			//Refer
_RecordsetPtr m_pRs_Timer2;			//Timer2
_RecordsetPtr m_pRs_Delay3;			//Delay3
_RecordsetPtr m_pRs_TempChange;		//温度递变
_RecordsetPtr m_pRs_OneOnOneOff;	//ThreadProc_OneOnOneOff
_RecordsetPtr m_pRs_OneOnOff;		//普通单独开关机执行流程专用
_RecordsetPtr m_pRs_AllOnAllOff;	//ThreadProc_AllOnAllOff专属，执行流程
_RecordsetPtr m_pRs_AllOnAllOff_In;	//ThreadProc_AllOnAllOff专属，内部嵌套使用
_RecordsetPtr m_pRs_Text;			//运行记录导出专用
_RecordsetPtr m_pRs_PropList;		//运行过程PropListCtrl专用
_RecordsetPtr m_pRs_KuCtrl;			//库控线程专用
_RecordsetPtr m_pRs_TempTemp;		//温温专用
_RecordsetPtr m_pRs_TempMax;		//温温专用，同开最多数循环
_RecordsetPtr m_pRs_TimeTemp;		//时温专用
_RecordsetPtr m_pRs_TimeMax;		//时温最多循环专用
_RecordsetPtr m_pRs_TimeTime;		//时时专用
_RecordsetPtr m_pRs_CtrlHand;		//手动控制专用，可能在自动过程中手动
_RecordsetPtr m_pRs_ManualYSJ;		//手动开关压缩机专用
_RecordsetPtr m_pRs_ManualYSJ_In;	//手动开关压缩机专用

//C - 库房设置
//要关联修改↓
//#define NumOnItem			10
//#define NumOnDir			5
//#define NumSetFunc		2
//#define NumCidItem		11
//#define NumKuCtrlMode		3
//#define NumCtrlTimeTemp	4
//#define NumKuWorkMode		3
CString aOnItem[] = {"压缩机", "水泵", "氨阀", "氨泵", "风机", "分液", "回气", "延时", "吸气", "排气", "电磁阀"};	//开机流程选项框
CString aOnDir[] = {"请选择", "档位到", "开度到", "单位秒","选开关"};
CString aSetFunc[] = {"开机流程", "关机流程"};		//多种开关机设置
CString aSetPropZu[] = {"压缩机", "库房", "特殊"};	
CString aCidItem[] = {"压缩机", "氨阀", "分液", "回气", "吸气", "排气", "风机", "水泵", "氨泵", "电磁阀", "氨检"};	//ID选项框
CString aKuCtrlMode[] = {"温开温关", "时开温关", "时开时关"};
CString aCtrlTimeTemp[4] = {"时开温控温关", "时开温控时关", "时到温控温关", "时到温控时关"};
CString aKuWorkMode[] = {"同开同关", "同开异关", "异开异关"};
CString aAFAuto[] = {"不自动", "开自动", "关自动"};

//D - 结构体
Prop_t g_StProp;
CXMsg_t g_StCXMsg;
KuCtrl_t *g_pStKuCtrl = NULL;
TimeCtrl_t *g_pStTimeCtrl = NULL;
AnS_t *g_pStAnS = NULL;
TempSor_t *g_pStTempSor = NULL;
Cid_t *g_pStCid = NULL;
YSJ_t *g_pStYSJ = NULL;
KuYSJ_t *g_pStKuYSJ = NULL;
CS_t *g_pStCS = NULL;
int g_nCS_Index = -1;				//冷库结构体数组序号

//E - 设置
BYTE g_byCAN12ID = 100;				//下位机ID>100，与CAN2相连	151201
BOOL g_bCAN2_InUse = FALSE;			//是否使用CAN2
BYTE g_byKuOnMax_SetKu = 0;			//同开最多库房数
BYTE g_byKuSum_SetKu = 0;			//库房总数
BYTE g_byTempSum_SetKu = 0;			//温度节点总数
BYTE g_byYSJSum_SetKu = 0;			//压缩机总数
BYTE g_byPropSMS_SetKu = 0;			//开关机短信
BYTE g_byKuHour_SetKu = 0;			//温度递变小时
int g_nTimeS_SetKu = 720;			//运行时间保护
BYTE g_byAnsSum_SetKu = 2;			//氨检总数
BYTE g_byCidSum_SetKu = 0;			//节点设置总数（包括延时0123、主机12）

//F - 系统运行权限
COleDateTime g_mTimeNow;
COleDateTime g_mTimeAuthorST;		//系统运行起始时间
int g_nRunningDays = 0;				//系统已运行时间
int g_nAuthorDays = 0;				//用户可运行的时间，=0不能运行，=-1永远
BOOL g_bAuthorAll = FALSE;			//用户是否拥有永久权限，TRUE:永远
BOOL g_bSysCanRun = TRUE;			//系统能否运行，TRUE：能
BOOL g_bFlag8_10 = FALSE;			//8:10的系列操作
int g_nMsgEx = MSG_EX_NO;			//异常短信码，cs.h中定义

//G - 系统模拟
BOOL g_bFlagSimCAN = FALSE;
BOOL g_bFlagSimTEMP = FALSE;
BOOL g_bFlagOnlyOnOff = FALSE;

//H - 遍历的暂停与停止
BOOL g_bWaitRef = FALSE;			//ID遍历线程暂停中
BOOL g_bStopRef = FALSE;			//停止ID遍历-CanDlg

//I - 句柄
HANDLE g_hTimeProtect = NULL;		//开机超时
HANDLE g_hWatch = NULL;				//系统死机
HANDLE g_hPropING = NULL;			//（全部or单独）开关机流程
HANDLE g_hKuCtrl = NULL;			//自动控制
HANDLE g_hKuYSJ = NULL;				//压缩机流程执行线程
HANDLE g_hIDRefer = NULL;			//单元遍历
HANDLE g_hMsg = NULL;				//短信
HANDLE g_hTimer2 = NULL;			//延时2
HANDLE g_hDelay3 = NULL;			//延时3

//J - 自动控制进程
BOOL g_bStopProp = FALSE;			//终止所有开关机流程
BOOL g_bStopKuCtrl = FALSE;			//终止自动控制
BOOL g_bFlagKuCtrlING_Wait = FALSE;	//用于暂停、恢复库房控制线程
BOOL g_bFlagPropING_Wait = FALSE;	//151116用于暂停、恢复流程执行线程
BOOL g_bUrgentStop = FALSE;			//紧急关机标志
BOOL g_bUrgentING = FALSE;			//紧急关机正在执行中
BOOL g_bUrgentJump = FALSE;			//紧急关机过程中，压缩机关或吸气0跳过，则水泵和排气不关
BOOL g_bOneKeyStart = FALSE;		//直接开启压缩机
BOOL g_bOneKeyStop = FALSE;			//强制关机
BOOL g_bStopDelay3 = FALSE;			//延时3线程停止

int g_nCtrlMode = 0;				//控制模式总数
BYTE g_byTimeTempSum = 0;			//多控制，时开温关库房总数150807
BYTE g_byTempTempSum = 0;			//多控制，温开温关库房总数150807
BYTE *g_aKuNumTimeTemp = NULL;		//存储，时开温关，库房数
BYTE *g_aKuNumTempTemp = NULL;		//存储，温开温关，库房数

BYTE g_byKuCtrlSum_St = 0;			//自动控制在用库房-总数
BYTE g_byKuOnSum = 0;				//自动控制过程中，库房开总数
BYTE g_byYSJOnSum = 0;				//自动控制过程中，压缩机开总数

BOOL g_bInTimer2 = FALSE;			//处于延时2温控中
BOOL g_bInDelay3 = FALSE;			//150915处于延时3温控中
int g_nFirstDelay3S = -12345;		//首个延时3需要的时间
BOOL g_bInKuTempChange = FALSE;		//处于温度递变中，防止多次变化
BOOL g_bKuLinkYSJ = FALSE;			//151116有库房关联压缩机的情况

//K - 时开温关
BOOL g_bOnProp_MaxOn = FALSE;		//同开为TRUE,异开为FALSE
BOOL g_bTimeSt_SetKu = FALSE;		//时开温关，始时开机=TRUE
BOOL g_bAllOffJump_SetKu = FALSE;	//时开温关，全关跳过=TRUE
BOOL g_bAllOffDo_SetKu = FALSE;		//时开温关，全关即止=TRUE;全关等开=FALSE;
BOOL g_bTimeOut_SetKu = FALSE;		//时开温关，超时即关=TRUE

BOOL g_bFlagInSpan = FALSE;			//成功查询当前时间位于哪个峰谷时间段内
BOOL g_bFlagAllKuOff = TRUE;		//所有库房已关机
BOOL g_bFlagAllYSJOff = TRUE;		//所有压缩机开关状态，执行完毕开机流程FALSE，执行完毕关机流程TRUE
BOOL g_bAllKuDoOnce = FALSE;		//时间到达，开机之后，进入温控--所有的库房都遍历一遍了
BOOL g_bKuOnMaxWhile = FALSE;		//进入同开最多的内部循环
BOOL g_bOffAfterCtrl = FALSE;		//执行完一遍关机TRUE后的继续温控
BOOL g_bStopAuto = FALSE;			//时开温关，停止温控

BYTE g_byTimeCtrlSum_St = 0;		//峰谷用电时间段总数
CString g_strNowTimeKu = "全部库房";

//L - 系统稳定性
BOOL g_bTimeProtectOUT = FALSE;		//开机超时保护退出
BOOL g_bFlagKuCtrlING_Watch = FALSE;//死机保护，KuCtrl进行中

//K - 压缩机
BOOL g_bEnKuYSJ = FALSE;			//使能库房控制压缩机
BOOL g_bStopKuYSJ = FALSE;			//压缩机流程停止

/////////////////////////////////////////////////////////////////////////////
// CCSApp initialization
BOOL CCSApp::InitInstance()
{
	//保证只有一个实例存在
	g_hCCSPEvent = CreateEvent(NULL, FALSE, FALSE, "CCSP");	//命名的实例
	if(g_hCCSPEvent)
	{
		if(ERROR_ALREADY_EXISTS == GetLastError())
		{
			return FALSE;
		}
	}

	g_nCS_Index = GetCS_Index();
	if(g_nCS_Index < 0)	
	{
		if(g_pStCS != NULL)
		{
			delete [] g_pStCS;
			g_pStCS = NULL;
		}
		return FALSE;
	}

	if(LOGIN_EN)
	{
		CLOGINDlg dlgLogin;
		BOOL bFlagLogin = dlgLogin.Login(dlgLogin);
		if(!bFlagLogin)
			return FALSE;	
	}

	m_pCon = NULL;
	AfxOleInit();  //OLE 初始化，启动COM组件初始化
	if( !AdoInit() )
		return FALSE;

	//全局变量赋初值
	g_StProp.Func = "";
	g_StProp.pEdit = NULL;
	g_StProp.byKuNum = 0;
	g_StProp.strSqlProp = "SetProp";
	g_StProp.strKuNum = "全部库房";	//赋初值

	memset(g_StCXMsg.YSJ, 0, KU_YSJ);
	for(int i=0; i<12; i++)
	{
		g_StCXMsg.Ku[i] = (float)88.8;
	}
	for(int i=12; i<22; i++)
	{
		g_StCXMsg.Ku[i] = (float)-88.8;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	//Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	//Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//获取总数
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum = 0";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-5.1
	if(!m_pRs->BOF)
	{
		CString strTimeS = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempNum"));
		CString strKuSum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuYSJ"));
		CString strKuOnMax = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuHQ"));
		CString strKuTempSum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFY"));
		CString strAnsSum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFJ"));
		CString strPropSMS =  (LPCSTR)_bstr_t(m_pRs->GetCollect("RuKuING"));
		g_byKuSum_SetKu = atoi(strKuSum);
		g_byKuOnMax_SetKu = atoi(strKuOnMax);
		g_byTempSum_SetKu = atoi(strKuTempSum);
		g_byAnsSum_SetKu = atoi(strAnsSum);
		g_nTimeS_SetKu = atoi(strTimeS);
		g_byPropSMS_SetKu = atoi(strPropSMS);

		CString strKuHour = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuCtrlMode"));
		g_byKuHour_SetKu = atoi(strKuHour);
		if( (g_byKuHour_SetKu > 23) || (g_byKuHour_SetKu < 0) )
		{
			AfxMessageBox("温度递变小时 0~23");
		}

		CString strAuthorST = (LPCSTR)_bstr_t(m_pRs->GetCollect("ChangeST")); 
		g_mTimeAuthorST.ParseDateTime(strAuthorST);
		CString strRunningDays = (LPCSTR)_bstr_t(m_pRs->GetCollect("ChangeDays")); 
		g_nRunningDays = atoi(strRunningDays);
		CString strAuthorDays = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuStore")); 
		g_nAuthorDays = atoi(strAuthorDays);
		(g_nAuthorDays == -1)? g_bAuthorAll = TRUE: g_bAuthorAll = FALSE;
		if(!g_bAuthorAll)
		{
			if(g_nAuthorDays == 0)
			{
				g_bSysCanRun = FALSE;
			}
			else
			{
				(g_nRunningDays > g_nAuthorDays)? g_bSysCanRun = FALSE: g_bSysCanRun = TRUE;
			}		
		}
		else
		{
			g_bSysCanRun = TRUE;
		}

		if(g_byKuSum_SetKu < g_byKuOnMax_SetKu)
		{
			AfxMessageBox("库房总数 不应小于 同开最多库房数");
		}

		m_pRs->raw_Close();	//RS-5.2.1
	}
	else
	{
		m_pRs->raw_Close();	//RS-5.2.2

		AfxMessageBox("获取总数失败，请检查数据库库房设置");
		return FALSE;
	}

	strSql = "SELECT * FROM SetYSJ WHERE YSJ > 0";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-6.1
	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{
		nListEntry++;
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//RS-6.2
	g_byYSJSum_SetKu = nListEntry;

	CCANDlg dlg;
	//CSETDlg dlg;
	//CDISPDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{
		
	}

	return FALSE;
}

//
int CCSApp::AdoInit()
{
	//先释放未关闭的结果集和连接
	if(m_pRs != NULL)
	{
		if(m_pRs->State != adStateClosed)
		{
			m_pRs->Close();
			m_pRs.Release();
		}
	}

	if(m_pRs_KuNum != NULL)
	{
		if(m_pRs_KuNum->State != adStateClosed)
		{
			m_pRs_KuNum->Close();
			m_pRs_KuNum.Release();
		}
	}

	if(m_pRs_KuProp != NULL)
	{
		if(m_pRs_KuProp->State != adStateClosed)
		{
			m_pRs_KuProp->Close();
			m_pRs_KuProp.Release();
		}
	}

	if(m_pRs_SelectKu != NULL)
	{
		if(m_pRs_SelectKu->State != adStateClosed)
		{
			m_pRs_SelectKu->Close();
			m_pRs_SelectKu.Release();
		}
	}

	if(m_pRs_Msg != NULL)
	{
		if(m_pRs_Msg->State != adStateClosed)
		{
			m_pRs_Msg->Close();
			m_pRs_Msg.Release();
		}
	}

	if(m_pRs_Lock != NULL)
	{
		if(m_pRs_Lock->State != adStateClosed)
		{
			m_pRs_Lock->Close();
			m_pRs_Lock.Release();
		}
	}

	if(m_pRs_Refer != NULL)
	{
		if(m_pRs_Refer->State != adStateClosed)
		{
			m_pRs_Refer->Close();
			m_pRs_Refer.Release();
		}
	}

	if(m_pRs_AddKuProp != NULL)
	{
		if(m_pRs_AddKuProp->State != adStateClosed)
		{
			m_pRs_AddKuProp->Close();
			m_pRs_AddKuProp.Release();
		}
	}

	if(m_pRs_Timer2 != NULL)
	{
		if(m_pRs_Timer2->State != adStateClosed)
		{
			m_pRs_Timer2->Close();
			m_pRs_Timer2.Release();
		}
	}

	if(m_pRs_OneOnOneOff != NULL)
	{
		if(m_pRs_OneOnOneOff->State != adStateClosed)
		{
			m_pRs_OneOnOneOff->Close();
			m_pRs_OneOnOneOff.Release();
		}
	}

	if(m_pRs_OneOnOff != NULL)
	{
		if(m_pRs_OneOnOff->State != adStateClosed)
		{
			m_pRs_OneOnOff->Close();
			m_pRs_OneOnOff.Release();
		}
	}

	if(m_pRs_TempChange != NULL)
	{
		if(m_pRs_TempChange->State != adStateClosed)
		{
			m_pRs_TempChange->Close();
			m_pRs_TempChange.Release();
		}
	}

	if(m_pRs_AllOnAllOff != NULL)
	{
		if(m_pRs_AllOnAllOff->State != adStateClosed)
		{
			m_pRs_AllOnAllOff->Close();
			m_pRs_AllOnAllOff.Release();
		}
	}

	if(m_pRs_AllOnAllOff_In != NULL)
	{
		if(m_pRs_AllOnAllOff_In->State != adStateClosed)
		{
			m_pRs_AllOnAllOff_In->Close();
			m_pRs_AllOnAllOff_In.Release();
		}
	}

	if(m_pRs_Text != NULL)
	{
		if(m_pRs_Text->State != adStateClosed)
		{
			m_pRs_Text->Close();
			m_pRs_Text.Release();
		}
	}

	if(m_pRs_Delay3 != NULL)
	{
		if(m_pRs_Delay3->State != adStateClosed)
		{
			m_pRs_Delay3->Close();
			m_pRs_Delay3.Release();
		}
	}

	if(m_pRs_PropList != NULL)
	{
		if(m_pRs_PropList->State != adStateClosed)
		{
			m_pRs_PropList->Close();
			m_pRs_PropList.Release();
		}
	}

	if(m_pRs_KuCtrl != NULL)
	{
		if(m_pRs_KuCtrl->State != adStateClosed)
		{
			m_pRs_KuCtrl->Close();
			m_pRs_KuCtrl.Release();
		}
	}

	if(m_pRs_TempTemp != NULL)
	{
		if(m_pRs_TempTemp->State != adStateClosed)
		{
			m_pRs_TempTemp->Close();
			m_pRs_TempTemp.Release();
		}
	}

	if(m_pRs_TempMax != NULL)
	{
		if(m_pRs_TempMax->State != adStateClosed)
		{
			m_pRs_TempMax->Close();
			m_pRs_TempMax.Release();
		}
	}

	if(m_pRs_TimeTemp != NULL)
	{
		if(m_pRs_TimeTemp->State != adStateClosed)
		{
			m_pRs_TimeTemp->Close();
			m_pRs_TimeTemp.Release();
		}
	}

	if(m_pRs_TimeMax != NULL)
	{
		if(m_pRs_TimeMax->State != adStateClosed)
		{
			m_pRs_TimeMax->Close();
			m_pRs_TimeMax.Release();
		}
	}

	if(m_pRs_KuOffTemp != NULL)
	{
		if(m_pRs_KuOffTemp->State != adStateClosed)
		{
			m_pRs_KuOffTemp->Close();
			m_pRs_KuOffTemp.Release();
		}
	}

	if(m_pRs_TimeTime != NULL)
	{
		if(m_pRs_TimeTime->State != adStateClosed)
		{
			m_pRs_TimeTime->Close();
			m_pRs_TimeTime.Release();
		}
	}

	if(m_pRs_CtrlHand != NULL)
	{
		if(m_pRs_CtrlHand->State != adStateClosed)
		{
			m_pRs_CtrlHand->Close();
			m_pRs_CtrlHand.Release();
		}
	}

	if(m_pRs_CidProp != NULL)
	{
		if(m_pRs_CidProp->State != adStateClosed)
		{
			m_pRs_CidProp->Close();
			m_pRs_CidProp.Release();
		}
	}

	if(m_pRs_UrgentProp != NULL)
	{
		if(m_pRs_UrgentProp->State != adStateClosed)
		{
			m_pRs_UrgentProp->Close();
			m_pRs_UrgentProp.Release();
		}
	}

	if(m_pRs_KuStatus != NULL)
	{
		if(m_pRs_KuStatus->State != adStateClosed)
		{
			m_pRs_KuStatus->Close();
			m_pRs_KuStatus.Release();
		}
	}

	if(m_pRs_ManualYSJ != NULL)
	{
		if(m_pRs_ManualYSJ->State != adStateClosed)
		{
			m_pRs_ManualYSJ->Close();
			m_pRs_ManualYSJ.Release();
		}
	}

	if(m_pRs_ManualYSJ_In != NULL)
	{
		if(m_pRs_ManualYSJ_In->State != adStateClosed)
		{
			m_pRs_ManualYSJ_In->Close();
			m_pRs_ManualYSJ_In.Release();
		}
	}

	if(m_pCon != NULL)
	{
		if(m_pCon->State)
		{
			m_pCon->Close();
			m_pCon.Release();
		}
	}

	CString strAdoConn = "";	//ADO连接串
	strAdoConn.Format("driver={SQL Server};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s", "127.0.0.1", "sa", "120614", g_pStCS[g_nCS_Index].strCS_SQL);
	//建立连接后面直接利用了连接对象的Execute方法执行SQL命令
	try
	{		
		m_pCon.CreateInstance(_uuidof(Connection));
		m_pCon->CursorLocation =adUseClient;
		m_pCon->ConnectionString = (_bstr_t)strAdoConn;
		m_pCon->Open("","","",-1);
		m_pRs.CreateInstance(_uuidof(Recordset));//
		m_pRs_Msg.CreateInstance(_uuidof(Recordset));//
		m_pRs_Lock.CreateInstance(_uuidof(Recordset));//
		m_pRs_Refer.CreateInstance(_uuidof(Recordset));//
		m_pRs_AddKuProp.CreateInstance(_uuidof(Recordset));//
		m_pRs_Timer2.CreateInstance(_uuidof(Recordset));//
		m_pRs_OneOnOneOff.CreateInstance(_uuidof(Recordset));//
		m_pRs_OneOnOff.CreateInstance(_uuidof(Recordset));
		m_pRs_TempChange.CreateInstance(_uuidof(Recordset));//
		m_pRs_AllOnAllOff.CreateInstance(_uuidof(Recordset));//
		m_pRs_AllOnAllOff_In.CreateInstance(_uuidof(Recordset));//
		m_pRs_Text.CreateInstance(_uuidof(Recordset));//
		m_pRs_Delay3.CreateInstance(_uuidof(Recordset));//
		m_pRs_PropList.CreateInstance(_uuidof(Recordset));//
		m_pRs_KuCtrl.CreateInstance(_uuidof(Recordset));//
		m_pRs_TempTemp.CreateInstance(_uuidof(Recordset));//
		m_pRs_TempMax.CreateInstance(_uuidof(Recordset));//
		m_pRs_TimeTemp.CreateInstance(_uuidof(Recordset));//
		m_pRs_TimeTime.CreateInstance(_uuidof(Recordset));//
		m_pRs_TimeMax.CreateInstance(_uuidof(Recordset));//
		m_pRs_CtrlHand.CreateInstance(_uuidof(Recordset));//
		m_pRs_CidProp.CreateInstance(_uuidof(Recordset));//
		m_pRs_UrgentProp.CreateInstance(_uuidof(Recordset));//
		m_pRs_KuNum.CreateInstance(_uuidof(Recordset));//
		m_pRs_KuProp.CreateInstance(_uuidof(Recordset));//
		m_pRs_SelectKu.CreateInstance(_uuidof(Recordset));//
		m_pRs_KuOffTemp.CreateInstance(_uuidof(Recordset));//
		m_pRs_KuStatus.CreateInstance(_uuidof(Recordset));//
		m_pRs_ManualYSJ.CreateInstance(_uuidof(Recordset));//
		m_pRs_ManualYSJ_In.CreateInstance(_uuidof(Recordset));//
	}
	catch(_com_error)
	{
		AfxMessageBox("连接未启动，请 检查连接 或 5分钟后重试");
		PostQuitMessage(WM_CLOSE);
		return 0;
	}
	catch(...)
	{
		AfxMessageBox("连接未启动，请 重启 电脑");
		return 0;
	}	

	return 1;
}

//
char HexChar(char c)
{
	if((c>='0')&&(c<='9'))
		return c-0x30;
	else if((c>='A')&&(c<='F'))
		return c-'A'+10;
	else if((c>='a')&&(c<='f'))
		return c-'a'+10;
	else 
		return 0x10;
}

//以空格或冒号分割，字符串转化为十六进制
int Str2Hex(CString str, char* data)
{
	int t,t1;
	int rlen=0,len=str.GetLength();
	//data.SetSize(len/2);
	for(int i=0;i<len;)
	{
		char l,h=str[i];
		if( (h==' ') || (h == ':') )
		{
			i++;
			continue;
		}
		i++;
		if(i>=len)
			break;
		l=str[i];
		t=HexChar(h);
		t1=HexChar(l);
		if((t==16)||(t1==16))
			break;
		else 
			t=t*16+t1;
		i++;
		data[rlen]=(char)t;
		rlen++;
	}
	return rlen;
}

int CharDec(char c)	//oxff
{
	int dec = 0;
	dec = (c >> 4)*10 + (c & 0x0F);
	return dec;
}

//以"英文，"分割的字符串转化为十进制数组
int Str2Dec(CString all, BYTE* data)
{
   int pos=all.Find(",",0);						//,的位置
   CString tem;
   int i = 0;									//个数
   if(pos == -1)
   {
	   data[0] = atoi(all);
	   return 1;
   }
   else
   {
	   while(pos!=-1)
	   {
			tem=all.Left(pos);						//提取 左边 pos个 字节
			all=all.Right(all.GetLength()-pos-1);
	      
			data[i]=atoi(tem);
			i++;

			pos=all.Find(",",0);	
			if(pos == -1)
			{
				data[i] = atoi(all);
				i++;
			}
	   }  
   }

   return i;
}

//以"英文，"分割的字符串转化为十进制数组
int Time2Dec(CString all, BYTE* data)
{
	int pos=all.Find(":",0);						//,的位置
   CString tem;
   int i = 0;									//个数
   if(pos == -1)
   {
	   data[0] = atoi(all);
	   return 1;
   }
   else
   {
	   while(pos!=-1)
	   {
			tem=all.Left(pos);						//提取 左边 pos个 字节
			all=all.Right(all.GetLength()-pos-1);
	      
			data[i]=atoi(tem);
			i++;

			pos=all.Find(",",0);	
			if(pos == -1)
			{
				data[i] = atoi(all);
				i++;
			}
	   }  
   }

   return i;
}
int Priority2Dec(CString all, BYTE* data)
{
   int pos=all.Find(">",0);						//,的位置
   CString tem;
   int i = 0;									//个数
   if(pos == -1)
   {
	   data[0] = atoi(all);
	   return 1;
   }
   else
   {
	   while(pos!=-1)
	   {
			tem=all.Left(pos);						//提取 左边 pos个 字节
			all=all.Right(all.GetLength()-pos-1);
	      
			data[i]=atoi(tem);
			i++;

			pos=all.Find(">",0);	
			if(pos == -1)
			{
				data[i] = atoi(all);
				i++;
			}
	   }  
   }

   return i;
}
//
void Doing2Text(CString strSQLText)
{
	CString strSqlTextTem = "";
	strSqlTextTem.Format("INSERT INTO Text (Text) VALUES ('%s')", strSQLText);
	m_pCon->Execute(_bstr_t(strSqlTextTem),NULL,adCmdText);
}
void Sql2Excel(BOOL bVisible)
{
	KillProcessFromName(_T("EXCEL.EXE"));

	if(!g_bAuthorAll)
	{
		CString strText = "";
		strText.Format("使用权限%d天，已运行%d天", g_nAuthorDays, g_nRunningDays);
		Doing2Text(strText);
	}
	//150914
	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->SetDlgItemTextA(IDC_EDIT_PROP, "运行记录备份中，请等待 最多 2分钟……");
	}
	Doing2Text("导出至 D:\\CCSP\\运行记录");
	
	_Application ExcelApp; 
	Workbooks wbsMyBooks; 
	_Workbook wbMyBook; 
	Worksheets wssMysheets; 
	_Worksheet wsMysheet; 
	Range rgMyRge; 

	if (!ExcelApp.CreateDispatch("Excel.Application",NULL)) 
	{ 
		AfxMessageBox("创建Excel服务失败!"); 
		exit(1); 
	} 
	//利用模板文件建立新文档
	wbsMyBooks.AttachDispatch(ExcelApp.GetWorkbooks(),true); 
	wbMyBook.AttachDispatch(wbsMyBooks.Add(_variant_t("D:\\CCSP\\text.xls"))); 
	//得到Worksheets 
	wssMysheets.AttachDispatch(wbMyBook.GetWorksheets(),true); 
	//得到sheet1 
	wsMysheet.AttachDispatch(wssMysheets.GetItem(_variant_t("sheet1")),true); 

	//设置列宽
	rgMyRge.AttachDispatch(wsMysheet.GetColumns(),true); 
	rgMyRge.AttachDispatch(rgMyRge.GetItem(_variant_t((long)1),vtMissing).pdispVal,true); 
	rgMyRge.SetColumnWidth(_variant_t((long)5)); 
	rgMyRge.AttachDispatch(rgMyRge.GetItem(_variant_t((long)2),vtMissing).pdispVal,true); 
	rgMyRge.SetColumnWidth(_variant_t((long)20)); 
	rgMyRge.AttachDispatch(rgMyRge.GetItem(_variant_t((long)2),vtMissing).pdispVal,true); 
	rgMyRge.SetColumnWidth(_variant_t((long)100));

	//得到全部Cells，此时,rgMyRge是cells的集合 
	rgMyRge.AttachDispatch(wsMysheet.GetCells(),true); 
	//设置1行1列的单元的值 
	rgMyRge.SetItem(_variant_t((long)1),_variant_t((long)1),_variant_t("序号")); 
	rgMyRge.SetItem(_variant_t((long)1),_variant_t((long)2),_variant_t("时间")); 
	rgMyRge.SetItem(_variant_t((long)1),_variant_t((long)3),_variant_t("运行记录")); 

	//将数据库导出至Excel
	CString strSqlText = "SELECT * FROM Text ORDER BY ID ASC";
	m_pRs_Text->Open((_variant_t)strSqlText,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//143.1

	//150914--最多备份近期50000行
	int nListEntry = 0;
	int nFirstID = 0;
	while( !m_pRs_Text->adoEOF )
	{
		if(nListEntry == 0)
		{
			CString strFirstID = (LPCSTR)_bstr_t(m_pRs_Text->GetCollect("ID"));
			nFirstID = atoi(strFirstID);
		}
		nListEntry ++;
		m_pRs_Text->MoveNext();
	}
	m_pRs_Text->raw_Close();	//143.2
	if(nListEntry > 50000)
	{
		strSqlText.Format("DELETE FROM Text WHERE ID < %d", nFirstID+nListEntry-50000);
		m_pCon->Execute(_bstr_t(strSqlText),NULL,adCmdText);
		Doing2Text("最多备份近期50000行");
	}
	
	strSqlText = "SELECT * FROM Text ORDER BY ID ASC";
	m_pRs_Text->Open((_variant_t)strSqlText,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//144.1
	nListEntry = 2;
	while( ! m_pRs_Text->adoEOF )
	{
		CString strTem = "";
		strTem.Format("%d", nListEntry-1);
		rgMyRge.SetItem(_variant_t((long)nListEntry),_variant_t((long)1),_variant_t(strTem));
		
		for (int i=1; i<m_pRs_Text->GetFields()->Count; i++)//150825
		{
			strTem = (TCHAR *)(_bstr_t)m_pRs_Text->GetFields()->GetItem((long)i)->Value;
			rgMyRge.SetItem(_variant_t((long)nListEntry),_variant_t((long)i+1),_variant_t(strTem));//150825
		}
		
		nListEntry ++;
		m_pRs_Text->MoveNext();
	}	
	m_pRs_Text->raw_Close();	//144.2
	
	//150914
	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->SetDlgItemTextA(IDC_EDIT_PROP, "运行记录管理完毕，可进行其他操作");
	}

	CString strTimeNow = g_mTimeNow.Format("%Y-%m-%d %H.%M.%S");
	CString strFilePath = "D:\\CCSP\\运行记录\\" + strTimeNow + ".xls";
	
	//covOptional 可选参数的VARIANT类型  
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	wbMyBook.SaveAs(COleVariant(strFilePath),covOptional,covOptional,covOptional,  
		covOptional,covOptional,(long)0, covOptional,covOptional, 
		covOptional,covOptional); 
	wbMyBook.SetSaved(true); 

	//打印预览 
	bVisible? ExcelApp.SetVisible(TRUE): ExcelApp.SetVisible(FALSE);
	ExcelApp.SetUserControl(TRUE);

	//释放对象 
	rgMyRge.ReleaseDispatch(); 
	wsMysheet.ReleaseDispatch(); 
	wssMysheets.ReleaseDispatch(); 
	wbMyBook.ReleaseDispatch();
	wbsMyBooks.ReleaseDispatch();

	if(!bVisible) ExcelApp.Quit();
	ExcelApp.ReleaseDispatch();

	if(!bVisible) KillProcessFromName(_T("EXCEL.EXE"));
}

inline BOOL EnableDebugPrivilege(BOOL fEnable)
{ 
	BOOL fOK = FALSE; 
	HANDLE hToken = NULL; 
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
	{ 
		TOKEN_PRIVILEGES tp; 
		tp.PrivilegeCount = 1; 
		LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid); 
		tp.Privileges[0].Attributes	= fEnable ? SE_PRIVILEGE_ENABLED : 0; 
		AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL); 
		fOK	= (GetLastError()==ERROR_SUCCESS); 
		CloseHandle(hToken); 
	} 
	return fOK; 
}  

void KillProcessFromName(LPCTSTR name)//name为你要终止的进程的名称 
{ 
	EnableDebugPrivilege(TRUE);
 
    PROCESSENTRY32 pe;//定义一个PROCESSENTRY32结类型的变量 
    pe.dwSize=sizeof(PROCESSENTRY32);//一定要先为dwSize赋值 
    HANDLE hShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);// 创建快照句柄 
    if (Process32First(hShot,&pe)) 
    { 
        DWORD dwMyProcessId = ::GetCurrentProcessId();
        while(Process32Next(hShot,&pe))
        { 
            if ( (_stricmp(pe.szExeFile,name)==0)  && (dwMyProcessId != pe.th32ProcessID  ) ) //判断此进程是否为你要终止的进程 
            {
                HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe.th32ProcessID);//如果是就利用其ID获得句柄 
                if( hProcess )
                {
                    TerminateProcess(hProcess,0);//终止该进程 
                }
            }
        } 
    } 
    CloseHandle(hShot);//Close 
}
//
void ResetGlobalCtrlValue()
{
	g_bStopProp = FALSE;			//终止所有开关机流程
	g_bStopKuCtrl = FALSE;			//终止自动控制
	g_bUrgentStop = FALSE;			//紧急关机标志
	g_bUrgentING = FALSE;			//紧急关机正在执行中
	g_bUrgentJump = FALSE;
	g_bOneKeyStop = FALSE;			//强制关机
	g_bStopDelay3 = FALSE;			//延时3线程停止

	g_byKuOnSum = 0;				//自动控制过程中，库房开总数
	g_byYSJOnSum = 0;				//自动控制过程中，压缩机开总数

	g_bInTimer2 = FALSE;			//处于延时2温控中
	g_bInDelay3 = FALSE;			//150915处于延时3温控中

	g_nFirstDelay3S = -12345;		//首个延时3需要的时间
	g_bFlagKuCtrlING_Wait = FALSE;	//用于暂停、恢复库房控制线程
	g_bFlagPropING_Wait = FALSE;

	g_bFlagInSpan = FALSE;			//成功查询当前时间位于哪个峰谷时间段内
	g_bFlagAllKuOff = TRUE;			//所有库房已关机
	g_bFlagAllYSJOff = TRUE;		//所有压缩机开关状态，执行完毕开机流程FALSE，执行完毕关机流程TRUE
	g_bAllKuDoOnce = FALSE;			//时间到达，开机之后，进入温控--所有的库房都遍历一遍了
	g_bKuOnMaxWhile = FALSE;		//进入同开最多的内部循环
	g_bOffAfterCtrl = FALSE;		//执行完一遍关机TRUE后的继续温控
	g_bStopAuto = FALSE;			//时开温关，停止温控

	g_bTimeProtectOUT = FALSE;		//开机超时保护退出
	g_bFlagKuCtrlING_Watch = FALSE;	//死机保护，KuCtrl进行中

	g_bEnKuYSJ = FALSE;				//使能库房控制压缩机
	g_bStopKuYSJ = FALSE;			//压缩机流程停止
	g_bKuLinkYSJ = FALSE;			//151116有库房关联压缩机的情况

	g_hTimeProtect = NULL;			//开机超时
	g_hWatch = NULL;				//系统死机
	g_hPropING = NULL;				//（全部or单独）开关机流程
	g_hKuYSJ = NULL;				//压缩机流程执行线程
	g_hTimer2 = NULL;				//延时2
	g_hDelay3 = NULL;				//延时3
	g_hMsg = NULL;					//短信
}