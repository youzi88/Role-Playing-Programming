//
// 实例的主窗口
//
// Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __MainWin_h__
#define __MainWin_h__

#include "Window.h"
#include "dib.h"
#include "dc.h"

//
// MainWin 类别
//
class CMainWin: public CWindow {
public:
LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

void OnCommand(UINT notifyCode, UINT id, HWND ctrl);

protected:
virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

BOOL OnCreate(CREATESTRUCT *cs);
void OnPaint();

protected:
CDib ViewImage; // 显示
} ;

#endif
