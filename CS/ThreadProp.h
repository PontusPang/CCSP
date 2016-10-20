#ifndef _ThreadProc_H
#define _ThreadProc_H

void YSJCmd(int nNum, int nDWKD, int nNC, CString strDir, CEdit *pEdit);
void DQCmd(CString strItem, int nNum, int nDQ, int nNC, CEdit *pEdit);
void FMCmd(CString strItem, int nNum, int nDWKD, CEdit *pEdit);
void AFCmd(CString strFunc, int nNum, int nDWKD, CEdit *pEdit);
void TimerCmd(CString strFunc, int nTimeNO, int nTimeGoal, CEdit *pEdit);

void ItemDisp(CString strPropItem, int nItemNum, int nDWKD, int nNC, int nAFYW, CListCtrl* pList);
void ItemReferCtrlListDisp(BYTE *aRcv, CListCtrl* pList, CString strItem, int nItemNum, int nCidDQ);
void KuWorkStatusListDisp(int nKuNum, BOOL bWorkStatus, BOOL bAllKu);

UINT ThreadProc_KuTempChange(LPVOID wParam);

int GetKuTemp(CEdit *pEdit, int nKuIndex);
BOOL GetKuStatus(BOOL bBorA, int nKuExeNum, CEdit *pEdit);

int GetKuCtrlSt();
int GetKuIndex_St(BYTE byKuNum, BOOL bUrgentStop);
void PropExecute(_RecordsetPtr &m_pRsProp, CString strFunc, CEdit *pEdit);

UINT ThreadProc_Prop_AllOnAllOff(LPVOID wParam);
UINT ThreadProc_Prop_OneOnOneOff(LPVOID wParam);
UINT ThreadProc_Timer2(LPVOID wParam);
UINT ThreadProc_KuCtrl(LPVOID wParam);

void AddUrgentStopProp(CString strItem, int nItemNum);
void SelectPropUrgent();
void UrgentStopExecute(CEdit *pEdit);
void OneKeyStopExecute(CEdit *pEdit);
void Auto_UrgentStop(CString str);

UINT ThreadProc_CidRefer(LPVOID wParam);	//CANDlg

UINT ThreadProc_Prop_Delay3(LPVOID wParam);
BOOL SP_Delay3(CEdit *pEdit);
void ST_Delay3(CEdit *pEdit);
void SP_KuCtrl();
void ST_KuCtrl();
void ST_AllOnAllOff(Prop_t &g_StProp);
void ST_OneOnOneOff(Prop_t &g_StProp);
void Recovery(CEdit *pEdit);
void SP_PropING();
void ST_PropING();

#endif