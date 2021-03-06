// SetDlgDQ.cpp : 实现文件
//

#include "stdafx.h"
#include "CS.h"
#include "SetDlgDQ.h"
#include "SETDlg.h"
#include "SetDlgFGYSJ.h"
#include "Security.h"

CSetDlgDQ *pSetDlgDQ = NULL;

extern _ConnectionPtr m_pCon;	//ADO连接对象
extern _RecordsetPtr m_pRs;		//ADO记录集对象

extern BYTE g_byTempSum_SetKu;		//温度节点总数
extern BYTE g_byPropSMS_SetKu;

extern CSETDlg *pSetDlg;
extern CSetDlgFGYSJ *pSetDlgFGYSJ;
extern TempSor_t *g_pStTempSor;

extern CS_t *g_pStCS;
extern int g_nCS_Index;

// CSetDlgDQ 对话框

IMPLEMENT_DYNAMIC(CSetDlgDQ, CDialog)

CSetDlgDQ::CSetDlgDQ(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDlgDQ::IDD, pParent)
	, m_byDQNum(0)
	, m_strDQItem(_T(""))
	, m_fJZTempData(0)
	, m_strMyPhone(_T(""))
	, m_strUrPhone(_T(""))
	, m_fTempDnS(0)
{

}

CSetDlgDQ::~CSetDlgDQ()
{
}

void CSetDlgDQ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DQ_NUM, m_byDQNum);
	DDX_Text(pDX, IDC_EDIT_DQ_ITEM, m_strDQItem);
	DDX_Text(pDX, IDC_EDIT_JZ_DATA, m_fJZTempData);
	DDX_Text(pDX, IDC_EDIT_MY_PHONE, m_strMyPhone);
	DDX_Text(pDX, IDC_EDIT_UR_PHONE, m_strUrPhone);
	DDX_Text(pDX, IDC_EDIT_TEMP_DN_S, m_fTempDnS);
}


BEGIN_MESSAGE_MAP(CSetDlgDQ, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DQ_OK, &CSetDlgDQ::OnBnClickedButtonDqOk)
	ON_BN_CLICKED(IDC_BUTTON_DQ_DEL, &CSetDlgDQ::OnBnClickedButtonDqDel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DQ, &CSetDlgDQ::OnNMDblclkListDq)
	ON_BN_CLICKED(IDC_BUTTON_JZ_TEMP_OK, &CSetDlgDQ::OnBnClickedButtonJzTempOk)
	ON_BN_CLICKED(IDC_BUTTON_JZ_TEMP_DEL, &CSetDlgDQ::OnBnClickedButtonJzTempDel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_JZ_TEMP, &CSetDlgDQ::OnNMDblclkListJzTemp)
	ON_BN_CLICKED(IDC_CHECK_MY_PHONE, &CSetDlgDQ::OnBnClickedCheckMyPhone)
	ON_BN_CLICKED(IDC_BUTTON_UR_PHONE_OK, &CSetDlgDQ::OnBnClickedButtonUrPhoneOk)
	ON_BN_CLICKED(IDC_BUTTON_UR_PHONE_DEL, &CSetDlgDQ::OnBnClickedButtonUrPhoneDel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_UR_PHONE, &CSetDlgDQ::OnNMDblclkListUrPhone)
	ON_BN_CLICKED(IDC_BUTTON_ONLY_FY, &CSetDlgDQ::OnBnClickedButtonOnlyFy)
	ON_BN_CLICKED(IDC_BUTTON_ONLY_FJ, &CSetDlgDQ::OnBnClickedButtonOnlyFj)
	ON_BN_CLICKED(IDC_CHECK_SMS_PROP, &CSetDlgDQ::OnBnClickedCheckSmsProp)
	ON_BN_CLICKED(IDC_BUTTON_SETTEMP_JZ, &CSetDlgDQ::OnBnClickedButtonSettempJz)
	ON_BN_CLICKED(IDC_BUTTON_SETTEMP_DNS, &CSetDlgDQ::OnBnClickedButtonSettempDns)
	ON_BN_CLICKED(IDC_BUTTON_SETTEMP_NUM, &CSetDlgDQ::OnBnClickedButtonSettempNum)
END_MESSAGE_MAP()


