; CLW ファイルは MFC ClassWizard の情報を含んでいます。

[General Info]
Version=1
LastClass=CAddDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ParamEdit.h"

ClassCount=3
Class1=CParamEditApp
Class2=CParamEditDlg

ResourceCount=3
Resource2=IDD_PARAMEDIT_DIALOG
Resource1=IDR_MAINFRAME
Class3=CAddDlg
Resource3=IDD_ADD

[CLS:CParamEditApp]
Type=0
HeaderFile=ParamEdit.h
ImplementationFile=ParamEdit.cpp
Filter=N

[CLS:CParamEditDlg]
Type=0
HeaderFile=ParamEditDlg.h
ImplementationFile=ParamEditDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CParamEditDlg



[DLG:IDD_PARAMEDIT_DIALOG]
Type=1
Class=?
ControlCount=26
Control1=IDC_STATIC,static,1342308352
Control2=IDC_NAME,combobox,1344340227
Control3=IDC_STATIC,static,1342308352
Control4=IDC_LEVEL,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_MOVEDIST,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_ATTACKDIST,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_ATTACK,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_MAGICPOWER,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_DEFENCE,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_RESISTANCE,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_HP,edit,1350631552
Control19=IDC_STATIC,static,1342308352
Control20=IDC_MP,edit,1350631552
Control21=IDC_STATIC,static,1342308352
Control22=IDC_MAGIC,combobox,1344339971
Control23=IDC_SET,button,1342242816
Control24=IDC_ADD,button,1342242816
Control25=IDOK,button,1342242817
Control26=IDCANCEL,button,1342242816

[DLG:IDD_ADD]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_NAMEEDIT,edit,1350631552
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816

[CLS:CAddDlg]
Type=0
HeaderFile=AddDlg.h
ImplementationFile=AddDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_NAMEEDIT
VirtualFilter=dWC

