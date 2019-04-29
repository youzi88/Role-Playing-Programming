//
//	MCI���/CD-DA/Wave���
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include "Mci.h"
#include "Window.h"
#include "Misc.h"

//
// ����
//
bool CMci::Open(CWindow *window)
{
	Window = window;

	return true;
}

//
// �ر�
//
bool CMci::Close()
{
	return true;
}

//
// ���ţ��������룩
//
bool CMci::Play(int no)
{
	return false;
}

//
// ���ţ�������
//
bool CMci::Play(const char *name)
{
	return false;
}

//
// ���²���
//
bool CMci::Replay()
{
	return false;
}

//
// ֹͣ
//
bool CMci::Stop()
{
	return false;
}

//
// ��ʾ�����¼�
//
void CMci::MciErrorMessageBox(DWORD err)
{
	char	errstr[256];
	mciGetErrorString(err, errstr, sizeof(errstr));
	Window->MessageBox(errstr, errTitle);
}

//
// ����MCI��open����
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
// ����MCI��open����
//
bool CMci::MciOpen()
{
	char	command[128];
	sprintf(command, "open %s wait", device);
	return MciOpen(command);
}

//
// ����MCI��open����
//
bool CMci::MciOpen(const char *dev, const char *element)
{
	char	command[_MAX_PATH + 128];
	sprintf(command, "open \"%s!%s\" alias %s wait", dev, element, device);
	return MciOpen(command);
}

//
// ����MCI��close����
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
// ����MCI��play����
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
// ����MCI��stop����
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
// ����MCI��set����
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
// ����MCI��status����
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
// ���̻����Ƿ���ý�壿
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
// �������̻�
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
#ifdef	notdef		//�����̻���û��CD�����ִ���ʱ #ifdef -> #ifndef
					// �ԈႢ�Ȃ����A�f�[�^�g?�b�N�Ɂu���ʗp�t�@�C?�v�����Ă�����
					// ������`�F�b�N����Ɨǂ��ł��傤
			if (Window->MessageBox(ApplicationTitle "���b�c�h?�C�u�ɓ���Ă��������B",
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
// �رչ��̻�
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
// ��ʼ����CD��ֻ����ָ��������
//
bool CDAudio::Play(int no)
{
	if (!IsOpen)
		return false;

	Stop();
	PlayTrack = no;

	no++;		// ��������MixedCDΪ��׼�����Բ���1������

	// �����������
	// �趨ֻ���Ŵ˷�Χ

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
// ��ʼ����CD�����趨����һ����error��
//
bool CDAudio::Play(const char *)
{
	return false;
}

//
// ���²���CD
//
bool CDAudio::Replay()
{
	return Play(PlayTrack);
}

//
// ֹͣ����CD
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
// ����Wave
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
// ����Wave����ʵ����
//
bool WaveOut::Play(int)
{
	return false;
}

//
// ֹͣ����Wave
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
