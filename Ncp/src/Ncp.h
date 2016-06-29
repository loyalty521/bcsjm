////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file Ncp.h
/// @brief Ncp协议实现库导入/导出文件
///
/// 导入/导出Ncp协议
///
/// @version 1.0
/// @author  李守红
/// @date    2013年12月1日
///
///
/// 修订说明：最初版本
///
////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _NCP_NCP_H
#define _NCP_NCP_H

// 在Win32下定义__func__宏
#ifndef __func__
#   define __func__ __FUNCTION__
#endif

#if defined(_WIN32) && (defined(NCP_EXPORTS) || defined(NCP_DYNAMIC_LINK))
#   ifdef NCP_EXPORTS
#       define NCP_API __declspec(dllexport)
#		pragma warning(disable: 4251)
#   else
#       define NCP_API __declspec(dllimport)
#   endif
#else
#   define NCP_API
#endif

#if !defined(_MemLeakCheckHelper_DEFINED) && defined(_DEBUG ) && defined(_WIN32)// && defined(NCP_EXPORTS)
#define _MemLeakCheckHelper_DEFINED

#include <crtdbg.h>

static struct MemLeakCheckHelper
{
	MemLeakCheckHelper()
	{
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	}
} __memCheckHelper;

//#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// gcc 4.9以下版本的steady_clock缺省编译（编译GCC）时，实际为system_clock，因此需要替换std::chrono::steady_clock::now
#ifndef __STEADY_CLOCK_NOW_DEFIEND__
#   define __STEADY_CLOCK_NOW_DEFIEND__
#   if !defined(__GNUC__) || (__GNUC__ >= 4 && __GNUC_MINOR__ >= 9)
#       define steady_clock_now() std::chrono::steady_clock::now()
#   else
#       include <chrono>

        static inline std::chrono::steady_clock::time_point steady_clock_now()
        {
            timespec ts;
            clock_gettime( CLOCK_MONOTONIC, &ts);

            long long resolution =  std::chrono::steady_clock::period::den / std::chrono::steady_clock::period::num;
            
            return std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration(
              static_cast<std::chrono::steady_clock::rep>(ts.tv_sec) * resolution + ts.tv_nsec / (1000000000ll / resolution)));   
        }
#   endif
#endif

#include <cassert>
#include <cstring>

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>

#ifdef _UNICODE
    typedef std::wstring tstring;
    typedef wchar_t tchar;

#   ifndef _T 
#       define _T(x) L##x
#   endif
#else
    typedef std::string tstring;;
    typedef char tchar;

#   ifndef _T
#       define _T(x) x
#   endif
#endif

#include "Socket.h"  //Socket::Address

namespace Ncp
{
#pragma pack(push)
#pragma pack(1)
    /// 版本定义
    struct NCP_API Version {
        unsigned int   major : 4;      ///< 主要版本
        unsigned int   minor : 4;      ///< 次版本
        unsigned int   revision : 8;   ///< 修订版本
        unsigned int   year : 7;       ///< 年（自2000）
        unsigned int   month : 4;      ///< 月（[1, 12]）
        unsigned int   day : 5;        ///< 日（[1, 31]）

        /**
         * @brief 兼容性判断
         * @param [in] v 客户端版本
         * @return 兼容返回true，否则返回false
         */
        bool Compatible(const Version & v) const
        {
            return major == v.major && (minor > v.minor || (minor == v.minor && revision >= v.revision));
        }

        /**
         * @brief 获取版本的字符串形式
         * @param [in] withBuild 指示是否包含Build日期信息
         * @return 返回字符串
         */
#ifdef _UNICODE
        template <typename Ch=wchar_t>
#else
        template <typename Ch=char>
#endif
        std::basic_string<Ch> to_string(bool withBuild = true) const
        {
            std::basic_ostringstream<Ch> oss;

            oss << major << "." << minor << "." << revision;
            
            if (withBuild)
            {
                oss << " Build 20";
                oss << std::setw(2) << std::setfill((Ch)'0') << year << "-";
                oss << std::setw(2) << std::setfill((Ch)'0') << month << "-";
                oss << std::setw(2) << std::setfill((Ch)'0') << day;
            }

            return oss.str();
        }

