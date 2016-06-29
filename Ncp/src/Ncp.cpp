////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file Ncp.cpp
/// @brief Ncp部分实现代码
///
/// 包含静态库链接、Ncp版本管理等内容
///
/// @version 1.0
/// @author  李守红
/// @date    2013年12月1日
///
///
/// 修订说明：最初版本
///
////////////////////////////////////////////////////////////////////////

#include "Ncp.h"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "IoService.lib")
#endif

namespace Ncp
{
    /// 本地版本定义
    const static Version version = 
    { 
        2,  // 主版本号
        1,  // 次版本号
        8,  // 修订版本号
        14, // 年（自2000）
        10,  // 月（[1, 12]）
        29  // 日（[1, 31]）
    };

    // 获取本地NCP版本
    NCP_API const Version & GetVersion() { return version; }
};

#if defined(_WIN32) && defined(NCP_EXPORTS)

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
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