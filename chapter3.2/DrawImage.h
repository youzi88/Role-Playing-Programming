//
// DIB Section
//
// Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __DrawImage_h__
#define __DrawImage_h__

#include "Image.h"
#include "Misc.h"

//
// ʹ��DIB section��24bit��DIB���
//
class CDrawImage: public CImage {
public:
	CDrawImage();
	~CDrawImage();
	
	BOOL Create(HDC dc, int width, int height);
	
	void Draw(HDC dc, int x, int y, int w, int h, int ox=0, int oy=0);
	void Draw(HDC dc, const CRect &rect, CPoint point);
	void Draw(HDC dc, const CRect &rect);
	
	HBITMAP GetBitmapHandle() const { return hBitmap; }
	
protected:
	BITMAPINFOHEADER InfoHeader;
	HBITMAP hBitmap;
} ;

// inline ��Ա��ʽ

// ����
//
// ��Ȼ��3�֣��������Ա�����̬��һ��֮�⣬������ͬ
//
inline void CDrawImage::Draw(HDC dc, int x, int y, int w, int h, int ox, int oy)
{
	HDC memdc = CreateCompatibleDC(dc);
	HGDIOBJ oldbmp = SelectObject(memdc, hBitmap);
	BitBlt(dc, x, y, w, h, memdc, ox, oy, SRCCOPY);
	GdiFlush();
	SelectObject(memdc, oldbmp);
	DeleteDC(memdc);
}

inline void CDrawImage::Draw(HDC dc, const CRect &rect, CPoint point)
{
	Draw(dc, rect.left, rect.top, rect.Width(), rect.Height(), point.x, point.y);
}

inline void CDrawImage::Draw(HDC dc, const CRect &rect)
{
	Draw(dc, rect.left, rect.top, rect.Width(), rect.Height(), rect.left, rect.top);
}

#endif