        /**
         * @brief 从字符串构建版本对象
         * @return 返回版本对象
         */
#ifdef _UNICODE
        template <typename Ch=wchar_t>
#else
        template <typename Ch=char>
#endif
        static Version from_string(const std::basic_string<Ch> & str)
        {
            std::basic_istringstream<Ch> iss(str);
            Ch splitter;

            unsigned int major, minor, revision, year = 0, month = 0, day = 0;
            iss >> major >> splitter >> minor >> splitter >> revision;
            
            if (str.find("Build") != std::basic_string<Ch>::npos)
            {
                std::basic_string<Ch> builds;
                iss >> splitter >> builds >> splitter >> year >> splitter >> month >> splitter >> day;
            }

            return { major, minor, revision, year, month, day };
        }

    };
#pragma pack(pop)

#ifdef _WIN32
    union NCP_API IoService::Socket::Address;
#endif

    typedef IoService::Socket::Address Address;

    /// 错误号，可以通过std::to_string获取错误号对应的字符串形式
    /// 添加新的错误号时，请记得在下面重载函数std::to_string(ErrorCode)处添加响应代码！！！
    enum class ErrorCode : unsigned short
    {
        OK                      = 0,    ///< 正常
        InvalidCRC              = 1,    ///< CRC校验失败
        OperateNotSupport       = 2,    ///< 不支持操作
        ExecuteFailed           = 3,    ///< 执行失败
        ReceiverIsBusy          = 4,    ///< 接收方忙
        InvalidAppUnitLength    = 5,    ///< 应用数据单元长度不正确
        InvalidObjectCount      = 6,    ///< 信息对象数目不正确
        InvalidDeviceType       = 7,    ///< 设备类型不正确
        PacketTimeout           = 8,    ///< 数据包过期
        InvalidObjectData       = 9,    ///< 信息对象数据无效
        //10~999

        RequestNotFound         = 1000,  ///< 未找到对应ID的请求项
        UnknownDeviceType       = 1001,  ///< 未知设备类型
        VersionNotSupport       = 1002,  ///< 协议版本不支持
        SerialNumberExists      = 1003,  ///< 序列号重复（已注册具有同样序列号的设备）
        EncryptError            = 1004,  ///< 加密字段错误（仅客户端使用）
        InvalidData             = 1005,  ///< 数据无效
        PacketNotComplete       = 1006,  ///< 数据包不完整
        DeviceDisconnected      = 1007,  ///< 设备已断开
        RequestTimeout          = 1008,  ///< 请求超时
        CreateDeviceFailed      = 1009,  ///< 创建设备失败
    };

    // 添加新的设备类型时：
    //  1.在DeviceTypeId中添加响应设备类型定制
    //  2.在DeviceType::GetList中的map初始化列表中添加相应定义
    //  3.修改DeviceType中相关设备类型判断函数；
    //  4.把设备相关的结构体定义添加到NcpDevices.h/NcpDevices.cpp中
    //  5.把设备实现代码添加到Devices文件夹下面

    /// 设备类型定义
    enum class DeviceTypeId : unsigned short
    {
        /// 未知设备类型
        UnkownDeviceType                    = 0x0000,   
        /// 图像型火焰火灾探测器
        VideoFlameFireDetector              = 0x0601,   
        /// 图像型烟火复合火灾探测器
        VideoSFCompositeFireDetector        = 0x0602,   
        /// 线型光束感烟火灾探测器
        LinearBeamSmokeFireDetector         = 0x0603,   
        /// 可视化线型光束感烟火灾探测器
        VisualLinearBeamSmokeFireDetector   = 0x0604,   
        /// 室外型图像型火焰火灾探测器
        VideoFlameFireDetectorOD            = 0x0605,   
        /// 室外型图像型烟火复合火灾探测器
        VideoSFCompositeFireDetectorOD      = 0x0606,   
        /// 防爆型图像型火焰火灾探测器
        VideoFlameFireDetectorEx            = 0x0607,   
        /// 防爆型图像型烟火复合火灾探测器
        VideoSFCompositeFireDetectorEx      = 0x0608,   
        /// 防爆型线型光束感烟火灾探测器
        LinearBeamSmokeFireDetectorEx       = 0x0609,   
        /// 防爆型可视化线型光束感烟火灾探测器
        VisualLinearBeamSmokeFireDetectorEx = 0x060A,
		/// 网络视频解码器
		NetworkVideoRecorder				= 0x0700,
        /// 自动消防炮解码器
        HydrantDecoder                      = 0x0519,   
        /// 射流灭火装置解码器
        JettingHydrantDecoder               = 0x0421,   
        /// 小炮区域控制箱
        JettingHydrantDomainCtrlBox         = 0x050A,
		/// 单输入单输出通用模块
		GeneralModule1In1Out                = 0x0420,
		/// 双输入双输出通用模块
		GeneralModule2In2Out                = 0x0422,
		/// 3输入双输出通用模块
		GeneralModule3In2Out                = 0x0423,
        /// 图像型射流灭火装置解码器
        VideoJettingHydrantDecoder          = 0x0424,
        /// 电控炮系统泵模块
        EleHydrantPumpModule                = 0x0425,
        /// 二代自动消防炮解码器
        HydrantDecoder2                     = 0x0530,
		/// 网络视频解码器
		NVR                                 = 0x0700,
		/// 多参量探测器
		MULTI_DET                           = 0xFFFF,
		/// 多参量无线探测器
		MULTI_DET_WIRELESS                  = 0xFFFE,
		/// 气体泄漏探测器
		GAS_DET                             = 0xFFFD
    };

