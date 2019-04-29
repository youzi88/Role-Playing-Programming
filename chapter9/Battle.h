//
// 执行战斗
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__battle_h__
#define	__battle_h__

#include "Action.h"
#include "Misc.h"
#include "Params.h"
#include "TextReader.h"
#include "Lexer.h"
#include "Character.h"

//
// 魔法效果用的贴图零件
//
class CMagicSprite: public CMapSprite {
  public:
	void SetFireBall(CImage *image, CCharacter *from);
	bool MoveFireBall(CCharacter *from, CCharacter *to, int count);
} ;

//
// Fireball的设定
//
inline void CMagicSprite::SetFireBall(CImage *_image, CCharacter *from)
{
	Set(_image, CPoint(0, 0), CPoint(0, 0), _image->Size(), 100);
	SetMapPoint(from->GetMapPoint());
	draw_pos.y -= from->GetSize().cy;
	offset.cy = from->GetSize().cy;
}

//
// Fireball的移动
//
inline bool CMagicSprite::MoveFireBall(CCharacter *from, CCharacter *to, int count)
{
	CPoint opos((MAPGRID_WIDTH - size.cx) / 2, MAPGRID_HEIGHT - size.cy);
	CPoint fpos = IndexToPoint(from->GetMapPoint()) + opos;
	CPoint tpos = IndexToPoint(to->GetMapPoint()) + opos;
	CSize diff = fpos - tpos;
	int n = ((abs(diff.cx) > abs(diff.cy)? abs(diff.cx): abs(diff.cy)) + 31) / 32;
	int fy = from->GetSize().cy;
	int dy = from->GetSize().cy - (to->GetSize().cy / 2);
	int ofs = fy - dy * count / n;
	draw_pos.x = fpos.x - diff.cx * count / n;
	draw_pos.y = fpos.y - diff.cy * count / n - ofs;
	offset.cy = ofs;

	return count <= n;
}

class CMainWin;

//
// 战斗类别
//
class CBattleAction: public CAction {
  protected:
	// 状态
	enum	{
		STATUS_WAIT_START,
		STATUS_NONE,
		STATUS_MOVE,
		STATUS_ATTACK,
		STATUS_MAGIC_ENEMY,
		STATUS_MAGIC_PLAYER,
		STATUS_COMMAND,
		STATUS_NEXTTURN,
		STATUS_ANIME,
		STATUS_BATTLE_END,
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
		TIMER_ATTACK_ID,
		TIMER_NUMBER_ID,
		TIMER_MAGIC_ID,
		TIMER_LEVELUP_ID,
		TIMER_MOVE_TIME = 50,
		TIMER_ATTACK_TIME = 100,
		TIMER_NUMBER_TIME = 100,
		TIMER_MAGIC_TIME = 100,
		TIMER_LEVELUP_TIME = 100,
	} ;
	enum	{
		PLAYER_TURN,
		ENEMY_TURN,
	} ;
	enum	{
		COMMAND_NO_SELECT = -1,	// 未选取
		COMMAND_MOVE,			// 移动
		COMMAND_ATTACK,			// 直接攻击
		COMMAND_MAGIC_FIRST,	// 魔法1
		COMMAND_MAGIC_END = COMMAND_MAGIC_FIRST + MAX_MAGIC - 1,
		MAX_COMMAND,			// 最大命令数
	} ;

	class MapData {
	  public:
		MapData()
		{
			sprite = 0;
			type = 0;
			move_dist = 0;
			attack_dist = 0;
		}

		// 可移动吗?
		bool ChkMove(int _dist)
		{
			return type == CBattleAction::NONE
				|| ((type & CBattleAction::MOVEDIST) != 0 && move_dist < _dist);
		}

		// 可攻击吗?
		bool ChkAttack(int _dist)
		{
			return attack_dist < _dist;
		}

		// 判定在地图上的型别
		bool ChkType(unsigned char _type)
		{
			return (type & _type) == _type;
		}

	  public:
		CMapSprite *sprite;		// 地图上的sprite
		unsigned char type;		// 地图上好像有什么东西?
		char move_dist;			// 移动距离(作业用)
		char attack_dist;		// 攻击距离(作业用)
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

	// 图像的管理
	class CChip {
	  public:
		CChip(const char *_name, const char *file)
		{
			name = _name;
			image = new CImage;
			if (!image->LoadImage(file)) {
				delete image;
				image = 0;
			}
		}
		CChip(const CChip &chip)
		{
			name = chip.name;
			image = chip.image;
			((CChip &)chip).image = 0;	//防止多重delete
		}
		~CChip()
		{
			delete image;
		}

		CImage *image;
		string name;
	} ;

  public:
	CBattleAction(CMainWin *win, CAction *oldAction);
	~CBattleAction();

	virtual	void Pause();
	virtual	void Resume();

	virtual	void LButtonDown(UINT modKeys, CPoint point);
	virtual	void RButtonDown(UINT modKeys, CPoint point);
	virtual	void MouseMove(UINT modKeys, CPoint point);

	virtual	void KeyDown(UINT key);
	virtual bool TimedOut(int timerId);

	virtual BOOL IdleAction();
	virtual void WipeDone();
	virtual void WaveDone();
	virtual void Abort();

	bool Load(const char *name);

	void BattleEnd(bool iswin);

