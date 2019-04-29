//
// 载入、储存动作
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "MainWin.h"
#include "LoadSave.h"

//
// 初始化
//
CGameLoadSaveAction::CGameLoadSaveAction(CMainWin *parent, CAction *oldAction)
	:CAction(parent, oldAction)
{
	Selection = -1;
	Pressed = FALSE;
	CancelPressed = FALSE;
	Flags = 0;
}

//
// 按下鼠标左键时的处理
//
void CGameLoadSaveAction::LButtonDown(UINT modKeys, CPoint point)
{
	Pressed = TRUE;
}

//
// 放开鼠标左键时的处理
//
void CGameLoadSaveAction::LButtonUp(UINT modKeys, CPoint point)
{
	Pressed = FALSE;
	if (Selection >= 0)
		DoLoadSave();
}

//
// 按下鼠标右键时的处理
//
void CGameLoadSaveAction::RButtonDown(UINT modKeys, CPoint point)
{
	CancelPressed = TRUE;
}

//
// 放开鼠标右键时的处理
//
void CGameLoadSaveAction::RButtonUp(UINT modKeys, CPoint point)
{
	if (CancelPressed)
		Parent->CancelLoadSaveMenu(Flags);
}

//
// 鼠标指标移动时的处理
//
void CGameLoadSaveAction::MouseMove(UINT modKeys, CPoint point)
{
	int sel = Parent->GetLoadSaveSelect(point);
	if (sel != Selection) {
		Parent->SelectLoadSaveMenu(Selection, false);
		Selection = sel;
		Parent->SelectLoadSaveMenu(Selection, true);
	}
}

//
// 按下键盘时的处理
//
void CGameLoadSaveAction::KeyDown(UINT key)
{
	switch (key) {
	  case VK_RETURN:
	  case VK_SPACE:	// 执行载入／储存
		if (Selection >= 0)
			DoLoadSave();
		break;

	  case VK_ESCAPE:	// 取消
		Parent->CancelLoadSaveMenu(Flags);
		break;

	  case VK_UP:		// 选择前一项
		{
			int		sel = Parent->PrevLoadSaveSelect(Selection);
			if (sel != Selection) {
				Parent->SelectLoadSaveMenu(Selection, false);
				Selection = sel;
				Parent->SelectLoadSaveMenu(Selection, true);
			}
		}
		break;

	  case VK_DOWN:		// 选择后一项
		{
			int		sel = Parent->NextLoadSaveSelect(Selection);
			if (sel != Selection) {
				Parent->SelectLoadSaveMenu(Selection, false);
				Selection = sel;
				Parent->SelectLoadSaveMenu(Selection, true);
			}
		}
		break;
	}
}

//
// 计时器的处理
//
bool CGameLoadSaveAction::TimedOut(int timerId)
{
	switch (timerId) {
	  case CMainWin::TimerSleep:
		Flags |= CMainWin::IS_TIMEDOUT;
		break;
	}
	return true;
}

//
// 载入游戏进度
//
void CGameLoadAction::DoLoadSave()
{
	Parent->LoadGame(Selection);
}

//
// 储存游戏进度
//
void CGameSaveAction::DoLoadSave()
{
	Parent->SaveGame(Selection, Flags);
}