    /// 设备类型
    class NCP_API DeviceType
    {
    public:
        /**
         * @brief 从设备类型标志和设备类型名称构造类型实例
         * @param [in] typeId 设备类型标志
         * @param [in] name   设备类型名称（必须是字面常量）
         */
        DeviceType(DeviceTypeId typeId, const tchar * name) :
            m_typeId(typeId),
            m_name(name)
        {
        }

        /// 不允许使用的构造函数，只允许使用字面常量字符串指定设备类型名称
        DeviceType(DeviceTypeId, const tstring &) = delete;

        /// 缺省构造
        DeviceType() = default;

        /// 缺省copy构造
        DeviceType(const DeviceType &) = default;

        /// 缺省copy赋值
        DeviceType & operator = (const DeviceType &) = default;

        /**
         * @brief 获取设备类型对应的类型ID
         * @return 返回类型ID
         */
        DeviceTypeId GetId() const { return m_typeId; }

        /// 是否是图像型火灾探测器
        static bool IsVideoFireDetector(DeviceTypeId typeId)
        {
            return typeId == DeviceTypeId::VideoFlameFireDetector ||
                typeId == DeviceTypeId::VideoSFCompositeFireDetector ||
                typeId == DeviceTypeId::LinearBeamSmokeFireDetector ||
                typeId == DeviceTypeId::VisualLinearBeamSmokeFireDetector ||
                typeId == DeviceTypeId::VideoFlameFireDetectorOD ||
                typeId == DeviceTypeId::VideoSFCompositeFireDetectorOD ||
                typeId == DeviceTypeId::VideoFlameFireDetectorEx ||
                typeId == DeviceTypeId::VideoSFCompositeFireDetectorEx ||
                typeId == DeviceTypeId::LinearBeamSmokeFireDetectorEx ||
                typeId == DeviceTypeId::VisualLinearBeamSmokeFireDetectorEx;
        }

        /// 是否是室外型设备
        static bool IsOutdoor(DeviceTypeId typeId)
        {
            return typeId == DeviceTypeId::VideoFlameFireDetectorOD ||
                typeId == DeviceTypeId::VideoSFCompositeFireDetectorOD;
        }

        /// 是否是防爆型设备
        static bool IsEx(DeviceTypeId typeId)
        {
            return typeId == DeviceTypeId::VideoFlameFireDetectorEx ||
                typeId == DeviceTypeId::VideoSFCompositeFireDetectorEx ||
                typeId == DeviceTypeId::LinearBeamSmokeFireDetectorEx ||
                typeId == DeviceTypeId::VisualLinearBeamSmokeFireDetectorEx;
        }
        
        /// 获取视频路数
        static uint32_t NeedVideoChannels(DeviceTypeId typeId)
        {
            switch(typeId)
            {
            case DeviceTypeId::VideoFlameFireDetector:
            case DeviceTypeId::VideoSFCompositeFireDetector:
            case DeviceTypeId::VisualLinearBeamSmokeFireDetector:
            case DeviceTypeId::VisualLinearBeamSmokeFireDetectorEx:
            case DeviceTypeId::VideoFlameFireDetectorOD:
            case DeviceTypeId::VideoFlameFireDetectorEx:
            case DeviceTypeId::VideoSFCompositeFireDetectorOD:
            case DeviceTypeId::VideoSFCompositeFireDetectorEx:
            case DeviceTypeId::HydrantDecoder:   
			case DeviceTypeId::HydrantDecoder2://add by tq 20150508
                return 2;
            
            case DeviceTypeId::LinearBeamSmokeFireDetectorEx:
            case DeviceTypeId::LinearBeamSmokeFireDetector:            
                return 1;
            
            default:
                return 0;
            }
        };
        
