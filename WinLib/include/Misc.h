//
//	miscellaneous
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__MISC_H__
#define	__MISC_H__

#include <mbctype.h>

//
// 各種(miscellaneous)函式
//
#ifndef	_MAX_PATH
#define	_MAX_PATH	260
#endif

#define	UC(c)	(unsigned char)(c)

typedef	unsigned char	byte_t;

//
// 常數
//
const	COLORREF	WhitePixel = RGB(255, 255, 255);
const	COLORREF	BlackPixel = RGB(0, 0, 0);
const	COLORREF	GrayPixel = RGB(128, 128, 192);
const	COLORREF	RedPixel = RGB(255, 96, 96);

//
// 完整路徑
//
BOOL	FullPath(char *path, const char *file);

//
// 亂數
//
inline int Rand(int fact) { return rand() % fact; }

//
// SECURITY_ATTRIBUTES
//
class CSecurityAttributes: public _SECURITY_ATTRIBUTES {
  public:
	CSecurityAttributes(void *sd=NULL, BOOL ih=FALSE)
	{
		nLength = sizeof(SECURITY_ATTRIBUTES);
		lpSecurityDescriptor = sd;
		bInheritHandle = ih;
	}
} ;

#ifdef _DEBUG

void __cdecl DebugPrintf(const char *fmt, ...);
#define	TRACE	::DebugPrintf

bool AssertFailed(const char *expr, const char *file, unsigned line);

#define	ASSERT(exp)	\
	do \
		if (!(exp) && AssertFailed(#exp, __FILE__, __LINE__)) \
			DebugBreak(); \
	while (0)

#else

inline void __cdecl DebugPrintf(const char *, ...) { }
#define	TRACE	1? (void)0: ::DebugPrintf

#define ASSERT(exp)	(void)(0)

#endif

#pragma warning(disable: 4127)

#endif
