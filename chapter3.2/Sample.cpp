//
// Script player application
//
// Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Sample.h"
#include "resource.h"

//
// Application的对象实例(instance)
//
CSampleApp theApp;

//
// Application 初始化
//
BOOL CSampleApp::InitInstance()
{
	// 产生主窗口MainWin
	if (!MainWin.Create(this, "Sample", LoadMenu(IDC_APPMENU)))
		return FALSE;
	
	MainWnd = &MainWin;
	
	return TRUE;
}
