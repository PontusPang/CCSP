#include "stdafx.h"
#include "CS.h"
#include "SETDlg.h"
#include "CANDlg.h"
#include "Security.h"
#include "AdvCanThread.h"
#include <ATLComTime.h>
#include "ThreadProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSETDlg *pSetDlg = NULL;

extern _ConnectionPtr m_pCon;	//ADO连接对象
extern _RecordsetPtr m_pRs;		//ADO记录集对象

extern CString aOnItem[NumOnItem];
extern CString aOnDir[NumOnDir];
extern CString aSetFunc[NumSetFunc];
extern CString aSetPropZu[NumSetPropZu];	
extern CString aCidItem[NumCidItem];
extern CString aKuCtrlMode[NumKuCtrlMode];
extern CString aKuWorkMode[NumKuWorkMode];
extern CString aCtrlTimeTemp[NumCtrlTimeTemp];

extern HANDLE	g_hDevice_CAN1;
extern DWORD	g_dwCobId;
extern BYTE		g_SendData[8];
extern BYTE		g_RcvData[8];
extern BOOL		g_bOpened_CAN1;

extern CSetDlgFMAF *pSetDlgFMAF;
extern CSetDlgFGYSJ *pSetDlgFGYSJ;
extern CSetDlgDQ *pSetDlgDQ;
extern CCANDlg *g_pCanDlg;

extern BYTE g_byKuOnMax_SetKu;			//同开最多库房数
extern BYTE g_byKuSum_SetKu;			//库房总数
extern BYTE g_byTempSum_SetKu;			//温度节点总数
extern BYTE g_byAnsSum_SetKu;
extern BYTE g_byKuHour_SetKu;
extern int g_nTimeS_SetKu;
extern TempSor_t *g_pStTempSor;

extern COleDateTime g_mTimeNow;

// CSETDlg dialog

CSETDlg::CSETDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSETDlg::IDD, pParent)
	, m_byCidItemNum(0)
	, m_byCidID(0)
	, m_byKuSum(0)
	, m_byTempSum(0)
	, m_strKuStore(_T(""))
	, m_fTempUp(0)
	, m_fTempDn(0)
	, m_strTempNum(_T(""))
	, m_strKuFY(_T(""))
	, m_strKuHQ(_T(""))
	, m_strKuFJ(_T(""))
	, m_strKuYSJ(_T(""))
	, m_nDWKD(0)
	, m_byItemNum(0)
	, m_byZuYSJNum(0)
	, m_byZuKuNum(0)
	, m_byZuGXNum(0)
	, m_byKuAllZu(0)
	, m_byKuOnMax(0)
	, m_byCidDQ(0)
	, m_fTempDn2(0)
	, m_fTempUp2(0)
	, m_byKuTempChangeDays(0)
	, m_byAnsSum(0)
	, m_nTimeS(0)
	, m_byKuTempChangeHour(0)
{
}
void CSETDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSETDlg)
	DDX_Text(pDX, IDC_EDIT_CID_NUM, m_byCidItemNum);
	DDX_Text(pDX, IDC_EDIT_CID_ID, m_byCidID);
	DDX_Text(pDX, IDC_EDIT_KU_SUM, m_byKuSum);
	DDX_Text(pDX, IDC_EDIT_TEMP_SUM, m_byTempSum);
	DDX_Text(pDX, IDC_EDIT_KU_WU, m_strKuStore);
	DDX_Text(pDX, IDC_EDIT_TEMP_UP, m_fTempUp);
	DDX_Text(pDX, IDC_EDIT_TEMP_DN, m_fTempDn);
	DDX_Text(pDX, IDC_EDIT_TEMP_NUM, m_strTempNum);
	DDX_Text(pDX, IDC_EDIT_KU_FY, m_strKuFY);
	DDX_Text(pDX, IDC_EDIT_KU_HQ, m_strKuHQ);
	DDX_Text(pDX, IDC_EDIT_KU_FJ, m_strKuFJ);
	DDX_Text(pDX, IDC_EDIT_KU_YSJ, m_strKuYSJ);
	DDX_Control(pDX, IDC_TAB_SET_DLG, m_TabSheet);
	DDX_Text(pDX, IDC_EDIT_ON_DWKD, m_nDWKD);
	DDX_Text(pDX, IDC_EDIT_ON_NUM, m_byItemNum);
	DDX_Text(pDX, IDC_EDIT_PROP_ZU_YSJ, m_byZuYSJNum);
	DDX_Text(pDX, IDC_EDIT_PROP_ZU_KU, m_byZuKuNum);
	DDX_Text(pDX, IDC_EDIT_PROP_ZU_GX, m_byZuGXNum);
	DDX_Text(pDX, IDC_EDIT_KU_ALL_ZU, m_byKuAllZu);
	DDX_Text(pDX, IDC_EDIT_KU_ON_MAX, m_byKuOnMax);
	DDX_Text(pDX, IDC_EDIT_CID_DQ, m_byCidDQ);
	DDX_Text(pDX, IDC_EDIT_TEMP_DN2, m_fTempDn2);
	DDX_Text(pDX, IDC_EDIT_TEMP_UP2, m_fTempUp2);
	DDX_Text(pDX, IDC_EDIT_KU_TEMP_CHANGE_DAYS, m_byKuTempChangeDays);
	DDX_Text(pDX, IDC_EDIT_ANS_SUM, m_byAnsSum);
	DDX_Text(pDX, IDC_EDIT_KU_TIME_S, m_nTimeS);
	DDX_Text(pDX, IDC_EDIT_KU_SET_TEMP_CHANGE_HOUR, m_byKuTempChangeHour);
	DDV_MinMaxByte(pDX, m_byKuTempChangeHour, 0, 23);
}
BEGIN_MESSAGE_MAP(CSETDlg, CDialog)
	//{{AFX_MSG_MAP(CSETDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ID_ADD, &CSETDlg::OnBnClickedButtonIdAdd)
	ON_BN_CLICKED(IDC_BUTTON_ID_DEL, &CSETDlg::OnBnClickedButtonIdDel)
	ON_BN_CLICKED(IDC_CHECK_KU_SUM, &CSETDlg::OnBnClickedCheckKuSum)
	ON_BN_CLICKED(IDC_BUTTON_KU_OK, &CSETDlg::OnBnClickedButtonKuOk)
	ON_BN_CLICKED(IDC_BUTTON_KU_DEL, &CSETDlg::OnBnClickedButtonKuDel)
	ON_BN_CLICKED(IDC_RADIO_KU_CTRL1, &CSETDlg::OnBnClickedRadioKuCtrl1)
	ON_BN_CLICKED(IDC_RADIO_KU_CTRL3, &CSETDlg::OnBnClickedRadioKuCtrl3)
	ON_BN_CLICKED(IDC_RADIO_KU_CTRL2, &CSETDlg::OnBnClickedRadioKuCtrl2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CID, &CSETDlg::OnNMDblclkListCid)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_KU, &CSETDlg::OnNMDblclkListKu)
	ON_LBN_SELCHANGE(IDC_LIST_ON_ITEM, &CSETDlg::OnLbnSelchangeListOnItem)
	ON_BN_CLICKED(IDC_BUTTON_ON_ADD, &CSETDlg::OnBnClickedButtonOnAdd)
	ON_BN_CLICKED(IDC_BUTTON_ON_DEL, &CSETDlg::OnBnClickedButtonOnDel)
	ON_BN_CLICKED(IDC_BUTTON_ON_INS, &CSETDlg::OnBnClickedButtonOnIns)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL_ON_PROP, &CSETDlg::OnNMDblclkListCtrlOnProp)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL_OFF_PROP, &CSETDlg::OnNMDblclkListCtrlOffProp)
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_CHECK_KU_ON_MAX, &CSETDlg::OnBnClickedCheckKuOnMax)
	ON_BN_CLICKED(IDC_CHECK_KU_TEMP_CHANGE, &CSETDlg::OnBnClickedCheckKuTempChange)
	ON_BN_CLICKED(IDC_CHECK_ANS_SUM, &CSETDlg::OnBnClickedCheckAnsSum)
	ON_BN_CLICKED(IDC_CHECK_KU_TIME_S, &CSETDlg::OnBnClickedCheckKuTimeS)
	ON_BN_CLICKED(IDC_BUTTON_SETKU_YSJ, &CSETDlg::OnBnClickedButtonSetkuYsj)
	ON_BN_CLICKED(IDC_BUTTON_SETKU_FY, &CSETDlg::OnBnClickedButtonSetkuFy)
	ON_BN_CLICKED(IDC_BUTTON_SETKU_HQ, &CSETDlg::OnBnClickedButtonSetkuHq)
	ON_BN_CLICKED(IDC_BUTTON_SETKU_FJ, &CSETDlg::OnBnClickedButtonSetkuFj)
	ON_BN_CLICKED(IDC_CHECK_ONLY_OFF_FJ, &CSETDlg::OnBnClickedCheckOnlyOffFj)
	ON_BN_CLICKED(IDC_RADIO_KU2, &CSETDlg::OnBnClickedRadioKu2)
	ON_BN_CLICKED(IDC_CHECK_KU_SET_TEMP_CHANGE_HOUR, &CSETDlg::OnBnClickedCheckKuSetTempChangeHour)
	ON_BN_CLICKED(IDC_BUTTON_SETKU_MODE, &CSETDlg::OnBnClickedButtonSetkuMode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSETDlg message handlers

BOOL CSETDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	pSetDlg = this;

	//全屏
	ShowWindow(SW_SHOWMAXIMIZED);

	//漏出下面任务栏
	/*if(SIM_STATUS || !LOGIN_EN)	//仅供测试
	{
		int nCx = GetSystemMetrics(SM_CXFULLSCREEN);
		int nCy = GetSystemMetrics(SM_CYFULLSCREEN);
		CRect rt;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);
		nCy = rt.bottom;
		MoveWindow(0, 0, nCx, nCy);
	}*/

	CidInit();
	PropInit();
	KuInit();

	m_TabSheet.AddPage("【峰谷用电·压缩机】", &dlgSetFGYSJ, IDD_SET_DIALOG_FGYSJ);
	m_TabSheet.AddPage("【氨阀】", &dlgSetFMAF, IDD_SET_DIALOG_FMAF);
	m_TabSheet.AddPage("【电气·短信·温度校准】", &dlgSetDQ, IDD_SET_DIALOG_DQ);
	m_TabSheet.Show();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSETDlg::PreTranslateMessage(MSG* pMsg)
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

LRESULT CSETDlg::OnNcHitTest(CPoint point)
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
/////////////////////////开关机流程设置///////////////////////////////////////
void CSETDlg::PropInit()
{
	CButton* pOnFunc = (CButton*)GetDlgItem(IDC_RADIO_PROP1);
	pOnFunc->SetCheck(1);	
	
	CListBox* pPropItem = (CListBox*)GetDlgItem(IDC_LIST_ON_ITEM);
	for(int i=0; i<NumOnItem; i++)
	{
		pPropItem->AddString(*(aOnItem+i));
	}

	CStatic *pDir = (CStatic *)GetDlgItem(IDC_STATIC_ON_DIR);
	pDir->SetWindowText(aOnDir[0]);

	CListCtrl* pOnList = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_ON_PROP);
	CListCtrl* pOffList = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_OFF_PROP);
	pOnList->SetExtendedStyle( pOnList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线
	pOffList->SetExtendedStyle( pOffList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	pOnList->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_CENTER, 45 );
	pOnList->InsertColumn( 1, LPCTSTR("项目"), LVCFMT_CENTER, 60 );
	pOnList->InsertColumn( 2, LPCTSTR("号"), LVCFMT_CENTER, 30 );
	pOnList->InsertColumn( 3, LPCTSTR("特征"), LVCFMT_CENTER, 60 );
	pOnList->InsertColumn( 4, LPCTSTR("数"), LVCFMT_CENTER, 30 );
	pOnList->InsertColumn( 5, LPCTSTR("分组"), LVCFMT_CENTER, 70 );

	pOffList->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_CENTER, 45 );
	pOffList->InsertColumn( 1, LPCTSTR("项目"), LVCFMT_CENTER, 60 );
	pOffList->InsertColumn( 2, LPCTSTR("号"), LVCFMT_CENTER, 30 );
	pOffList->InsertColumn( 3, LPCTSTR("特征"), LVCFMT_CENTER, 60 );
	pOffList->InsertColumn( 4, LPCTSTR("数"), LVCFMT_CENTER, 30 );
	pOffList->InsertColumn( 5, LPCTSTR("分组"), LVCFMT_CENTER, 70 );

	for(int i=0; i<NumSetPropZu; i++)
	{
		CButton* pPropZu = (CButton*)GetDlgItem(IDC_RADIO_PROP_ZU_1+i);
		pPropZu->SetWindowTextA(aSetPropZu[i]);
		if(i == 0)
		{
			pPropZu->SetCheck(1);
		}
	}

	CButton* pNC2 = (CButton*)GetDlgItem(IDC_RADIO_NC2);
	pNC2->SetCheck(1);

	PropLoad();

	return;
}

