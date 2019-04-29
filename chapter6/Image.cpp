//
//	24Bits/Pixel ͼ��
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
// ���캯��
//
CImage::CImage(int width, int height)
{
	Create(width, height);
}

//
// ����DIB
//
BOOL CImage::Create(int width, int height)
{
	return CDib::Create(width, height, 24);
}

//
// ��CGPATHָ����·������BMPͼ��
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
// ��������
//
void CImage::Copy(const CImage *image, const CRect &rect)
{
	int		len = rect.Width() * 3;
	for (int y=rect.top; y<rect.bottom; y++) {
		memcpy(GetBits(rect.left, y), image->GetBits(rect.left, y), len);
	}
}

//
// ��������(��ָ��������Դ����)
//
void CImage::Copy(const CImage *image, const CRect &rect, CPoint point)
{
	int		len = rect.Width() * 3;
	for (int y=rect.top; y<rect.bottom; y++) {
		memcpy(GetBits(rect.left, y), image->GetBits(point.x, point.y++), len);
	}
}

//
// ���ƣ�������͸����ɫ��
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
// ���ƣ�������͸����ɫ��
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
