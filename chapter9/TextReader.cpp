//
// TextReader
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "TextReader.h"

//
// 构造函数
//
TextReader::TextReader(const char *name)
	:filename(name), read_index(0), buffer(0)
{
	CSecurityAttributes	sa;
	HANDLE hFile = ::CreateFile(name, GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD length = ::GetFileSize(hFile, 0);	                // 求出档案大小
		buffer = new char [length + 1];
		DWORD nread;
		if (!::ReadFile(hFile, buffer, length, &nread, 0) || length != nread) {	// 错误
			delete buffer;
			buffer = 0;
		}
		else {
			char *endptr = buffer + length;
			char *p = buffer;
			int lineno = 1;
			*endptr = '\0';		// 卫兵
			while (p < endptr && *p != '\027') {
				data.push_back(ScriptData(p, lineno));
				while (UC(*p) >= ' ' || *p == '\t')
					p++;

				if (*p == '\0')		// 最后没有换行码
					break;

				if (*p == '\r')		// Windows的跳行码是"/r/n"
					*p++ = '\0';	// 故字符串后面要以'/r'结尾
				if (*p == '\n')
					*p++ = '\0';
				lineno++;
			}
		}
		::CloseHandle(hFile);
	}
	data.push_back(ScriptData("end", 0));	// 最后面多加一个空的"end"
}

//
// 析构函数
//
TextReader::~TextReader()
{
	delete buffer;
}
