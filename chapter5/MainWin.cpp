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
// ���캯��
//
//	�����ĳ�ʼ��
//	��window��OnCreate��ʼ��ʱ����������κζ���
//
CMainWin::CMainWin()
{
	Action = new CAction(this);	// ���⶯����dummy action��

	TIMECAPS	timeCaps;
	timeGetDevCaps(&timeCaps, sizeof(timeCaps));
	TimePeriod = max(timeCaps.wPeriodMin, 1U);
	timeBeginPeriod(TimePeriod);
}

//
// ��������
//
CMainWin::~CMainWin()
{
	timeEndPeriod(TimePeriod);

	DeleteAllAction();
}

//
// window������ǰ�ô���
//
//	ָ����ʽ�ʹ�С
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
// �¼�����
//
LRESULT CMainWin::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	  case WM_FIRSTACTION:		// ������͵��¼�
		OnFirstAction();
		break;

	  case WM_CLOSE:			// �رմ���
		OnClose();
		break;

	  case WM_ERASEBKGND:		// ɾ������
		return FALSE;			// �޶���

	  case WM_LBUTTONDOWN:		// ����������
		Action->LButtonDown(wParam, CPoint(lParam));
		break;

	  case WM_LBUTTONUP:		// �ſ�������
		Action->LButtonUp(wParam, CPoint(lParam));
		break;

	  case WM_RBUTTONDOWN:		// ��������Ҽ�
		Action->RButtonDown(wParam, CPoint(lParam));
		break;

	  case WM_RBUTTONUP:		// �ſ�����Ҽ�
		Action->RButtonUp(wParam, CPoint(lParam));
		break;

	  case WM_MOUSEMOVE:		// ���ָ���ƶ�
		Action->MouseMove(wParam, CPoint(lParam));
		break;

	  case WM_KEYDOWN:		// ���¼���
		Action->KeyDown(wParam);
		break;

	  case WM_TIMER:		// ��ʱ������
		OnTimer(wParam);
		break;

	  default:
		return CWindow::WindowProc(uMsg, wParam, lParam);
	}
	return 0L;
}

//
// WM_CREATE�Ĵ���
//
BOOL CMainWin::OnCreate(CREATESTRUCT *cs)
{
	LoadAccelTable(IDC_APPACCEL);

	CClientDC	dc(this);

	// ȷ��ͼ��ռ�
	if (!ViewImage.Create(dc, WindowWidth, WindowHeight)) {
		MessageBox("û���㹻���ڴ�ռ䡣/n"
			"���Ƚ�������ʹ���е�Ӧ�ó����������������");
		return FALSE;
	}

	// �����ʾ��ͼ��
	ViewImage.Clear();

	// ���ʹ����ĵ�һ���¼�
	while (!PostMessage(WM_FIRSTACTION)) {
		// ��������������PostMessage�ᷢ��error���ʷ�������
#ifdef	_DEBUG
		TRACE("PostMessage Error code = %d\n", GetLastError());
#endif
		::Sleep(110);	// �����¼��Ķ������Ե�һ��
	}

	return TRUE;
}

//
// ��Ϸ�ġ���һ��������
//
void CMainWin::OnFirstAction()
{
	Battle();		// ս��
}

//
// WM_CLOSE�Ĵ���
//
void CMainWin::OnClose()
{
	if (MessageBox("ȷ��Ҫ����������?", ApplicationTitle,
		MB_ICONQUESTION|MB_OKCANCEL) == IDOK) {
		::DestroyWindow(hWnd);
	}
}

//
// WM_PAINT�Ĵ������»��ƣ�
//
void CMainWin::OnPaint()
{
	CPaintDC	dc(this);
	ViewImage.Draw(dc, dc.ps.rcPaint);
}

//
// WM_TIMER�Ĵ���
//
void CMainWin::OnTimer(int id)
{
	if (Action->TimedOut(id))
		KillTimer(id);
}

//
// WM_COMMAND�Ĵ����˵�����
//
void CMainWin::OnCommand(UINT notifyCode, UINT id, HWND ctrl)
{
	switch (id) {
	  case ID_APP_EXIT:			// ����
		SendMessage(WM_CLOSE);
		break;

	  case ID_APP_ABOUT:			// �汾��Ϣ
		CAboutDlg().DoModal(IDD_ABOUT, hWnd);
		break;

	  default:
		break;
	}
}

//
// ɾ������
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
// ս��
//
BOOL CMainWin::Battle()
{
	CBattleAction *action = new CBattleAction(this, Action);
	if (!action->Init()) {
		delete action;
		return false;
	}
	Action = action;
	PostMessage(WM_KICKIDLE);	// Ϊ�˰�ȫ��������Ϳ��¼�
	return true;
}
