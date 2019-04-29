//
// Main window--������
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

//
// ���������
//
class CMainWin: public CWindow {
  public:
	CMainWin();
	~CMainWin();

	LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnCommand(UINT notifyCode, UINT id, HWND ctrl);

	void DeleteAllAction();

	// ս������ʹ��
	BOOL Battle();

	CDrawImage *GetViewImage() { return &ViewImage; }

  protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

	BOOL OnCreate(CREATESTRUCT *cs);
	void OnFirstAction();
	void OnClose();
	void OnPaint();
	void OnTimer(int id);

  protected:
	CAction    *Action;					// Ŀǰ��Action

	CDrawImage	ViewImage;				// ��ʾ

	UINT		TimePeriod;				// Timer�ķֱ���
} ;

#endif
