//
// 参数
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include "Params.h"
#include "File.h"
#include "Misc.h"

//
// 角色的状态
//

#define	DEF_MAGIC_HEAL		{ MAGIC_HEAL, MAGIC_TYPE_SELF, 1, 15, 3 }
#define	DEF_MAGIC_FIREBALL	{ MAGIC_FIREBALL, MAGIC_TYPE_ATTACK, 3, 15, 3 }

static CStatus init_status[] = {	// 初始化参数
	{
		"主公",		// 名称
		1,			// 等级
		0,			// 经验
		3,			// 移动距离
		1,			// 攻击距离
		10,			// 攻击力
		0,			// 魔法力
		15,			// 防御力
		10,			// 抵抗力
		30,			// max HP
		20,			// max MP
		30,			// HP
		20,			// MP
		0,			// 使用魔法数
	},
	{
		"魔法师",	// 名称
		1,			// 等级
		0,			// 经验
		3,			// 移动距离
		1,			// 攻击距离
		5,			// 攻击力
		10,			// 魔法力
		10,			// 防御力
		20,			// 抵抗力
		25,			// max HP
		20,			// max MP
		25,			// HP
		20,			// MP
		1,			// 使用魔法数
		DEF_MAGIC_FIREBALL,	// 使用魔法
	},
	{
		"神官",		// 名称
		1,			// 等级
		0,			// 经验
		2,			// 移动距离
		1,			// 攻击距离
		1,			// 攻击力
		20,			// 魔法力
		8,			// 防御力
		15,			// 抵抗力
		25,			// max HP
		20,			// max MP
		25,			// HP
		20,			// MP
		1,			// 使用魔法数
		DEF_MAGIC_HEAL,	// 使用魔法
	},
	{
		"怪物",		// 名称
		1,			// 等级
		0,			// 经验
		3,			// 移动距离
		2,			// 攻击距离
		8,			// 攻击力
		10,			// 魔法力
		10,			// 防御力
		12,			// 抵抗力
		20,			// max HP
		20,			// max MP
		20,			// HP
		20,			// MP
		1,			// 使用魔法数
		DEF_MAGIC_FIREBALL,		// 使用魔法
	},
	{
		"泥巴怪",	// 名称
		1,			// 等级
		0,			// 经验
		3,			// 移动距离
		1,			// 攻击距离
		7,			// 攻击力
		0,			// 魔法力
		10,			// 防御力
		8,			// 抵抗力
		20,			// max HP
		20,			// max MP
		20,			// HP
		20,			// MP
		0,			// 使用魔法数
	},
} ;

CStatus *CParams::GetStatus(const char *name)
{	
	// 准备好登录
	{
		for (int i=0; i<MAX_CHARACTER; i++) {
			if (strcmp(status[i].name, name) == 0)
				return status + i;
		}
	}
	// 读出初始化的信息
	{
		for (int i=0; i<sizeof(init_status) / sizeof(init_status[0]); i++) {
			if (strcmp(init_status[i].name, name) == 0)
				return init_status + i;
		}
	}
	return 0;
}
