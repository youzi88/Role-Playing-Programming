//
// 执行战斗
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__battle_h__
#define	__battle_h__

#include "Action.h"
#include "Misc.h"
#include "Character.h"

class CMainWin;

//
// 战斗类别
//
class CBattleAction: public CAction {
  protected:
	// 状态
	enum	{
		STATUS_NONE,
		STATUS_MOVE,
		STATUS_ANIME,
	} ;
	enum	{
		NONE            = 0,		// 一般地点
		RESTRICTED_AREA = 1 << 0,	// 禁区
		PLAYER          = 1 << 1,	// 有其他游戏者
		ENEMY           = 1 << 2,	// 有敌人
		OBJECT          = 1 << 3,	// 有东西在
		MOVEDIST        = 1 << 4,	// 有移动范围的光标
		ATTACKDIST      = 1 << 5,	// 有攻击范围的光标
	} ;
	enum	{
		TIMER_MOVE_ID = 1000,
		TIMER_MOVE_TIME = 50,
	} ;

	class MapData {
	  public:
		MapData()
		{
			sprite = 0;
			type = 0;
			dist = 0;
		}

		bool ChkMove(int _dist)
		{
			return type == CBattleAction::NONE
				|| ((type & CBattleAction::MOVEDIST) != 0 && dist < _dist);
		}

	  public:
		CMapSprite *sprite;		// 地图上的sprite
		unsigned char type;		// 地图上好像有什么东西?
		char dist;				// 移动、攻击的距离（作业用）
	} ;
	friend class MapData;

	class MoveAnime {
	  public:
		MoveAnime(CCharacter *_character, int _action)
		{
			action = _action;
			character = _character;
			count = 0;
		}

		int action;
		int count;
		CCharacter *character;
	} ;

  public:
	CBattleAction(CMainWin *win, CAction *oldAction);
	~CBattleAction();

	virtual	void LButtonDown(UINT modKeys, CPoint point);
	virtual	void MouseMove(UINT modKeys, CPoint point);

	virtual bool TimedOut(int timerId);

	bool Init();

  protected:
	void DrawMap(CImage *image, const CRect &rect);
	void DrawMap(CImage *image);

	void SetCursor(CPoint point);
	void ClearCursor();
	void DrawCursor();
	void AddSprite(CSprite *s);
	void RemoveSprite(CSprite *s);
	void RedrawSprite(CSprite *s);
	void FindDist(int x, int y, int dist, bool first=true);
	void AddDistCursor(int x, int y, int dist, int type);
	void ClearDistCursor();
	void MovePlayer(CCharacter *character, CPoint point);
	void MoveCharacter(int x, int y, int dist, CCharacter *character);
	void StartAnime();

	void UpdateRect(const CRect &rect);

  protected:
	CDrawImage *ViewImage;
	CImage		MapImage;
	CImage		Cursor;
	CImage		Player;
	CImage		Enemy;
	MapData    *_map_data;
	MapData   **map_data;
	CSize		MapSize;
	CRect		MapRect;
	int			status;

	// 地图光标的显示位置
	CPoint		CursorPos;

	// 选择行动的人物信息
	CPoint		SelPos;
	CCharacter *SelChar;

	list<CCharacter> player_list;	// 游戏者
	list<CCharacter> enemy_list;	// 敌人
	list<CMapSprite> cursor_list;
	CMapSprite CursorSprite;
	list<MoveAnime> anime;

	class mapcomp: public binary_function<const CSprite *, const CSprite *, bool> {
	  public:
		result_type operator()(first_argument_type x, second_argument_type y) const
		{
			return *x < *y;
		}
	} ;
	typedef set<CSprite *, mapcomp>	mapset;

	mapset sprite;				// 所有sprite的清单
} ;

inline void CBattleAction::UpdateRect(const CRect &rect)
{
	DrawMap(ViewImage, rect);
	Parent->InvalidateRect(&rect);
}

#endif
