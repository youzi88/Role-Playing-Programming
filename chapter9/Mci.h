//
//	MCI/CD-DA/Wave类别
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __MUSIC_H__
#define __MUSIC_H__

class CWindow;

//
//	MCI类别
//
class CMci	{
  public:
	CMci(const char *_device, const char *_errTitle);
	~CMci();

	virtual bool Open(CWindow *win);
	virtual bool Close();

	virtual bool Play(int no);
	virtual bool Play(const char *name);
	virtual bool Replay();
	virtual bool Stop();

	DWORD GetId() const { return Id; }

  protected:
	void MciErrorMessageBox(DWORD err);
	bool MciOpen(const char *command);
	bool MciOpen();
	bool MciOpen(const char *dev, const char *element);
	bool MciClose();
	bool MciPlay(const char *request);
	bool MciStop();
	bool MciSet(const char *request);
	bool MciStatus(const char *request, char *result, int resultlen);

  protected:
	CWindow *Window;
	const char *device;
	const char *errTitle;
	DWORD	Id;
} ;

// inline 成员函数

inline CMci::CMci(const char *_device, const char *_errTitle)
	: Id(0), device(_device), errTitle(_errTitle)
{
}

inline CMci::~CMci()
{
	Close();
}

//
//	CD音讯类别
//
class CDAudio: public CMci {
  public:
	CDAudio(): CMci("cdaudio", "CD音讯类别"), IsOpen(false), PlayTrack(0) {}

	virtual bool Open(CWindow *win);
	virtual bool Close();

	virtual bool Play(const char *name);
	virtual bool Play(int no);
	virtual bool Replay();
	virtual bool Stop();

  protected:
	int MediaChk();

  protected:
	int		PlayTrack;
	bool	IsOpen;
} ;

//
//	Wave类别
//
class WaveOut: public CMci {
  public:
	WaveOut(): CMci("sound", "Wave音讯error"), playing(false) {}

	virtual bool Play(const char *name);
	virtual bool Play(int no);
	virtual bool Stop();

  protected:
	bool	playing;
} ;

#endif