        /**
         * @brief 获取设备类型的名称
         * @return 返回设备类型名称
         */
        const tchar * GetName() const { return m_name; }

        /**
         * @brief 根据设备类型ID查找对应类型
         * @param [in] typeId 设备类型ID
         * @return 返回对应的设备类型对象，如果未找到则返回类型ID为UnkownDeviceType、名称为"未知设备类型"的设备类型对象
         */
        static DeviceType FromId(DeviceTypeId typeId)
        {
            auto it = GetList().find(typeId);

            if (it == GetList().end())
            {
                return { DeviceTypeId::UnkownDeviceType, _T("未知设备类型") };
            }
            else
            {
                return { it->first, it->second };
            }
        }

        /**
         * @brief 从设备类型名称得到设备类型对象
         * @param [in] str 设备类型名称
         * @return 返回对应的设备类型对象，如果未找到则返回类型ID为UnkownDeviceType、名称为"未知设备类型"的设备类型对象
         */
        static DeviceType FromName(const tstring & str)
        {
            auto it = std::find_if(GetList().begin(), GetList().end(), 
                                   [&str](decltype(*GetList().begin()) & v) { return str == v.second; });
            
            if (it == GetList().end())
            {
                return { DeviceTypeId::UnkownDeviceType, _T("未知设备类型") };
            }
            else
            {
                return { it->first, it->second };
            }
        }

        /**
         * @brief 获取指定设备类型ID对应的设备名称
         * @param [in] typeId 设备类型ID
         * @return 返回对应的设备类型名称，如果未找到则返回"未知设备类型"
         */
        static tstring GetTypeName(DeviceTypeId typeId)
        {
            auto it = GetList().find(typeId);
            return it == GetList().end() ? _T("未知设备类型") : it->second;
        }

        /**
         * @brief 获取所有已知设备类型
         * @return 返回设备类型映射表
         */
        static const std::map<DeviceTypeId, const tchar *> & All()
        {
            return GetList();
        }

        /**
         * @brief 判断是否是已知的设备类型
         * @return 如果是则返回true，否则返回false
         */
        static bool IsKnownDeviceType(DeviceTypeId typeId)
        {
            return GetList().find(typeId) != GetList().end();
        }

        /**
         * @brief 判断是否是已知的设备类型
         * @return 如果是则返回true，否则返回false
         */
        static bool IsKnownDeviceType(const tstring & str)
        {
            return GetList().end() != std::find_if(GetList().begin(), GetList().end(), 
                                   [&str](decltype(*GetList().begin()) & v) { return str == v.second; });
        }

    private:
        DeviceTypeId    m_typeId = DeviceTypeId::UnkownDeviceType;  ///< 类型ID
        const tchar *   m_name = _T("未知设备类型");                  ///< 类型名称

        /// 获取设备类型列表
        static std::map<DeviceTypeId, const tchar *> & GetList()
        {
            static std::map<DeviceTypeId, const tchar *> types = 
            {
                { DeviceTypeId::VideoFlameFireDetector,             _T("图像型火焰火灾探测器") },
                { DeviceTypeId::VideoSFCompositeFireDetector,       _T("图像型烟火复合火灾探测器") },
                { DeviceTypeId::LinearBeamSmokeFireDetector,        _T("线型光束感烟火灾探测器") },
                { DeviceTypeId::VisualLinearBeamSmokeFireDetector,  _T("可视化线型光束感烟火灾探测器") },
                { DeviceTypeId::VideoFlameFireDetectorOD,           _T("室外型图像型火焰火灾探测器") },
                { DeviceTypeId::VideoSFCompositeFireDetectorOD,     _T("室外型图像型烟火复合火灾探测器") },
                { DeviceTypeId::VideoFlameFireDetectorEx,           _T("防爆型图像型火焰火灾探测器") },
                { DeviceTypeId::VideoSFCompositeFireDetectorEx,     _T("防爆型图像型烟火复合火灾探测器") },
                { DeviceTypeId::LinearBeamSmokeFireDetectorEx,      _T("防爆型线型光束感烟火灾探测器") },
                { DeviceTypeId::VisualLinearBeamSmokeFireDetectorEx, _T("防爆型可视化线型光束感烟火灾探测器") },
				{ DeviceTypeId::NetworkVideoRecorder,				_T("网络视频解码器") },
                { DeviceTypeId::HydrantDecoder,                     _T("自动消防炮解码器") },
                { DeviceTypeId::JettingHydrantDecoder,              _T("射流灭火装置解码器") },
                { DeviceTypeId::JettingHydrantDomainCtrlBox,        _T("小炮区域控制箱") },
				{ DeviceTypeId::GeneralModule1In1Out,				_T("单输入单输出通用模块") },
				{ DeviceTypeId::GeneralModule2In2Out,				_T("双输入双输出通用模块") },
				{ DeviceTypeId::GeneralModule3In2Out,				_T("3输入双输出通用模块") },
                { DeviceTypeId::VideoJettingHydrantDecoder,         _T("图像型射流灭火装置解码器") },
                { DeviceTypeId::HydrantDecoder2,                    _T("二代自动消防炮解码器") },
				{ DeviceTypeId::MULTI_DET,                          _T("多参量探测器") },
				{ DeviceTypeId::MULTI_DET_WIRELESS,                 _T("多参量无线探测器") },
				{ DeviceTypeId::GAS_DET,                            _T("气体泄漏探测器") },     
				{ DeviceTypeId::HydrantDecoder2,                    _T("自动消防炮") },  //add by tq 20150508
				{ DeviceTypeId::NVR,                                _T("网络视频解码器") }
            };

            return types;
        }
    };

#pragma pack(push)
#pragma pack(1)

