//
//	24Bits/Pixel图像
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__image_h
#define	__image_h

#include "Dib.h"
#include "Misc.h"

class CDC;

//
// 专给24bit使用的CIB类别
//
class CImage: public CDib {
  public:
	CImage(): CDib() {}
	CImage(int width, int height);

	BOOL Create(int width, int height);

	BOOL LoadImage(const char *name, int ox=0, int oy=0);

	void Copy(const CImage *image, const CRect &rect, CPoint point);
	void Copy(const CImage *image, const CRect &rect);
	void Copy(const CImage *image);
	void FillRect(const CRect &rect, COLORREF color);
	void FillRect(int x, int y, int w, int h, COLORREF color);
	void MixImage(const CImage *image, const CRect &rect, COLORREF trans=RGB(0, 255, 0));
	void MixImage(const CImage *image, const CRect &rect, CPoint src_pos, COLORREF trans=RGB(0, 255, 0));
	void DrawRect(const CRect &rect, COLORREF color);
	void DrawRect(int x, int y, int w, int h, COLORREF color);
	void FillHalfToneRect(const CRect &rect);
	void FillHalfToneRect(int x, int y, int w, int h);
	void DrawFrameRect(const CRect &rect, COLORREF color=WhitePixel);
	void DrawFrameRect(int x, int y, int w, int h, COLORREF color=WhitePixel);
	COLORREF GetPixel(CPoint point) const;
} ;

// inline 成员函式

inline void CImage::Copy(const CImage *image)
{
	Copy(image, CRect(0, 0, image->Width(), image->Height()));
}

inline void CImage::FillRect(int x, int y, int w, int h, COLORREF color)
{
	FillRect(CRect(x, y, x + w, y + h), color);
}

inline void CImage::DrawRect(int x, int y, int w, int h, COLORREF color)
{
	DrawRect(CRect(x, y, x + w, y + h), color);
}

inline void CImage::FillHalfToneRect(int x, int y, int w, int h)
{
	FillHalfToneRect(CRect(x, y, x + w, y + h));
}

inline void CImage::DrawFrameRect(const CRect &rect, COLORREF color)
{
	DrawFrameRect(rect.left, rect.top, rect.Width(), rect.Height(), color);
}

inline COLORREF CImage::GetPixel(CPoint point) const
{
	const byte_t *p = (byte_t *)GetBits(point.x, point.y);
	return RGB(p[2], p[1], p[0]);
}

#endif
