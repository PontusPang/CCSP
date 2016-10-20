#pragma once


// CSetDlgFGYSJ 对话框

class CSetDlgFGYSJ : public CDialog
{
	DECLARE_DYNAMIC(CSetDlgFGYSJ)

public:
	CSetDlgFGYSJ(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetDlgFGYSJ();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_SET_DIALOG_FGYSJ };

	BYTE m_byStartH;
	BYTE m_byStartM;
	BYTE m_byStopH;
	BYTE m_byStopM;
	CString m_strTimeKuNum;
	afx_msg void OnBnClickedButtonFgydAdd();
	afx_msg void OnBnClickedButtonFgydDel();

	void FGTimeInit(void);
	void FGTimeLoad(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void YSJInit(void);
public:
	void YSJLoad(void);
public:
	afx_msg void OnBnClickedButtonYsjOk();
public:
	BYTE m_byYSJ;
	BYTE m_byDW;
	BYTE m_byXQ;
	BYTE m_byPQ;
	BYTE m_byAF;
	BYTE m_byAB;
	BYTE m_bySB;
public:
	afx_msg void OnBnClickedButtonYsjDel();
public:
	afx_msg void OnBnClickedButtonFgydEdit();
public:
	afx_msg void OnBnClickedCheckAllKu();
public:
	afx_msg void OnNMDblclkListFg(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMDblclkListYsj(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnBnClickedButtonSetysjAf();
public:
	afx_msg void OnBnClickedButtonSetysjAb();
public:
	afx_msg void OnBnClickedCheckYsjPriority();
public:
	void PriYSJLoad(void);
public:
	void KuYSJInit(void);
public:
	BYTE m_byKuYSJKuNum;
public:
	BYTE m_byKuYSJYSJNum;
public:
	afx_msg void OnBnClickedButtonKuYsjYes();
public:
	void KuYSJLoad(void);
public:
	afx_msg void OnBnClickedButtonKuYsjNo();
public:
	afx_msg void OnNMDblclkListKuYsj(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedCheckKuYsj();
};

BOOL EnKuYSJ_SetKu(void);
BOOL EnKuYSJ();
int GetYSJSt();
int GetYSJIndex_St(BYTE byYSJNum);
void YSJOnOffSel(CString strFunc, int nYSJIndex, BOOL bResetPropYSJ);
void YSJOnOffExe(CString strFunc, CEdit *pEdit, int nYSJIndex);
UINT ThreadProc_Prop_YSJ(LPVOID wParam);
int GetKuYSJSt();
void YSJDelOnProp(CString strFunc);
void YSJDelOffProp(CString strFunc);
void KuYSJ(CEdit *pEdit);
void KuLinkYSJ(CString strFunc, CEdit *pEdit, int nKuIndex);
