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
// ��ɫ��״̬
//

#define	DEF_MAGIC_HEAL		{ MAGIC_HEAL, MAGIC_TYPE_SELF, 1, 15, 3 }
#define	DEF_MAGIC_FIREBALL	{ MAGIC_FIREBALL, MAGIC_TYPE_ATTACK, 3, 15, 3 }

static CStatus init_status[] = {	// ��ʼ������
	{
		"����",		// ����
		1,			// �ȼ�
		0,			// ����
		3,			// �ƶ�����
		1,			// ��������
		10,			// ������
		0,			// ħ����
		15,			// ������
		10,			// �ֿ���
		30,			// max HP
		20,			// max MP
		30,			// HP
		20,			// MP
		0,			// ʹ��ħ����
	},
	{
		"ħ��ʦ",	// ����
		1,			// �ȼ�
		0,			// ����
		3,			// �ƶ�����
		1,			// ��������
		5,			// ������
		10,			// ħ����
		10,			// ������
		20,			// �ֿ���
		25,			// max HP
		20,			// max MP
		25,			// HP
		20,			// MP
		1,			// ʹ��ħ����
		DEF_MAGIC_FIREBALL,	// ʹ��ħ��
	},
	{
		"���",		// ����
		1,			// �ȼ�
		0,			// ����
		2,			// �ƶ�����
		1,			// ��������
		1,			// ������
		20,			// ħ����
		8,			// ������
		15,			// �ֿ���
		25,			// max HP
		20,			// max MP
		25,			// HP
		20,			// MP
		1,			// ʹ��ħ����
		DEF_MAGIC_HEAL,	// ʹ��ħ��
	},
	{
		"����",		// ����
		1,			// �ȼ�
		0,			// ����
		3,			// �ƶ�����
		2,			// ��������
		8,			// ������
		10,			// ħ����
		10,			// ������
		12,			// �ֿ���
		20,			// max HP
		20,			// max MP
		20,			// HP
		20,			// MP
		1,			// ʹ��ħ����
		DEF_MAGIC_FIREBALL,		// ʹ��ħ��
	},
	{
		"��͹�",	// ����
		1,			// �ȼ�
		0,			// ����
		3,			// �ƶ�����
		1,			// ��������
		7,			// ������
		0,			// ħ����
		10,			// ������
		8,			// �ֿ���
		20,			// max HP
		20,			// max MP
		20,			// HP
		20,			// MP
		0,			// ʹ��ħ����
	},
} ;

CStatus *CParams::GetStatus(const char *name)
{
	// ׼���õ�¼
	{
		for (int i=0; i<MAX_CHARACTER; i++) {
			if (strcmp(status[i].name, name) == 0)
				return status + i;
		}
	}
	// ������ʼ������Ϣ
	{
		for (int i=0; i<sizeof(init_status) / sizeof(init_status[0]); i++) {
			if (strcmp(init_status[i].name, name) == 0)
				return init_status + i;
		}
	}
	return 0;
}
