////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file NcpImpl.hpp
/// @brief Ncpͨ�ù���ʵ���ļ�
///
/// ʵ��NcpЭ��ͨ�ù��ܲ���
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
#ifndef _NCP_NCPIMPL_HPP
#define _NCP_NCPIMPL_HPP

// ֻ����Ncp������Ŀ�а������ļ����ڲ�ͷ�ļ���

#include <numeric> //std::accumulate
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>


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

#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "IoService.h"
#include "IoPort.h"

namespace Ncp
{
#pragma pack(push)
#pragma pack(1)
    /// ע��ṹ��
    struct RegisterStruct
    {
        /// ��ʼ��־����������־��
        enum {MAGIC_HEAD = 0x4040, MAGIC_TAIL = 0x2323};

        unsigned short  magicHead = MAGIC_HEAD; ///< ��ʼ��־��
        SerialNumber    sn;                     ///< ���к�
        Version         ncpVersion;             ///< NCP�汾
        Version         softVersion;            ///< ����汾
        Version         platVersion;            ///< ƽ̨�汾
        unsigned char   encrypting[256];        ///< �����ֶ�
        unsigned short  magicTail = MAGIC_TAIL; ///< ������־��

        /// �ж�ע��ṹ���Ƿ���Ч�������ʹ�ã�
        ErrorCode Validate() const
        {
            // �ж���ʼ��־����������־��
            if (magicHead != MAGIC_HEAD || magicTail != MAGIC_TAIL)
            {
                return ErrorCode::InvalidData;
            }

            // �ж϶�Ӧ�豸�����Ƿ���֪
            if (!DeviceType::IsKnownDeviceType(sn.type))
            {
                return ErrorCode::UnknownDeviceType;
            }

            // �жϰ汾������
            if (!GetVersion().Compatible(ncpVersion))
            {
                return ErrorCode::VersionNotSupport;
            }

            return ErrorCode::OK;
        }

        /**
         * @brief �жϻ�Ӧע��ṹ���Ƿ���Ч���ͻ���ʹ�ã�
         * @param [in] rs �ͻ��˷��͵�ע�����ݰ��������ֶ��Ѽ��㣩
         */ 
        ErrorCode Validate(const RegisterStruct & rs) const
        {
            // У����ʼ��־���ͽ�����־��
            if (magicHead != MAGIC_HEAD || magicTail != MAGIC_TAIL)
            {
                return ErrorCode::InvalidData;
            }

            // У������ֶ�
            if (memcmp(rs.encrypting, encrypting, sizeof(encrypting)) != 0)
            {
                return ErrorCode::EncryptError;
            }

            return ErrorCode::OK;
        }

        /// ����
        void Encrypt()
        {
            unsigned char * p = reinterpret_cast<unsigned char *>(this);
            size_t i = 0;

            //����ע��������ֽں�m
            unsigned char m = std::accumulate(p, p + sizeof(RegisterStruct), 0);

            //�����ֶ���mȡ��򣬲���������
            for (auto & v : encrypting)
            {
                v ^= m; 
                v += (((5 + (i++)) & 0xf) << 4);    
            }        
        }
    };
#pragma pack(pop)

    /// ע���
    class RegisterPacket
    {
    public:
        /// ȱʡ���캯��
        RegisterPacket() = default;
        
        /// copy����
        RegisterPacket(const RegisterPacket & rhs) :
            m_buffer(new std::vector<char>(*rhs.m_buffer))
        {
        }

        /// move����
        RegisterPacket(RegisterPacket && rhs) :
            m_buffer(rhs.m_buffer)
        {
            rhs.m_buffer.reset(new std::vector<char>());    
        }

        /// copy��ֵ
        RegisterPacket & operator = (const RegisterPacket & rhs)
        {
            if (this != &rhs)
            {
                m_buffer.reset(new std::vector<char>(*rhs.m_buffer));
            }

            return *this;
        }

