//
// Framework (DC wraper)
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Window.h"
#include "Misc.h"
#include "dc.h"

//
// CDC ��������
//
CDC::~CDC()
{
}

//
// CClientDC ���캯��
//
CClientDC::CClientDC(CWindow *win)
{
	hWnd = *win;
	hDC = ::GetDC(hWnd);
}

//
// CClientDC ��������
//
CClientDC::~CClientDC()
{
	::ReleaseDC(hWnd, hDC);
}

//
// CPaintDC ���캯��
//
CPaintDC::CPaintDC(CWindow *win)
{
	hWnd = *win;
	hDC = ::BeginPaint(hWnd, &ps);
}

//
// CPaintDC ��������
//
CPaintDC::~CPaintDC()
{
	::EndPaint(hWnd, &ps);
}

//
// CMemoryDC ���캯��
//
CMemoryDC::CMemoryDC(HDC hdc)
{
	hDC = ::CreateCompatibleDC(hdc);
}

//
// CMemoryDC ��������
//
CMemoryDC::~CMemoryDC()
{
	::DeleteDC(hDC);
}

//
// CScreenDC ���캯��
//
CScreenDC::CScreenDC()
{
	hDC = ::GetDC(0);
}

//
// CScreenDC ��������
//
CScreenDC::~CScreenDC()
{
	::ReleaseDC(0, hDC);
}