    /// 序列号定义
    struct NCP_API SerialNumber
    {
        /// 设备类型
        DeviceTypeId    type = DeviceTypeId::UnkownDeviceType;
        /// 生产年份
        unsigned char   year = 0;
        /// 生产月份
        unsigned char   month = 0;
        /// 流水号
        unsigned short  id = 0;
        
        /// 生成缺省构造函数
        SerialNumber() = default;

        /**
         * @brief 构造函数
         * @param [in] _type  设备类型
         * @param [in] _year  生产年份
         * @param [in] _month 生产月份
         * @param [in] _id    流水号
         */
        SerialNumber(DeviceTypeId _type, unsigned char _year, unsigned char _month, unsigned short _id) :
            type(_type),
            year(_year),
            month(_month),
            id(_id)
        {}

        /// 转字符串
#if defined(_UNICODE)
        template <typename CharType=wchar_t>
#else
        template <typename CharType=char>
#endif
        std::basic_string<CharType> to_string() const
        {
            std::basic_ostringstream<CharType> oss;
            oss << std::noshowbase << std::hex << std::setw(4) << std::setfill((CharType)'0') << (int)type << '-';
            oss << std::noshowbase << std::hex << std::setw(2) << std::setfill((CharType)'0') << (int)year << '-';
            oss << std::noshowbase << std::hex << std::setw(2) << std::setfill((CharType)'0') << (int)month << '-';
            oss << std::noshowbase << std::hex << std::setw(4) << std::setfill((CharType)'0') << (int)id;

            return oss.str();
        }

        /// 从字符串构造对象
#if defined(_UNICODE)
        template <typename CharType=wchar_t>
#else
        template <typename CharType=char>
#endif
        static SerialNumber from_string(const std::basic_string<CharType> & str)
        {
            unsigned short _type, _year, _month, _id;
            SerialNumber sn;

            std::basic_istringstream<CharType> iss(str);
            CharType splitter;
            iss >> std::hex >> _type >> splitter >> std::hex >> _year >> splitter >> std::hex >> _month >> splitter >> std::hex >> _id;
            
            return { DeviceTypeId(_type), (unsigned char)_year, (unsigned char)_month, _id };
        }

        /// 小于比较操作符
        bool operator < (const SerialNumber & sn) const
        {
            return memcmp(this, &sn, sizeof(sn)) < 0;
        }

        /// 等于比较操作符
        bool operator == (const SerialNumber & sn) const
        {
            return type == sn.type && year == sn.year && month == sn.month && id == sn.id;
        }

        /// 不等于比较操作符
        bool operator != (const SerialNumber & sn) const
        {
            return !operator == (sn);
        }
    };

#pragma pack(pop)

    /// 方法（控制单元命令字节）
    enum class Method : unsigned char
    {
        Ctrl    = 1,    ///< 控制
        Upload  = 2,    ///< 上传
        Ack     = 3,    ///< 确认
        Read    = 4,    ///< 查询
        Reply   = 5,    ///< 应答
        Deny    = 6,    ///< 否认
        Write   = 7     ///< 设置
    };

