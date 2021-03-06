#include "stdafx.h"
#include "CS.h"
#include "SetDlgFGYSJ.h"
#include "CANDlg.h"
#include <ATLComTime.h>
#include "ThreadProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSetDlgFGYSJ *pSetDlgFGYSJ = NULL;

extern _ConnectionPtr m_pCon;	//ADO连接对象
extern _RecordsetPtr m_pRs;		//ADO记录集对象
extern _RecordsetPtr m_pRs_ManualYSJ;
extern _RecordsetPtr m_pRs_ManualYSJ_In;

extern CCANDlg *g_pCanDlg;
extern Prop_t g_StProp;
extern YSJ_t *g_pStYSJ;
extern KuYSJ_t *g_pStKuYSJ;
extern KuCtrl_t *g_pStKuCtrl;

extern BYTE g_byYSJSum_SetKu;			//压缩机总数
extern BYTE g_byKuOnMax_SetKu;			//同开最多库房数
extern BYTE g_byKuSum_SetKu;			//库房总数
extern BYTE g_byKuCtrlSum_St;		//自动控制在用库房总数

extern BOOL g_bInDelay3;
extern BOOL g_bUrgentStop;
extern BOOL g_bEnKuYSJ;

extern HANDLE g_hKuYSJ;
extern BOOL g_bKuLinkYSJ;
extern BOOL g_bStopKuYSJ;
extern BYTE g_byKuOnSum;
extern BYTE g_byYSJOnSum;

extern HANDLE g_hTimer2;
extern BOOL g_bKuOnMaxWhile;	//在时温、温温循环中
extern HANDLE g_hKuCtrl;
extern HANDLE g_hPropING;

BYTE g_byKuYSJSum_Set = 0;		//压缩机控制库房数，设置条数

// CSetDlgFGYSJ 对话框

IMPLEMENT_DYNAMIC(CSetDlgFGYSJ, CDialog)

CSetDlgFGYSJ::CSetDlgFGYSJ(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDlgFGYSJ::IDD, pParent)
	, m_byStartH(0)
	, m_byStartM(0)
	, m_byStopH(0)
	, m_byStopM(0)
	, m_strTimeKuNum(_T(""))
	, m_byYSJ(0)
	, m_byDW(0)
	, m_byXQ(0)
	, m_byPQ(0)
	, m_byAF(0)
	, m_byAB(0)
	, m_bySB(0)
	, m_byKuYSJKuNum(1)
	, m_byKuYSJYSJNum(1)
{

}

CSetDlgFGYSJ::~CSetDlgFGYSJ()
{
}

void CSetDlgFGYSJ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FGYD_H1, m_byStartH);
	DDX_Text(pDX, IDC_EDIT_FGYD_M1, m_byStartM);
	DDX_Text(pDX, IDC_EDIT_FGYD_H2, m_byStopH);
	DDX_Text(pDX, IDC_EDIT_FGYD_M2, m_byStopM);
	DDX_Text(pDX, IDC_EDIT_FG_KU_NUM, m_strTimeKuNum);
	DDX_Text(pDX, IDC_EDIT_YSJ, m_byYSJ);
	DDX_Text(pDX, IDC_EDIT_DW, m_byDW);
	DDX_Text(pDX, IDC_EDIT_XQ, m_byXQ);
	DDX_Text(pDX, IDC_EDIT_PQ, m_byPQ);
	DDX_Text(pDX, IDC_EDIT_AF, m_byAF);
	DDX_Text(pDX, IDC_EDIT_AB, m_byAB);
	DDX_Text(pDX, IDC_EDIT_SB, m_bySB);
	DDV_MinMaxByte(pDX, m_byStartH, 0, 23);
	DDV_MinMaxByte(pDX, m_byStartM, 0, 59);
	DDV_MinMaxByte(pDX, m_byStopH, 0, 23);
	DDV_MinMaxByte(pDX, m_byStopM, 0, 59);
	DDX_Text(pDX, IDC_EDIT_KU_YSJ_KUNUM, m_byKuYSJKuNum);
	DDV_MinMaxByte(pDX, m_byKuYSJKuNum, 1, 255);
	DDX_Text(pDX, IDC_EDIT_KU_YSJ_YSJNUM, m_byKuYSJYSJNum);
	DDV_MinMaxByte(pDX, m_byKuYSJYSJNum, 1, 255);
}


BEGIN_MESSAGE_MAP(CSetDlgFGYSJ, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FGYD_ADD, &CSetDlgFGYSJ::OnBnClickedButtonFgydAdd)
	ON_BN_CLICKED(IDC_BUTTON_FGYD_DEL, &CSetDlgFGYSJ::OnBnClickedButtonFgydDel)
	ON_BN_CLICKED(IDC_BUTTON_YSJ_OK, &CSetDlgFGYSJ::OnBnClickedButtonYsjOk)
	ON_BN_CLICKED(IDC_BUTTON_YSJ_DEL, &CSetDlgFGYSJ::OnBnClickedButtonYsjDel)
	ON_BN_CLICKED(IDC_BUTTON_FGYD_EDIT, &CSetDlgFGYSJ::OnBnClickedButtonFgydEdit)
	ON_BN_CLICKED(IDC_CHECK_ALL_KU, &CSetDlgFGYSJ::OnBnClickedCheckAllKu)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FG, &CSetDlgFGYSJ::OnNMDblclkListFg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_YSJ, &CSetDlgFGYSJ::OnNMDblclkListYsj)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SETYSJ_AF, &CSetDlgFGYSJ::OnBnClickedButtonSetysjAf)
	ON_BN_CLICKED(IDC_BUTTON_SETYSJ_AB, &CSetDlgFGYSJ::OnBnClickedButtonSetysjAb)
	ON_BN_CLICKED(IDC_CHECK_YSJ_PRIORITY, &CSetDlgFGYSJ::OnBnClickedCheckYsjPriority)
	ON_BN_CLICKED(IDC_BUTTON_KU_YSJ_YES, &CSetDlgFGYSJ::OnBnClickedButtonKuYsjYes)
	ON_BN_CLICKED(IDC_BUTTON_KU_YSJ_NO, &CSetDlgFGYSJ::OnBnClickedButtonKuYsjNo)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_KU_YSJ, &CSetDlgFGYSJ::OnNMDblclkListKuYsj)
	ON_BN_CLICKED(IDC_CHECK_KU_YSJ, &CSetDlgFGYSJ::OnBnClickedCheckKuYsj)
