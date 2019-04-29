//
// Charater--角色
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__character_h__
#define	__character_h__

#include "Sprite.h"
#include "coordinate.h"

class CMapSprite: public CSprite {
  public:
	enum	{
		DEPTH_AREA,
		DEPTH_CURSOR,
		DEPTH_CHAR,
		DEPTH_OBJECT,
	} ;

  public:
	CMapSprite();
	CMapSprite(CImage *dib, int x, int y, CSize size, int d);
	CMapSprite(CImage *dib, int x, int y, int d);

	void SetMapPoint(CPoint point);
	CPoint GetMapPoint() const { return map_point; }

  protected:
	CPoint map_point;	// 地图上的位置
} ;

inline CMapSprite::CMapSprite()
{
	map_point.x = map_point.y = 0;
}

inline CMapSprite::CMapSprite(CImage *dib, int x, int y, CSize size, int d)
	:CSprite(dib, IndexToPoint(x, y) + CPoint((MAPGRID_WIDTH - size.cx) / 2, MAPGRID_HEIGHT - size.cy), size, d)
{
	map_point.x = x;
	map_point.y = y;
}

inline CMapSprite::CMapSprite(CImage *dib, int x, int y, int d)
{
	CSize size(dib->Width(), dib->Height());
	Set(dib, IndexToPoint(x, y) + CPoint((MAPGRID_WIDTH - size.cx) / 2, MAPGRID_HEIGHT - size.cy), size, d);
	map_point.x = x;
	map_point.y = y;
}

inline void CMapSprite::SetMapPoint(CPoint point)
{
	map_point = point;
	SetDrawPos(IndexToPoint(point) + CPoint((MAPGRID_WIDTH - size.cx) / 2, MAPGRID_HEIGHT - size.cy));
}

class CCharacter: public CMapSprite {
  public:
	enum	{
		NIMAGES     = 3,
		MOVE_OFS    = 0,
		MOVE_STEP   = 3,
	} ;
	enum	{
		LEFT,
		DOWN,
		RIGHT,
		UP,
	} ;

  public:
	CCharacter(CImage *dib, int x, int y, int d);

	void SetDirection(int dir);	// 设定方向
	bool MoveAnime(int direction, int count);
	void NormalStatus();

  public:
	int direction;			// 人物的方向
} ;

inline CCharacter::CCharacter(CImage *dib, int x, int y, int d)
	:CMapSprite(dib, x, y, CSize(dib->Width() / NIMAGES, dib->Height() / 4), d)
{
	direction = UP;
}

inline void CCharacter::SetDirection(int dir)
{
	direction = dir;
	src_pos.y = dir * size.cy;
}

inline void CCharacter::NormalStatus()
{
	src_pos.x = MOVE_OFS * size.cx;
	src_pos.y = direction * size.cy;
}

#endif