        /// move��ֵ
        RegisterPacket & operator = (RegisterPacket && rhs)
        {
            if (this != &rhs)
            {
                m_buffer = rhs.m_buffer;
                rhs.m_buffer.reset(new std::vector<char>());
            }

            return *this;
        }

        /// ��ȡע��ṹ��
        RegisterStruct & GetStruct()
        {
            return *reinterpret_cast<RegisterStruct *>(&m_buffer->at(0));
        }

        /// ��ȡ����ע��ṹ��
        const RegisterStruct & GetStruct() const
        {
            return *reinterpret_cast<const RegisterStruct *>(&m_buffer->at(0));
        }

        /// ��ȡ������
        std::shared_ptr<std::vector<char>> GetBuffer()
        {
            return m_buffer;
        }

        /// ��ȡ����������
        std::shared_ptr<const std::vector<char>> GetBuffer() const
        {
            return m_buffer;
        }

        /// �ж��Ƿ�����
        bool Completed() const
        {
            return m_buffer->size() == sizeof(RegisterStruct);
        }

        /// ��ջ�����
        void Clear()
        {
            m_buffer->clear();
        }

        /**
         * @brief ������ݣ��ݹ飩
         * @param [in]  data ����
         * @param [in]  len  ���ݳ���
         * @param [out] end  ʣ������ָ�루����Ϊnullptr��
         * @return ����������ErrorOK������������PacketNotComplete�����򷵻ذ�У�������
         */
        ErrorCode Append(const char * data, size_t len, const char ** end)
        {
            if (m_buffer->size() == sizeof(RegisterStruct))
            {
                *end = data;
                return ErrorCode::OK;
            }

            if (data == nullptr || len == 0)
            {
                *end = nullptr;
                return ErrorCode::PacketNotComplete;
            }

            if (m_buffer->empty()) // ��ע������
            {
                unsigned short magicHead = RegisterStruct::MAGIC_HEAD;
                const char * head = std::search(data, data + len, (char *)&magicHead, (char *)&magicHead + 2);

                if (head != data)
                {
                    printf("%s[%s:%d] invalid register data, %u bytes(or %u bytes if end by 0x23) ignored!\n", __FILE__, __func__, __LINE__, (unsigned)(head - data), (unsigned)(head - data - 1));
                }

                if (head != data + len) // �ҵ���ʼ��־��
                {
                    // �Թ���ʼ��־��֮ǰ������
                    len -= head - data;
                    data = head;

                    // ����ʼ��־����ʼ���Ƶ�ע��ṹ��
                    size_t szNeeded = len >= sizeof(RegisterStruct) ? sizeof(RegisterStruct) : len;

                    m_buffer->insert(m_buffer->end(), data, data + szNeeded);

                    // ����ʣ�໺��������
                    data += szNeeded;
                    len -= szNeeded;
                }
                else 
                {
                    if (data[len - 1] == (magicHead & 0xff)) //���һ���ַ��ǰ����ʼ��־��
                    {
                        m_buffer->push_back(RegisterStruct::MAGIC_HEAD & 0xff);
                    }

                    data += len;
                    len = 0;
                }
            }
            else if (m_buffer->size() == 1) //ֻ��1���ַ�
            {
                if (data[0] == (RegisterStruct::MAGIC_HEAD & 0xff))
                {
                    m_buffer->push_back(RegisterStruct::MAGIC_HEAD & 0xff);

                    ++data;
                    --len;
                }
                else
                {
                    printf("%s[%s:%d] head should be 0x23!\n", __FILE__, __func__, __LINE__);
                    m_buffer->clear();
                }
            }
            else if (m_buffer->size() < sizeof(RegisterStruct)) // ������
            {
                size_t szNeeded = len > sizeof(RegisterStruct) - m_buffer->size() ? sizeof(RegisterStruct) - m_buffer->size() : len;

                m_buffer->insert(m_buffer->end(), data, data + szNeeded);

                data += szNeeded;
                len -= szNeeded;
            }

            return Append(data, len, end);
        }

