//
// Framework (dialog)
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "Application.h"
#include "Dialog.h"

//
// (Default) �Ի�����¼�����ʽ
//
BOOL CDialog::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

//
// ��ͨһ��ĶԻ����¼�����ʽ
//
// ÿ�������¼�����ʽ(��Ա��ʽ)���ᱻ����
//
BOOL CALLBACK CDialog::_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDialog *pThis = (CDialog *)GetWindowLong(hDlg, DWL_USER);

	if (uMsg == WM_INITDIALOG) {
		pThis = (CDialog *)lParam;
		pThis->hDlg = hDlg;
		SetWindowLong(hDlg, DWL_USER, lParam);
	}
	else if (!pThis) {
		return FALSE;
	}
	return pThis->DlgProc(uMsg, wParam, lParam);
}

//
// ����һ��modal�Ի���
//
int CDialog::DoModal(UINT id, HWND hParent)
{
	return ::DialogBoxParam(*Application, MAKEINTRESOURCE(id), hParent, _DlgProc, (LPARAM)this);
}

//
// ����һ��modeless�Ի���
//
HWND CDialog::DoModeless(UINT id, HWND hParent)
{
	return ::CreateDialogParam(*Application, MAKEINTRESOURCE(id), hParent, _DlgProc, (LPARAM)this);
}

//
// ���ŶԻ������ϲ㴰�ڵ��м�
//
void CDialog::CenterWindow()
{
	HWND	hParent = ::GetParent(hDlg);
	CRect	rectParent;
	CRect	rectWindow;

	::GetWindowRect(hDlg, &rectWindow);
	::GetClientRect(hParent, &rectParent);

	CPoint	pt(rectParent.Width() / 2, rectParent.Height() / 2);

	::ClientToScreen(hParent, &pt);

	int	width = rectWindow.Width();
	int	height = rectWindow.Height();

	pt.x -= width / 2;
	pt.y -= height / 2;

	::MoveWindow(hDlg, pt.x, pt.y, width, height, FALSE);
}
