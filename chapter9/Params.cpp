//
// ����
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
// ��ħ����?
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
	// �����ȼ�
	level++;

	// UP��������20%
	experience = experience * 12 / 10;
	attack_power = attack_power * 12 / 10;
	magic_power = magic_power * 12 / 10;
	defence_power = defence_power * 12 / 10;
	resistance = resistance * 12 / 10;
	max_hit_point = max_hit_point * 12 / 10;
	max_magic_point = max_magic_point * 12 / 10;

	// �ָ�HP��MP
	hit_point = max_hit_point;
	magic_point = max_magic_point;
}

#define	HEADER_ID	"DATA"

//
// ��������
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
// ��������
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
// ������ʼ��
//
void CParams::Clear()
{
	memset(this, 0, sizeof(CParams));
}

//
// ��������ֵ
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
// �����ı���
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
// �����ɫ��״̬
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
// CInitalStatus�Ķ���ʵ��(instance)
//
static CInitalStatus InitalStatus;

//
// ���캯��
//
CInitalStatus::CInitalStatus()
{
	Params.Load(PARAMETERFILE);
}

//
// ����״̬
//
bool CInitalStatus::GetStatus(const char *name, CStatus *param)
{
	const CParamFile::parameter *p = Params.Find(name);
	if (p == 0)		// �޴˵���
		return false;

	strncpy(param->name, p->name, 16);			// ��������
	param->level = p->level;					// �ȼ�
	param->experience = 0;						// ����ֵ
	param->move_dist = p->move_dist;			// �ƶ�����
	param->attack_dist = p->attack_dist;		// ��������
	param->attack_power = p->attack_power;		// ������
	param->magic_power = p->magic_power;		// ħ����
	param->defence_power = p->defence_power;	// ������
	param->resistance = p->resistance;			// �ֿ���
	param->max_hit_point = p->hit_point;		// max HP
	param->max_magic_point = p->magic_point;	// max MP
	param->hit_point = param->max_hit_point;	// HP
	param->magic_point = param->max_magic_point; // MP

	param->nmagic = 0;							// ʹ��ħ����
	switch (p->magic) {
	  case 1:		// Heal
		param->nmagic = 1;						// ʹ��ħ����
		param->magic[0] = DEF_MAGIC_HEAL;		// ħ��
		break;

	  case 2:		// Fireball
		param->nmagic = 1;						// ʹ��ħ����
		param->magic[0] = DEF_MAGIC_FIREBALL;	// ħ��
		break;
	}
	return true;
}

bool CParams::GetStatus(const char *name, CStatus *_status)
{
	// �ѵ�¼����ʱ��
	for (int i=0; i<nstatus; i++) {
		if (strcmp(status[i].name, name) == 0) {
			*_status = status[i];
			_status->hit_point = _status->max_hit_point;	// HP
			_status->magic_point = _status->max_magic_point; // MP
			return true;
		}
	}
	// �ӳ�ʼ��Ϣ�嵥�ж�ȡ
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
