//
// Sprite--��ͼ���
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Image.h"
#include "Sprite.h"

// ��������
//
CSprite::~CSprite()
{
}

// sprite�Ļ���
//
void CSprite::Draw(CImage *_image)
{
	CRect rect;
	GetRect(&rect);
	_image->MixImage(image, rect, src_pos);
}

// sprite�Ļ���(���޼�)
//
void CSprite::Draw(CImage *_image, const CRect &rect)
{
	CRect r;
	GetRect(&r);
	r &= rect;
	if (!r.IsRectEmpty()) {
		CPoint	src = src_pos;
		if (draw_pos.x < r.left) {
			src.x += r.left - draw_pos.x;
		}
		if (draw_pos.y < r.top) {
			src.y += r.top - draw_pos.y;
		}
		_image->MixImage(image, r, src);
	}
}

// ��ָ����������ͼ������?
//
BOOL CSprite::PtInImage(CPoint pt)
{
	if (PtIn(pt)) {
		if (image->GetPixel(pt + -draw_pos + src_pos) != RGB(0, 0, 255))
			return TRUE;
	}
	return FALSE;
}
