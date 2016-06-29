////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file Ncp.h
/// @brief NcpЭ��ʵ�ֿ⵼��/�����ļ�
///
/// ����/����NcpЭ��
///
/// @version 1.0
/// @author  ���غ�
/// @date    2013��12��1��
///
///
/// �޶�˵��������汾
///
////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _NCP_NCP_H
#define _NCP_NCP_H

// ��Win32�¶���__func__��
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

// gcc 4.9���°汾��steady_clockȱʡ���루����GCC��ʱ��ʵ��Ϊsystem_clock�������Ҫ�滻std::chrono::steady_clock::now
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
    /// �汾����
    struct NCP_API Version {
        unsigned int   major : 4;      ///< ��Ҫ�汾
        unsigned int   minor : 4;      ///< �ΰ汾
        unsigned int   revision : 8;   ///< �޶��汾
        unsigned int   year : 7;       ///< �꣨��2000��
        unsigned int   month : 4;      ///< �£�[1, 12]��
        unsigned int   day : 5;        ///< �գ�[1, 31]��

        /**
         * @brief �������ж�
         * @param [in] v �ͻ��˰汾
         * @return ���ݷ���true�����򷵻�false
         */
        bool Compatible(const Version & v) const
        {
            return major == v.major && (minor > v.minor || (minor == v.minor && revision >= v.revision));
        }

        /**
         * @brief ��ȡ�汾���ַ�����ʽ
         * @param [in] withBuild ָʾ�Ƿ����Build������Ϣ
         * @return �����ַ���
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
         * @brief ���ַ��������汾����
         * @return ���ذ汾����
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

    /// ����ţ�����ͨ��std::to_string��ȡ����Ŷ�Ӧ���ַ�����ʽ
    /// ����µĴ����ʱ����ǵ����������غ���std::to_string(ErrorCode)�������Ӧ���룡����
    enum class ErrorCode : unsigned short
    {
        OK                      = 0,    ///< ����
        InvalidCRC              = 1,    ///< CRCУ��ʧ��
        OperateNotSupport       = 2,    ///< ��֧�ֲ���
        ExecuteFailed           = 3,    ///< ִ��ʧ��
        ReceiverIsBusy          = 4,    ///< ���շ�æ
        InvalidAppUnitLength    = 5,    ///< Ӧ�����ݵ�Ԫ���Ȳ���ȷ
        InvalidObjectCount      = 6,    ///< ��Ϣ������Ŀ����ȷ
        InvalidDeviceType       = 7,    ///< �豸���Ͳ���ȷ
        PacketTimeout           = 8,    ///< ���ݰ�����
        InvalidObjectData       = 9,    ///< ��Ϣ����������Ч
        //10~999

        RequestNotFound         = 1000,  ///< δ�ҵ���ӦID��������
        UnknownDeviceType       = 1001,  ///< δ֪�豸����
        VersionNotSupport       = 1002,  ///< Э��汾��֧��
        SerialNumberExists      = 1003,  ///< ���к��ظ�����ע�����ͬ�����кŵ��豸��
        EncryptError            = 1004,  ///< �����ֶδ��󣨽��ͻ���ʹ�ã�
        InvalidData             = 1005,  ///< ������Ч
        PacketNotComplete       = 1006,  ///< ���ݰ�������
        DeviceDisconnected      = 1007,  ///< �豸�ѶϿ�
        RequestTimeout          = 1008,  ///< ����ʱ
        CreateDeviceFailed      = 1009,  ///< �����豸ʧ��
    };

    // ����µ��豸����ʱ��
    //  1.��DeviceTypeId�������Ӧ�豸���Ͷ���
    //  2.��DeviceType::GetList�е�map��ʼ���б��������Ӧ����
    //  3.�޸�DeviceType������豸�����жϺ�����
    //  4.���豸��صĽṹ�嶨����ӵ�NcpDevices.h/NcpDevices.cpp��
    //  5.���豸ʵ�ִ�����ӵ�Devices�ļ�������

    /// �豸���Ͷ���
    enum class DeviceTypeId : unsigned short
    {
        /// δ֪�豸����
        UnkownDeviceType                    = 0x0000,   
        /// ͼ���ͻ������̽����
        VideoFlameFireDetector              = 0x0601,   
        /// ͼ�����̻𸴺ϻ���̽����
        VideoSFCompositeFireDetector        = 0x0602,   
        /// ���͹������̻���̽����
        LinearBeamSmokeFireDetector         = 0x0603,   
        /// ���ӻ����͹������̻���̽����
        VisualLinearBeamSmokeFireDetector   = 0x0604,   
        /// ������ͼ���ͻ������̽����
        VideoFlameFireDetectorOD            = 0x0605,   
        /// ������ͼ�����̻𸴺ϻ���̽����
        VideoSFCompositeFireDetectorOD      = 0x0606,   
        /// ������ͼ���ͻ������̽����
        VideoFlameFireDetectorEx            = 0x0607,   
        /// ������ͼ�����̻𸴺ϻ���̽����
        VideoSFCompositeFireDetectorEx      = 0x0608,   
        /// ���������͹������̻���̽����
        LinearBeamSmokeFireDetectorEx       = 0x0609,   
        /// �����Ϳ��ӻ����͹������̻���̽����
        VisualLinearBeamSmokeFireDetectorEx = 0x060A,
		/// ������Ƶ������
		NetworkVideoRecorder				= 0x0700,
        /// �Զ������ڽ�����
        HydrantDecoder                      = 0x0519,   
        /// �������װ�ý�����
        JettingHydrantDecoder               = 0x0421,   
        /// С�����������
        JettingHydrantDomainCtrlBox         = 0x050A,
		/// �����뵥���ͨ��ģ��
		GeneralModule1In1Out                = 0x0420,
		/// ˫����˫���ͨ��ģ��
		GeneralModule2In2Out                = 0x0422,
		/// 3����˫���ͨ��ģ��
		GeneralModule3In2Out                = 0x0423,
        /// ͼ�����������װ�ý�����
        VideoJettingHydrantDecoder          = 0x0424,
        /// �����ϵͳ��ģ��
        EleHydrantPumpModule                = 0x0425,
        /// �����Զ������ڽ�����
        HydrantDecoder2                     = 0x0530,
		/// ������Ƶ������
		NVR                                 = 0x0700,
		/// �����̽����
		MULTI_DET                           = 0xFFFF,
		/// ���������̽����
		MULTI_DET_WIRELESS                  = 0xFFFE,
		/// ����й©̽����
		GAS_DET                             = 0xFFFD
    };

    /// �豸����
    class NCP_API DeviceType
    {
    public:
        /**
         * @brief ���豸���ͱ�־���豸�������ƹ�������ʵ��
         * @param [in] typeId �豸���ͱ�־
         * @param [in] name   �豸�������ƣ����������泣����
         */
        DeviceType(DeviceTypeId typeId, const tchar * name) :
            m_typeId(typeId),
            m_name(name)
        {
        }

        /// ������ʹ�õĹ��캯����ֻ����ʹ�����泣���ַ���ָ���豸��������
        DeviceType(DeviceTypeId, const tstring &) = delete;

        /// ȱʡ����
        DeviceType() = default;

        /// ȱʡcopy����
        DeviceType(const DeviceType &) = default;

        /// ȱʡcopy��ֵ
        DeviceType & operator = (const DeviceType &) = default;

        /**
         * @brief ��ȡ�豸���Ͷ�Ӧ������ID
         * @return ��������ID
         */
        DeviceTypeId GetId() const { return m_typeId; }

        /// �Ƿ���ͼ���ͻ���̽����
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

        /// �Ƿ����������豸
        static bool IsOutdoor(DeviceTypeId typeId)
        {
            return typeId == DeviceTypeId::VideoFlameFireDetectorOD ||
                typeId == DeviceTypeId::VideoSFCompositeFireDetectorOD;
        }

        /// �Ƿ��Ƿ������豸
        static bool IsEx(DeviceTypeId typeId)
        {
            return typeId == DeviceTypeId::VideoFlameFireDetectorEx ||
                typeId == DeviceTypeId::VideoSFCompositeFireDetectorEx ||
                typeId == DeviceTypeId::LinearBeamSmokeFireDetectorEx ||
                typeId == DeviceTypeId::VisualLinearBeamSmokeFireDetectorEx;
        }
        
        /// ��ȡ��Ƶ·��
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
         * @brief ��ȡ�豸���͵�����
         * @return �����豸��������
         */
        const tchar * GetName() const { return m_name; }

        /**
         * @brief �����豸����ID���Ҷ�Ӧ����
         * @param [in] typeId �豸����ID
         * @return ���ض�Ӧ���豸���Ͷ������δ�ҵ��򷵻�����IDΪUnkownDeviceType������Ϊ"δ֪�豸����"���豸���Ͷ���
         */
        static DeviceType FromId(DeviceTypeId typeId)
        {
            auto it = GetList().find(typeId);

            if (it == GetList().end())
            {
                return { DeviceTypeId::UnkownDeviceType, _T("δ֪�豸����") };
            }
            else
            {
                return { it->first, it->second };
            }
        }

        /**
         * @brief ���豸�������Ƶõ��豸���Ͷ���
         * @param [in] str �豸��������
         * @return ���ض�Ӧ���豸���Ͷ������δ�ҵ��򷵻�����IDΪUnkownDeviceType������Ϊ"δ֪�豸����"���豸���Ͷ���
         */
        static DeviceType FromName(const tstring & str)
        {
            auto it = std::find_if(GetList().begin(), GetList().end(), 
                                   [&str](decltype(*GetList().begin()) & v) { return str == v.second; });
            
            if (it == GetList().end())
            {
                return { DeviceTypeId::UnkownDeviceType, _T("δ֪�豸����") };
            }
            else
            {
                return { it->first, it->second };
            }
        }

        /**
         * @brief ��ȡָ���豸����ID��Ӧ���豸����
         * @param [in] typeId �豸����ID
         * @return ���ض�Ӧ���豸�������ƣ����δ�ҵ��򷵻�"δ֪�豸����"
         */
        static tstring GetTypeName(DeviceTypeId typeId)
        {
            auto it = GetList().find(typeId);
            return it == GetList().end() ? _T("δ֪�豸����") : it->second;
        }

        /**
         * @brief ��ȡ������֪�豸����
         * @return �����豸����ӳ���
         */
        static const std::map<DeviceTypeId, const tchar *> & All()
        {
            return GetList();
        }

        /**
         * @brief �ж��Ƿ�����֪���豸����
         * @return ������򷵻�true�����򷵻�false
         */
        static bool IsKnownDeviceType(DeviceTypeId typeId)
        {
            return GetList().find(typeId) != GetList().end();
        }

        /**
         * @brief �ж��Ƿ�����֪���豸����
         * @return ������򷵻�true�����򷵻�false
         */
        static bool IsKnownDeviceType(const tstring & str)
        {
            return GetList().end() != std::find_if(GetList().begin(), GetList().end(), 
                                   [&str](decltype(*GetList().begin()) & v) { return str == v.second; });
        }

    private:
        DeviceTypeId    m_typeId = DeviceTypeId::UnkownDeviceType;  ///< ����ID
        const tchar *   m_name = _T("δ֪�豸����");                  ///< ��������

        /// ��ȡ�豸�����б�
        static std::map<DeviceTypeId, const tchar *> & GetList()
        {
            static std::map<DeviceTypeId, const tchar *> types = 
            {
                { DeviceTypeId::VideoFlameFireDetector,             _T("ͼ���ͻ������̽����") },
                { DeviceTypeId::VideoSFCompositeFireDetector,       _T("ͼ�����̻𸴺ϻ���̽����") },
                { DeviceTypeId::LinearBeamSmokeFireDetector,        _T("���͹������̻���̽����") },
                { DeviceTypeId::VisualLinearBeamSmokeFireDetector,  _T("���ӻ����͹������̻���̽����") },
                { DeviceTypeId::VideoFlameFireDetectorOD,           _T("������ͼ���ͻ������̽����") },
                { DeviceTypeId::VideoSFCompositeFireDetectorOD,     _T("������ͼ�����̻𸴺ϻ���̽����") },
                { DeviceTypeId::VideoFlameFireDetectorEx,           _T("������ͼ���ͻ������̽����") },
                { DeviceTypeId::VideoSFCompositeFireDetectorEx,     _T("������ͼ�����̻𸴺ϻ���̽����") },
                { DeviceTypeId::LinearBeamSmokeFireDetectorEx,      _T("���������͹������̻���̽����") },
                { DeviceTypeId::VisualLinearBeamSmokeFireDetectorEx, _T("�����Ϳ��ӻ����͹������̻���̽����") },
				{ DeviceTypeId::NetworkVideoRecorder,				_T("������Ƶ������") },
                { DeviceTypeId::HydrantDecoder,                     _T("�Զ������ڽ�����") },
                { DeviceTypeId::JettingHydrantDecoder,              _T("�������װ�ý�����") },
                { DeviceTypeId::JettingHydrantDomainCtrlBox,        _T("С�����������") },
				{ DeviceTypeId::GeneralModule1In1Out,				_T("�����뵥���ͨ��ģ��") },
				{ DeviceTypeId::GeneralModule2In2Out,				_T("˫����˫���ͨ��ģ��") },
				{ DeviceTypeId::GeneralModule3In2Out,				_T("3����˫���ͨ��ģ��") },
                { DeviceTypeId::VideoJettingHydrantDecoder,         _T("ͼ�����������װ�ý�����") },
                { DeviceTypeId::HydrantDecoder2,                    _T("�����Զ������ڽ�����") },
				{ DeviceTypeId::MULTI_DET,                          _T("�����̽����") },
				{ DeviceTypeId::MULTI_DET_WIRELESS,                 _T("���������̽����") },
				{ DeviceTypeId::GAS_DET,                            _T("����й©̽����") },     
				{ DeviceTypeId::HydrantDecoder2,                    _T("�Զ�������") },  //add by tq 20150508
				{ DeviceTypeId::NVR,                                _T("������Ƶ������") }
            };

            return types;
        }
    };

