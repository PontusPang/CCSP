#pragma once


// CSetDlgDQ 对话框

class CSetDlgDQ : public CDialog
{
	DECLARE_DYNAMIC(CSetDlgDQ)

public:
	CSetDlgDQ(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetDlgDQ();

// 对话框数据
	enum { IDD = IDD_SET_DIALOG_DQ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	BYTE m_byDQNum;
public:
	void DQInit(void);
public:
	void DQLoad(void);
public:
	afx_msg void OnBnClickedButtonDqOk();
public:
	CString m_strDQItem;
public:
	afx_msg void OnBnClickedButtonDqDel();
public:
	afx_msg void OnNMDblclkListDq(NMHDR *pNMHDR, LRESULT *pResult);
public:
	virtual BOOL OnInitDialog();
public:
	void JZTempInit(void);
public:
	void JZTempLoad(void);
public:
	afx_msg void OnBnClickedButtonJzTempOk();
public:
	float m_fJZTempData;
public:
	afx_msg void OnBnClickedButtonJzTempDel();
public:
	afx_msg void OnNMDblclkListJzTemp(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void MyPhoneLoad(void);
public:
	afx_msg void OnBnClickedCheckMyPhone();
public:
	CString m_strMyPhone;
public:
	CString m_strUrPhone;
public:
	void PhoneInit(void);
public:
	void PhoneLoad(void);
public:
	afx_msg void OnBnClickedButtonUrPhoneOk();
public:
	afx_msg void OnBnClickedButtonUrPhoneDel();
public:
	afx_msg void OnNMDblclkListUrPhone(NMHDR *pNMHDR, LRESULT *pResult);
public:
	float m_fTempDnS;
public:
	afx_msg void OnBnClickedButtonOnlyFy();
public:
	afx_msg void OnBnClickedButtonOnlyFj();
public:
	afx_msg void OnBnClickedCheckSmsProp();
public:
	void PropMsgLoad(void);
public:
	afx_msg void OnBnClickedButtonSettempJz();
public:
	afx_msg void OnBnClickedButtonSettempDns();
public:
	afx_msg void OnBnClickedButtonSettempNum();
};