    /// 资源（应用单元信息类型标志）
    enum class Resource : unsigned char
    {
        SerialNumber            = 1,    ///< 序列号   rw
        Version                 = 2,    ///< 版本     r
        Status                  = 3,    ///< 状态     r
        SystemTime              = 4,    ///< 系统时间  rw
        AnalogValue             = 5,    ///< 模拟量值  r
        Parameters              = 6,    ///< 参数     rw
        AlarmPicture            = 7,    ///< 报警图片  ru
        OperationRecord         = 8,    ///< 运行记录  r
        AlgorithmList           = 9,    ///< 算法列表  rw
        AlgorithmParameters     = 10,   ///< 算法参数  rw
        Location                = 11,   ///< 物理位置  rw
        RecordStoreDevices      = 12,   ///< 视频录像存储设备 rw
        RecordList              = 13,   ///< 消防设施录像记录 r
        ChannelList             = 14,   ///< 消防设施视频通道列表
		TargetPicture			= 15,   ///< 定位图片  ru
        //16~199

        Reset                   = 200,  ///< 复位     c
        Reboot                  = 201,  ///< 重启
        CtrlRelay               = 202,  ///< 控制继电器
        Disable                 = 203,  ///< 屏蔽/解除屏蔽消防设施
        SetDefaultParameters    = 204,  ///< 恢复消防设施出厂设置
        CtrlHydrant             = 205,  ///< 控制自动跟踪定位射流灭火装置 
        CtrlRecordStoreDevices  = 206,  ///< 消防设施录像存储设备控制
        CtrlRecordFile          = 207,  ///< 消防设施录像文件操作
		CtrlRemote              = 208,  ///< 切换现场远程状态
        //209~255
    };

    /// 获取本地NCP版本
    NCP_API const Version & GetVersion();
};

namespace std
{
    /**
     * @brief 获取错误号对应的字符串描述
     * @return 返回描述字符串对象
     */
    template <typename Ch = char>
    inline std::basic_string<Ch> to_string(Ncp::ErrorCode ec)
    {
        std::basic_ostringstream<Ch> oss;

        switch(ec)
        {
        case Ncp::ErrorCode::OK:
            oss << "正常";
            break;
        case Ncp::ErrorCode::InvalidCRC:
            oss << "CRC校验失败";
            break;
        case Ncp::ErrorCode::OperateNotSupport:
            oss << "不支持操作";
            break;
        case Ncp::ErrorCode::ExecuteFailed:
            oss << "执行失败";
            break;
        case Ncp::ErrorCode::ReceiverIsBusy:
            oss << "接收方忙";
            break;
        case Ncp::ErrorCode::InvalidAppUnitLength:
            oss << "应用数据单元长度不正确";
            break;
        case Ncp::ErrorCode::InvalidObjectCount:
            oss << "信息对象数目不正确";
            break;
        case Ncp::ErrorCode::InvalidDeviceType:
            oss << "设备类型不正确";
            break;
        case Ncp::ErrorCode::PacketTimeout:
            oss << "数据包过期";
            break;
        case Ncp::ErrorCode::InvalidObjectData:
            oss << "信息对象数据无效";
            break;
 
        case Ncp::ErrorCode::RequestNotFound:
            oss << "未找到对应ID的请求项";
            break;
        case Ncp::ErrorCode::UnknownDeviceType:
            oss << "未知设备类型";
            break;
        case Ncp::ErrorCode::VersionNotSupport:
            oss << "协议版本不支持";
            break;
        case Ncp::ErrorCode::SerialNumberExists:
            oss << "序列号重复";
            break;
        case Ncp::ErrorCode::EncryptError:
            oss << "加密字段错误";
            break;
        case Ncp::ErrorCode::InvalidData:
            oss << "数据无效";
            break;
        case Ncp::ErrorCode::PacketNotComplete:
            oss << "数据包不完整";
            break;
        case Ncp::ErrorCode::DeviceDisconnected:
            oss << "设备已断开";
            break;
        case Ncp::ErrorCode::RequestTimeout:
            oss << "请求超时";
            break;
        case Ncp::ErrorCode::CreateDeviceFailed:
            oss << "创建设备失败";
            break;
        default:
            oss << "未知错误";
            break;
        }

        return oss.str();
    }
}

#ifndef _UNALIGNED_
#ifdef _WIN32
#define _UNALIGNED_ __declspec(align(1))
#else
#define _UNALIGNED_ __attribute__((aligned(1))
#endif
#endif

#include "NcpDevices.h"

#ifndef NCP_EXPORTS
#include "Server/Server.h"
#ifndef _WIN32
#include "Client/Client.h"
#endif //_WIN32
#endif //NCP_EXPORTS

#endif