    private:
        /// ������
        std::shared_ptr<std::vector<char>> m_buffer { new std::vector<char>() };
    };

    /// Ncp���ݰ�����
    class Packet
    {
    public:
        /// ��ʼ��־����������־��
        enum { MAGIC_HEAD = 0x4040, MAGIC_TAIL = 0x2323 };

#pragma pack(push)
#pragma pack(1)
        /// ��ͷ�ṹ
        struct Head
        {
            unsigned short  magicHead = MAGIC_HEAD; ///< ��ʼ��־��
            unsigned short  id = 0;                 ///< ��ˮ��
            unsigned char   time[6];                ///< ʱ���ǩ
            unsigned int    sourceAddress = 0;      ///< Դ��ַ��������
            unsigned int    destAddress = 0;        ///< Ŀ���ַ��������
            unsigned int    appBytes = 0;           ///< Ӧ�����ݵ�Ԫ���ȣ�ע�⣺�˴�ָ������Ϣ�����ֽ�����
            Method          method;                 ///< ����
            Resource        resource;               ///< ��Դ
            DeviceTypeId    deviceType;             ///< �豸����
            unsigned char   objectCount = 0;        ///< ��Ϣ�������

            /// У���ͷ��Ч��
            ErrorCode Validate() const
            {
                // У����ʼ��־��
                if (magicHead != MAGIC_HEAD)
                {
                    return ErrorCode::InvalidData;
                }

                // Ӧ�����ݵ�Ԫ���ȳ�������Ϣ�������
                if ((appBytes == 0) != (objectCount == 0) || appBytes > 200 * 1024 * 1024)//Ӧ�����ݵ�Ԫ��������
                {
                    return ErrorCode::InvalidAppUnitLength;
                }

                // ÿ����Ϣ����ĳ��Ȳ��ܵ���1���ֽ�
                if (objectCount > appBytes)
                {
                    return ErrorCode::InvalidObjectCount;
                }

                return ErrorCode::OK;
            }
        };

        /// ��β�ṹ
        struct Tail
        {
            unsigned short  crc = 0;                ///< CRCУ��
            unsigned short  magicTail = MAGIC_TAIL; ///< ������־��

            /// ��βУ�飨������CRCУ�飩
            ErrorCode Validate() const 
            { 
                return magicTail == MAGIC_TAIL ? ErrorCode::OK : ErrorCode::InvalidData; 
            }
        };

#pragma pack(pop)
        /// У������Ncp���ݰ���Ч��
        ErrorCode Validate() const 
        { 
            // ��ͷָ��Ͱ�βָ��Ӧ�Ѿ���
            if (m_head == nullptr || m_tail == nullptr)
            {
                return ErrorCode::PacketNotComplete;
            }

            // У���ͷ
            ErrorCode err = m_head->Validate();

            if (err != ErrorCode::OK)
            {
                return err;
            }

            // У���β
            err = m_tail->Validate();

            if (err != ErrorCode::OK)
            {
                return err;
            }

            // CRCУ��
            if (m_head->method == Method::Ctrl && Crc() != m_tail->crc)
            {
                return ErrorCode::InvalidCRC;
            }

            return ErrorCode::OK;
        }

        /// ���캯��
        Packet() :
            m_buffer(new std::vector<char>())
        {
        }

        /// copy���캯��
        Packet(const Packet & pkt) :
            m_buffer(new std::vector<char>(*pkt.m_buffer))
        {
            AssignPointers();
        }

        /// move���캯��
        Packet(Packet && pkt) : //move
            m_head(pkt.m_head),
            m_tail(pkt.m_tail),
            m_objects(pkt.m_objects),
            m_buffer(pkt.m_buffer)
        {
            pkt.m_buffer.reset();
            pkt.m_head = nullptr;
            pkt.m_tail = nullptr;
            pkt.m_objects = nullptr;
        }

