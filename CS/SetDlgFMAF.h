#pragma once


// CSetDlgFMAF �Ի���

class CSetDlgFMAF : public CDialog
{
	DECLARE_DYNAMIC(CSetDlgFMAF)

public:
	CSetDlgFMAF(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetDlgFMAF();

// �Ի�������
	enum { IDD = IDD_SET_DIALOG_FMAF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
public:
	BYTE m_byAFNum;
public:
	void AFInit(void);
public:
	virtual BOOL OnInitDialog();
public:
	void AFLoad(void);
public:
	afx_msg void OnBnClickedButtonAfOk();
public:
	afx_msg void OnBnClickedButtonAfDel();
public:
	afx_msg void OnNMDblclkListAf(NMHDR *pNMHDR, LRESULT *pResult);
public:
	BYTE m_byYW;
public:
	BYTE m_byAFKD;
public:
	BYTE m_byAFTime;
public:
	BYTE m_byAFFD;
public:
	BYTE m_byAFSD;
public:
	afx_msg void OnBnClickedButtonAfOk2();
public:
	afx_msg void OnBnClickedButtonAfDel2();
};