END_MESSAGE_MAP()
// CSetDlgFGYSJ 消息处理程序
BOOL CSetDlgFGYSJ::OnInitDialog() 
{
	CDialog::OnInitDialog();

	pSetDlgFGYSJ = this;

	FGTimeInit();
	YSJInit();
	KuYSJInit();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetDlgFGYSJ::OnDestroy()
{
	CDialog::OnDestroy();

	pSetDlgFGYSJ = NULL;

	// TODO: 在此处添加消息处理程序代码	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSetDlgFGYSJ::FGTimeInit(void)
{
	CListCtrl* pFGList = (CListCtrl*)GetDlgItem(IDC_LIST_FG);

	pFGList->SetExtendedStyle( pFGList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pFGList->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_CENTER, 45 );
	pFGList->InsertColumn( 1, LPCTSTR("起始时间"), LVCFMT_CENTER, 80 );
	pFGList->InsertColumn( 2, LPCTSTR("终止时间"), LVCFMT_CENTER, 80 );
	pFGList->InsertColumn( 3, LPCTSTR("使用库房"), LVCFMT_CENTER, 80 );

	CButton* pAllKu = (CButton*)GetDlgItem(IDC_CHECK_ALL_KU);
	pAllKu->SetCheck(1);

	FGTimeLoad();
}

void CSetDlgFGYSJ::FGTimeLoad(void)
{
	CListCtrl* pFGList = (CListCtrl*)GetDlgItem(IDC_LIST_FG);
	pFGList->DeleteAllItems();

	//char szBuffer[256];
	CString strSql = "";
	strSql = "SELECT * FROM SetFGYD ORDER BY TimeStartH, TimeStartM ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-37.1

	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{	
		CString strTimeStartH = "", strTimeStartM = "", strTimeStopH = "", strTimeStopM = "";
		int nTimeStartH, nTimeStartM, nTimeStopH, nTimeStopM;
		strTimeStartH = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeStartH"));
		strTimeStartM = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeStartM"));
		strTimeStopH = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeStopH"));
		strTimeStopM = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeStopM"));	
		nTimeStartH = atoi(strTimeStartH);
		nTimeStartM = atoi(strTimeStartM);
		nTimeStopH = atoi(strTimeStopH);
		nTimeStopM = atoi(strTimeStopM);

		CString strListEntry = "";
		strListEntry.Format("%d", nListEntry+1);
		pFGList->InsertItem( nListEntry, LPCTSTR( strListEntry ), 0 );
		//pFGList->InsertItem( nListEntry, LPCTSTR( ltoa( nListEntry+1, szBuffer, 10) ), 0 );
		CString strStartT = "";
		strStartT.Format("%02d:%02d", nTimeStartH, nTimeStartM);
		pFGList->SetItemText( nListEntry, 1, LPCTSTR(strStartT) );
		CString strStopT = "";
		strStopT.Format("%02d:%02d", nTimeStopH, nTimeStopM);
		pFGList->SetItemText( nListEntry, 2, LPCTSTR(strStopT) );

		CString strTimeKuNum = "";
		strTimeKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeKuNum"));
		pFGList->SetItemText( nListEntry, 3, LPCTSTR(strTimeKuNum) );

		nListEntry++;
		m_pRs->MoveNext();
	}

	m_pRs->raw_Close();	//RS-37.2

	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->KuCtrlDispLoad();
		g_pCanDlg->YSJCtrlDispLoad();
	}
}

void CSetDlgFGYSJ::OnBnClickedCheckAllKu()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit* pKuNum = (CEdit *)GetDlgItem(IDC_EDIT_FG_KU_NUM);
	CButton* pAllKu = (CButton*)GetDlgItem(IDC_CHECK_ALL_KU);
	if(pAllKu->GetCheck())
	{
		UpdateData(TRUE);
		m_strTimeKuNum = "";
		UpdateData(FALSE);
		pKuNum->EnableWindow(FALSE);
	} 
	else
	{
		pKuNum->EnableWindow(TRUE);
	}	
}
void CSetDlgFGYSJ::OnBnClickedButtonFgydAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strSql = "";

	//150907
	CButton* pAllKu = (CButton*)GetDlgItem(IDC_CHECK_ALL_KU);
	if(pAllKu->GetCheck())
	{
		m_strTimeKuNum = "全部库房";
	} 
	else
	{
		BYTE *aKuNum = (BYTE *)malloc(g_byKuSum_SetKu * sizeof(BYTE));
		int nPartKuSum = Str2Dec(m_strTimeKuNum, aKuNum);
		free(aKuNum);
		if(nPartKuSum > g_byKuOnMax_SetKu)
		{
			MessageBox("部分库房总数，暂不支持 大于 同开最多数");
			return;
		}

		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1 AND KuCtrlMode = '温开温关'";
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-38.1
		if(!m_pRs->BOF)			//未存在该库房数
		{
			MessageBox("部分库房总数，暂不支持 温开温关");
			m_pRs->raw_Close();	//RS-38.2.1
			return;
		}
		else
		{
			m_pRs->raw_Close();	//RS-38.2.2
		}
	}

	strSql.Format("SELECT * From SetFGYD WHERE [TimeStartH]=%d and [TimeStartM]=%d and [TimeStopH]=%d and [TimeStopM]=%d"
		,m_byStartH ,m_byStartM ,m_byStopH ,m_byStopM);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-39.1
	if(!m_pRs->BOF)	//已存在该峰谷用电时间段
	{
		MessageBox("峰谷用电时间段已存在，将覆盖");
		strSql.Format("DELETE FROM SetFGYD WHERE [TimeStartH]=%d and [TimeStartM]=%d and [TimeStopH]=%d and [TimeStopM]=%d"
			,m_byStartH ,m_byStartM ,m_byStopH ,m_byStopM);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}
	m_pRs->raw_Close();	//RS-39.2
	
	if( (m_strTimeKuNum != "")&&(m_byStartH>=0)&&(m_byStartH<=23)&&(m_byStopH>=0)&&(m_byStopH<=23)&&(m_byStartM>=0)&&(m_byStartM<=59)&&(m_byStopM>=0)&&(m_byStopM<=59) )
	{
		CString strFGYDSetText = "";
		
		strSql.Format("INSERT INTO SetFGYD VALUES (%d,%d,%d,%d,'%s')"
			,m_byStartH,m_byStartM,m_byStopH,m_byStopM,m_strTimeKuNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		FGTimeLoad();

		strFGYDSetText.Format("设置·增加时间 %d:%d - %d:%d  %s",m_byStartH,m_byStartM,m_byStopH,m_byStopM,m_strTimeKuNum);
		Doing2Text(strFGYDSetText);
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_FG);
	int nCount = pList->GetItemCount();
	if(nCount > 0)
	{
		pList->EnsureVisible(nCount-1, 0);
	}

	MessageBox("请确认：库房设置-时间保护");
}

void CSetDlgFGYSJ::OnBnClickedButtonFgydDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pFGList = (CListCtrl*) GetDlgItem(IDC_LIST_FG);
	int nKuIndex = pFGList->GetNextItem(-1, LVNI_SELECTED);

	if(nKuIndex != -1)
	{
		CString strFGYDSetText = "";
		
		CString strStart = pFGList->GetItemText(nKuIndex, 1);
		CString strStop = pFGList->GetItemText(nKuIndex, 2);

		BYTE byStart[2] = {0,0};
		BYTE byStop[2] = {0,0};
		Time2Dec(strStart, byStart);
		Time2Dec(strStop, byStop);

		BYTE byStartH = byStart[0];
		BYTE byStartM = byStart[1];
		BYTE byStopH = byStop[0];
		BYTE byStopM = byStop[1];

		CString strSql = "";
		strSql.Format("DELETE FROM SetFGYD where [TimeStartH]=%d and [TimeStartM]=%d and [TimeStopH]=%d and [TimeStopM]=%d"
						  ,byStartH,byStartM,byStopH,byStopM);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		FGTimeLoad();

		strFGYDSetText.Format("设置·删除时间 %d:%d - %d:%d",byStartH,byStartM,byStopH,byStopM);
		Doing2Text(strFGYDSetText);
	}
}

