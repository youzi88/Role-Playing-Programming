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
// 构造函数
//
//	变量的初始化
//	当window由OnCreate初始化时，不会带出任何东西
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

	Action = new CAction(this);	// 虚拟动作（dummy action）
	hFont = 0;

	// 开始播放CD
	// 若没有CD则在Open时设成Off
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
// 析构函数
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

	  case WM_KEYDOWN:			// 按下键盘
		Action->KeyDown(wParam);
		break;

	  case WM_DESTROY:			// 取消窗口
		OnDestroy();
		break;

	  case WM_TIMER:			// 计时器到期
		OnTimer(wParam);
		break;

	  case MM_MCINOTIFY:		// MCI的事件
		OnMciNotify(wParam, LOWORD(lParam));
		break;

	  default:
		return CWindow::WindowProc(uMsg, wParam, lParam);
	}
	return 0L;
}

//
// IDLE的处理
//
BOOL CMainWin::OnIdle(long count)
{
	if (ViewEffect) {							// 是否在特效中?
		if (ViewEffect->Step(timeGetTime()))	// 特效执行
			return TRUE;						// 特效继续
		StopWipe();								// 特效结束
		Action->WipeDone();						// 结束通报给动作
	}
	return Action->IdleAction();
}

//
// WM_CREATE的处理
//
BOOL CMainWin::OnCreate(CREATESTRUCT *cs)
{
	LoadAccelTable(IDC_APPACCEL);

	CClientDC	dc(this);

	// 确认图像内存空间
	if (!ViewImage.Create(dc, WindowWidth, WindowHeight)
	 || !MixedImage.Create(dc, WindowWidth, WindowHeight)
	 || !BackLayer.Create(WindowWidth, WindowHeight)
	 || !OverlapLayer.Create(WindowWidth, WindowHeight)) {
		MessageBox("内存不足。\n"
			"请关闭其他应用程序再启动。");
		return FALSE;
	}

	// 清除显示用图像
	ViewImage.Clear();

	// 先行产生字型
	if ((hFont = CreateFont(-MessageFont, 0, 0, 0, MessageStyle, FALSE, FALSE, FALSE,
		GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "宋体")) == 0) {
		MessageBox("没有宋体!");
		return FALSE;
	}

	// 发送触发的第一个事件
	while (!PostMessage(WM_FIRSTACTION)) {
		// 若伫列已满，则PostMessage会发生error，故反覆传送
#ifdef	_DEBUG
		TRACE("PostMessage Error code = %d\n", GetLastError());
#endif
		::Sleep(110);	// 重送事件的动作再稍等一下
	}

	Param.Clear();		// 清除参数内的东西

	return TRUE;
}

//
// 游戏的“第一个”动作
//
void CMainWin::OnFirstAction()
{
	// 如果需要，CD-DA会开启
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
// WM_DESTROY的处理(重新安排)
//
void CMainWin::OnDestroy()
{
	// 如果正在播放音乐就停止，
	// 若是用MCI，应用程序虽然结束但播放仍然会继续，
	// 所以要是窗口被关掉的时候就强制停止。

	if (music) {
		music->Stop();
		music->Close();
		music = 0;
	}
	CWindow::OnDestroy();
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

	  case ID_MUSIC_CD:				// BGM/ON
		ChangeMusicMode(MusicCD);
		break;

	  case ID_MUSIC_OFF:			// BGM/OFF
		ChangeMusicMode(MusicOff);
		break;

	  case ID_LOADGAME:				// 载入
		if (IsLoadOK())
			SetGameLoadAction();
		break;

	  case ID_SAVEGAME:				// 储存
		if (IsSaveOK())
			SetGameSaveAction();
		break;

	  case ID_STOPSCRIPT:			// 脚本中断
		if (IsSaveOK()
		 && MessageBox("要中断故事，对吗?", ApplicationTitle,
		 	MB_ICONQUESTION|MB_OKCANCEL) == IDOK)
			Action->Abort();
		break;

	  default:
		break;
	}
}

