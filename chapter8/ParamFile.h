//
// ������
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__PARAMFILE_H__
#define	__PARAMFILE_H__

#include <string>
#include <map>

class CParamFile {
  public:
	struct parameter {
		char	name[16];			// ��������
		short	level;				// �ȼ�
		short	experience;			// ����ֵ
		short	move_dist;			// �ƶ�����
		short	attack_dist;		// ��������
		short	attack_power;		// ������
		short	magic_power;		// ħ����
		short	defence_power;		// ������
		short	resistance;			// �ֿ���
		short	hit_point;			// HP
		short	magic_point;		// MP
		short	magic;				// ʹ��ħ��
		short	filler;				// ����
	} ;

  public:
	bool Load(const char *name);
	bool Save(const char *name);

	const parameter *Find(const char *name);
	void Add(const char *name, const parameter &param);

	void FindFirst();
	bool FindNext(char *name);

  protected:
	typedef std::map<std::string, parameter> params_t;
	params_t params;
	params_t::iterator iter;
} ;

#endif