// CSetDlgDQ 消息处理程序
BOOL CSetDlgDQ::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pSetDlgDQ = this;

	DQInit();
	JZTempInit();
	PhoneInit();

	if( !LOGIN_EN || (strcmp(g_pStCS[g_nCS_Index].strLoginName, "新平") == 0) )	//仅供测试
	{
		(CButton*)GetDlgItem(IDC_BUTTON_ONLY_FY)->EnableWindow(TRUE);
		(CButton*)GetDlgItem(IDC_BUTTON_ONLY_FJ)->EnableWindow(TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//
void CSetDlgDQ::DQInit(void)
{
	CListCtrl* pDQList = (CListCtrl*)GetDlgItem(IDC_LIST_DQ);

	pDQList->SetExtendedStyle( pDQList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pDQList->InsertColumn( 0, LPCTSTR("项目"), LVCFMT_CENTER, 55 );
	pDQList->InsertColumn( 1, LPCTSTR("序号"), LVCFMT_CENTER, 45 );
	pDQList->InsertColumn( 2, LPCTSTR("开关类型"), LVCFMT_CENTER, 80 );

	CButton* pDQNC1 = (CButton*)GetDlgItem(IDC_RADIO_DQ_NC1);
	pDQNC1->SetCheck(1);

	DQLoad();
}

void CSetDlgDQ::DQLoad(void)
{
	CListCtrl* pDQList = (CListCtrl*)GetDlgItem(IDC_LIST_DQ);
	pDQList->DeleteAllItems();
	CString strSql = "";
	strSql = "SELECT * FROM SetDQ ORDER BY DQItem, DQNum ASC";
	m_pRs->Open((_variant_t)strSql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//RS-28.1

	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{	
		CString strTem = "";
		strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)1)->Value;
		pDQList->InsertItem( nListEntry, LPCTSTR( strTem ), 0 );
		
		for (int i=2; i<m_pRs->GetFields()->Count; i++)
		{
			strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)i)->Value;
			pDQList->SetItemText( nListEntry, i-1, LPCTSTR( strTem ) );
		}

		nListEntry++;
		m_pRs->MoveNext();
	}

	m_pRs->raw_Close();	//RS-28.2
}

void CSetDlgDQ::OnBnClickedButtonDqOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strSql = "";
	strSql.Format("SELECT * From SetDQ WHERE DQItem = '%s' AND DQNum = %d", m_strDQItem, m_byDQNum);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-29.1
	if(!m_pRs->BOF)	
	{
		CString strReDQ = "";
		strReDQ.Format("%s%d已存在，将覆盖", m_strDQItem, m_byDQNum);
		MessageBox(strReDQ);
		strSql.Format("DELETE FROM SetDQ WHERE DQItem = '%s' AND DQNum = %d", m_strDQItem, m_byDQNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}
	m_pRs->raw_Close();	//RS-29.2

	if(m_byDQNum)
	{
		CString strDQNC = "";
		CButton* pDQNC = (CButton*)GetDlgItem(IDC_RADIO_DQ_NC1);
		( pDQNC->GetCheck() == 1 )? strDQNC = "单动开关": strDQNC = "自锁开关";

		strSql.Format("INSERT INTO SetDQ ([DQItem],[DQNum],[DQNC]) VALUES ('%s',%d,'%s')"
			,m_strDQItem, m_byDQNum, strDQNC);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		DQLoad();	
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_DQ);
	int nCount = pList->GetItemCount();
	if(nCount > 0)
	{
		pList->EnsureVisible(nCount-1, 0);
	}
}

void CSetDlgDQ::OnBnClickedButtonDqDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pDQList = (CListCtrl*)GetDlgItem(IDC_LIST_DQ);
	int nDQIndex = pDQList->GetNextItem(-1, LVNI_SELECTED);

	if(nDQIndex != -1)
	{
		CString strDQItem = pDQList->GetItemText(nDQIndex, 0);
		CString strDQNum = pDQList->GetItemText(nDQIndex, 1);
		int nDQNum = atoi(strDQNum);
		CString strSql = "";
		strSql.Format("DELETE FROM SetDQ WHERE DQItem = '%s' AND DQNum = %d", strDQItem, nDQNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		DQLoad();
	}
}

