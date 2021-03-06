// SetDlgFMAF.cpp : 实现文件
//

#include "stdafx.h"
#include "CS.h"
#include "SetDlgFMAF.h"
#include "AdvCanThread.h"
#include "ThreadProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CSetDlgFMAF 对话框

CSetDlgFMAF *pSetDlgFMAF;

extern CString aAFAuto[];
extern CString aAFFMHot[];

extern _ConnectionPtr m_pCon;	//ADO连接对象
extern _RecordsetPtr m_pRs;		//ADO记录集对象

extern HANDLE	g_hDevice_CAN1;
extern DWORD	g_dwCobId;
extern BYTE		g_SendData[8];
extern BYTE		g_RcvData[8];
extern BOOL		g_bOpened_CAN1;
extern BOOL		g_bExeTrue;			//执行成功标志
extern BOOL		g_bCxTrue;			//查询成功标志

extern COleDateTime g_mTimeNow;

IMPLEMENT_DYNAMIC(CSetDlgFMAF, CDialog)

CSetDlgFMAF::CSetDlgFMAF(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDlgFMAF::IDD, pParent)
	, m_byAFNum(0)
	, m_byYW(0)
	, m_byAFKD(0)
	, m_byAFTime(0)
	, m_byAFFD(0)
	, m_byAFSD(0)
{

}

CSetDlgFMAF::~CSetDlgFMAF()
{
}

void CSetDlgFMAF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AF_NUM, m_byAFNum);
	DDX_Text(pDX, IDC_EDIT_YW, m_byYW);
	DDX_Text(pDX, IDC_EDIT_AF_KD, m_byAFKD);
	DDX_Text(pDX, IDC_EDIT_AF_TIME, m_byAFTime);
	DDX_Text(pDX, IDC_EDIT_AF_FD, m_byAFFD);
	DDX_Text(pDX, IDC_EDIT_AF_SD, m_byAFSD);
}


BEGIN_MESSAGE_MAP(CSetDlgFMAF, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_AF_OK, &CSetDlgFMAF::OnBnClickedButtonAfOk)
	ON_BN_CLICKED(IDC_BUTTON_AF_DEL, &CSetDlgFMAF::OnBnClickedButtonAfDel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_AF, &CSetDlgFMAF::OnNMDblclkListAf)
	ON_BN_CLICKED(IDC_BUTTON_AF_OK2, &CSetDlgFMAF::OnBnClickedButtonAfOk2)
	ON_BN_CLICKED(IDC_BUTTON_AF_DEL2, &CSetDlgFMAF::OnBnClickedButtonAfDel2)
END_MESSAGE_MAP()

BOOL CSetDlgFMAF::OnInitDialog()
{
	CDialog::OnInitDialog();

	pSetDlgFMAF = this;
	// TODO:  在此添加额外的初始化
	AFInit();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
// CSetDlgFMAF 消息处理程序
void CSetDlgFMAF::AFInit(void)
{
	CListCtrl* pAFList = (CListCtrl*)GetDlgItem(IDC_LIST_AF);

	pAFList->SetExtendedStyle( pAFList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pAFList->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_CENTER, 45 );
	pAFList->InsertColumn( 1, LPCTSTR("自动"), LVCFMT_CENTER, 60 );
	pAFList->InsertColumn( 2, LPCTSTR("加热"), LVCFMT_CENTER, 60 );
	pAFList->InsertColumn( 3, LPCTSTR("液位"), LVCFMT_CENTER, 45 );
	pAFList->InsertColumn( 4, LPCTSTR("开度"), LVCFMT_CENTER, 45 );

	CButton* pAFAuto1 = (CButton*)GetDlgItem(IDC_RADIO_AF_AUTO1);
	pAFAuto1->SetCheck(1);

	CListCtrl* pAFList2 = (CListCtrl*)GetDlgItem(IDC_LIST_AF2);

	pAFList2->SetExtendedStyle( pAFList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pAFList2->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_CENTER, 45 );
	pAFList2->InsertColumn( 1, LPCTSTR("时间"), LVCFMT_CENTER, 70 );
	pAFList2->InsertColumn( 2, LPCTSTR("浮动"), LVCFMT_CENTER, 70 );
	pAFList2->InsertColumn( 3, LPCTSTR("速度"), LVCFMT_CENTER, 70 );

	AFLoad();
}

void CSetDlgFMAF::AFLoad(void)
{
	CListCtrl* pAFList = (CListCtrl*)GetDlgItem(IDC_LIST_AF);
	pAFList->DeleteAllItems();

	CListCtrl* pAFList2 = (CListCtrl*)GetDlgItem(IDC_LIST_AF2);
	pAFList2->DeleteAllItems();

	CString strSql = "";
	strSql = "SELECT * FROM SetAF ORDER BY AFNum ASC";
	m_pRs->Open((_variant_t)strSql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//RS-44.1

	int nListEntry = 0;
	while( !m_pRs->adoEOF )
	{	
		CString strTem = "";
		strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)1)->Value;
		pAFList->InsertItem( nListEntry, LPCTSTR( strTem ), 0 );
		pAFList2->InsertItem( nListEntry, LPCTSTR( strTem ), 0 );
		
		for (int i=2; i<m_pRs->GetFields()->Count; i++)
		{
			strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)i)->Value;
			
			if(i < 6)
			{
				pAFList->SetItemText( nListEntry, i-1, LPCTSTR( strTem ) );
			}
			else
			{
				pAFList2->SetItemText( nListEntry, i-5, LPCTSTR( strTem ) );
			}


		}

		nListEntry++;
		m_pRs->MoveNext();
	}

	m_pRs->raw_Close();	//RS-44.2
}

