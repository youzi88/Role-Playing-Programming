//
// 执行战斗
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
// 构造函数
//
CBattleAction::CBattleAction(CMainWin *win, CAction *oldAction):
	CAction(win, oldAction), _map_data(0), map_data(0),
	ViewImage(win->GetViewImage())
{
}

//
// 析构函数
//
CBattleAction::~CBattleAction()
{
	delete _map_data;
	delete map_data;
}

//
// 鼠标指标移动时的处理
//
void CBattleAction::MouseMove(UINT modKeys, CPoint point)
{
	SetCursor(PointToIndex(point));
}

//
// 初始化
//
bool CBattleAction::Init()
{
	// 读入CG资料
	if (!Cursor.LoadImage("cursor")
	 || !MapImage.LoadImage("map1")
	 || !Player.LoadImage("fighter")
	 || !Enemy.LoadImage("puyo"))
		return false;

	// 设定sprite
	CursorSprite.Set(&Cursor, CPoint(0, 0), 
            CSize(MAPGRID_WIDTH, MAPGRID_HEIGHT), CMapSprite::DEPTH_CURSOR);

	CursorPos.x = -1;
	CursorPos.y = -1;
	SelPos.x = -1;
	SelPos.y = -1;
	SelChar = 0;

	MapSize.cx = MapSize.cy = 10;		// 地图的尺寸大小
	MapRect.SetRect(0, 0, MapSize.cx, MapSize.cy);

	// 确认map-data的可用空间
	_map_data = new MapData[MapSize.cx * MapSize.cy];
	map_data = new MapData *[MapSize.cy];

	for (int i=0; i<MapSize.cy; i++) {
		map_data[i] = _map_data + i * MapSize.cx;
	}

	// 把游戏者的人物新增到座标5, 5
	{
		map_data[5][5].type = PLAYER;
		player_list.push_back(CCharacter(&Player, 5, 5, CCharacter::DEPTH_CHAR));
		CCharacter &s = player_list.back();
		sprite.insert(&s);
		s.SetDirection(CCharacter::RIGHT);
		map_data[5][5].sprite = &s;
	}
	// 把敌人的人物新增到座标6, 3
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
// 产生地图画面
//
void CBattleAction::DrawMap(CImage *image, const CRect &rect)
{
	// 若地图有背景，则显示之
	if (MapImage.IsOK()) {
		image->Copy(&MapImage, rect);
	}
	// 显示sprite
	for (mapset::iterator p = sprite.begin(); p != sprite.end(); p++) {
		(*p)->Draw(image, rect);
	}
}

//
// 显示地图（整个地图）
//
void CBattleAction::DrawMap(CImage *image)
{
	DrawMap(image, CRect(0, 0, WindowWidth, WindowHeight));
}

//
// 移动地图光标
//
void CBattleAction::SetCursor(CPoint point)
{
	if (CursorPos != point) {		// 若地图位置有更改
		ClearCursor();			// 则删除目前显示的光标
		CursorPos = point;		// 重新设定新的位置
		DrawCursor();			// 显示新的光标
	}
}

//
// 删除地图光标
//
void CBattleAction::ClearCursor()
{
	if (MapRect.PtInRect(CursorPos)) {
		RemoveSprite(&CursorSprite);
	}
	CursorPos.x = CursorPos.y = -1;
}

//
// 显示地图光标
//
void CBattleAction::DrawCursor()
{
	if (MapRect.PtInRect(CursorPos)) {
		CursorSprite.SetMapPoint(CursorPos);
		AddSprite(&CursorSprite);
	}
}

// 重新绘制sprite
void CBattleAction::RedrawSprite(CSprite *s)
{
	CRect	rect;
	s->GetRect(&rect);
	UpdateRect(rect);
}

// 新增sprite
void CBattleAction::AddSprite(CSprite *s)
{
	sprite.insert(s);
	RedrawSprite(s);
}

// 删除sprite

void CBattleAction::RemoveSprite(CSprite *s)
{
	if (sprite.erase(s)) {
		RedrawSprite(s);
	}
}
