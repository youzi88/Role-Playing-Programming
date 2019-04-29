//
//	SIZE, POINT, RECT 的Wrapper
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __Geomtry_h__
#define __Geomtry_h__

class CSize;
class CPoint;
class CRect;

//
//	CSize class(SIZE的Wrapper)
//
class CSize: public tagSIZE {
  public:
	CSize();
	CSize(int cx, int cy);
	CSize(SIZE size);
	CSize(DWORD size);

	BOOL operator==(SIZE size) const;
	BOOL operator!=(SIZE size) const;
	void operator+=(SIZE size);
	void operator-=(SIZE size);

	CSize operator+(SIZE size) const;
	CSize operator-(SIZE size) const;
	CSize operator-() const;
} ;

//
//	CPoint class(POINT的Wrapper)
//
class CPoint: public tagPOINT {
  public:
	CPoint();
	CPoint(int x, int y);
	CPoint(POINT point);
	CPoint(DWORD point);

	void Offset(int offx, int offy);
	void Offset(POINT point);
	void Offset(SIZE size);
	BOOL operator==(POINT point) const;
	BOOL operator!=(POINT point) const;
	void operator+=(SIZE size);
	void operator-=(SIZE size);
	void operator+=(POINT point);
	void operator-=(POINT point);

	CPoint operator+(SIZE size) const;
	CPoint operator-(SIZE size) const;
	CPoint operator-() const;
	CPoint operator+(POINT point) const;

	CSize operator-(POINT point) const;
} ;

//
//	CRect class(RECT的Wrapper)
//
#ifdef _GNU_H_WINDOWS32_STRUCTURES
class CRect: public _RECT {
#else
class CRect: public tagRECT {
#endif
  public:
	CRect();
	CRect(int x1, int y1, int x2, int y2);
	CRect(const RECT &rect);

	int Width() const;
	int Height() const;
	CSize Size() const;
	CPoint &TopLeft();
	CPoint &BottomRight();
	const CPoint &TopLeft() const;
	const CPoint &BottomRight() const;

	BOOL IsRectEmpty() const;
	BOOL IsRectNull() const;
	BOOL PtInRect(POINT point) const;

	void SetRect(int x1, int y1, int x2, int y2);
	void SetRectEmpty();

	void InflateRect(int x, int y);
	void InflateRect(SIZE size);

	void OffsetRect(int x, int y);
	void OffsetRect(SIZE size);
	void OffsetRect(POINT point);
	void NormalizeRect();

	void operator=(const RECT &rect);
	BOOL operator==(const RECT &rect) const;
	BOOL operator!=(const RECT &rect) const;
	void operator+=(POINT point);
	void operator+=(SIZE size);
	void operator+=(const RECT &rect);
	void operator-=(POINT point);
	void operator-=(SIZE size);
	void operator-=(const RECT &rect);
	void operator&=(const RECT &rect);
	void operator|=(const RECT &rect);

