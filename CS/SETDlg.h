#include "afxwin.h"
#include "afxcmn.h"

#if !defined(AFX_SETDLG_H__77432B51_D481_4D38_B8E8_0CC7C3BDB419__INCLUDED_)
#define AFX_SETDLG_H__77432B51_D481_4D38_B8E8_0CC7C3BDB419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tabsheet.h"
#include "SetDlgFGYSJ.h"
#include "SetDlgFMAF.h"
#include "SetDlgDQ.h"
// SETDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSETDlg dialog

class CSETDlg : public CDialog
{
// Construction
public:
	CSETDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSETDlg)
	enum { IDD = IDD_SET_DIALOG };
	//}}AFX_DATA

	CSetDlgFGYSJ dlgSetFGYSJ;
	CSetDlgFMAF dlgSetFMAF;
	CSetDlgDQ dlgSetDQ;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSETDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSETDlg)
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG


public:
	//流程
	BYTE m_byItemNum;
	int m_nDWKD;
	BYTE m_byZuYSJNum;
	BYTE m_byZuKuNum;
	BYTE m_byZuGXNum;
	void PropInit();
	void SetPropLoad(CString strFunc, CListCtrl *pList);
	void PropLoad();
	void SetItem(int byOnOff);
	afx_msg void OnLbnSelchangeListOnItem();
	void SetPropAdd(CString strFunc);
	afx_msg void OnBnClickedButtonOnAdd();
	void SetPropDel(CString strFunc, CListCtrl *pList);
	afx_msg void OnBnClickedButtonOnDel();
	void CSETDlg::SetPropIns(CString strFunc, CListCtrl *pList);
	afx_msg void OnBnClickedButtonOnIns();
	afx_msg void OnNMDblclkListCtrlOnProp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListCtrlOffProp(NMHDR *pNMHDR, LRESULT *pResult);

	//序号显示
	void CidInit();
	void CidLoad();
	afx_msg void OnBnClickedButtonIdAdd();
	BYTE m_byCidItemNum;
	BYTE m_byCidID;
	afx_msg void OnBnClickedButtonIdDel();
	afx_msg void OnNMDblclkListCid(NMHDR *pNMHDR, LRESULT *pResult);

	//库房设置
	void KuInit(void);
	void KuLoad(void);
	BYTE m_byKuSum;
	BYTE m_byTempSum;
	void SumLoad(void);
	afx_msg void OnBnClickedCheckKuSum();
	afx_msg void OnBnClickedButtonKuOk();
	CString m_strKuStore;
	float m_fTempUp;
	float m_fTempDn;
	CString m_strTempNum;
	afx_msg void OnBnClickedButtonKuDel();
	CString m_strKuFY;
	CString m_strKuHQ;
	CString m_strKuFJ;
	CString m_strKuYSJ;
	CTabSheet m_TabSheet;
	afx_msg void OnBnClickedButtonKuEdit();
	afx_msg void OnBnClickedRadioKuCtrl1();
	afx_msg void OnBnClickedRadioKuCtrl3();
	afx_msg void OnBnClickedRadioKuCtrl2();
	afx_msg void OnNMDblclkListKu(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
public:
	BYTE m_byKuAllZu;
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
public:
	BYTE m_byKuOnMax;
public:
	afx_msg void OnBnClickedCheckKuOnMax();
public:
	BYTE m_byCidDQ;
public:
	float m_fTempDn2;
public:
	float m_fTempUp2;
public:
	BYTE m_byKuTempChangeDays;
public:
	afx_msg void OnBnClickedCheckKuTempChange();
public:
	BYTE m_byAnsSum;
public:
	afx_msg void OnBnClickedCheckAnsSum();
public:
	afx_msg void OnBnClickedCheckKuTimeS();
public:
	int m_nTimeS;
public:
	afx_msg void OnBnClickedButtonSetkuYsj();
public:
	afx_msg void OnBnClickedButtonSetkuFy();
public:
	afx_msg void OnBnClickedButtonSetkuHq();
public:
	afx_msg void OnBnClickedButtonSetkuFj();
public:
	afx_msg void OnBnClickedCheckOnlyOffFj();
public:
	afx_msg void OnBnClickedRadioKu2();
public:
	afx_msg void OnBnClickedCheckKuSetTempChangeHour();
public:
	BYTE m_byKuTempChangeHour;
public:
	afx_msg void OnBnClickedButtonSetkuMode();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETDLG_H__77432B51_D481_4D38_B8E8_0CC7C3BDB419__INCLUDED_)
