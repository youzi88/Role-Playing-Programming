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
// CDC 析构函数
//
CDC::~CDC()
{
}

//
// CClientDC 构造函数
//
CClientDC::CClientDC(CWindow *win)
{
	hWnd = *win;
	hDC = ::GetDC(hWnd);
}

//
// CClientDC 析构函数
//
CClientDC::~CClientDC()
{
	::ReleaseDC(hWnd, hDC);
}

//
// CPaintDC 构造函数
//
CPaintDC::CPaintDC(CWindow *win)
{
	hWnd = *win;
	hDC = ::BeginPaint(hWnd, &ps);
}

//
// CPaintDC 析构函数
//
CPaintDC::~CPaintDC()
{
	::EndPaint(hWnd, &ps);
}

//
// CMemoryDC 构造函数
//
CMemoryDC::CMemoryDC(HDC hdc)
{
	hDC = ::CreateCompatibleDC(hdc);
}

//
// CMemoryDC 析构函数
//
CMemoryDC::~CMemoryDC()
{
	::DeleteDC(hDC);
}

//
// CScreenDC 构造函数
//
CScreenDC::CScreenDC()
{
	hDC = ::GetDC(0);
}

//
// CScreenDC 析构函数
//
CScreenDC::~CScreenDC()
{
	::ReleaseDC(0, hDC);
}
