//
// 画面特效
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__effect_h__
#define	__effect_h__

class CMainWin;
class CDrawImage;
class CImage;

//
// 特效类别
//
class CViewEffect {
  public:
	CViewEffect(CMainWin *win, unsigned step, CDrawImage *dst, CImage *src=0,
		const CRect &rect=CRect(0, 0, WindowWidth, WindowHeight));

	bool Step(unsigned time);

  protected:
	virtual bool Step() = 0;

  protected:
	CMainWin   *Window;
	CDrawImage *Dst;
	CImage     *Src;

	UINT		TimeBase;
	int			EffectCnt;
	CRect		EffectRect;
	UINT		lastTime;
} ;

// inline成员函式

inline CViewEffect::CViewEffect(CMainWin *win, unsigned step, CDrawImage *dst, CImage *src, const CRect &rect)
	:Window(win), Dst(dst), Src(src), TimeBase(step), EffectRect(rect), EffectCnt(0), lastTime(0)
{
}

inline bool CViewEffect::Step(unsigned time)
{
	if (TimeBase <= time - lastTime) {
		lastTime = time;
		return Step();
	}
	return true;
}

//
// 特效
//
// 继承CViewEffect
//
// step（成员变量TimeBase）是以ms为单位来指定间隔的，
// 所以1秒8影格（Frame)的时候就指定为1000/8
//

//
// Wipe in
//
class EffectWipeIn: public CViewEffect {
  public:
	EffectWipeIn(CMainWin *win, CDrawImage *dst, CImage *src, const CRect &rect)
		:CViewEffect(win, 1000 / 8, dst, src, rect) {}

  protected:
	bool Step();
} ;

//
// Wipe out
//
class EffectWipeOut: public CViewEffect {
  public:
	EffectWipeOut(CMainWin *win, CDrawImage *dst, CImage *src)
		:CViewEffect(win, 1000 / 8, dst, src) {}

  protected:
	bool Step();
} ;

//
// Fade in
//
class EffectFadeIn: public CViewEffect {
  public:
	EffectFadeIn(CMainWin *win, CDrawImage *dst, CImage *src)
		:CViewEffect(win, 1000 / 16, dst, src) {}

  protected:
	bool Step();
} ;

//
// Fade out
//
class EffectFadeOut: public CViewEffect {
  public:
	EffectFadeOut(CMainWin *win, CDrawImage *dst, CImage *src)
		:CViewEffect(win, 1000 / 16, dst, src) {}

  protected:
	bool Step();
} ;

//
// White in
//
class EffectWhiteIn: public CViewEffect {
  public:
	EffectWhiteIn(CMainWin *win, CDrawImage *dst, CImage *src)
		:CViewEffect(win, 1000 / 16, dst, src) {}

  protected:
	bool Step();
} ;

//
// White out
//
class EffectWhiteOut: public CViewEffect {
  public:
	EffectWhiteOut(CMainWin *win, CDrawImage *dst, CImage *src)
		:CViewEffect(win, 1000 / 16, dst, src) {}

  protected:
	bool Step();
} ;

//
// Mix
//
class EffectMixFade: public CViewEffect {
  public:
	EffectMixFade(CMainWin *win, CDrawImage *dst, CImage *src, const CRect &rect)
		:CViewEffect(win, 1000 / 8, dst, src, rect) {}

  protected:
	bool Step();
} ;

//
// Flash
//
class EffectFlash: public CViewEffect {
  public:
	EffectFlash(CMainWin *win, CDrawImage *dst)
		:CViewEffect(win, 1000 / 24, dst) {}

  protected:
	bool Step();
} ;

//
// Shake
//
class EffectShake: public CViewEffect {
  public:
	EffectShake(CMainWin *win, CDrawImage *dst)
		:CViewEffect(win, 1000 / 24, dst) {}

  protected:
	bool Step();
} ;

#endif