void CSetDlgDQ::OnNMDblclkListDq(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CListCtrl* pDQList = (CListCtrl*) GetDlgItem(IDC_LIST_DQ);
	int nDQIndex = pDQList->GetNextItem(-1, LVNI_SELECTED);

	if(nDQIndex > -1)
	{
		//UpdateData(TRUE);
		
		m_strDQItem = pDQList->GetItemText(nDQIndex, 0);
		CString strDQNum = pDQList->GetItemText(nDQIndex, 1);
		m_byDQNum = atoi(strDQNum);
		
		CString strDQNC = "";
		strDQNC = pDQList->GetItemText(nDQIndex, 2);
		CButton* pDQNC1 = (CButton*)GetDlgItem(IDC_RADIO_DQ_NC1);
		CButton* pDQNC2 = (CButton*)GetDlgItem(IDC_RADIO_DQ_NC2);
		if(strcmp(strDQNC, "单动开关") == 0)
		{
			pDQNC1->SetCheck(1);
			pDQNC2->SetCheck(0);
		}
		else
		{
			pDQNC1->SetCheck(0);
			pDQNC2->SetCheck(1);		
		}

		UpdateData(FALSE);
	}
}
//
void CSetDlgDQ::JZTempInit(void)
{
	CListCtrl* pJZTempList = (CListCtrl*)GetDlgItem(IDC_LIST_JZ_TEMP);

	pJZTempList->SetExtendedStyle( pJZTempList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pJZTempList->InsertColumn( 0, LPCTSTR("编号"), LVCFMT_CENTER, 50 );
	pJZTempList->InsertColumn( 1, LPCTSTR("校准值"), LVCFMT_CENTER, 80 );
	pJZTempList->InsertColumn( 2, LPCTSTR("下限报警"), LVCFMT_CENTER, 80 );

	CComboBox* pTempNum = (CComboBox*)GetDlgItem(IDC_COMBO_JZ_TEMP_NUM);
	pTempNum->ResetContent();
	for(int i=0; i<g_byTempSum_SetKu; i++)
	{
		CString strTempNum = "";
		strTempNum.Format("%d", i+1);
		pTempNum->AddString(strTempNum);
	}

	JZTempLoad();
}

void CSetDlgDQ::JZTempLoad(void)
{
	CListCtrl* pJZTempList = (CListCtrl*)GetDlgItem(IDC_LIST_JZ_TEMP);
	pJZTempList->DeleteAllItems();
	CString strSql = "";
	strSql = "SELECT * FROM SetJZTemp ORDER BY TempNum ASC";
	m_pRs->Open((_variant_t)strSql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//RS-30.1

	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{	
		CString strTem = "";
		strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)1)->Value;
		pJZTempList->InsertItem( nListEntry, LPCTSTR( strTem ), 0 );
		
		for (int i=2; i<m_pRs->GetFields()->Count; i++)
		{
			strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)i)->Value;
			pJZTempList->SetItemText( nListEntry, i-1, LPCTSTR( strTem ) );
		}

		nListEntry++;
		m_pRs->MoveNext();
	}

	m_pRs->raw_Close();	//RS-30.2
}

void CSetDlgDQ::OnBnClickedButtonJzTempOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strSql = "";

	CComboBox* pTempNum = (CComboBox*)GetDlgItem(IDC_COMBO_JZ_TEMP_NUM);
	int nTempNum = pTempNum->GetCurSel()+1;

	if(nTempNum)
	{
		CString strTempSetText = "";
		
		strSql.Format("SELECT * From SetJZTemp WHERE TempNum = %d", nTempNum);
		m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-31.1
		if(!m_pRs->BOF)	//已存在该温度编号
		{
			CString strReJZTemp = "";
			strReJZTemp.Format("温度编号%d已存在，将覆盖", nTempNum);
			MessageBox(strReJZTemp);
			strSql.Format("DELETE FROM SetJZTemp WHERE TempNum = %d", nTempNum);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		}
		m_pRs->raw_Close();	//RS-31.2

		strSql.Format("INSERT INTO SetJZTemp ([TempNum],[JZData],[TempDnS]) VALUES (%d,%.1f,%.1f)"
			, nTempNum, m_fJZTempData, m_fTempDnS);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		JZTempLoad();

		//150907-及时更新
		TempS_GetTempSorST();

		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_JZ_TEMP);
		int nCount = pList->GetItemCount();
		if(nCount > 0)
		{
			pList->EnsureVisible(nCount-1, 0);
		}

		strTempSetText.Format("设置·温度节点%d 校准值%.1f 下限报警值%.1f",  nTempNum, m_fJZTempData, m_fTempDnS);
		Doing2Text(strTempSetText);
	}
}

