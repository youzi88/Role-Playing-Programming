//
// �ʻ�����Ĳ���
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//

#ifndef	__lexer_h__
#define	__lexer_h__

//
// �����������Ľṹ��
//
struct LexValue	{
	string value;
	int type;
} ;

//
// Lexer���
//
class Lexer	{
  public:
	// �Ǻ�����
	enum	{
		IsNumber,
		IsString,
		IsDelimitter,
		IsLabel,
		IsMinus,
	} ;

  private:
	enum	{
		IsSpace = IsMinus + 1,
		IsTerminater,
		IsQuotation,
	} ;

  public:
	Lexer(const char *str);

	int NumToken() const { return nToken; }
	void NextToken() { Count++; }

	const char *GetString(int index=-1);
	bool GetValue(int *value, int index=-1);
	int GetType(int index=-1);

  protected:
	const char *SkipSpace(const char *p);
	int CharType(unsigned char ch);
	bool MatchType(int &type, unsigned char ch);

  protected:
	int			nToken;
	vector<LexValue> Value;
	int			Count;
} ;

// inline��Ա����

inline const char *Lexer::GetString(int index)
{
	if (index >= 0)
		Count = index;
	if (Count >= nToken)
		return NULL;
	return Value[Count++].value.c_str();
}

inline int	Lexer::GetType(int index)
{
	if (index >= 0)
		Count = index;
	if (Count >= nToken)
		return -1;
	return Value[Count].type;
}

#endif
