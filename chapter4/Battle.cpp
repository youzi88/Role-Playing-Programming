//
// ִ��ս��
//
//	Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include <stdlib.h>
#include "MainWin.h"
#include "File.h"
#include "Misc.h"
#include "Battle.h"

//
// ���캯��
//
CBattleAction::CBattleAction(CMainWin *win, CAction *oldAction):
	CAction(win, oldAction), _map_data(0), map_data(0),
	ViewImage(win->GetViewImage())
{
}

//
// ��������
//
CBattleAction::~CBattleAction()
{
	delete _map_data;
	delete map_data;
}

//
// ���ָ���ƶ�ʱ�Ĵ���
//
void CBattleAction::MouseMove(UINT modKeys, CPoint point)
{
	SetCursor(PointToIndex(point));
}

//
// ��ʼ��
//
bool CBattleAction::Init()
{
	// ����CG����
	if (!Cursor.LoadImage("cursor")
	 || !MapImage.LoadImage("map1")
	 || !Player.LoadImage("fighter")
	 || !Enemy.LoadImage("puyo"))
		return false;

	// �趨sprite
	CursorSprite.Set(&Cursor, CPoint(0, 0), 
            CSize(MAPGRID_WIDTH, MAPGRID_HEIGHT), CMapSprite::DEPTH_CURSOR);

	CursorPos.x = -1;
	CursorPos.y = -1;
	SelPos.x = -1;
	SelPos.y = -1;
	SelChar = 0;

	MapSize.cx = MapSize.cy = 10;		// ��ͼ�ĳߴ��С
	MapRect.SetRect(0, 0, MapSize.cx, MapSize.cy);

	// ȷ��map-data�Ŀ��ÿռ�
	_map_data = new MapData[MapSize.cx * MapSize.cy];
	map_data = new MapData *[MapSize.cy];

	for (int i=0; i<MapSize.cy; i++) {
		map_data[i] = _map_data + i * MapSize.cx;
	}

	// ����Ϸ�ߵ���������������5, 5
	{
		map_data[5][5].type = PLAYER;
		player_list.push_back(CCharacter(&Player, 5, 5, CCharacter::DEPTH_CHAR));
		CCharacter &s = player_list.back();
		sprite.insert(&s);
		s.SetDirection(CCharacter::RIGHT);
		map_data[5][5].sprite = &s;
	}
	// �ѵ��˵���������������6, 3
	{
		map_data[3][6].type = ENEMY;
		enemy_list.push_back(CCharacter(&Enemy, 6, 3, CCharacter::DEPTH_CHAR));
		CCharacter &s = enemy_list.back();
		sprite.insert(&s);
		s.SetDirection(CCharacter::RIGHT);
		map_data[3][6].sprite = &s;
	}
	DrawMap(ViewImage);

	Parent->InvalidateRect(NULL);

	return true;
}

//
// ������ͼ����
//
void CBattleAction::DrawMap(CImage *image, const CRect &rect)
{
	// ����ͼ�б���������ʾ֮
	if (MapImage.IsOK()) {
		image->Copy(&MapImage, rect);
	}
	// ��ʾsprite
	for (mapset::iterator p = sprite.begin(); p != sprite.end(); p++) {
		(*p)->Draw(image, rect);
	}
}

//
// ��ʾ��ͼ��������ͼ��
//
void CBattleAction::DrawMap(CImage *image)
{
	DrawMap(image, CRect(0, 0, WindowWidth, WindowHeight));
}

//
// �ƶ���ͼ���
//
void CBattleAction::SetCursor(CPoint point)
{
	if (CursorPos != point) {		// ����ͼλ���и���
		ClearCursor();			// ��ɾ��Ŀǰ��ʾ�Ĺ��
		CursorPos = point;		// �����趨�µ�λ��
		DrawCursor();			// ��ʾ�µĹ��
	}
}

//
// ɾ����ͼ���
//
void CBattleAction::ClearCursor()
{
	if (MapRect.PtInRect(CursorPos)) {
		RemoveSprite(&CursorSprite);
	}
	CursorPos.x = CursorPos.y = -1;
}

//
// ��ʾ��ͼ���
//
void CBattleAction::DrawCursor()
{
	if (MapRect.PtInRect(CursorPos)) {
		CursorSprite.SetMapPoint(CursorPos);
		AddSprite(&CursorSprite);
	}
}

// ���»���sprite
void CBattleAction::RedrawSprite(CSprite *s)
{
	CRect	rect;
	s->GetRect(&rect);
	UpdateRect(rect);
}

// ����sprite
void CBattleAction::AddSprite(CSprite *s)
{
	sprite.insert(s);
	RedrawSprite(s);
}

// ɾ��sprite

void CBattleAction::RemoveSprite(CSprite *s)
{
	if (sprite.erase(s)) {
		RedrawSprite(s);
	}
}