	CRect operator+(POINT point) const;
	CRect operator-(POINT point) const;
	CRect operator+(SIZE size) const;
	CRect operator-(SIZE size) const;
	CRect operator&(const RECT &rect) const;
	CRect operator|(const RECT &rect) const;
} ;

// inline成員函式

inline CSize::CSize()
{
}

inline CSize::CSize(int _cx, int _cy)
{
	cx = _cx;
	cy = _cy;
}

inline CSize::CSize(SIZE size)
{
	*(SIZE *)this = size;
}

inline CSize::CSize(DWORD size)
{
	cx = (short)LOWORD(size);
	cy = (short)HIWORD(size);
}

inline BOOL CSize::operator==(SIZE size) const
{
	return (cx == size.cx && cy == size.cy);
}

inline BOOL CSize::operator!=(SIZE size) const
{
	return (cx != size.cx || cy != size.cy);
}

inline void CSize::operator+=(SIZE size)
{
	cx += size.cx;
	cy += size.cy;
}

inline void CSize::operator-=(SIZE size)
{
	cx -= size.cx;
	cy -= size.cy;
}

inline CSize CSize::operator+(SIZE size) const
{
	return CSize(cx + size.cx, cy + size.cy);
}

inline CSize CSize::operator-(SIZE size) const
{
	return CSize(cx - size.cx, cy - size.cy);
}

inline CSize CSize::operator-() const
{
	return CSize(-cx, -cy);
}

inline CPoint::CPoint()
{
}

inline CPoint::CPoint(int _x, int _y)
{
	x = _x;
	y = _y;
}

inline CPoint::CPoint(POINT point)
{
	*(POINT *)this = point;
}

inline CPoint::CPoint(DWORD point)
{
	x = (short)LOWORD(point);
	y = (short)HIWORD(point);
}

inline void CPoint::Offset(int offx, int offy)
{
	x += offx;
	y += offy;
}

inline void CPoint::Offset(POINT point)
{
	x += point.x;
	y += point.y;
}

inline void CPoint::Offset(SIZE size)
{
	x += size.cx;
	y += size.cy;
}

inline BOOL CPoint::operator==(POINT point) const
{
	return (x == point.x && y == point.y);
}

inline BOOL CPoint::operator!=(POINT point) const
{
	return (x != point.x || y != point.y);
}

inline void CPoint::operator+=(SIZE size)
{
	x += size.cx;
	y += size.cy;
}

inline void CPoint::operator-=(SIZE size)
{
	x -= size.cx;
	y -= size.cy;
}

inline void CPoint::operator+=(POINT point)
{
	x += point.x;
	y += point.y;
}

inline void CPoint::operator-=(POINT point)
{
	x -= point.x;
	y -= point.y;
}

inline CPoint CPoint::operator+(SIZE size) const
{
	return CPoint(x + size.cx, y + size.cy);
}

inline CPoint CPoint::operator-(SIZE size) const
{
	return CPoint(x - size.cx, y - size.cy);
}

inline CPoint CPoint::operator-() const
{
	return CPoint(-x, -y);
}

inline CPoint CPoint::operator+(POINT point) const
{
	return CPoint(x + point.x, y + point.y);
}

inline CSize CPoint::operator-(POINT point) const
{
	return CSize(x - point.x, y - point.y);
}

inline CRect::CRect()
{
}

inline CRect::CRect(int x1, int y1, int x2, int y2)
{
	left = x1;
	top = y1;
	right = x2;
	bottom = y2;
}

inline CRect::CRect(const RECT &rect)
{
	left = rect.left;
	top = rect.top;
	right = rect.right;
	bottom = rect.bottom;
}

inline int CRect::Width() const
{
	return right - left;
}

inline int CRect::Height() const
{
	return bottom - top;
}

inline CSize CRect::Size() const
{
	return CSize(right - left, bottom - top);
}

inline CPoint &CRect::TopLeft()
{
	return *((CPoint *)this);
}

inline CPoint &CRect::BottomRight()
{
	return *((CPoint *)this + 1);
}

inline const CPoint &CRect::TopLeft() const
{
	return *((CPoint *)this);
}

inline const CPoint &CRect::BottomRight() const
{
	return *((CPoint *)this + 1);
}

inline BOOL CRect::IsRectEmpty() const
{
	return (left == right || top == bottom);
}

inline BOOL CRect::IsRectNull() const
{
	return (left == 0 && right == 0 && top == 0 && bottom == 0);
}

inline BOOL CRect::PtInRect(POINT point) const
{
	return ::PtInRect(this, point);
}

inline void CRect::SetRect(int x1, int y1, int x2, int y2)
{
	left = x1;
	top = y1;
	right = x2;
	bottom = y2;
}

inline void CRect::SetRectEmpty()
{
	left = top = right = bottom = 0;
}

inline void CRect::InflateRect(int x, int y)
{
	::InflateRect(this, x, y);
}

inline void CRect::InflateRect(SIZE size)
{
	::InflateRect(this, size.cx, size.cy);
}

inline void CRect::OffsetRect(int x, int y)
{
	::OffsetRect(this, x, y);
}

inline void CRect::OffsetRect(POINT point)
{
	::OffsetRect(this, point.x, point.y);
}

inline void CRect::OffsetRect(SIZE size)
{
	::OffsetRect(this, size.cx, size.cy);
}

inline void CRect::operator=(const RECT &srcRect)
{
	::CopyRect(this, &srcRect);
}

inline BOOL CRect::operator==(const RECT &rect) const
{
	return ::EqualRect(this, &rect);
}

inline BOOL CRect::operator!=(const RECT &rect) const
{
	return !::EqualRect(this, &rect);
}

inline void CRect::operator+=(POINT point)
{
	::OffsetRect(this, point.x, point.y);
}

inline void CRect::operator+=(SIZE size)
{
	::OffsetRect(this, size.cx, size.cy);
}

inline void CRect::operator+=(const RECT &rect)
{
	left += rect.left;
	top += rect.top;
	right += rect.right;
	bottom += rect.bottom;
}

inline void CRect::operator-=(POINT point)
{
	::OffsetRect(this, -point.x, -point.y);
}

inline void CRect::operator-=(SIZE size)
{
	::OffsetRect(this, -size.cx, -size.cy);
}

inline void CRect::operator-=(const RECT &rect)
{
	left -= rect.left;
	top -= rect.top;
	right -= rect.right;
	bottom -= rect.bottom;
}

inline void CRect::operator&=(const RECT &rect)
{
	::IntersectRect(this, this, &rect);
}

inline void CRect::operator|=(const RECT &rect)
{
	::UnionRect(this, this, &rect);
}

inline CRect CRect::operator+(POINT pt) const
{
	CRect rect(*this);
	::OffsetRect(&rect, pt.x, pt.y);
	return rect;
}

inline CRect CRect::operator-(POINT pt) const
{
	CRect rect(*this);
	::OffsetRect(&rect, -pt.x, -pt.y);
	return rect;
}

inline CRect CRect::operator+(SIZE size) const
{
	CRect rect(*this);
	::OffsetRect(&rect, size.cx, size.cy);
	return rect;
}

inline CRect CRect::operator-(SIZE size) const
{
	CRect rect(*this);
	::OffsetRect(&rect, -size.cx, -size.cy);
	return rect;
}

inline CRect CRect::operator&(const RECT &rect2) const
{
	CRect rect;
	::IntersectRect(&rect, this, &rect2);
	return rect;
}

inline CRect CRect::operator|(const RECT &rect2) const
{
	CRect rect;
	::UnionRect(&rect, this, &rect2);
	return rect;
}

#endif
