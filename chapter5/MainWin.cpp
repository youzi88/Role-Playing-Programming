//
// Main Window
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Sample.h"
#include "Window.h"
#include "MainWin.h"
#include "AboutDlg.h"
#include "Battle.h"
#include "Misc.h"
#include "dc.h"
#include "resource.h"

//
// 构造函数
//
//	变量的初始化
//	当window由OnCreate初始化时，不会带出任何东西
//
CMainWin::CMainWin()
{
	Action = new CAction(this);	// 虚拟动作（dummy action）

	TIMECAPS	timeCaps;
	timeGetDevCaps(&timeCaps, sizeof(timeCaps));
	TimePeriod = max(timeCaps.wPeriodMin, 1U);
	timeBeginPeriod(TimePeriod);
}

//
// 析构函数
//
CMainWin::~CMainWin()
{
	timeEndPeriod(TimePeriod);

	DeleteAllAction();
}

//
// window产生的前置处理
//
//	指定样式和大小
//
BOOL CMainWin::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.dwExStyle = WS_EX_CLIENTEDGE;
	cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	CRect	rect(0, 0, WindowWidth, WindowHeight);
	::AdjustWindowRectEx(&rect, cs.style, TRUE, cs.dwExStyle);

	int width = rect.Width();
	int height = rect.Height();

	CRect rcArea;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);

	int	x = rcArea.left + (rcArea.Width() - width) / 2;
	int	y = rcArea.top + (rcArea.Height() - height) / 2;

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
// 事件处理
//
LRESULT CMainWin::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	  case WM_FIRSTACTION:		// 最初传送的事件
		OnFirstAction();
		break;

	  case WM_CLOSE:			// 关闭窗口
		OnClose();
		break;

	  case WM_ERASEBKGND:		// 删除背景
		return FALSE;			// 无动作

	  case WM_LBUTTONDOWN:		// 按下鼠标左键
		Action->LButtonDown(wParam, CPoint(lParam));
		break;

	  case WM_LBUTTONUP:		// 放开鼠标左键
		Action->LButtonUp(wParam, CPoint(lParam));
		break;

	  case WM_RBUTTONDOWN:		// 按下鼠标右键
		Action->RButtonDown(wParam, CPoint(lParam));
		break;

	  case WM_RBUTTONUP:		// 放开鼠标右键
		Action->RButtonUp(wParam, CPoint(lParam));
		break;

	  case WM_MOUSEMOVE:		// 鼠标指标移动
		Action->MouseMove(wParam, CPoint(lParam));
		break;

	  case WM_KEYDOWN:		// 按下键盘
		Action->KeyDown(wParam);
		break;

	  case WM_TIMER:		// 计时器到期
		OnTimer(wParam);
		break;

	  default:
		return CWindow::WindowProc(uMsg, wParam, lParam);
	}
	return 0L;
}

//
// WM_CREATE的处理
//
BOOL CMainWin::OnCreate(CREATESTRUCT *cs)
{
	LoadAccelTable(IDC_APPACCEL);

	CClientDC	dc(this);

	// 确认图像空间
	if (!ViewImage.Create(dc, WindowWidth, WindowHeight)) {
		MessageBox("没有足够的内存空间。/n"
			"请先结束其他使用中的应用程序后，再重新启动。");
		return FALSE;
	}

	// 清除显示用图像
	ViewImage.Clear();

	// 发送触发的第一个事件
	while (!PostMessage(WM_FIRSTACTION)) {
		// 若伫列已满，则PostMessage会发生error，故反覆传送
#ifdef	_DEBUG
		TRACE("PostMessage Error code = %d\n", GetLastError());
#endif
		::Sleep(110);	// 重送事件的动作再稍等一下
	}

	return TRUE;
}

//
// 游戏的“第一个”动作
//
void CMainWin::OnFirstAction()
{
	Battle();		// 战斗
}

//
// WM_CLOSE的处理
//
void CMainWin::OnClose()
{
	if (MessageBox("确定要结束程序吗?", ApplicationTitle,
		MB_ICONQUESTION|MB_OKCANCEL) == IDOK) {
		::DestroyWindow(hWnd);
	}
}

//
// WM_PAINT的处理（重新绘制）
//
void CMainWin::OnPaint()
{
	CPaintDC	dc(this);
	ViewImage.Draw(dc, dc.ps.rcPaint);
}

//
// WM_TIMER的处理
//
void CMainWin::OnTimer(int id)
{
	if (Action->TimedOut(id))
		KillTimer(id);
}

//
// WM_COMMAND的处理（菜单处理）
//
void CMainWin::OnCommand(UINT notifyCode, UINT id, HWND ctrl)
{
	switch (id) {
	  case ID_APP_EXIT:			// 结束
		SendMessage(WM_CLOSE);
		break;

	  case ID_APP_ABOUT:			// 版本信息
		CAboutDlg().DoModal(IDD_ABOUT, hWnd);
		break;

	  default:
		break;
	}
}

//
// 删除动作
//
void CMainWin::DeleteAllAction()
{
	while (Action) {
		CAction *action = Action->GetOldAction();
		delete Action;
		Action = action;
	}
}

//
// 战斗
//
BOOL CMainWin::Battle()
{
	CBattleAction *action = new CBattleAction(this, Action);
	if (!action->Init()) {
		delete action;
		return false;
	}
	Action = action;
	PostMessage(WM_KICKIDLE);	// 为了安全起见，传送空事件
	return true;
}
