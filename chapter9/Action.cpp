//
//  使用者的动作（基本）
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "Action.h"
#include "MainWin.h"

//
// 析构函数(Destructor)
//
CAction::~CAction()
{
}

//
// 动作暂停
//
void CAction::Pause()
{
}

//
// 动作再开始
//
void CAction::Resume()
{
}

//
// 鼠标左键按下时的处理
//
void CAction::LButtonDown(UINT, CPoint)
{
}

//
// 鼠标左键放开时的处理
//
void CAction::LButtonUp(UINT, CPoint)
{
}

//
// 鼠标右键按下时的处理
//
void CAction::RButtonDown(UINT, CPoint)
{
}

//
// 鼠标右键放开时的处理
//
void CAction::RButtonUp(UINT, CPoint)
{
}

//
// 鼠标值标移动时的处理
//
void CAction::MouseMove(UINT, CPoint)
{
}

//
// 键盘按下时的处理
//
void CAction::KeyDown(UINT)
{
}

//
// IDLE的处理
//
BOOL CAction::IdleAction()
{
	return FALSE;
}

//
// 计时器（timer）的处理
//
bool CAction::TimedOut(int timerId)
{
	return true;
}

//
// 音乐播放结束时的处理
//
// ·同一首再次播放
//
void CAction::MusicDone(int music)
{
	if (music)
		Parent->RestartMusic();
}

//
// 擦入结束时的处理
//
void CAction::WipeDone()
{
}

//
// Wave播放结束时的处理
//
void CAction::WaveDone()
{
}

//
// 是否可储存/载入?
//
BOOL CAction::IsSaveLoadOK() const
{
	return FALSE;	// 预设为不可
}

//
// 中断
//
void CAction::Abort()
{
}
