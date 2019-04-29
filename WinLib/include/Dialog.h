//
// Framework (dialog)
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__dialog_h__
#define	__dialog_h__

//
// Dialog class
//
class CDialog {
  public:
	CDialog(): hDlg(0) {}

	int DoModal(UINT idRes, HWND hParent);
	HWND DoModeless(UINT idRes, HWND hParent);

	BOOL IsDialogMessage(LPMSG lpmsg);

	LONG SendDlgItemMessage(int nIDDlgItem, UINT Msg, WPARAM wParam=0, LPARAM lParam=0);
	BOOL CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton);

	void Show(BOOL flag);
	HWND GetDlgItem(int id);
	BOOL SetDlgItemText(int id, const char *str);

	void CenterWindow();

  protected:
	static BOOL CALLBACK _DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

  protected:
	HWND hDlg;
} ;

// inline¦¨­û¨ç¦¡

inline BOOL CDialog::IsDialogMessage(LPMSG lpmsg)
{
	return hDlg? ::IsDialogMessage(hDlg, lpmsg): FALSE;
}

inline LONG CDialog::SendDlgItemMessage(int nID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return ::SendDlgItemMessage(hDlg, nID, Msg, wParam, lParam);
}

inline BOOL CDialog::CheckRadioButton(int nIDFirst, int nIDLast, int nIDCheck)
{
	return ::CheckRadioButton(hDlg, nIDFirst, nIDLast, nIDCheck);
}

inline void CDialog::Show(BOOL flag)
{
	::ShowWindow(hDlg, flag? SW_SHOW: SW_HIDE);
}

inline HWND CDialog::GetDlgItem(int id)
{
	return ::GetDlgItem(hDlg, id);
}

inline BOOL CDialog::SetDlgItemText(int id, const char *str)
{
	return ::SetDlgItemText(hDlg, id, str);
}

#endif
