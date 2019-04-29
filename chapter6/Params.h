//
// ����
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
// ħ��
//
enum	{
	MAGIC_HEAL,			// �ָ�
	MAGIC_FIREBALL,		// Fire ball
	SPECIAL_ATTACK,		// ���⹥��(��ɱ��)
} ;

enum	{
	MAGIC_TYPE_SELF,
	MAGIC_TYPE_ATTACK,
} ;

struct CMagicParam {
	int		id;					// ħ��������
	int		type;				// ħ��������
	int		dist;				// ��Ч��Χ
	int		power;				// Ч��
	int		use_mp;				// ʹ��MP
} ;

//
// �����ɫ��״̬
//

struct CStatus {
	char		name[16];			// ����
	int			level;				// �ȼ�
	int			experience;			// ����ֵ
	int			move_dist;			// �ƶ�����
	int			attack_dist;		// ��������
	int			attack_power;		// ������
	int			magic_power;		// ħ����
	int			defence_power;		// ������
	int			resistance;			// �ֿ���
	int			max_hit_point;		// max HP
	int			max_magic_point;	// max MP
	int			hit_point;			// HP
	int			magic_point;		// MP
	int			nmagic;				// ʹ��ħ����
	CMagicParam	magic[MAX_MAGIC];	// ħ��

  public:
	void Clear();					// ״̬���
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
// ��Ϸ�������
//
struct CParams	{
	CStatus			status[MAX_CHARACTER];

  public:
//	void Clear();

	// ս������
	void InitStatus();
	CStatus *GetStatus(const char *name);
} ;

#endif
