//
// TextReader
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "TextReader.h"

//
// ���캯��
//
TextReader::TextReader(const char *name)
	:filename(name), read_index(0), buffer(0)
{
	CSecurityAttributes	sa;
	HANDLE hFile = ::CreateFile(name, GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD length = ::GetFileSize(hFile, 0);	                // ���������С
		buffer = new char [length + 1];
		DWORD nread;
		if (!::ReadFile(hFile, buffer, length, &nread, 0) || length != nread) {	// ����
			delete buffer;
			buffer = 0;
		}
		else {
			char *endptr = buffer + length;
			char *p = buffer;
			int lineno = 1;
			*endptr = '\0';		// ����
			while (p < endptr && *p != '\027') {
				data.push_back(ScriptData(p, lineno));
				while (UC(*p) >= ' ' || *p == '\t')
					p++;

				if (*p == '\0')		// ���û�л�����
					break;

				if (*p == '\r')		// Windows����������"/r/n"
					*p++ = '\0';	// ���ַ�������Ҫ��'/r'��β
				if (*p == '\n')
					*p++ = '\0';
				lineno++;
			}
		}
		::CloseHandle(hFile);
	}
	data.push_back(ScriptData("end", 0));	// �������һ���յ�"end"
}

//
// ��������
//
TextReader::~TextReader()
{
	delete buffer;
}