void CSETDlg::SetPropLoad(CString strFunc,CListCtrl *pList)	//深拷贝问题，参数改为引用或指针
{	
	pList->DeleteAllItems();
	
	CString strSql = "";

	//151014-1
	if(strcmp(strFunc, "关机流程") == 0)
	{
		strSql = "DELETE PropUrgent";
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}

	strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' ORDER BY NO ASC", strFunc);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-10.1	

	int nListEntry = 0;

	while( !m_pRs->adoEOF )
	{	
		CString strNO = "", strItem = "", strNum = "", strDir = "";
		CString strNC = "", strDWKD = "", strZuItem = "", strZuNum = "", strZu = "";

		strNO = (LPCSTR)_bstr_t(m_pRs->GetCollect("NO"));
		pList->InsertItem( nListEntry, LPCTSTR( strNO ), 0 );

		strItem = (LPCSTR)_bstr_t(m_pRs->GetCollect("Item"));
		pList->SetItemText( nListEntry, 1, LPCTSTR( strItem ) );
		
		strNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("Num"));
		if( (strcmp(strNum, "0") == 0) && (strcmp(strItem, "风机") == 0) )
		{
			pList->SetItemText( nListEntry, 2, LPCTSTR( NULL ) );	
		}
		else
		{
			pList->SetItemText( nListEntry, 2, LPCTSTR( strNum ) );
		}

		strDir = (LPCSTR)_bstr_t(m_pRs->GetCollect("Dir"));
		pList->SetItemText( nListEntry, 3, LPCTSTR( strDir ) );
		
		if( ( !strcmp(strItem, *(aOnItem+1) ) ) ||
			( !strcmp(strItem, *(aOnItem+3) ) ) || 
			( !strcmp(strItem, *(aOnItem+4) ) ) ||
			( !strcmp(strItem, *(aOnItem+10) ) )|| 
			( ( !strcmp(strItem, *(aOnItem+0) ) ) && ( !strcmp(strDir, *(aOnDir+4) ) ) ) )
		{
			strNC = (LPCSTR)_bstr_t(m_pRs->GetCollect("NC"));
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
			strDWKD = (LPCSTR)_bstr_t(m_pRs->GetCollect("DWKD"));
			pList->SetItemText( nListEntry, 4, LPCTSTR( strDWKD ) );
		}

		strZuItem = (LPCSTR)_bstr_t(m_pRs->GetCollect("ZuItem"));
		strZuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("ZuNum"));
		strZu = strZuItem + strZuNum;
		pList->SetItemText( nListEntry, 5, LPCTSTR( strZu ) );

		//151014-2修改的是关机流程，则重新添加PropUrgent
		if(strcmp(strFunc, "关机流程") == 0)
		{
			AddUrgentStopProp(strItem, atoi(strNum));
		}
		
		nListEntry ++;
		m_pRs->MoveNext();
	}

	m_pRs->raw_Close();	//RS-10.2

	return;
}

void CSETDlg::PropLoad()
{
	CListCtrl* pOnList = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_ON_PROP);
	CListCtrl* pOffList = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_OFF_PROP);

	SetPropLoad(*aSetFunc,  pOnList);
	SetPropLoad(*(aSetFunc+1), pOffList);

	return;
}

void CSETDlg::SetItem(int nOnOff)
{
	CListBox* pPropItem = (CListBox*)GetDlgItem(IDC_LIST_ON_ITEM);
	CStatic *pDir = (CStatic *)GetDlgItem(IDC_STATIC_ON_DIR);
	CEdit *pNum = (CEdit *)GetDlgItem(IDC_EDIT_ON_NUM);
	CEdit *pDWKD = (CEdit *)GetDlgItem(IDC_EDIT_ON_DWKD);

	CButton* pNC1 = (CButton*)GetDlgItem(IDC_RADIO_NC1);
	CButton* pNC2 = (CButton*)GetDlgItem(IDC_RADIO_NC2);

	CButton* pYSJZu = (CButton*) GetDlgItem(IDC_RADIO_PROP_ZU_1);
	CButton* pKuZu = (CButton*) GetDlgItem(IDC_RADIO_PROP_ZU_2);
	CButton* pGXZu = (CButton*) GetDlgItem(IDC_RADIO_PROP_ZU_3);
	CEdit* pYSJZuNum = (CEdit*) GetDlgItem(IDC_EDIT_PROP_ZU_YSJ);
	CEdit* pKuZuNum = (CEdit*) GetDlgItem(IDC_EDIT_PROP_ZU_KU);
	CEdit* pGXZuNum = (CEdit*) GetDlgItem(IDC_EDIT_PROP_ZU_GX);

	CString strItemNum = "";
	pNum->GetWindowTextA(strItemNum);

	switch( pPropItem->GetCurSel() )
	{
	case 0: //压缩机
	{
		if( pNC2->GetCheck() )	//选择关,档位到；选择开或关后，再点击压缩机
		{
			pDir->SetWindowText(aOnDir[1]);

			pNum->SetReadOnly(FALSE);
			pDWKD->SetReadOnly(FALSE);
			pNC1->EnableWindow(TRUE);
		}
		else
		{
			pDir->SetWindowText(aOnDir[4]);

			pNum->SetReadOnly(FALSE);
			pDWKD->SetReadOnly(TRUE);
		}

		pYSJZu->SetCheck(1);
		pYSJZuNum->SetWindowTextA(strItemNum);
		pKuZu->SetCheck(0);
		pKuZuNum->SetWindowTextA("0");
		pGXZu->SetCheck(0);
		pGXZuNum->SetWindowTextA("0");

		break;
	}
	case 1: //开关
	case 3:  
	case 10: 
	{
		pDir->SetWindowText(aOnDir[4]);

		pNum->SetReadOnly(FALSE);
		pDWKD->SetReadOnly(TRUE);
		pNC1->EnableWindow(TRUE);

		pYSJZu->SetCheck(0);
		pYSJZuNum->SetWindowTextA("0");
		pKuZu->SetCheck(0);
		pKuZuNum->SetWindowTextA("0");
		pGXZu->SetCheck(1);
		pGXZuNum->SetWindowTextA("");

		break;
	}
	case 4://风机
	{
		pDir->SetWindowText(aOnDir[4]);

		pNum->SetReadOnly(TRUE);
		pDWKD->SetReadOnly(TRUE);
		pNC1->EnableWindow(TRUE);

		pYSJZu->SetCheck(0);
		pYSJZuNum->SetWindowTextA("0");
		pKuZu->SetCheck(0);
		pKuZuNum->SetWindowTextA("0");
		pGXZu->SetCheck(1);
		pGXZuNum->SetWindowTextA("");

		break;
	}
	case 2: //档位开度
	{
		pDir->SetWindowText(aOnDir[2]);

		pNum->SetReadOnly(FALSE);
		pDWKD->SetReadOnly(FALSE);
		pNC1->EnableWindow(FALSE);

		pYSJZu->SetCheck(0);
		pYSJZuNum->SetWindowTextA("0");
		pKuZu->SetCheck(0);
		pKuZuNum->SetWindowTextA("0");
		pGXZu->SetCheck(1);
		pGXZuNum->SetWindowTextA("");

		break;
	}
	case 8:
	case 9:
	{
		pDir->SetWindowText(aOnDir[2]);

		pNum->SetReadOnly(FALSE);
		pDWKD->SetReadOnly(FALSE);
		pNC1->EnableWindow(FALSE);

		pYSJZu->SetCheck(1);
		pYSJZuNum->SetWindowTextA(strItemNum);
		pKuZu->SetCheck(0);
		pKuZuNum->SetWindowTextA("0");
		pGXZu->SetCheck(0);
		pGXZuNum->SetWindowTextA("0");

		break;
	}
	case 5: 		
	case 6:
	{
		pDir->SetWindowText(aOnDir[2]);

		pNum->SetReadOnly(FALSE);
		pDWKD->SetReadOnly(FALSE);
		pNC1->EnableWindow(FALSE);

		UpdateData(TRUE);
		pYSJZu->SetCheck(0);
		pYSJZuNum->SetWindowTextA("0");
		pKuZu->SetCheck(1);
		pKuZuNum->SetWindowTextA(strItemNum);
		pGXZu->SetCheck(0);
		pGXZuNum->SetWindowTextA("0");

		break;		
	}
	case 7: //延时
	{
		//m_byItemNum = 0;
		UpdateData(TRUE);
		UpdateData(FALSE);

		pDir->SetWindowText(aOnDir[3]);

		pNum->SetReadOnly(FALSE);
		pDWKD->SetReadOnly(FALSE);
		pNC1->EnableWindow(FALSE);

		break;
	}
	default: break;
	}

	return;
}

void CSETDlg::OnLbnSelchangeListOnItem()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE byOnOffSet = 0; 
	CButton * pOnSet = (CButton *)GetDlgItem(IDC_RADIO_PROP1);
	( pOnSet->GetCheck() )? byOnOffSet = 1: byOnOffSet = 0;
	
	(byOnOffSet == 1)? SetItem(0): SetItem(1);	

	return;
}
void CSETDlg::SetPropAdd(CString strFunc)
{
	CString strPropSetText = "";
	CString strSql = "";
	
	CListBox* pPropItem = (CListBox*)GetDlgItem(IDC_LIST_ON_ITEM);
	int nItemNum = pPropItem->GetCurSel();

	int nNO = 0;

	if(nItemNum > -1)
	{
		UpdateData(TRUE);

		strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' ORDER BY NO ASC", strFunc);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-11.1

		//序号
		int nTemNO = 0;
		while( !m_pRs->adoEOF )
		{
			nTemNO++;
			m_pRs->MoveNext();
		}		
		m_pRs->raw_Close();	//RS-11.2

		nNO = nTemNO + 1;	//流程最大数加1

		CListCtrl* pOnProp = NULL;
		(strcmp(strFunc, "关机流程") == 0)?
			pOnProp = (CListCtrl*) GetDlgItem(IDC_LIST_CTRL_OFF_PROP):
			pOnProp = (CListCtrl*) GetDlgItem(IDC_LIST_CTRL_ON_PROP);
		int nOnPropIndex = pOnProp->GetNextItem(-1, LVNI_SELECTED);
		int nCurNO = 0;
		if(nOnPropIndex > -1)
		{
			CString strCurNO = pOnProp->GetItemText(nOnPropIndex, 0);
			nCurNO = atoi(strCurNO);

			strSql.Format("SELECT * From SetProp WHERE NO = %d AND Func = '%s'", nCurNO, strFunc);
			m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//55.1
			if(!m_pRs->BOF)	
			{
				CString strRe = "";
				strRe.Format("%s %d 已存在，将覆盖", strFunc, nCurNO);
				MessageBox(strRe);
				strSql.Format("DELETE FROM SetProp WHERE NO = %d AND Func = '%s'", nCurNO, strFunc);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
			m_pRs->raw_Close();	//55.2

			nNO = nCurNO;
		}
		
		//项目
		CString strItem = *( aOnItem + nItemNum );

		//号码
		int nDWKD = m_nDWKD;
		BYTE byNC = -1;

		//类型
		CStatic *pDir = (CStatic *)GetDlgItem(IDC_STATIC_ON_DIR);
		CString strDir = "";
		pDir->GetWindowTextA(strDir);

		if(strcmp(strDir, aOnDir[4]) == 0)	//选开关
		{
			CButton* pNC1 = (CButton*)GetDlgItem(IDC_RADIO_NC1);
			( pNC1->GetCheck() )? byNC=1: byNC=0;
			
			nDWKD = -1;
		}
		else
		{
			nDWKD = m_nDWKD;
			byNC = -1;
		}

		if(strcmp(strDir, aOnDir[3]) == 0)	//延时秒
		{
			byNC = -1;
		}

		//分组
		CString strPropZu = ""; 
		BYTE byZuNum=0;
		for(int i=0; i<3; i++)
		{
			CButton* pPropZu = (CButton*)GetDlgItem(IDC_RADIO_PROP_ZU_1+i);
			if( pPropZu->GetCheck() == 1 )
			{
				strPropZu = aSetPropZu[i];
				switch(i)
				{
				case 0: byZuNum = m_byZuYSJNum; break;
				case 1: byZuNum = m_byZuKuNum; break;
				case 2: byZuNum = m_byZuGXNum; break;
				default: break;
				}
			}
		}

		strSql.Format("INSERT INTO SetProp VALUES('%s',%d,'%s',%d,'%s',%d,%d,'%s',%d,%d)"
			,strFunc,nNO,strItem,m_byItemNum,strDir,nDWKD,byNC,strPropZu,byZuNum,FALSE);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		PropLoad();

		strPropSetText.Format("设置·流程增加 %s%d: %s-%d-%s-%d-%d %s%d", strFunc,nNO,strItem,m_byItemNum,strDir,nDWKD,byNC,strPropZu,byZuNum);
		Doing2Text(strPropSetText);
	}

	CListCtrl* pList = NULL;
	(strcmp(strFunc, "关机流程") == 0)?
		pList = (CListCtrl*) GetDlgItem(IDC_LIST_CTRL_OFF_PROP):
		pList = (CListCtrl*) GetDlgItem(IDC_LIST_CTRL_ON_PROP);
	int nCount = pList->GetItemCount();
	if(nCount > 0)
	{
		pList->EnsureVisible(nCount-1, 0);
	}

	return;
}

void CSETDlg::OnBnClickedButtonOnAdd() 
{
	BYTE nOnOffSet = 0; 
	CButton * pOnSet = (CButton *)GetDlgItem(IDC_RADIO_PROP1);
	( pOnSet->GetCheck() )? nOnOffSet = 1: nOnOffSet = 0;

	(nOnOffSet == 1)? SetPropAdd(*aSetFunc): SetPropAdd(*(aSetFunc+1));
	return;
}

void CSETDlg::SetPropDel(CString strFunc, CListCtrl *pList)
{
	int nCurNO = 0;			//数据库中序号
	CString strTempCurNO = "";
	CString strSql = "";

	int nPropIndex = -1;	//流程表中序号
	nPropIndex = pList->GetNextItem(-1, LVNI_SELECTED);

	if( nPropIndex != -1 )	//选中流程后才能删除，得到的是选中的那一项的行数，0~N-1,
	{
		CString strPropSetText = "";
		
		strTempCurNO = pList->GetItemText(nPropIndex, 0);
		nCurNO = atoi(strTempCurNO);

		strSql.Format("DELETE FROM SetProp where NO=%d and Func='%s'",nCurNO, strFunc);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' ORDER BY NO ASC", strFunc);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-12.1
		
		while( ! m_pRs->adoEOF )
		{
			strSql.Format("UPDATE SetProp SET [NO] = %d WHERE NO = %d and Func='%s'",nCurNO,nCurNO+1,strFunc);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			nCurNO++;
			m_pRs->MoveNext();
		}

		m_pRs->raw_Close();	//RS-12.2
		
		PropLoad();

		strPropSetText.Format("设置·流程删除 %s%d", strFunc, nCurNO);
		Doing2Text(strPropSetText);
	}

	return;
}

void CSETDlg::OnBnClickedButtonOnDel()
{
	BYTE nOnOffSet = 0; 
	CButton * pOnSet = (CButton *)GetDlgItem(IDC_RADIO_PROP1);
	( pOnSet->GetCheck() )? nOnOffSet = 1: nOnOffSet = 0;

	CListCtrl* pOnList = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_ON_PROP);
	CListCtrl* pOffList = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_OFF_PROP);

	(nOnOffSet == 1)? SetPropDel(*aSetFunc, pOnList): SetPropDel(*(aSetFunc+1), pOffList);

	return;
}