void CSetDlgFMAF::OnBnClickedButtonAfOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strSql = "";
	strSql.Format("SELECT * From SetAF WHERE AFNum = %d", m_byAFNum);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-45.1
	if(!m_pRs->BOF)	
	{
		CString strRe = "";
		strRe.Format("氨阀%d 已存在，将覆盖", m_byAFNum);
		MessageBox(strRe);
		strSql.Format("DELETE FROM SetAF WHERE AFNum = %d", m_byAFNum);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
	}
	m_pRs->raw_Close();	//RS-45.2

	if(m_byAFNum != 0)
	{
		CString strAFSetText = "";
		
		CString strAuto = "";
		for(int i=0; i<3; i++)
		{
			CButton* pAFAuto = (CButton*)GetDlgItem(IDC_RADIO_AF_AUTO1+i);
			if( pAFAuto->GetCheck() == 1 )
			{
				pAFAuto->GetWindowTextA(strAuto);
				break;
			}
		}

		if(strcmp(strAuto, "开自动") != 0)
		{
			MessageBox("氨阀不自动！联系售后？");
		}

		strSql.Format("INSERT INTO SetAF ([AFNum],[Auto],[Hot],[YW],[KD],[TIME],[FD],[SD]) \
					  VALUES (%d,'%s','%s',%d,%d,%d,%d,%d)",
			m_byAFNum, strAuto, "不加热", m_byYW, m_byAFKD, m_byAFTime, m_byAFFD, m_byAFSD);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		AFLoad();

		strAFSetText.Format("设置·氨阀%d %s 液位经验值%d 开度经验值%d 时间间隔%d 上下浮动%d 调节速度%d"
			,m_byAFNum, strAuto, m_byYW, m_byAFKD, m_byAFTime, m_byAFFD, m_byAFSD);
		Doing2Text(strAFSetText);
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_AF);
	int nCount = pList->GetItemCount();
	if(nCount > 0)
	{
		pList->EnsureVisible(nCount-1, 0);
	}
}

void CSetDlgFMAF::OnBnClickedButtonAfDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pAFList = (CListCtrl*)GetDlgItem(IDC_LIST_AF);
	int nAFIndex = pAFList->GetNextItem(-1, LVNI_SELECTED);

	if(nAFIndex != -1)
	{
		CString strAF = pAFList->GetItemText(nAFIndex, 0);
		int nCurAF = atoi(strAF);
		CString strSql = "";
		strSql.Format("DELETE FROM SetAF WHERE AFNum = %d", nCurAF);
		m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);

		AFLoad();

		Doing2Text("设置·删除氨阀");
	}
}

