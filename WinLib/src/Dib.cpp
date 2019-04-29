//
//	DIB base class
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Dib.h"
#include "Misc.h"
#include "File.h"

//
// 构造函数
//
// 以IsOK()成员函式来判断Create是否有错误
//
CDib::CDib(int width, int height, int depth)
	:Bits(0), Info(0), hGlobal(0)
{
	Create(width, height, depth);
}

//
// 析构函数
//
CDib::~CDib()
{
	if (hGlobal) {
		::GlobalUnlock(hGlobal);
		::GlobalFree(hGlobal);
	}
}

//
// 产生DIB
//
BOOL CDib::Create(int width, int height, int depth)
{
	W = width;
	H = height;
	D = depth;

	bytes_per_line = ScanBytes(width, depth);
	bytes_per_pixel = PixelBytes(depth);

	long	bitsAlloc = bytes_per_line * height;
	long	headerSize = sizeof(BITMAPINFOHEADER);
	if (depth == 8)
		headerSize += sizeof(RGBQUAD) * 256;

	if ((hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, bitsAlloc + headerSize)) == 0)
		return FALSE;

	Info = (BITMAPINFO *)::GlobalLock(hGlobal);
	Bits = (void *)((char *)Info + headerSize);

	Info->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	Info->bmiHeader.biWidth			= width;
	Info->bmiHeader.biHeight		= height;
	Info->bmiHeader.biBitCount		= (unsigned short)depth;
	Info->bmiHeader.biPlanes		= 1;
	Info->bmiHeader.biXPelsPerMeter	= 0;
	Info->bmiHeader.biYPelsPerMeter	= 0;
	Info->bmiHeader.biClrUsed		= 0;
	Info->bmiHeader.biClrImportant	= 0;
	Info->bmiHeader.biCompression	= BI_RGB;
	Info->bmiHeader.biSizeImage		= bitsAlloc;

	return TRUE;
}

//
// 用0清除DIB的位元图像
//
void CDib::Clear()
{
	memset(GetBits(), 0, bytes_per_line * H);
}

//
// 读入BMP档
//
//	若并无DIB，则产生一个跟BMP同样大小的DIB
//
BOOL CDib::LoadBMP(CFile &file, int ox, int oy)
{
	BITMAPFILEHEADER	header;
	BITMAPINFOHEADER	infoHeader;
	if (file.Read(&header, sizeof(header)) != sizeof(header)
	 || header.bfType != ((WORD) ('M' << 8) | 'B')
	 || file.Read(&infoHeader, sizeof(infoHeader)) != sizeof(infoHeader)
	 || infoHeader.biSize < sizeof(BITMAPINFOHEADER)
	 || infoHeader.biCompression != BI_RGB)
		return FALSE;

	int		width = infoHeader.biWidth;
	int		height = infoHeader.biHeight;
	int		depth = infoHeader.biBitCount;
	if (!IsOK()) {
		if (!Create(width + ox, height + oy, depth))
			return FALSE;
	}
	if (width + ox > Width() || height + oy > Height() || depth != Depth())
		return FALSE;

	int		bits_offset = sizeof(BITMAPFILEHEADER) + infoHeader.biSize;
	if (depth == 8) {
		int csize = sizeof(RGBQUAD) * 256;
		if (file.Read(Info->bmiColors, csize) != csize)
			return FALSE;
		bits_offset += csize;
	}
	file.Seek(bits_offset, CFile::begin);

	int		length = width * BytesPerPixel();
	int		filler = ScanBytes(width, depth) - length;

	for (int y=height-1; y>=0; y--) {
		if (file.Read(GetBits(ox, oy + y), length) != length)
			return FALSE;
		if (filler)
			file.Seek(filler, CFile::current);
	}
	return TRUE;
}
