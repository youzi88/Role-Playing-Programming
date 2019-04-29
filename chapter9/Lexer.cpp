//
// 脚本(Script)解析编译器
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//

#include "StdAfx.h"
#include <mbctype.h>
#include "Lexer.h"

//
// 忽略空白字符
//
const char *Lexer::SkipSpace(const char *p)
{
	while (*p && isspace(*p))
		p++;
	return p;
}

//
// 判断字符种类
//
int Lexer::CharType(unsigned char ch)
{
	if (ch == '\0' || ch == '\n')
		return IsTerminater;
	if (isdigit(ch))
		return IsNumber;
	if (isalpha(ch) || _ismbblead(ch) || ch == '_')
		return IsString;
	if (isspace(ch))
		return IsSpace;
	if (ch == '"')
		return IsQuotation;
	if (ch == '-')
		return IsMinus;
	return IsDelimitter;
}

//
// 字符种类是否相同？
//
bool Lexer::MatchType(int &type, unsigned char ch)
{
	int	t = CharType(ch);
	switch (type) {
	  case IsLabel:
		if (ch == '*')
			return true;
		// no break

	  case IsNumber:
		if (t == IsString)
			type = IsString;
		// no break

	  case IsString:
		return (t == IsString || t == IsNumber);
	}
	return type == t;
}

//
// 记号切割
//
Lexer::Lexer(const char *str)
{
	for (nToken = 0; ; nToken++) {
		str = SkipSpace(str);
		if (*str == '\0' || *str == ';')
			break;
		int type = CharType(*str);

		if (type == IsTerminater && type == IsSpace)
			break;

		LexValue	value;

		if (type == IsQuotation) {
			value.type = IsString;
			str++;
			while (*str && CharType(*str) != IsQuotation) {
				if (_ismbblead(*str))
					value.value += *str++;
				value.value += *str++;
			}
			if (*str)
				str++;
		}
		else {
			if (*str == '-' && CharType(str[1]) == IsNumber) {
				value.value += '-';
				value.type = IsMinus;
				str++;
			}
			else {
				if (*str == '*' && nToken == 0)
					type = IsLabel;

				value.type = type;
				while (*str && MatchType(type, *str)) {
					if (_ismbblead(*str))
						value.value += *str++;
					value.value += *str++;
				}
				if (value.type == IsNumber)
					value.type = type;
			}
		}
		Value.push_back(value);
	}
	Count = 0;
}

//
// 读入数值
//
bool Lexer::GetValue(int *value, int index)
{
	bool	minus = false;
	int type = GetType(index);
	if (type == IsMinus) {
		minus = true;
		NextToken();
		type = GetType();
	}
	if (type != IsNumber)
		return false;
	const char *p = GetString();
	if (p == 0)
		return false;

	char *q;
	int v = strtol(p, &q, 10);
	*value = minus? -v: v;

	return true;
}