        /// copy��ֵ
        Packet & operator = (const Packet & pkt)
        {
            if (this != &pkt)
            {
                m_buffer.reset(new std::vector<char>(*pkt.m_buffer));
                AssignPointers();
            }

            return *this;
        }

        /// move��ֵ
        Packet & operator = (Packet && pkt) throw()
        {
            this->swap(pkt);
            return *this;
        }

        /// ���ݽ���
        void swap(Packet & pkt)
        {
            std::swap(m_buffer, pkt.m_buffer);
            std::swap(m_head, pkt.m_head);
            std::swap(m_tail, pkt.m_tail);
            std::swap(m_objects, pkt.m_objects);
        }

        /**
         * @brief ������ݣ��ݹ飩
         * @param [in]  data ����
         * @param [in]  len  ���ݳ���
         * @param [out] end  ʣ������ָ�루����Ϊnullptr��
         * @return ����������ErrorOK������������PacketNotComplete�����򷵻ذ�У�������
         */
        ErrorCode Append(const char * data, size_t len, const char ** end)
        {
            // ���Ncp���ݰ��Ƿ��Ѿ�����
            if (m_buffer->size() >= sizeof(Head)+sizeof(Tail))
            {
                const Head * head = reinterpret_cast<const Head *>(&m_buffer->at(0));
                // �����Ƿ�����
                if (m_buffer->size() == sizeof(Head) + sizeof(Tail) + head->appBytes)
                {
                    // ������
                    AssignPointers();

                    *end = data;
                    return Validate();
                }
            }

            // ������
            if (data == nullptr || len == 0)
            {
                *end = nullptr;
                return ErrorCode::PacketNotComplete;
            }

            // �����ݰ�
            if (m_buffer->empty())
            {
                // ������ʼ��־��
                unsigned short magicHead = MAGIC_HEAD;
                const char * head = std::search(data, data + len, (char *)&magicHead, (char *)&magicHead + 2);

                if (head == nullptr)
                {
                    // ���û���ҵ���ʼ��־�������ж����һ���ַ��Ƿ��ǰ����ʼ��־��������ǣ��򱣴�
                    if (data[len - 1] == (MAGIC_HEAD & 0xff))
                    {
                        m_buffer->push_back(MAGIC_HEAD & 0xff);
                    }

                    // ������Ч���ݣ�����
                    *end = nullptr;
                    return ErrorCode::PacketNotComplete;
                }
                else
                {
                    // ������һ����ͷ��С
                    size_t szNeeded = (size_t)(data + len - head) >= sizeof(Head) ? sizeof(Head) : (size_t)(data + len - head);

                    m_buffer->insert(m_buffer->end(), head, head + szNeeded);
                    return Append(head + szNeeded, data + len - head - szNeeded, end);
                }
            }
            // ֻ����˰����ʼ��־��
            else if (m_buffer->size() == 1)
            {
                if (data[0] == (MAGIC_HEAD & 0xff))
                {
                    size_t szNeeded = len >= sizeof(Head) - 1 ? sizeof(Head) - 1 : len;

                    m_buffer->insert(m_buffer->end(), data, data + szNeeded);
                    return Append(data + szNeeded, len - szNeeded, end);
                }
                else
                {
                    m_buffer->clear();
                    return Append(data, len, end);
                }
            }
            // ��ͷ������
            else if (m_buffer->size() < sizeof(Head))
            {
                size_t szNeeded = len >= sizeof(Head) - m_buffer->size() ? sizeof(Head) - m_buffer->size() : len;

                m_buffer->insert(m_buffer->end(), data, data + szNeeded);
                return Append(data + szNeeded, len - szNeeded, end);
            }
            // ��ͷ������
            else if (m_buffer->size() == sizeof(Head))
            {
                const Head * head = reinterpret_cast<const Head *>(&m_buffer->at(0));
                // У���ͷ
                ErrorCode ec = head->Validate();

                if (ec == ErrorCode::OK)
                {
                    m_buffer->reserve(sizeof(Head) + sizeof(Tail) + head->appBytes);
                    head = reinterpret_cast<const Head *>(&m_buffer->at(0));

                    size_t szNeeded = len >= head->appBytes + sizeof(Tail) ? head->appBytes + sizeof(Tail) : len;
                    m_buffer->insert(m_buffer->end(), data, data + szNeeded);
                    return Append(data + szNeeded, len - szNeeded, end);
                }
                else
                {
                    *end = data;
                    return ec;
                }
            }
            else
            {
                const Head * head = reinterpret_cast<const Head *>(&m_buffer->at(0));

                size_t szNeeded =
                    len >= head->appBytes + sizeof(Tail) + sizeof(Head) - m_buffer->size() ?
                    head->appBytes + sizeof(Tail) + sizeof(Head) - m_buffer->size() : len;

                m_buffer->insert(m_buffer->end(), data, data + szNeeded);
                return Append(data + szNeeded, len - szNeeded, end);
            }
        }

