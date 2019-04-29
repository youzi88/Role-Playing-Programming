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
// �������ڵ�ǰ�ô���
//
// �趨��ʽ���С
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
	case WM_CLOSE: // �رմ���
		OnClose();
		break;
		
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
	
	CClientDC dc(this);
	
	// ͼ�������ȷ��
	if (!ViewImage.Create(dc, 640, 480)) {
		MessageBox("�ڴ治��ȷ��ͼ������\n"
			"��ر�����������Ӧ�ó�������������");
		return FALSE;
	}
	
	{
		CFile file("sample.bmp"); // ����ʵ��CGͼ�ļ�
		if (!file || !Back.LoadBMP(file)) {
			MessageBox("�޷���ȡCGͼ����");
			return FALSE;
		}
	}
	{
		CFile file("overlap.bmp"); // ����Ҫ�ص���CGͼ��
		if (!file || !Overlap.LoadBMP(file)) {
			MessageBox("�޷���ȡҪ�ص���CGͼ����");
			return FALSE;
		}
	}
	// ��������
	ViewImage.Copy(&Back);
	// �ص�
	ViewImage.MixImage(&Overlap, CRect(0, 0, 640, 480));
	
	return TRUE;
}

//
// WM_CLOSE �Ĵ���
//
void CMainWin::OnClose()
{
	if (MessageBox("Ҫ������������?", "Sample",
		MB_ICONQUESTION|MB_OKCANCEL) == IDOK) {
		::DestroyWindow(hWnd);
	}
}

//
// WM_PAINT �Ĵ���(���»���)
//
void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	ViewImage.Draw(dc, dc.ps.rcPaint);
}

//
// WM_COMMAND �Ĵ���(����˵��˵�)
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
