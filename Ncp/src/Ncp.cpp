////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file Ncp.cpp
/// @brief Ncp����ʵ�ִ���
///
/// ������̬�����ӡ�Ncp�汾���������
///
/// @version 1.0
/// @author  ���غ�
/// @date    2013��12��1��
///
///
/// �޶�˵��������汾
///
////////////////////////////////////////////////////////////////////////

#include "Ncp.h"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "IoService.lib")
#endif

namespace Ncp
{
    /// ���ذ汾����
    const static Version version = 
    { 
        2,  // ���汾��
        1,  // �ΰ汾��
        8,  // �޶��汾��
        14, // �꣨��2000��
        10,  // �£�[1, 12]��
        29  // �գ�[1, 31]��
    };

    // ��ȡ����NCP�汾
    NCP_API const Version & GetVersion() { return version; }
};

#if defined(_WIN32) && defined(NCP_EXPORTS)

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif