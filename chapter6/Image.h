//
//	24Bits/Pixelͼ��
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__image_h
#define	__image_h

#include "Dib.h"
#include "Misc.h"

class CDC;

//
// ר��24bitʹ�õ�CIB���
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
	void MixImage(const CImage *image, const CRect &rect, COLORREF trans=RGB(0, 255, 0));
	void MixImage(const CImage *image, const CRect &rect, CPoint src_pos, COLORREF trans=RGB(0, 255, 0));
} ;

// inline ��Ա��ʽ

inline void CImage::Copy(const CImage *image)
{
	Copy(image, CRect(0, 0, image->Width(), image->Height()));
}

#endif
