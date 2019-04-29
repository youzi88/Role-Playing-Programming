//
//	DIB Section
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__DrawImage_h__
#define	__DrawImage_h__

#include "Image.h"
#include "Misc.h"

//
//	使用DIB section的24bit的DIB类别
//
class CDrawImage: public CImage {
  public:
	CDrawImage();
	~CDrawImage();

	BOOL Create(HDC dc, int width, int height);

	void Draw(HDC dc, int x, int y, int w, int h, int ox=0, int oy=0);
	void Draw(HDC dc, const CRect &rect, CPoint point);
	void Draw(HDC dc, const CRect &rect);

	void DrawText(HFONT hFont, int x, int y, const char *str, COLORREF color=WhitePixel);

	void WipeIn(CImage *image, const CRect &rect, int count);
	void WipeOut(const CRect &rect, int count);
	void FadeFromBlack(CImage *image, const CRect &rect, int count);
	void FadeToBlack(CImage *image, const CRect &rect, int count);
	void FadeFromWhite(CImage *image, const CRect &rect, int count);
	void FadeToWhite(CImage *image, const CRect &rect, int count);

	void Mix(CImage *image, const CRect &rect, int count);

	HBITMAP	GetBitmapHandle() const { return hBitmap; }

  protected:
	void FadeCvt(CImage *image, const CRect &rect, const byte_t *cvt);

  protected:
	BITMAPINFOHEADER InfoHeader;
	HBITMAP	hBitmap;
} ;

// inline 成员函式

// 绘制
//
// 虽然有3种，但除了引数型态不一样之外，动作均同
//
inline void CDrawImage::Draw(HDC dc, int x, int y, int w, int h, int ox, int oy)
{
	HDC	memdc = CreateCompatibleDC(dc);
	HGDIOBJ	oldbmp = SelectObject(memdc, hBitmap);
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

//
// 要描绘到CDrawImage中的 DC 类别
//
class CImageDC: public CMemoryDC {
  public:
	CImageDC(CDrawImage *image);
	~CImageDC();

  protected:
	HBITMAP	oldBmp;
} ;

#endif