void CSetDlgDQ::OnBnClickedButtonJzTempDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pJZTempList = (CListCtrl*)GetDlgItem(IDC_LIST_JZ_TEMP);
	int nJZTempIndex = pJZTempList->GetNextItem(-1, LVNI_SELECTED);

	if(nJZTempIndex != -1)
	{
		CString strTempNum = pJZTempList->GetItemText(nJZTempIndex, 0);
		int nTempNum = atoi(strTempNum);
		CString strSql = "";
		strSql.Format("DELETE FROM SetJZTemp WHERE TempNum = %d", nTempNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		JZTempLoad();

		Doing2Text("设置·删除温度");
	}
}

void CSetDlgDQ::OnNMDblclkListJzTemp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CListCtrl* pJZTempList = (CListCtrl*)GetDlgItem(IDC_LIST_JZ_TEMP);
	int nJZTempIndex = pJZTempList->GetNextItem(-1, LVNI_SELECTED);

	if(nJZTempIndex != -1)
	{
		CString strTempNum = pJZTempList->GetItemText(nJZTempIndex, 0);
		CComboBox* pTempNum = (CComboBox*)GetDlgItem(IDC_COMBO_JZ_TEMP_NUM);
		pTempNum->SelectString(-1, strTempNum);

		CString strJZTempData = pJZTempList->GetItemText(nJZTempIndex, 1);
		m_fJZTempData = (float)atof(strJZTempData);

		CString strTempDnS = pJZTempList->GetItemText(nJZTempIndex, 2);
		m_fTempDnS = (float)atof(strTempDnS);

		UpdateData(FALSE);
	}
}
void CSetDlgDQ::OnBnClickedButtonSettempJz()
{
	CString str = "将统一修改 校准值";
	int nMSGB = AfxMessageBox(str, MB_YESNO);
	if(nMSGB == IDYES)
	{
		UpdateData(TRUE);

		CString strSql = "";
		strSql.Format("UPDATE SetJZTemp SET [JZData] = %.1f", m_fJZTempData);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		JZTempLoad();

		//150907-及时更新
		TempS_GetTempSorST();

		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_JZ_TEMP);
		int nCount = pList->GetItemCount();
		if(nCount > 0)
		{
			pList->EnsureVisible(nCount-1, 0);
		}

		CString strTempSetText = "统一修改校准值";
		Doing2Text(strTempSetText);
	}
}
void CSetDlgDQ::OnBnClickedButtonSettempDns()
{
	CString str = "将统一修改 下限报警";
	int nMSGB = AfxMessageBox(str, MB_YESNO);
	if(nMSGB == IDYES)
	{
		UpdateData(TRUE);

		CString strSql = "";
		strSql.Format("UPDATE SetJZTemp SET [TempDnS] = %.1f", m_fTempDnS);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		JZTempLoad();

		//150907-及时更新
		TempS_GetTempSorST();

		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_JZ_TEMP);
		int nCount = pList->GetItemCount();
		if(nCount > 0)
		{
			pList->EnsureVisible(nCount-1, 0);
		}

		CString strTempSetText = "统一修改下限报警";
		Doing2Text(strTempSetText);
	}
}
void CSetDlgDQ::OnBnClickedButtonSettempNum()
{
	CString str = "将统一修改 编号\n\n校准值 = 0\n\n下限报警 = -255";
	int nMSGB = AfxMessageBox(str, MB_YESNO);
	if(nMSGB == IDYES)
	{
		CString strSql = "DELETE FROM SetJZTemp";
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		for(int i=0; i<g_byTempSum_SetKu; i++)
		{
			strSql.Format("INSERT INTO SetJZTemp ([TempNum],[JZData],[TempDnS]) VALUES (%d,0.0,-255.0)", i+1);
			m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);			
		}

		JZTempLoad();

		//150907-及时更新
		TempS_GetTempSorST();

		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_JZ_TEMP);
		int nCount = pList->GetItemCount();
		if(nCount > 0)
		{
			pList->EnsureVisible(nCount-1, 0);
		}

		CString strTempSetText = "统一修改编号";
		Doing2Text(strTempSetText);
	}
}
//
void CSetDlgDQ::MyPhoneLoad(void)
{
	CString strSql = "";
	strSql = "SELECT * FROM SetPhone WHERE MyUr = '本机'";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-32.1
	if(!m_pRs->BOF)
	{
		m_strMyPhone = (LPCSTR)_bstr_t(m_pRs->GetCollect("Phone"));
		UpdateData(FALSE);
	}
	else
	{
		MessageBox("获取本机号码失败，请检查数据库设置");
	}
	m_pRs->raw_Close();	//RS-32.2

	CButton* pCheckMyPhone = (CButton*)GetDlgItem(IDC_CHECK_MY_PHONE);
	CEdit* pEditMyPhone = (CEdit*)GetDlgItem(IDC_EDIT_MY_PHONE);
	CButton* pPhoneOK = (CButton*)GetDlgItem(IDC_BUTTON_UR_PHONE_OK);

	(strcmp(m_strMyPhone, "") == 0)? pCheckMyPhone->SetCheck(0): pCheckMyPhone->SetCheck(1);
	
	BOOL bAlreadyMyPhone = pCheckMyPhone->GetCheck();
	pEditMyPhone->EnableWindow(!bAlreadyMyPhone);
	pPhoneOK->EnableWindow(bAlreadyMyPhone);
}

