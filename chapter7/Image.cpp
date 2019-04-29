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
void CImage::FillRect(const CRect &rect, COLORREF color)
{
	const unsigned char b = GetBValue(color);
	const unsigned char g = GetGValue(color);
	const unsigned char r = GetRValue(color);

	for (int y=rect.top; y<rect.bottom; y++) {
		byte_t *p = (byte_t *)GetBits(rect.left, y);
		for (int x=rect.left; x<rect.right; x++) {
			*p++ = b;
			*p++ = g;
			*p++ = r;
		}
	}
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

//
// ���ƾ���
//
void CImage::DrawRect(const CRect &rect, COLORREF color)
{
	int		width = rect.Width();
	int		height = rect.Height();
	FillRect(rect.left, rect.top, width, 1, color);
	FillRect(rect.left, rect.top, 1, height, color);
	FillRect(rect.right - 1, rect.top, 1, height, color);
	FillRect(rect.left, rect.bottom - 1, width, 1, color);
}

//
// ����͸����50%�ĺ�ɫ
//
void CImage::FillHalfToneRect(const CRect &rect)
{
	for (int y=rect.top; y<rect.bottom; y++) {
		byte_t *p = (byte_t *)GetBits(rect.left, y);
		for (int x=rect.left; x<rect.right; x++) {
			*p++ /= 2;
			*p++ /= 2;
			*p++ /= 2;
		}
	}
}

//
// ���Ʊ߿�
//
void CImage::DrawFrameRect(int x, int y, int w, int h, COLORREF color)
{
	DrawRect(x, y + 1, w, h - 2, color);
	DrawRect(x + 1, y, w - 2, h, color);
	FillHalfToneRect(x + 2, y + 2, w - 4, h - 4);
}
