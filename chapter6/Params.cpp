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
