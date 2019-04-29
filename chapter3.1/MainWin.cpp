//
// ʵ����������
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
// �������ڵ�ǰ����ҵ
// �趨��ʽ���С
// 
//
BOOL CMainWin::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.dwExStyle = WS_EX_CLIENTEDGE;
	cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	
	// ������ڴ�С 
		CRect rect(0, 0, 640, 480);
	::AdjustWindowRectEx(&rect, cs.style, TRUE, cs.dwExStyle);
	
	int width = rect.Width();
	int height = rect.Height();
	
	// ������������С
	// ��������ָ���ˡ������С����ⲿ��
	// ����������
	CRect rcArea;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
	
	// ����Ϊ�ƶ������������������
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
// �¼�����
//
LRESULT CMainWin::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_ERASEBKGND: // ȥ����
		return FALSE; // �޶���
		
	default:
		return CWindow::WindowProc(uMsg, wParam, lParam);
	}
	return 0L;
}

//
// WM_CREATE �Ĵ���
//
BOOL CMainWin::OnCreate(CREATESTRUCT *cs)
{
	LoadAccelTable(IDC_APPACCEL);
	
	CFile file("sample.bmp"); // ����ʵ��CG
	if (!file || !ViewImage.LoadBMP(file)) {
		MessageBox("�޷���ȡCG�n��");
		return FALSE;
	}
	return TRUE;
}

//
// WM_PAINT �Ĵ������»��ƣ�
//
void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	
	// ��ʾ
	::SetDIBitsToDevice(dc, 0, 0,
		ViewImage.Width(), ViewImage.Height(),
		0, 0, 0, ViewImage.Height(),
		ViewImage.GetBits(),
		ViewImage.GetInfo(),
		DIB_RGB_COLORS);
}

//
// WM_COMMAND �Ĵ����˵�����
//
void CMainWin::OnCommand(UINT notifyCode, UINT id, HWND ctrl)
{
	switch (id) {
	case ID_APP_EXIT: // ����
		SendMessage(WM_CLOSE);
		break;
		
	default:
		break;
	}
}
