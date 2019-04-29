//
// 24Bits/Pixel图像
//
// Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __image_h
#define __image_h

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

void Copy(const CImage *image, const CRect &rect);
void Copy(const CImage *image);
void MixImage(const CImage *image, const CRect &rect, COLORREF trans=RGB(0, 255, 0));
} ;

// inline 成员函式

inline void CImage::Copy(const CImage *image)
{
Copy(image, CRect(0, 0, image->Width(), image->Height()));
}

#endif
