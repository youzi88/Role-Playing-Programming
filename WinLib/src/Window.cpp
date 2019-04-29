//
//	Window Framework
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Application.h"
#include "Window.h"
#include "Misc.h"
#include "dc.h"

//
// Window List 的路径
//
CWindow *CWindow::WindowListTop;

//
// 在建立时的暂时指标
//
CWindow *CWindow::WindowInitPtr;

//
// 构造函数
//
CWindow::CWindow()
{
	App = 0;
	hWnd = 0;
	LastMsg = WM_NULL;
	LastWParam = 0;
	LastLParam = 0;

	hAccelTable = 0;

	Next = 0;
}

//
// 析构函数
//
CWindow::~CWindow()
{
}

//
// 从WindowList取得指定的CWindow对象
//
CWindow *CWindow::GetWindow(HWND hwnd)
{
	for (CWindow *p=WindowListTop; p; p=p->Next) {
		if (p->hWnd == hwnd)
			return p;
	}
	return 0;
}

//
// 将Window登录注册到WindowList
//
BOOL CWindow::AddWindowList(HWND hWndNew)
{
	Next = WindowListTop;
	WindowListTop = this;
	hWnd = hWndNew;
	return TRUE;
}

//
// 从WindowList删除Window
//
HWND CWindow::DeleteWindowList()
{
	for (CWindow **p = &WindowListTop; *p; p = &((*p)->Next)) {
		if (*p == this) {
			*p = Next;
			break;
		}
	}
	return hWnd;
}

//
// 事件处理函式（共用）
//
//	调用事件处理函式(成员函式)
//
LRESULT CALLBACK CWindow::_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWindow *win = GetWindow(hwnd);

	if (win == 0 && WindowInitPtr != 0) {
		win = WindowInitPtr;
		WindowInitPtr->AddWindowList(hwnd);
		WindowInitPtr = 0;
	}
	if (win) {
		win->SetLastParam(uMsg, wParam, lParam);
		return win->WindowProc(uMsg, wParam, lParam);
	}
	return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//
// 读入快速键对照表（Keyboard Acceleration Table）
//
BOOL CWindow::LoadAccelTable(const char *name)
{
	hAccelTable = Application->LoadAccelerators(name);
	return (hAccelTable != NULL);
}

//
// 事件的前置处理
//
BOOL CWindow::PreTranslateMessage(MSG *msg)
{
	if (msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST) {
		return hAccelTable != NULL && ::TranslateAccelerator(hWnd, hAccelTable, msg);
	}
	return FALSE;
}

//
// 产生窗口
//
BOOL CWindow::Create(CWinApp *app, const char *name, HMENU hMenu, CWindow *parent)
{
	App = app;

	CREATESTRUCT cs;
	cs.dwExStyle = WS_EX_CLIENTEDGE;
	cs.lpszClass = NULL;
	cs.lpszName = name;
	cs.style = WS_OVERLAPPEDWINDOW;
	cs.x = CW_USEDEFAULT;
	cs.y = CW_USEDEFAULT;
	cs.cx = CW_USEDEFAULT;
	cs.cy = CW_USEDEFAULT;
	cs.hwndParent = parent? parent->hWnd: 0;
	cs.hMenu = hMenu;
	cs.hInstance = *app;
	cs.lpCreateParams = 0;

	if (!PreCreateWindow(cs))
		return FALSE;

	if (cs.lpszClass == 0) {
		cs.lpszClass = "Window";
		if (!Application->RegisterWndClass(cs.lpszClass, CS_VREDRAW | CS_HREDRAW))
			return FALSE;
	}

	WindowInitPtr = this;
	HWND hWnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass,
		cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy,
		cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	if (WindowInitPtr != NULL) {
		WindowInitPtr = NULL;
		return FALSE;
	}
	if (hWnd == NULL)
		return FALSE;
	return TRUE;
}

//
// 产生窗口的前置处理
//
BOOL CWindow::PreCreateWindow(CREATESTRUCT &cs)
{
	return TRUE;
}

//
// 成员函式的事件处理函式
//
LRESULT CWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	  case WM_CREATE:
		if (!OnCreate((LPCREATESTRUCT)lParam))
			return -1;
		break;

	  case WM_PAINT:
		OnPaint();
		break;

	  case WM_DESTROY:
		OnDestroy();
		break;

	  case WM_COMMAND:
		OnCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
		break;

	  case WM_INITMENUPOPUP:
		OnInitMenuPopup((HMENU)wParam, LOWORD(lParam), HIWORD(lParam));
		break;

	  case WM_NCDESTROY:
		DeleteWindowList();
		break;

	  default:
		return Default();
	}
	return 0L;
}

//
// IDLE处理
//
BOOL CWindow::OnIdle(long count)
{
	return FALSE;
}

//
// WM_CREATE 的处理函式
//
BOOL CWindow::OnCreate(CREATESTRUCT *cs)
{
	return TRUE;
}

//
// WM_PAINT 的处理函式
//
void CWindow::OnPaint()
{
	CPaintDC	dc(this);	// 这是为了调用在 OnPaint 里产生 CPaintDC 的
					// BeginPaint 与 EndPaint
}

//
// WM_DESTROY 的处理函式
//
void CWindow::OnDestroy()
{
	::PostQuitMessage(0);
}

//
// WM_COMMAND 的处理函式
//
void CWindow::OnCommand(UINT notifyCode, UINT id, HWND ctrl)
{
}

//
// WM_INITMENUPOPUP 的处理函式
//
//	每个菜单项目都要调用OnInitSubMenu
//
void CWindow::OnInitMenuPopup(HMENU hMenu, UINT, BOOL sysMenu)
{
	if (sysMenu)
		return;

	int indexMax = ::GetMenuItemCount(hMenu);
	for (int index=0; index < indexMax; index++) {
		OnInitSubMenu(hMenu, ::GetMenuItemID(hMenu, index));
	}
}

void CWindow::OnInitSubMenu(HMENU hMenu, UINT id)
{
}
