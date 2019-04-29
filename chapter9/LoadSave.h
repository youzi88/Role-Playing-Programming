//
// 载入、储存动作
//
//	Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__loadsave_h__
#define	__loadsave_h__

#include "Action.h"

//
// 操作载入或储存游戏进度的画面的类别
//
class CGameLoadSaveAction: public CAction {
  public:
	CGameLoadSaveAction(CMainWin *parent, CAction *oldAction);

	virtual	void LButtonDown(UINT modKeys, CPoint point);
	virtual	void LButtonUp(UINT modKeys, CPoint point);
	virtual	void RButtonDown(UINT modKeys, CPoint point);
	virtual	void RButtonUp(UINT modKeys, CPoint point);
	virtual	void MouseMove(UINT modKeys, CPoint point);
	virtual	void KeyDown(UINT key);
	virtual bool TimedOut(int timerId);

  protected:
	virtual void DoLoadSave() = 0;

	int		Selection;
	BOOL	Pressed;
	BOOL	CancelPressed;
	int		Flags;
} ;

class CGameLoadAction: public CGameLoadSaveAction {
  public:
	CGameLoadAction(CMainWin *parent, CAction *oldAction)
		: CGameLoadSaveAction(parent, oldAction)
	{
	}

  protected:
	virtual void DoLoadSave();
} ;

class CGameSaveAction: public CGameLoadSaveAction {
  public:
	CGameSaveAction(CMainWin *parent, CAction *oldAction)
		: CGameLoadSaveAction(parent, oldAction)
	{
	}

  protected:
	virtual void DoLoadSave();
} ;

#endif
