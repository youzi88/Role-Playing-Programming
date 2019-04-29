//
// MainWin--主窗口
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __MainWin_h__
#define __MainWin_h__

#include "Window.h"
#include "dc.h"
#include "Action.h"
#include "Image.h"
#include "DrawImage.h"
#include "Effect.h"
#include "Mci.h"
#include "Params.h"
#include "less.h"

//
// 主窗口类别
//
class CMainWin: public CWindow {
  public:
	// 常数

	// 音乐模式
	enum	{
		MusicCD,
		MusicOff,
	} ;

	// 计时器ID
	enum	{
		TimerSleep,
	} ;

	// 待机中的事件旗标
	enum	{
		IS_TIMEDOUT = (1 << 0),
	} ;

	// 项目最大数
	enum	{
		MAX_MENU_ITEM = 8,
		MAX_MENU_TEXT = 60,
		MAX_SAVE_TEXT = 62,
	} ;

  protected:
	// 事件显示位置
	enum	{
		MSG_W = (MessageFont + 1) * MessageWidth / 2 + 16,
		MSG_H = (MessageFont + 2) * MessageLine + 14,
		MSG_X = (WindowWidth - MSG_W) / 2,
		MSG_Y = WindowHeight - MSG_H - 8,
		MSG_TEXTOFFSET_X = MSG_X + 8,
		MSG_TEXTOFFSET_Y = MSG_Y + 7,
		MSG_TEXTSPACE_Y = 2,
		WAITMARK_X = MessageWidth - 2,
		WAITMARK_Y = MessageLine - 1,
	} ;

	// 菜单显示位置
	enum	{
		MENU_X = 20,
		MENU_Y = MSG_Y - 2,		// 不可重叠到事件区域
		MENU_WIDTH = (MAX_MENU_TEXT + 2) * MessageFont / 2,
		MENU_HEIGHT = (MAX_MENU_ITEM + 1) * MessageFont,
		MENU_MIN_WIDTH = 50,
		MENU_FRAME_WIDTH = 10,
		MENU_FRAME_HEIGHT = 10,
		MENU_ITEM_SPACE = 2,
		MENU_ITEM_HEIGHT = MessageFont + MENU_ITEM_SPACE,
	} ;

	// 储存／载入菜单显示位置
	enum	{
		SAVE_ITEM_HEIGHT = 32,
		SAVE_ITEM_SPACE = 4,
		SAVE_ITEM_INTERVAL = SAVE_ITEM_HEIGHT + SAVE_ITEM_SPACE,
		SAVE_W = 400,
		SAVE_H = SAVE_ITEM_INTERVAL * PARAMS_MAX_SAVE + SAVE_ITEM_HEIGHT,
		SAVE_X = (WindowWidth - SAVE_W) / 2,
		SAVE_Y = (WindowHeight - SAVE_H) / 2,
		SAVE_TEXT_OFFSET_X = SAVE_X + 10,
		SAVE_TEXT_OFFSET_Y = SAVE_Y + 8,
		SAVE_TITLE_WIDTH = 72,
	} ;

	// 菜单项目
	struct CMenuItem {
		char		text[MAX_MENU_TEXT];
		int			anser;
		int			length;
		COLORREF	color;
	} ;

	// 画面合成项目
	enum	{
		TextMessage = 1 << 0,
		TextWaitMark = 1 << 1,
		MenuFrame = 1 << 2,
		SaveTitle = 1 << 3,
		MenuItemFirst = 4,
		SaveItemFirst = 12,
	} ;
	static unsigned MenuItem(int n) { return 1 << (MenuItemFirst + n); }
	static unsigned SaveItem(int n) { return 1 << (SaveItemFirst + n); }

  public:
	CMainWin();
	~CMainWin();

	LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void OnInitSubMenu(HMENU hMenu, UINT id);
	virtual BOOL OnIdle(long count);

	void OnCommand(UINT notifyCode, UINT id, HWND ctrl);

	void DeleteAllAction();
	void ScriptDone();
	void SetGameLoadAction();
	void SetGameSaveAction();

	bool StartScript(const char *name, int mode=0);
	void StartMainMenu();

	void WriteMessage(const char *msg);
	void ClearMessage();
	void HideWaitMark();
	void ShowMessageWindow();
	void HideMessageWindow(bool update=true);
	void FlipMessageWindow();

	void SetMenuItem(const char *lavel, int anser);
	void OpenMenu();
	void HideMenuWindow(bool update=true);
	void ClearMenuItemCount() { MenuCount = 0; }
	int GetMenuItemCount() const { return MenuCount; }
	int GetMenuAnser(int index) const { return MenuBuffer[index].anser; }
	int GetMenuSelect(CPoint point);
	void SelectMenu(int index, bool select);

