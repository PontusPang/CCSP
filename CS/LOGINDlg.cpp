// LOGINDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CS.h"
#include "LOGINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CS_t *g_pStCS;
extern int g_nCS_Index;

// CLOGINDlg 对话框

IMPLEMENT_DYNAMIC(CLOGINDlg, CDialog)

CLOGINDlg::CLOGINDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLOGINDlg::IDD, pParent)
	, m_strUID(_T(g_pStCS[g_nCS_Index].strLoginName))
	, m_strPWD(_T(""))
{

}

CLOGINDlg::~CLOGINDlg()
{
}

void CLOGINDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_UID, m_strUID);
	DDX_Text(pDX, IDC_EDIT_PWD, m_strPWD);
}


BEGIN_MESSAGE_MAP(CLOGINDlg, CDialog)
END_MESSAGE_MAP()

// CLOGINDlg 消息处理程序
BOOL CLOGINDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	GetDlgItem(IDC_EDIT_PWD)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
BOOL CLOGINDlg::Login(CLOGINDlg & dlg)
{
	int nResponse = dlg.DoModal();
	while( (dlg.m_strUID != g_pStCS[g_nCS_Index].strLoginName) || (dlg.m_strPWD != g_pStCS[g_nCS_Index].strLoginPwd) )
	{
		if(nResponse == IDCANCEL)
		{
			::PostQuitMessage(0);	
			return FALSE;
		}

		dlg.m_strUID = g_pStCS[g_nCS_Index].strLoginName;
		dlg.m_strPWD = "";
		nResponse = dlg.DoModal();
	}

	return TRUE;
}
