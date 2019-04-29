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
enum	{
	THINK_ATTACK,				// ����ֱ�ӹ���
	THINK_ATTACK_ONLY,			// ֱֻ�ӹ���
	THINK_MAGIC,				// ����ħ������
	THINK_MAGIC_ONLY,			// ֻħ������
	THINK_WEAK_TARGET = 0,		// ���ȹ��������Ķ���
	THINK_NEAR_TARGET = 1 << 2,	// ���ȹ����Ͻӽ��Ķ���
} ;

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
	int			think_pattern;		// ˼��ģʽ
	int			nmagic;				// ʹ��ħ����
	CMagicParam	magic[MAX_MAGIC];	// ħ��


  public:
	void Clear();					// ״̬���
	int FindMagic(int id) const;	// ��ħ����?
	void LevelUp();
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
	unsigned char	save_month;
	unsigned char	save_date;
	unsigned char	save_hour;
	unsigned char	save_minute;
	unsigned long	script_pos;
	char			last_script[16];
	char			last_bg[16];
	char			last_overlap[16];
	char			last_work[16];
	unsigned long	last_bgm;
	unsigned long	show_flag;
	long			nvalue;
	struct	{
		char		name[16];
		long		value;
	} value_tab[PARAMS_MAX_VALUES];
	int				nstatus;
	CStatus			status[MAX_CHARACTER];

  public:
	// �ű�����
	void Clear();
	void ClearBackCG();
	void ClearOverlapCG();
	void ClearWorkCG();
	void SetBackCG(const char *file);
	void SetOverlapCG(const char *file);
	void SetWorkCG(const char *file);
	void SetShowFlag();
	void ResetShowFlag(BOOL white=FALSE);
	BOOL Load(int no);
	BOOL Save(int no);

	// ս������
	void InitStatus();
	bool GetStatus(const char *name, CStatus *status);
	void SetStatus(const char *name, const CStatus *status);
} ;

// inline��Ա����

// ȥ������CG
inline void CParams::ClearBackCG()
{
	last_bg[0] = 0;
}

// ȥ���ص�CG
inline void CParams::ClearOverlapCG()
{
	last_overlap[0] = 0;
	last_work[0] = 0;
}

// ����ҵ��CG
inline void CParams::ClearWorkCG()
{
	last_overlap[0] = 0;
	last_work[0] = 0;
}

// �趨����CG
inline void CParams::SetBackCG(const char *file)
{
	strcpy(last_bg, file);
}

// �趨�ص�CG
inline void CParams::SetOverlapCG(const char *file)
{
	strcpy(last_overlap, file);
	last_work[0] = 0;
}

	void SetWorkCG(const char *file);

// �趨��ҵ��CG
inline void CParams::SetWorkCG(const char *file)
{
	strcpy(last_work, file);
	last_overlap[0] = 0;
}

// �趨��ʾ���
inline void CParams::SetShowFlag()
{
	show_flag = SHOWCG_IMAGE;
}

// ɾ����ʾ�����
inline void CParams::ResetShowFlag(BOOL white)
{
	show_flag = white? SHOWCG_WHITENESS: SHOWCG_BLACKNESS;
}

#endif
