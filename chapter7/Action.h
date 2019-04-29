//
// ʹ���ߵĶ���(����)
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__action_h__
#define	__action_h__

class CMainWin;

//
//	�������
//
//	�Ѵ����粻���ڷ������¼��ȣ���
//	�Ӹ���״����д�����CMainWin���ó���
//
class CAction {
  public:
	CAction(CMainWin *win, CAction *oldAction=0): Parent(win), OldAction(oldAction) {}
	virtual ~CAction();

	virtual	void Pause();
	virtual	void Resume();

	virtual	void LButtonDown(UINT modKeys, CPoint point);
	virtual	void LButtonUp(UINT modKeys, CPoint point);
	virtual	void RButtonDown(UINT modKeys, CPoint point);
	virtual	void RButtonUp(UINT modKeys, CPoint point);
	virtual	void MouseMove(UINT modKeys, CPoint point);
	virtual	void KeyDown(UINT key);
	virtual bool TimedOut(int timerId);
	virtual BOOL IdleAction();
	virtual void MusicDone(int music);
	virtual void WipeDone();
	virtual void WaveDone();
	virtual BOOL IsSaveLoadOK() const;
	virtual void Abort();

	CAction *GetOldAction() const { return OldAction; }

  protected:
	CMainWin   *Parent;
	CAction    *OldAction;
} ;

#endif
