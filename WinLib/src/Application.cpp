//
//	�tApplication Framework
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include <locale.h>
#include "Application.h"
#include "Misc.h"

#ifdef __BORLANDC__
static int WINAPI (*WinMainPtr)(HINSTANCE, HINSTANCE, LPSTR, int);
#endif

CWinApp *Application;

//
// ���캯��
//
CWinApp::CWinApp()
{
#ifdef	_DEBUG
	if (Application != 0) {
		TRACE("CWinApp �Ѿ���¼ע����ϡ�\n");
	}
#endif
	Instance = 0;
	CmdLine = 0;
	CmdShow = 0;

	Application = this;
	msgCur.message = WM_NULL;
	MainWnd = 0;

#ifdef __BORLANDC__
	// WinMain���յ�ָ��
	// ���������������
	WinMainPtr = &WinMain;
#endif
}

//
// �f��������
//
CWinApp::~CWinApp()
{
	Application = 0;
}

//
// Application ��ʼ��
//
BOOL CWinApp::InitInstance()
{
	return TRUE;
}

//
// IDLE����
//
BOOL CWinApp::OnIdle(long count)
{
	return MainWnd && MainWnd->OnIdle(count);
}

//
// �¼���Ȧ(Message Loop)
//
int CWinApp::Run()
{
	bool idle = true;
	long count = 0;

	if (MainWnd)
		MainWnd->ShowWindow(CmdShow);

	for (;;) {
		if (::PeekMessage(&msgCur, NULL, 0, 0, PM_NOREMOVE)) {
			if (!::GetMessage(&msgCur, NULL, 0, 0))
				return msgCur.wParam;

			if (!PreTranslateMessage(&msgCur)) {
				::TranslateMessage(&msgCur);
				::DispatchMessage(&msgCur);
			}
			idle = true;
			count = 0;
		}
		else if (idle) {
			if (!OnIdle(count++))
				idle = false;
		}
		else {
			::WaitMessage();
		}
	}
}

//
// �¼�ǰ�ô���
//
BOOL CWinApp::PreTranslateMessage(MSG *msg)
{
	return MainWnd->PreTranslateMessage(msg);
}

//
// ��������¼ע��
//
//	ͬһ���Ʋ����ظ���¼ע��
//
BOOL CWinApp::RegisterWndClass(const char *name, UINT classStyle, HCURSOR hCursor,
	HBRUSH hbrBackground, HICON hIcon)
{
	WNDCLASS	wndclass;

	// �Ƿ��Ѿ���¼ע�᣿
	if (!GetClassInfo(Instance, name, &wndclass)) {		
		wndclass.style = classStyle;
		wndclass.lpfnWndProc = CWindow::_WindowProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = Instance;
		wndclass.hIcon = hIcon;
		wndclass.hCursor = hCursor;
		wndclass.hbrBackground = hbrBackground;
		wndclass.lpszMenuName = 0;
		wndclass.lpszClassName = name;

		if (!::RegisterClass(&wndclass)) {
			TRACE("Can't register window class [%s]\n", name);
			return FALSE;
		}
	}
	return TRUE;
}
