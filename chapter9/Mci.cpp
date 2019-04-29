//
//	MCI类别/CD-DA/Wave类别
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "Mci.h"
#include "Window.h"
#include "Misc.h"

//
// 开启
//
bool CMci::Open(CWindow *window)
{
	Window = window;

	return true;
}

//
// 关闭
//
bool CMci::Close()
{
	return true;
}

//
// 播放（乐曲号码）
//
bool CMci::Play(int no)
{
	return false;
}

//
// 播放（档名）
//
bool CMci::Play(const char *name)
{
	return false;
}

//
// 重新播放
//
bool CMci::Replay()
{
	return false;
}

//
// 停止
//
bool CMci::Stop()
{
	return false;
}

//
// 显示错误事件
//
void CMci::MciErrorMessageBox(DWORD err)
{
	char	errstr[256];
	mciGetErrorString(err, errstr, sizeof(errstr));
	Window->MessageBox(errstr, errTitle);
}

//
// 发行MCI的open命令
//
bool CMci::MciOpen(const char *command)
{
	DWORD err;
	char	result[128];
	if ((err = mciSendString(command, result, sizeof(result), 0)) != 0) {
		MciErrorMessageBox(err);
		return false;
	}

	char *p;
	Id = strtol(result, &p, 0);

	return true;
}

//
// 发行MCI的open命令
//
bool CMci::MciOpen()
{
	char	command[128];
	sprintf(command, "open %s wait", device);
	return MciOpen(command);
}

//
// 发行MCI的open命令
//
bool CMci::MciOpen(const char *dev, const char *element)
{
	char	command[_MAX_PATH + 128];
	sprintf(command, "open \"%s!%s\" alias %s wait", dev, element, device);
	return MciOpen(command);
}

//
// 发行MCI的close命令
//
bool CMci::MciClose()
{
	Id = 0;

	char	command[128];
	sprintf(command, "close %s wait", device);
	if (mciSendString(command, NULL, 0, 0) != 0)
		return false;
	return true;
}

//
// 发行MCI的play命令
//
bool CMci::MciPlay(const char *request)
{
	char	command[256];
	sprintf(command, "play %s %s notify", device, request);

	DWORD	err;
	if ((err = mciSendString(command, NULL, 0, *Window)) != 0) {
		MciErrorMessageBox(err);
		return false;
	}
	return true;
}

//
// 发行MCI的stop命令
//
bool CMci::MciStop()
{
	char	command[128];
	sprintf(command, "stop %s wait", device);
	if (mciSendString(command, NULL, 0, 0) != 0)
		return false;
	return true;
}

//
// 发行MCI的set命令
//
bool CMci::MciSet(const char *request)
{
	char	command[128];
	sprintf(command, "set %s %s wait", device, request);

	DWORD	err;
	if ((err = mciSendString(command, NULL, 0, 0)) != 0) {
		MciErrorMessageBox(err);
		return false;
	}
	return true;
}

//
// 发行MCI的status命令
//
bool CMci::MciStatus(const char *request, char *result, int resultlen)
{
	char	command[128];
	sprintf(command, "status %s %s wait", device, request);

	DWORD err;
	if ((err = mciSendString(command, result, resultlen, 0)) != 0) {
		MciErrorMessageBox(err);
		return false;
	}
	return true;
}

//
// 光盘机里是否有媒体？
//
int CDAudio::MediaChk()
{
	char	result[128];
	if (!MciStatus("media present", result, sizeof(result)))
		return -1;
	if (stricmp(result, "true") == 0)
		return 0;
	return 1;
}

//
// 开启光盘机
//
bool CDAudio::Open(CWindow *win)
{
	CMci::Open(win);

	if (!MciOpen())
		return false;

	if (!MciSet("time format tmsf")) {
		MciClose();
		return false;
	}

	bool chkOk = false;

	do {
		switch (MediaChk()) {
		  case -1:
			MciClose();
			return false;

		  case 0:
			chkOk = true;
			break;

		  default:
#ifdef	notdef		//若光盘机中没有CD而出现错误时 #ifdef -> #ifndef
					// 娫堘偄側偄偐丄僨乕僞僩?僢僋偵乽幆暿梡僼傽僀?乿傪擖傟偰偍偄偰
					// 偦傟傪僠僃僢僋偡傞偲椙偄偱偟傚偆
			if (Window->MessageBox(ApplicationTitle "傪俠俢僪?僀僽偵擖傟偰偔偩偝偄丅",
				ApplicationTitle, MB_RETRYCANCEL) == IDCANCEL) {
				MciClose();
				return false;
			}
			break;
#else
			MciClose();
			return false;
#endif
		}
	} while (!chkOk) ;

	IsOpen = true;

	return true;
}

//
// 关闭光盘机
//
bool CDAudio::Close()
{
	if (IsOpen) {
		Stop();
		MciClose();
	}
	IsOpen = false;
	return true;
}

//
// 开始播放CD（只播放指定乐曲）
//
bool CDAudio::Play(int no)
{
	if (!IsOpen)
		return false;

	Stop();
	PlayTrack = no;

	no++;		// 这里是以MixedCD为基准，所以播放1首乐曲

	// 检查乐曲长度
	// 设定只播放此范围

	char	request[64];
	sprintf(request, "length track %d", no);

	char	result[128];
	if (!MciStatus(request, result, sizeof(result)))
		return false;

	sprintf(request, "from %d:00:00:00 to %d:%s", no, no, result);
	if (!MciPlay(request))
		return false;
	return true;
}

//
// 开始播放CD（若设定档名一定会error）
//
bool CDAudio::Play(const char *)
{
	return false;
}

//
// 重新播放CD
//
bool CDAudio::Replay()
{
	return Play(PlayTrack);
}

//
// 停止播放CD
//
bool CDAudio::Stop()
{
	if (!IsOpen)
		return false;

	if (PlayTrack > 0) {
		MciStop();
		PlayTrack = 0;
	}
	return true;
}

//
// 播放Wave
//
bool WaveOut::Play(const char *name)
{
	Stop();

	char	fullpath[_MAX_PATH];
	FullPath(fullpath, name);

	if (!MciOpen("waveaudio", fullpath))
		return false;

	if (!MciPlay(""))
		return false;

	playing = true;

	return true;
}

//
// 播放Wave（非实作）
//
bool WaveOut::Play(int)
{
	return false;
}

//
// 停止播放Wave
//
bool WaveOut::Stop()
{
	if (playing) {
		playing = false;
		MciStop();
		MciClose();
	}
	return true;
}