	void SetClickMenuItem(int r, int g, int b, int anser);
	int GetClickMenuSelect(CPoint point);

	BOOL LoadImageBack(const char *name);
	BOOL LoadImageOverlap(const char *name);
	BOOL LoadImageWork(const char *name);
	BOOL ClearBack();
	BOOL ClearOverlap();

	const CRect &GetInvalidRect() const { return InvalidRect; }
	void Invalidate(const CRect &rect) { InvalidRect |= rect; }
	void Invalidate() { InvalidRect.SetRect(0, 0, WindowWidth, WindowHeight); }

	void Repaint(const CRect &rect);
	void CopyAndRepaint(const CRect &rect);

	BOOL StartMusic(int no);
	BOOL RestartMusic();
	BOOL StopMusic();
	BOOL StartWave(const char *name);

	void ShowLoadSaveMenu(BOOL isSave);
	void CancelLoadSaveMenu(int flags);
	int GetLoadSaveSelect(CPoint point);
	int NextLoadSaveSelect(int index);
	int PrevLoadSaveSelect(int index);
	void SelectLoadSaveMenu(int index, bool select);

	void WipeIn();
	void WipeOut();
	void FadeIn();
	void FadeOut();
	void CutIn();
	void CutOut(BOOL white=FALSE);
	void WhiteIn();
	void WhiteOut();
	void CutIn(const CRect &rect);
	void WipeIn(const CRect &rect);
	void MixFade(const CRect &rect);
	void Flash();
	void Shake();

	void LoadGame(int no);
	void SaveGame(int no, int flags);
	CParams &GetParam() { return Param; }

	// 战斗画面用
	BOOL Battle(const char *name);

	CDrawImage *GetViewImage() { return &ViewImage; }
	CDrawImage *GetMixedImage() { return &MixedImage; }
	CImage     *GetBackLayer() { return &BackLayer; }
	HFONT GetFont() const { return hFont; }

  protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

	BOOL OnCreate(CREATESTRUCT *cs);
	void OnFirstAction();
	void OnClose();
	void OnPaint();
	void OnDestroy();
	void OnTimer(int id);

	void OnMciNotify(unsigned flag, unsigned id);

	void ChangeMusicMode(int mode);

	BOOL IsLoadOK() const;
	BOOL IsSaveOK() const;

	BOOL Update(bool repaint=true);

	void HideAllLayer(COLORREF pix);

	int MsgX(int x) const { return x * (MessageFont + 1) / 2 + MSG_TEXTOFFSET_X; }
	int MsgY(int y) const { return y * (MessageFont + MSG_TEXTSPACE_Y) + MSG_TEXTOFFSET_Y; }
	BOOL Kinsoku(const char *p);

	void Mixing(const CRect &rect, unsigned flags = ~0);

	int FormatMessage(const char *msg);

	void ShowLoadSaveMenu(BOOL isSave, CRect *rect);
	void HideLoadSaveMenu();

	void StopWipe();

  protected:
	// 变量表格
	typedef map<string, int, ics_less>	valuemap;
	valuemap	value_table;

  public:
	int GetValue(string name) { return value_table[name]; }
	void SetValue(string name, int value) { value_table[name] = value; }
	void AddValue(string name, int value) { value_table[name] += value; }
	void DelValue(string name, int value) { value_table[name] -= value; }
	void ClearValue() { value_table.clear(); }

  protected:
	HFONT	hFont;					// 文字内容、菜单的字型

	int	MusicMode;					// BGM播放模式

	CMci   	*music;					// 播放BGM的装置
	CDAudio	cdaudio;				// 播放CD-DA的类别
	int	MusicNo;					// 播缝中的乐曲号码
	WaveOut	wave;					// 播放Wave

	CParams	Param;					// 参数
	CAction *Action;				// 现在的Action

	CDrawImage	ViewImage;			// 显示
	CDrawImage	MixedImage;			// 合成结果
	CImage		BackLayer;			// 背景
	CImage		OverlapLayer;		// 重叠
	BOOL		TextDisplay;		// 显示文字内容？
	BOOL		WaitMarkShowing;	// 显示等待记号？

	CRect		InvalidRect;		// 无效区域
	CRect		TextRect;			// 文字内容区域
	CRect		WaitMarkRect;		// 等待记号区域
	CRect		MenuRect;			// 菜单区域
	BOOL		BackShow;			// 显示背景？
	BOOL		OverlapShow;		// 显示重叠？
	BOOL		TextShow;			// 显示文字内容？
	BOOL		MenuShow;			// 显示菜单？
	BOOL		SaveShow;			// 显示储存／载入？
	CRect		SaveRect;			// 储存／载入区域
	COLORREF	BgColor;			// 无背景时的填满颜色

	CViewEffect *ViewEffect;		// 效果
	UINT		TimePeriod;			// 计时器的解译能力

