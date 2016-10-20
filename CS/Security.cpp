#include "stdafx.h"
#include "CS.h"
#include "CANDlg.h"
#include "SetDlgFGYSJ.h"
#include "ThreadProp_TimeTime.h"
#include "ThreadProp.h"
#include "GetHDSerial.h"

#include "Security.h"
#include <ATLComTime.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* CRC 高位字节值表*/
unsigned char const auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* CRC 低位字节值表*/
unsigned char const auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

char g_aAnsComSendData[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x04, 0x44, 0x09};	//两路
CString g_strAnsRcvUp3 = "";

extern _ConnectionPtr m_pCon;	//ADO连接对象
extern _RecordsetPtr m_pRs;		//ADO记录集对象
extern _RecordsetPtr m_pRs_Msg;	//Msg
extern _RecordsetPtr m_pRs_Lock;	//Lock

extern BYTE		g_RcvData[8];
extern BOOL		g_bOpened_CAN1;

extern BOOL g_bUrgentStop;		//紧急关机标志
extern BOOL g_bUrgentING;		//紧急关机正在执行中
extern BOOL g_bUrgentJump;
extern BOOL g_bStopProp;
extern BOOL g_bStopKuCtrl;
extern BOOL g_bOneKeyStop;
extern BOOL g_bStopRef;

extern CCANDlg *g_pCanDlg;

extern int g_nMsgEx;	//异常短信码，cs.h中定义
extern HANDLE g_hMsg;
int g_nIDEx = 0;			//上报异常的节点ID

extern AnS_t *g_pStAnS;
extern TempSor_t *g_pStTempSor;
extern CXMsg_t g_StCXMsg;

extern BYTE g_byAnsSum_SetKu;
extern BYTE g_byTempSum_SetKu;		//温度节点总数
extern BYTE g_byYSJSum_SetKu;		//压缩机总数
extern BYTE g_byKuSum_SetKu;		//库房总数
extern BOOL g_bFlagTempReferING;
extern BOOL g_bFlagSimTEMP;

extern COleDateTime g_mTimeNow;
extern COleDateTime g_mTimeAuthorST;	//系统起始时间
extern int g_nRunningDays;		//系统已运行时间
extern int g_nAuthorDays;		//用户可运行的时间，=0不能运行，=-1永远
extern BOOL g_bAuthorAll;		//用户是否拥有全部权限，TRUE:永远
extern BOOL g_bSysCanRun;		//系统能否运行，TRUE：能

extern BOOL g_bTimeProtectOUT;		//定时保护中断
extern int g_nTimeS_SetKu;

HANDLE g_hTimerTime = NULL;
extern HANDLE g_hTimeProtect;
extern HANDLE g_hKuCtrl;
extern HANDLE g_hPropING;
extern HANDLE g_hWatch;

extern BOOL g_bFlag8_10;

extern BOOL g_bFlagKuCtrlING_Watch;	//自动控制进行中
int g_nInvalidReadWatch = 0;
COleDateTime g_mTimeWatch; //系统起始时间

