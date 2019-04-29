//
// ������Ч
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include <stdlib.h>
#include "Sample.h"
#include "MainWin.h"
#include "Effect.h"
#include "Image.h"
#include "misc.h"

//
// ��Ч�����ļ̳�
// 
// һ�δ���һ�����趯�����ڵȴ�ʱ������
//
// ����ķֽ⶯��
//
bool EffectWipeIn::Step()
{
	Dst->WipeIn(Src, EffectRect, EffectCnt);
	Window->Repaint(EffectRect);

	if (++EffectCnt >= 8)
		return false;
	return true;
}

//
// �����ķֽ⶯��
//
bool EffectWipeOut::Step()
{
	Dst->WipeOut(EffectRect, EffectCnt);
	Window->Repaint(EffectRect);

	if (++EffectCnt >= 8)
		return false;
	return true;
}

//
// ����ķֽ⶯��
//
bool EffectFadeIn::Step()
{
	Dst->FadeFromBlack(Src, EffectRect, EffectCnt);
	Window->Repaint(EffectRect);

	if (++EffectCnt >= 16)
		return false;
	return true;
}

//
// �����ķֽ⶯��
//
bool EffectFadeOut::Step()
{
	Dst->FadeToBlack(Src, EffectRect, EffectCnt);
	Window->Repaint(EffectRect);

	if (++EffectCnt >= 16)
		return false;
	return true;
}

//
// �׽��ķֽ⶯��
//
bool EffectWhiteIn::Step()
{
	Dst->FadeFromWhite(Src, EffectRect, EffectCnt);
	Window->Repaint(EffectRect);

	if (++EffectCnt >= 16)
		return false;
	return true;
}

//
// �׳��ķֽ⶯��
//
bool EffectWhiteOut::Step()
{
	Dst->FadeToWhite(Src, EffectRect, EffectCnt);
	Window->Repaint(EffectRect);

	if (++EffectCnt >= 16)
		return false;
	return true;
}

//
// MixFade�ķֽ⶯��
//
bool EffectMixFade::Step()
{
	Dst->Mix(Src, EffectRect, EffectCnt);
	Window->Repaint(EffectRect);

	if (++EffectCnt >= 8)
		return false;
	return true;
}

//
// �����ķֽ⶯��
//
bool EffectFlash::Step()
{
	switch (EffectCnt++) {
	  case 0:	// Ϳ��
		{
			CClientDC	dc(Window);
			dc.SetBkColor(RGB(255, 255, 255));
			dc.ExtTextOut(0, 0, ETO_OPAQUE, &EffectRect, 0, 0, NULL);
		}
		break;

	  case 1:	// �ش�
		Window->Repaint(EffectRect);
		break;

	  default:	// һ�������ظ�
		return false;
	}
	return true;
}

//
// ����ҡ���ķֽ⶯��
//
// ����˳�����ϡ��¡��ϡ����ظ�
//
bool EffectShake::Step()
{
	int		x, y, w, h, ox, oy;
	CRect	rect;

	switch (EffectCnt) {
	  case 0:
	  case 2:
		x = 0;
		y = 0;
		w = WindowWidth;
		h = WindowHeight - 10;
		ox = 0;
		oy = 10;
		rect.SetRect(0, WindowHeight - 10, WindowWidth, WindowHeight);
		break;

	  case 4:
		x = 0;
		y = 0;
		w = WindowWidth;
		h = WindowHeight;
		ox = 0;
		oy = 0;
		rect.SetRect(0, 0, 0, 0);
		break;

	  case 1:
	  case 3:
		x = 0;
		y = 10;
		w = WindowWidth;
		h = WindowHeight - 10;
		ox = 0;
		oy = 0;
		rect.SetRect(0, 0, WindowWidth, 10);
		break;

	  default:
		return false;
	}

	CClientDC	dc(Window);
	Dst->Draw(dc, x, y, w, h, ox, oy);
	if (x != ox || y != oy) {
		dc.SetBkColor(RGB(0, 0, 0));
		dc.ExtTextOut(0, 0, ETO_OPAQUE, &rect, 0, 0, NULL);
	}

	if (++EffectCnt >= 5)
		return false;
	return true;
}
