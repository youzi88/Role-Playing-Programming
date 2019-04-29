//
//	Application Framework
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __Application_h__
#define __Application_h__

#define	WM_KICKIDLE		0x036A

#include "Window.h"

//
//	Application 類別
//
class CWinApp {
  public:
	CWinApp();
	~CWinApp();

	void SetWinMainArgs(HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);

	BOOL RegisterWndClass(const char *name, UINT classStyle, HCURSOR hCursor=0,
		HBRUSH hbrBackground=(HBRUSH)(COLOR_WINDOW+1), HICON hIcon=0);

	virtual BOOL InitInstance();
	virtual BOOL OnIdle(long count);
	virtual BOOL PreTranslateMessage(MSG *msg);

	int Run();

	HRSRC FindResource(const char *name, const char *type);
	HGLOBAL LoadResource(HRSRC hres);
	HACCEL LoadAccelerators(const char *name);
	HMENU LoadMenu(const char *name);
	HMENU LoadMenu(int resId);
	HICON LoadIcon(const char *name);
	HICON LoadIcon(int resId);

	HINSTANCE GetInstance() const { return Instance; }
	operator HINSTANCE() const { return Instance; }

	CWindow *GetMainWnd() { return MainWnd; }

  protected:
	HINSTANCE Instance;
	LPSTR CmdLine;
	int CmdShow;

	MSG	msgCur;

	CWindow *MainWnd;
} ;

// inline 成員函數

inline void CWinApp::SetWinMainArgs(HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	Instance = hInstance;
	CmdLine = lpszCmdLine;
	CmdShow = nCmdShow;
}

inline HRSRC CWinApp::FindResource(const char *name, const char *type)
{
	return ::FindResource(Instance, name, type);
}

inline HGLOBAL CWinApp::LoadResource(HRSRC hres)
{
	return ::LoadResource(Instance, hres);
}

inline HACCEL CWinApp::LoadAccelerators(const char *name)
{
	return ::LoadAccelerators(Instance, name);
}

inline HMENU CWinApp::LoadMenu(const char *name)
{
	return ::LoadMenu(Instance, name);
}

inline HMENU CWinApp::LoadMenu(int resId)
{
	return ::LoadMenu(Instance, MAKEINTRESOURCE(resId));
}

inline HICON CWinApp::LoadIcon(const char *name)
{
	return ::LoadIcon(Instance, name);
}

inline HICON CWinApp::LoadIcon(int resId)
{
	return ::LoadIcon(Instance, MAKEINTRESOURCE(resId));
}

extern CWinApp *Application;

#endif