void CSetDlgFGYSJ::OnBnClickedButtonFgydEdit()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pFGList = (CListCtrl*) GetDlgItem(IDC_LIST_FG);
	int nKuIndex = pFGList->GetNextItem(-1, LVNI_SELECTED);

	if(nKuIndex > -1)
	{
		CString strStart = pFGList->GetItemText(nKuIndex, 1);
		CString strStop = pFGList->GetItemText(nKuIndex, 2);

		char byStart[2] = {0,0};
		char byStop[2] = {0,0};
		Str2Hex(strStart, byStart);
		Str2Hex(strStop, byStop);
		
		m_byStartH = CharDec(byStart[0]);
		m_byStartM = CharDec(byStart[1]);
		m_byStopH = CharDec(byStop[0]);
		m_byStopM = CharDec(byStop[1]);

		CString strKuNum = "";
		strKuNum = pFGList->GetItemText(nKuIndex, 3);
		
		CButton* pAllKu = (CButton*)GetDlgItem(IDC_CHECK_ALL_KU);
		CEdit* pKuNum = (CEdit *)GetDlgItem(IDC_EDIT_FG_KU_NUM);
		if(strcmp(strKuNum, "全部库房") == 0)
		{
			pAllKu->SetCheck(1);
			pKuNum->EnableWindow(FALSE);
			m_strTimeKuNum = "";
		}
		else
		{
			pAllKu->SetCheck(0);
			pKuNum->EnableWindow(TRUE);
			m_strTimeKuNum = strKuNum;
		}
		
		UpdateData(FALSE);

		CString strSql = "";
		strSql.Format("DELETE FROM SetFGYD where [TimeStartH]=%d and [TimeStartM]=%d and [TimeStopH]=%d and [TimeStopM]=%d"
						  ,m_byStartH,m_byStartM,m_byStopH,m_byStopM);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		FGTimeLoad();
	}
}
void CSetDlgFGYSJ::OnNMDblclkListFg(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	OnBnClickedButtonFgydEdit();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSetDlgFGYSJ::YSJInit(void)
{
	CListCtrl* pYSJList = (CListCtrl*)GetDlgItem(IDC_LIST_YSJ);

	pYSJList->SetExtendedStyle( pYSJList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pYSJList->InsertColumn( 0, LPCTSTR("压缩机"), LVCFMT_CENTER, 60 );
	pYSJList->InsertColumn( 1, LPCTSTR("档位"), LVCFMT_CENTER, 45 );
	pYSJList->InsertColumn( 2, LPCTSTR("吸气"), LVCFMT_CENTER, 45 );
	pYSJList->InsertColumn( 3, LPCTSTR("排气"), LVCFMT_CENTER, 45 );
	pYSJList->InsertColumn( 4, LPCTSTR("氨阀"), LVCFMT_CENTER, 45 );
	pYSJList->InsertColumn( 5, LPCTSTR("氨泵"), LVCFMT_CENTER, 45 );
	pYSJList->InsertColumn( 6, LPCTSTR("水泵"), LVCFMT_CENTER, 45 );
	pYSJList->InsertColumn( 7, LPCTSTR("水"), LVCFMT_CENTER, 30 );
	pYSJList->InsertColumn( 8, LPCTSTR("转"), LVCFMT_CENTER, 30 );
	pYSJList->InsertColumn( 9, LPCTSTR("优先级"), LVCFMT_CENTER, 60 );

	CButton* pWT = (CButton*)GetDlgItem(IDC_CHECK_YSJ_WT);
	CButton* pRT = (CButton*)GetDlgItem(IDC_CHECK_YSJ_RT);
	pWT->SetCheck(1);
	pRT->SetCheck(1);

	YSJLoad();
}

void CSetDlgFGYSJ::YSJLoad(void)
{
	CListCtrl* pYSJList = (CListCtrl*)GetDlgItem(IDC_LIST_YSJ);
	pYSJList->DeleteAllItems();

	CString strSql = "";
	strSql = "SELECT * FROM SetYSJ WHERE YSJ > 0 ORDER BY YSJ ASC";
	m_pRs->Open((_variant_t)strSql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//RS-40.1

	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{	
		CString strTem = "";
		strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)1)->Value;
		pYSJList->InsertItem( nListEntry, LPCTSTR( strTem ), 0 );
		
		int nCount = m_pRs->GetFields()->Count;
		for (int i=2; i<nCount; i++)
		{
			strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)i)->Value;
			if((i<nCount-3) || (i == nCount-1))
			{
				pYSJList->SetItemText( nListEntry, i-1, LPCTSTR( strTem ) );
			}
			else
			{
				(strcmp(strTem, "0")==0)? 
					pYSJList->SetItemText( nListEntry, i-1, LPCTSTR( "关" ) ):
					pYSJList->SetItemText( nListEntry, i-1, LPCTSTR( "开" ) );
			}
			
		}


		nListEntry++;
		m_pRs->MoveNext();
	}

	m_pRs->raw_Close();	//RS-40.2

	//及时更新
	g_byYSJSum_SetKu = nListEntry;

	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->KuCtrlDispLoad();
		g_pCanDlg->YSJCtrlDispLoad();
	}

	PriYSJLoad();
}

void CSetDlgFGYSJ::PriYSJLoad(void)
{
	//151105 压缩机使用的优先级
	CButton* pCheckPriYSJ = (CButton*)GetDlgItem(IDC_CHECK_YSJ_PRIORITY);
	CEdit* pEditPriYSJ = (CEdit *)GetDlgItem(IDC_EDIT_YSJ_PRIORITY);
	CString strSql = "SELECT * FROM SetYSJ WHERE YSJ > 0 ORDER BY PRI , YSJ ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-1.1
	int nYSJIndex = 0;
	CString strPriYSJ = "";
	while(!m_pRs->adoEOF)
	{
		CString strYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("YSJ"));
		int nYSJ = atoi(strYSJ);
		
		strPriYSJ += strYSJ;
		if(nYSJIndex < g_byYSJSum_SetKu-1)
			strPriYSJ += ">";

		nYSJIndex ++;
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//151105-1.2
	pEditPriYSJ->SetWindowTextA(strPriYSJ);

	if(strcmp(strPriYSJ, "") != 0)
	{
		pCheckPriYSJ->SetCheck(1);
		pEditPriYSJ->EnableWindow(0);
	}
}
void CSetDlgFGYSJ::OnBnClickedButtonYsjOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CString strSql = "";
	strSql.Format("SELECT * From SetYSJ WHERE YSJ = %d", m_byYSJ);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-41.1
	int nPri = 0;
	if(!m_pRs->BOF)	//已存在该峰谷用电时间段
	{
		CString strPri = (LPCSTR)_bstr_t(m_pRs->GetCollect("PRI"));
		nPri = atoi(strPri);
		
		CString strRe = "";
		strRe.Format("压缩机%d 已存在，将覆盖", m_byYSJ);
		MessageBox(strRe);
		strSql.Format("DELETE FROM SetYSJ WHERE YSJ = %d", m_byYSJ);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}
	m_pRs->raw_Close();	//RS-41.2

	if( (m_byYSJ != 0) && (m_byDW != 0) )
	{
		//151029
		BOOL bWT = TRUE;
		BOOL bRT = TRUE;
		//CButton* pWT = (CButton*)GetDlgItem(IDC_CHECK_YSJ_WT);
		//CButton* pRT = (CButton*)GetDlgItem(IDC_CHECK_YSJ_RT);
		//(pWT->GetCheck())? bWT=TRUE: bWT=FALSE;
		//(pRT->GetCheck())? bRT=TRUE: bRT=FALSE;

		strSql.Format("INSERT INTO SetYSJ ([YSJ],[DW],[XQ],[PQ],[AF],[AB],[SB],[WT],[RT],[PRI]) \
					  VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
					  m_byYSJ, m_byDW, m_byXQ, m_byPQ, m_byAF, m_byAB, m_bySB, bWT, bRT, nPri);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		YSJLoad();	
	}

	//CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_YSJ);
	//int nCount = pList->GetItemCount();
	//if(nCount > 0)
	//{
	//	pList->EnsureVisible(nCount-1, 0);
	//}

	Doing2Text("设置·加压缩机");
}

void CSetDlgFGYSJ::OnBnClickedButtonYsjDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pYSJList = (CListCtrl*)GetDlgItem(IDC_LIST_YSJ);
	int nKuIndex = pYSJList->GetNextItem(-1, LVNI_SELECTED);

	if(nKuIndex != -1)
	{
		CString strYSJ = pYSJList->GetItemText(nKuIndex, 0);
		int nCurYSJ = atoi(strYSJ);
		CString strSql = "";
		strSql.Format("DELETE FROM SetYSJ WHERE YSJ = %d", nCurYSJ);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		YSJLoad();

		Doing2Text("设置·删压缩机");
	}
}