void CSetDlgDQ::OnBnClickedCheckMyPhone()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pCheckMyPhone = (CButton*)GetDlgItem(IDC_CHECK_MY_PHONE);
	CEdit* pEditMyPhone = (CEdit*)GetDlgItem(IDC_EDIT_MY_PHONE);
	CButton* pPhoneOK = (CButton*)GetDlgItem(IDC_BUTTON_UR_PHONE_OK);

	if( pCheckMyPhone->GetCheck() )
	{		
		UpdateData(TRUE);	
		CString strSql = "";
		strSql.Format("UPDATE SetPhone SET [Phone] = '%s' WHERE MyUr = '本机'", m_strMyPhone);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		MessageBox("本机短信号码已修改！");

		MyPhoneLoad();
	}
	else
	{
		pCheckMyPhone->EnableWindow(TRUE);
		pEditMyPhone->EnableWindow(TRUE);
		pPhoneOK->EnableWindow(FALSE);
	}
}

void CSetDlgDQ::PhoneInit(void)
{
	CListCtrl* pUrPhoneList = (CListCtrl*)GetDlgItem(IDC_LIST_UR_PHONE);

	pUrPhoneList->SetExtendedStyle( pUrPhoneList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pUrPhoneList->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_LEFT, 45 );
	pUrPhoneList->InsertColumn( 1, LPCTSTR("号码"), LVCFMT_CENTER, 100 );

	MyPhoneLoad();
	PhoneLoad();
	PropMsgLoad();
}

void CSetDlgDQ::PhoneLoad(void)
{
	CListCtrl* pUrPhoneList = (CListCtrl*)GetDlgItem(IDC_LIST_UR_PHONE);
	pUrPhoneList->DeleteAllItems();
	CString strSql = "";
	strSql = "SELECT * FROM SetPhone WHERE MyUr = '目标' ORDER BY ID ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-33.1
	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{
		CString strListEntry = "";
		strListEntry.Format("%d", nListEntry+1);
		pUrPhoneList->InsertItem( nListEntry, LPCTSTR( strListEntry ), 0 );

		CString strUrPhone = (LPCSTR)_bstr_t(m_pRs->GetCollect("Phone"));
		pUrPhoneList->SetItemText( nListEntry, 1, LPCTSTR( strUrPhone ) );
		
		nListEntry ++;
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();	//RS-33.2
}

void CSetDlgDQ::OnBnClickedButtonUrPhoneOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strSql = "";
	strSql.Format("SELECT * From SetPhone WHERE Phone = '%s'", m_strUrPhone);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-34.1

	if(!m_pRs->BOF)	//已存在该电话号码
	{
		MessageBox("该电话号码已存在");
	}
	else
	{
		strSql.Format("INSERT INTO SetPhone values('目标', '%s')", m_strUrPhone);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}
	m_pRs->raw_Close();	//RS-34.2

	PhoneLoad();
}

void CSetDlgDQ::OnBnClickedButtonUrPhoneDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pUrPhoneList = (CListCtrl*)GetDlgItem(IDC_LIST_UR_PHONE);
	int nCidIndex = pUrPhoneList->GetNextItem(-1, LVNI_SELECTED);

	if(nCidIndex > 0)
	{
		CString strCurPhone = pUrPhoneList->GetItemText(nCidIndex, 1);
		CString strSql = "";
		strSql.Format("DELETE FROM SetPhone WHERE Phone = '%s'", strCurPhone);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);		

		PhoneLoad();
	}
}

void CSetDlgDQ::OnNMDblclkListUrPhone(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CListCtrl* pUrPhoneList = (CListCtrl*)GetDlgItem(IDC_LIST_UR_PHONE);
	int nUrPhoneIndex = pUrPhoneList->GetNextItem(-1, LVNI_SELECTED);

	if(nUrPhoneIndex > 0)
	{
		m_strUrPhone = pUrPhoneList->GetItemText(nUrPhoneIndex, 1);
		UpdateData(FALSE);
	}
}

