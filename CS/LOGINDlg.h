#pragma once


// CLOGINDlg �Ի���

class CLOGINDlg : public CDialog
{
	DECLARE_DYNAMIC(CLOGINDlg)

public:
	CLOGINDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLOGINDlg();

// �Ի�������
	enum { IDD = IDD_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUID;
	CString m_strPWD;
	BOOL Login(CLOGINDlg & dlg);
public:
	virtual BOOL OnInitDialog();
};
