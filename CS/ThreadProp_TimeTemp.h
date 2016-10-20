#ifndef _ThreadProp_TimeTemp_H
#define _ThreadProp_TimeTemp_H

void TimeOnTempOff_TempOn_KuOnMax(CEdit *pEdit, int nKuIndex, CString str, COleDateTime TimeStop, CString strKuNum, int nKuExeSum, BYTE *aKuNum);
void GetKuTemp_OffKu(CEdit *pEdit);
void TimeOnTempOff_TempCtrl_AllOff(CEdit *pEdit, int nKuIndex, CString strKuNum, CString str);
void TimeOnTempOff_TempCtrl_AllOn(CEdit *pEdit, int nKuIndex, CString strKuNum, CString str);
void TimeOnTempOff_TempCtrl_TimeOut(CEdit *pEdit, CString strKuNum, CString strTimeNow, CString strTimeStop);
void TimeOnTempOff_TempCtrl_TimeIn(CEdit *pEdit, CString strKuNum, CString strTimeNow, CString strTimeStop);
void TimeOnTempOff_TempCtrl(CEdit *pEdit, CString strWorkMode, CString strKuNum, COleDateTime TimeStop);
void TimeOnTempOff(CEdit *pEdit, CString strWorkMode);

#endif