#ifndef _ThreadProp_TempTemp_H
#define _ThreadProp_TempTemp_H

void TempOnTempOff_TempOn_KuOnMax(CEdit *pEdit, int nKuIndex, CString str);
void Temp_On_AllOnOneOff(CEdit *pEdit, int nKuIndex, CString str, CString strKuNum);
void Temp_On_AllOnAllOff(CEdit *pEdit, int nKuIndex, CString str);
void TempOrTime_On_OneOnOneOff(CEdit *pEdit, int nKuIndex, CString str);
void Temp_Off_AllOff(CEdit *pEdit, int nKuIndex, CString str);
void TempOrTime_Off_OneOff(CEdit *pEdit, int nKuIndex, CString str);
void TempOnTempOff(CEdit *pEdit, CString strWorkMode);

#endif