void CSetDlgFGYSJ::OnNMDblclkListYsj(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CListCtrl* pYSJList = (CListCtrl*) GetDlgItem(IDC_LIST_YSJ);
	int nYSJIndex = pYSJList->GetNextItem(-1, LVNI_SELECTED);

	if(nYSJIndex > -1)
	{
		CString strYSJItem;
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 0);
		m_byYSJ = atoi(strYSJItem);
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 1);
		m_byDW = atoi(strYSJItem);
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 2);
		m_byXQ = atoi(strYSJItem);
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 3);
		m_byPQ = atoi(strYSJItem);
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 4);
		m_byAF = atoi(strYSJItem);
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 5);
		m_byAB = atoi(strYSJItem);
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 6);
		m_bySB = atoi(strYSJItem);
		UpdateData(FALSE);
		
		CButton* pWT = (CButton*)GetDlgItem(IDC_CHECK_YSJ_WT);
		CButton* pRT = (CButton*)GetDlgItem(IDC_CHECK_YSJ_RT);
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 7);
		(strcmp(strYSJItem, "关") == 0)? pWT->SetCheck(0): pWT->SetCheck(1);
		strYSJItem = pYSJList->GetItemText(nYSJIndex, 8);
		(strcmp(strYSJItem, "关") == 0)? pRT->SetCheck(0): pRT->SetCheck(1);
	}
}

void CSetDlgFGYSJ::OnBnClickedButtonSetysjAf()
{
	CString strSql = "";
	int nMSGB = AfxMessageBox("将统一修改 氨阀", MB_YESNO);
	if(nMSGB == IDYES)
	{
		UpdateData(TRUE);
		if(m_byAF)
		{
			strSql.Format("SELECT * FROM SetCid WHERE IdItem = '氨阀' AND IdItemNum = %d", m_byAF);
			m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-42.1
			if(!m_pRs->BOF)
			{
				m_pRs->raw_Close();	//RS-42.2.1
				strSql.Format("UPDATE SetYSJ SET [AF] = %d", m_byAF);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
				YSJLoad();	//内部使用m_pRs
			}
			else
			{
				m_pRs->raw_Close();	//RS-42.2.2
				MessageBox("请正确填写氨阀号码");
			}
		}
		else
		{
			//没有输入
			MessageBox("请正确填写氨阀序号");
		}
	}
}

void CSetDlgFGYSJ::OnBnClickedButtonSetysjAb()
{
	CString strSql = "";
	int nMSGB = AfxMessageBox("将统一修改 氨泵", MB_YESNO);
	if(nMSGB == IDYES)
	{
		UpdateData(TRUE);
		if(m_byAB)
		{
			strSql.Format("SELECT * FROM SetCid WHERE IdItem = '氨泵' AND IdItemNum = %d", m_byAB);
			m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-43.1
			if(!m_pRs->BOF)
			{
				m_pRs->raw_Close();	//RS-43.2.1
				strSql.Format("UPDATE SetYSJ SET [AB] = %d", m_byAB);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
				YSJLoad();	//内部使用m_pRs
			}
			else
			{
				m_pRs->raw_Close();	//RS-43.2.2
				MessageBox("请正确填写氨泵号码");
			}
		}
		else
		{
			//没有输入
			MessageBox("请正确填写氨泵序号");
		}
	}
}

