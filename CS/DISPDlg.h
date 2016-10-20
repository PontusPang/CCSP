// CSDlg.h : header file
//

#if !defined(AFX_CSDLG_H__4B2A8EC3_C569_480F_AF37_5BCFB173F7D8__INCLUDED_)
#define AFX_CSDLG_H__4B2A8EC3_C569_480F_AF37_5BCFB173F7D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDISPDlg dialog

class CDISPDlg : public CDialog
{
// Construction
public:
	CDISPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDISPDlg)
	enum { IDD = IDD_DISP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDISPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDISPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void DoingTextInit(void);
public:
	void DoingTextLoad(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSDLG_H__4B2A8EC3_C569_480F_AF37_5BCFB173F7D8__INCLUDED_)
