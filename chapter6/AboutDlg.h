//
// About-关于对话框
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__AboutDlg_h__
#define	__AboutDlg_h__

#include "Dialog.h"

//
// Dialog类别的继承，事件处理的执行
//
// 对话框的支援现在已OK
//
class CAboutDlg: public CDialog	{
  protected:
	virtual BOOL DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
} ;

#endif
