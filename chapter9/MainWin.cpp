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
#include "Script.h"
#include "Battle.h"
#include "LoadSave.h"
#include "Misc.h"
#include "dc.h"
#include "resource.h"

//
// ���캯��
//
//	�����ĳ�ʼ��
//	��window��OnCreate��ʼ��ʱ����������κζ���
//
CMainWin::CMainWin():
	TextRect(MSG_X, MSG_Y, MSG_X + MSG_W, MSG_Y + MSG_H),
	SaveRect(SAVE_X, SAVE_Y, SAVE_X + SAVE_W, SAVE_Y + SAVE_H),
	WaitMarkRect(MsgX(WAITMARK_X), MsgY(WAITMARK_Y),
		MsgX(WAITMARK_X) + MessageFont, MsgY(WAITMARK_Y) + MessageFont),
	InvalidRect(0, 0, 0, 0)
{
	CurX = CurY = 0;
	BgColor = BlackPixel;
	TextDisplay = FALSE;
	WaitMarkShowing = FALSE;

	BackShow = FALSE;
	OverlapShow = FALSE;
	TextShow = FALSE;
	MenuShow = FALSE;
	SaveShow = FALSE;

	Action = new CAction(this);	// ���⶯����dummy action��
	hFont = 0;

	// ��ʼ����CD
	// ��û��CD����Openʱ���Off
	MusicMode = MusicCD;
	music = &cdaudio;
	MusicNo = 0;

	TIMECAPS	timeCaps;
	timeGetDevCaps(&timeCaps, sizeof(timeCaps));
	TimePeriod = max(timeCaps.wPeriodMin, 1U);
	timeBeginPeriod(TimePeriod);

	ViewEffect = 0;
}

//
// ��������
//
CMainWin::~CMainWin()
{
	timeEndPeriod(TimePeriod);

	if (hFont) {
		DeleteObject(hFont);
		hFont = 0;
	}
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

	  case WM_KEYDOWN:			// ���¼���
		Action->KeyDown(wParam);
		break;

	  case WM_DESTROY:			// ȡ������
		OnDestroy();
		break;

	  case WM_TIMER:			// ��ʱ������
		OnTimer(wParam);
		break;

	  case MM_MCINOTIFY:		// MCI���¼�
		OnMciNotify(wParam, LOWORD(lParam));
		break;

	  default:
		return CWindow::WindowProc(uMsg, wParam, lParam);
	}
	return 0L;
}

//
// IDLE�Ĵ���
//
BOOL CMainWin::OnIdle(long count)
{
	if (ViewEffect) {							// �Ƿ�����Ч��?
		if (ViewEffect->Step(timeGetTime()))	// ��Чִ��
			return TRUE;						// ��Ч����
		StopWipe();								// ��Ч����
		Action->WipeDone();						// ����ͨ��������
	}
	return Action->IdleAction();
}

//
// WM_CREATE�Ĵ���
//
BOOL CMainWin::OnCreate(CREATESTRUCT *cs)
{
	LoadAccelTable(IDC_APPACCEL);

	CClientDC	dc(this);

	// ȷ��ͼ���ڴ�ռ�
	if (!ViewImage.Create(dc, WindowWidth, WindowHeight)
	 || !MixedImage.Create(dc, WindowWidth, WindowHeight)
	 || !BackLayer.Create(WindowWidth, WindowHeight)
	 || !OverlapLayer.Create(WindowWidth, WindowHeight)) {
		MessageBox("�ڴ治�㡣\n"
			"��ر�����Ӧ�ó�����������");
		return FALSE;
	}

	// �����ʾ��ͼ��
	ViewImage.Clear();

	// ���в�������
	if ((hFont = CreateFont(-MessageFont, 0, 0, 0, MessageStyle, FALSE, FALSE, FALSE,
		GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "����")) == 0) {
		MessageBox("û������!");
		return FALSE;
	}

	// ���ʹ����ĵ�һ���¼�
	while (!PostMessage(WM_FIRSTACTION)) {
		// ��������������PostMessage�ᷢ��error���ʷ�������
#ifdef	_DEBUG
		TRACE("PostMessage Error code = %d\n", GetLastError());
#endif
		::Sleep(110);	// �����¼��Ķ������Ե�һ��
	}

	Param.Clear();		// ��������ڵĶ���

	return TRUE;
}

