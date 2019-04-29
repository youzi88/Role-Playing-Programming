//
// Main window--主窗口
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
#include "Params.h"

//
// 主窗口类别
//
class CMainWin: public CWindow {
  public:
	CMainWin();
	~CMainWin();

	LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnIdle(long count);

	void OnCommand(UINT notifyCode, UINT id, HWND ctrl);

	void DeleteAllAction();

	// 战斗画面使用
	BOOL Battle();

	CDrawImage *GetViewImage() { return &ViewImage; }
	CParams &GetParam() { return Param; }

  protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

	BOOL OnCreate(CREATESTRUCT *cs);
	void OnFirstAction();
	void OnClose();
	void OnPaint();
	void OnTimer(int id);

  protected:
	CParams		Param;					// 参数
	CAction    *Action;					// 目前的Action

	CDrawImage	ViewImage;				// 显示

	UINT		TimePeriod;				// Timer的分辨率
} ;

#endif