        /**
         * @brief ��ȡ��ͷָ��
         * @return ���ذ�ͷָ��
         */
        Head * GetHead() 
        { 
            return m_head; 
        }

        /**
         * @brief ��ȡ������ͷָ��
         * @return ���س�����ͷָ��
         */
        const Head * GetHead() const 
        { 
            return m_head; 
        };

        /**
         * @brief ��ȡ��βָ��
         * @return ���ذ�βָ��
         */
        Tail * GetTail() 
        { 
            return m_tail; 
        };

        /**
         * @brief ��ȡ������βָ��
         * @return ���س�����βָ��
         */
        const Tail * GetTail() const 
        { 
            return m_tail; 
        };

        /**
         * @brief ��ȡ������Ϣ��������ָ��
         * @param [out] sz ��Ϣ�������ݳ���
         * @return ���س�����Ϣ��������ָ��
         */
        const char * GetObjects(size_t & sz) const 
        { 
            sz = m_objects == nullptr ? 0u : m_buffer->size() - sizeof(Head) - sizeof(Tail);
            return m_objects;
        };

        /**
         * @brief ��ȡ��Ϣ��������ָ��
         * @param [out] sz ��Ϣ�������ݳ���
         * @return ������Ϣ��������ָ��
         */
        char * GetObjects(size_t & sz) 
        { 
            return const_cast<char *>(static_cast<const Packet * const>(this)->GetObjects(sz));
        };

        /**
         * @brief ��ȡ������Ϣ��������ָ��
         * @return ���س�����Ϣ��������ָ��
         */
        const char * GetObjects() const 
        { 
            return m_objects; 
        }

        /**
         * @brief ��ȡ��Ϣ��������ָ��
         * @return ������Ϣ��������ָ��
         */
        char * GetObjects() 
        { 
            return m_objects; 
        }

        /**
         * @brief �����Գ���β֮�������ֽڵ�CRC16ֵ��ȡ����
         * @return ����CRC16
         */
        unsigned short Crc() const
        {
            //CRC16��
            static unsigned short __ccitt_table[256] = {
                0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
                0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
                0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
                0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
                0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
                0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
                0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
                0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
                0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
                0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
                0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
                0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
                0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
                0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
                0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
                0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
                0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
                0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
                0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
                0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
                0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
                0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
                0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
                0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
                0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
                0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
                0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
                0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
                0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
                0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
                0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
                0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
            }; 

            unsigned short c = 0;

            std::for_each(m_buffer->begin(), m_buffer->begin() + sizeof(Head) + m_head->appBytes, 
                            [&c](char q){ c = __ccitt_table[(c >> 8 ^ q) & 0xff] ^ (c << 8); });

            return ~c;
        }

