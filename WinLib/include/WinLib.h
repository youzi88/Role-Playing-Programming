#define	STRICT					// ��Windows.h�ұa�X���Y���ˬd�Ҧ�
#define WIN32_LEAN_AND_MEAN		// ����֥Ϊ��w�q�ǤJ
#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC		// �T�O�����{���X�ҭn�Ϊ��O����
#endif
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#define	DEBUG_NEW	new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define	DEBUG_NEW	new
#endif
#include <Application.h>
#include <Window.h>
#include <dc.h>
#include <Dialog.h>
#include <Geometry.h>
#include <Misc.h>

// �bVC++���A�o���X�s�����{���w
#ifdef	_MSC_VER
	#ifdef	_DEBUG
		#pragma comment(lib, "WinLibD.lib")
	#else
		#pragma comment(lib, "WinLib.lib")
	#endif
#endif
