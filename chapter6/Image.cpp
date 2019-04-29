//
//	24Bits/Pixel 图像
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Application.h"
#include "Image.h"
#include "File.h"
#include "Misc.h"
#include "dc.h"

//
// 构造函数
//
CImage::CImage(int width, int height)
{
	Create(width, height);
}

//
// 产生DIB
//
BOOL CImage::Create(int width, int height)
{
	return CDib::Create(width, height, 24);
}

//
// 从CGPATH指定的路径载入BMP图档
//
BOOL CImage::LoadImage(const char *name, int ox, int oy)
{
	char	path[_MAX_PATH];
	sprintf(path, CGPATH "%s.bmp", name);
	CFile	file(path);
	if (!file)
		return FALSE;
	return LoadBMP(file, ox, oy);
}

//
// 复制区域
//
void CImage::Copy(const CImage *image, const CRect &rect)
{
	int		len = rect.Width() * 3;
	for (int y=rect.top; y<rect.bottom; y++) {
		memcpy(GetBits(rect.left, y), image->GetBits(rect.left, y), len);
	}
}

//
// 复制区域(有指定复制来源座标)
//
void CImage::Copy(const CImage *image, const CRect &rect, CPoint point)
{
	int		len = rect.Width() * 3;
	for (int y=rect.top; y<rect.bottom; y++) {
		memcpy(GetBits(rect.left, y), image->GetBits(point.x, point.y++), len);
	}
}

//
// 复制（考虑了透明颜色）
//
void CImage::MixImage(const CImage *image, const CRect &rect, COLORREF trans_color)
{
	const unsigned char trans_b = GetBValue(trans_color);
	const unsigned char trans_g = GetGValue(trans_color);
	const unsigned char trans_r = GetRValue(trans_color);

	for (int y=rect.top; y<rect.bottom; y++) {
		byte_t *p = (byte_t *)GetBits(rect.left, y);
		const byte_t *q = (byte_t *)image->GetBits(rect.left, y);
		for (int x=rect.left; x<rect.right; x++) {
			const byte_t b = *q++;
			const byte_t g = *q++;
			const byte_t r = *q++;

			if (b != trans_b || g != trans_g || r != trans_r) {
				p[0] = b;
				p[1] = g;
				p[2] = r;
			}
			p += 3;
		}
	}
}
//
// 复制（考虑了透明颜色）
//
void CImage::MixImage(const CImage *image, const CRect &rect, CPoint point, COLORREF trans_color)
{
	const unsigned char trans_b = GetBValue(trans_color);
	const unsigned char trans_g = GetGValue(trans_color);
	const unsigned char trans_r = GetRValue(trans_color);

	for (int y=rect.top; y<rect.bottom; y++) {
		byte_t *p = (byte_t *)GetBits(rect.left, y);
		const byte_t *q = (byte_t *)image->GetBits(point.x, point.y++);
		for (int x=rect.left; x<rect.right; x++) {
			const byte_t b = *q++;
			const byte_t g = *q++;
			const byte_t r = *q++;

			if (b != trans_b || g != trans_g || r != trans_r) {
				p[0] = b;
				p[1] = g;
				p[2] = r;
			}
			p += 3;
		}
	}
}
