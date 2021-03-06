//
// Character--��ɫ
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "Image.h"
#include "Sprite.h"
#include "Character.h"
using namespace std;

bool CCharacter::MoveAnime(int dir, int count)
{
	direction = dir;
	CPoint map_pos = map_point;
	switch (dir) {
	  case UP:
		draw_pos.x -= MAPGRID_WIDTH / 8;
		draw_pos.y -= MAPGRID_HEIGHT / 8;
		map_pos.y--;
		break;

	  case DOWN:
		draw_pos.x += MAPGRID_WIDTH / 8;
		draw_pos.y += MAPGRID_HEIGHT / 8;
		map_pos.y++;
		break;

	  case LEFT:
		draw_pos.x -= MAPGRID_WIDTH / 8;
		draw_pos.y += MAPGRID_HEIGHT / 8;
		map_pos.x--;
		break;

	  case RIGHT:
		draw_pos.x += MAPGRID_WIDTH / 8;
		draw_pos.y -= MAPGRID_HEIGHT / 8;
		map_pos.x++;
		break;
	}
	static int index_table[] = {
		1, 0, 2, 0,
	} ;
	src_pos.x = (MOVE_OFS + index_table[count]) * size.cx;
	src_pos.y = dir * size.cy;
	if (count == 3) {
		SetMapPoint(map_pos);
		return true;
	}
	return false;
}
