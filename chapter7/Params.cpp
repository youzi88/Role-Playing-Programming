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

#define	HEADER_ID	"DATA"

//
// 载入数据
//
static BOOL LoadData(const char *path, void *buf, int length)
{
	CFile	file(path, CFile::read);

	if (!file)
		return FALSE;

	if (file.Read(buf, length) != length)
		return FALSE;
	return TRUE;
}

//
// 储存数据
//
static BOOL SaveData(const char *path, const void *buf, int length)
{
	CFile	file(path, CFile::write);

	if (!file)
		return FALSE;

	if (file.Write(buf, length) != length)
		return FALSE;
	return TRUE;
}

//
// 参数初始化
//
void CParams::Clear()
{
	memset(this, 0, sizeof(CParams));
}

//
// 读出参数值
//
BOOL CParams::Load(int no)
{
	char	file[16];
	sprintf(file, "SAVE%04d.DAT", no + 1);

	unsigned char	buf[sizeof(CParams) + 4];

	if (!LoadData(file, buf, sizeof(buf)))
		return FALSE;

	if (memcmp(buf, HEADER_ID, 4) != 0)
		return FALSE;

	memcpy(this, buf + 4, sizeof(CParams));
	return TRUE;
}

//
// 参数的保存
//
BOOL CParams::Save(int no)
{
	SYSTEMTIME	time;
	GetLocalTime(&time);

	save_month = (unsigned char)time.wMonth;
	save_date = (unsigned char)time.wDay;
	save_hour = (unsigned char)time.wHour;
	save_minute = (unsigned char)time.wMinute;

	unsigned char	buf[sizeof(CParams) + 4];

	memset(buf, 0, sizeof(buf));
	memcpy(buf, HEADER_ID, 4);
	memcpy(buf + 4, this, sizeof(CParams));

	char	file[16];
	sprintf(file, "SAVE%04d.DAT", no + 1);

	if (!SaveData(file, buf, sizeof(buf)))
		return FALSE;
	return TRUE;
}

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
