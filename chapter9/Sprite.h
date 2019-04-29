//
// Sprite
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__sprite_h__
#define	__sprite_h__

class CImage;
class CSprite {
  public:
	CSprite();
	CSprite(CImage *image, CPoint pos, CSize _size, int d);
	CSprite(CImage *image, CPoint pos, CPoint src, CSize _size, int d);
	virtual ~CSprite();

	void Set(CImage *image, CPoint pos, CSize _size, int d);
	void Set(CImage *image, CPoint pos, CPoint src, CSize _size, int d);
	void Draw(CImage *image);
	void Draw(CImage *image, const CRect &rect);
	void GetRect(CRect *r) const;
	CSize GetSize() const;
	void SetDrawPos(int x, int y);
	void SetDrawPos(CPoint point);
	CPoint GetDrawPos() const;
	void SetSrcPos(int x, int y);
	CPoint GetSrcPos() const;
	int GetDepth() const;
	void SetDepth(int d);
	BOOL PtIn(POINT pt);
	BOOL PtInImage(CPoint pt);
	void Show(bool _show=true) { show = _show; }
	bool IsShow() const { return show; }

	bool operator<(const CSprite &x) const;

  protected:
	CPoint	draw_pos;
	CPoint	src_pos;
	CSize	size;
	CSize	offset;
	CImage *image;
	int		depth;
	bool	show;
} ;

// ���캯��
//
inline CSprite::CSprite()
{
	draw_pos.x = 0;
	draw_pos.y = 0;
	src_pos.x = 0;
	src_pos.y = 0;
	size.cx = 0;
	size.cy = 0;
	offset.cx = 0;
	offset.cy = 0;
	image = 0;
	depth = 0;
	show = true;
}

inline CSprite::CSprite(CImage *_image, CPoint pos, CSize _size, int d)
{
	draw_pos = pos;
	src_pos.x = 0;
	src_pos.y = 0;
	size = _size;
	offset.cx = 0;
	offset.cy = 0;
	image = _image;
	depth = d;
	show = true;
}

inline CSprite::CSprite(CImage *_image, CPoint pos, CPoint src, CSize _size, int d)
{
	draw_pos = pos;
	src_pos = src;
	size = _size;
	offset.cx = 0;
	offset.cy = 0;
	image = _image;
	depth = d;
	show = true;
}

// �������趨�ڹ���õĶ�����
//
inline void CSprite::Set(CImage *_image, CPoint pos, CSize _size, int d)
{
	draw_pos = pos;
	size = _size;
	image = _image;
	depth = d;
}

inline void CSprite::Set(CImage *_image, CPoint pos, CPoint src, CSize _size, int d)
{
	draw_pos = pos;
	src_pos = src;
	size = _size;
	image = _image;
	depth = d;
}

// ȡ��ͼ���С
//
inline CSize CSprite::GetSize() const
{
	return size;
}

// �趨ͼ����Դ������
//
inline void CSprite::SetSrcPos(int x, int y)
{
	src_pos.x = x;
	src_pos.y = y;
}

// ȡ��ͼ����Դ������
//
inline CPoint CSprite::GetSrcPos() const
{
	return src_pos;
}

// �趨ͼ����Ŀ��(���)������
//
inline void CSprite::SetDrawPos(int x, int y)
{
	draw_pos.x = x;
	draw_pos.y = y;
	offset.cx = offset.cy = 0;
}

inline void CSprite::SetDrawPos(CPoint point)
{
	draw_pos = point;
	offset.cx = offset.cy = 0;
}

// ȡ��ͼ����Ŀ�ģ���棩������
//
inline CPoint CSprite::GetDrawPos() const
{
	return draw_pos;
}

// ȡ��ͼ�����(����˳��)
//
inline int CSprite::GetDepth() const
{
	return depth;
}

// �趨ͼ�����(����˳��)
//
inline void CSprite::SetDepth(int d)
{
	depth = d;
}

// ȡ��ͼ�����ʾ��Χ
//
inline void CSprite::GetRect(CRect *r) const
{
	r->left = draw_pos.x;
	r->top = draw_pos.y;
	r->right = draw_pos.x + size.cx;
	r->bottom = draw_pos.y + size.cy;
}

// ָ�������Ƿ���ͼ����ʾ�ķ�Χ��?
//
inline BOOL CSprite::PtIn(POINT pt)
{
	CRect rect;
	GetRect(&rect);
	return rect.PtInRect(pt);
}

// Ϊ��Ҫ������ʾ˳��Ҫ�Ƚϴ�С��ϵ������
//
inline bool CSprite::operator<(const CSprite &x) const
{
	int y1 = draw_pos.y + offset.cy + size.cy;
	int y2 = x.draw_pos.y + x.offset.cy + x.size.cy;
	if (y1 == y2) {
		int x1 = draw_pos.x + offset.cx + (size.cx + 1) / 2;
		int x2 = x.draw_pos.x + x.offset.cx + (x.size.cx + 1) / 2;
		if (x1 == x2) {
			return depth < x.depth;
		}
		return x1 < x2;
	}
	return y1 < y2;
}

#endif
