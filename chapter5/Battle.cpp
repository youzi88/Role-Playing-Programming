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
	status = STATUS_NONE;
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
// ����������ʱִ�еĴ���
//
void CBattleAction::LButtonDown(UINT modKeys, CPoint point)
{
	CPoint pos = PointToIndex(point);	// ��Ҫ��ͼ����
	switch (status) {
	  case STATUS_NONE:
		// ѡȡ��Ϸ�Ľ�ɫ
		if (MapRect.PtInRect(pos) && map_data[pos.y][pos.x].type & PLAYER) {
			SelPos = pos;
			SelChar = (CCharacter *)map_data[SelPos.y][SelPos.x].sprite;
			FindDist(pos.x, pos.y, 4);	// ��ʱ�̶���4��
			status = STATUS_MOVE;
		}
		break;

	  case STATUS_MOVE:
		// ��ѡʱ���ƶ���Χ
		if (MapRect.PtInRect(pos) && map_data[pos.y][pos.x].type & MOVEDIST) {
			MovePlayer(SelChar, pos);
		}
		break;

	  case STATUS_ANIME:
		break;
	}
}

//
// �ƶ����ʱ�Ĵ���
//
void CBattleAction::MouseMove(UINT modKeys, CPoint point)
{
	switch (status) {
	  case STATUS_NONE:			// ��ʼ״̬
	  case STATUS_MOVE:			// ��ʾ�ƶ���Χ
		SetCursor(PointToIndex(point));
		break;
	}
}

//
// ��ʱ���Ĵ���
//
bool CBattleAction::TimedOut(int timerId)
{
	switch (timerId) {
	  case TIMER_MOVE_ID:	// �ƶ�����
		{
			ASSERT(status == STATUS_ANIME);
			MoveAnime &ar = anime.front();
			CCharacter *character = ar.character;
			RemoveSprite(character);
			character->MoveAnime(ar.action, ar.count);
			AddSprite(character);
			if (ar.count++ == 3) {	// 4��ģʽ��ǰ������һ��
				anime.pop_front();
                                // �����ٵ�¼�����������
				if (anime.size() == 0) {
					CPoint pos = character->GetMapPoint();
					map_data[pos.y][pos.x].type |= PLAYER;
					map_data[pos.y][pos.x].sprite = character;
					status = STATUS_NONE;
					return true;
				}
			}
		}
		break;
	}
	return false;
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
	status = STATUS_NONE;

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

// Ѱ���ƶ���Χ

void CBattleAction::FindDist(int x, int y, int dist, bool first)
{
	if (!first) {
		AddDistCursor(x, y, dist, 0);
		map_data[y][x].type |= MOVEDIST;
	}
	map_data[y][x].dist = dist;
	if (dist == 0)
		return;
	if (y > MapRect.top && map_data[y - 1][x].ChkMove(dist))
		FindDist(x, y - 1, dist - 1, false);
	if (y < MapRect.bottom - 1 && map_data[y + 1][x].ChkMove(dist))
		FindDist(x, y + 1, dist - 1, false);
	if (x > MapRect.left && map_data[y][x - 1].ChkMove(dist))
		FindDist(x - 1, y, dist - 1, false);
	if (x < MapRect.right - 1 && map_data[y][x + 1].ChkMove(dist))
		FindDist(x + 1, y, dist - 1, false);
}

// ��ʾ��Χ

void CBattleAction::AddDistCursor(int x, int y, int dist, int type)
{
        // �ѵ�¼�Ĳ���
	if ((map_data[y][x].type & (MOVEDIST | ATTACKDIST)) != 0)	
		return;

	cursor_list.push_back(CMapSprite(&Cursor, x, y,
                CSize(MAPGRID_WIDTH, MAPGRID_HEIGHT), CMapSprite::DEPTH_AREA));
	CMapSprite &s = cursor_list.back();
	s.SetSrcPos(0, 32 + type * 32);
	AddSprite(&s);
}

// ɾ����ʾ��Χ

void CBattleAction::ClearDistCursor()
{
	while (cursor_list.size()) {
		CMapSprite &cr = cursor_list.front();
		CPoint pos = cr.GetMapPoint();
		RemoveSprite(&cr);
		map_data[pos.y][pos.x].type &= ~(MOVEDIST | ATTACKDIST);
		map_data[pos.y][pos.x].dist = 0;
		cursor_list.pop_front();
	}
}

// �����ƶ�

void CBattleAction::MovePlayer(CCharacter *character, CPoint point)
{
	MoveCharacter(point.x, point.y, map_data[point.y][point.x].dist, character);

	CPoint pos = character->GetMapPoint();
	map_data[pos.y][pos.x].type = NONE;
	map_data[pos.y][pos.x].sprite = 0;
	map_data[pos.y][pos.x].dist = 0;

	SetCursor(CPoint(-1, -1));
	ClearDistCursor();
	StartAnime();
}

// ��¼������ƶ�����

void CBattleAction::MoveCharacter(int x, int y, int dist, CCharacter *character)
{
	if (map_data[y][x].sprite != character) {
		dist++;
		if (y > MapRect.top && map_data[y - 1][x].dist == dist) {
			MoveCharacter(x, y - 1, dist, character);
			anime.push_back(MoveAnime(character, CCharacter::DOWN));
		}
		else if (y < MapRect.bottom - 1 && map_data[y + 1][x].dist == dist) {
			MoveCharacter(x, y + 1, dist, character);
			anime.push_back(MoveAnime(character, CCharacter::UP));
		}
		else if (x > MapRect.left && map_data[y][x - 1].dist == dist) {
			MoveCharacter(x - 1, y, dist, character);
			anime.push_back(MoveAnime(character, CCharacter::RIGHT));
		}
		else if (x < MapRect.right - 1 && map_data[y][x + 1].dist == dist) {
			MoveCharacter(x + 1, y, dist, character);
			anime.push_back(MoveAnime(character, CCharacter::LEFT));
		}
	}
}

// ִ���ƶ�����

void CBattleAction::StartAnime()
{
	if (anime.size()) {
		Parent->SetTimer(TIMER_MOVE_ID, TIMER_MOVE_TIME);
		status = STATUS_ANIME;
	}
	else {
		status = STATUS_NONE;
	}
}
