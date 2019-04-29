//
//	DIB Section
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "DrawImage.h"
#include "dc.h"
#include "Misc.h"

//
// 构造函数
//
CDrawImage::CDrawImage()
	:CImage(), hBitmap(0)
{
}

//
// 析构函数
//
CDrawImage::~CDrawImage()
{
	GdiFlush();
	if (hBitmap) {
		::DeleteObject(hBitmap);
	}
}

//
// 制作DIB section
//
BOOL CDrawImage::Create(HDC dc, int width, int height)
{
	W = width;
	H = height;
	D = 24;

	bytes_per_line = ScanBytes(width, 24);
	bytes_per_pixel = PixelBytes(24);

	InfoHeader.biSize			= sizeof(BITMAPINFOHEADER);
	InfoHeader.biWidth			= width;
	InfoHeader.biHeight			= height;
	InfoHeader.biBitCount		= 24;
	InfoHeader.biPlanes			= 1;
	InfoHeader.biXPelsPerMeter	= 0;
	InfoHeader.biYPelsPerMeter	= 0;
	InfoHeader.biClrUsed		= 0;
	InfoHeader.biClrImportant	= 0;
	InfoHeader.biCompression	= BI_RGB;
	InfoHeader.biSizeImage		= bytes_per_line * height;

	Info = (BITMAPINFO *)&InfoHeader;
	hBitmap = CreateDIBSection(dc, Info, DIB_RGB_COLORS, &Bits, NULL, 0);

	return hBitmap != 0;
}

//
// DIB字符串的描绘
//
void CDrawImage::DrawText(HFONT hFont, int x1, int y1, const char *str, COLORREF color)
{
	CMemoryDC	dc(0);
	HBITMAP	oldBitmap = dc.SelectObject(hBitmap);
	HFONT	oldFont = dc.SelectObject(hFont);

	dc.SetTextColor(color);
	dc.SetBkMode(TRANSPARENT);

	dc.ExtTextOut(x1, y1, 0, 0, str, strlen(str), NULL);

	dc.SelectObject(oldBitmap);
	dc.SelectObject(oldFont);

	GdiFlush();
}

//
// 擦入处理
//
void CDrawImage::WipeIn(CImage *image, const CRect &rect, int count)
{
	int		len = rect.Width() * 3;
	for (int y=rect.top + count; y<rect.bottom; y+=8) {
		memcpy(GetBits(rect.left, y), image->GetBits(rect.left, y), len);
	}
}

//
// 擦出处理
//
void CDrawImage::WipeOut(const CRect &rect, int count)
{
	int		len = rect.Width() * 3;
	for (int y=rect.top + count; y<rect.bottom; y+=8) {
		memset(GetBits(rect.left, y), 0, len);
	}
}

//
// Fading处理
//
void CDrawImage::FadeCvt(CImage *image, const CRect &rect, const byte_t *cvt)
{
	for (int y=rect.top; y<rect.bottom; y++) {
		byte_t *p1 = (byte_t *)GetBits(rect.left, y);
		byte_t *p2 = (byte_t *)image->GetBits(rect.left, y);

		for (int x=rect.left; x<rect.right; x++) {
			*p1++ = cvt[*p2++];		// blue
			*p1++ = cvt[*p2++];		// green
			*p1++ = cvt[*p2++];		// red
		}
	}
}

//
// “黑->图像”处理
//
void CDrawImage::FadeFromBlack(CImage *image, const CRect &rect, int count)
{
	byte_t	cvt[256];

	count++;
	for (int i=0; i<256; i++) {
		cvt[i] = (byte_t)((i * count) / 16);
	}
	FadeCvt(image, rect, cvt);
}

//
//  “图像->黑”处理
//
void CDrawImage::FadeToBlack(CImage *image, const CRect &rect, int count)
{
	byte_t	cvt[256];

	count = 15 - count;
	for (int i=0; i<256; i++) {
		cvt[i] = (byte_t)((i * count) / 16);
	}
	FadeCvt(image, rect, cvt);
}

//
// “白->图像”处理
//
void CDrawImage::FadeFromWhite(CImage *image, const CRect &rect, int count)
{
	byte_t	cvt[256];

	count++;
	int		level = 255 * (16 - count);
	for (int i=0; i<256; i++) {
		cvt[i] = (byte_t)((i * count + level) / 16);
	}
	FadeCvt(image, rect, cvt);
}

//
// “图像->白”处理
//
void CDrawImage::FadeToWhite(CImage *image, const CRect &rect, int count)
{
	byte_t	cvt[256];

	count = 15 - count;
	int		level = 255 * (16 - count);
	for (int i=0; i<256; i++) {
		cvt[i] = (byte_t)((i * count + level) / 16);
	}
	FadeCvt(image, rect, cvt);
}

//
// Title Mixing处理
//
void CDrawImage::Mix(CImage *image, const CRect &rect, int count)
{
	static unsigned	BitMask[] = {
		0x2080,	// 0010 0000 1000 0000
		0xa0a0,	// 1010 0000 1010 0000
		0xa1a4,	// 1010 0001 1010 0100
		0xa5a5,	// 1010 0101 1010 0101
		0xada7,	// 1010 1101 1010 0111
		0xafaf,	// 1010 1111 1010 1111
		0xefbf,	// 1110 1111 1011 1111
		0xffff,	// 1111 1111 1111 1111
	} ;
	static unsigned	XMask[] = {
		0xf000, 0x0f00, 0x00f0, 0x000f,
	} ;
	static unsigned	YMask[] = {
		0x8888, 0x4444, 0x2222, 0x1111,
	} ;

	for (int y=rect.top; y<rect.bottom; y++) {
		unsigned char *p = (unsigned char *)GetBits(rect.left, y);
		byte_t *q = (byte_t *)image->GetBits(rect.left, y);
		unsigned	mask = BitMask[count] & YMask[y & 3];

		for (int x=rect.left; x<rect.right; x++) {
			if (mask & XMask[x & 3]) {
				p[0] = q[0];	// blue
				p[1] = q[1];	// green
				p[2] = q[2];	// red
			}
			p += 3;
			q += 3;
		}
	}
}

//
// 描绘到CDrawImage的DC类别
//
// 构造函数
//
CImageDC::CImageDC(CDrawImage *image)
	:CMemoryDC(0)
{
	oldBmp = SelectObject(image->GetBitmapHandle());
}

//
// 析构函数
//
CImageDC::~CImageDC()
{
	::GdiFlush();
	SelectObject(oldBmp);
}