//
// ��Ϸ�ġ���һ��������
//
void CMainWin::OnFirstAction()
{
	// �����Ҫ��CD-DA�Ὺ��
	if (music) {
		if (!music->Open(this) && MusicMode == MusicCD) {
			MusicMode = MusicOff;
			music = 0;
		}
	}
	wave.Open(this);

	StartMainMenu();
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
// WM_DESTROY�Ĵ���(���°���)
//
void CMainWin::OnDestroy()
{
	// ������ڲ������־�ֹͣ��
	// ������MCI��Ӧ�ó�����Ȼ������������Ȼ�������
	// ����Ҫ�Ǵ��ڱ��ص���ʱ���ǿ��ֹͣ��

	if (music) {
		music->Stop();
		music->Close();
		music = 0;
	}
	CWindow::OnDestroy();
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

	  case ID_MUSIC_CD:				// BGM/ON
		ChangeMusicMode(MusicCD);
		break;

	  case ID_MUSIC_OFF:			// BGM/OFF
		ChangeMusicMode(MusicOff);
		break;

	  case ID_LOADGAME:				// ����
		if (IsLoadOK())
			SetGameLoadAction();
		break;

	  case ID_SAVEGAME:				// ����
		if (IsSaveOK())
			SetGameSaveAction();
		break;

	  case ID_STOPSCRIPT:			// �ű��ж�
		if (IsSaveOK()
		 && MessageBox("Ҫ�жϹ��£�����?", ApplicationTitle,
		 	MB_ICONQUESTION|MB_OKCANCEL) == IDOK)
			Action->Abort();
		break;

	  default:
		break;
	}
}

//
// �˵���Ŀ�ĳ�ʼ��
//
void CMainWin::OnInitSubMenu(HMENU hMenu, UINT id)
{
	switch (id) {
	  case ID_MUSIC_CD:				// BGM/ON
		// ���˵���Ŀ
		CheckMenuItem(hMenu, id, MusicMode == MusicCD? MF_CHECKED: MF_UNCHECKED);
		break;

	  case ID_MUSIC_OFF:			// BGM/OFF
		// ���˵���Ŀ
		CheckMenuItem(hMenu, id, MusicMode == MusicOff? MF_CHECKED: MF_UNCHECKED);
		break;

	  case ID_LOADGAME:				// ����
		// ֻ���ڿ�������ʱ�˵��ϵ���Ŀ��������
		EnableMenuItem(hMenu, id, IsLoadOK()? MF_ENABLED: (MF_DISABLED | MF_GRAYED));
		break;

	  case ID_SAVEGAME:				// ����
		// ֻ���ڿ��Դ���ʱ�˵��ϵ���Ŀ��������
		EnableMenuItem(hMenu, id, IsSaveOK()? MF_ENABLED: (MF_DISABLED | MF_GRAYED));
		break;

	  case ID_STOPSCRIPT:			// �ű��ж�
		// ֻ���ڿ����ж�ʱ�˵��ϵ���Ŀ��������
		EnableMenuItem(hMenu, id, IsSaveOK()? MF_ENABLED: (MF_DISABLED | MF_GRAYED));
		break;
	}
}

//
// MCI�������¼�
//
// ��Ϊ�趨����play����call back�������ڲ��Ž���ʱ����
//
void CMainWin::OnMciNotify(unsigned flag, unsigned id)
{
	if (flag == MCI_NOTIFY_SUCCESSFUL) {
		if (music && music->GetId() == id) {
			Action->MusicDone(MusicNo);
		}
		else if (wave.GetId() == id) {
			wave.Stop();
			Action->WaveDone();
		}
	}
}

