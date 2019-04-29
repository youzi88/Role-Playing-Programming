//
// Character--角色
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

bool CCharacter::AttackAnime(int count)
{
	src_pos.x = (ATTACK_OFS + count) * size.cx;
	src_pos.y = direction * size.cy;
	return count == ATTACK_STEP - 1;
}

bool CCharacter::HitAnime(int count)
{
	src_pos.x = (HIT_OFS + count) * size.cx;
	src_pos.y = direction * size.cy;
	return count == HIT_STEP - 1;
}

bool CCharacter::MagicAnime(int count)
{
	src_pos.x = (MAGIC_OFS + count) * size.cx;
	src_pos.y = direction * size.cy;
	return count == MAGIC_STEP - 1;
}

bool CCharacter::LevelUp()
{
	// 求出下一次提升等级所需的经验值
	// 这里是每次UP20%

	int next_experience = 100;
	for (int i=1; i<status.level; i++) {
		next_experience = next_experience * 12 / 10;
	}

	// 提升等级？
	if (status.experience < next_experience)
		return false;		

	// 提升等级

	status.experience -= next_experience;

	status.LevelUp();

	while (LevelUp())
		;
	return true;
}
