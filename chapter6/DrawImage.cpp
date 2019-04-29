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
