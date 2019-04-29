//
// �ɮת���X�P��J
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef __FILE_H__
#define __FILE_H__

#include <io.h>
#include <stdio.h>
#include <share.h>

//
//	CFile Class
//
class CFile {
  public:
	enum	{
		read,
		write,
	} ;
	enum	{
		begin = SEEK_SET,
		current = SEEK_CUR,
		end = SEEK_END,
	} ;

  public:
	CFile(): fp(0) {}
	CFile(const char *file, int mode=read);
	~CFile();

	bool Open(const char *file, int mode=read);
	bool Close();

	int Read(void *buffer, int length);
	int Write(const void *buffer, int length);
	long Seek(long offset, int from);
#ifdef _MSC_VER
	long GetFileSize() { return _filelength(_fileno(fp)); }
#else
  #ifdef __BORLANDC__
	long GetFileSize() { return filelength(_fileno(fp)); }
  #endif
#endif

	bool IsOk() const { return fp != 0; }
	bool Error() { return ferror(fp) != 0; }

	bool operator !() { return fp == 0 || ferror(fp) != 0; }

  protected:
	FILE *fp;
} ;

// inline�����禡

// �غc��
//
// �P�H�ɮ׶}�Ҫ��غc
//
inline CFile::CFile(const char *file, int mode)
{
	Open(file, mode);
}

//
// �Ѻc��
//
// �p�G�٨S�����A�N������
//
inline CFile::~CFile()
{
	Close();
}

//
// �}���ɮ�
//
// Ū�M�g���Ҧ�
//
inline bool CFile::Open(const char *file, int mode)
{
#ifdef _MSC_VER
	fp = _fsopen(file, mode == read? "rb": "wb", _SH_DENYWR);
#else
	fp = fopen(file, mode == read? "rb": "wb");
#endif
	return fp != 0;
}

//
// Closing
//
inline bool CFile::Close()
{
	if (fp != 0) {
		fclose(fp);
		fp = 0;
	}
	return TRUE;
}

//
// Reading
//
inline int CFile::Read(void *buffer, int length)
{
	return fread(buffer, 1, length, fp);
}

//
// Writing
//
inline int CFile::Write(const void *buffer, int length)
{
	return fwrite(buffer, 1, length, fp);
}

//
// �����ɮת��s���m
//
inline long CFile::Seek(long offset, int from)
{
	return fseek(fp, offset, from);
}

#endif
