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
#include "ParamFile.h"

//
// 有魔法吗?
//
int CStatus::FindMagic(int id) const
{
	for (int i=0; i<nmagic; i++) {
		if (id == magic[i].id)
			return i;
	}
	return -1;
}

void CStatus::LevelUp()
{
	// 提升等级
	level++;

	// UP各项能力20%
	experience = experience * 12 / 10;
	attack_power = attack_power * 12 / 10;
	magic_power = magic_power * 12 / 10;
	defence_power = defence_power * 12 / 10;
	resistance = resistance * 12 / 10;
	max_hit_point = max_hit_point * 12 / 10;
	max_magic_point = max_magic_point * 12 / 10;

	// 恢复HP和MP
	hit_point = max_hit_point;
	magic_point = max_magic_point;
}

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
// 人物角色的状态
//

static CMagicParam DEF_MAGIC_HEAL = { MAGIC_HEAL, MAGIC_TYPE_SELF, 1, 15, 4 } ;
static CMagicParam DEF_MAGIC_FIREBALL = { MAGIC_FIREBALL, MAGIC_TYPE_ATTACK, 3, 10, 5 } ;

class CInitalStatus {
  public:
	CInitalStatus();
	bool GetStatus(const char *name, CStatus *param);

  public:
	CParamFile	Params;
} ;

//
// CInitalStatus的对象实例(instance)
//
static CInitalStatus InitalStatus;

//
// 构造函数
//
CInitalStatus::CInitalStatus()
{
	Params.Load(PARAMETERFILE);
}

//
// 读入状态
//
bool CInitalStatus::GetStatus(const char *name, CStatus *param)
{
	const CParamFile::parameter *p = Params.Find(name);
	if (p == 0)		// 无此档案
		return false;

	strncpy(param->name, p->name, 16);			// 人物名称
	param->level = p->level;					// 等级
	param->experience = 0;						// 经验值
	param->move_dist = p->move_dist;			// 移动距离
	param->attack_dist = p->attack_dist;		// 攻击距离
	param->attack_power = p->attack_power;		// 攻击力
	param->magic_power = p->magic_power;		// 魔法力
	param->defence_power = p->defence_power;	// 防御力
	param->resistance = p->resistance;			// 抵抗力
	param->max_hit_point = p->hit_point;		// max HP
	param->max_magic_point = p->magic_point;	// max MP
	param->hit_point = param->max_hit_point;	// HP
	param->magic_point = param->max_magic_point; // MP

	param->nmagic = 0;							// 使用魔法数
	switch (p->magic) {
	  case 1:		// Heal
		param->nmagic = 1;						// 使用魔法数
		param->magic[0] = DEF_MAGIC_HEAL;		// 魔法
		break;

	  case 2:		// Fireball
		param->nmagic = 1;						// 使用魔法数
		param->magic[0] = DEF_MAGIC_FIREBALL;	// 魔法
		break;
	}
	return true;
}

bool CParams::GetStatus(const char *name, CStatus *_status)
{
	// 已登录过的时候
	for (int i=0; i<nstatus; i++) {
		if (strcmp(status[i].name, name) == 0) {
			*_status = status[i];
			_status->hit_point = _status->max_hit_point;	// HP
			_status->magic_point = _status->max_magic_point; // MP
			return true;
		}
	}
	// 从初始信息清单中读取
	return InitalStatus.GetStatus(name, _status);
}

void CParams::SetStatus(const char *name, const CStatus *_status)
{
	for (int i=0; i<nstatus; i++) {
		if (strcmp(status[i].name, name) == 0) {
			status[i] = *_status;
			return;
		}
	}
	status[nstatus++] = *_status;
}
