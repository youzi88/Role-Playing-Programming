//
// SPRITE
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__sprite_h__
#define	__sprite_h__

class CImage;
class CSprite {
  public:
	CSprite();
	CSprite(CImage *image, CPoint pos, CSize _size);
	CSprite(CImage *image, CPoint pos, CPoint src, CSize _size);
	virtual ~CSprite();

	void Set(CImage *image, CPoint pos, CSize _size);
	void Set(CImage *image, CPoint pos, CPoint src, CSize _size);
	void Draw(CImage *image);
	void Draw(CImage *image, const CRect &rect);
	void GetRect(CRect *r) const;
	CSize GetSize() const;
	void SetDrawPos(int x, int y);
	void SetDrawPos(CPoint point);
	CPoint GetDrawPos() const;
	void SetSrcPos(int x, int y);
	CPoint GetSrcPos() const;
	BOOL PtIn(POINT pt);

  protected:
	CPoint	draw_pos;
	CPoint	src_pos;
	CSize	size;
	CImage *image;
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
	image = 0;
}

inline CSprite::CSprite(CImage *_image, CPoint pos, CSize _size)
{
	draw_pos = pos;
	src_pos.x = 0;
	src_pos.y = 0;
	size = _size;
	image = _image;
}

inline CSprite::CSprite(CImage *_image, CPoint pos, CPoint src, CSize _size)
{
	draw_pos = pos;
	src_pos = src;
	size = _size;
	image = _image;
}

// �������趨�ڹ���õĶ�����
//
inline void CSprite::Set(CImage *_image, CPoint pos, CSize _size)
{
	draw_pos = pos;
	size = _size;
	image = _image;
}

inline void CSprite::Set(CImage *_image, CPoint pos, CPoint src, CSize _size)
{
	draw_pos = pos;
	src_pos = src;
	size = _size;
	image = _image;
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
}

inline void CSprite::SetDrawPos(CPoint point)
{
	draw_pos = point;
}

// ȡ��ͼ����Ŀ�ģ���棩������
//
inline CPoint CSprite::GetDrawPos() const
{
	return draw_pos;
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

#endif
