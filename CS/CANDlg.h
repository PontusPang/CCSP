#pragma once
#include "afxwin.h"
#include "SerialPort.h"
#include "CWMPPlayer4.h"
#include "CWMPControls.h"
#include "CWMPSettings.h"
#include "afxcmn.h"

// CCANDlg 对话框

class CCANDlg : public CDialog
{
	DECLARE_DYNAMIC(CCANDlg)

public:
	CCANDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCANDlg();

// 对话框数据
	enum { IDD = IDD_CAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton m_cbOpenClose;
	CButton m_cbStartSend;
	CButton m_cbStartRcv;

	CSerialPort m_SerialPort_TEMP;	
	BOOL m_bSerialPortOpened_TEMP;
	CSerialPort m_SerialPort_MSG;	
	BOOL m_bSerialPortOpened_MSG;
	CSerialPort m_SerialPort_ANS;	
	BOOL m_bSerialPortOpened_ANS;

public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnBnClickedButtonStartOn();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg LONG OnComm(WPARAM ch, LPARAM port);
	CString m_strComRcvData;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();
public:
	afx_msg void OnBnClickedButtonTempTest();
public:
	afx_msg void OnBnClickedButtonOpen2();
public:
	afx_msg void OnBnClickedButtonClose2();
public:
	afx_msg void OnBnClickedButtonSend2();
public:
	CString m_strProp;
public:
	CEdit m_ctrlProp;
	CFont m_editFont;
public:
	afx_msg void OnBnClickedButtonStopOn();
public:
	afx_msg void OnBnClickedButtonStartTempCtrl();
public:
	float TempNum(int Num, CEdit *pEdit);
public:
	afx_msg void OnBnClickedButtonStopTempCtrl();
public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnBnClickedButtonRstKuProp();
public:
	afx_msg void OnBnClickedButtonUrgentStop();
public:
	void TempDisp(int nTempCount, float fTemp);
public:
	void CidCtrlInit(void);
public:
	void CidCtrlLoad(void);
public:
	afx_msg void OnNMDblclkListCidCtrl(NMHDR *pNMHDR, LRESULT *pResult);
public:
	BYTE m_byCidCtrlID;
public:
	BYTE m_byCidCtrlItemNum;
public:
	afx_msg void OnBnClickedButtonReferAllCtrl();
public:
	afx_msg void OnBnClickedButtonStopRefCtrl();
public:
	afx_msg void OnBnClickedButtonTestOpenCtrl();
public:
	afx_msg void OnBnClickedButtonTestCloseCtrl();
public:
	afx_msg void OnBnClickedButtonTestCxCtrl();
public:
	afx_msg void OnBnClickedButtonStartOnekey();
public:
	afx_msg void OnBnClickedButtonStopOnekey();
public:
	afx_msg void OnNMDblclkListTemp(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void TempListInit(void);
	void TempListLoad(void);
public:
	afx_msg void OnBnClickedButtonTestToCtrl();
public:
	BYTE m_byTestTO;
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	void KuCtrlDispInit(void);
public:
	void KuCtrlDispLoad(void);
public:
	void YSJCtrlDispInit(void);
public:
	void YSJCtrlDispLoad(void);
public:
	afx_msg void OnBnClickedButtonRukuClear();
public:
	void UpdateRuKuComobox(void);
public:
	afx_msg void OnBnClickedButtonCtrlOpen();
public:
	afx_msg void OnBnClickedButtonCtrlClose();
public:
	afx_msg void OnBnClickedCheckCanSimulate();
public:
	afx_msg void OnBnClickedCheckTempSimulate();
public:
	BYTE m_byTempSimZH;
public:
	BYTE m_byTempSimX;
public:
	afx_msg void OnBnClickedButtonTempSimOk();
public:
	afx_msg void OnBnClickedButtonStartOff();
public:
	afx_msg void OnBnClickedButtonResumeTempCtrl();
public:
	afx_msg void OnBnClickedButtonSuspendTempCtrl();
public:
	afx_msg void OnPaint();
public:
	BYTE m_byCidCtrlDQ;
public:
	afx_msg void OnNMRdblclkListTemp(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButtonKuOnManual();
public:
	void UpdateKuManual(void);
public:
	afx_msg void OnBnClickedButtonKuOffManual();
public:
	afx_msg void OnBnClickedButtonCxText();
public:
	void ST_TempRefer(void);
public:
	void SP_TempRefer(void);
public:
	afx_msg void OnBnClickedCheckTempUpCtrl();
public:
	afx_msg void OnBnClickedCheckTempDnCtrl();
public:
	afx_msg void OnBnClickedButtonTempUpDnCtrl();
public:
	float m_fTempUpCtrl;
public:
	float m_fTempDnCtrl;
public:
	CWMPPlayer4 m_player;
	CWMPControls m_playerCtrl;
	CWMPSettings m_playerSet; 
public:
	afx_msg void OnBnClickedButtonWmpSt();
public:
	afx_msg void OnBnClickedButtonWmpStop();
public:
	afx_msg void OnBnClickedButtonWmpSp();
public:
	afx_msg void OnBnClickedButtonWmpUr();
public:
	afx_msg void OnBnClickedButtonOpenAns();
public:
	afx_msg void OnBnClickedButtonCloseAns();
public:
	void AnsListInit(void);
public:
	CListCtrl m_AnsList;
public:
	CEdit m_cComRcv;
public:
	afx_msg void OnBnClickedCheckOnOffCycle();
public:
	void PropListInit(void);
public:
	void PropListLoad(CString strProp, CString strFunc);
	afx_msg void OnBnClickedCheckYsjManual();
public:
	afx_msg void OnBnClickedButtonYsjOffManual();
public:
	afx_msg void OnBnClickedButtonManualYsjStop();
public:
	afx_msg void OnBnClickedButtonYsjOnManual();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnBnClickedButtonQuit();
};
void SP_CidRefer();
void ST_CidRefer();