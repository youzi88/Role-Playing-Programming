//
// Script player main window
//
// Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include <file.h>
#include "Sample.h"
#include "Window.h"
#include "MainWin.h"
#include "resource.h"

//
// 产生窗口的前置处理
//
// 设定样式与大小
//
BOOL CMainWin::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.dwExStyle = WS_EX_CLIENTEDGE;
	cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	
	// 求出窗口大小
	CRect rect(0, 0, 640, 480);
	::AdjustWindowRectEx(&rect, cs.style, TRUE, cs.dwExStyle);
	
	int width = rect.Width();
	int height = rect.Height();
	
	// 求出工作区域大小
	// 工作区是指除了「工作列」以外部分
	// 的整个画面
	CRect rcArea;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
	
	// 调整为移动到工作区域的正中央
	int x = rcArea.left + (rcArea.Width() - width) / 2;
	int y = rcArea.top + (rcArea.Height() - height) / 2;
	
	cs.x = x;
	cs.y = y;
	cs.cx = width;
	cs.cy = height;
	cs.lpszClass = "MainWindow";
	
	if (!Application->RegisterWndClass(cs.lpszClass,
		CS_VREDRAW | CS_HREDRAW | CS_OWNDC, LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)::GetStockObject(BLACK_BRUSH), Application->LoadIcon(IDC_APPICON)))
		return FALSE;
	return TRUE;
}

//
// 事件处理
//
LRESULT CMainWin::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CLOSE: // 关闭窗口
		OnClose();
		break;
		
	case WM_ERASEBKGND: // 去背景
		return FALSE; // 无动作
		
	default:
		return CWindow::WindowProc(uMsg, wParam, lParam);
	}
	return 0L;
}

//
// WM_CREATE 的处理
//
BOOL CMainWin::OnCreate(CREATESTRUCT *cs)
{
	LoadAccelTable(IDC_APPACCEL);
	
	CClientDC dc(this);
	
	// 图像区域的确保
	if (!ViewImage.Create(dc, 640, 480)) {
		MessageBox("内存不能确保图像区域\n"
			"请关闭其它开启的应用程序再重新启动");
		return FALSE;
	}
	
	{
		CFile file("sample.bmp"); // 开启实例CG图文件
		if (!file || !Back.LoadBMP(file)) {
			MessageBox("无法读取CG图档。");
			return FALSE;
		}
	}
	{
		CFile file("overlap.bmp"); // 开启要重迭的CG图档
		if (!file || !Overlap.LoadBMP(file)) {
			MessageBox("无法读取要重迭的CG图档。");
			return FALSE;
		}
	}
	// 背景复制
	ViewImage.Copy(&Back);
	// 重迭
	ViewImage.MixImage(&Overlap, CRect(0, 0, 640, 480));
	
	return TRUE;
}

//
// WM_CLOSE 的处理
//
void CMainWin::OnClose()
{
	if (MessageBox("要结束程序了吗?", "Sample",
		MB_ICONQUESTION|MB_OKCANCEL) == IDOK) {
		::DestroyWindow(hWnd);
	}
}

//
// WM_PAINT 的处理(重新绘制)
//
void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	ViewImage.Draw(dc, dc.ps.rcPaint);
}

//
// WM_COMMAND 的处理(处理菜单菜单)
//
void CMainWin::OnCommand(UINT notifyCode, UINT id, HWND ctrl)
{
	switch (id) {
	case ID_APP_EXIT: // 结束
		SendMessage(WM_CLOSE);
		break;
		
	default:
		break;
	}
}
