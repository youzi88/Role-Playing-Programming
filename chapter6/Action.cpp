//
//  ʹ���ߵĶ�����������
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "Action.h"
#include "MainWin.h"

//
// ��������(Destructor)
//
CAction::~CAction()
{
}

//
// ������ͣ
//
void CAction::Pause()
{
}

//
// �����ٿ�ʼ
//
void CAction::Resume()
{
}

//
// ����������ʱ�Ĵ���
//
void CAction::LButtonDown(UINT, CPoint)
{
}

//
// �������ſ�ʱ�Ĵ���
//
void CAction::LButtonUp(UINT, CPoint)
{
}

//
// ����Ҽ�����ʱ�Ĵ���
//
void CAction::RButtonDown(UINT, CPoint)
{
}

//
// ����Ҽ��ſ�ʱ�Ĵ���
//
void CAction::RButtonUp(UINT, CPoint)
{
}

//
// ���ֵ���ƶ�ʱ�Ĵ���
//
void CAction::MouseMove(UINT, CPoint)
{
}

//
// ���̰���ʱ�Ĵ���
//
void CAction::KeyDown(UINT)
{
}

//
// IDLE�Ĵ���
//
BOOL CAction::IdleAction()
{
	return FALSE;
}

//
// ��ʱ����timer���Ĵ���
//
bool CAction::TimedOut(int timerId)
{
	return true;
}