extern CS_t *g_pStCS;
//识别机器码
DWORD GetVolumeSerial()	//磁盘序列号
{
	DWORD	dwVolumeSerialNumber;

	GetVolumeInformation("C:\\", NULL, NULL, &dwVolumeSerialNumber, NULL, NULL, NULL, NULL);
	dwVolumeSerialNumber ^= 0x12345678;

	//CString str= "";
	//str.Format("%08X", dwVolumeSerialNumber);
	//AfxMessageBox(str);

	return	dwVolumeSerialNumber;
}
CString GetHDSerial()	//硬盘序列号
{
	char* temp;
	CString strHDSerial = "";
	CGetHDSerial HDSerial;   
	temp=HDSerial.GetHDSerial();
	strHDSerial.Format("%s",temp);
	CString strCutNumber = strHDSerial.Right(8);

	//AfxMessageBox(strCutNumber);

	return strCutNumber;
}
void InitCS()
{
	if(g_pStCS != NULL)					//结构体数组
	{
		delete [] g_pStCS;
		g_pStCS = NULL;
	}	
	g_pStCS = new CS_t[CS_SUCCESS];
	for(int i=0; i<CS_SUCCESS; i++)
	{
		g_pStCS[i].dwNumVolumeSerial = 0xFFFFFFFF;
		g_pStCS[i].strHDSerial = "";
		g_pStCS[i].strLoginName = "";
		g_pStCS[i].strLoginPwd = "";
		g_pStCS[i].strCS_SQL = "";
		g_pStCS[i].bFlagEnMusic = FALSE;
		g_pStCS[i].strCS_Name = "";
		g_pStCS[i].strCS_Worker = "";
		g_pStCS[i].strCS_Phone = "";
		g_pStCS[i].strCS_SMS = "";	
	}

	//#define CS_SUCCESS 14
	g_pStCS[14].strLoginName = "TigerWit";
	g_pStCS[14].strLoginPwd = "tg168";
	g_pStCS[14].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[14].dwNumVolumeSerial = 0x488B99D3;
	g_pStCS[14].strHDSerial = "G156808D";
	g_pStCS[14].bFlagEnMusic = FALSE;
	g_pStCS[14].strCS_Name = "TigerWit";
	g_pStCS[14].strCS_Worker = "SonggaV";
	g_pStCS[14].strCS_Phone = "";
	g_pStCS[14].strCS_SMS = "";

	g_pStCS[13].strLoginName = "萱源";
	g_pStCS[13].strLoginPwd = "xy168";
	g_pStCS[13].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[13].dwNumVolumeSerial = 0x00B005EB;
	g_pStCS[13].strHDSerial = "W3TLPQT9";
	g_pStCS[13].bFlagEnMusic = FALSE;
	g_pStCS[13].strCS_Name = "成都萱源农产品有限公司3-12";
	g_pStCS[13].strCS_Worker = "陈建孝";
	g_pStCS[13].strCS_Phone = "";
	g_pStCS[13].strCS_SMS = "";

	g_pStCS[12].strLoginName = "天磊";
	g_pStCS[12].strLoginPwd = "tl168";
	g_pStCS[12].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[12].dwNumVolumeSerial = 0x00B005EB;
	g_pStCS[12].strHDSerial = "S3T29TJ5";
	g_pStCS[12].bFlagEnMusic = FALSE;
	g_pStCS[12].strCS_Name = "青岛天磊蔬菜食品有限公司2-9";
	g_pStCS[12].strCS_Worker = "徐永忠";
	g_pStCS[12].strCS_Phone = "15964985098";
	g_pStCS[12].strCS_SMS = "15092107791";

	g_pStCS[0].strLoginName = "栖霞源鑫";
	g_pStCS[0].strLoginPwd = "yuanxin168";
	g_pStCS[0].strCS_SQL = "CS_QXYX_141012";
	g_pStCS[0].dwNumVolumeSerial = 0xCAEDB8B2;
	g_pStCS[0].strHDSerial = "W3TCZNDL";
	g_pStCS[0].bFlagEnMusic = FALSE;
	g_pStCS[0].strCS_Name = "烟台源鑫果蔬冷藏1-8";
	g_pStCS[0].strCS_Worker = "王金福";
	g_pStCS[0].strCS_Phone = "138-5357-7173";
	g_pStCS[0].strCS_SMS = "15253591031";

	g_pStCS[1].strLoginName = "栖霞锦程";
	g_pStCS[1].strLoginPwd = "770610";
	g_pStCS[1].strCS_SQL = "CS_QXJC_141014";
	g_pStCS[1].dwNumVolumeSerial = 0xCAEDB8B2;
	g_pStCS[1].strHDSerial = "W3TCTXL1";
	g_pStCS[1].bFlagEnMusic = FALSE;
	g_pStCS[1].strCS_Name = "栖霞锦程果蔬有限公司2-13";
	g_pStCS[1].strCS_Worker = "程绍龙";
	g_pStCS[1].strCS_Phone = "137-9259-5368";
	g_pStCS[1].strCS_SMS = "15166899019";

	g_pStCS[2].strLoginName = "赵县新星";
	g_pStCS[2].strLoginPwd = "zxxx168";
	g_pStCS[2].strCS_SQL = "CS_ZXJH_141211";
	g_pStCS[2].dwNumVolumeSerial = 0xCAEDB8B2;
	g_pStCS[2].strHDSerial = "W3TCZN9Y";
	g_pStCS[2].bFlagEnMusic = FALSE;
	g_pStCS[2].strCS_Name = "赵县新星鲜果库1-7";
	g_pStCS[2].strCS_Worker = "赵江辉";
	g_pStCS[2].strCS_Phone = "139-3314-9888";
	g_pStCS[2].strCS_SMS = "15933119379";

	g_pStCS[3].strLoginName = "振伟";
	g_pStCS[3].strLoginPwd = "zhw168";
	g_pStCS[3].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[3].dwNumVolumeSerial = 0xC24D10B1;
	g_pStCS[3].strHDSerial = "W4X0651X";
	g_pStCS[3].bFlagEnMusic = FALSE;
	g_pStCS[3].strCS_Name = "秀才营村振伟2-7";
	g_pStCS[3].strCS_Worker = "振伟";
	g_pStCS[3].strCS_Phone = "130-1158-5590";
	g_pStCS[3].strCS_SMS = "13785163749";

	g_pStCS[4].strLoginName = "祥卫";
	g_pStCS[4].strLoginPwd = "xw168";
	g_pStCS[4].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[4].dwNumVolumeSerial = 0xC24D10B1;
	g_pStCS[4].strHDSerial = "W53015ZG";
	g_pStCS[4].bFlagEnMusic = FALSE;
	g_pStCS[4].strCS_Name = "赵县大安三康果业1-6";
	g_pStCS[4].strCS_Worker = "李祥卫";
	g_pStCS[4].strCS_Phone = "137-8435-4899";
	g_pStCS[4].strCS_SMS = "13833174441";

	g_pStCS[5].strLoginName = "京华";
	g_pStCS[5].strLoginPwd = "jh168";
	g_pStCS[5].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[5].dwNumVolumeSerial = 0xC24D10B1;
	g_pStCS[5].strHDSerial = "W530DY2M";
	g_pStCS[5].bFlagEnMusic = FALSE;
	g_pStCS[5].strCS_Name = "贤门楼村高京华A-1-8";
	g_pStCS[5].strCS_Worker = "高京华";
	g_pStCS[5].strCS_Phone = "150-3117-1798";
	g_pStCS[5].strCS_SMS = "15075189081";

	g_pStCS[6].strLoginName = "新平";
	g_pStCS[6].strLoginPwd = "xp168";
	g_pStCS[6].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[6].dwNumVolumeSerial = 0xCAEDB8B2;
	g_pStCS[6].strHDSerial = "W4Y2K413";
	g_pStCS[6].bFlagEnMusic = FALSE;
	g_pStCS[6].strCS_Name = "赵县冀展2-10";
	g_pStCS[6].strCS_Worker = "李新平";
	g_pStCS[6].strCS_Phone = "189-3197-6208";
	g_pStCS[6].strCS_SMS = "";

	g_pStCS[7].strLoginName = "京华";
	g_pStCS[7].strLoginPwd = "jh168";
	g_pStCS[7].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[7].dwNumVolumeSerial = 0xC24D10B1;
	g_pStCS[7].strHDSerial = "W4X063K3";
	g_pStCS[7].bFlagEnMusic = FALSE;
	g_pStCS[7].strCS_Name = "贤门楼村高京华B-1-9";
	g_pStCS[7].strCS_Worker = "高京华";
	g_pStCS[7].strCS_Phone = "150-3117-1798";
	g_pStCS[7].strCS_SMS = "15176899807";

	g_pStCS[8].strLoginName = "广勤";
	g_pStCS[8].strLoginPwd = "gq168";
	g_pStCS[8].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[8].dwNumVolumeSerial = 0x00B005EB;
	g_pStCS[8].strHDSerial = "S3T29THK";
	g_pStCS[8].bFlagEnMusic = FALSE;
	g_pStCS[8].strCS_Name = "广勤2-9";
	g_pStCS[8].strCS_Worker = "广勤";
	g_pStCS[8].strCS_Phone = "136-0311-1097";
	g_pStCS[8].strCS_SMS = "13833467437";

	g_pStCS[9].strLoginName = "新圣安控";//"栖霞源鑫"
	g_pStCS[9].strLoginPwd = "xsak168";
	g_pStCS[9].strCS_SQL = "CS_QXJC_141014";
	//g_pStCS[9].strCS_SQL = "CS_ZW17_150313";
	//g_pStCS[0].strCS_SQL = "CS_QXYX_141012";
	g_pStCS[9].dwNumVolumeSerial = 0xC24D10B1;
	g_pStCS[9].strHDSerial = "Z6E0SV0G";
	g_pStCS[9].bFlagEnMusic = FALSE;
	g_pStCS[9].strCS_Name = "青岛新圣安控机电科技";
	g_pStCS[9].strCS_Worker = "逄淑松";
	g_pStCS[9].strCS_Phone = "185-6066-8933";
	g_pStCS[9].strCS_SMS = "18678969506";

	g_pStCS[10].strLoginName = "笔记本";//"栖霞源鑫"
	g_pStCS[10].strLoginPwd = "xsak168";
	g_pStCS[10].strCS_SQL = "CS_QXJC_141014";
	g_pStCS[10].dwNumVolumeSerial = 0xC24D10B1;
	g_pStCS[10].strHDSerial = "AA3H4061";
	g_pStCS[10].bFlagEnMusic = FALSE;
	g_pStCS[10].strCS_Name = "青岛新圣安控机电科技Thinkpad";
	g_pStCS[10].strCS_Worker = "逄淑松";
	g_pStCS[10].strCS_Phone = "185-6066-8933";
	g_pStCS[10].strCS_SMS = "";

	g_pStCS[11].strLoginName = "笔记本";//"栖霞源鑫"
	g_pStCS[11].strLoginPwd = "xsak168";
	g_pStCS[11].strCS_SQL = "CS_ZW17_150313";
	g_pStCS[11].dwNumVolumeSerial = 0xAE8CCFB6;
	g_pStCS[11].strHDSerial = "5VG46WG4";
	g_pStCS[11].bFlagEnMusic = FALSE;
	g_pStCS[11].strCS_Name = "青岛新圣安控机电科技Dell";
	g_pStCS[11].strCS_Worker = "陶晓军";
	g_pStCS[11].strCS_Phone = "";
	g_pStCS[11].strCS_SMS = "";
}
int GetCS_Index()
{
	int nCS_Index = -1;

	InitCS();

	DWORD dwMySerial = GetVolumeSerial();	
	CString strMySerial = GetHDSerial();

	for(int i=0; i<CS_SUCCESS; i++)
	{
		if((g_pStCS[i].dwNumVolumeSerial == dwMySerial) && (strcmp(g_pStCS[i].strHDSerial, strMySerial) == 0) )
		{
			nCS_Index = i;
			break;
		}
	}

	return nCS_Index;
}
//异常处理
CString Phone2Msg(CString strUrPhone)
{
	CString strMsgPhone = "";

	CString strPart = "";
	CString strAll = strUrPhone;

	for(int i=1; i<6; i++)
	{
		strPart = strAll.Left(2);
		strMsgPhone += strPart.Right(1);
		strMsgPhone += strPart.Left(1);	

		int nLast = 11-2*i;
		strAll = strAll.Right(11-2*i);

		if(nLast < 2)
		{
			strMsgPhone += "F";
			strMsgPhone += strAll;				
		}
	}

	return strMsgPhone;
}
CString Float2Unicode(float fTemp)
{
	CString strUnicode = "";
	if(fTemp >= 1E-7)
	{
		int nXS = (int)(fTemp*10) % 10;
		CString strXS = "";
		strXS.Format("003%d", nXS);

		int nZS = (int)fTemp;
		int nGW = nZS % 10;
		CString strGW = "";
		strGW.Format("003%d", nGW);

		int nSW = nZS/10;
		CString strSW = "";
		strSW.Format("003%d", nSW);

		strUnicode = "0020"+strSW+strGW+"002E"+strXS;
	}
	else
	{
		fTemp = 0 - fTemp;

		int nXS = (int)(fTemp*10) % 10;
		CString strXS = "";
		strXS.Format("003%d", nXS);

		int nZS = (int)fTemp;
		int nGW = nZS % 10;
		CString strGW = "";
		strGW.Format("003%d", nGW);

		int nSW = nZS/10;
		CString strSW = "";
		strSW.Format("003%d", nSW);

		strUnicode = "002D"+strSW+strGW+"002E"+strXS;
	}

	return strUnicode;
}
void Msg_QXYX(CSerialPort *m_SerialPort_MSG)
{//2机8库
	CString j1 = "0031";
	CString j2 = "0032";

	(g_StCXMsg.YSJ[0])?
		j1 += "5F00": j1 += "5173";
		j1 += "003B";
	(g_StCXMsg.YSJ[1])?
		j2 += "5F00": j2 += "5173";
		j2 += "003B000A";//分号+回车

	CString strTemp = "";

	CString k1 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[0]);
	k1 += strTemp;
	k1 += "003B";

	CString k2 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[1]);
	k2 += strTemp;
	k2 += "003B000A";

	CString k3 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[2]);
	k3 += strTemp;
	k3 += "003B";

	CString k4 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[3]);
	k4 += strTemp;
	k4 += "003B000A";

	CString k5 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[4]);
	k5 += strTemp;
	k5 += "003B";

	CString k6 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[5]);
	k6 += strTemp;
	k6 += "003B000A";

	CString k7 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[6]);
	k7 += strTemp;
	k7 += "003B";

	CString k8 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[7]);
	k8 += strTemp;
	k8 += "3002";	

	CString strSqlMsg = "SELECT * FROM SetPhone WHERE MyUr = '目标'";
	m_pRs_Msg = m_pCon->Execute(_bstr_t(strSqlMsg),NULL,adCmdText);	//94.1
	while(!m_pRs_Msg->adoEOF)
	{
		CString strUrPhone = (LPCSTR)_bstr_t(m_pRs_Msg->GetCollect("Phone"));
		CString strMsgPhone = Phone2Msg(strUrPhone);

		CString strMsg = "";
		strMsg.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
			,"0031000B81" ,strMsgPhone, "0008A7", "74", j1, j2, k1, k2, k3, k4, k5, k6, k7, k8);
		strMsg += (char)(26);

		m_SerialPort_MSG->WriteToPort("AT+CMGD=1,4\r\n");	//删除SIM卡上已发和未发的全部短信
		Sleep(2000);

		m_SerialPort_MSG->WriteToPort("AT+CMGS=130\r\n");
		Sleep(1000);

		m_SerialPort_MSG->WriteToPort(strMsg);	

		Sleep(3500);
		
		m_pRs_Msg->MoveNext();
	}
	m_pRs_Msg->raw_Close();	//94.2
}
void Msg1(CSerialPort *m_SerialPort_MSG)
{
	//第一条短信
	CString YSJ = "538B7F29673A";
	YSJ += "000A";
	CString j1 = "0031";
	CString j2 = "0032";
	CString j3 = "0033";
	CString j4 = "0034";

	(g_StCXMsg.YSJ[0])?
		j1 += "5F00": j1 += "5173";
		j1 += "003B";
	(g_StCXMsg.YSJ[1])?
		j2 += "5F00": j2 += "5173";
		j2 += "003B000A";//分号+回车
	(g_StCXMsg.YSJ[2])?
		j3 += "5F00": j3 += "5173";
		j3 += "003B";
	(g_StCXMsg.YSJ[3])?
		j4 += "5F00": j4 += "5173";
		j4 += "003B000A";//分号+回车

	CString Ku1_4 = "0031002D0034";
	Ku1_4 += "000A";
		
	CString strTemp = "";

	CString k1 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[0]);
	k1 += strTemp;
	k1 += "003B";

	CString k2 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[1]);
	k2 += strTemp;
	k2 += "000A";

	CString k3 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[2]);
	k3 += strTemp;
	k3 += "003B";

	CString k4 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[3]);
	k4 += strTemp;

	CString strSqlMsg = "SELECT * FROM SetPhone WHERE MyUr = '目标'";
	m_pRs_Msg = m_pCon->Execute(_bstr_t(strSqlMsg),NULL,adCmdText);	//95.1
	while(!m_pRs_Msg->adoEOF)
	{
		CString strUrPhone = (LPCSTR)_bstr_t(m_pRs_Msg->GetCollect("Phone"));
		CString strMsgPhone = Phone2Msg(strUrPhone);

		CString strMsg = "";
		strMsg.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
			,"0031000B81" ,strMsgPhone, "0008A7", "5A", YSJ, j1, j2, j3, j4, Ku1_4, k1, k2, k3, k4);
		strMsg += (char)(26);

		m_SerialPort_MSG->WriteToPort("AT+CMGD=1,4\r\n");
		Sleep(2000);

		m_SerialPort_MSG->WriteToPort("AT+CMGS=104\r\n");
		Sleep(1000);

		m_SerialPort_MSG->WriteToPort(strMsg);	

		Sleep(3500);
		
		m_pRs_Msg->MoveNext();
	}
	m_pRs_Msg->raw_Close();	//95.2
}
void Msg2(CSerialPort *m_SerialPort_MSG)
{
	//第二条短信
	CString Ku5_13 = "0035002D00310033";
	Ku5_13 += "000A";

	CString strTemp = "";

	CString k5 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[4]);
	k5 += strTemp;
	k5 += "003B";

	CString k6 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[5]);
	k6 += strTemp;
	k6 += "000A";

	CString k7 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[6]);
	k7 += strTemp;
	k7 += "003B";

	CString k8 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[7]);
	k8 += strTemp;
	k8 += "000A";

	CString k9 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[8]);
	k9 += strTemp;
	k9 += "003B";

	CString k10 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[9]);
	k10 += strTemp;
	k10 += "000A";

	CString k11 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[10]);
	k11 += strTemp;
	k11 += "003B";

	CString k12 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[11]);
	k12 += strTemp;
	k12 += "000A";

	CString k13 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[12]);
	k13 += strTemp;

	CString strSqlMsg = "SELECT * FROM SetPhone WHERE MyUr = '目标'";
	m_pRs_Msg = m_pCon->Execute(_bstr_t(strSqlMsg),NULL,adCmdText);	//96.1
	while( !m_pRs_Msg->adoEOF )
	{
		CString strUrPhone = (LPCSTR)_bstr_t(m_pRs_Msg->GetCollect("Phone"));
		CString strMsgPhone = Phone2Msg(strUrPhone);

		CString strMsg = "";
		strMsg.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
			,"0031000B81" ,strMsgPhone, "0008A7", "74", Ku5_13, k5, k6, k7, k8, k9, k10, k11, k12, k13);
		strMsg += (char)(26);

		m_SerialPort_MSG->WriteToPort("AT+CMGD=1,4\r\n");
		Sleep(2000);

		m_SerialPort_MSG->WriteToPort("AT+CMGS=130\r\n");
		Sleep(1000);

		m_SerialPort_MSG->WriteToPort(strMsg);	

		Sleep(3500);
		
		m_pRs_Msg->MoveNext();
	}
	m_pRs_Msg->raw_Close();	//96.2
}
void Msg3(CSerialPort *m_SerialPort_MSG)
{
	//第三条短信
	CString Ku14_22 = "00310034002D00320032";
	Ku14_22 += "000A";

	CString strTemp = "";

	CString k14 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[13]);
	k14 += strTemp;
	k14 += "003B";

	CString k15 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[14]);
	k15 += strTemp;
	k15 += "000A";

	CString k16 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[15]);
	k16 += strTemp;
	k16 += "003B";

	CString k17 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[16]);
	k17 += strTemp;
	k17 += "000A";

	CString k18 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[17]);
	k18 += strTemp;
	k18 += "003B";

	CString k19 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[18]);
	k19 += strTemp;
	k19 += "000A";

	CString k20 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[19]);
	k20 += strTemp;
	k20 += "003B";

	CString k21 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[20]);
	k21 += strTemp;
	k21 += "000A";

	CString k22 = "";
	strTemp = Float2Unicode(g_StCXMsg.Ku[21]);
	k22 += strTemp;

	CString strSqlMsg = "SELECT * FROM SetPhone WHERE MyUr = '目标'";
	m_pRs_Msg = m_pCon->Execute(_bstr_t(strSqlMsg),NULL,adCmdText);	//97.1
	while( !m_pRs_Msg->adoEOF )
	{
		CString strUrPhone = (LPCSTR)_bstr_t(m_pRs_Msg->GetCollect("Phone"));
		CString strMsgPhone = Phone2Msg(strUrPhone);

		CString strMsg = "";
		strMsg.Format("%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
			,"0031000B81" ,strMsgPhone, "0008A7", "76", Ku14_22, k14, k15, k16, k17, k18, k19, k20, k21, k22);
		strMsg += (char)(26);

		m_SerialPort_MSG->WriteToPort("AT+CMGD=1,4\r\n");
		Sleep(2000);

		m_SerialPort_MSG->WriteToPort("AT+CMGS=132\r\n");
		Sleep(1000);

		m_SerialPort_MSG->WriteToPort(strMsg);	

		Sleep(3500);
		
		m_pRs_Msg->MoveNext();
	}
	m_pRs_Msg->raw_Close();	//97.2
}
void MsgEx(int nEx, int nID, CSerialPort *m_SerialPort_MSG)
{
	CString strEx = "";
	switch(nEx)
	{
	case MSG_EX_WATER:	//水流异常
		{	//如果是压缩机，则为水流异常；如果是其他节点，则为电流异常
			//不从数据库中读取，从CanDlg右侧列表中读取，减轻数据库压力--不行，界面后期可能会改
			//从数据库中提取，更方便
			CString strSqlID = "";
			strSqlID.Format("SELECT * FROM SetCid WHERE Cid = %d AND Cid > 2", nID);
			m_pRs_Msg = m_pCon->Execute(_bstr_t(strSqlID),NULL,adCmdText);	//98.1
			if(!m_pRs_Msg->BOF)
			{
				CString strItem = (LPCSTR)_bstr_t(m_pRs_Msg->GetCollect("IdItem"));
				if(strcmp(strItem, "压缩机") == 0)
				{
					strEx = "6C346D415F025E38";	//水流异常
				}
				else
				{
					strEx = "75356D415F025E38";	//电流异常
				}
			}
			else
			{
				strEx = "6C346D415F025E38";
			}
			m_pRs_Msg->raw_Close();	//98.2
			
			break;
		}
	case MSG_EX_YSJ:	//转动异常
		{
			strEx = "8F6C52A85F025E38";
			break;
		}
	case MSG_EX_TIMES:	//开机超时
		{
			strEx = "5F00673A8D8565F6";
			break;
		}
	case MSG_EX_LOST:	//单元丢失
		{
			strEx = "535551434E225931";
			break;
		}
	case MSG_EX_ANS:	//漏氨报警
		{
			strEx = "6F0F6C2862A58B66";
			break;
		}
	case MSG_EX_CHSH:	//冲霜水位
		{
			strEx = "51B2971C6C344F4D";
			break;
		}
	case MSG_AUTHOR_NO:	//权限丢失
		{
			strEx = "674396504E225931";
			break;
		}
	case MSG_AUTHOR_30:	//权限有限
		{
			strEx = "6743965067099650";
			break;
		}
	case MSG_AUTHOR_FR:	//权限永久
		{
			strEx = "674396506C384E45";
			break;
		}
	case MSG_EX_T_RDS://读温超时
		{
			strEx = "8BFB6E298D8565F6";
			break;		
		}
	case MSG_EX_T_LOST://温度丢失
		{
			strEx = "6E295EA64E225931";
			break;			
		}
	case MSG_EX_T_DNS://温度超限
		{
			strEx = "6E295EA68D859650";
			break;			
		}
	case MSG_PROP_ON://开始开机
		{
			strEx = "5F0059CB5F00673A";
			break;			
		}
	case MSG_PROP_OFF://关机结束
		{
			strEx = "5173673A7ED3675F";
			break;			
		}
	case MSG_EX_WATCH://循环跑飞
		{
			strEx = "5faa73af8dd198de";
			break;		
		}
	case MSG_EX_ELECT://市电异常
		{
			strEx = "5e0275355f025e38";
			break;		
		}
	default:
		break;
	}
	
	CString strSqlMsg = "SELECT * FROM SetPhone WHERE MyUr = '目标'";
	m_pRs_Msg = m_pCon->Execute(_bstr_t(strSqlMsg),NULL,adCmdText);	//99.1
	while( !m_pRs_Msg->adoEOF )
	{
		CString strUrPhone = (LPCSTR)_bstr_t(m_pRs_Msg->GetCollect("Phone"));
		CString strMsgPhone = Phone2Msg(strUrPhone);

		CString strMsg = "";
		strMsg.Format("%s%s%s%s%s"
			,"0031000B81" ,strMsgPhone, "0008A7", "08", strEx);
		strMsg += (char)(26);

		m_SerialPort_MSG->WriteToPort("AT+CMGD=1,4\r\n");
		Sleep(2000);

		m_SerialPort_MSG->WriteToPort("AT+CMGS=22\r\n");
		Sleep(1000);

		m_SerialPort_MSG->WriteToPort(strMsg);	

		Sleep(3500);
		
		m_pRs_Msg->MoveNext();
	}
	m_pRs_Msg->raw_Close();	//99.2

	if(SIM_STATUS)
		AfxMessageBox("异常短信");
}
void Msg_OnProp()
{
	g_nMsgEx = MSG_PROP_ON;
	if(g_pCanDlg->m_bSerialPortOpened_MSG) //检查串口是否打开
	{
		if(g_hMsg == NULL)
		{	
			CWinThread* pThread;
			pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			g_hMsg = pThread->m_hThread;
		}
		else
		{
			Doing2Text("开始开机，短信被占用");
		}
	}
	else
	{
		Doing2Text("开始开机，短信未开启");
	}
}
void Msg_OffProp()
{
	g_nMsgEx = MSG_PROP_OFF;
	if(g_pCanDlg->m_bSerialPortOpened_MSG) //检查串口是否打开
	{
		if(g_hMsg == NULL)
		{	
			CWinThread* pThread;
			pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			g_hMsg = pThread->m_hThread;
		}
		else
		{
			Doing2Text("关机完毕，短信被占用");
		}
	}
	else
	{
		Doing2Text("关机完毕，短信未开启");
	}
}
UINT ThreadProc_Msg(LPVOID wParam)
{	
	CSerialPort *m_SerialPort_MSG = (CSerialPort*)wParam;

	if(g_bFlagSimTEMP)
	{
		//AfxMessageBox("温度模拟，短信发送");
		Doing2Text("温度模拟，短信发送");
		return -1;
	}

	if(!g_pCanDlg->m_bSerialPortOpened_MSG) 
	{
		AfxMessageBox("请打开短信通道");
		return -1;
	}

	if(!g_nMsgEx)
	{
		if( (g_byYSJSum_SetKu < 3) && (g_byKuSum_SetKu < 9) )
		{
			Msg_QXYX(m_SerialPort_MSG);	//最大2机8库
		}
		else
		{
			Msg1(m_SerialPort_MSG);
			Msg2(m_SerialPort_MSG);
			
			if(g_byKuSum_SetKu > 13)
			{
				Msg3(m_SerialPort_MSG);	
			}		
		}
	}
	else
	{
		MsgEx(g_nMsgEx, g_nIDEx, m_SerialPort_MSG);
	}

	g_nMsgEx = -1;
	g_nIDEx = 0;
	Doing2Text("短信发送完毕");

	g_hMsg = NULL;	//151111 句柄复位很重要
	return 0;
}
BOOL ExEvent()
{
	if( (g_RcvData[2] == 0xA2) || (g_RcvData[2] == 0xA3) || (g_RcvData[2] == 0xA4) || (g_RcvData[2] == 0xA5) )
	{	
		//紧急关机--关机流程不执行紧急关机
		if(!g_bUrgentING)
		{
			CString strEx = "";
			switch(g_RcvData[2])
			{
			case 0xA2:	
				{
					CString strSqlID = "";
					strSqlID.Format("SELECT * FROM SetCid WHERE Cid = %d AND Cid > 2", g_RcvData[3]);
					m_pRs_Msg = m_pCon->Execute(_bstr_t(strSqlID),NULL,adCmdText);	//100.1
					if(!m_pRs_Msg->BOF)
					{
						CString strItem = (LPCSTR)_bstr_t(m_pRs_Msg->GetCollect("IdItem"));
						if(strcmp(strItem, "压缩机") == 0)
						{
							strEx = "水流异常";
						}
						else
						{
							strEx = "电流异常";	
						}
					}
					else
					{
						strEx = "水流异常";
					}
					m_pRs_Msg->raw_Close();	//100.2
					break;
				}
			case 0xA3:	strEx = "转动异常";	break;
			case 0xA4:	strEx = "漏氨报警";	break;
			case 0xA5:	strEx = "冲霜水位";	break;
			case 0xA7:	strEx = "市电异常";	break;	//151214
			default:	break;
			}			
			
			CString strBackError = "";
			strBackError.Format("下位机%d反馈故障%s，停止控制，报警，紧急关机", g_RcvData[3], strEx);
			Doing2Text(strBackError);
			
			// 发送紧急短信
			if(g_hMsg == NULL)
			{				
				//g_nMsgEx = **;//已赋值	
				CWinThread* pThread;
				pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
				g_hMsg = pThread->m_hThread;
			}
			else
			{
				Doing2Text("短信被占用");
			}

			Auto_UrgentStop("单元反馈 意外发生");
		}

		g_RcvData[2] = 0xFF;

		return TRUE;
	}	

	return FALSE;
}
//氨检
void ANS_CRC16( char *puchMsg, unsigned char usDataLen)
{
	unsigned char uchCRCHi=0xFF ; /* 高 CRC 字节初始化*/
	unsigned char uchCRCLo=0xFF ; /* 低 CRC 字节初始化*/
	unsigned char uIndex ; /* CRC 循环中的索引*/
	while (usDataLen--) /* 传输消息缓冲区*/
	{
		uIndex = uchCRCHi ^ (*puchMsg) ; /* 计算 CRC */
		puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
		uchCRCLo = auchCRCLo[uIndex] ;
	}

	g_aAnsComSendData[6] = (char)uchCRCHi;
	g_aAnsComSendData[7] = (char)uchCRCLo;
}