	char	MsgBuffer[MessageLine][MessageWidth + 1];
	int		CurX;
	int		CurY;

	CMenuItem	MenuBuffer[MAX_MENU_ITEM];
	int			MenuCount;
	struct	{
		COLORREF color;
		int anser;
	} ClickMenuBuffer[MAX_MENU_ITEM];

	BOOL	IsSaveMenu;
	struct	{
		bool		activate;
		char		title[MAX_SAVE_TEXT + 1];
		COLORREF	color;
	} DataTitle[PARAMS_MAX_SAVE];

	static const CRect Position[];
} ;

// inline成员函式

//
// 显示在内存上的图像
//
inline void CMainWin::Repaint(const CRect &rect)
{
	ViewImage.Draw(CClientDC(this), rect);
}

//
// 从合成用的图像复制到显示用的图像
//
inline void CMainWin::CopyAndRepaint(const CRect &rect)
{
	ViewImage.Copy(&MixedImage, rect);
	Repaint(rect);
}

//
// 从鼠标键的位置取得菜单项目
//
inline int CMainWin::GetMenuSelect(CPoint point)
{
	if (point.x < MenuRect.left + MENU_FRAME_WIDTH
	 || point.y < MenuRect.top + MENU_FRAME_HEIGHT
	 || point.x >= MenuRect.right - MENU_FRAME_WIDTH
	 || point.y >= MenuRect.bottom - MENU_FRAME_HEIGHT)
		return -1;
	return (point.y - MenuRect.top - MENU_FRAME_WIDTH) / MENU_ITEM_HEIGHT;
}

//
// 从鼠标指标位置取得点选菜单的项目
//
inline int CMainWin::GetClickMenuSelect(CPoint point)
{
	COLORREF color = OverlapLayer.GetPixel(point);
	for (int i=0; i<MenuCount; i++) {
		if (ClickMenuBuffer[i].color == color)
			return  ClickMenuBuffer[i].anser;
	}
	return -1;
}

//
// Wipe in
//
inline void CMainWin::WipeIn(const CRect &rect)
{
	Update(false);
	ViewEffect = new EffectWipeIn(this, &ViewImage, &MixedImage, rect);
}

//
// Wipe in
//
inline void CMainWin::WipeIn()
{
	WipeIn(CRect(0, 0, WindowWidth, WindowHeight));
}

//
// Wipe out
//
inline void CMainWin::WipeOut()
{
	HideMessageWindow();
	ViewEffect = new EffectWipeOut(this, &ViewImage, &MixedImage);
	HideAllLayer(BlackPixel);
}

//
// Fade in
//
inline void CMainWin::FadeIn()
{
	Update(false);
	ViewEffect = new EffectFadeIn(this, &ViewImage, &MixedImage);
}

//
// Fade out
//
inline void CMainWin::FadeOut()
{
	HideMessageWindow();
	ViewEffect = new EffectFadeOut(this, &ViewImage, &MixedImage);
	HideAllLayer(BlackPixel);
}

//
// Cut in
//
inline void CMainWin::CutIn(const CRect &rect)
{
	Update(false);
	CopyAndRepaint(rect);
}

//
// Cut in
//
inline void CMainWin::CutIn()
{
	CutIn(CRect(0, 0, WindowWidth, WindowHeight));
}

//
// Cut out
//
inline void CMainWin::CutOut(BOOL white)
{
	HideMessageWindow();
	HideAllLayer(white? WhitePixel: BlackPixel);
	Invalidate();
	Update();
}

//
// White in
//
inline void CMainWin::WhiteIn()
{
	Update(false);
	ViewEffect = new EffectWhiteIn(this, &ViewImage, &MixedImage);
}

//
// White out
//
inline void CMainWin::WhiteOut()
{
	HideMessageWindow();
	ViewEffect = new EffectWhiteOut(this, &ViewImage, &MixedImage);
	HideAllLayer(WhitePixel);
}

//
// Mix Fade
//
inline void CMainWin::MixFade(const CRect &rect)
{
	Update(false);
	ViewEffect = new EffectMixFade(this, &ViewImage, &MixedImage, rect);
}

//
// Shake
//
inline void CMainWin::Shake()
{
	ViewEffect = new EffectShake(this, &ViewImage);
}

//
// Flash
//
inline void CMainWin::Flash()
{
	ViewEffect = new EffectFlash(this, &ViewImage);
}

//
// 特效停止
//
inline void CMainWin::StopWipe()
{
	delete ViewEffect;
	ViewEffect = 0;
}

//
// 可以载入吗?
//
inline BOOL CMainWin::IsLoadOK() const
{
	return Action->IsSaveLoadOK();
}

//
// 可以储存吗?
//
inline BOOL CMainWin::IsSaveOK() const
{
	return Action->IsSaveLoadOK();
}

#endif
