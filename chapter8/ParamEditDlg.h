// ParamEditDlg.h : header file
//

#if !defined(AFX_PARAMEDITDLG_H__3CD1FC0B_C28D_4C87_A66E_5919EB57D61F__INCLUDED_)
#define AFX_PARAMEDITDLG_H__3CD1FC0B_C28D_4C87_A66E_5919EB57D61F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParamFile.h"

/////////////////////////////////////////////////////////////////////////////
// CParamEditDlg dialog

class CParamEditDlg : public CDialog
{
// Construction
public:
	CParamEditDlg(CWnd* pParent = NULL);	// standard constructor

protected:
	void ChangeName();

// Dialog Data
	//{{AFX_DATA(CParamEditDlg)
	enum { IDD = IDD_PARAMEDIT_DIALOG };
	CComboBox	m_name;
	int		m_attack;
	int		m_attack_dist;
	int		m_defence;
	int		m_hp;
	int		m_level;
	int		m_magic_power;
	int		m_move_dist;
	int		m_mp;
	int		m_resistance;
	int		m_magic;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CParamFile params;

	// Generated message map functions
	//{{AFX_MSG(CParamEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnSelchangeName();
	afx_msg void OnAdd();
	afx_msg void OnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMEDITDLG_H__3CD1FC0B_C28D_4C87_A66E_5919EB57D61F__INCLUDED_)