void ANS_GetStatus()
{
	g_aAnsComSendData[0] = 0x01;	//设备ID
	g_aAnsComSendData[1] = 0x03;	//功能码
	g_aAnsComSendData[2] = 0x00;	//地址高位
	g_aAnsComSendData[3] = 0x00;	//地址低位

	//1个巡检路数 对应 2个寄存器长度
	if(g_byAnsSum_SetKu < 255)
	{
		g_aAnsComSendData[4] = 0x00;
		g_aAnsComSendData[5] = (char)g_byAnsSum_SetKu*2;
	}

	g_aAnsComSendData[6] = 0x00;
	g_aAnsComSendData[7] = 0x00;
	ANS_CRC16( g_aAnsComSendData, 6);

	g_strAnsRcvUp3.Format("%02X", 4*g_byAnsSum_SetKu);
	g_strAnsRcvUp3 = "0103" + g_strAnsRcvUp3;

	if(g_pStAnS == NULL)
	{
		g_pStAnS = new AnS_t[g_byAnsSum_SetKu];
		for(int i=0; i<g_byAnsSum_SetKu; i++)
		{
			g_pStAnS[i].byNum = 0;
			g_pStAnS[i].byCon = 0;
			g_pStAnS[i].strStatus = "正常";
			g_pStAnS[i].byStatusNum = 0;
		}
	}
}
//温度保护
void TempS_GetTempSorST()
{
	//动态申请结构体数组
	if(g_pStTempSor != NULL)
	{
		delete [] g_pStTempSor;
		g_pStTempSor = NULL;
	}
	g_pStTempSor = new TempSor_t[g_byTempSum_SetKu];
	for(int i=0; i<g_byTempSum_SetKu; i++)
	{
		memset(&g_pStTempSor[i], 0, sizeof(TempSor_t));
		g_pStTempSor[i].fTempDnS = -255.0;
	}

	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 ORDER BY KuNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-8.1
	int nKuTempIndex = 0;
	while(!m_pRs->adoEOF)
	{
		//151114
		nKuTempIndex++;
		if(nKuTempIndex*2 > g_byTempSum_SetKu)
			break;

		//获取库房编号
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
		BYTE byKuNum = atoi(strKuNum);

		CString strTempDn = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempDn"));			
		float fTempDn=(float)atof(strTempDn);

		CString strKuStatus = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuStatus"));
		BOOL bKuStatus = FALSE;
		(strcmp(strKuStatus, "0") == 0)? bKuStatus = FALSE: bKuStatus = TRUE;

		//获取测温序号
		BYTE aTempNum[2] = {0, 0};
		CString strTempNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempNum"));
		Str2Dec(strTempNum, aTempNum);

		int nKuIndex = byKuNum-1;
		g_pStTempSor[nKuIndex*2].byKuNum = byKuNum;
		g_pStTempSor[nKuIndex*2].bKuStatus = bKuStatus;
		g_pStTempSor[nKuIndex*2].fKuTempDn = fTempDn;
		g_pStTempSor[nKuIndex*2].byTempIndex = aTempNum[0];
		g_pStTempSor[nKuIndex*2].mTimeLast.SetDate(2014, 7, 8);	//memset初始值为0或1899，设置成2015作为在用探头OnTimer中判断
		g_pStTempSor[nKuIndex*2+1].byKuNum = byKuNum;
		g_pStTempSor[nKuIndex*2+1].bKuStatus = bKuStatus;
		g_pStTempSor[nKuIndex*2+1].fKuTempDn = fTempDn;
		g_pStTempSor[nKuIndex*2+1].byTempIndex = aTempNum[1];
		g_pStTempSor[nKuIndex*2+1].mTimeLast.SetDate(2014, 7, 8);

		m_pRs->MoveNext();
	}	
	m_pRs->raw_Close();	//RS-8.2

	strSql = "SELECT * FROM SetJZTemp WHERE TempNum > 0";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-9.1
	int nTempIndex = 0;
	while(!m_pRs->adoEOF)
	{
		//151114
		nTempIndex++;
		if(nTempIndex > g_byTempSum_SetKu)
			break;
		
		//获取温度编号
		CString strTempNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempNum"));
		BYTE byTempNum = atoi(strTempNum);

		//校准值
		CString strJZTempData = (LPCSTR)_bstr_t(m_pRs->GetCollect("JZData"));
		float fJZTempData = (float)atof(strJZTempData);
		//下限报警值
		CString strTempDnS = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempDnS"));
		float fTempDnS = (float)atof(strTempDnS);

		int i=0;
		for(i=0; i<g_byTempSum_SetKu; i++)
		{
			if( (g_pStTempSor[i].byTempIndex > 0) && (g_pStTempSor[i].byTempIndex == byTempNum) )
			{
				g_pStTempSor[i].fTempJZ = fJZTempData;
				g_pStTempSor[i].fTempDnS = fTempDnS;
				break;
			}
		}
	
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//RS-9.2

	for(int i=0; i<g_byTempSum_SetKu; i++)
	{
		float fTempDnV = g_pStTempSor[i].fKuTempDn - g_pStTempSor[i].fTempDnS;
		if(fTempDnV < 0.5)
		{
			CString strText = "";
			strText.Format("温度探头%d 下限报警 过于敏感！", g_pStTempSor[i].byTempIndex);
			Doing2Text(strText);
			AfxMessageBox(strText);
		}
	}
}

void TempS_GetTempSorTime()
{
	//判断探头温度是否更新
	if(g_pStTempSor != NULL)
	{
		for(int nTempIndex = 0; nTempIndex < g_byTempSum_SetKu; nTempIndex++)
		{
			if(!g_pCanDlg->m_bSerialPortOpened_TEMP)
				break;
			if( (g_pStTempSor[nTempIndex].bKuStatus) && (g_pStTempSor[nTempIndex].mTimeLast.GetYear() > 2014) )	//默认值为1899，有效的初始值为2014
			{
				COleDateTimeSpan mTimeCAL;
				mTimeCAL = g_mTimeNow - g_pStTempSor[nTempIndex].mTimeLast;
				int nTotalMinutes = (int)mTimeCAL.GetTotalMinutes();
				//int nTotalMinutes = (int)mTimeCAL.GetTotalSeconds();

				if(nTotalMinutes > 5)
				{
					if(!g_pStTempSor[nTempIndex].bTimeS)
					{
						g_pStTempSor[nTempIndex].bTimeS = TRUE;
						
						CString str = "";
						str.Format("探头%d已5分钟没有读取到温度", g_pStTempSor[nTempIndex].byTempIndex);
						Doing2Text(str);


						//发送温度异常短信
						if(g_hMsg == NULL)
						{
							g_nMsgEx = MSG_EX_T_RDS;	//温度超时

							CWinThread* pThread;
							pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
							g_hMsg = pThread->m_hThread;
						}
						else
						{
							Doing2Text("温度异常，短信被占用");
							g_pStTempSor[nTempIndex].bTimeS = FALSE;
						}
					}
				}
				else
				{
					g_pStTempSor[nTempIndex].bTimeS = FALSE;
				}
			}
		}
	}
}
//时间保护
UINT ThreadProc_TimeS(LPVOID wParam)
{
	COleDateTime TimeProtectST;
	COleDateTimeSpan TimeProtectCAL;
	int nTotalMinutes;
	
	//获取当前时间
	TimeProtectST = g_mTimeNow;
	CString strTimeNow = g_mTimeNow.Format("%H:%M:%S");
	CString strTimeNowProtect = "定时保护 起始时间：" + strTimeNow;
	Doing2Text(strTimeNowProtect);

	do
	{
		Sleep(1000);

		TimeProtectCAL = g_mTimeNow - TimeProtectST;

		nTotalMinutes = (int)TimeProtectCAL.GetTotalMinutes();
		//nTotalMinutes = (int)TimeProtectCAL.GetTotalSeconds();
	}
	while((!g_bStopKuCtrl) && (!g_bTimeProtectOUT) && (nTotalMinutes < g_nTimeS_SetKu));

	if((!g_bStopKuCtrl) && (!g_bTimeProtectOUT) && (nTotalMinutes >= g_nTimeS_SetKu))
	{
		if(!LOGIN_EN)	//仅供测试
			AfxMessageBox("超出保护时间，报警");
		Doing2Text("超出保护时间，报警");

		// 发送紧急短信
		if(g_hMsg == NULL)
		{				
			g_nMsgEx = MSG_EX_TIMES;	//温度异常
			CWinThread* pThread;
			pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			g_hMsg = pThread->m_hThread;
		}
		else
		{
			Doing2Text("温度异常，短信被占用");
		}
	}

	strTimeNow = g_mTimeNow.Format("%H:%M:%S");
	strTimeNowProtect = "定时保护 终止时间：" + strTimeNow;
	Doing2Text(strTimeNowProtect);	

	g_hTimeProtect = NULL;
	return 0;
}
void TimeS_OnPropST()
{				
	if(g_hTimeProtect == NULL)
	{
		g_bTimeProtectOUT = FALSE;
		
		CWinThread* pThread;
		pThread = AfxBeginThread(ThreadProc_TimeS, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		g_hTimeProtect = pThread->m_hThread;		
	}	
}
//SMS Lock
int Lock_GetAuthorRunningDays()
{
	COleDateTimeSpan TimeRunningCAL;

	TimeRunningCAL = g_mTimeNow - g_mTimeAuthorST;

	int nRunningDays = (int)TimeRunningCAL.GetTotalDays();
	
	return nRunningDays;
}
void Lock_GetSMS(CString strRcv)
{
	CString strTime = "";
	strTime.Format("%d-%02d-%02d 8:00:00", g_mTimeNow.GetYear(), g_mTimeNow.GetMonth(), g_mTimeNow.GetDay());
	
	CString strSmsLock = "";
	if(strcmp(strRcv, "55466240") == 0)		//商所，30天
	{
		strSmsLock.Format("UPDATE SetKu SET [KuStore] = '%s',[ChangeDays] = 0,[ChangeST] = '%s' WHERE KuNum = 0"
			, "30", strTime);
		m_pCon->Execute(_bstr_t(strSmsLock),NULL,adCmdText);
		Doing2Text("用户获得30天权限");
	}
	else if(strcmp(strRcv, "9F99795E") == 0)	//龙神，永久
	{
		strSmsLock.Format("UPDATE SetKu SET [KuStore] = '%s',[ChangeDays] = 0,[ChangeST] = '%s' WHERE KuNum = 0"
			, "-1", strTime);
		m_pCon->Execute(_bstr_t(strSmsLock),NULL,adCmdText);
		Doing2Text("用户获得永久权限");
	}
	else if(strcmp(strRcv, "677E560E") == 0)		//松嘎，上锁
	{
		strSmsLock.Format("UPDATE SetKu SET [KuStore] = '%s',[ChangeDays] = 0,[ChangeST] = '%s' WHERE KuNum = 0"
			, "0", strTime);
		m_pCon->Execute(_bstr_t(strSmsLock),NULL,adCmdText);
		Doing2Text("用户失去使用权限");
	}

	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum = 0";
	m_pRs_Lock = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//101.1
	if(!m_pRs_Lock->BOF)
	{
		CString strAuthorST = (LPCSTR)_bstr_t(m_pRs_Lock->GetCollect("ChangeST")); 
		g_mTimeAuthorST.ParseDateTime(strAuthorST);
		CString strRunningDays = (LPCSTR)_bstr_t(m_pRs_Lock->GetCollect("ChangeDays")); 
		g_nRunningDays = atoi(strRunningDays);
		CString strAuthorDays = (LPCSTR)_bstr_t(m_pRs_Lock->GetCollect("KuStore")); 
		g_nAuthorDays = atoi(strAuthorDays);
	}
	m_pRs_Lock->raw_Close();	//101.2

	(g_nAuthorDays == -1)? g_bAuthorAll = TRUE: g_bAuthorAll = FALSE;
	if(!g_bAuthorAll)
	{
		if(g_nAuthorDays == 0)
		{
			g_bSysCanRun = FALSE;
			g_nMsgEx = MSG_AUTHOR_NO;	//权限丢失
		}
		else
		{
			(g_nRunningDays > g_nAuthorDays)? g_bSysCanRun = FALSE: g_bSysCanRun = TRUE;
			g_nMsgEx = MSG_AUTHOR_30;	//权限有限
		}		
	}
	else
	{
		g_bSysCanRun = TRUE;
		g_nMsgEx = MSG_AUTHOR_FR;	//权限永久
	}

	if(!g_bSysCanRun)
	{
		Lock_SysCanNotRun();
	}

	Lock_SendSMS();
}
void Lock_SendSMS()
{
	//Send SMS, "权限异常"

	if(g_pCanDlg->m_bSerialPortOpened_MSG) //检查串口是否打开
	{
		if(g_hMsg == NULL)
		{	
			CWinThread* pThread;
			pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			g_hMsg = pThread->m_hThread;
		}
		else
		{
			Doing2Text("权限异常，短信被占用");
		}
	}
	else
	{
		Doing2Text("权限异常，短信未开启");
	}
}
void Lock_OnPropAuthorRunning(CEdit *pEdit)
{
	CString strText = "";
	if(g_mTimeNow.GetHour() > 8)
	{			
		int nRunningDays = Lock_GetAuthorRunningDays();
		g_nRunningDays = max(nRunningDays, g_nRunningDays);

		strText.Format("UPDATE SetKu SET [ChangeDays] = %d WHERE KuNum = 0", g_nRunningDays);
		m_pCon->Execute(_bstr_t(strText),NULL,adCmdText);
	}

	strText.Format("使用权限%d天，已运行%d天", g_nAuthorDays, g_nRunningDays);
	Doing2Text(strText);
	pEdit->SetWindowTextA(strText);
	Sleep(1000);

	(g_nRunningDays > g_nAuthorDays)? g_bSysCanRun = FALSE: g_bSysCanRun = TRUE;
	//超过权限时间的后续处理，不能执行流程，不能手动控制，不能读温度，只能收发短信
	if(!g_bSysCanRun)
	{
		Lock_SysCanNotRun();
	}
}
void Lock_DayAuthorRunning()
{
	CString strText = "";
	if(!g_bAuthorAll)
	{
		g_nRunningDays++;	//每天8:10分，运行时间自动加1

		strText.Format("UPDATE SetKu SET [ChangeDays] = %d WHERE KuNum = 0", g_nRunningDays);
		m_pCon->Execute(_bstr_t(strText),NULL,adCmdText);

		strText.Format("使用权限%d天，已运行%d天", g_nAuthorDays, g_nRunningDays);
		Doing2Text(strText);

		//Authority days less than 3, send SMS
		int nLessAuthorDays = g_nAuthorDays - g_nRunningDays;
		if((nLessAuthorDays < 3) && (nLessAuthorDays > -1))
		{
			g_nMsgEx = MSG_AUTHOR_30;	//权限有限
			Lock_SendSMS();
		}

		(g_nRunningDays > g_nAuthorDays)? g_bSysCanRun = FALSE: g_bSysCanRun = TRUE;

		if(!g_bSysCanRun)
		{
			Lock_SysCanNotRun();
		}
	}
}

void Lock_SysCanNotRun()
{
	//①停止正在进行的
	if(g_hKuCtrl != NULL)
	{
		//没在紧急关机，则紧急关机
		if(!g_bUrgentING)
		{
			Auto_UrgentStop("权限丢失");
		}

		Sleep(1000);
	}

	ST_CidRefer();
	g_bStopRef = TRUE;

	g_bFlagTempReferING = FALSE;
	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->SetDlgItemTextA(IDC_BUTTON_TEMP_TEST, "开测温");
		g_pCanDlg->KillTimer(TIMER_ID_TEMP);
	}

	Doing2Text("超过使用期限，请联系售后");

	//②后续不能开启-CAN COM1-已处理
}
//TIMER TIME
UINT ThreadProc_TIMER_Time(LPVOID wParam)
{
	if(g_pCanDlg->m_bSerialPortOpened_TEMP)
		TempS_GetTempSorTime();

	//8:10
	if(g_bFlag8_10)
	{
		g_bFlag8_10 = FALSE;

		Lock_DayAuthorRunning();
	}

	g_hTimerTime = NULL;	//151111
	return 0;
}

//程序死机保护
UINT ThreadProc_CCSP_Watch(LPVOID wParam)
{
	g_nInvalidReadWatch = 0;
	while(g_bFlagKuCtrlING_Watch)
	{
		Sleep(1000);

		int nTotalMinutes = Read_WatchFile();
		//151219
		if(nTotalMinutes > 2)	//等待2+1分钟
		{
			//数据库错误弹出的对话框
			::FindWindowA(0, "Microsoft Visual Studio");	//Runtime等也可
			HWND hwnd = NULL;
			::GetWindow(hwnd, 0);
			::SendMessageA(hwnd, WM_QUIT, NULL, NULL);
						
			//尽量先能 正常中止
			g_bStopKuCtrl = TRUE;	//TimeS\KuTempChange\AllOnAllOff\OneOnOneOff
			g_bStopProp = TRUE;		//Timer2\Delay3
			g_bStopRef = TRUE;		//Ref
			Sleep(2000);

			//强制中止 会导致内存泄露
			if(g_hKuCtrl != NULL)
			{
				TerminateThread(g_hKuCtrl, 0);
				//151219
				if(g_hPropING != NULL)
					TerminateThread(g_hPropING, 0);
				Sleep(2000);
				g_hKuCtrl = NULL;	//151118 及时复位
				Doing2Text("强制关闭库控线程");
			}
			
			//紧急关机
			Auto_UrgentStop("程序循环");
			//151219
			if(g_bUrgentStop)	//紧急关机后不会出现一键关机
			{
				if(!g_bUrgentING)
					UrgentStopExecute(&g_pCanDlg->m_ctrlProp);
				
				//执行完毕151014
				g_bUrgentING = FALSE;
				g_bUrgentJump = FALSE;
			}		
			Sleep(2000);

			//紧急短信
			g_nMsgEx = MSG_EX_WATCH;
			if(g_pCanDlg->m_bSerialPortOpened_MSG) //检查串口是否打开
			{
				if(g_hMsg == NULL)
				{	
					CWinThread* pThread;
					pThread = AfxBeginThread(ThreadProc_Msg, &g_pCanDlg->m_SerialPort_MSG, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
					g_hMsg = pThread->m_hThread;
				}
				else
				{
					Doing2Text("关机完毕，短信被占用");
				}
			}
			else
			{
				Doing2Text("关机完毕，短信未开启");
			}

			break;
		}
	}
	g_hWatch = NULL;
	return -1;
}
int Read_WatchFile()
{
	/*int nCount = g_mFileWatch.GetLength();
	char *fileBuff = new char[nCount + 1];
	fileBuff[nCount] = '\0';
	g_mFileWatch.SeekToBegin();
	g_mFileWatch.Read(fileBuff, nCount+1);
	CString strFileWatch = fileBuff;*/

	COleDateTime m_LastWatchTime = g_mTimeWatch;
	//m_LastWatchTime.ParseDateTime(strFileWatch);	//不是时间格式，也不会出错，错误计数与复位
	/*if(m_LastWatchTime.m_dt < 1.0)
	{
		g_nInvalidReadWatch++;

		if(g_nInvalidReadWatch > 10)
			return 255;	//返回255，就肯定会触发紧急关机

		return 0;
	}
	else
	{
		g_nInvalidReadWatch = 0;
	}*/

	//不使用全局变量，独立
	COleDateTime m_TimeNow;
	m_TimeNow = COleDateTime::GetCurrentTime();
	COleDateTimeSpan m_SpanWatchTime = m_TimeNow - m_LastWatchTime;

	int nTotalMinutes = (int)m_SpanWatchTime.GetTotalMinutes();
	//int nTotalMinutes = (int)m_SpanWatchTime.GetTotalSeconds();
	if(nTotalMinutes < 0)
	{
		g_nInvalidReadWatch++;

		if(g_nInvalidReadWatch > 10)
			return 255;	//返回255，就肯定会触发紧急关机

		return 0;	
	}
	else
	{
		g_nInvalidReadWatch = 0;
	}

	//delete [] fileBuff;
	return nTotalMinutes;
}
void Write_WatchFile()
{	
	//g_mFileWatch.SetLength(0);
	
	COleDateTime m_TimeNow;
	g_mTimeWatch = COleDateTime::GetCurrentTime();

	CString strTimeNow = "";
	strTimeNow = _T(g_mTimeWatch.Format("%Y-%m-%d %H:%M:%S"));
	
	//g_mFileWatch.Write(strTimeNow,strTimeNow.GetLength()*sizeof(wchar_t));
	//int nCount = strTimeNow.GetLength();
	//g_mFileWatch.Write(strTimeNow,nCount*sizeof(char));

	if(g_pCanDlg != NULL)
		g_pCanDlg->GetDlgItem(IDC_EDIT_CHSH)->SetWindowTextA(strTimeNow);
}