        /**
         * @brief ���ð�βCRC�ֶ�ֵΪ����CRC16ֵ������β֮�������ֶε�CRC16��
         */
        void SetCrc()
        {
            m_tail->crc = Crc();
        }

        /**
         * @brief �ж�һ�����ݰ��Ƿ��ǻ�Ӧ��
         * @return ����ǻ�Ӧ���򷵻�true�����򷵻�false
         */
        bool IsResponse() const
        {
            return m_head->method == Method::Ack || m_head->method == Method::Deny || m_head->method == Method::Reply;
        }

        /**
         * @brief ��ȡһ�����ϰ���Ӧ�Ĵ�����
         * @return 
         */
        ErrorCode GetErrorCode() const
        {
            if (m_head->method == Method::Deny)
            {
                ErrorCode ec;
                memcpy(&ec, m_objects, sizeof(ec)); 
                return ec;
                //return *(_UNALIGNED_<ErrorCode> *)m_objects;
            }
            else
            {
                return ErrorCode::OK;
            }
        }

        /**
         * @brief ����һ�����ݰ�
         * @param [in] id          ��ˮ��
         * @param [in] method      ����
         * @param [in] resource    ��Դ
         * @param [in] deviceType  �豸����
         * @param [in] appData     Ӧ�����ݵ�Ԫ
         * @param [in] appBytes    Ӧ�����ݵ�Ԫ�ֽ���
         * @param [in] objectCount ��Ϣ������Ŀ
         * @return ���ش�������Ŀ��ָ��
         */
        static std::shared_ptr<Packet> Create(unsigned short id, Method method, Resource resource, DeviceTypeId deviceType, const void * appData, size_t appBytes, size_t objectCount)
        {
            assert((objectCount == 0) == (appBytes == 0));

            if ((objectCount == 0) != (appBytes == 0))
            {
                return nullptr;
            }

            Head head;
            head.id = id;
            head.appBytes = appBytes;
            head.method = method;
            head.resource = resource;
            head.deviceType = deviceType;
            head.objectCount = objectCount;
            
            Tail tail;

            std::shared_ptr<Packet> pkt(new Packet());
			auto buffer = pkt->m_buffer;

            buffer->reserve(sizeof(Head) + sizeof(Tail) + appBytes);
            buffer->insert(buffer->end(), reinterpret_cast<char *>(&head), reinterpret_cast<char *>(&head) + sizeof(Head));
            
            if (appData != nullptr)
            {
                buffer->insert(buffer->end(), reinterpret_cast<const char *>(appData), reinterpret_cast<const char *>(appData) + appBytes);
            }
            else
            {
                buffer->insert(buffer->end(), appBytes, 0);
            }

            buffer->insert(buffer->end(), reinterpret_cast<char *>(&tail), reinterpret_cast<char *>(&tail) + sizeof(Tail));
            
            pkt->AssignPointers();

            // ��������Ӧ������CRCУ��ֵ
            if (method == Method::Ctrl)
            {
                pkt->SetCrc();
            }

            return pkt;
        }

        /**
         * @brief ����һ���������κ���Ϣ��������ݰ�
         * @param [in] id          ��ˮ��
         * @param [in] method      ����
         * @param [in] resource    ��Դ
         * @param [in] deviceType  �豸����
         * @return ���ش�������Ŀ��ָ��
         */
        static std::shared_ptr<Packet> Create(unsigned short id, Method method, Resource resource, DeviceTypeId deviceType)
        {
            return Create(id, method, resource, deviceType, nullptr, 0, 0);
        }