void CSetDlgDQ::OnBnClickedCheckSmsProp()
{
	//150827
	CButton* pPropSMS = (CButton*)GetDlgItem(IDC_CHECK_SMS_PROP);
	CString strSql = "";
	if(pPropSMS->GetCheck())
	{
		strSql = "UPDATE SetKu SET [RuKuING] = 1 WHERE KuNum = 0";
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		g_byPropSMS_SetKu = -1;

		MessageBox("发送 开关机短信");
		Doing2Text("设置·发送流程短信");
	}
	else
	{
		strSql = "UPDATE SetKu SET [RuKuING] = 0 WHERE KuNum = 0";
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		g_byPropSMS_SetKu = 0;

		MessageBox("不发送 开关机短信");
		Doing2Text("设置·取消流程短信");
	}
	PropMsgLoad();
}
void CSetDlgDQ::PropMsgLoad(void)
{
	CString strSql = "SELECT * FROM SetKu WHERE KuNum = 0";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//151021-1-1
	if(!m_pRs->BOF)
	{
		CString strPropMsg = (LPCSTR)_bstr_t(m_pRs->GetCollect("RuKuING"));
		(strcmp(strPropMsg, "0") == 0)?	g_byPropSMS_SetKu = 0:	g_byPropSMS_SetKu = -1;
	}
	m_pRs->raw_Close();	//151021-1-2
	CButton* pPropSMS = (CButton*)GetDlgItem(IDC_CHECK_SMS_PROP);
	pPropSMS->SetCheck((BOOL)g_byPropSMS_SetKu);
}
/////////////////////////////////////////////////////////////////////
void CSetDlgDQ::OnBnClickedButtonOnlyFy()
{
	//新平2-10  仅使用排管，不操作风机
	//① 将风机在库房分组屏蔽
	//② 单独开关机只风机 - 否
	//③ 双分液，回气
	CString strSql = "";
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 ORDER BY KuNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-35.1
	while(!m_pRs->adoEOF)
	{
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
		int nKuNum = atoi(strKuNum);

		CString strDualFY = "";
		strDualFY.Format("%d,%d", nKuNum*2-1, nKuNum*2);
		strSql.Format("UPDATE SetKu SET [KuFY] = '%s', [KuHQ] = '%s' \
					  WHERE KuNum > 0 AND KuNum = %d", strDualFY, strKuNum, nKuNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
		
		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();		//RS-35.2

	strSql = "UPDATE SetKu SET [KuFJ] = '0', [KuOnlyOffFJ] = 0 WHERE KuNum > 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	pSetDlg->KuLoad();
	//③ 将氨阀、氨泵切换为排管
	strSql = "UPDATE SetYSJ SET [AF] = 1, [AB] = 1";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	pSetDlgFGYSJ->YSJLoad();

	Doing2Text("设置·排管控制");
}

void CSetDlgDQ::OnBnClickedButtonOnlyFj()
{
	//新平2-10  仅使用排管，不操作风机
	CString strSql = "";
	//① 将风机改为库房序号
	strSql = "SELECT * FROM SetKu WHERE KuNum > 0 ORDER BY KuNum ASC";
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-36.1
	while(!m_pRs->adoEOF)
	{
		CString strKuNum = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuNum"));
		int nKuNum = atoi(strKuNum);

		strSql.Format("UPDATE SetKu SET [KuFJ] = '%s' WHERE KuNum > 0 AND KuNum = %d", strKuNum, nKuNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		m_pRs->MoveNext();
	}
	m_pRs->raw_Close();		//RS-36.2
	//② 单独开关机只风机 - 是
	strSql = "UPDATE SetKu SET [KuOnlyOffFJ] = 1 WHERE KuNum > 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	//④ 若开关机流程中完全不操作分液回气，则屏蔽,操作下列语句 -- 若需要动作，则将下列语句屏蔽即可
	strSql = "UPDATE SetKu SET [KuFY] = '0', [KuHQ] = '0' WHERE KuNum > 0";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

	pSetDlg->KuLoad();
	//③ 将氨阀、氨泵切换为排管
	strSql = "UPDATE SetYSJ SET [AF] = 2, [AB] = 2";
	m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	pSetDlgFGYSJ->YSJLoad();

	Doing2Text("设置·风机控制");
}

