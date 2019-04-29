//
// Application
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__Sample_h__
#define	__Sample_h__

#include "Application.h"
#include "MainWin.h"

#define	WM_FIRSTACTION	(WM_USER + 1)

//
// 应用程序类别
//
class CSampleApp: public CWinApp {
  public:
	CSampleApp();
	~CSampleApp();

	BOOL InitInstance();

  protected:
	CMainWin	MainWin;
	HANDLE		hMutex;
} ;

#endif
