//
// 实例的主窗口
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
// 产生窗口的前置作业
// 设定样式与大小
// 
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
	
	CFile file("sample.bmp"); // 开启实例CG
	if (!file || !ViewImage.LoadBMP(file)) {
		MessageBox("无法读取CG檔。");
		return FALSE;
	}
	return TRUE;
}

//
// WM_PAINT 的处理（重新绘制）
//
void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	
	// 显示
	::SetDIBitsToDevice(dc, 0, 0,
		ViewImage.Width(), ViewImage.Height(),
		0, 0, 0, ViewImage.Height(),
		ViewImage.GetBits(),
		ViewImage.GetInfo(),
		DIB_RGB_COLORS);
}

//
// WM_COMMAND 的处理（菜单处理）
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
