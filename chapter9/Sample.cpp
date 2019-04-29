//
//	Application
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Sample.h"
#include "resource.h"

//
// Application的对象实例(instance)
//
CSampleApp	theApp;

//
// 构造函数
//
CSampleApp::CSampleApp()
{
	hMutex = 0;
}

//
// 析构函数
//
CSampleApp::~CSampleApp()
{
	if (hMutex)
		::CloseHandle(hMutex);
}

//
// Application 初始化
//
BOOL CSampleApp::InitInstance()
{
	// 防止Application多重启动
	if ((hMutex = ::CreateMutex(NULL, FALSE, ApplicationName "_Running")) == NULL)
		return FALSE;

	DWORD	Ret = ::WaitForSingleObject(hMutex, 0);
	if (Ret == WAIT_TIMEOUT || Ret == WAIT_FAILED)
		return FALSE;

	// 若不是真彩模式则不能启动应用程序
	HDC		dc = GetDC(0);
	int		nbits = GetDeviceCaps(dc, BITSPIXEL);
	ReleaseDC(0, dc);

	if (nbits < 15) {
		::MessageBox(0, "程序不能在256色以下执行。\n请更换显示模式。",
			ApplicationTitle, MB_ICONSTOP|MB_OK);
		return FALSE;
	}

	// 产生主窗口MainWin
	if (!MainWin.Create(this, ApplicationTitle, LoadMenu(IDC_APPMENU)))
		return FALSE;

	MainWnd = &MainWin;

	return TRUE;
}
