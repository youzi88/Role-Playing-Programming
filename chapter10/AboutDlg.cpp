//
// About-���ڶԻ���
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "Application.h"
#include "Dialog.h"
#include "AboutDlg.h"
#include "resource.h"

//
// �����ڱ����������¼����ԶԻ���������
//
//	���ɳ�ʼ��������IDC_COMPANY��IDC_TITLE�ᱻ��д
//
BOOL CAboutDlg::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	  case WM_INITDIALOG:		// ��ʼ��
		{
			char	copyright[256];
			sprintf(copyright, "Copyright(c) %s %s", __DATE__ + 7, CompanyName);
			SetDlgItemText(IDC_COMPANY, copyright);
			SetDlgItemText(IDC_TITLE, ApplicationTitle);
			CenterWindow();
		}
		break;

	  case WM_COMMAND:			// ���°�ť
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
