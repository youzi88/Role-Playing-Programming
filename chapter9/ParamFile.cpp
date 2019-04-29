//
// 参数档
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "StdAfx.h"
#include "ParamFile.h"
using namespace std;

//
// 从档案读取参数
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
// 把参数写进档案
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
// 搜寻内部表格中设定名称所对应的参数
//
const CParamFile::parameter *CParamFile::Find(const char *name)
{
	params_t::iterator p = params.find(name);
	if (p == params.end())	// 无此参数
		return 0;
	return &p->second;
}

//
// 登录到参数表格
//
void CParamFile::Add(const char *name, const CParamFile::parameter &param)
{
	params[name] = param;
}

//
// 准备名称一览表
//
void CParamFile::FindFirst()
{
	iter = params.end();
}

//
// 读出1个名称
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
