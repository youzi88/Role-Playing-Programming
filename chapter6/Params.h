//
// 参数
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__params_h
#define	__params_h

enum	{
	SHOWCG_BLACKNESS,
	SHOWCG_IMAGE,
	SHOWCG_WHITENESS,
} ;

//
// 魔法
//
enum	{
	MAGIC_HEAL,			// 恢复
	MAGIC_FIREBALL,		// Fire ball
	SPECIAL_ATTACK,		// 特殊攻击(必杀技)
} ;

enum	{
	MAGIC_TYPE_SELF,
	MAGIC_TYPE_ATTACK,
} ;

struct CMagicParam {
	int		id;					// 魔法的名称
	int		type;				// 魔法的类型
	int		dist;				// 有效范围
	int		power;				// 效果
	int		use_mp;				// 使用MP
} ;

//
// 人物角色的状态
//

struct CStatus {
	char		name[16];			// 名称
	int			level;				// 等级
	int			experience;			// 经验值
	int			move_dist;			// 移动距离
	int			attack_dist;		// 攻击距离
	int			attack_power;		// 攻击力
	int			magic_power;		// 魔法力
	int			defence_power;		// 防御力
	int			resistance;			// 抵抗力
	int			max_hit_point;		// max HP
	int			max_magic_point;	// max MP
	int			hit_point;			// HP
	int			magic_point;		// MP
	int			nmagic;				// 使用魔法数
	CMagicParam	magic[MAX_MAGIC];	// 魔法

  public:
	void Clear();					// 状态清除
} ;

inline void CStatus::Clear()
{
	name[0] = 0;
	level = 0;
	experience = 0;
	move_dist = 0;
	attack_dist = 0;
	attack_power = 0;
	magic_power = 0;
	defence_power = 0;
	resistance = 0;
	max_hit_point = 0;
	max_magic_point = 0;
	hit_point = 0;
	magic_point = 0;
	nmagic = 0;
}

//
// 游戏参数类别
//
struct CParams	{
	CStatus			status[MAX_CHARACTER];

  public:
//	void Clear();

	// 战斗参数
	void InitStatus();
	CStatus *GetStatus(const char *name);
} ;

#endif