//
// 菜单项目的初始化
//
void CMainWin::OnInitSubMenu(HMENU hMenu, UINT id)
{
	switch (id) {
	  case ID_MUSIC_CD:				// BGM/ON
		// 检查菜单项目
		CheckMenuItem(hMenu, id, MusicMode == MusicCD? MF_CHECKED: MF_UNCHECKED);
		break;

	  case ID_MUSIC_OFF:			// BGM/OFF
		// 检查菜单项目
		CheckMenuItem(hMenu, id, MusicMode == MusicOff? MF_CHECKED: MF_UNCHECKED);
		break;

	  case ID_LOADGAME:				// 载入
		// 只有在可以载入时菜单上的项目才有作用
		EnableMenuItem(hMenu, id, IsLoadOK()? MF_ENABLED: (MF_DISABLED | MF_GRAYED));
		break;

	  case ID_SAVEGAME:				// 储存
		// 只有在可以储存时菜单上的项目才有作用
		EnableMenuItem(hMenu, id, IsSaveOK()? MF_ENABLED: (MF_DISABLED | MF_GRAYED));
		break;

	  case ID_STOPSCRIPT:			// 脚本中断
		// 只有在可以中断时菜单上的项目才有作用
		EnableMenuItem(hMenu, id, IsSaveOK()? MF_ENABLED: (MF_DISABLED | MF_GRAYED));
		break;
	}
}

