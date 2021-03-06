// CSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CS.h"
#include "DISPDlg.h"
#include "LOGINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern _ConnectionPtr m_pCon;	//ADO连接对象
extern _RecordsetPtr m_pRs;		//ADO记录集对象

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAfOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDISPDlg dialog

CDISPDlg::CDISPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDISPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDISPDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDISPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDISPDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDISPDlg, CDialog)
	//{{AFX_MSG_MAP(CDISPDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDISPDlg message handlers

BOOL CDISPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	DoingTextInit();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDISPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDISPDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDISPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
//
void CDISPDlg::DoingTextInit(void)
{
	CListCtrl* pTextList = (CListCtrl*)GetDlgItem(IDC_LIST_DISP_TEXT);

	pTextList->SetExtendedStyle( pTextList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	//选中一行,网格线

	pTextList->InsertColumn( 0, LPCTSTR("序号"), LVCFMT_CENTER, 50 );
	pTextList->InsertColumn( 1, LPCTSTR("时间"), LVCFMT_CENTER, 200 );
	pTextList->InsertColumn( 2, LPCTSTR("记录"), LVCFMT_LEFT, 650 );

	DoingTextLoad();
}

void CDISPDlg::DoingTextLoad(void)
{
	CListCtrl* pTextList = (CListCtrl*)GetDlgItem(IDC_LIST_DISP_TEXT);
	pTextList->DeleteAllItems();

	CString strSql = "";
	strSql = "SELECT * FROM Text ORDER BY Time ASC";
	m_pRs->Open((_variant_t)strSql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);	//RS-7.1

	int nListEntry = 0;
	while( ! m_pRs->adoEOF )
	{
		CString strTem = "";
		strTem.Format("%d", nListEntry+1);
		pTextList->InsertItem( nListEntry, LPCTSTR( strTem ), 0 );
		
		for (int i=0; i<m_pRs->GetFields()->Count; i++)
		{
			strTem = (TCHAR *)(_bstr_t)m_pRs->GetFields()->GetItem((long)i)->Value;
			pTextList->SetItemText( nListEntry, i+1, LPCTSTR( strTem ) );
		}
		
		nListEntry ++;
		m_pRs->MoveNext();
	}	

	m_pRs->raw_Close();	//RS-7.2

	int nCount = pTextList->GetItemCount();
	if(nCount > 0)
	{
		pTextList->EnsureVisible(nCount-2, 0);
	}

	//strSql.Format("EXEC master..xp_cmdshell 'bcp %s out E:\Text2.xls -c -q -S%s -U%s -P%s'", \
	//	"CS_QXJC_141014.dbo.Text", "127.0.0.1", "sa", "120614");
	//strSql = "EXEC master..xp_cmdshell 'bcp CS_QXJC_141014.dbo.Text yout E:\Text2.xls -c -q -S127.0.0.1 -Usa -P120614'";
	//m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
}
