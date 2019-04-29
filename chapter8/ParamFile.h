//
// 参数档
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
		char	name[16];			// 人物名称
		short	level;				// 等级
		short	experience;			// 经验值
		short	move_dist;			// 移动距离
		short	attack_dist;		// 攻击距离
		short	attack_power;		// 攻击力
		short	magic_power;		// 魔法力
		short	defence_power;		// 防御力
		short	resistance;			// 抵抗力
		short	hit_point;			// HP
		short	magic_point;		// MP
		short	magic;				// 使用魔法
		short	filler;				// 补满
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
