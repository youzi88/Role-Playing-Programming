//
// ������
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "ParamFile.h"
using namespace std;

//
// �ӵ�����ȡ����
//
bool CParamFile::Load(const char *name)
{
	FILE *fp = fopen(name, "rb");
	if (fp == 0)
		return false;

	long count;
	if (fread(&count, 1, sizeof(count), fp) == sizeof(count)) {
		for (long i=0; i<count; i++) {
			parameter p;
			if (fread(&p, 1, sizeof(p), fp) == sizeof(p)) {
				params[p.name] = p;
			}
		}
	}
	fclose(fp);

	return true;
}

//
// �Ѳ���д������
//
bool CParamFile::Save(const char *name)
{
	FILE *fp = fopen(name, "wb");
	if (fp == 0)
		return false;

	long count = params.size();
	fwrite(&count, 1, sizeof(count), fp);
	for (params_t::iterator p = params.begin(); p != params.end(); ++p) {
		fwrite(&p->second, 1, sizeof(p->second), fp);
	}
	fclose(fp);

	return true;
}

//
// ��Ѱ�ڲ�������趨��������Ӧ�Ĳ���
//
const CParamFile::parameter *CParamFile::Find(const char *name)
{
	params_t::iterator p = params.find(name);
	if (p == params.end())	// �޴˲���
		return 0;
	return &p->second;
}

//
// ��¼���������
//
void CParamFile::Add(const char *name, const CParamFile::parameter &param)
{
	params[name] = param;
}

//
// ׼������һ����
//
void CParamFile::FindFirst()
{
	iter = params.end();
}

//
// ����1������
//
bool CParamFile::FindNext(char *name)
{
	if (iter == params.end())
		iter = params.begin();
	else
		++iter;
	if (iter == params.end())
		return false;

	strncpy(name, iter->second.name, 16);
	return true;
}