#pragma pack(push)
#pragma pack(1)

    /// ���кŶ���
    struct NCP_API SerialNumber
    {
        /// �豸����
        DeviceTypeId    type = DeviceTypeId::UnkownDeviceType;
        /// �������
        unsigned char   year = 0;
        /// �����·�
        unsigned char   month = 0;
        /// ��ˮ��
        unsigned short  id = 0;
        
        /// ����ȱʡ���캯��
        SerialNumber() = default;

        /**
         * @brief ���캯��
         * @param [in] _type  �豸����
         * @param [in] _year  �������
         * @param [in] _month �����·�
         * @param [in] _id    ��ˮ��
         */
        SerialNumber(DeviceTypeId _type, unsigned char _year, unsigned char _month, unsigned short _id) :
            type(_type),
            year(_year),
            month(_month),
            id(_id)
        {}

        /// ת�ַ���
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

        /// ���ַ����������
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

        /// С�ڱȽϲ�����
        bool operator < (const SerialNumber & sn) const
        {
            return memcmp(this, &sn, sizeof(sn)) < 0;
        }

        /// ���ڱȽϲ�����
        bool operator == (const SerialNumber & sn) const
        {
            return type == sn.type && year == sn.year && month == sn.month && id == sn.id;
        }

        /// �����ڱȽϲ�����
        bool operator != (const SerialNumber & sn) const
        {
            return !operator == (sn);
        }
    };

