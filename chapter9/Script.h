//
// 执行Script
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__script_h__
#define	__script_h__

#include "Action.h"
#include "Misc.h"
#include "Params.h"
#include "TextReader.h"
#include "Lexer.h"
#include "less.h"

class CMainWin;

//
// Script class
//
class CScriptAction: public CAction {
  protected:
	// 状态
	enum	{
		BreakGame = -1,
		Continue = 0,
		WaitNextIdle,
		WaitKeyPressed,
		WaitTimeOut,
		WaitMenuDone,
		WaitClickMenuDone,
		WaitWipeDone,
		WaitWaveDone,
	} ;

	// CG位置
	enum	{
		POSITION_BACK,
		POSITION_BACKONLY,
		POSITION_OVERLAP,
		POSITION_WORK,
	} ;

	// Update Code
	enum	{
		UPDATE_NOW,
		UPDATE_OVERLAP,
		UPDATE_WIPE,
	} ;

  public:
	// Mode flag
	enum	{
		MODE_SYSTEM,
		MODE_SCENARIO,
	} ;

  protected:
	typedef int (CScriptAction::*cmd_t)(Lexer &);
	typedef pair<const char *, cmd_t>	CmdTab;
	typedef map<const char *, cmd_t, ic_less>	cmdmap;
	typedef map<string, int, ics_less>	labelmap;

  public:
	CScriptAction(CMainWin *win, CAction *oldAction, int mode);
	~CScriptAction();

	virtual	void Pause();
	virtual	void Resume();

	virtual	void LButtonDown(UINT modKeys, CPoint point);
	virtual	void LButtonUp(UINT modKeys, CPoint point);
	virtual	void RButtonDown(UINT modKeys, CPoint point);
	virtual	void MouseMove(UINT modKeys, CPoint point);

	virtual	void KeyDown(UINT key);
	virtual bool TimedOut(int timerId);

	virtual BOOL IdleAction();
	virtual void WipeDone();
	virtual void WaveDone();

	virtual BOOL IsSaveLoadOK() const;
	virtual void Abort();

	BOOL Setup(CParams &param);
	BOOL Load(const char *name);

  protected:
	char *__cdecl Format(const char *fmt,...);
	bool LoadFile(const char *name);
	int GotoCommand(const char *label);
	int LoadGraphic(const char *file, int pos);
	int UpdateImage(int flag);
	int Clear(int pos);

	int Step();
	cmd_t ParseCommand(Lexer &lexer);
	int ParserString(const char *str);

	int SetLabel(Lexer &lexer);
	int SetCmd(Lexer &lexer);
	int GotoCmd(Lexer &lexer);
	int IfCmd(Lexer &lexer);
	int MenuCmd(Lexer &lexer);
	int ClickMenuCmd(Lexer &lexer);
	int ExecCmd(Lexer &lexer);
	int LoadCmd(Lexer &lexer);
	int UpdateCmd(Lexer &lexer);
	int ClearCmd(Lexer &lexer);
	int WaitCmd(Lexer &lexer);
	int MusicCmd(Lexer &lexer);
	int SoundCmd(Lexer &lexer);
	int TextCmd(Lexer &lexer);
	int ModeCmd(Lexer &lexer);
	int SystemCmd(Lexer &lexer);
	int StopmCmd(Lexer &lexer);
	int FadeInCmd(Lexer &lexer);
	int FadeOutCmd(Lexer &lexer);
	int WipeInCmd(Lexer &lexer);
	int WipeOutCmd(Lexer &lexer);
	int CutInCmd(Lexer &lexer);
	int CutOutCmd(Lexer &lexer);
	int WhiteInCmd(Lexer &lexer);
	int WhiteOutCmd(Lexer &lexer);
	int FlashCmd(Lexer &lexer);
	int ShakeCmd(Lexer &lexer);
	int BattleCmd(Lexer &lexer);
	int EndCmd(Lexer &lexer);

	void __cdecl Error(const char *fmt, ...);

	bool GetValue(Lexer &lexer, int *value);
	bool ChkTermination(const char *str);
	bool ChkKeyword(const char *str, const char *keyword);
	int GetPosition(const char *str);
	int GetUpdateType(const char *str);

  protected:
	BOOL		Pressed;
	int			MenuSelect;
	int			PlayMode;
	string		MenuAnser;

	TextReader *reader;
	int			status;
	cmdmap		cmd_table;
	labelmap	label_table;
} ;

// inline 成员函数

inline BOOL CScriptAction::IsSaveLoadOK() const
{
	return PlayMode != MODE_SYSTEM && (status == WaitKeyPressed || status == WaitMenuDone);
}

#endif
