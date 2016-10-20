; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSETDlg
LastTemplate=CDaoRecordset
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "CS.h"

ClassCount=7
Class1=CCSApp
Class2=CCSDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_CS_DIALOG
Class4=CSETDlg
Class5=CDaoOnProp
Class6=DaoSetProp
Class7=CDaoSetProp
Resource4=IDD_SET_DIALOG

[CLS:CCSApp]
Type=0
HeaderFile=CS.h
ImplementationFile=CS.cpp
Filter=N

[CLS:CCSDlg]
Type=0
HeaderFile=CSDlg.h
ImplementationFile=CSDlg.cpp
Filter=D
LastObject=CCSDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=CSDlg.h
ImplementationFile=CSDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_CS_DIALOG]
Type=1
Class=CCSDlg
ControlCount=0

[DLG:IDD_SET_DIALOG]
Type=1
Class=CSETDlg
ControlCount=12
Control1=IDC_STATIC,button,1342177287
Control2=IDC_LIST_ON_ITEM,listbox,1352728833
Control3=IDC_EDIT_ON_NUM,edit,1350639744
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_ON_DWKD,edit,1350639744
Control6=IDC_STATIC_ON_DIR,static,1342308352
Control7=IDC_RADIO_ON_ON,button,1342308361
Control8=IDC_RADIO_ON_OFF,button,1342177289
Control9=IDC_BUTTON_ON_ADD,button,1342242816
Control10=IDC_BUTTON_ON_INS,button,1342242816
Control11=IDC_BUTTON_ON_DEL,button,1342242816
Control12=IDC_LIST_CTRL_ON_PROP,SysListView32,1350631425

[CLS:CSETDlg]
Type=0
HeaderFile=SETDlg.h
ImplementationFile=SETDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_LIST_CTRL_ON_PROP
VirtualFilter=dWC

[DB:CDAOOnProp]
DB=1
DBType=DAO
ColumnCount=7
Column1=[序号], 4, 4
Column2=[项目], 12, 50
Column3=[编号], -6, 1
Column4=[特征], 12, 50
Column5=[参数], 5, 2
Column6=[状态], -7, 1
Column7=[其他], -1, 0

[CLS:CDaoOnProp]
Type=0
HeaderFile=DaoOnProp.h
ImplementationFile=DaoOnProp.cpp
BaseClass=CDaoRecordset
Filter=N
VirtualFilter=x

[DB:CDaoOnProp]
DB=1
DBType=DAO
ColumnCount=7
Column1=[序号], 4, 4
Column2=[项目], 12, 50
Column3=[编号], -6, 1
Column4=[特征], 12, 50
Column5=[参数], 5, 2
Column6=[状态], -7, 1
Column7=[其他], -1, 0

[CLS:DaoSetProp]
Type=0
HeaderFile=DaoSetProp.h
ImplementationFile=DaoSetProp.cpp
BaseClass=CDaoRecordset
Filter=N
VirtualFilter=x

[DB:DaoSetProp]
DB=1
DBType=DAO
ColumnCount=7
Column1=[Func], 12, 50
Column2=[NO], 4, 4
Column3=[Item], 12, 50
Column4=[Num], -6, 1
Column5=[Dir], 12, 50
Column6=[DWKD], 5, 2
Column7=[OF], -7, 1

[CLS:CDaoSetProp]
Type=0
HeaderFile=DaoSetProp.h
ImplementationFile=DaoSetProp.cpp
BaseClass=CDaoRecordset
Filter=N
VirtualFilter=x

[DB:CDaoSetProp]
DB=1
DBType=DAO
ColumnCount=7
Column1=[Func], 12, 50
Column2=[NO], 4, 4
Column3=[Item], 12, 50
Column4=[Num], -6, 1
Column5=[Dir], 12, 50
Column6=[DWKD], 5, 2
Column7=[OF], -7, 1

