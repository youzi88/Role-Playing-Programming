//
// ���롢���涯��
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "MainWin.h"
#include "LoadSave.h"

//
// ��ʼ��
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
// ����������ʱ�Ĵ���
//
void CGameLoadSaveAction::LButtonDown(UINT modKeys, CPoint point)
{
	Pressed = TRUE;
}

//
// �ſ�������ʱ�Ĵ���
//
void CGameLoadSaveAction::LButtonUp(UINT modKeys, CPoint point)
{
	Pressed = FALSE;
	if (Selection >= 0)
		DoLoadSave();
}

//
// ��������Ҽ�ʱ�Ĵ���
//
void CGameLoadSaveAction::RButtonDown(UINT modKeys, CPoint point)
{
	CancelPressed = TRUE;
}

//
// �ſ�����Ҽ�ʱ�Ĵ���
//
void CGameLoadSaveAction::RButtonUp(UINT modKeys, CPoint point)
{
	if (CancelPressed)
		Parent->CancelLoadSaveMenu(Flags);
}

//
// ���ָ���ƶ�ʱ�Ĵ���
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
// ���¼���ʱ�Ĵ���
//
void CGameLoadSaveAction::KeyDown(UINT key)
{
	switch (key) {
	  case VK_RETURN:
	  case VK_SPACE:	// ִ�����룯����
		if (Selection >= 0)
			DoLoadSave();
		break;

	  case VK_ESCAPE:	// ȡ��
		Parent->CancelLoadSaveMenu(Flags);
		break;

	  case VK_UP:		// ѡ��ǰһ��
		{
			int		sel = Parent->PrevLoadSaveSelect(Selection);
			if (sel != Selection) {
				Parent->SelectLoadSaveMenu(Selection, false);
				Selection = sel;
				Parent->SelectLoadSaveMenu(Selection, true);
			}
		}
		break;

	  case VK_DOWN:		// ѡ���һ��
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
// ��ʱ���Ĵ���
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
// ������Ϸ����
//
void CGameLoadAction::DoLoadSave()
{
	Parent->LoadGame(Selection);
}

//
// ������Ϸ����
//
void CGameSaveAction::DoLoadSave()
{
	Parent->SaveGame(Selection, Flags);
}
