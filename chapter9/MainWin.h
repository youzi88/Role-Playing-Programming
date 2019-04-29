//
// MainWin--������
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
// ���������
//
class CMainWin: public CWindow {
  public:
	// ����

	// ����ģʽ
	enum	{
		MusicCD,
		MusicOff,
	} ;

	// ��ʱ��ID
	enum	{
		TimerSleep,
	} ;

	// �����е��¼����
	enum	{
		IS_TIMEDOUT = (1 << 0),
	} ;

	// ��Ŀ�����
	enum	{
		MAX_MENU_ITEM = 8,
		MAX_MENU_TEXT = 60,
		MAX_SAVE_TEXT = 62,
	} ;

  protected:
	// �¼���ʾλ��
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

	// �˵���ʾλ��
	enum	{
		MENU_X = 20,
		MENU_Y = MSG_Y - 2,		// �����ص����¼�����
		MENU_WIDTH = (MAX_MENU_TEXT + 2) * MessageFont / 2,
		MENU_HEIGHT = (MAX_MENU_ITEM + 1) * MessageFont,
		MENU_MIN_WIDTH = 50,
		MENU_FRAME_WIDTH = 10,
		MENU_FRAME_HEIGHT = 10,
		MENU_ITEM_SPACE = 2,
		MENU_ITEM_HEIGHT = MessageFont + MENU_ITEM_SPACE,
	} ;

	// ���棯����˵���ʾλ��
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

	// �˵���Ŀ
	struct CMenuItem {
		char		text[MAX_MENU_TEXT];
		int			anser;
		int			length;
		COLORREF	color;
	} ;

	// ����ϳ���Ŀ
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

	// ս��������
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
	// �������
	typedef map<string, int, ics_less>	valuemap;
	valuemap	value_table;

  public:
	int GetValue(string name) { return value_table[name]; }
	void SetValue(string name, int value) { value_table[name] = value; }
	void AddValue(string name, int value) { value_table[name] += value; }
	void DelValue(string name, int value) { value_table[name] -= value; }
	void ClearValue() { value_table.clear(); }

  protected:
	HFONT	hFont;					// �������ݡ��˵�������

	int	MusicMode;					// BGM����ģʽ

	CMci   	*music;					// ����BGM��װ��
	CDAudio	cdaudio;				// ����CD-DA�����
	int	MusicNo;					// �����е���������
	WaveOut	wave;					// ����Wave

	CParams	Param;					// ����
	CAction *Action;				// ���ڵ�Action

	CDrawImage	ViewImage;			// ��ʾ
	CDrawImage	MixedImage;			// �ϳɽ��
	CImage		BackLayer;			// ����
	CImage		OverlapLayer;		// �ص�
	BOOL		TextDisplay;		// ��ʾ�������ݣ�
	BOOL		WaitMarkShowing;	// ��ʾ�ȴ��Ǻţ�

	CRect		InvalidRect;		// ��Ч����
	CRect		TextRect;			// ������������
	CRect		WaitMarkRect;		// �ȴ��Ǻ�����
	CRect		MenuRect;			// �˵�����
	BOOL		BackShow;			// ��ʾ������
	BOOL		OverlapShow;		// ��ʾ�ص���
	BOOL		TextShow;			// ��ʾ�������ݣ�
	BOOL		MenuShow;			// ��ʾ�˵���
	BOOL		SaveShow;			// ��ʾ���棯���룿
	CRect		SaveRect;			// ���棯��������
	COLORREF	BgColor;			// �ޱ���ʱ��������ɫ

	CViewEffect *ViewEffect;		// Ч��
	UINT		TimePeriod;			// ��ʱ���Ľ�������

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

// inline��Ա��ʽ

//
// ��ʾ���ڴ��ϵ�ͼ��
//
inline void CMainWin::Repaint(const CRect &rect)
{
	ViewImage.Draw(CClientDC(this), rect);
}

//
// �Ӻϳ��õ�ͼ���Ƶ���ʾ�õ�ͼ��
//
inline void CMainWin::CopyAndRepaint(const CRect &rect)
{
	ViewImage.Copy(&MixedImage, rect);
	Repaint(rect);
}

//
// ��������λ��ȡ�ò˵���Ŀ
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
// �����ָ��λ��ȡ�õ�ѡ�˵�����Ŀ
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
// ��Чֹͣ
//
inline void CMainWin::StopWipe()
{
	delete ViewEffect;
	ViewEffect = 0;
}

//
// ����������?
//
inline BOOL CMainWin::IsLoadOK() const
{
	return Action->IsSaveLoadOK();
}

//
// ���Դ�����?
//
inline BOOL CMainWin::IsSaveOK() const
{
	return Action->IsSaveLoadOK();
}

#endif