        /**
         * @brief ����һ������������Ϣ��������ݰ�
         * @param [in] id          ��ˮ��
         * @param [in] method      ����
         * @param [in] resource    ��Դ
         * @param [in] deviceType  �豸����
         * @param [in] data        ��Ϣ��������
         * @return ���ش�������Ŀ��ָ��
         */
        template <typename T>
        static std::shared_ptr<Packet> Create(unsigned short id, Method method, Resource resource, DeviceTypeId deviceType, const T & data)
        {
            return Create(id, method, resource, deviceType, &data, sizeof(T), 1);
        }

        /**
         * @brief ����һ��������vector�е���Ϣ��������ݰ�
         * @param [in] id          ��ˮ��
         * @param [in] method      ����
         * @param [in] resource    ��Դ
         * @param [in] deviceType  �豸����
         * @param [in] v           ��Ϣ��������
         * @param [in] objectCount ��Ϣ������Ŀ�����Ϊ0����Ϊv��Ԫ����
         * @return ���ش�������Ŀ��ָ��
         */
        template <typename T>
        static std::shared_ptr<Packet> Create(unsigned short id, Method method, Resource resource, DeviceTypeId deviceType, const std::vector<T> & v, size_t objectCount = 0)
        {
            return Create(id, method, resource, deviceType, &v.at(0), v.size() * sizeof(T), objectCount == 0 ? v.size() : objectCount);
        }

        /**
         * @brief ����һ�������������е���Ϣ��������ݰ�
         * @param [in] id          ��ˮ��
         * @param [in] method      ����
         * @param [in] resource    ��Դ
         * @param [in] deviceType  �豸����
         * @param [in] data        ��Ϣ��������
         * @param [in] objectCount ��Ϣ������Ŀ�����Ϊ0����Ϊdata��Ԫ����
         * @return ���ش�������Ŀ��ָ��
         */
        template <typename ArrayType, size_t ArraySize>
        static std::shared_ptr<Packet> Create(unsigned short id, Method method, Resource resource, DeviceTypeId deviceType, const ArrayType (&data)[ArraySize], size_t objectCount = 0)
        {
            return Create(id, method, resource, deviceType, &data, sizeof(data), objectCount == 0 ? ArraySize : objectCount);
        }

        /**
         * @brief ������Ӧ��
         * @param [in] pktReq ��Ӧ�������
         * @param [in] ec     ����ţ����ΪOK�򴴽�Ack�������򴴽�Deny��
         * @return ���ش����Ļ�Ӧ��
         */
        static std::shared_ptr<Packet> Create(std::shared_ptr<const Packet> pktReq, ErrorCode ec = ErrorCode::OK)
        {
			auto head = pktReq->m_head;

            if (ec == ErrorCode::OK)
            {
                return Create(head->id, Method::Ack, head->resource, head->deviceType);
            }
            else
            {
                return Create(head->id, Method::Deny, head->resource, head->deviceType, ec);
            }
        }

        /**
         * @brief ��������������Ϣ�����Reply��
         * @param [in] pktReq ��Ӧ�������
         * @param [in] data   ��������Ϣ��������
         * @return ���ش����Ļ�Ӧ��
         */
        template <typename T>
        static std::shared_ptr<Packet> Create(std::shared_ptr<const Packet> pktReq, const T & data)
        {
            return Create(pktReq, reinterpret_cast<const char *>(&data), sizeof(T), 1);
        }

        /**
         * @brief ������Ϣ��������������еĵ�Reply��
         * @param [in] pktReq      ��Ӧ�������
         * @param [in] data        ��Ϣ��������
         * @param [in] objectCount ��Ϣ������Ŀ�����Ϊ0������Ϣ������ĿΪ�����С
         * @return ���ش����Ļ�Ӧ��
         */
        template <typename ArrayType, size_t ArraySize>
        static std::shared_ptr<Packet> Create(std::shared_ptr<const Packet> pktReq, const ArrayType (&data)[ArraySize], size_t objectCount = 0)
        {
            return Create(pktReq, reinterpret_cast<const char *>(data), sizeof(data), objectCount == 0 ? ArraySize : objectCount);
        }