  protected:
	bool LoadMap(const char *name);
	bool PlayerCmd(TextReader &reader, Lexer &lexer);
	bool EnemyCmd(TextReader &reader, Lexer &lexer);
	bool ObjectCmd(TextReader &reader, Lexer &lexer);
	bool MapSizeCmd(TextReader &reader, Lexer &lexer);
	bool MapImageCmd(TextReader &reader, Lexer &lexer);
	bool MapCmd(TextReader &reader, Lexer &lexer);
	void __cdecl Error(TextReader &reader, const char *fmt, ...);
	int FindMapStatus(const char *find);
	CImage *AddImage(const char *file);
	void DrawMap(CImage *image, const CRect &rect);
	void DrawMap(CImage *image);

	int PlayerNumber(int number) const { return PLAYER | (number << 8); }
	int EnemyNumber(int number) const { return ENEMY | (number << 8); }
	int ObjectNumber(int number) const { return OBJECT | (number << 8); }

	void SetCursor(CPoint point);
	void ClearCursor();
	void DrawCursor();
	void AddSprite(CSprite *s);
	void RemoveSprite(CSprite *s);
	void RedrawSprite(CSprite *s);
	void FindDist(int x, int y, int dist, bool first=true);
	void FindAttack(int x, int y, int dist, bool first=true);
	void AddDistCursor(int x, int y, int dist, int type);
	void ClearDistCursor();
	void MovePlayer(CCharacter *character, CPoint point);
	void MoveCharacter(int x, int y, int dist, CCharacter *character);
	void StartAnime();
	void ChangeTurn(int _turn);
	void ShowCommand(CPoint point);
	void HighlightCommand(int newsel, bool selection);
	void HighlightCommand(CPoint point);
	void HideCommand();
	void ShowTurnEnd(CPoint point);
	void HideTurnEnd();
	void SelectTurnEnd(bool sel);
	void Attack(CCharacter *from, CCharacter *to, bool player);
	void AttackPhase2();
	void AttackPhase3();
	void LevelUp();
	void Magic(CCharacter *from, CCharacter *to, bool player);
	void FindAttackCalc(int x, int y, int dist);
	void MoveEnemy();
	void AttackEnemy();
	void ChangeStatus(CPoint point);
	void ChangePlayerStatus(CCharacter *ch);
	void SetPlayerStatus(int player);
	void SelectCommand(CPoint point);
	int FindCommand(CPoint point);
	CPoint CommandIndex(int idx);

	// 魔法动作
	bool MagicHeal();
	void HealPhase2();
	bool MagicFireBall();
	void FireBallPhase2();

	void UpdateRect(const CRect &rect);

  protected:
	CDrawImage *ViewImage;
	CDrawImage *MixedImage;
	CImage     *BackLayer;
	CImage		MapImage;
	CImage		Cursor;
	CImage		Command;
	CImage		TurnEnd;
	CImage		StatusBase;		// 基本状态的显示
	CDrawImage	Status[MAX_CHARACTER];
	CImage		PopupImageBase;
	CImage		PopupParts;
	CDrawImage	PopupImage;
	MapData    *_map_data;
	MapData   **map_data;
	CSize		MapSize;
	CRect		MapRect;
	int			status;
	int			turn;
	int			select;
	bool		sel_turnend;
	CMagicParam magic_param;
	HFONT		hFont;

	// 地图光标显示位置
	CPoint		CursorPos;

	// 角色动作选择的信息
	CPoint		SelPos;
	CCharacter *SelChar;

	// 角色的状态信息
	CCharacter *StatusChar;

	// 攻击动画
	CCharacter *AttackFrom;
	CCharacter *AttackTo;
	bool AttackPlayerToEnemy;
	int AttackAnimeCount;
	int AttackAnimeState;
	CSprite AttackSprite;

	// 魔法
	CImage FireBallImage;
	CImage HealImage;
	CImage Explosion;
	CMagicSprite MagicSprite[2];

	map<int, CChip> chip_list;	// chip图像清单

	list<CCharacter> player_list;	// Player--游戏者
	list<CCharacter> enemy_list;	// Enemy---敌方
	list<CMapSprite> object_list;	// 障碍物
	list<CMapSprite> cursor_list;
	CMapSprite CursorSprite;
	CSprite CommandSprite[MAX_COMMAND];
	CSprite TurnEndSprite;
	CSprite StatusSprite[MAX_CHARACTER];
	CCharacter *StatusCharacter[MAX_CHARACTER];
	CSprite PopupSprite;
	CSprite Number[10];				// 数字
	int NumberView;
	list<MoveAnime> anime;

	class mapcomp: public binary_function<const CSprite *, const CSprite *, bool> {
	  public:
		result_type operator()(first_argument_type x, second_argument_type y) const
		{
			return *x < *y;
		}
	} ;
	typedef set<CSprite *, mapcomp>	mapset;

	mapset sprite;				// 所有贴图零件(sprite)的清单

	// EnemyTuen的暂时变量
	std::list<CCharacter>::iterator enemy_ptr;
	CCharacter *attack_target;
	bool magic_attack;
} ;

inline void CBattleAction::UpdateRect(const CRect &rect)
{
	DrawMap(ViewImage, rect);
	Parent->InvalidateRect(&rect);
}

#endif
