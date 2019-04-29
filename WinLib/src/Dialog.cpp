//
// Framework (dialog)
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "Application.h"
#include "Dialog.h"

//
// (Default) 对话框的事件处理函式
//
BOOL CDialog::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

//
// 共通一般的对话框事件处理函式
//
// 每个类别的事件处理函式(成员函式)都会被调用
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
// 开启一个modal对话框
//
int CDialog::DoModal(UINT id, HWND hParent)
{
	return ::DialogBoxParam(*Application, MAKEINTRESOURCE(id), hParent, _DlgProc, (LPARAM)this);
}

//
// 开启一个modeless对话框
//
HWND CDialog::DoModeless(UINT id, HWND hParent)
{
	return ::CreateDialogParam(*Application, MAKEINTRESOURCE(id), hParent, _DlgProc, (LPARAM)this);
}

//
// 安排对话框在上层窗口的中间
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