#pragma pack(pop)

    /// ���������Ƶ�Ԫ�����ֽڣ�
    enum class Method : unsigned char
    {
        Ctrl    = 1,    ///< ����
        Upload  = 2,    ///< �ϴ�
        Ack     = 3,    ///< ȷ��
        Read    = 4,    ///< ��ѯ
        Reply   = 5,    ///< Ӧ��
        Deny    = 6,    ///< ����
        Write   = 7     ///< ����
    };

    /// ��Դ��Ӧ�õ�Ԫ��Ϣ���ͱ�־��
    enum class Resource : unsigned char
    {
        SerialNumber            = 1,    ///< ���к�   rw
        Version                 = 2,    ///< �汾     r
        Status                  = 3,    ///< ״̬     r
        SystemTime              = 4,    ///< ϵͳʱ��  rw
        AnalogValue             = 5,    ///< ģ����ֵ  r
        Parameters              = 6,    ///< ����     rw
        AlarmPicture            = 7,    ///< ����ͼƬ  ru
        OperationRecord         = 8,    ///< ���м�¼  r
        AlgorithmList           = 9,    ///< �㷨�б�  rw
        AlgorithmParameters     = 10,   ///< �㷨����  rw
        Location                = 11,   ///< ����λ��  rw
        RecordStoreDevices      = 12,   ///< ��Ƶ¼��洢�豸 rw
        RecordList              = 13,   ///< ������ʩ¼���¼ r
        ChannelList             = 14,   ///< ������ʩ��Ƶͨ���б�
		TargetPicture			= 15,   ///< ��λͼƬ  ru
        //16~199

        Reset                   = 200,  ///< ��λ     c
        Reboot                  = 201,  ///< ����
        CtrlRelay               = 202,  ///< ���Ƽ̵���
        Disable                 = 203,  ///< ����/�������������ʩ
        SetDefaultParameters    = 204,  ///< �ָ�������ʩ��������
        CtrlHydrant             = 205,  ///< �����Զ����ٶ�λ�������װ�� 
        CtrlRecordStoreDevices  = 206,  ///< ������ʩ¼��洢�豸����
        CtrlRecordFile          = 207,  ///< ������ʩ¼���ļ�����
		CtrlRemote              = 208,  ///< �л��ֳ�Զ��״̬
        //209~255
    };

    /// ��ȡ����NCP�汾
    NCP_API const Version & GetVersion();
};