void CSetDlgFGYSJ::OnBnClickedCheckYsjPriority()
{
	CButton* pCheckPriYSJ = (CButton*)GetDlgItem(IDC_CHECK_YSJ_PRIORITY);
	CEdit* pEditPriYSJ = (CEdit *)GetDlgItem(IDC_EDIT_YSJ_PRIORITY);
	if(pCheckPriYSJ->GetCheck())
	{
		CString strPriYSJ = "";
		pEditPriYSJ->GetWindowTextA(strPriYSJ);
		pEditPriYSJ->EnableWindow(FALSE);

		BYTE *aPriYSJ = new BYTE[g_byYSJSum_SetKu];
		Priority2Dec(strPriYSJ, aPriYSJ);
		
		//填写的压缩机序号必须存在，且必须为大于号
		for(int i=0; i<g_byYSJSum_SetKu; i++)
		{
			if( (aPriYSJ[i] <= 0) || (aPriYSJ[i] > g_byYSJSum_SetKu) )
			{
				MessageBox("请填写正确的 压缩机序号 与 大于号");
				delete [] aPriYSJ;
				pCheckPriYSJ->SetCheck(0);
				pEditPriYSJ->EnableWindow(TRUE);
				return;
			}
		}

		//压缩机序号不能重复，如果没有优先级，则按压缩机序号填写
		for(int i=0; i<g_byYSJSum_SetKu-1; i++)
		{
			for(int j=i+1; j<g_byYSJSum_SetKu; j++)
			{
				if(aPriYSJ[i] == aPriYSJ[j])
				{
					MessageBox("请填写正确的 压缩机序号 与 大于号");
					delete [] aPriYSJ;
					pCheckPriYSJ->SetCheck(0);
					pEditPriYSJ->EnableWindow(TRUE);
					return;				
				}
			}
		}
		
		for(int i=0; i<g_byYSJSum_SetKu; i++)
		{			
			CString strSql = "";
			strSql.Format("UPDATE SetYSJ SET [PRI] = %d WHERE YSJ = %d", i+1, aPriYSJ[i]);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		
		YSJLoad();

		delete [] aPriYSJ;
	}
	else
	{
		pEditPriYSJ->EnableWindow(TRUE);
	}
}


void CSetDlgFGYSJ::KuYSJInit(void)
{
	CListCtrl* pKuYSJList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_YSJ);
	pKuYSJList->SetExtendedStyle( pKuYSJList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线
	pKuYSJList->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_CENTER, 45  );
	pKuYSJList->InsertColumn( 1, LPCTSTR("压缩机数"), LVCFMT_CENTER, 80 );
	pKuYSJList->InsertColumn( 2, LPCTSTR("库房数"), LVCFMT_CENTER, 60 );
	KuYSJLoad();

	CString strSql = "SELECT * FROM SetKu WHERE KuNum = 0";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-8.1
	CString strEnKuYSJ =  (LPCSTR)_bstr_t(m_pRs->GetCollect("KuOnlyOffFJ"));
	m_pRs->raw_Close();	//151105-8.2
	(strcmp(strEnKuYSJ, "0") == 0)? g_bEnKuYSJ = FALSE: g_bEnKuYSJ = TRUE;
	CButton *pBtnCheckKuYSJ = (CButton*) GetDlgItem(IDC_CHECK_KU_YSJ);
	pBtnCheckKuYSJ->SetCheck(g_bEnKuYSJ);

	GetDlgItem(IDC_BUTTON_KU_YSJ_YES)->EnableWindow(!g_bEnKuYSJ);
	GetDlgItem(IDC_BUTTON_KU_YSJ_NO)->EnableWindow(!g_bEnKuYSJ);
	GetDlgItem(IDC_CHECK_YSJ_PRIORITY)->EnableWindow(!g_bEnKuYSJ);
}
void CSetDlgFGYSJ::KuYSJLoad(void)
{
	CListCtrl* pKuYSJList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_YSJ);
	pKuYSJList->DeleteAllItems();

	CString strSql = "";
	strSql = "SELECT * FROM SetKuYSJ ORDER BY YSJNum ASC";
	m_pRs->Open((_variant_t)strSql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//151105-3.1
	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{	
		CString strTem = "";
		strTem.Format("%d", nListEntry+1);
		pKuYSJList->InsertItem( nListEntry, LPCTSTR( strTem ), 0 );
		
		int nCount = m_pRs->GetFields()->Count;
		for (int i=1; i<nCount; i++)
		{
			strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)i)->Value;
			pKuYSJList->SetItemText( nListEntry, i, LPCTSTR( strTem ) );	
		}

		nListEntry++;
		m_pRs->MoveNext();
	}

	m_pRs->raw_Close();	//151105-3.2
}
void CSetDlgFGYSJ::OnBnClickedButtonKuYsjYes()
{
	UpdateData(TRUE);

	if(m_byKuYSJYSJNum<g_byYSJSum_SetKu+1 && m_byKuYSJKuNum<g_byKuSum_SetKu+1)
	{		
		CString strSql = "";
		strSql.Format("SELECT * From SetKuYSJ WHERE YSJNum = %d", m_byKuYSJYSJNum);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-2.1
		if(!m_pRs->BOF)
		{
			CString strRe = "";
			strRe.Format("%d台压缩机控制库房数 已存在，将覆盖", m_byKuYSJYSJNum);
			MessageBox(strRe);
			strSql.Format("DELETE FROM SetKuYSJ WHERE YSJNum = %d", m_byKuYSJYSJNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	
		}
		m_pRs->raw_Close();	//151105-2.2		
		
		if(m_byKuYSJYSJNum == g_byYSJSum_SetKu)
			m_byKuYSJKuNum = g_byKuSum_SetKu;
		
		strSql.Format("INSERT INTO SetKuYSJ ([YSJNum],[KuNum]) VALUES (%d,%d)", m_byKuYSJYSJNum, m_byKuYSJKuNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		KuYSJLoad();			
	}
	else
	{
		MessageBox("请填写正确数字");
	}
}
void CSetDlgFGYSJ::OnBnClickedButtonKuYsjNo()
{
	CListCtrl* pKuYSJList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_YSJ);
	int nKuYSJIndex = pKuYSJList->GetNextItem(-1, LVNI_SELECTED);

	if(nKuYSJIndex != -1)
	{
		CString strYSJNum = pKuYSJList->GetItemText(nKuYSJIndex, 1);
		int nCurYSJNum = atoi(strYSJNum);
		CString strSql = "";
		strSql.Format("DELETE FROM SetKuYSJ WHERE YSJNum = %d", nCurYSJNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		KuYSJLoad();
	}
}
void CSetDlgFGYSJ::OnNMDblclkListKuYsj(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	CListCtrl* pKuYSJList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_YSJ);
	int nKuYSJIndex = pKuYSJList->GetNextItem(-1, LVNI_SELECTED);

	if(nKuYSJIndex > -1)
	{
		CString strYSJNum = pKuYSJList->GetItemText(nKuYSJIndex, 1);
		m_byKuYSJYSJNum = atoi(strYSJNum);
		CString strKuNum = pKuYSJList->GetItemText(nKuYSJIndex, 2);
		m_byKuYSJKuNum = atoi(strKuNum);
		UpdateData(FALSE);
	}
}

void CSetDlgFGYSJ::OnBnClickedCheckKuYsj()
{

	CButton *pBtnCheckKuYSJ = (CButton*) GetDlgItem(IDC_CHECK_KU_YSJ);
	BOOL bChecked = pBtnCheckKuYSJ->GetCheck();
	
	if(bChecked)	
	{
		BOOL bEnKuYSJ = EnKuYSJ();
		pBtnCheckKuYSJ->SetCheck(bEnKuYSJ);
		bChecked = pBtnCheckKuYSJ->GetCheck();
	}

	CString strSql = "";
	strSql.Format("UPDATE SetKu SET [KuOnlyOffFJ] = %d WHERE KuNum = 0", bChecked);
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	CString strEnKuYSJ = "";
	bChecked? strEnKuYSJ = "将执行库房数开关压缩机": strEnKuYSJ = "不执行库房数开关压缩机";
	MessageBox(strEnKuYSJ);

	GetDlgItem(IDC_BUTTON_KU_YSJ_YES)->EnableWindow(!bChecked);
	GetDlgItem(IDC_BUTTON_KU_YSJ_NO)->EnableWindow(!bChecked);
	GetDlgItem(IDC_CHECK_YSJ_PRIORITY)->EnableWindow(!bChecked);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GetYSJSt()
{
	if(!g_byYSJSum_SetKu)	return 0;

	if(g_pStYSJ != NULL)
	{
		delete [] g_pStYSJ;
		g_pStYSJ = NULL;
	}
	g_pStYSJ = new YSJ_t[g_byYSJSum_SetKu];
	for(int i=0; i<g_byYSJSum_SetKu; i++)
	{
		memset(&g_pStYSJ[i], 0, sizeof(YSJ_t));
	}

	CString strSql = "";
	strSql = "SELECT * FROM SetYSJ WHERE YSJ > 0 ORDER BY YSJ ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151029-1.1
	int nYSJIndex = 0;
	while(!m_pRs->adoEOF)
	{
		CString strYSJ = "", strDW = "", strXQ = "", strPQ = "", strAF = "", strAB = "", strSB = "", strPri = "";
		strYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("YSJ"));
		g_pStYSJ[nYSJIndex].byYSJ = atoi(strYSJ);
		strDW = (LPCSTR)_bstr_t(m_pRs->GetCollect("DW"));
		g_pStYSJ[nYSJIndex].byDW = atoi(strDW);
		strXQ = (LPCSTR)_bstr_t(m_pRs->GetCollect("XQ"));
		g_pStYSJ[nYSJIndex].byXQ = atoi(strXQ);
		strPQ = (LPCSTR)_bstr_t(m_pRs->GetCollect("PQ"));
		g_pStYSJ[nYSJIndex].byPQ = atoi(strPQ);
		strAF = (LPCSTR)_bstr_t(m_pRs->GetCollect("AF"));
		g_pStYSJ[nYSJIndex].byAF = atoi(strAF);
		strAB = (LPCSTR)_bstr_t(m_pRs->GetCollect("AB"));
		g_pStYSJ[nYSJIndex].byAB = atoi(strAB);
		strSB = (LPCSTR)_bstr_t(m_pRs->GetCollect("SB"));
		g_pStYSJ[nYSJIndex].bySB = atoi(strSB);
		strPri = (LPCSTR)_bstr_t(m_pRs->GetCollect("PRI"));
		g_pStYSJ[nYSJIndex].byPri = atoi(strPri);
		
		nYSJIndex ++;
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//151029-1.2.2

	return 1;
}
int GetYSJIndex_St(BYTE byYSJNum)
{
	int nYSJIndex = -1;
	for(int i=0; i<g_byYSJSum_SetKu; i++)
	{
		if(g_pStYSJ[i].byYSJ == byYSJNum)
		{
			nYSJIndex = i;
			break;
		}	
	}
	return nYSJIndex;
}
void YSJOnOffSel(CString strFunc, int nYSJIndex, BOOL bResetPropYSJ)
{
	//流程执行查询数组配置
	CString aYSJProp[6]={"水泵", "压缩机", "吸气", "排气", "氨阀", "氨泵"};

	CString strSql = "";
	if(bResetPropYSJ)
	{
		strSql = "DELETE FROM PropYSJ";
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}

	for(int i=0; i<6; i++)
	{
		int nYSJItemNum = 0;
		if(strcmp(aYSJProp[i], "水泵") == 0)
		{
			nYSJItemNum = g_pStYSJ[nYSJIndex].bySB;
			
			//去除共用
			for(int j=0; j<g_byYSJSum_SetKu; j++)
			{
				if(j == nYSJIndex)
					continue;
				if(g_pStYSJ[j].bySB == g_pStYSJ[nYSJIndex].bySB)
				{
					if(g_pStYSJ[j].bNC) 
					{
						nYSJItemNum = 0;
						continue;
					}
				}
			}

			if(nYSJItemNum == 0)
				continue;
		}
		else if(strcmp(aYSJProp[i], "压缩机") == 0)
		{
			nYSJItemNum = g_pStYSJ[nYSJIndex].byYSJ;
			if(nYSJItemNum == 0)
				continue;
		}
		else if(strcmp(aYSJProp[i], "吸气") == 0)
		{
			nYSJItemNum = g_pStYSJ[nYSJIndex].byXQ;
			if(nYSJItemNum == 0)
				continue;
		}
		else if(strcmp(aYSJProp[i], "排气") == 0)
		{
			nYSJItemNum = g_pStYSJ[nYSJIndex].byPQ;
			if(nYSJItemNum == 0)
				continue;
		}
		else if(strcmp(aYSJProp[i], "氨阀") == 0)
		{
			nYSJItemNum = g_pStYSJ[nYSJIndex].byAF;

			//去除共用
			for(int j=0; j<g_byYSJSum_SetKu; j++)
			{
				if(j == nYSJIndex)
					continue;
				if(g_pStYSJ[j].byAF == g_pStYSJ[nYSJIndex].byAF)
				{
					if(g_pStYSJ[j].bNC) 
					{
						nYSJItemNum = 0;
						continue;
					}
				}
			}

			if(nYSJItemNum == 0)
				continue;
		}
		else if(strcmp(aYSJProp[i], "氨泵") == 0)
		{
			nYSJItemNum = g_pStYSJ[nYSJIndex].byAB;

			//去除共用
			for(int j=0; j<g_byYSJSum_SetKu; j++)
			{
				if(j == nYSJIndex)
					continue;
				if(g_pStYSJ[j].byAB == g_pStYSJ[nYSJIndex].byAB)
				{
					if(g_pStYSJ[j].bNC) 
					{
						nYSJItemNum = 0;
						continue;
					}
				}
			}

			if(nYSJItemNum == 0)
				continue;
		}

		strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' AND Item = '%s' AND Num = %d", 
			strFunc, aYSJProp[i], nYSJItemNum);
		m_pRs_ManualYSJ = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151030-1.1
		while(!m_pRs_ManualYSJ->adoEOF)
		{
			CString strYSJItemZu = "";
			CString strYSJItemZuNum = "";
			int nYSJItemZuNum = 0;
			strYSJItemZu = (LPCSTR)_bstr_t(m_pRs_ManualYSJ->GetCollect("ZuItem"));
			strYSJItemZuNum = (LPCSTR)_bstr_t(m_pRs_ManualYSJ->GetCollect("ZuNum"));
			nYSJItemZuNum = atoi(strYSJItemZuNum);	

			strSql.Format("SELECT * FROM PropYSJ WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
				, strFunc, strYSJItemZu, nYSJItemZuNum);
			m_pRs_ManualYSJ_In = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151030-2.1
			if(m_pRs_ManualYSJ_In->BOF)
			{
				strSql.Format("INSERT INTO PropYSJ\
							  SELECT [ID],[Func],[NO],[Item],[Num],[Dir],[DWKD],[NC],[ZuItem],[ZuNum],[Flag] \
							  FROM SetProp WHERE Func = '%s' AND ZuItem = '%s' AND ZuNum = %d"
							  , strFunc, strYSJItemZu, nYSJItemZuNum);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
			m_pRs_ManualYSJ_In->raw_Close();		//151030-2.2

			m_pRs_ManualYSJ->MoveNext();
		}
		m_pRs_ManualYSJ->raw_Close();	//151030-1.2
	}

	strSql = "DELETE FROM PropYSJ WHERE (Item = '延时' AND Num = 2) OR (Item = '延时' AND Num = 3)";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	strSql = "UPDATE PropYSJ SET [Flag] = 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
}
void YSJOnOffExe(CString strFunc, CEdit *pEdit, int nYSJIndex)
{
	YSJOnOffSel(strFunc, nYSJIndex, TRUE);
	SP_Delay3(pEdit);

	CString strText = "";
	strText.Format("压缩机%d, %s, 开始执行", g_pStYSJ[nYSJIndex].byYSJ, strFunc);
	pEdit->SetWindowTextA(strText);
	Doing2Text(strText);
	Sleep(1500);

	g_StProp.Func = strFunc;
	g_StProp.pEdit = pEdit;
	g_StProp.byKuNum = 0;	//无用

	if(g_hKuYSJ == NULL)
	{
		CWinThread* pThread = AfxBeginThread(ThreadProc_Prop_YSJ, &g_StProp, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		g_hKuYSJ = pThread->m_hThread;
	}

	SP_KuCtrl();
}
UINT ThreadProc_Prop_YSJ(LPVOID wParam)
{
	SP_CidRefer();
	
	//参数传递
	Prop_t *pStProp = (Prop_t *) wParam;
	CString strFunc = pStProp->Func;
	CEdit *pEdit = pStProp->pEdit;
	
	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->PropListLoad("PropYSJ", strFunc);
		g_pCanDlg->GetDlgItem(IDC_CHECK_YSJ_MANUAL)->EnableWindow(FALSE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_YSJ_OFF_MANUAL)->EnableWindow(FALSE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_YSJ_ON_MANUAL)->EnableWindow(FALSE);
	}
	
	CString strSql = "";
	strSql.Format("SELECT * FROM PropYSJ left JOIN SetCid \
				  ON PropYSJ.Item = SetCid.IdItem AND PropYSJ.Num = SetCid.IdItemNum \
				  WHERE Func = '%s' ORDER BY NO ASC", strFunc);	
	m_pRs_ManualYSJ = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151030-3.1
	if(!m_pRs_ManualYSJ->BOF)
	{
		g_bStopKuYSJ = FALSE;
		PropExecute(m_pRs_ManualYSJ, strFunc, pEdit);
	}
	m_pRs_ManualYSJ->raw_Close();	//151030-3.2

	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->GetDlgItem(IDC_CHECK_YSJ_MANUAL)->EnableWindow(TRUE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_YSJ_OFF_MANUAL)->EnableWindow(TRUE);
		g_pCanDlg->GetDlgItem(IDC_BUTTON_YSJ_ON_MANUAL)->EnableWindow(TRUE);
		
		g_bInDelay3 ?
			g_pCanDlg->PropListLoad("PropCHSH", "关机流程"):
			g_pCanDlg->PropListLoad("PropYSJ", "清空流程");

		CString strText = "";
		strText.Format("压缩机%s, 执行结束", strFunc);
		Doing2Text(strText);
		pEdit->SetWindowTextA(strText);
		Sleep(1500);
	}	

	g_hKuYSJ = NULL;

	ST_PropING();	//151116 - 过程中没有SP，会执行完毕，复位g_hPropING = NULL
	if(g_hPropING == NULL)	//151116 - 之前过程分别SP了下列线程
	{
		ST_KuCtrl();
		ST_Delay3(pEdit);
		ST_CidRefer();	
	}
	
	return 1;
}
BOOL EnKuYSJ_SetKu()
{
	CButton *pBtnCheckKuYSJ = NULL;
	if(pSetDlgFGYSJ != NULL)
		pBtnCheckKuYSJ = (CButton*) pSetDlgFGYSJ->GetDlgItem(IDC_CHECK_KU_YSJ);

	CString strSql = "";

	strSql = "SELECT * FROM SetYSJ WHERE YSJ > 0 ORDER BY YSJ ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-19.1
	CString strYSJAll = "";
	while(!m_pRs->adoEOF)
	{
		CString strYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("YSJ"));
		strYSJAll += strYSJ;
		m_pRs->MoveNext();
		if(!m_pRs->adoEOF)
		{
			strYSJAll += ",";
		}
	}
	m_pRs->raw_Close();	//RS-19.2
	
	int nKuSumTem = 0;
	int nKuSumIndex = -1;
	if(strcmp(strYSJAll, "") == 0)
	{
		AfxMessageBox("请先进行压缩机设置");
	}
	else
	{
		strSql = "SELECT * FROM SetKu WHERE KuNum > 0 AND KuStatus = 1";
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151110-1.1
		while(!m_pRs->adoEOF)
		{
			nKuSumTem++;
			m_pRs->MoveNext();
		}
		m_pRs->MoveFirst();
		nKuSumIndex = 0;
		while(!m_pRs->adoEOF)
		{
			CString strKuYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuYSJ"));
			if(strcmp(strKuYSJ, strYSJAll) != 0)
			{
				Doing2Text("库房数自动控制压缩机，暂时需要库房统一使用全部压缩机");
				break;
			}	
			nKuSumIndex++;
			m_pRs->MoveNext();
		}
		m_pRs->raw_Close();	//151110-1.2
	}

	strSql = "SELECT * FROM SetKu WHERE KuNum = 0";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-7.1
	CString strEnKuYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuOnlyOffFJ"));
	m_pRs->raw_Close();	//151105-7.2
	

	if(nKuSumIndex == nKuSumTem)
	{
		if(pSetDlgFGYSJ != NULL)
			pBtnCheckKuYSJ->EnableWindow(TRUE);
		return TRUE;
	}
	else
	{
		strSql = "UPDATE SetKu SET [KuOnlyOffFJ] = 0 WHERE KuNum = 0";
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		if(pSetDlgFGYSJ != NULL)
		{
			pBtnCheckKuYSJ->EnableWindow(FALSE);
			pBtnCheckKuYSJ->SetCheck(0);
		}
		return FALSE;
	}
}
BOOL EnKuYSJ()
{
	//首先判断压缩机设置的优先级是否正确
	CString strSql = "SELECT * FROM SetYSJ WHERE YSJ > 0 ORDER BY PRI ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-4.1
	if(m_pRs->BOF)
	{
		AfxMessageBox("库房数控制压缩机 - 请设置压缩机");
		m_pRs->raw_Close();	//151105-4.2.1
		return FALSE;
	}
	else
	{
		while(!m_pRs->adoEOF)
		{
			CString strPri = (LPCSTR)_bstr_t(m_pRs->GetCollect("PRI"));	
			int nPri = atoi(strPri);
			if( nPri<1 || nPri>g_byYSJSum_SetKu )
			{
				AfxMessageBox("库房数控制压缩机 - 请正确设置压缩机优先级");
				m_pRs->raw_Close();	//151105-4.2.2
				return FALSE;
			}

			m_pRs->MoveNext();
		}
		m_pRs->raw_Close();	//151105-4.2.3
	}

	//检查是否有超过压缩机总数的无效设置
	strSql.Format("SELECT * FROM SetKuYSJ WHERE YSJNum > %d", g_byYSJSum_SetKu);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-9.1
	if(!m_pRs->BOF)
	{
		m_pRs->raw_Close();	//151105-9.2.1
		AfxMessageBox("库房数控制压缩机 - 请设置正确压缩机台数");
		return FALSE;	
	}
	m_pRs->raw_Close();	//151105-9.2.2
	
	//然后检查库房压缩机是否设置
	strSql.Format("SELECT * FROM SetKuYSJ WHERE YSJNum = %d", g_byYSJSum_SetKu);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-5.1
	if(m_pRs->BOF)
	{
		m_pRs->raw_Close();	//151105-5.2.1
		AfxMessageBox("库房数控制压缩机 - 请设置库房压缩机");
		return FALSE;
	}
	else
	{
		CString strKuMax = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));	
		m_pRs->raw_Close();	//151105-5.2.2
		int nKuMax = atoi(strKuMax);
		if(g_byKuSum_SetKu != nKuMax)
		{
			AfxMessageBox("库房数控制压缩机 - 请填写正确数字");
			return FALSE;
		}
	}

	//然后检查压缩机数增加，库房数也只能增加
	strSql = "SELECT * FROM SetKuYSJ ORDER BY YSJNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151105-6.1
	int nListKuYSJ = 0;
	while(!m_pRs->adoEOF)
	{
		CString strYSJNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("YSJNum"));
		int nYSJNum = atoi(strYSJNum);
		CString strKuNumLast = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
		int nKuNumLast = atoi(strKuNumLast);

		if(nKuNumLast < g_byKuSum_SetKu)
		{
			m_pRs->MoveNext();
			CString strKuNumNext = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
			int nKuNumNext = atoi(strKuNumNext);
			if(nKuNumLast < nKuNumNext)
			{
				m_pRs->MovePrevious();
			}
			else
			{
				AfxMessageBox("库房数控制压缩机 - 请填写正确数字");
				m_pRs->raw_Close();	//151105-6.2.1
				return FALSE;
			}
		}
		else
		{
			if(nYSJNum != g_byYSJSum_SetKu)
			{
				AfxMessageBox("库房数控制压缩机 - 请填写正确数字");
				m_pRs->raw_Close();	//151105-6.2.2
				return FALSE;
			}
		}
		nListKuYSJ++;
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//151105-6.2.3

	if(nListKuYSJ < 2)
	{
		AfxMessageBox("库房数控制压缩机 - 请设置完全");
		return FALSE;	
	}

	if(!EnKuYSJ_SetKu())	//151110
	{
		AfxMessageBox("库房数控制压缩机 - 库房需统一使用全部压缩机");
		return FALSE;
	}

	return TRUE;
}
int GetKuYSJSt()
{
	CString strSql = "";
	strSql.Format("SELECT * FROM SetKuYSJ ORDER BY KuNum ASC");
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151106-1.1
	int nKuYSJList = 0;
	while(!m_pRs->adoEOF)
	{
		nKuYSJList++;
		m_pRs->MoveNext();
	}	

	if(g_pStKuYSJ != NULL)
	{
		delete [] g_pStKuYSJ;
		g_pStKuYSJ = NULL;
	}
	g_pStKuYSJ = new KuYSJ_t[nKuYSJList];
	for(int i=0; i<nKuYSJList; i++)
	{
		memset(&g_pStKuYSJ[i], 0, sizeof(KuYSJ_t));
	}

	m_pRs->MoveFirst();
	int nKuYSJIndex = 0;
	while(!m_pRs->adoEOF)
	{
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
		int nKuNum = atoi(strKuNum);
		CString strYSJNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("YSJNum"));
		int nYSJNum = atoi(strYSJNum);
		g_pStKuYSJ[nKuYSJIndex].KuNum = nKuNum;
		g_pStKuYSJ[nKuYSJIndex].YSJNum = nYSJNum;

		nKuYSJIndex++;
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//151106-1.2

	g_byKuYSJSum_Set = nKuYSJIndex;

	return 1;
}
void YSJDelOffProp(CString strFunc)
{
	//PropTem已经筛选处理完毕

	//只有关机流程的会处理
	if((g_hKuCtrl != NULL) && !g_bUrgentStop && (strcmp(strFunc, "关机流程") == 0))
	{
		//查找关闭的压缩机
		for(int i=0; i<g_byYSJSum_SetKu; i++)
		{
			if(!g_pStYSJ[i].bNC)
			{
				//PropYSJ筛选进入压缩机关闭相关
				CString strText = "";
				strText.Format("关机流程，已经关闭压缩机%d", g_pStYSJ[i].byYSJ);
				Doing2Text(strText);
				YSJOnOffSel(strFunc, i, TRUE);

				//删除PropTem中PropYSJ相关
				CString strSql = "SELECT * FROM PropYSJ ORDER BY NO ASC";
				m_pRs_ManualYSJ = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151104-1.1
				while(!m_pRs_ManualYSJ->adoEOF)
				{		
					CString strYSJZu = (LPCSTR)_bstr_t(m_pRs_ManualYSJ->GetCollect("ZuItem"));
					CString strYSJZuNum = (LPCSTR)_bstr_t(m_pRs_ManualYSJ->GetCollect("ZuNum"));
					int nYSJZuNum = atoi(strYSJZuNum);

					//151111 删除整组，延时0依附
					strSql.Format("DELETE FROM PropTem WHERE ZuItem = '%s' AND ZuNum = %d", strYSJZu, nYSJZuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

					m_pRs_ManualYSJ->MoveNext();
				}
				m_pRs_ManualYSJ->raw_Close();		//151104-1.2
			}
		}
	}
}
void YSJDelOnProp(CString strFunc)
{
	if( g_bEnKuYSJ && (g_hKuCtrl != NULL) && (strcmp(strFunc, "开机流程") == 0) )
	{
		//根据开机的库房数，确定要开启的压缩机总数
		int nKuYSJIndex = 0;
		int nYSJOnSum = 0;
		for(nKuYSJIndex=0; nKuYSJIndex<g_byKuYSJSum_Set; nKuYSJIndex++)
		{
			if(g_byKuOnSum <= g_pStKuYSJ[nKuYSJIndex].KuNum)
			{
				nYSJOnSum = g_pStKuYSJ[nKuYSJIndex].YSJNum;
				break;
			}
		}

		if(nYSJOnSum == g_byYSJSum_SetKu)	return;

		//根据压缩机总数，根据优先级，确定要开启的压缩机
		for(int nYSJOnIndex=0; nYSJOnIndex<nYSJOnSum; nYSJOnIndex++)
		{
			for(int nYSJIndex=0; nYSJIndex<g_byYSJSum_SetKu; nYSJIndex++)
			{
				if(g_pStYSJ[nYSJIndex].byPri == nYSJOnIndex+1)
				{	
					CString strText = "";
					strText.Format("开机流程，将要开启压缩机%d", g_pStYSJ[nYSJIndex].byYSJ);
					Doing2Text(strText);
					//根据要开启的压缩机，筛选PropYSJ
					(nYSJOnIndex == 0)?	//如果是第一个压缩机筛选，则清空PropYSJ
						YSJOnOffSel(strFunc, nYSJIndex, TRUE):
						YSJOnOffSel(strFunc, nYSJIndex, FALSE);

					break;
				}
			}
		}

		//删除PropTem中PropYSJ,相同项目但不同号码
		CString aYSJProp[6]={"水泵", "压缩机", "吸气", "排气", "氨阀", "氨泵"};
		CString strSql = "SELECT * FROM PropTem ORDER BY NO ASC";
		m_pRs_ManualYSJ = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151111-1.1
		while(!m_pRs_ManualYSJ->adoEOF)
		{
			CString strYSJItem = (LPCSTR)_bstr_t(m_pRs_ManualYSJ->GetCollect("Item"));
			CString strYSJItemNum = (LPCSTR)_bstr_t(m_pRs_ManualYSJ->GetCollect("Num"));
			int nYSJItemNum = atoi(strYSJItemNum);
			CString strYSJZu = (LPCSTR)_bstr_t(m_pRs_ManualYSJ->GetCollect("ZuItem"));
			CString strYSJZuNum = (LPCSTR)_bstr_t(m_pRs_ManualYSJ->GetCollect("ZuNum"));
			int nYSJZuNum = atoi(strYSJZuNum);

			//判断PropTem的项目是否属于压缩机范畴
			for(int i=0; i<6; i++)
			{
				if(strcmp(strYSJItem, aYSJProp[i]) == 0)
				{
					//属于压缩机范畴，在PropYSJ中不存在，则删除
					strSql.Format("SELECT * FROM PropYSJ WHERE Item = '%s' AND Num = %d", strYSJItem, nYSJItemNum);
					m_pRs_ManualYSJ_In = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151111-2.1
					if(m_pRs_ManualYSJ_In->BOF)
					{
						//删除一组，延时肯定依附于分组，所以不必单独考虑延时
						strSql.Format("DELETE FROM PropTem WHERE ZuItem = '%s' AND ZuNum = %d", strYSJZu, nYSJZuNum);
						m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);							
					}
					m_pRs_ManualYSJ_In->raw_Close();	//151111-2.2
					break;
				}
			}			
	
			m_pRs_ManualYSJ->MoveNext();
		}
		m_pRs_ManualYSJ->raw_Close();	//151111-1.2
	}
}
void KuYSJ(CEdit *pEdit)
{
	//已经得到当前开着的库房数g_byKuOnSum
	//已得到当前开着的压缩机数g_byYSJOnSum
	if(!g_bEnKuYSJ)	return;
	if(g_hTimer2 != NULL)	return;
	
	//根据当前库房数，确定要开启的压缩机总数
	int nKuYSJIndex = 0;
	int nYSJOnSum = 0;
	for(nKuYSJIndex=0; nKuYSJIndex<g_byKuYSJSum_Set; nKuYSJIndex++)
	{
		if(g_byKuOnSum <= g_pStKuYSJ[nKuYSJIndex].KuNum)
		{
			nYSJOnSum = g_pStKuYSJ[nKuYSJIndex].YSJNum;
			break;
		}
	}

	//开关压缩机
	if(nYSJOnSum > g_byYSJOnSum)
	{
		//需要开启压缩机
		//开启优先级别为nYSJOnSum的压缩机，寻找
		//根据压缩机总数，根据优先级，确定要开启的压缩机
		for(int nYSJIndex=0; nYSJIndex<g_byYSJSum_SetKu; nYSJIndex++)
		{
			if(g_pStYSJ[nYSJIndex].byPri == nYSJOnSum)
			{					
				CString str = "";
				str.Format("压缩机%d已开启", g_pStYSJ[nYSJIndex].byYSJ);
				(!g_pStYSJ[nYSJIndex].bNC)?
					YSJOnOffExe("开机流程", pEdit, nYSJIndex):
					Doing2Text(str);
				break;
			}
		}
	}
	else if(nYSJOnSum < g_byYSJOnSum)
	{
		//需要关闭压缩机	//不会出现0<1，则不会关闭最后一个压缩机
		
		//如果刚在循环中关完机，此时等待中库房立即会开机，则不关压缩机
		if(g_bKuOnMaxWhile)
		{
			Doing2Text("同开最多数，等待中库房立即开启，不关闭压缩机");
			return;
		}

		for(int nYSJIndex=0; nYSJIndex<g_byYSJSum_SetKu; nYSJIndex++)
		{
			if(g_pStYSJ[nYSJIndex].byPri == g_byYSJOnSum)
			{	
				CString str = "";
				str.Format("压缩机%d已关闭", g_pStYSJ[nYSJIndex].byYSJ);
				(g_pStYSJ[nYSJIndex].bNC)?
					YSJOnOffExe("关机流程", pEdit, nYSJIndex):
					Doing2Text(str);
				break;
			}
		}
	}
}
void KuLinkYSJ(CString strFunc, CEdit *pEdit, int nKuIndex)
{
	if(!g_bKuLinkYSJ)
		return;
	//库房号关联压缩机
	if(strcmp(strFunc, "开机流程") == 0)
	{
		for(int i=0; i<KU_YSJ; i++)
		{
			if(g_pStKuCtrl[nKuIndex].aYSJNum[i])
			{
				int nYSJIndex = GetYSJIndex_St(g_pStKuCtrl[nKuIndex].aYSJNum[i]);
				if(!g_pStYSJ[nYSJIndex].bNC)
				{				
					YSJOnOffExe(strFunc, pEdit, nYSJIndex);
					SP_PropING();
				}
			}
		}
	}
	else
	{
		//开着的压缩机数要大于1
		if(g_byYSJOnSum > 1)
		{
			for(int i=0; i<KU_YSJ; i++)
			{
				//该库房关联的某个压缩机
				if(g_pStKuCtrl[nKuIndex].aYSJNum[i])
				{
					int nYSJIndex = GetYSJIndex_St(g_pStKuCtrl[nKuIndex].aYSJNum[i]);
					if(g_pStYSJ[nYSJIndex].bNC)
					{				
						//其他库房是否共用该压缩机
						BOOL bShareYSJKuOn = FALSE;
						int j = 0;
						for(j=0; j<g_byKuCtrlSum_St; j++)
						{
							if(j==nKuIndex)
								continue;
							for(int k=0; k<KU_YSJ; k++)
							{
								//遍历在用库房，库房的压缩机与关机库房共用？
								if(g_pStKuCtrl[j].aYSJNum[k] > 0)
								{
									if(g_pStKuCtrl[j].aYSJNum[k] == g_pStKuCtrl[nKuIndex].aYSJNum[i])
									{
										//共用库房处于开机状态？
										if(g_pStKuCtrl[j].bFlagOn)
										{
											bShareYSJKuOn = TRUE;
											break;
										}
									}								
								}
							}
							if(bShareYSJKuOn)
								break;
						}
						if((j!=nKuIndex) && !bShareYSJKuOn)
						{
							YSJOnOffExe(strFunc, pEdit, nYSJIndex);
							SP_PropING();
						}
					}
				}
			}
		}
	}
}