void CSETDlg::SetPropIns(CString strFunc, CListCtrl *pList)
{
	MessageBox("流程插入，向前插入【150912】");
	CString strSql = "";
	
	CListBox* pPropItem = (CListBox*)GetDlgItem(IDC_LIST_ON_ITEM);
	int nItemNum = pPropItem->GetCurSel();

	int nPropIndex = -1;	//流程表中序号
	nPropIndex = pList->GetNextItem(-1, LVNI_SELECTED);

	if( (nPropIndex > -1) && (nItemNum > -1) )
	{
		CString strPropSetText = "";
		
		CString strCurNO = pList->GetItemText(nPropIndex, 0);
		int nCurNO = atoi(strCurNO);

		//处理数据库中序号
		strSql.Format("SELECT * FROM SetProp WHERE Func = '%s' ORDER BY NO ASC", strFunc);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-13.1
		int nPropCount = 0;
		while( !m_pRs->adoEOF )
		{
			nPropCount++;
			m_pRs->MoveNext();
		}

		m_pRs->MoveFirst();

		int i = 0;
		while( !m_pRs->adoEOF )
		{
			//strSql.Format("UPDATE SetProp SET [NO] = %d WHERE NO = %d and NO > %d and Func = '%s'"	//150912
			strSql.Format("UPDATE SetProp SET [NO] = %d WHERE NO = %d and NO >= %d and Func = '%s'"
				,nPropCount+1-i, nPropCount-i, nCurNO, strFunc);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			i++;
			m_pRs->MoveNext();
		}
		m_pRs->raw_Close();	//RS-13.2

		UpdateData(TRUE);
		//流程序号
		//int nNO = nPropIndex+2;	//150912
		int nNO = nPropIndex+1;

		//项目
		CString strItem = *( aOnItem + nItemNum );

		//号码
		int nDWKD = m_nDWKD;
		BYTE byNC = -1;

		//类型
		CStatic *pDir = (CStatic *)GetDlgItem(IDC_STATIC_ON_DIR);
		CString strDir = "";
		pDir->GetWindowTextA(strDir);

		if(strcmp(strDir, aOnDir[4]) == 0)	//选开关
		{
			CButton* pNC1 = (CButton*)GetDlgItem(IDC_RADIO_NC1);
			( pNC1->GetCheck() )? byNC=1: byNC=0;
			
			nDWKD = -1;
		}
		else
		{
			nDWKD = m_nDWKD;
			byNC = -1;
		}

		if(strcmp(strDir, aOnDir[3]) == 0)	//延时秒
		{
			byNC = -1;
		}

		//分组
		CString strPropZu = ""; 
		BYTE byZuNum=0;
		for(int i=0; i<3; i++)
		{
			CButton* pPropZu = (CButton*)GetDlgItem(IDC_RADIO_PROP_ZU_1+i);
			if( pPropZu->GetCheck() == 1 )
			{
				strPropZu = aSetPropZu[i];
				switch(i)
				{
				case 0: byZuNum = m_byZuYSJNum; break;
				case 1: byZuNum = m_byZuKuNum; break;
				case 2: byZuNum = m_byZuGXNum; break;
				default: break;
				}
			}
		}

		strSql.Format("INSERT INTO SetProp VALUES('%s',%d,'%s',%d,'%s',%d,%d,'%s',%d,%d)"
			,strFunc,nNO,strItem,m_byItemNum,strDir,nDWKD,byNC,strPropZu,byZuNum,FALSE);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		
		PropLoad();

		strPropSetText.Format("设置·流程插入 %s%d: %s-%d-%s-%d-%d %s%d", strFunc,nNO,strItem,m_byItemNum,strDir,nDWKD,byNC,strPropZu,byZuNum);
		Doing2Text(strPropSetText);
	}

	return;
}

void CSETDlg::OnBnClickedButtonOnIns()
{
	BYTE nOnOffSet = 0; 
	CButton * pOnSet = (CButton *)GetDlgItem(IDC_RADIO_PROP1);
	( pOnSet->GetCheck() )? nOnOffSet = 1: nOnOffSet = 0;

	CListCtrl* pOnList = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_ON_PROP);
	CListCtrl* pOffList = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_OFF_PROP);

	(nOnOffSet == 1)? SetPropIns(*aSetFunc, pOnList): SetPropIns(*(aSetFunc+1), pOffList);

	return;
}

void CSETDlg::OnNMDblclkListCtrlOnProp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CString strSql = "";

	CListCtrl* pOnProp = (CListCtrl*) GetDlgItem(IDC_LIST_CTRL_ON_PROP);
	int nOnPropIndex = pOnProp->GetNextItem(-1, LVNI_SELECTED);

	if(nOnPropIndex > -1)
	{
		CString strCurNO = pOnProp->GetItemText(nOnPropIndex, 0);
		int nCurNO = atoi(strCurNO);
		
		CListBox* pPropItem = (CListBox*) GetDlgItem(IDC_LIST_ON_ITEM);
		CString strCurItem = pOnProp->GetItemText(nOnPropIndex, 1);
		int nCurSelPropItem = 0;
		for(int i=0; i<NumOnItem; i++)
		{
			if(strcmp(strCurItem, aOnItem[i]) == 0)
			{
				nCurSelPropItem = i;
				break;
			}
		}
		CString strItem = "";
		pPropItem->SetCurSel(nCurSelPropItem);
		pPropItem->GetText(nCurSelPropItem, strItem);
		if(strcmp(strItem, strCurItem) != 0)
		{
			pPropItem->SetCurSel(-1);
		}
		OnLbnSelchangeListOnItem();

		CString strItemNum = pOnProp->GetItemText(nOnPropIndex, 2);
		m_byItemNum = atoi(strItemNum);

		UpdateData(FALSE);

		CString strParam = pOnProp->GetItemText(nOnPropIndex, 4);
		if( (strcmp(strParam, "开") == 0) || (strcmp(strParam, "关") == 0) )
		{
			for(int i=0; i<2; i++)
			{
				CButton* pNC = (CButton*)GetDlgItem(IDC_RADIO_NC1+i);
				CString strStatus = "";
				pNC->GetWindowTextA(strStatus);
				(strcmp(strParam, strStatus) == 0)? pNC->SetCheck(1): pNC->SetCheck(0);
			}		
		}
		else
		{
			m_nDWKD = atoi(strParam);
			UpdateData(FALSE);
		}

		strSql.Format("SELECT * FROM SetProp WHERE Func = '开机流程' AND NO = %d", nCurNO);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-14.1
		if(!m_pRs->BOF)
		{
			CString strZuItem = (LPCSTR)_bstr_t(m_pRs->GetCollect("ZuItem"));
			CString strZuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("ZuNum"));
			CEdit *pYSJ = (CEdit*)GetDlgItem(IDC_EDIT_PROP_ZU_YSJ);
			CEdit *pKu = (CEdit*)GetDlgItem(IDC_EDIT_PROP_ZU_KU);
			CEdit *pGX = (CEdit*)GetDlgItem(IDC_EDIT_PROP_ZU_GX);
			
			for(int k=0; k<3; k++)
			{
				CButton* pZuItem = (CButton*)GetDlgItem(IDC_RADIO_PROP_ZU_1+k);
				CString strZu = "";
				pZuItem->GetWindowTextA(strZu);
				(strcmp(strZuItem, strZu) == 0)? pZuItem->SetCheck(1): pZuItem->SetCheck(0);
				
				if(strcmp(strZuItem, "压缩机") == 0)
				{
					pYSJ->SetWindowTextA(strZuNum); 
				}
				else if(strcmp(strZuItem, "库房") == 0)
				{
					pKu->SetWindowTextA(strZuNum);
				}
				else if(strcmp(strZuItem, "特殊") == 0)
				{
					pGX->SetWindowTextA(strZuNum);
				}
			}
		}
		m_pRs->raw_Close();	//RS-14.2

		CButton* pOnFunc = (CButton*)GetDlgItem(IDC_RADIO_PROP1);
		pOnFunc->SetCheck(1);
		CButton* pOffFunc = (CButton*)GetDlgItem(IDC_RADIO_PROP2);
		pOffFunc->SetCheck(0);	
	}
}
void CSETDlg::OnNMDblclkListCtrlOffProp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CString strSql = "";

	CListCtrl* pOffProp = (CListCtrl*) GetDlgItem(IDC_LIST_CTRL_OFF_PROP);
	int nOffPropIndex = pOffProp->GetNextItem(-1, LVNI_SELECTED);

	if(nOffPropIndex > -1)
	{
		CString strCurNO = pOffProp->GetItemText(nOffPropIndex, 0);
		int nCurNO = atoi(strCurNO);
		
		CListBox* pPropItem = (CListBox*) GetDlgItem(IDC_LIST_ON_ITEM);
		CString strCurItem = pOffProp->GetItemText(nOffPropIndex, 1);
		int nCurSelPropItem = 0;
		for(int i=0; i<NumOnItem; i++)
		{
			if(strcmp(strCurItem, aOnItem[i]) == 0)
			{
				nCurSelPropItem = i;
				break;
			}
		}
		CString strItem = "";
		pPropItem->SetCurSel(nCurSelPropItem);
		pPropItem->GetText(nCurSelPropItem, strItem);
		if(strcmp(strItem, strCurItem) != 0)
		{
			pPropItem->SetCurSel(-1);
		}
		OnLbnSelchangeListOnItem();

		CString strItemNum = pOffProp->GetItemText(nOffPropIndex, 2);
		m_byItemNum = atoi(strItemNum);

		UpdateData(FALSE);

		CString strParam = pOffProp->GetItemText(nOffPropIndex, 4);
		if( (strcmp(strParam, "开") == 0) || (strcmp(strParam, "关") == 0) )
		{
			for(int i=0; i<2; i++)
			{
				CButton* pNC = (CButton*)GetDlgItem(IDC_RADIO_NC1+i);
				CString strStatus = "";
				pNC->GetWindowTextA(strStatus);
				(strcmp(strParam, strStatus) == 0)? pNC->SetCheck(1): pNC->SetCheck(0);
			}		
		}
		else
		{
			m_nDWKD = atoi(strParam);
			UpdateData(FALSE);
		}

		strSql.Format("SELECT * FROM SetProp WHERE Func = '关机流程' AND NO = %d", nCurNO);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-15.1
		if(!m_pRs->BOF)
		{
			CString strZuItem = (LPCSTR)_bstr_t(m_pRs->GetCollect("ZuItem"));
			CString strZuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("ZuNum"));
			CEdit *pYSJ = (CEdit*)GetDlgItem(IDC_EDIT_PROP_ZU_YSJ);
			CEdit *pKu = (CEdit*)GetDlgItem(IDC_EDIT_PROP_ZU_KU);
			CEdit *pGX = (CEdit*)GetDlgItem(IDC_EDIT_PROP_ZU_GX);
			
			for(int k=0; k<3; k++)
			{
				CButton* pZuItem = (CButton*)GetDlgItem(IDC_RADIO_PROP_ZU_1+k);
				CString strZu = "";
				pZuItem->GetWindowTextA(strZu);
				(strcmp(strZuItem, strZu) == 0)? pZuItem->SetCheck(1): pZuItem->SetCheck(0);
				
				if(strcmp(strZuItem, "压缩机") == 0)
				{
					pYSJ->SetWindowTextA(strZuNum); 
				}
				else if(strcmp(strZuItem, "库房") == 0)
				{
					pKu->SetWindowTextA(strZuNum);
				}
				else if(strcmp(strZuItem, "特殊") == 0)
				{
					pGX->SetWindowTextA(strZuNum);
				}
			}
		}
		m_pRs->raw_Close();	//RS-15.2

		CButton* pOnFunc = (CButton*)GetDlgItem(IDC_RADIO_PROP1);
		pOnFunc->SetCheck(0);
		CButton* pOffFunc = (CButton*)GetDlgItem(IDC_RADIO_PROP2);
		pOffFunc->SetCheck(1);	
	}
}
/////////////////////////库房设置/////////////////////////////////////////
BOOL bFlagEditLastWorkStatus = FALSE;	//编辑之前存储库房的工作状态
void CSETDlg::SumLoad(void)
{
	CString strSql = "";
	CButton* pKuSum = (CButton*)GetDlgItem(IDC_CHECK_KU_SUM);
	CEdit* pEditKuSum = (CEdit*)GetDlgItem(IDC_EDIT_KU_SUM);
	CEdit* pEditTempSum = (CEdit*)GetDlgItem(IDC_EDIT_TEMP_SUM);
	CButton* pKuOK = (CButton*)GetDlgItem(IDC_BUTTON_KU_OK);
	CComboBox* pKuNum = (CComboBox*)GetDlgItem(IDC_COMBO_KU_NUM);

	CButton* pKuOnMax = (CButton*)GetDlgItem(IDC_CHECK_KU_ON_MAX);
	CEdit* pEditKuOnMax = (CEdit*)GetDlgItem(IDC_EDIT_KU_ON_MAX);

	CButton* pAnsSum = (CButton*)GetDlgItem(IDC_CHECK_ANS_SUM);
	CEdit* pEditAnsSum = (CEdit*)GetDlgItem(IDC_EDIT_ANS_SUM);
	CButton* pTimeS = (CButton*)GetDlgItem(IDC_CHECK_KU_TIME_S);
	CEdit* pEditTimeS = (CEdit*)GetDlgItem(IDC_EDIT_KU_TIME_S);

	CButton* pBtKuHour = (CButton*)GetDlgItem(IDC_CHECK_KU_SET_TEMP_CHANGE_HOUR);
	CEdit* pEditKuHour = (CEdit*)GetDlgItem(IDC_EDIT_KU_SET_TEMP_CHANGE_HOUR);

	strSql = "SELECT * FROM SetKu WHERE KuNum = 0";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-16.1
	CString strKuSum = "", strTempSum = "", strKuOnMax = "", strAnsSum = "", strTimeS = "", strKuHour = "";
	if(!m_pRs->BOF)
	{
		strTimeS = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempNum"));
		strKuSum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuYSJ"));
		strTempSum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFY"));
		strKuOnMax = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuHQ"));
		strAnsSum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFJ"));
		strKuHour = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuCtrlMode"));
	}
	else
	{
		MessageBox("获取总数失败，请检查数据库库房设置");
	}
	m_pRs->raw_Close();	//RS-16.2

	m_nTimeS = atoi(strTimeS);
	m_byKuSum = atoi(strKuSum);
	m_byTempSum = atoi(strTempSum);
	m_byKuOnMax = atoi(strKuOnMax);
	m_byAnsSum = atoi(strAnsSum);
	m_byKuTempChangeHour = atoi(strKuHour);

	//及时更新
	g_nTimeS_SetKu = m_nTimeS;
	g_byKuSum_SetKu = m_byKuSum;
	g_byKuOnMax_SetKu = m_byKuOnMax;
	g_byTempSum_SetKu = m_byTempSum;
	g_byAnsSum_SetKu = m_byAnsSum;
	g_byKuHour_SetKu = m_byKuTempChangeHour;

	UpdateData(FALSE);
	(m_byKuSum != 0)? pKuSum->SetCheck(1) : pKuSum->SetCheck(0);
	(m_byKuOnMax != 0)? pKuOnMax->SetCheck(1): pKuOnMax->SetCheck(0);
	(m_byAnsSum != 0)? pAnsSum->SetCheck(1): pAnsSum->SetCheck(0);
	(m_nTimeS != 0)? pTimeS->SetCheck(1): pTimeS->SetCheck(0);
	pBtKuHour->SetCheck( (m_byKuTempChangeHour >= 0) && (m_byKuTempChangeHour <= 23) );

	if(pKuSum->GetCheck())
	{
		pEditKuSum->EnableWindow(FALSE);
		pEditTempSum->EnableWindow(FALSE);
		pKuOK->EnableWindow(TRUE);
	} 
	else
	{
		pEditKuSum->EnableWindow(TRUE);
		pEditTempSum->EnableWindow(TRUE);
		pKuOK->EnableWindow(FALSE);
	}

	(pKuOnMax->GetCheck())? pEditKuOnMax->EnableWindow(FALSE): pEditKuOnMax->EnableWindow(TRUE);
	(pAnsSum->GetCheck())? pEditAnsSum->EnableWindow(FALSE): pEditAnsSum->EnableWindow(TRUE);
	(pTimeS->GetCheck())? pEditTimeS->EnableWindow(FALSE): pEditTimeS->EnableWindow(TRUE);
	pEditKuHour->EnableWindow(!pBtKuHour->GetCheck());

	pKuNum->ResetContent();
	for(int i=0; i<m_byKuSum; i++)
	{
		CString strKuNum = "";
		strKuNum.Format("%d", i+1);
		pKuNum->AddString(strKuNum);
	}
}

void CSETDlg::OnBnClickedCheckKuSum()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pKuSum = (CButton*)GetDlgItem(IDC_CHECK_KU_SUM);
	CEdit* pEditKuSum = (CEdit*)GetDlgItem(IDC_EDIT_KU_SUM);
	CEdit* pEditTempSum = (CEdit*)GetDlgItem(IDC_EDIT_TEMP_SUM);
	CButton* pKuOK = (CButton*)GetDlgItem(IDC_BUTTON_KU_OK);

	if( pKuSum->GetCheck() )
	{		
		UpdateData(TRUE);

		if(m_byKuSum < m_byKuOnMax)
		{
			MessageBox("库房总数 不应小于 同开最多库房数");
			pKuSum->SetCheck(FALSE);
			return;
		}

		CString strKuSum = "";
		strKuSum.Format("%d", m_byKuSum);
		CString strTempSum = "";
		strTempSum.Format("%d", m_byTempSum);

		CString strSql = "";
		strSql.Format("UPDATE SetKu SET [KuYSJ] = '%s', [KuFY] = '%s' WHERE KuNum = 0"
			, strKuSum, strTempSum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		MessageBox("库房总数、温度节点总数已修改！");

		SumLoad();

		if(g_pCanDlg != NULL)
			g_pCanDlg->TempListLoad();
	}
	else
	{
		pEditKuSum->EnableWindow(TRUE);
		pEditTempSum->EnableWindow(TRUE);
		pKuOK->EnableWindow(FALSE);
	}

	Doing2Text("设置·库温总数");
}

void CSETDlg::OnBnClickedCheckKuOnMax()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pKuOnMax = (CButton*)GetDlgItem(IDC_CHECK_KU_ON_MAX);
	CEdit* pEditKuOnMax = (CEdit*)GetDlgItem(IDC_EDIT_KU_ON_MAX);

	if( pKuOnMax->GetCheck() )
	{		
		UpdateData(TRUE);	

		if(m_byKuOnMax > m_byKuSum)
		{
			MessageBox("同开最多库房数 不应大于 库房总数");
			pKuOnMax->SetCheck(FALSE);
			return;
		}

		CString strKuOnMax = "";
		strKuOnMax.Format("%d", m_byKuOnMax);

		CString strSql = "";
		strSql.Format("UPDATE SetKu SET [KuHQ] = '%s' WHERE KuNum = 0", strKuOnMax);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		MessageBox("同开最多库房数已修改！");

		SumLoad();
	}
	else
	{
		pEditKuOnMax->EnableWindow(TRUE);
	}

	Doing2Text("设置·同开最多");
}
void CSETDlg::OnBnClickedCheckAnsSum()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pAnsSum = (CButton*)GetDlgItem(IDC_CHECK_ANS_SUM);
	CEdit* pEditAnsSum = (CEdit*)GetDlgItem(IDC_EDIT_ANS_SUM);

	if( pAnsSum->GetCheck() )
	{		
		UpdateData(TRUE);	

		CString strAnsSum = "";
		strAnsSum.Format("%d", m_byAnsSum);

		CString strSql = "";
		strSql.Format("UPDATE SetKu SET [KuFJ] = '%s' WHERE KuNum = 0", strAnsSum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		MessageBox("氨检巡路总数已修改！");

		SumLoad();
	}
	else
	{
		pEditAnsSum->EnableWindow(TRUE);
	}

	Doing2Text("设置·氨检总数");
}

void CSETDlg::OnBnClickedCheckKuTimeS()
{
	CButton* pTimeS = (CButton*)GetDlgItem(IDC_CHECK_KU_TIME_S);
	CEdit* pEditTimeS = (CEdit*)GetDlgItem(IDC_EDIT_KU_TIME_S);

	if( pTimeS->GetCheck() )
	{		
		UpdateData(TRUE);	

		CString strTimeS = "";
		strTimeS.Format("%d", m_nTimeS);

		CString strSql = "";
		strSql.Format("UPDATE SetKu SET [TempNum] = '%s' WHERE KuNum = 0", strTimeS);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		MessageBox("运行时间保护已修改！");

		SumLoad();
	}
	else
	{
		pEditTimeS->EnableWindow(TRUE);
	}

	Doing2Text("设置·运行时间");
}

void CSETDlg::KuInit(void)
{
	CListCtrl* pKuList = (CListCtrl*)GetDlgItem(IDC_LIST_KU);

	pKuList->SetExtendedStyle( pKuList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pKuList->InsertColumn( 0, LPCTSTR("库房"), LVCFMT_LEFT, 45 );
	pKuList->InsertColumn( 1, LPCTSTR("测温"), LVCFMT_CENTER, 55 );
	pKuList->InsertColumn( 2, LPCTSTR("压缩机"), LVCFMT_CENTER, 60 );
	pKuList->InsertColumn( 3, LPCTSTR("分液"), LVCFMT_CENTER, 60 );
	pKuList->InsertColumn( 4, LPCTSTR("回气"), LVCFMT_CENTER, 45 );
	pKuList->InsertColumn( 5, LPCTSTR("风机"), LVCFMT_CENTER, 45 );
	pKuList->InsertColumn( 6, LPCTSTR("使用"), LVCFMT_CENTER, 90 );
	pKuList->InsertColumn( 7, LPCTSTR("物品"), LVCFMT_CENTER, 45 );
	pKuList->InsertColumn( 8, LPCTSTR("现上限"), LVCFMT_CENTER, 60 );
	pKuList->InsertColumn( 9, LPCTSTR("现下限"), LVCFMT_CENTER, 60 );
	pKuList->InsertColumn( 10, LPCTSTR("控制模式"), LVCFMT_CENTER, 80 );
	pKuList->InsertColumn( 11, LPCTSTR("起始时间"), LVCFMT_CENTER, 80 );
	pKuList->InsertColumn( 12, LPCTSTR("温控过程"), LVCFMT_CENTER, 80 );
	pKuList->InsertColumn( 13, LPCTSTR("终止时间"), LVCFMT_CENTER, 80 );
	pKuList->InsertColumn( 14, LPCTSTR("工作模式"), LVCFMT_CENTER, 80 );
	pKuList->InsertColumn( 15, LPCTSTR("分组"), LVCFMT_CENTER, 45 );
	pKuList->InsertColumn( 16, LPCTSTR("只风"), LVCFMT_CENTER, 45 );

	CListCtrl* pKuTempChangeList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_TEMP_CHANGE);
	pKuTempChangeList->SetExtendedStyle( pKuTempChangeList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线
	pKuTempChangeList->InsertColumn( 0, LPCTSTR("库房"), LVCFMT_LEFT, 45 );
	pKuTempChangeList->InsertColumn( 1, LPCTSTR("终上限"), LVCFMT_CENTER, 60 );
	pKuTempChangeList->InsertColumn( 2, LPCTSTR("终下限"), LVCFMT_CENTER, 60 );
	pKuTempChangeList->InsertColumn( 3, LPCTSTR("天"), LVCFMT_CENTER, 35 );
	//pKuTempChangeList->InsertColumn( 4, LPCTSTR("日期"), LVCFMT_CENTER, 60 );

	SumLoad();
	KuLoad();

	CButton* pKu = (CButton*)GetDlgItem(IDC_RADIO_KU1);
	pKu->SetCheck(1);

	CButton* pCtrlMode = (CButton*)GetDlgItem(IDC_RADIO_KU_CTRL2);
	CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE2);
	pCtrlMode->SetCheck(1);
	pWorkMode->SetCheck(1);

	CButton* pTimeStOn = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_ON);
	CButton* pTimeStOff = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_OFF);
	CButton* pAllOffOff = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_OFF);
	CButton* pAllOffOn = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_ON);
	CButton* pAllOffNo = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_NO);
	CButton* pTimeOutTime = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TIME);
	CButton* pTimeOutTemp = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TEMP);
	pTimeStOn->SetCheck(1);
	pTimeStOff->SetCheck(0);
	pAllOffOff->SetCheck(1);
	pAllOffOn->SetCheck(0);
	pAllOffNo->SetCheck(0);
	pTimeOutTime->SetCheck(1);
	pTimeOutTemp->SetCheck(0);

	CButton* pOnlyOffFJ = (CButton*)GetDlgItem(IDC_CHECK_ONLY_OFF_FJ);
	pOnlyOffFJ->SetCheck(0);

	OnBnClickedCheckKuTempChange();
}