//
// MCI发出的事件
//
// 因为设定是用play来做call back，所以在播放结束时调用
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
// 音乐播放换成新的
//
void CMainWin::ChangeMusicMode(int mode)
{
	if (MusicMode != mode) {		// 若变更模式的话
		MusicMode = mode;
		if (music) {				// 播放中就停止
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
			music->Play(MusicNo);	// 播放现在所指定的曲子
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
// Script结束
//
void CMainWin::ScriptDone()
{
	StopWipe();
	StopMusic();	// 停止音乐播放

	CAction *action = Action->GetOldAction();
	delete Action;
	Action = action;
	if (Action == 0)		// 没有被储存的动作
		StartMainMenu();	// 回到主菜单
}

//
// 切换到载入画面操作
//
void CMainWin::SetGameLoadAction()
{
	ShowLoadSaveMenu(FALSE);
	Action->Pause();
	Action = new CGameLoadAction(this, Action);
	PostMessage(WM_KICKIDLE);	// 为了慎重起见发送空事件
}

//
// 切换的储存画面操作
//
void CMainWin::SetGameSaveAction()
{
	ShowLoadSaveMenu(TRUE);
	Action->Pause();
	Action = new CGameSaveAction(this, Action);
	PostMessage(WM_KICKIDLE);	// 为了慎重起见发送空事件
}

//
// 执行Script
//
bool CMainWin::StartScript(const char *name, int mode)
{
	CScriptAction *action = new CScriptAction(this, Action, mode);
	if (!action->Load(name)) {			// 读入脚本
		delete action;
		return false;
	}

	Action = action;
	PostMessage(WM_KICKIDLE);	// 为了慎重起见发送空事件
	return true;
}

//
// 主菜单
//
// 执行主菜单Script
//
void CMainWin::StartMainMenu()
{
	DeleteAllAction();
	if (!StartScript("main", CScriptAction::MODE_SYSTEM))
		::DestroyWindow(hWnd);
}

//
// 在事件范围中写内容
//
void CMainWin::WriteMessage(const char *msg)
{
	FormatMessage(msg);

	WaitMarkShowing = TRUE;

	ShowMessageWindow();
}

//
// 消除“等待按键”符号
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
// 描绘菜单
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
// 选取/非选取显示菜单项目
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
// 显示事件范围
//
void CMainWin::ShowMessageWindow()
{
	TextDisplay = TRUE;

	TextShow = TRUE;
	Invalidate(TextRect);
	Update();
}

//
// 隐藏事件范围
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
// 选择/不选择显示事件范围
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
// 隐藏菜单
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
// 画面的全部设定成不显示
//
// pix 是填满画面的颜色
//
void CMainWin::HideAllLayer(COLORREF pix)
{
	BgColor = pix;
	BackShow = FALSE;
	OverlapShow = FALSE;
}

//
// 装配显示的内容
//
void CMainWin::Mixing(const CRect &rect, unsigned flags)
{
	// 背景
	if (BackShow)
		MixedImage.Copy(&BackLayer, rect);		// 有背景的话就复制
	else
		MixedImage.FillRect(rect, BgColor);		// 没有背景的话就涂满

	// 重叠
	if (OverlapShow)							// 有重叠的话就合成
		MixedImage.MixImage(&OverlapLayer, rect);

	// 储存与载入菜单
	if (SaveShow) {
		if (flags & SaveTitle) {
			MixedImage.DrawFrameRect(SAVE_X, SAVE_Y, SAVE_TITLE_WIDTH, SAVE_ITEM_HEIGHT);
			MixedImage.DrawText(hFont, SAVE_TEXT_OFFSET_X, SAVE_TEXT_OFFSET_Y,
				IsSaveMenu? "储存": "载入");
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
		// 事件范围
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
			// 等待按键的符号（使用▼、挪用DrawText）
			if (WaitMarkShowing && flags & TextWaitMark)
				MixedImage.DrawText(hFont, MsgX(WAITMARK_X), MsgY(WAITMARK_Y), "▼");
		}
		// 菜单
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
// 若画面有变化时则重新绘制
//
BOOL CMainWin::Update(bool repaint)
{
	if (!InvalidRect.IsRectEmpty()) {			// 有无效区域
		Mixing(InvalidRect);					// 合成
		if (repaint)
			CopyAndRepaint(InvalidRect);		// 重新绘制
		InvalidRect.SetRectEmpty();				// 将无效区域设为“无”
		return TRUE;							// 已Update
	}
	return FALSE;								// 什么都不做
}

//
// 读取背景CG
//
BOOL CMainWin::LoadImageBack(const char *name)
{
	BackShow = TRUE;
	Invalidate();
	return BackLayer.LoadImage(name);
}

//
// 读取重叠CG
//
BOOL CMainWin::LoadImageOverlap(const char *name)
{
	OverlapShow = TRUE;
	Invalidate();
	return OverlapLayer.LoadImage(name);
}

//
// 读取作业用的CG
//
BOOL CMainWin::LoadImageWork(const char *name)
{
	OverlapShow = FALSE;
	return OverlapLayer.LoadImage(name);
}

//
// 消去背景CG
//
BOOL CMainWin::ClearBack()
{
	BackShow = FALSE;
	Invalidate();
	return TRUE;
}

//
// 消去重叠CG
//
// 实际上只有变更显示状况
//
BOOL CMainWin::ClearOverlap()
{
	OverlapShow = FALSE;
	Invalidate();
	return TRUE;
}

//
// 判定是否为禁用的文字(字符)
//
// 这里处理的是行尾最好禁用的字符部分
// 这里和系统中使用的字符有关
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
// 清除内文缓冲装置（Text Buffer）
//
void CMainWin::ClearMessage()
{
	HideMessageWindow();

	CurX = CurY = 0;

	for (int i=0; i<MessageLine; i++)
		MsgBuffer[i][0] = 0;
}

//
// 在内文缓冲装置中格式化字符串后再复制
//
// 因为正在处理禁用条例、限定日文
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
// 设置菜单项目
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
// 设置点选菜单项目
//
void CMainWin::SetClickMenuItem(int r, int g, int b, int anser)
{
	ClickMenuBuffer[MenuCount].anser = anser;
	ClickMenuBuffer[MenuCount].color = RGB(r, g, b);
	MenuCount++;
}

//
// 载入游戏
//
void CMainWin::LoadGame(int no)
{
	CParams param;
	if (!param.Load(no)) {
		MessageBox("无法载入。");
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

		// 设定变量
		value_table.clear();
		for (int i=0; i<Param.nvalue; i++) {
			value_table[Param.value_tab[i].name] = Param.value_tab[i].value;
		}

		// 删除动作（Action)
		DeleteAllAction();
		Action = action;
		PostMessage(WM_KICKIDLE);
	}
}

//
// 储存游戏
//
void CMainWin::SaveGame(int no, int flags)
{
	// 设定变量
	Param.nvalue = value_table.size();
	int n = 0;
	for (valuemap::iterator p = value_table.begin(); p != value_table.end(); ++p) {
		strncpy(Param.value_tab[n].name, p->first.c_str(), 16);
		Param.value_tab[n].value = p->second;
		n++;
	}

	if (!Param.Save(no)) {
		MessageBox("无法储存。");
		return;
	}
	CancelLoadSaveMenu(flags);
}

//
// 载入·显示储存菜单
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
// 载入·消去储存菜单
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
// 载入·回复储存画面
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
// 切换载入与储存项目的选取/非选取
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
// 取得载入与储存项目
//
// 等间距并列，其位置可由计算得到
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
// 取得下一项载入·储存项目
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
// 取得前一项载入·储存项目
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
// 音乐播放
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
// 音乐重新播放
//
BOOL CMainWin::RestartMusic()
{
	if (music)
		return music->Replay();
	return TRUE;
}

//
// 音乐停止
//
BOOL CMainWin::StopMusic()
{
	MusicNo = 0;
	if (music)
		return music->Stop();
	return TRUE;
}

//
// 播放Wave
//
BOOL CMainWin::StartWave(const char *name)
{
	char	path[_MAX_PATH];
	sprintf(path, WAVEPATH "%s.wav", name);

	return wave.Play(path);
}

//
// 战斗
//
BOOL CMainWin::Battle(const char *name)
{
	CBattleAction *action = new CBattleAction(this, Action);
	if (!action->Load(name)) {
		delete action;
		return false;
	}
	Action = action;
	PostMessage(WM_KICKIDLE);	// 为存安全，发送空事件
	return true;
}