//
// ���ֲ��Ż����µ�
//
void CMainWin::ChangeMusicMode(int mode)
{
	if (MusicMode != mode) {		// �����ģʽ�Ļ�
		MusicMode = mode;
		if (music) {				// �����о�ֹͣ
			music->Stop();
			music->Close();
			music = 0;
		}
		switch (MusicMode) {
		  case MusicCD:
			music = &cdaudio;
			if (!music->Open(this)) {
				MusicMode = MusicOff;
				music = 0;
			}
			break;
		}
		if (music && MusicNo > 0)
			music->Play(MusicNo);	// ����������ָ��������
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
// Script����
//
void CMainWin::ScriptDone()
{
	StopWipe();
	StopMusic();	// ֹͣ���ֲ���

	CAction *action = Action->GetOldAction();
	delete Action;
	Action = action;
	if (Action == 0)		// û�б�����Ķ���
		StartMainMenu();	// �ص����˵�
}

//
// �л������뻭�����
//
void CMainWin::SetGameLoadAction()
{
	ShowLoadSaveMenu(FALSE);
	Action->Pause();
	Action = new CGameLoadAction(this, Action);
	PostMessage(WM_KICKIDLE);	// Ϊ������������Ϳ��¼�
}

//
// �л��Ĵ��滭�����
//
void CMainWin::SetGameSaveAction()
{
	ShowLoadSaveMenu(TRUE);
	Action->Pause();
	Action = new CGameSaveAction(this, Action);
	PostMessage(WM_KICKIDLE);	// Ϊ������������Ϳ��¼�
}

//
// ִ��Script
//
bool CMainWin::StartScript(const char *name, int mode)
{
	CScriptAction *action = new CScriptAction(this, Action, mode);
	if (!action->Load(name)) {			// ����ű�
		delete action;
		return false;
	}

	Action = action;
	PostMessage(WM_KICKIDLE);	// Ϊ������������Ϳ��¼�
	return true;
}

//
// ���˵�
//
// ִ�����˵�Script
//
void CMainWin::StartMainMenu()
{
	DeleteAllAction();
	if (!StartScript("main", CScriptAction::MODE_SYSTEM))
		::DestroyWindow(hWnd);
}

//
// ���¼���Χ��д����
//
void CMainWin::WriteMessage(const char *msg)
{
	FormatMessage(msg);

	WaitMarkShowing = TRUE;

	ShowMessageWindow();
}

//
// �������ȴ�����������
//
void CMainWin::HideWaitMark()
{
	if (WaitMarkShowing) {
		WaitMarkShowing = FALSE;
		if (TextShow) {
			Mixing(WaitMarkRect, TextWaitMark);
			CopyAndRepaint(WaitMarkRect);
		}
	}
}

//
// ���˵�
//
void CMainWin::OpenMenu()
{
	int		maxlen = MENU_MIN_WIDTH;

	{
		CMemoryDC	memdc(0);
		HFONT	oldFont = memdc.SelectObject(hFont);

		for (int i=0; i<MenuCount; i++) {
			CSize	size;
			memdc.GetTextExtentPoint32(MenuBuffer[i].text, MenuBuffer[i].length, &size);
			if (maxlen < size.cx)
				maxlen = size.cx;
		}
		memdc.SelectObject(oldFont);
	}

	MenuRect.top = MENU_Y - ((MENU_FRAME_HEIGHT * 2) + MenuCount * MENU_ITEM_HEIGHT
		- MENU_ITEM_SPACE);
	MenuRect.left = MENU_X;
	MenuRect.bottom = MENU_Y;
	MenuRect.right = MENU_X + (MENU_FRAME_WIDTH * 2) + maxlen;

	MenuShow = TRUE;
	Mixing(MenuRect);
	CopyAndRepaint(MenuRect);
}

//
// ѡȡ/��ѡȡ��ʾ�˵���Ŀ
//
void CMainWin::SelectMenu(int index, bool select)
{
	if (index >= 0) {
		MenuBuffer[index].color = select? RedPixel: WhitePixel;
		CRect	r;
		r.left = MenuRect.left + MENU_FRAME_WIDTH;
		r.top = MenuRect.top + MENU_FRAME_HEIGHT + MENU_ITEM_HEIGHT * index;
		r.right = r.left + MenuRect.Width() - MENU_FRAME_WIDTH * 2;
		r.bottom = r.top + MessageFont;
		Mixing(r, MenuItem(index));
		CopyAndRepaint(r);
	}
}

//
// ��ʾ�¼���Χ
//
void CMainWin::ShowMessageWindow()
{
	TextDisplay = TRUE;

	TextShow = TRUE;
	Invalidate(TextRect);
	Update();
}

//
// �����¼���Χ
//
void CMainWin::HideMessageWindow(bool update)
{
	TextDisplay = FALSE;

	if (TextShow) {
		TextShow = FALSE;
		Invalidate(TextRect);
		if (update)
			Update();
	}
}

//
// ѡ��/��ѡ����ʾ�¼���Χ
//
void CMainWin::FlipMessageWindow()
{
	if (TextDisplay) {
		TextShow = TextShow? FALSE: TRUE;
		Invalidate(TextRect);
		Update();
	}
}

//
// ���ز˵�
//
void CMainWin::HideMenuWindow(bool update)
{
	if (MenuShow) {
		MenuShow = FALSE;
		Invalidate(MenuRect);
		if (update)
			Update();
	}
}

//
// �����ȫ���趨�ɲ���ʾ
//
// pix �������������ɫ
//
void CMainWin::HideAllLayer(COLORREF pix)
{
	BgColor = pix;
	BackShow = FALSE;
	OverlapShow = FALSE;
}

//
// װ����ʾ������
//
void CMainWin::Mixing(const CRect &rect, unsigned flags)
{
	// ����
	if (BackShow)
		MixedImage.Copy(&BackLayer, rect);		// �б����Ļ��͸���
	else
		MixedImage.FillRect(rect, BgColor);		// û�б����Ļ���Ϳ��

	// �ص�
	if (OverlapShow)							// ���ص��Ļ��ͺϳ�
		MixedImage.MixImage(&OverlapLayer, rect);

	// ����������˵�
	if (SaveShow) {
		if (flags & SaveTitle) {
			MixedImage.DrawFrameRect(SAVE_X, SAVE_Y, SAVE_TITLE_WIDTH, SAVE_ITEM_HEIGHT);
			MixedImage.DrawText(hFont, SAVE_TEXT_OFFSET_X, SAVE_TEXT_OFFSET_Y,
				IsSaveMenu? "����": "����");
		}
		for (int i=0; i<PARAMS_MAX_SAVE; i++) {
			if (flags & SaveItem(i)) {
				int		y = (i + 1) * SAVE_ITEM_INTERVAL;
				MixedImage.DrawFrameRect(SAVE_X, SAVE_Y + y, SAVE_W, SAVE_ITEM_HEIGHT,
					DataTitle[i].color);
				MixedImage.DrawText(hFont, SAVE_TEXT_OFFSET_X, SAVE_TEXT_OFFSET_Y + y,
					DataTitle[i].title, DataTitle[i].color);
			}
		}
	}
	else {
		// �¼���Χ
		if (TextShow) {
			if (flags & TextMessage) {
				MixedImage.DrawFrameRect(TextRect);
				for (int i=0; i<MessageLine; i++) {
					MixedImage.DrawText(hFont, MsgX(0), MsgY(i), MsgBuffer[i]);
				}
			}
			else {
				MixedImage.FillHalfToneRect(TextRect & rect);
			}
			// �ȴ������ķ��ţ�ʹ�è���Ų��DrawText��
			if (WaitMarkShowing && flags & TextWaitMark)
				MixedImage.DrawText(hFont, MsgX(WAITMARK_X), MsgY(WAITMARK_Y), "��");
		}
		// �˵�
		if (MenuShow) {
			if (flags & MenuFrame)
				MixedImage.DrawFrameRect(MenuRect);
			else
				MixedImage.FillHalfToneRect(MenuRect & rect);

			for (int i=0; i<MenuCount; i++) {
				if (flags & MenuItem(i)) {
					MixedImage.DrawText(hFont,
						MenuRect.left + MENU_FRAME_WIDTH,
						MenuRect.top + MENU_FRAME_HEIGHT + MENU_ITEM_HEIGHT * i,
						MenuBuffer[i].text, MenuBuffer[i].color);
				}
			}
		}
	}
}

//
// �������б仯ʱ�����»���
//
BOOL CMainWin::Update(bool repaint)
{
	if (!InvalidRect.IsRectEmpty()) {			// ����Ч����
		Mixing(InvalidRect);					// �ϳ�
		if (repaint)
			CopyAndRepaint(InvalidRect);		// ���»���
		InvalidRect.SetRectEmpty();				// ����Ч������Ϊ���ޡ�
		return TRUE;							// ��Update
	}
	return FALSE;								// ʲô������
}

//
// ��ȡ����CG
//
BOOL CMainWin::LoadImageBack(const char *name)
{
	BackShow = TRUE;
	Invalidate();
	return BackLayer.LoadImage(name);
}

//
// ��ȡ�ص�CG
//
BOOL CMainWin::LoadImageOverlap(const char *name)
{
	OverlapShow = TRUE;
	Invalidate();
	return OverlapLayer.LoadImage(name);
}

//
// ��ȡ��ҵ�õ�CG
//
BOOL CMainWin::LoadImageWork(const char *name)
{
	OverlapShow = FALSE;
	return OverlapLayer.LoadImage(name);
}

//
// ��ȥ����CG
//
BOOL CMainWin::ClearBack()
{
	BackShow = FALSE;
	Invalidate();
	return TRUE;
}

//
// ��ȥ�ص�CG
//
// ʵ����ֻ�б����ʾ״��
//
BOOL CMainWin::ClearOverlap()
{
	OverlapShow = FALSE;
	Invalidate();
	return TRUE;
}

//
// �ж��Ƿ�Ϊ���õ�����(�ַ�)
//
// ���ﴦ�������β��ý��õ��ַ�����
// �����ϵͳ��ʹ�õ��ַ��й�
//
BOOL CMainWin::Kinsoku(const char *p)
{
	switch (UC(p[0])) {
	  case 0x81:
		switch (UC(p[1])) {
		  case 0x41:
		  case 0x42:
		  case 0x49:
		  case 0x48:
		  case 0x5B:
		  case 0x6A:
		  case 0x76:
		  case 0x78:
		  case 0x99:
		  case 0xf4:
			return TRUE;
		}
		break;

	  case 0x82:
		switch (UC(p[1])) {
		  case 0x9f:
		  case 0xa1:
		  case 0xa3:
		  case 0xa5:
		  case 0xa7:
		  case 0xe1:
		  case 0xe3:
		  case 0xe5:
		  case 0xc1:
			return TRUE;
		}
		break;

	  case 0x83:
		switch (UC(p[1])) {
		  case 0x40:
		  case 0x42:
		  case 0x44:
		  case 0x46:
		  case 0x48:
		  case 0x83:
		  case 0x85:
		  case 0x87:
		  case 0x62:
			return TRUE;
		}
	}
	return FALSE;
}

#define	STR_LIMIT	(MessageWidth - 2)
#define	STR_WIDTH	(STR_LIMIT - 2)

//
// ������Ļ���װ�ã�Text Buffer��
//
void CMainWin::ClearMessage()
{
	HideMessageWindow();

	CurX = CurY = 0;

	for (int i=0; i<MessageLine; i++)
		MsgBuffer[i][0] = 0;
}

//
// �����Ļ���װ���и�ʽ���ַ������ٸ���
//
// ��Ϊ���ڴ�������������޶�����
//
int CMainWin::FormatMessage(const char *msg)
{
	CurX = CurY = 0;

	for (int i=0; i<MessageLine; i++)
		MsgBuffer[i][0] = 0;

	while (*msg && CurY < MessageLine) {
		if (*msg == '\n') {
			msg++;
			MsgBuffer[CurY][CurX] = 0;
			CurX = 0;
			CurY++;
		}
		else if (_ismbblead(*msg)) {
			if (CurX >= STR_LIMIT || (CurX >= STR_WIDTH && Kinsoku(msg) == 0)) {
				MsgBuffer[CurY][CurX] = 0;
				CurX = 0;
				CurY++;
			}
			MsgBuffer[CurY][CurX++] = *msg++;
			MsgBuffer[CurY][CurX++] = *msg++;
		}
		else {
			if (CurX >= STR_WIDTH) {
				MsgBuffer[CurY][CurX] = 0;
				CurX = 0;
				CurY++;
			}
			MsgBuffer[CurY][CurX++] = *msg++;
		}
	}
	if (CurX > 0 && CurY < MessageLine) {
		MsgBuffer[CurY][CurX] = 0;
		CurY++;
	}
	return CurY;
}

//
// ���ò˵���Ŀ
//
void CMainWin::SetMenuItem(const char *str, int anser)
{
	int		n = strlen(str);
	memcpy(MenuBuffer[MenuCount].text, str, n + 1);
	MenuBuffer[MenuCount].anser = anser;
	MenuBuffer[MenuCount].length = n;
	MenuBuffer[MenuCount].color = WhitePixel;
	MenuCount++;
}

//
// ���õ�ѡ�˵���Ŀ
//
void CMainWin::SetClickMenuItem(int r, int g, int b, int anser)
{
	ClickMenuBuffer[MenuCount].anser = anser;
	ClickMenuBuffer[MenuCount].color = RGB(r, g, b);
	MenuCount++;
}

//
// ������Ϸ
//
void CMainWin::LoadGame(int no)
{
	CParams param;
	if (!param.Load(no)) {
		MessageBox("�޷����롣");
		return;
	}

	CScriptAction *action = new CScriptAction(this, 0, CScriptAction::MODE_SCENARIO);
	if (action->Load(param.last_script)) {
		HideMessageWindow(false);
		HideMenuWindow(false);
		if (SaveShow) {
			SaveShow = FALSE;
			Invalidate(SaveRect);
		}
		Update();
		action->Setup(param);
		Param = param;

		// �趨����
		value_table.clear();
		for (int i=0; i<Param.nvalue; i++) {
			value_table[Param.value_tab[i].name] = Param.value_tab[i].value;
		}

		// ɾ��������Action)
		DeleteAllAction();
		Action = action;
		PostMessage(WM_KICKIDLE);
	}
}

//
// ������Ϸ
//
void CMainWin::SaveGame(int no, int flags)
{
	// �趨����
	Param.nvalue = value_table.size();
	int n = 0;
	for (valuemap::iterator p = value_table.begin(); p != value_table.end(); ++p) {
		strncpy(Param.value_tab[n].name, p->first.c_str(), 16);
		Param.value_tab[n].value = p->second;
		n++;
	}

	if (!Param.Save(no)) {
		MessageBox("�޷����档");
		return;
	}
	CancelLoadSaveMenu(flags);
}

//
// ���롤��ʾ����˵�
//
void CMainWin::ShowLoadSaveMenu(BOOL isSave)
{
	IsSaveMenu = isSave;
	SaveShow = TRUE;

	for (int i=0; i<PARAMS_MAX_SAVE; i++) {
		CParams	param;

		if (param.Load(i)) {
			DataTitle[i].activate = true;
			sprintf(DataTitle[i].title, "%2d: %2d/%2d %2d:%02d", i + 1,
				param.save_month, param.save_date,
				param.save_hour, param.save_minute);
		}
		else {
			DataTitle[i].activate = IsSaveMenu? true: false;
			sprintf(DataTitle[i].title, "%2d: -- no data --", i + 1);
		}
		DataTitle[i].color = DataTitle[i].activate? WhitePixel: GrayPixel;
	}

	Invalidate(SaveRect);
	if (TextShow)
		Invalidate(TextRect);
	if (MenuShow)
		Invalidate(MenuRect);

	Update();
}

//
// ���롤��ȥ����˵�
//
void CMainWin::HideLoadSaveMenu()
{
	SaveShow = FALSE;

	Invalidate(SaveRect);
	if (TextShow)
		Invalidate(TextRect);
	if (MenuShow)
		Invalidate(MenuRect);

	Update();
}

//
// ���롤�ظ����滭��
//
void CMainWin::CancelLoadSaveMenu(int flags)
{
	HideLoadSaveMenu();

	CAction *old = Action->GetOldAction();
	delete Action;
	Action = old;
	Action->Resume();

	if (flags & IS_TIMEDOUT)
		Action->TimedOut(TimerSleep);

	PostMessage(WM_KICKIDLE);
}

//
// �л������봢����Ŀ��ѡȡ/��ѡȡ
//
void CMainWin::SelectLoadSaveMenu(int index, bool select)
{
	if (index >= 0) {
		DataTitle[index].color = select? RedPixel: WhitePixel;
		int		y = index * SAVE_ITEM_INTERVAL + SAVE_ITEM_INTERVAL;
		CRect	rect(SAVE_X, SAVE_Y + y, SAVE_X + SAVE_W, SAVE_Y + y + SAVE_ITEM_HEIGHT);
		Mixing(rect, SaveItem(index));
		CopyAndRepaint(rect);
	}
}

//
// ȡ�������봢����Ŀ
//
// �ȼ�ಢ�У���λ�ÿ��ɼ���õ�
//
int CMainWin::GetLoadSaveSelect(CPoint point)
{
	if (point.x >= SAVE_X && point.x < SAVE_X + SAVE_W
	 && point.y >= SAVE_Y + SAVE_ITEM_INTERVAL) {
		int		index = point.y - SAVE_Y - SAVE_ITEM_INTERVAL;
		if (index % SAVE_ITEM_INTERVAL < SAVE_ITEM_HEIGHT) {
			index /= SAVE_ITEM_INTERVAL;
			if (index < PARAMS_MAX_SAVE && DataTitle[index].activate)
				return index;
		}
	}
	return -1;
}

//
// ȡ����һ�����롤������Ŀ
//
int CMainWin::NextLoadSaveSelect(int index)
{
	for (int i=1; i<=PARAMS_MAX_SAVE; i++) {
		int	next = (index + i) % PARAMS_MAX_SAVE;
		if (DataTitle[next].activate)
			return next;
	}
	return -1;
}

//
// ȡ��ǰһ�����롤������Ŀ
//
int CMainWin::PrevLoadSaveSelect(int index)
{
	for (int i=PARAMS_MAX_SAVE - 1; i>0; i--) {
		int	prev = (index + i) % PARAMS_MAX_SAVE;
		if (DataTitle[prev].activate)
			return prev;
	}
	return -1;
}

//
// ���ֲ���
//
BOOL CMainWin::StartMusic(int no)
{
	if (MusicNo != no) {
		MusicNo = no;
		if (music) {
			music->Stop();
			return music->Play(no);
		}
	}
	return TRUE;
}

//
// �������²���
//
BOOL CMainWin::RestartMusic()
{
	if (music)
		return music->Replay();
	return TRUE;
}

//
// ����ֹͣ
//
BOOL CMainWin::StopMusic()
{
	MusicNo = 0;
	if (music)
		return music->Stop();
	return TRUE;
}

//
// ����Wave
//
BOOL CMainWin::StartWave(const char *name)
{
	char	path[_MAX_PATH];
	sprintf(path, WAVEPATH "%s.wav", name);

	return wave.Play(path);
}

//
// ս��
//
BOOL CMainWin::Battle(const char *name)
{
	CBattleAction *action = new CBattleAction(this, Action);
	if (!action->Load(name)) {
		delete action;
		return false;
	}
	Action = action;
	PostMessage(WM_KICKIDLE);	// Ϊ�氲ȫ�����Ϳ��¼�
	return true;
}