        /**
         * @brief ������Ϣ��������ڻ������еĵ�Reply��
         * @param [in] pktReq      ��Ӧ�������
         * @param [in] data        ��Ϣ��������
         * @param [in] appBytes    Ӧ�����ݵ�Ԫ���ݳ���
         * @param [in] objectCount ��Ϣ������Ŀ
         * @return ���ش����Ļ�Ӧ��
         */
        template <typename T>
        static std::shared_ptr<Packet> Create(std::shared_ptr<const Packet> pktReq, const T * data, size_t appBytes, size_t objectCount = 1)
        {
            return Create(pktReq, reinterpret_cast<const char *>(data), appBytes, objectCount);
        }

        /**
         * @brief ������Ӧ��
         * @param [in] pktReq      ��Ӧ�������
         * @param [in] data        ��Ϣ��������
         * @param [in] appBytes    Ӧ�����ݵ�Ԫ���ݳ���
         * @param [in] objectCount ��Ϣ������Ŀ
         * @return ���ش����Ļ�Ӧ��
         */
        static std::shared_ptr<Packet> Create(std::shared_ptr<const Packet> pktReq, const char * data, size_t appBytes, size_t objectCount)
        {
            assert((appBytes == 0) == (objectCount == 0));

            if ((appBytes == 0) != (objectCount == 0))
            {
                return nullptr;
            }

			auto head = pktReq->m_head;

            return Create(head->id, appBytes > 0 ? Method::Reply : Method::Ack, 
                          head->resource, head->deviceType, data, appBytes, objectCount);
        }

        /// ��ȡ���ݰ���С
        size_t GetSize() const 
        { 
            return m_buffer->size(); 
        }

        /// ��ȡ���ݰ�������
        std::shared_ptr<const std::vector<char>> GetBuffer() const 
        { 
            return m_buffer; 
        }

    private:
        /// ����ָ��ֵ
        void AssignPointers()
        {
            m_head = nullptr;
            m_tail = nullptr;
            m_objects = nullptr;

            if (m_buffer && !m_buffer->empty())
            {
                m_head = reinterpret_cast<Head *>(&m_buffer->at(0));

                if (m_buffer->size() >= sizeof(Head) + sizeof(Tail))
                {
                    m_tail = reinterpret_cast<Tail *>(&m_buffer->at(m_buffer->size() - sizeof(Tail)));

                    if (m_buffer->size() > sizeof(Head) + sizeof(Tail))
                    {
                        m_objects = &m_buffer->at(sizeof(Head));
                    }
                }
            }
        }

    private:
        /// ��ͷָ��
        Head *              m_head = nullptr;
        /// ��βָ��
        Tail *              m_tail = nullptr;
        /// ��Ϣ��������ָ��
        char *              m_objects = nullptr;
        /// ������ָ��
        std::shared_ptr<std::vector<char>>   m_buffer;

        /// ���ʹ���
        size_t              m_sendTimes = 0u;

    };

    /// ���DeviceImpl��Ա
    #define CHECK_IMPL() \
    if (m_impl == nullptr) \
    { \
        return ErrorCode::DeviceDisconnected; \
    } \
    std::shared_ptr<DeviceImpl> impl = m_impl->lock(); \
    if (!impl) \
    {\
        return ErrorCode::DeviceDisconnected; \
    }

    /// ��Request�Ļ�Ӧ���л�ȡ������
    static inline ErrorCode ReqError(std::shared_ptr<Packet> pkt)
    {
        return pkt ? pkt->GetErrorCode() : ErrorCode::RequestTimeout;
    }

    /// ����cb1ȷ����������
    template <typename T, typename U>
    static inline std::function<void(std::shared_ptr<Packet>)> ReqCallback(T cb1, U cb2)
    {
        if (cb1)
        {
            return cb2;
        }
        else
        {
            return nullptr;
        }
    }
};

#endif