void CSETDlg::KuLoad(void)
{
	SumLoad();
	CListCtrl* pKuTempChangeList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_TEMP_CHANGE);
	pKuTempChangeList->DeleteAllItems();
	CListCtrl* pKuList = (CListCtrl*)GetDlgItem(IDC_LIST_KU);
	pKuList->DeleteAllItems();

	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 ORDER BY KuNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-17.1
	
	if(!m_pRs->BOF)	//后面有m_pRs->moveFirst
	{
		int nListEntry = 0;
		while( !m_pRs->adoEOF )
		{
			CString strKuNum = "", strTempNum = "", strKuStatus = "", strRuKuING = "", strKuStore = "", strTempUp = "", strTempDn = "";
			CString strKuCtrlMode = "", strKuWorkMode = "", strKuAllZu = "", strKuOnlyOffFJ = "";
			CString strKuYSJ = "", strKuFY = "", strKuHQ = "", strKuFJ = "";
			CString strTimeSt = "", strAllOff = "", strTimeOut = "";

			strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum")); 
			pKuList->InsertItem( nListEntry, LPCTSTR( strKuNum ), 0 );
			
			strTempNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempNum"));
			pKuList->SetItemText( nListEntry, 1, LPCTSTR(strTempNum) );
		
			strKuYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuYSJ"));
			strKuFY = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFY"));
			strKuHQ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuHQ"));
			strKuFJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuFJ"));
			pKuList->SetItemText( nListEntry, 2, LPCTSTR(strKuYSJ) );
			pKuList->SetItemText( nListEntry, 3, LPCTSTR(strKuFY) );
			pKuList->SetItemText( nListEntry, 4, LPCTSTR(strKuHQ) );
			pKuList->SetItemText( nListEntry, 5, LPCTSTR(strKuFJ) );

			strKuStatus = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuStatus"));	
			if(strcmp(strKuStatus, "0") == 0)
			{
				pKuList->SetItemText( nListEntry, 6, LPCTSTR("未用") );		//FALSE=0,表示未用
			}
			else
			{
				strRuKuING = (LPCSTR)_bstr_t(m_pRs->GetCollect("RuKuING"));

				(strcmp(strRuKuING, "0") == 0)?
					pKuList->SetItemText( nListEntry, 6, LPCTSTR("在用") ):
					pKuList->SetItemText( nListEntry, 6, LPCTSTR("在用+入库") );

				strKuStore = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuStore"));
				pKuList->SetItemText( nListEntry, 7, LPCTSTR(strKuStore) );		

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
				pKuList->SetItemText( nListEntry, 8, LPCTSTR(strTempUp) );
				pKuList->SetItemText( nListEntry, 9, LPCTSTR(strTempDn) );
			
				strKuCtrlMode = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuCtrlMode"));
				strKuWorkMode = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuWorkMode"));
				strKuAllZu = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuAllZu"));
				strKuOnlyOffFJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuOnlyOffFJ"));
				pKuList->SetItemText( nListEntry, 10, LPCTSTR(strKuCtrlMode) );
				pKuList->SetItemText( nListEntry, 14, LPCTSTR(strKuWorkMode) );
				pKuList->SetItemText( nListEntry, 15, LPCTSTR(strKuAllZu) );
				( strcmp(strKuOnlyOffFJ, "0") == 0)?
					pKuList->SetItemText( nListEntry, 16, LPCTSTR("否") ):
					pKuList->SetItemText( nListEntry, 16, LPCTSTR("是") );

				if(strcmp(strKuCtrlMode, "时开温关") == 0)
				{
					strTimeSt = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeSt"));
					strAllOff = (LPCSTR)_bstr_t(m_pRs->GetCollect("AllOff"));
					strTimeOut = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeOut"));	

					pKuList->SetItemText( nListEntry, 11, LPCTSTR(strTimeSt) );
					pKuList->SetItemText( nListEntry, 12, LPCTSTR(strAllOff) );
					pKuList->SetItemText( nListEntry, 13, LPCTSTR(strTimeOut) );
				}
				else
				{
					pKuList->SetItemText( nListEntry, 11, LPCTSTR("--") );
					pKuList->SetItemText( nListEntry, 12, LPCTSTR("--") );
					pKuList->SetItemText( nListEntry, 13, LPCTSTR("--") );			
				}

			}

			nListEntry++;
			m_pRs->MoveNext();
		}

		m_pRs->MoveFirst();
		int nChangeListEntry = 0;
		while(!m_pRs->adoEOF)
		{
			CString strChangeDays = "";
			BYTE byChangeDays = 255;
			strChangeDays = (LPCSTR)_bstr_t(m_pRs->GetCollect("ChangeDays"));
			byChangeDays = atoi(strChangeDays);
			//if( (byChangeDays != 255) && (byChangeDays != 0) )
			if( byChangeDays != 255)
			{
				CString strChangeKuNum = "", strTempUp2 = "", strTempDn2 = "", strSTTime = "";
				strChangeKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum")); 
				pKuTempChangeList->InsertItem( nChangeListEntry, LPCTSTR( strChangeKuNum ), 0 );

				strTempUp2 = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempUp2"));
				strTempDn2 = (LPCSTR)_bstr_t(m_pRs->GetCollect("TempDn2"));
				float fTempUp2=(float)atof(strTempUp2);
				if( (fTempUp2 > 0.0) && (fTempUp2 < 1.0) )
				{
					strTempUp2.Format("%.1f",fTempUp2);		
				}			
				float fTempDn2=(float)atof(strTempDn2);
				if( (fTempDn2 > 0.0) && (fTempDn2 < 1.0) )
				{
					strTempDn2.Format("%.1f",fTempDn2);		
				}
				pKuTempChangeList->SetItemText( nChangeListEntry, 1, LPCTSTR(strTempUp2) );
				pKuTempChangeList->SetItemText( nChangeListEntry, 2, LPCTSTR(strTempDn2) );

				pKuTempChangeList->SetItemText( nChangeListEntry, 3, LPCTSTR(strChangeDays) );

				/*strSTTime = (LPCSTR)_bstr_t(m_pRs->GetCollect("ChangeST"));
				COleDateTime TimeST;
				TimeST.ParseDateTime(strSTTime);
				CString strSTTimeDisp = "";
				strSTTimeDisp.Format("%02d-%02d", TimeST.GetMonth(), TimeST.GetDay());
				pKuTempChangeList->SetItemText( nChangeListEntry, 4, LPCTSTR(strSTTimeDisp) );*/

				nChangeListEntry++;
			}		
			m_pRs->MoveNext();
		}
	}
	m_pRs->raw_Close();		//RS-17.2

	if(g_pCanDlg != NULL)
	{
		g_pCanDlg->KuCtrlDispLoad();
		g_pCanDlg->YSJCtrlDispLoad();
		g_pCanDlg->UpdateRuKuComobox();
		g_pCanDlg->UpdateKuManual();
	}
}
void CSETDlg::OnBnClickedButtonKuOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strKuSetText = "";
	UpdateData(TRUE);

	CComboBox* pKuNum = (CComboBox*)GetDlgItem(IDC_COMBO_KU_NUM);
	int nKuNum = pKuNum->GetCurSel()+1;

	CString strSql = "";
	strSql.Format("SELECT * From SetKu WHERE KuNum = %d AND KuNum != 0", nKuNum);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-18.1
	if(!m_pRs->BOF)	//已存在该库房数
	{
		CString strRe = "";
		strRe.Format("库房%d 已存在，将覆盖", nKuNum);
		//MessageBox(strRe);
		strSql.Format("DELETE FROM SetKu WHERE KuNum = %d AND KuNum != 0", nKuNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}
	m_pRs->raw_Close();	//RS-18.2

	CButton* pKu = (CButton*)GetDlgItem(IDC_RADIO_KU1);
	BOOL bKuStatus;
	(pKu->GetCheck())? bKuStatus = FALSE : bKuStatus = TRUE;

	if( (nKuNum > 0) && (m_strTempNum != "") )
	{
		//150825
		if(strcmp(m_strKuStore, "") == 0)
		{
			m_strKuStore = "**";
		}
		
		if(bKuStatus)
		{
			BOOL bRuKuING = FALSE;
			CButton* pRuKuING = (CButton*)GetDlgItem(IDC_CHECK_RUKU_ING);
			( pRuKuING->GetCheck() )? bRuKuING = TRUE:bRuKuING = FALSE;
			
			CString strCtrlMode = "";
			for(int i=0; i<3; i++)
			{
				CButton* pCtrlMode = (CButton*)GetDlgItem(IDC_RADIO_KU_CTRL1+i);
				if( pCtrlMode->GetCheck() == 1 )
				{
					pCtrlMode->GetWindowTextA(strCtrlMode);	
					break;
				}
			}

			CString strWorkMode = "";
			for(int i=0; i<3; i++)
			{
				CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+i);
				if( pWorkMode->GetCheck() == 1 )
				{
					pWorkMode->GetWindowTextA(strWorkMode);	
					break;
				}
			}

			CString strTimeSt = "255", strAllOff = "255", strTimeOut = "255";
			if(strcmp(strCtrlMode, "时开温关") == 0)
			{
				CButton* pTimeStOn = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_ON);
				CButton* pTimeOutTime = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TIME);
				(pTimeStOn->GetCheck())? strTimeSt = "始时开机": strTimeSt = "始时不开";
				(pTimeOutTime->GetCheck())? strTimeOut = "终时即关": strTimeOut = "终时待温";

				CButton* pAllOffOff = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_OFF);
				CButton* pAllOffOn = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_ON);
				CButton* pAllOffNo = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_NO);
				if(pAllOffOff->GetCheck())
				{
					strAllOff = "全关即止";
				}
				else
				{
					(pAllOffOn->GetCheck())? strAllOff = "全关等开": strAllOff = "全关跳过";
				}
			}

			CString strKuAllZu = "";
			CButton* pKuAllZu = (CButton*)GetDlgItem(IDC_CHECK_KU_ALL_ZU);
			if( !pKuAllZu->GetCheck() )
			{
				m_byKuAllZu = 0;
			}

			BOOL bKuOnlyOffFJ = FALSE;
			CButton* pKuOnlyOffFJ = (CButton*)GetDlgItem(IDC_CHECK_ONLY_OFF_FJ);
			( pKuOnlyOffFJ->GetCheck() )? bKuOnlyOffFJ = TRUE: bKuOnlyOffFJ = FALSE;

			bFlagEditLastWorkStatus = FALSE;
			
			CString strTime = "1990-06-14 08:00:00";
			CButton* pBtKuTempChange = (CButton*)GetDlgItem(IDC_CHECK_KU_TEMP_CHANGE);
			if(pBtKuTempChange->GetCheck())
			{
				strTime.Format("%d-%02d-%02d 08:00:00", 
					g_mTimeNow.GetYear(), g_mTimeNow.GetMonth(), g_mTimeNow.GetDay());			
			}
			else
			{
				m_fTempUp2 = 255.0;
				m_fTempDn2 = 255.0;
				m_byKuTempChangeDays = 255;
				strTime = "1990-06-14 08:00:00";
			}
			
			//温度上限要大于下限
			if(m_fTempUp < m_fTempDn)
			{
				MessageBox("请检查现用温度上下限");
				float fTemp = 0.0;
				fTemp = m_fTempUp;
				m_fTempUp = m_fTempDn;
				m_fTempDn = fTemp;
			}

			if(m_fTempUp2 < m_fTempDn2)
			{
				MessageBox("请检查最终温度上下限");
				float fTemp2 = 0.0;
				fTemp2 = m_fTempUp2;
				m_fTempUp2 = m_fTempDn2;
				m_fTempDn2 = fTemp2;
			}
			
			//151028 自动只风机
			if((strcmp(m_strKuFY, "0") == 0) && (strcmp(m_strKuHQ, "0") == 0))
				bKuOnlyOffFJ = TRUE;
			
			strSql.Format("INSERT INTO SetKu ([KuNum],[TempNum],[KuStatus],[RuKuING],[WorkStatus],[KuStore],[TempUp],[TempDn],\
						  [KuCtrlMode],[KuWorkMode],[KuYSJ],[KuFY],[KuHQ],[KuFJ],[KuAllZu],[KuOnlyOffFJ],\
						  [TimeSt], [AllOff], [TimeOut], [TempUp2], [TempDn2], [ChangeDays], [ChangeST], [PartKu]) \
						   VALUES (%d,'%s',%d,%d,%d,'%s',%.1f,%.1f,'%s','%s','%s','%s','%s','%s'\
						   ,%d,%d,'%s','%s','%s',%.1f, %.1f, %d, '%s', %d)",
						   nKuNum,m_strTempNum,bKuStatus,bRuKuING,bFlagEditLastWorkStatus,m_strKuStore,m_fTempUp,m_fTempDn,
						   strCtrlMode,strWorkMode,m_strKuYSJ,m_strKuFY,m_strKuHQ,m_strKuFJ,m_byKuAllZu,bKuOnlyOffFJ,
						   strTimeSt, strAllOff, strTimeOut, m_fTempUp2, m_fTempDn2, m_byKuTempChangeDays, strTime, FALSE);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			strKuSetText.Format("设置·库房%d 温度上限%.1f 温度下限%.1f 最终上限%.1f 最终下限%.1f 变化天数%d",
				nKuNum, m_fTempUp, m_fTempDn, m_fTempUp2, m_fTempDn2, m_byKuTempChangeDays);
			MessageBox("请修改：温度校准-下限报警");
		}
		else
		{
			strSql.Format("INSERT INTO SetKu ([KuNum],[TempNum],[KuStatus],[RuKuING],[WorkStatus],[KuStore],[TempUp],[TempDn],\
						  [KuCtrlMode],[KuWorkMode],[KuYSJ],[KuFY],[KuHQ],[KuFJ],[KuAllZu],[KuOnlyOffFJ],\
						  [TimeSt], [AllOff], [TimeOut], [TempUp2], [TempDn2], [ChangeDays], [ChangeST], [PartKu]) \
						   VALUES (%d,'%s',%d,%d,%d,'%s',%.1f,%.1f,'%s','%s','%s','%s','%s',\
						   '%s',%d,%d,'%s','%s','%s', %.1f, %.1f, %d, '%s', %d)",
						   nKuNum,m_strTempNum,bKuStatus,FALSE,FALSE,"255",255.0,255.0,
						   "255","255",m_strKuYSJ,m_strKuFY,m_strKuHQ,m_strKuFJ,255,FALSE,
						   "255", "255", "255", 255.0, 255.0, 255, "1990-06-14 08:00:00", FALSE);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

			strKuSetText.Format("设置·库房%d 未用",nKuNum);
		}

		KuLoad();

		//150907-及时更新
		TempS_GetTempSorST();
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_KU);
	int nCount = pList->GetItemCount();
	if(nCount > 0)
	{
		pList->EnsureVisible(nCount-1, 0);
	}

	Doing2Text(strKuSetText);
	EnKuYSJ_SetKu();
}

void CSETDlg::OnBnClickedButtonKuDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pKuList = (CListCtrl*) GetDlgItem(IDC_LIST_KU);
	int nKuIndex = pKuList->GetNextItem(-1, LVNI_SELECTED);

	if(nKuIndex != -1)
	{
		CString strKuNum = pKuList->GetItemText(nKuIndex, 0);
		int nKuNum = atoi(strKuNum);
		
		CString strSql = "";
		strSql.Format("DELETE FROM SetKu WHERE KuNum = %d AND KuNum != 0",nKuNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		KuLoad();
	}
}

void CSETDlg::OnBnClickedButtonKuEdit()
{
	// TODO: 在此添加控件通知处理程序代码

	CListCtrl* pKuList = (CListCtrl*) GetDlgItem(IDC_LIST_KU);
	int nKuIndex = pKuList->GetNextItem(-1, LVNI_SELECTED);

	if(nKuIndex != -1)
	{
		CString strKuNum = pKuList->GetItemText(nKuIndex, 0);
		int nKuNum = atoi(strKuNum);
		if(nKuNum > m_byKuSum)
		{
			MessageBox("当前库房数大于库房总数，出错！");
		}
		CComboBox* pKuNum = (CComboBox*)GetDlgItem(IDC_COMBO_KU_NUM);
		pKuNum->SelectString(-1, strKuNum);

		m_strTempNum  = pKuList->GetItemText(nKuIndex, 1);
		m_strKuYSJ = pKuList->GetItemText(nKuIndex, 2);
		m_strKuFY = pKuList->GetItemText(nKuIndex, 3);
		m_strKuHQ = pKuList->GetItemText(nKuIndex, 4);
		m_strKuFJ = pKuList->GetItemText(nKuIndex, 5);
		m_strKuStore = pKuList->GetItemText(nKuIndex, 7);

		CString strSqlTempUp = pKuList->GetItemText(nKuIndex, 8);
		CString strSqlTempDn = pKuList->GetItemText(nKuIndex, 9);
		m_fTempUp = (float)atof(strSqlTempUp);
		m_fTempDn = (float)atof(strSqlTempDn);

		CString strKuAllZu = pKuList->GetItemText(nKuIndex, 13);
		m_byKuAllZu = atoi(strKuAllZu);

		CButton* pBtKuTempChange = (CButton*)GetDlgItem(IDC_CHECK_KU_TEMP_CHANGE);
		CListCtrl* pKuTempChangeList = (CListCtrl*)GetDlgItem(IDC_LIST_KU_TEMP_CHANGE);
		int nKuTempChangeIndex = 0;
		for(nKuTempChangeIndex=0; nKuTempChangeIndex<pKuTempChangeList->GetItemCount(); nKuTempChangeIndex++)
		{
			CString strKuTempChangeNum = pKuTempChangeList->GetItemText(nKuTempChangeIndex, 0);
			int nKuTempChangeNum = atoi(strKuTempChangeNum);
			if(nKuTempChangeNum == nKuNum)
			{
				CString strSqlTempUp2 = pKuTempChangeList->GetItemText(nKuTempChangeIndex, 1);
				CString strSqlTempDn2 = pKuTempChangeList->GetItemText(nKuTempChangeIndex, 2);
				CString strChangeDays = pKuTempChangeList->GetItemText(nKuTempChangeIndex, 3);
				m_fTempUp2 = (float)atof(strSqlTempUp2);
				m_fTempDn2 = (float)atof(strSqlTempDn2);
				m_byKuTempChangeDays = atoi(strChangeDays);
				pBtKuTempChange->SetCheck(1);
				break;
			}
			
			if(nKuTempChangeIndex == pKuTempChangeList->GetItemCount()-1)
			{
				m_fTempUp2 = 0.0;
				m_fTempDn2 = 0.0;
				m_byKuTempChangeDays = 0;
				pBtKuTempChange->SetCheck(0);
			}
		}
		OnBnClickedCheckKuTempChange();

		UpdateData(FALSE);

		CButton* pKuAllZu = (CButton*)GetDlgItem(IDC_CHECK_KU_ALL_ZU);
		m_byKuAllZu ? pKuAllZu->SetCheck(1): pKuAllZu->SetCheck(0);

		CString strKuOnlyOffFJ = pKuList->GetItemText(nKuIndex, 16);
		CButton* pKuOnlyOffFJ = (CButton*)GetDlgItem(IDC_CHECK_ONLY_OFF_FJ);
		(strcmp(strKuOnlyOffFJ, "是") == 0)?
			pKuOnlyOffFJ->SetCheck(1): pKuOnlyOffFJ->SetCheck(0);

		CString strKuStatus = pKuList->GetItemText(nKuIndex, 6);
		CString strKuStatusLeft = strKuStatus.Left(4);

		CButton* pRuKuING = (CButton*)GetDlgItem(IDC_CHECK_RUKU_ING);
		(strcmp(strKuStatusLeft, strKuStatus) == 0)? pRuKuING->SetCheck(0): pRuKuING->SetCheck(1);
		
		for(int i=0; i<2; i++)
		{
			CButton* pKu = (CButton*)GetDlgItem(IDC_RADIO_KU1+i);
			CString strStatus = "";
			pKu->GetWindowTextA(strStatus);
			(strcmp(strKuStatusLeft, strStatus) == 0)? pKu->SetCheck(1): pKu->SetCheck(0);
		}

		bFlagEditLastWorkStatus = FALSE;
		CString strSql = "";
		strSql.Format("SELECT * FROM SetKu WHERE KuNum > 0 AND KuNum = %d AND KuStatus = 1", nKuNum);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//56.1
		if(!m_pRs->BOF)
		{
			CString strKuWorkFlag = (LPCSTR)_bstr_t(m_pRs->GetCollect("WorkStatus"));
			(strcmp(strKuWorkFlag, "0") == 0)?
				bFlagEditLastWorkStatus = FALSE://关机状态
				bFlagEditLastWorkStatus = TRUE;	//开机状态
		}
		m_pRs->raw_Close();	//56.2

		CString strKuCtrlMode = pKuList->GetItemText(nKuIndex, 10);
		for(int j=0; j<NumKuCtrlMode; j++)
		{
			CButton* pCtrlMode = (CButton*)GetDlgItem(IDC_RADIO_KU_CTRL1+j);
			CString strMode = "";
			pCtrlMode->GetWindowTextA(strMode);
			(strcmp(strKuCtrlMode, strMode) == 0)? pCtrlMode->SetCheck(1): pCtrlMode->SetCheck(0);
		}

		CButton* pTimeStOn = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_ON);
		CButton* pTimeStOff = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_OFF);
		CButton* pAllOffOff = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_OFF);
		CButton* pAllOffOn = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_ON);
		CButton* pAllOffNo = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_NO);
		CButton* pTimeOutTime = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TIME);
		CButton* pTimeOutTemp = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TEMP);
		if(strcmp(strKuCtrlMode, "时开温关") == 0)
		{
			CString strTimeSt = pKuList->GetItemText(nKuIndex, 11);
			CString strAllOff = pKuList->GetItemText(nKuIndex, 12);
			CString strTimeOut = pKuList->GetItemText(nKuIndex, 13);

			pTimeStOn->SetCheck( (strcmp(strTimeSt, "始时开机") == 0) );
			pTimeStOff->SetCheck( (strcmp(strTimeSt, "始时不开") == 0) );
			pAllOffOff->SetCheck( (strcmp(strAllOff, "全关即止") == 0) );
			pAllOffOn->SetCheck( (strcmp(strAllOff, "全关等开") == 0) );
			pAllOffNo->SetCheck( (strcmp(strAllOff, "全关跳过") == 0) );
			pTimeOutTime->SetCheck( (strcmp(strTimeOut, "终时即关") == 0) );
			pTimeOutTemp->SetCheck( (strcmp(strTimeOut, "终时待温") == 0) );	
		}
		else
		{
			pTimeStOn->SetCheck(0);
			pTimeStOff->SetCheck(0);
			pAllOffOff->SetCheck(0);
			pAllOffOn->SetCheck(0);	
			pAllOffNo->SetCheck(0);
			pTimeOutTime->SetCheck(0);
			pTimeOutTemp->SetCheck(0);
		}

		CString strKuWorkMode = pKuList->GetItemText(nKuIndex, 14);
		for(int k=0; k<NumKuWorkMode; k++)
		{
			CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+k);
			CString strMode = "";
			pWorkMode->GetWindowTextA(strMode);
			(strcmp(strKuWorkMode, strMode) == 0)? pWorkMode->SetCheck(1): pWorkMode->SetCheck(0);
		}

		if(strcmp(strKuCtrlMode, "时开时关") == 0)
		{
			for(int j=0; j<NumKuWorkMode; j++)
			{
				CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+j);
				if(j != 1)
				{
					pWorkMode->EnableWindow(TRUE);		
				}
				else
				{
					pWorkMode->EnableWindow(FALSE);
					pWorkMode->SetCheck(0);
				}
			}	

			pTimeStOn->SetCheck(0);
			pTimeStOff->SetCheck(0);
			pAllOffOff->SetCheck(0);
			pAllOffOn->SetCheck(0);
			pAllOffNo->SetCheck(0);
			pTimeOutTime->SetCheck(0);
			pTimeOutTemp->SetCheck(0);
			pTimeStOn->EnableWindow(0);
			pTimeStOff->EnableWindow(0);
			pAllOffOff->EnableWindow(0);
			pAllOffOn->EnableWindow(0);	
			pAllOffNo->EnableWindow(0);	
			pTimeOutTime->EnableWindow(0);
			pTimeOutTemp->EnableWindow(0);
		}
		else if(strcmp(strKuCtrlMode, "时开温关") == 0)
		{
			for(int j=0; j<NumKuWorkMode; j++)
			{
				CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+j);
				//pWorkMode->EnableWindow(TRUE);
				if(j == 0)
				{
					pWorkMode->EnableWindow(FALSE);
					pWorkMode->SetCheck(FALSE);
				}
				else
				{
					pWorkMode->EnableWindow(TRUE);
					//pWorkMode->SetCheck(TRUE);
				}
			}

			pTimeStOn->EnableWindow(1);
			pTimeStOff->EnableWindow(1);
			pAllOffOff->EnableWindow(1);
			pAllOffOn->EnableWindow(1);	
			pAllOffNo->EnableWindow(1);
			pTimeOutTime->EnableWindow(1);
			pTimeOutTemp->EnableWindow(1);
		}
		else if(strcmp(strKuCtrlMode, "温开温关") == 0)
		{
			for(int j=0; j<NumKuWorkMode; j++)
			{
				CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+j);
				if(j == 0)
				{
					pWorkMode->EnableWindow(FALSE);
				}
				else
				{
					pWorkMode->EnableWindow(TRUE);
				}
			}
			
			pTimeStOn->SetCheck(0);
			pTimeStOff->SetCheck(0);
			pAllOffOff->SetCheck(0);
			pAllOffOn->SetCheck(0);
			pAllOffNo->SetCheck(0);
			pTimeOutTime->SetCheck(0);
			pTimeOutTemp->SetCheck(0);
			pTimeStOn->EnableWindow(0);
			pTimeStOff->EnableWindow(0);
			pAllOffOff->EnableWindow(0);
			pAllOffOn->EnableWindow(0);	
			pAllOffNo->EnableWindow(0);	
			pTimeOutTime->EnableWindow(0);
			pTimeOutTemp->EnableWindow(0);
		}
		else
		{
			for(int j=0; j<NumKuWorkMode; j++)
			{
				CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+j);
				pWorkMode->EnableWindow(TRUE);
				pWorkMode->SetCheck(0);
			}

			pTimeStOn->SetCheck(0);
			pTimeStOff->SetCheck(0);
			pAllOffOff->SetCheck(0);
			pAllOffOn->SetCheck(0);	
			pTimeOutTime->SetCheck(0);
			pTimeOutTemp->SetCheck(0);
			pTimeStOn->EnableWindow(1);
			pTimeStOff->EnableWindow(1);
			pAllOffOff->EnableWindow(1);
			pAllOffOn->EnableWindow(1);	
			pTimeOutTime->EnableWindow(1);
			pTimeOutTemp->EnableWindow(1);	
		}
	}
}

void CSETDlg::OnNMDblclkListKu(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	OnBnClickedButtonKuEdit();
}
void CSETDlg::OnBnClickedRadioKuCtrl1()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int j=0; j<NumKuWorkMode; j++)	
	{
		CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+j);		
		if(j == 0)
		{
			pWorkMode->EnableWindow(FALSE);
			//pWorkMode->SetCheck(FALSE);
		}
		else
		{
			pWorkMode->EnableWindow(TRUE);
			//pWorkMode->SetCheck(TRUE);
		}
	}

	CButton* pTimeStOn = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_ON);
	CButton* pTimeStOff = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_OFF);
	CButton* pAllOffOff = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_OFF);
	CButton* pAllOffOn = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_ON);
	CButton* pAllOffNo = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_NO);
	CButton* pTimeOutTime = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TIME);
	CButton* pTimeOutTemp = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TEMP);
	pTimeStOn->SetCheck(0);
	pTimeStOff->SetCheck(0);
	pAllOffOff->SetCheck(0);
	pAllOffOn->SetCheck(0);
	pAllOffNo->SetCheck(0);
	pTimeOutTime->SetCheck(0);
	pTimeOutTemp->SetCheck(0);
	pTimeStOn->EnableWindow(0);
	pTimeStOff->EnableWindow(0);
	pAllOffOff->EnableWindow(0);
	pAllOffOn->EnableWindow(0);	
	pAllOffNo->EnableWindow(0);	
	pTimeOutTime->EnableWindow(0);
	pTimeOutTemp->EnableWindow(0);
}

