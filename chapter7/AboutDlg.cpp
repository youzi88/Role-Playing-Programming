//
// About-关于对话框
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "Application.h"
#include "Dialog.h"
#include "AboutDlg.h"
#include "resource.h"

//
// “关于本软件”这个事件是以对话框来处理
//
//	藉由初始化动作，IDC_COMPANY和IDC_TITLE会被重写
//
BOOL CAboutDlg::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	  case WM_INITDIALOG:		// 初始化
		{
			char	copyright[256];
			sprintf(copyright, "Copyright(c) %s %s", __DATE__ + 7, CompanyName);
			SetDlgItemText(IDC_COMPANY, copyright);
			SetDlgItemText(IDC_TITLE, ApplicationTitle);
			CenterWindow();
		}
		break;

	  case WM_COMMAND:			// 按下按钮
		switch (LOWORD(wParam)) {
		  case IDOK:
		  case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		break;

	  default:
		return FALSE;
	}
	return TRUE;
}
