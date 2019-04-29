//
//	Application
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Sample.h"
#include "resource.h"

//
// Application�Ķ���ʵ��(instance)
//
CSampleApp	theApp;

//
// ���캯��
//
CSampleApp::CSampleApp()
{
	hMutex = 0;
}

//
// ��������
//
CSampleApp::~CSampleApp()
{
	if (hMutex)
		::CloseHandle(hMutex);
}

//
// Application ��ʼ��
//
BOOL CSampleApp::InitInstance()
{
	// ��ֹApplication��������
	if ((hMutex = ::CreateMutex(NULL, FALSE, ApplicationName "_Running")) == NULL)
		return FALSE;

	DWORD	Ret = ::WaitForSingleObject(hMutex, 0);
	if (Ret == WAIT_TIMEOUT || Ret == WAIT_FAILED)
		return FALSE;

	// ���������ģʽ��������Ӧ�ó���
	HDC		dc = GetDC(0);
	int		nbits = GetDeviceCaps(dc, BITSPIXEL);
	ReleaseDC(0, dc);

	if (nbits < 15) {
		::MessageBox(0, "��������256ɫ����ִ�С�\n�������ʾģʽ��",
			ApplicationTitle, MB_ICONSTOP|MB_OK);
		return FALSE;
	}

	// ����������MainWin
	if (!MainWin.Create(this, ApplicationTitle, LoadMenu(IDC_APPMENU)))
		return FALSE;

	MainWnd = &MainWin;

	return TRUE;
}
