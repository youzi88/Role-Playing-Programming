//
// Charater--角色
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__character_h__
#define	__character_h__

#include "Sprite.h"
#include "Params.h"
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

	bool operator==(CPoint p);

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

inline bool CMapSprite::operator==(CPoint p)
{
	return map_point.x == p.x && map_point.y == p.y;
}

class CCharacter: public CMapSprite {
  public:
	// 图像列
	// 移动		3
	// HIT		2
	// 攻击		4

	enum	{
		NIMAGES     = 9,
		MOVE_OFS    = 0,
		MOVE_STEP   = 3,
		HIT_OFS     = 3,
		HIT_STEP    = 2,
		ATTACK_OFS  = 5,
		ATTACK_STEP = 4,
		MAGIC_OFS   = 5,	// 攻击和魔法的模式
		MAGIC_STEP  = 4,	// 相同的东西转向
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
	bool AttackAnime(int count);
	bool HitAnime(int count);
	bool MagicAnime(int count);
	void NormalStatus();

	CPoint GetHeadPoint() const;
	CPoint GetCenterPoint() const;
	CPoint GetBottomPoint() const;

	const CMagicParam &GetMagicParam(int idx) const { return status.magic[idx]; }
	bool IsMagicOk(int idx) const;

	bool LevelUp();

	bool ClearStatus();

	bool operator==(const CCharacter &x);

  public:
	int direction;			// 人物的方向
	CStatus	status;
	bool move_done;
	bool attack_done;
} ;

inline CCharacter::CCharacter(CImage *dib, int x, int y, int d)
	:CMapSprite(dib, x, y, CSize(dib->Width() / NIMAGES, dib->Height() / 4), d)
{
	direction = UP;
	status.Clear();
	move_done = false;
	attack_done = false;
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

inline bool CCharacter::ClearStatus()
{
	move_done = false;
	attack_done = false;
	return true;
}

inline bool CCharacter::operator==(const CCharacter &x)
{
	return (map_point == x.map_point)? true: false;
}

inline bool CCharacter::IsMagicOk(int idx) const
{
	if (idx < status.nmagic)
		return status.magic[idx].use_mp <= status.magic_point;
	return false;
}

inline CPoint CCharacter::GetHeadPoint() const
{
	return CPoint(draw_pos.x + size.cx / 2, draw_pos.y);
}

inline CPoint CCharacter::GetCenterPoint() const
{
	return CPoint(draw_pos.x + size.cx / 2, draw_pos.y + size.cy / 2);
}

inline CPoint CCharacter::GetBottomPoint() const
{
	return CPoint(draw_pos.x + size.cx / 2, draw_pos.y + size.cy);
}

#endif
