//
// ִ��ս��
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
// ħ��Ч���õ���ͼ���
//
class CMagicSprite: public CMapSprite {
  public:
	void SetFireBall(CImage *image, CCharacter *from);
	bool MoveFireBall(CCharacter *from, CCharacter *to, int count);
} ;

//
// Fireball���趨
//
inline void CMagicSprite::SetFireBall(CImage *_image, CCharacter *from)
{
	Set(_image, CPoint(0, 0), CPoint(0, 0), _image->Size(), 100);
	SetMapPoint(from->GetMapPoint());
	draw_pos.y -= from->GetSize().cy;
	offset.cy = from->GetSize().cy;
}

//
// Fireball���ƶ�
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
// ս�����
//
class CBattleAction: public CAction {
  protected:
	// ״̬
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
		NONE            = 0,		// һ��ص�
		RESTRICTED_AREA = 1 << 0,	// ����
		PLAYER          = 1 << 1,	// ��������Ϸ��
		ENEMY           = 1 << 2,	// �е���
		OBJECT          = 1 << 3,	// �ж�����
		MOVEDIST        = 1 << 4,	// ���ƶ���Χ�Ĺ��
		ATTACKDIST      = 1 << 5,	// �й�����Χ�Ĺ��
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
		COMMAND_NO_SELECT = -1,	// δѡȡ
		COMMAND_MOVE,			// �ƶ�
		COMMAND_ATTACK,			// ֱ�ӹ���
		COMMAND_MAGIC_FIRST,	// ħ��1
		COMMAND_MAGIC_END = COMMAND_MAGIC_FIRST + MAX_MAGIC - 1,
		MAX_COMMAND,			// ���������
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

		// ���ƶ���?
		bool ChkMove(int _dist)
		{
			return type == CBattleAction::NONE
				|| ((type & CBattleAction::MOVEDIST) != 0 && move_dist < _dist);
		}

		// �ɹ�����?
		bool ChkAttack(int _dist)
		{
			return attack_dist < _dist;
		}

		// �ж��ڵ�ͼ�ϵ��ͱ�
		bool ChkType(unsigned char _type)
		{
			return (type & _type) == _type;
		}

	  public:
		CMapSprite *sprite;		// ��ͼ�ϵ�sprite
		unsigned char type;		// ��ͼ�Ϻ�����ʲô����?
		char move_dist;			// �ƶ�����(��ҵ��)
		char attack_dist;		// ��������(��ҵ��)
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

	// ͼ��Ĺ���
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
			((CChip &)chip).image = 0;	//��ֹ����delete
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

	// ħ������
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
	CImage		StatusBase;		// ����״̬����ʾ
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

	// ��ͼ�����ʾλ��
	CPoint		CursorPos;

	// ��ɫ����ѡ�����Ϣ
	CPoint		SelPos;
	CCharacter *SelChar;

	// ��ɫ��״̬��Ϣ
	CCharacter *StatusChar;

	// ��������
	CCharacter *AttackFrom;
	CCharacter *AttackTo;
	bool AttackPlayerToEnemy;
	int AttackAnimeCount;
	int AttackAnimeState;
	CSprite AttackSprite;

	// ħ��
	CImage FireBallImage;
	CImage HealImage;
	CImage Explosion;
	CMagicSprite MagicSprite[2];

	map<int, CChip> chip_list;	// chipͼ���嵥

	list<CCharacter> player_list;	// Player--��Ϸ��
	list<CCharacter> enemy_list;	// Enemy---�з�
	list<CMapSprite> object_list;	// �ϰ���
	list<CMapSprite> cursor_list;
	CMapSprite CursorSprite;
	CSprite CommandSprite[MAX_COMMAND];
	CSprite TurnEndSprite;
	CSprite StatusSprite[MAX_CHARACTER];
	CCharacter *StatusCharacter[MAX_CHARACTER];
	CSprite PopupSprite;
	CSprite Number[10];				// ����
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

	mapset sprite;				// ������ͼ���(sprite)���嵥

	// EnemyTuen����ʱ����
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