void CSETDlg::OnBnClickedRadioKuCtrl3()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int j=0; j<NumKuWorkMode; j++)
	{
		CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+j);
		if(j != 1)
		{
			pWorkMode->EnableWindow(TRUE);		
		}
		else
		{
			pWorkMode->EnableWindow(FALSE);
			pWorkMode->SetCheck(0);
		}
	}

	CButton* pTimeStOn = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_ON);
	CButton* pTimeStOff = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_OFF);
	CButton* pAllOffOff = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_OFF);
	CButton* pAllOffOn = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_ON);
	CButton* pAllOffNo = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_NO);
	CButton* pTimeOutTime = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TIME);
	CButton* pTimeOutTemp = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TEMP);
	pTimeStOn->SetCheck(0);
	pTimeStOff->SetCheck(0);
	pAllOffOff->SetCheck(0);
	pAllOffOn->SetCheck(0);
	pAllOffNo->SetCheck(0);
	pTimeOutTime->SetCheck(0);
	pTimeOutTemp->SetCheck(0);
	pTimeStOn->EnableWindow(0);
	pTimeStOff->EnableWindow(0);
	pAllOffOff->EnableWindow(0);
	pAllOffOn->EnableWindow(0);	
	pAllOffNo->EnableWindow(0);	
	pTimeOutTime->EnableWindow(0);
	pTimeOutTemp->EnableWindow(0);
}

void CSETDlg::OnBnClickedRadioKuCtrl2()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int j=0; j<NumKuWorkMode; j++)
	{
		CButton* pWorkMode = (CButton*)GetDlgItem(IDC_RADIO_WORK_MODE1+j);
		//pWorkMode->EnableWindow(TRUE);
		if(j == 0)
		{
			pWorkMode->EnableWindow(FALSE);
			pWorkMode->SetCheck(FALSE);
		}
		else
		{
			pWorkMode->EnableWindow(TRUE);
			(j == 1)? pWorkMode->SetCheck(TRUE): pWorkMode->SetCheck(FALSE);
		}
	}

	CButton* pTimeStOn = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_ON);
	CButton* pTimeStOff = (CButton*)GetDlgItem(IDC_RADIO_TIME_ST_OFF);
	CButton* pAllOffOff = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_OFF);
	CButton* pAllOffOn = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_ON);
	CButton* pAllOffNo = (CButton*)GetDlgItem(IDC_RADIO_ALL_OFF_NO);
	CButton* pTimeOutTime = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TIME);
	CButton* pTimeOutTemp = (CButton*)GetDlgItem(IDC_RADIO_TIME_OFF_TEMP);
	pTimeStOn->SetCheck(1);
	pTimeStOff->SetCheck(0);
	pAllOffOff->SetCheck(1);
	pAllOffOn->SetCheck(0);
	pAllOffNo->SetCheck(0);
	pTimeOutTime->SetCheck(1);
	pTimeOutTemp->SetCheck(0);
	pTimeStOn->EnableWindow(1);
	pTimeStOff->EnableWindow(1);
	pAllOffOff->EnableWindow(1);
	pAllOffOn->EnableWindow(1);	
	pAllOffNo->EnableWindow(1);
	pTimeOutTime->EnableWindow(1);
	pTimeOutTemp->EnableWindow(1);
}
void CSETDlg::OnBnClickedCheckKuTempChange()
{
	CButton* pBtKuTempChange = (CButton*)GetDlgItem(IDC_CHECK_KU_TEMP_CHANGE);
	CEdit* pEditKuTempDn2 = (CEdit*)GetDlgItem(IDC_EDIT_TEMP_DN2);
	CEdit* pEditKuTempUp2 = (CEdit*)GetDlgItem(IDC_EDIT_TEMP_UP2);
	CEdit* pEditKuTempChangeDays = (CEdit*)GetDlgItem(IDC_EDIT_KU_TEMP_CHANGE_DAYS);

	if(pBtKuTempChange->GetCheck())
	{
		pEditKuTempDn2->EnableWindow(TRUE);
		pEditKuTempUp2->EnableWindow(TRUE);
		pEditKuTempChangeDays->EnableWindow(TRUE);
	}
	else
	{
		pEditKuTempDn2->EnableWindow(FALSE);
		pEditKuTempUp2->EnableWindow(FALSE);
		pEditKuTempChangeDays->EnableWindow(FALSE);
	}
}

void CSETDlg::OnBnClickedCheckKuSetTempChangeHour()
{
	CButton* pBtKuTempChangeHour = (CButton*)GetDlgItem(IDC_CHECK_KU_SET_TEMP_CHANGE_HOUR);
	CEdit* pEditKuTempChangeHour = (CEdit*)GetDlgItem(IDC_EDIT_KU_SET_TEMP_CHANGE_HOUR);
	if(pBtKuTempChangeHour->GetCheck())
	{
		UpdateData(TRUE);	

		if( (m_byKuTempChangeHour > 23) || (m_byKuTempChangeHour < 0) )
		{
			MessageBox("温度递变小时 0~23");
			pBtKuTempChangeHour->SetCheck(FALSE);
			return;
		}

		CString strKuTempChangeHour = "";
		strKuTempChangeHour.Format("%d", m_byKuTempChangeHour);

		CString strSql = "";
		strSql.Format("UPDATE SetKu SET [KuCtrlMode] = '%s' WHERE KuNum = 0", strKuTempChangeHour);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		MessageBox("递变小时数已修改！");

		SumLoad();
	}
	else
	{
		pEditKuTempChangeHour->EnableWindow(TRUE);
	}
	Doing2Text("设置·递变小时");
}

////////////////////////库房快速设置///////////////////////////////////////////
void CSETDlg::OnBnClickedButtonSetkuYsj()
{
	CString str = "将统一修改 压缩机\n\n  空--全部\n   0--非法\n其他--复制";
	CString strSql = "";
	int nMSGB = AfxMessageBox(str, MB_YESNO);
	if(nMSGB == IDYES)
	{
		UpdateData(TRUE);

		if(strcmp(m_strKuYSJ, "0") == 0)
		{
			AfxMessageBox("不能填入0");
			return;
		}

		if(!m_strKuYSJ.GetLength())
		{
			strSql = "SELECT * FROM SetYSJ WHERE YSJ > 0 ORDER BY YSJ ASC";
			m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-19.1
			while(!m_pRs->adoEOF)
			{
				CString strYSJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("YSJ"));
				m_strKuYSJ += strYSJ;
				m_pRs->MoveNext();
				if(!m_pRs->adoEOF)
				{
					m_strKuYSJ += ",";
				}
			}
			m_pRs->raw_Close();	//RS-19.2
		}

		strSql.Format("UPDATE SetKu SET [KuYSJ] = '%s' WHERE KuNum > 0", m_strKuYSJ);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		KuLoad();

		CString strText = "";
		strText.Format("统一修改压缩机 - %s", m_strKuYSJ);
		Doing2Text(strText);

		EnKuYSJ_SetKu();
	}
}

