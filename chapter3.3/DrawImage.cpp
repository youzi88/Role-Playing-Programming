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
// 产生DIB section
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
