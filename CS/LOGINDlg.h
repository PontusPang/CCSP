#pragma once


// CLOGINDlg 对话框

class CLOGINDlg : public CDialog
{
	DECLARE_DYNAMIC(CLOGINDlg)

public:
	CLOGINDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLOGINDlg();

// 对话框数据
	enum { IDD = IDD_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUID;
	CString m_strPWD;
	BOOL Login(CLOGINDlg & dlg);
public:
	virtual BOOL OnInitDialog();
};
