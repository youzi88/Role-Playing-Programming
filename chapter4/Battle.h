//
// ִ��ս��
//
//	Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__battle_h__
#define	__battle_h__

#include "Action.h"
#include "Misc.h"
#include "Character.h"

class CMainWin;

//
// ս�����
//
class CBattleAction: public CAction {
  protected:
	// ״̬
	enum	{
		STATUS_NONE,
		STATUS_MOVE,
		STATUS_ANIME,
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
		CMapSprite *sprite;		// ��ͼ�ϵ�sprite
		unsigned char type;		// ��ͼ�Ϻ�����ʲô����?
		char dist;				// �ƶ��������ľ��루��ҵ�ã�
	} ;
	friend class MapData;

  public:
	CBattleAction(CMainWin *win, CAction *oldAction);
	~CBattleAction();

	virtual	void MouseMove(UINT modKeys, CPoint point);

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

	// ��ͼ������ʾλ��
	CPoint		CursorPos;

	// ѡ���ж���������Ϣ
	CPoint		SelPos;
	CCharacter *SelChar;

	list<CCharacter> player_list;	// ��Ϸ��
	list<CCharacter> enemy_list;	// ����
	list<CMapSprite> cursor_list;
	CMapSprite CursorSprite;

	class mapcomp: public binary_function<const CSprite *, const CSprite *, bool> {
	  public:
		result_type operator()(first_argument_type x, second_argument_type y) const
		{
			return *x < *y;
		}
	} ;
	typedef set<CSprite *, mapcomp>	mapset;

	mapset sprite;				// ����sprite���嵥
} ;

inline void CBattleAction::UpdateRect(const CRect &rect)
{
	DrawMap(ViewImage, rect);
	Parent->InvalidateRect(&rect);
}

#endif