void CSETDlg::OnBnClickedButtonSetkuFy()
{
	CString strSql = "";
	CString str = "将统一修改 分液\n\n  空--屏蔽\n   0--屏蔽\n   1--库号\n   2--双号\n   3--单号\n其他--双分液";
	int nMSGB = AfxMessageBox(str, MB_YESNO);
	if(nMSGB == IDYES)
	{
		UpdateData(TRUE);
		if(m_strKuFY.GetLength())
		{
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 ORDER BY KuNum ASC";
			m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-20.1
			while(!m_pRs->adoEOF)
			{
				CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
				int nKuNum = atoi(strKuNum);

				if(strcmp(m_strKuFY, "1") == 0)	//单分液，库房号
				{
					strSql.Format("UPDATE SetKu SET [KuFY] = '%s' WHERE KuNum > 0 AND KuNum = %d", strKuNum, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);				
				}
				else if(strcmp(m_strKuFY, "0") == 0) //与空效果相同
				{
					m_strKuFY = "0";
					strSql.Format("UPDATE SetKu SET [KuFY] = '%s' WHERE KuNum > 0 AND KuNum = %d", m_strKuFY, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
				}
				else if(strcmp(m_strKuFY, "2") == 0)
				{
					CString strFY = "";
					strFY.Format("%d", nKuNum*2);
					strSql.Format("UPDATE SetKu SET [KuFY] = '%s' WHERE KuNum > 0 AND KuNum = %d", strFY, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);				
				}
				else if(strcmp(m_strKuFY, "3") == 0)
				{
					CString strFY = "";
					strFY.Format("%d", nKuNum*2-1);
					strSql.Format("UPDATE SetKu SET [KuFY] = '%s' WHERE KuNum > 0 AND KuNum = %d", strFY, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);				
				}
				else	//其他，双分液
				{
					CString strDualFY = "";
					strDualFY.Format("%d,%d", nKuNum*2-1, nKuNum*2);
					strSql.Format("UPDATE SetKu SET [KuFY] = '%s' WHERE KuNum > 0 AND KuNum = %d", strDualFY, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
				}

				m_pRs->MoveNext();
			}
			m_pRs->raw_Close();	//RS-20.2	
		}
		else
		{
			m_strKuFY = "0";
			strSql = "UPDATE SetKu SET [KuFY] = '0' WHERE KuNum > 0";
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		
		KuLoad();

		CString strText = "";
		strText.Format("统一修改分液 - %s", m_strKuFY);
		Doing2Text(strText);
	}
}

void CSETDlg::OnBnClickedButtonSetkuHq()
{
	CString strSql = "";
	CString str = "将统一修改 回气\n\n  空--屏蔽\n   0--屏蔽\n其他--库号";
	int nMSGB = AfxMessageBox(str, MB_YESNO);
	if(nMSGB == IDYES)
	{
		UpdateData(TRUE);
		if(m_strKuHQ.GetLength())
		{
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 ORDER BY KuNum ASC";
			m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-21.1
			while(!m_pRs->adoEOF)
			{
				CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
				int nKuNum = atoi(strKuNum);

				if(strcmp(m_strKuHQ, "0") == 0)
				{
					strSql.Format("UPDATE SetKu SET [KuHQ] = '%s' WHERE KuNum > 0 AND KuNum = %d", m_strKuHQ, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
				}
				else
				{
					strSql.Format("UPDATE SetKu SET [KuHQ] = '%s' WHERE KuNum > 0 AND KuNum = %d", strKuNum, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
				}

				m_pRs->MoveNext();
			}
			m_pRs->raw_Close();		//RS-21.2	
		}
		else
		{
			m_strKuHQ = "0";
			strSql = "UPDATE SetKu SET [KuHQ] = '0' WHERE KuNum > 0";
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		
		KuLoad();

		CString strText = "";
		strText.Format("统一修改回气 - %s", m_strKuHQ);
		Doing2Text(strText);
	}
}

void CSETDlg::OnBnClickedButtonSetkuFj()
{
	CString strSql = "";
	CString str = "将统一修改 风机\n\n  空--屏蔽\n   0--屏蔽\n其他--库号";
	int nMSGB = AfxMessageBox(str, MB_YESNO);
	if(nMSGB == IDYES)
	{
		UpdateData(TRUE);
		if(m_strKuFJ.GetLength())
		{
			strSql = "SELECT * FROM SetKu WHERE KuNum > 0 ORDER BY KuNum ASC";
			m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-22.1
			while(!m_pRs->adoEOF)
			{
				CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
				int nKuNum = atoi(strKuNum);

				if(strcmp(m_strKuFJ, "0") == 0)
				{
					strSql.Format("UPDATE SetKu SET [KuFJ] = '%s' WHERE KuNum > 0 AND KuNum = %d", m_strKuFJ, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
				}
				else
				{
					strSql.Format("UPDATE SetKu SET [KuFJ] = '%s' WHERE KuNum > 0 AND KuNum = %d", strKuNum, nKuNum);
					m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
				}

				m_pRs->MoveNext();
			}
			m_pRs->raw_Close();		//RS-22.2	
		}
		else
		{
			m_strKuFJ = "0";
			strSql = "UPDATE SetKu SET [KuFJ] = '0' WHERE KuNum > 0";
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		
		KuLoad();

		CString strText = "";
		strText.Format("统一修改风机 - %s", m_strKuFJ);
		Doing2Text(strText);
	}
}

void CSETDlg::OnBnClickedCheckOnlyOffFj()
{
	CString strSql = "";
	CComboBox* pKuNum = (CComboBox*)GetDlgItem(IDC_COMBO_KU_NUM);
	int nKuNum = pKuNum->GetCurSel()+1;
	if(nKuNum == 0)
	{
		int nMSGB = AfxMessageBox("将统一修改 仅动作风机", MB_YESNO);
		if(nMSGB == IDYES)
		{
			CButton* pKuOnlyOffFJ = (CButton*)GetDlgItem(IDC_CHECK_ONLY_OFF_FJ);
			if( pKuOnlyOffFJ->GetCheck() )
			{
				strSql = "UPDATE SetKu SET [KuOnlyOffFJ] = 1 WHERE KuNum > 0";
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
			}
			else
			{
				strSql = "UPDATE SetKu SET [KuOnlyOffFJ] = 0 WHERE KuNum > 0";
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
			}

			KuLoad();

			CString strText = "";
			strText.Format("统一修改单开关机只风机 - %d", pKuOnlyOffFJ->GetCheck());
			Doing2Text(strText);
		}
	}
}

void CSETDlg::OnBnClickedRadioKu2()
{
	for(int i=0; i<3; i++)
	{
		CButton* pCtrlMode = (CButton*)GetDlgItem(IDC_RADIO_KU_CTRL1+i);
		if( i == 1 )
		{
			pCtrlMode->SetCheck(1);
			PostMessage( WM_COMMAND, MAKEWPARAM(IDC_RADIO_KU_CTRL2, BN_CLICKED), NULL);
		}
		else
		{
			pCtrlMode->SetCheck(0);	
		}
	}
}

void CSETDlg::OnBnClickedButtonSetkuMode()
{
	CListCtrl* pKuList = (CListCtrl*) GetDlgItem(IDC_LIST_KU);
	int nKuIndex = pKuList->GetNextItem(-1, LVNI_SELECTED);
	if(nKuIndex != -1)
	{
		CString str = "将统一修改 模式";
		int nMSGB = AfxMessageBox(str, MB_YESNO);
		if(nMSGB == IDYES)
		{
			CString strKuNum = pKuList->GetItemText(nKuIndex, 0);
			int nKuNum = atoi(strKuNum);
			CString strSql = "";
			strSql.Format("SELECT * FROM SetKu WHERE KuNum = %d AND KuNum > 0", nKuNum);
			m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151117-1.1

			if(!m_pRs->BOF)
			{
				CString strKuCtrlMode = "255", strKuWorkMode = "255";
				CString strTimeSt = "255", strAllOff = "255", strTimeOut = "255";
				strKuCtrlMode = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuCtrlMode"));
				strKuWorkMode = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuWorkMode"));
				if(strcmp(strKuCtrlMode, "时开温关") == 0)
				{
					strTimeSt = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeSt"));
					strAllOff = (LPCSTR)_bstr_t(m_pRs->GetCollect("AllOff"));
					strTimeOut = (LPCSTR)_bstr_t(m_pRs->GetCollect("TimeOut"));						
				}
				strSql.Format("UPDATE SetKu SET [KuCtrlMode] = '%s',[TimeSt] = '%s',[AllOff] = '%s',[TimeOut] = '%s',\
					[KuWorkMode] = '%s' WHERE KuNum > 0 AND KuStatus = 1",
					strKuCtrlMode, strTimeSt, strAllOff, strTimeOut, strKuWorkMode);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
			}
			m_pRs->raw_Close();	//151117-1.2

			KuLoad();

			CString strText = "统一修改模式设置";
			Doing2Text(strText);
		}	
	}
}

////////////////////////ID设置////////////////////////////////////////////////
void CSETDlg::CidInit()
{
	CListBox* pCidItem = (CListBox*)GetDlgItem(IDC_LIST_CID_ITEM);
	CListCtrl* pCidList = (CListCtrl*)GetDlgItem(IDC_LIST_CID);

	for(int i=0; i<sizeof(aCidItem)/4; i++)
	{
		pCidItem->AddString(*(aCidItem+i));
	}

	pCidList->SetExtendedStyle( pCidList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pCidList->InsertColumn( 0, LPCTSTR("编号"), LVCFMT_CENTER, 45 );
	pCidList->InsertColumn( 1, LPCTSTR("电气"), LVCFMT_CENTER, 45 );
	pCidList->InsertColumn( 2, LPCTSTR("项目"), LVCFMT_CENTER, 60 );
	pCidList->InsertColumn( 3, LPCTSTR("号码"), LVCFMT_LEFT, 45 );

	CidLoad();

	return;
}

void CSETDlg::CidLoad()
{
	CListCtrl* pCidList = (CListCtrl*)GetDlgItem(IDC_LIST_CID);
	pCidList->DeleteAllItems();
	CString strSql = "";
	strSql = "SELECT * FROM SetCid ORDER BY Cid,CidDQ,IdItemNum ASC";
	m_pRs->Open((_variant_t)strSql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//RS-23.1

	int nListEntry = 0;
	while( ! m_pRs->adoEOF )
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
	m_pRs->raw_Close();	//RS-23.2

	int nCount = pCidList->GetItemCount();
	if(nCount > 0)
	{
		pCidList->EnsureVisible(nCount-2, 0);
	}

	if(g_pCanDlg != NULL)
		g_pCanDlg->CidCtrlLoad();

	return;
}

void CSETDlg::OnBnClickedButtonIdAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strSql = "";
	UpdateData(TRUE);
	
	CListBox* pCidItem = (CListBox*) GetDlgItem(IDC_LIST_CID_ITEM);
	int nTempCidItem = pCidItem->GetCurSel();
	if( (nTempCidItem != -1) && (m_byCidID > 2) )
	{
		int nTempCidItem = pCidItem->GetCurSel();
		CString strCidItem = aCidItem[nTempCidItem];

		//去重
		if(m_byCidDQ != 0)
		{
			if( (strcmp(strCidItem, "水泵") == 0) || (strcmp(strCidItem, "氨泵") == 0) || (strcmp(strCidItem, "风机") == 0)  || (strcmp(strCidItem, "电磁阀") == 0) )
			{	
			}
			else
			{
				MessageBox("非电气节点不能设置电气编号");
				return;
			}		
		}

		strSql.Format("SELECT * From SetCid WHERE IdItem = '%s' AND IdItemNum = %d", strCidItem, m_byCidItemNum);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-24.1
		if(!m_pRs->BOF)	
		{
			if(m_byCidID > 2)
			{
				CString strRe = "";
				strRe.Format("%s %d 已存在，将覆盖", strCidItem, m_byCidItemNum);
				MessageBox(strRe);
				strSql.Format("DELETE FROM SetCid WHERE IdItem = '%s' AND IdItemNum = %d", strCidItem, m_byCidItemNum);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);		
			}
		}
		m_pRs->raw_Close();	//RS-24.2

		strSql.Format("SELECT * From SetCid WHERE Cid = %d AND CidDQ = %d", m_byCidID, m_byCidDQ);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-25.1
		if(!m_pRs->BOF)	//已存在该ID数
		{
			if(m_byCidID > 2)
			{
				CString strRe = "";
				strRe.Format("编号%d 电气%d 已存在，将覆盖", m_byCidID, m_byCidDQ);
				MessageBox(strRe);
				strSql.Format("DELETE FROM SetCid WHERE Cid = %d AND CidDQ = %d", m_byCidID, m_byCidDQ);
				m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);		
			}
		}
		m_pRs->raw_Close();	//RS-25.2
	
		//增加
		if(m_byCidDQ)
		{
			if( (strcmp(strCidItem, "水泵") == 0) || (strcmp(strCidItem, "氨泵") == 0) || (strcmp(strCidItem, "风机") == 0)  || (strcmp(strCidItem, "电磁阀") == 0) ) 
			{
				strSql.Format("INSERT INTO SetCid VALUES(%d, %d, '%s', %d)", m_byCidID, m_byCidDQ, strCidItem, m_byCidItemNum);		
			}
			else
			{
				strSql.Format("INSERT INTO SetCid VALUES(%d, %d, '%s', %d)", m_byCidID, 0, strCidItem, m_byCidItemNum);	
				MessageBox("非电气节点不能设置电气编号");
			}			
		}
		else
		{
			strSql.Format("INSERT INTO SetCid VALUES(%d, %d, '%s', %d)", m_byCidID, m_byCidDQ, strCidItem, m_byCidItemNum);
		}
	
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		CidLoad();	
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_CID);
	int nCount = pList->GetItemCount();
	if(nCount > 0)
	{
		pList->EnsureVisible(nCount-1, 0);
	}

	Doing2Text("设置·节点编号");

	return;
}

void CSETDlg::OnBnClickedButtonIdDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pCid = (CListCtrl*) GetDlgItem(IDC_LIST_CID);
	int nCidIndex = pCid->GetNextItem(-1, LVNI_SELECTED);

	if(nCidIndex > 3)	//延时和两个主机不能删0 1
	{
		CString strCurCid = pCid->GetItemText(nCidIndex, 0);
		int nCurCid = atoi(strCurCid);
		CString strCurCidDQ = pCid->GetItemText(nCidIndex, 1);
		int nCurCidDQ = atoi(strCurCidDQ);

		if(nCurCid > 2)
		{
			CString strSql = "";
			strSql.Format("DELETE FROM SetCid WHERE Cid = %d AND CidDQ = %d", nCurCid, nCurCidDQ);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);		
		}

		CidLoad();
	}

	Doing2Text("设置·节点编号");
	return;
}

void CSETDlg::OnNMDblclkListCid(NMHDR *pNMHDR, LRESULT *pResult)
{
	//CString aCidItem[] = {"压缩机", "氨阀", "分液", "回气", "吸气", "排气", "风机", "水泵", "氨泵", "电磁阀"};
	//#define NumCidItem	10
	//CString aOnItem[] = {"压缩机", "水泵", "氨阀", "氨泵", "风机", "分液", "回气", "延时", "吸气", "排气", "电磁阀"};
	//#define NumOnItem		11
	*pResult = 0;

	CListCtrl* pCid = (CListCtrl*) GetDlgItem(IDC_LIST_CID);
	int nCidIndex = pCid->GetNextItem(-1, LVNI_SELECTED);

	if(nCidIndex > -1)
	{
		//将ID设置和开关机流程设置分别填充
		CListBox* pCidItem = (CListBox*) GetDlgItem(IDC_LIST_CID_ITEM);
		CListBox* pPropItem = (CListBox*) GetDlgItem(IDC_LIST_ON_ITEM);

		CString strCurCid = pCid->GetItemText(nCidIndex, 0);
		m_byCidID = atoi(strCurCid);
		CString strCurCidDQ = pCid->GetItemText(nCidIndex, 1);
		m_byCidDQ = atoi(strCurCidDQ);
		CString strCurItem = pCid->GetItemText(nCidIndex, 2);
		CString strCurItemNum = pCid->GetItemText(nCidIndex, 3);
		m_byCidItemNum = atoi(strCurItemNum);
		//m_chOnNum = m_byCidItemNum;

		int nCurSelCidItem = 0;
		for(int i=0; i<NumCidItem; i++)
		{
			if(strcmp(strCurItem, aCidItem[i]) == 0)
			{
				nCurSelCidItem = i;
				break;
			}
		}
		int nCurSelPropItem = 0;
		for(int i=0; i<NumOnItem; i++)
		{
			if(strcmp(strCurItem, aOnItem[i]) == 0)
			{
				nCurSelPropItem = i;
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

		pPropItem->SetCurSel(nCurSelPropItem);
		pPropItem->GetText(nCurSelPropItem, strCurSel);
		if(strcmp(strCurSel, strCurItem) != 0)
		{
			pPropItem->SetCurSel(-1);
		}
		m_byItemNum = m_byCidItemNum;
		UpdateData(FALSE);
		OnLbnSelchangeListOnItem();
		
		//CString aOnItem[0-4] = {"压缩机", "水泵", "氨阀", "氨泵", "风机", 
		//5-10"分液", "回气", "延时", "吸气", "排气", "电磁阀"};
		if(strcmp(strCurItem, "主机") != 0)
		{
			switch(nCurSelPropItem)
			{
			case 0:	//压缩机
				{
					m_strKuYSJ = strCurItemNum;
					pSetDlgFGYSJ->m_byYSJ = m_byCidItemNum;
					break;
				}
			case 1:	//水泵
				{
					pSetDlgFGYSJ->m_bySB = m_byCidItemNum;
					pSetDlgDQ->m_strDQItem = strCurItem;
					pSetDlgDQ->m_byDQNum = m_byCidItemNum;
					break;
				}
			case 2:	//氨阀
				{
					pSetDlgFGYSJ->m_byAF = m_byCidItemNum;
					pSetDlgFMAF->m_byAFNum = m_byCidItemNum;				
					break;
				}
			case 3:	//氨泵
				{
					pSetDlgFGYSJ->m_byAB = m_byCidItemNum;
					pSetDlgDQ->m_strDQItem = strCurItem;
					pSetDlgDQ->m_byDQNum = m_byCidItemNum;
					break;
				}
			case 4:	//风机
				{
					m_strKuFJ = strCurItemNum;
					m_byItemNum=0;
					pSetDlgDQ->m_strDQItem = strCurItem;
					pSetDlgDQ->m_byDQNum = m_byCidItemNum;
					break;
				}
			case 5:	//分液
				{
					m_strKuFY = strCurItemNum;
					break;
				}
			case 6:	//回气
				{
					m_strKuHQ = strCurItemNum;
					break;
				}
			case 8:	//吸气
				{
					pSetDlgFGYSJ->m_byXQ = m_byCidItemNum;
					break;
				}
			case 9:	//排气
				{
					pSetDlgFGYSJ->m_byPQ = m_byCidItemNum;
					break;					
				}
			case 10://电磁阀
				{
					pSetDlgDQ->m_strDQItem = strCurItem;
					pSetDlgDQ->m_byDQNum = m_byCidItemNum;					
				}
			default: break;
			}		
		}

		pSetDlgFGYSJ->UpdateData(FALSE);
		pSetDlgFMAF->UpdateData(FALSE);
		pSetDlgDQ->UpdateData(FALSE);
		UpdateData(FALSE);
	}
}