void CSetDlgFMAF::OnNMDblclkListAf(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CListCtrl* pAFList = (CListCtrl*) GetDlgItem(IDC_LIST_AF);
	int nAFIndex = pAFList->GetNextItem(-1, LVNI_SELECTED);

	if(nAFIndex > -1)
	{
		UpdateData(TRUE);
	
		CString strAFNum = pAFList->GetItemText(nAFIndex, 0);
		m_byAFNum = atoi(strAFNum);
		CString strYW = pAFList->GetItemText(nAFIndex, 3);
		m_byYW = atoi(strYW);
		CString strKD = pAFList->GetItemText(nAFIndex, 4);
		m_byAFKD = atoi(strKD);
		UpdateData(FALSE);
		
		CString strAuto = "";
		strAuto = pAFList->GetItemText(nAFIndex, 1);
		for(int i=0; i<3; i++)
		{
			CButton* pAFAuto = (CButton*)GetDlgItem(IDC_RADIO_AF_AUTO1+i);
			if(strcmp(strAuto, aAFAuto[i]) == 0)
			{
				pAFAuto->SetCheck(1);
			}
			else
			{
				pAFAuto->SetCheck(0);
			}
		}
	}

	CListCtrl* pAFList2 = (CListCtrl*) GetDlgItem(IDC_LIST_AF2);
	int nAFIndex2 = pAFList->GetNextItem(-1, LVNI_SELECTED);
	if(nAFIndex > -1)
	{
		UpdateData(TRUE);
		CString strTIME = pAFList2->GetItemText(nAFIndex, 1);
		m_byAFTime = atoi(strTIME);
		CString strFD = pAFList2->GetItemText(nAFIndex, 2);
		m_byAFFD = atoi(strFD);
		CString strSD = pAFList2->GetItemText(nAFIndex, 3);
		m_byAFSD = atoi(strSD);
		UpdateData(FALSE);
	}
}

void CSetDlgFMAF::OnBnClickedButtonAfOk2()
{
	UpdateData(TRUE);
	if(m_byAFNum == 0)
	{
		AfxMessageBox("请双击选择氨阀");
		return;		
	}
	
	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请开控制");
		return;
	}

	int nCid = 0;
	CString strSql = "";
	strSql.Format("SELECT * FROM SetCid WHERE IdItem = '氨阀' AND IdItemNum = %d", m_byAFNum);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-46.1
	if(!m_pRs->BOF)
	{
		CString strCid = (LPCSTR)_bstr_t(m_pRs->GetCollect("Cid"));
		nCid = atoi(strCid);
	}
	m_pRs->raw_Close();	//RS-46.2

	g_dwCobId = nCid;

	g_SendData[0] = m_byAFKD;
	g_SendData[1] = m_byYW;	//液位经验值
	g_SendData[2] = 0xA0;
	g_SendData[3] = 0x01;

	CAN_Send();

	GetDlgItem(IDC_BUTTON_AF_OK2)->EnableWindow(FALSE);

	//等待反馈成功
	int n=0;
	while(!g_bExeTrue && g_RcvData[2] != 0xA1)
	{
		Sleep(200);
		n++;
		//ExEvent();
		if(n >= 5*5)
		{
			CString str = "";
			str.Format("%02d:%02d  %d通信超时，报警！", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), g_dwCobId);
			AfxMessageBox(str);
			break;
		}
	}

	GetDlgItem(IDC_BUTTON_AF_OK2)->EnableWindow(TRUE);

	Doing2Text("设置·氨阀经验");
}

void CSetDlgFMAF::OnBnClickedButtonAfDel2()
{
	UpdateData(TRUE);
	if(m_byAFNum == 0)
	{
		AfxMessageBox("请双击选择氨阀");
		return;		
	}
	
	if(!g_bOpened_CAN1)
	{
		AfxMessageBox("请开控制");
		return;
	}

	int nCid = 0;
	CString strSql = "";
	strSql.Format("SELECT * FROM SetCid WHERE IdItem = '氨阀' AND IdItemNum = %d", m_byAFNum);
	m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);	//RS-47.1
	if(!m_pRs->BOF)
	{
		CString strCid = (LPCSTR)_bstr_t(m_pRs->GetCollect("Cid"));
		nCid = atoi(strCid);
	}
	m_pRs->raw_Close();	//RS-47.2

	g_dwCobId = nCid;

	g_SendData[0] = m_byAFTime;
	g_SendData[1] = m_byAFFD;	//液位经验值
	g_SendData[2] = m_byAFSD;
	g_SendData[3] = 0x01;

	CAN_Send();

	GetDlgItem(IDC_BUTTON_AF_DEL2)->EnableWindow(FALSE);

	//等待反馈成功
	int n=0;
	while(!g_bExeTrue && g_RcvData[2] != 0xA1)
	{
		Sleep(200);
		n++;
		//ExEvent();
		if(n >= 5*5)
		{
			CString str = "";
			str.Format("%02d:%02d  %d通信超时，报警！", g_mTimeNow.GetHour(), g_mTimeNow.GetMinute(), g_dwCobId);
			AfxMessageBox(str);
			break;
		}
	}

	GetDlgItem(IDC_BUTTON_AF_DEL2)->EnableWindow(TRUE);

	Doing2Text("设置·氨阀变化");
}