namespace std
{
    /**
     * @brief ��ȡ����Ŷ�Ӧ���ַ�������
     * @return ���������ַ�������
     */
    template <typename Ch = char>
    inline std::basic_string<Ch> to_string(Ncp::ErrorCode ec)
    {
        std::basic_ostringstream<Ch> oss;

        switch(ec)
        {
        case Ncp::ErrorCode::OK:
            oss << "����";
            break;
        case Ncp::ErrorCode::InvalidCRC:
            oss << "CRCУ��ʧ��";
            break;
        case Ncp::ErrorCode::OperateNotSupport:
            oss << "��֧�ֲ���";
            break;
        case Ncp::ErrorCode::ExecuteFailed:
            oss << "ִ��ʧ��";
            break;
        case Ncp::ErrorCode::ReceiverIsBusy:
            oss << "���շ�æ";
            break;
        case Ncp::ErrorCode::InvalidAppUnitLength:
            oss << "Ӧ�����ݵ�Ԫ���Ȳ���ȷ";
            break;
        case Ncp::ErrorCode::InvalidObjectCount:
            oss << "��Ϣ������Ŀ����ȷ";
            break;
        case Ncp::ErrorCode::InvalidDeviceType:
            oss << "�豸���Ͳ���ȷ";
            break;
        case Ncp::ErrorCode::PacketTimeout:
            oss << "���ݰ�����";
            break;
        case Ncp::ErrorCode::InvalidObjectData:
            oss << "��Ϣ����������Ч";
            break;
 
        case Ncp::ErrorCode::RequestNotFound:
            oss << "δ�ҵ���ӦID��������";
            break;
        case Ncp::ErrorCode::UnknownDeviceType:
            oss << "δ֪�豸����";
            break;
        case Ncp::ErrorCode::VersionNotSupport:
            oss << "Э��汾��֧��";
            break;
        case Ncp::ErrorCode::SerialNumberExists:
            oss << "���к��ظ�";
            break;
        case Ncp::ErrorCode::EncryptError:
            oss << "�����ֶδ���";
            break;
        case Ncp::ErrorCode::InvalidData:
            oss << "������Ч";
            break;
        case Ncp::ErrorCode::PacketNotComplete:
            oss << "���ݰ�������";
            break;
        case Ncp::ErrorCode::DeviceDisconnected:
            oss << "�豸�ѶϿ�";
            break;
        case Ncp::ErrorCode::RequestTimeout:
            oss << "����ʱ";
            break;
        case Ncp::ErrorCode::CreateDeviceFailed:
            oss << "�����豸ʧ��";
            break;
        default:
            oss << "δ֪����";
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