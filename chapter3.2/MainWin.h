//
// Script player main window
//
// Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __MainWin_h__
#define __MainWin_h__

#include "Window.h"
#include "dc.h"
#include "Image.h"
#include "DrawImage.h"

//
// ÷˜“ï¥∞ÓêÑe
//
class CMainWin: public CWindow {
public:
	LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void OnCommand(UINT notifyCode, UINT id, HWND ctrl);
	
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	
	BOOL OnCreate(CREATESTRUCT *cs);
	void OnClose();
	void OnPaint();
	
protected:
	CDrawImage ViewImage; // Ô@ æ
	CImage Back; // ±≥æ∞
	CImage Overlap; // ÷ÿØB
} ;

#endif
