// ParamEdit.h : main header file for the PARAMEDIT application
//

#if !defined(AFX_PARAMEDIT_H__795CBC51_E07D_4AEB_84B6_805802B41E2D__INCLUDED_)
#define AFX_PARAMEDIT_H__795CBC51_E07D_4AEB_84B6_805802B41E2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CParamEditApp:
// See ParamEdit.cpp for the implementation of this class
//

class CParamEditApp : public CWinApp
{
public:
	CParamEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamEditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CParamEditApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMEDIT_H__795CBC51_E07D_4AEB_84B6_805802B41E2D__INCLUDED_)
