//
//	Script player main window
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __MainWin_h__
#define __MainWin_h__

#include "Window.h"
#include "dc.h"
#include "Image.h"
#include "DrawImage.h"
#include "Sprite.h"

//
//	���������
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
	CDrawImage	ViewImage;				// ��ʾ
	CImage		Back;					// ����
	CImage		SpriteImage;			// ��ͼ�����ͼ��
	CSprite		Sprite[4];				// ��ͼ���(sprite)
} ;

#endif
