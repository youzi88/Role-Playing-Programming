//
// Script player application
//
// Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Sample.h"
#include "resource.h"

//
// Application�Ķ���ʵ��(instance)
//
CSampleApp theApp;

//
// Application ��ʼ��
//
BOOL CSampleApp::InitInstance()
{
	// ����������MainWin
	if (!MainWin.Create(this, "Sample", LoadMenu(IDC_APPMENU)))
		return FALSE;
	
	MainWnd = &MainWin;
	
	return TRUE;
}
