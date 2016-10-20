#ifndef _ThreadProp_TimeTime_H
#define _ThreadProp_TimeTime_H

void YSJSomeku(CString strFunc, CEdit *pEdit, int nKuItemNum, int nKuNum);
void KuSomeKu(CString strFunc, CEdit *pEdit, int nKuNum);
void SelectSqlSomeKu(CString strFunc, CEdit *pEdit, CString strKuNum);
void SelectSqlAllKu(CString strFunc, CEdit *pEdit);

int GetTimeCtrlSt();
int NowTimeSpan(CEdit *pEdit, CString strWorkMode);
void GetTimeStartStop(COleDateTime TimeNow, int i);
void Time_On_OneOnOneOff(CEdit *pEdit, CString strKuNum);
void Time_Off_OneOnOneOff(CEdit *pEdit, CString strKuNum);
void Time_On_AllOnAllOff(CEdit *pEdit, CString strKuNum);
void Time_Off_AllOnAllOff(CEdit *pEdit, CString strKuNum);
void TimeOnTimeOff(CEdit *pEdit, CString strWorkMode);

#endif