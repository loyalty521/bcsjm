////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file Client.h
/// @brief Ncp�ͻ����ඨ���ļ�
///
/// ����Ncp�ͻ�����
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

#ifndef _NCP_CLIENT_CLIENT_H
#define _NCP_CLIENT_CLIENT_H

#include "../Ncp.h"
#include "../NcpImpl.hpp"

#include "TcpClient.h"
#include "Request.h"

namespace Ncp
{
    namespace Client
    {
        /// Ncp�ͻ���
        class NCP_API Client : public IoService::TcpClient
        {
        public:
            /**
             * @brief ���캯��
             * @param [in] addrRemote    ��������ַ
             * @param [in] addrLocal     ���ص�ַ��������Ϊ{0, 0}
             * @param [in] softVersion   ����汾
             * @param [in] platVersion   ƽ̨�汾
             * @param [in] sn            ���к�
             * @param [in] activeTimeout ���ʱ
             */
            Client(const Address & addrRemote, const Address & addrLocal, 
                Version softVersion, Version platVersion, const SerialNumber & sn, size_t activeTimeout = 60000u, 
                size_t timeoutOnce = 10000u, size_t maxResendTimes = 3);

            virtual ~Client()
            {
                if (!m_deleting)    
                {
                    m_deleting = true;
                    Close();
                }
            }

            /// �رտͻ���
            virtual void Close() override;

            /**
             * @brief �������к�
             * @param [in] sn �µ����к�
             * @note ��������к�������кŲ�ͬ���Ҵ�ʱ����������������ӣ���Ͽ�����
             */
            void SetSerialNumber(const SerialNumber & sn);

            /**
             * @brief ���÷�������ַ
             * @param [in] addrRemote �µķ�������ַ
             * @note ����µķ�������ַ��ɷ�������ַ��ͬ���Ҵ�ʱ����������������ӣ���Ͽ�����
             */
            void SetServerAddress(const Address & addrRemote);
            
            /**
             * @brief ִ��һ���ϴ�
             * @param [in] resource ��Դ
             * @param [in] data     ����
             * @return ִ�гɹ�����true�����򷵻�false
             */
            template <typename T>
            bool Upload(Ncp::Resource resource, T && data)
            {
                std::lock_guard<mutex_type> lock(m_lock);
                return Upload(Packet::Create(++m_pktId, Ncp::Method::Upload, resource, m_registerStruct.sn.type, std::forward<T>(data)));
            }

            /**
             * @brief ִ��һ���ϴ�
             * @param [in] resource    ��Դ
             * @param [in] data        ����
             * @param [in] objectCount ������������Ϊ0����ΪArrSz��
             * @return ִ�гɹ�����true�����򷵻�false
             */
            template <typename T, size_t ArrSz>
            bool Upload(Ncp::Resource resource, T (&data)[ArrSz], size_t objectCount = 0)
            {
                std::lock_guard<mutex_type> lock(m_lock);
                return Upload(Packet::Create(++m_pktId, Ncp::Method::Upload, resource, m_registerStruct.sn.type, data, objectCount));                
            }

            /**
             * @brief ִ��һ���ϴ�
             * @param [in] resource    ��Դ
             * @param [in] data        ����
             * @param [in] objectCount ������������Ϊ0����Ϊdata����
             * @return ִ�гɹ�����true�����򷵻�false
             */
            template <typename T>
            bool Upload(Ncp::Resource resource, std::vector<T> data, size_t objectCount = 0)
            {
               std::lock_guard<mutex_type> lock(m_lock);
                return Upload(Packet::Create(++m_pktId, Ncp::Method::Upload, resource, m_registerStruct.sn.type, data, objectCount));                 
            }

        protected:
            /// ���Ӵ�����
            virtual void OnConnected() override;

            /// �Ͽ�������
			virtual void OnDisconnected() override;

            /// ע����ɴ�����
            virtual void OnRegisted();

            /// ����ʧ�ܴ�����
			virtual void OnConnectFailed() override;

            /**
             * @brief ���ݵ��ﴦ����
             * @param [in] buffer ����������
             * @return �����´ν�Ҫ��ȡ���ֽ�����0��ʾȱʡ��������С��
             */
            size_t OnRead(std::shared_ptr<const std::vector<char>> buffer) override;

            /**
             * @brief �������
             * @param [in] ptr   ����ָ��
             * @param [in] bytes ���ݳ���
             */
            void PushData(const char * ptr, size_t bytes);         

            /// �ַ���������
            void DispatchRequest();

            /// ��ִ�а���ʱ������
            void HandleExecutedTimeout(unsigned short pktId)
            {
                std::lock_guard<mutex_type> lock(m_lock);
                m_inputsExecutedById.erase(pktId);
            }

            /**
             * @brief ���ó�ʱʱ��
             * @param [in] timeout ��ʱʱ��
             */
            void SetTimeout(size_t timeout);

            /// Ncp��������
            virtual void OnRequest(std::shared_ptr<const Packet> pkt);

            /**
             * @brief ����һ���ϴ����ͻ���ȫ�������첽�ϴ���
             * @param [in] pkt �ϴ����ݰ�
             * @return ִ�гɹ�����true�����򷵻�false
             */
            bool Upload(std::shared_ptr<Packet> pkt)
            {
                std::lock_guard<mutex_type> lock(m_lock);
				std::shared_ptr<IoService::Request<Packet>> request(new IoService::Request<Packet>(shared_from_this(), pkt));

                if (!request->Send(m_timeoutOnce, m_maxResendTimes))
                {
                    return false;
                }

                m_requestsById.insert({ pkt->GetHead()->id, request });
                return true;
            }
        protected:
            /// ��ʱ��ʱ�������ӷ�������ʱ�����ӻ��ʱ��
            IoService::InvokeState    m_timerTimeout;
            
            /// ���ͳ�ȥ��ע��ṹ��
            RegisterStruct      m_registerStruct;

            /// ���յ���ע���Ӧ��
            RegisterPacket      m_registerRecv;

            /// �Ƿ���ע��
            bool                m_registed = false;

            /// ��ǰ�����
            std::shared_ptr<Packet> m_inputPkt;

            /// ���������ϴ���ӳ���
            std::map<unsigned short, std::shared_ptr<IoService::Request<Packet>>>  m_requestsById;

            /// �����ӳ��� 
            std::map<unsigned short, std::shared_ptr<Packet>>   m_inputsById;

            /// ���������
            std::queue<std::weak_ptr<Packet>>                   m_inputs;

            /// ��ִ�а�ӳ���
            std::map<unsigned short, std::shared_ptr<Packet>>   m_inputsExecutedById;

            /// ���ʱʱ�䣨���룩
            size_t              m_activeTimeout = 60000u;

            /// ��ǰʹ�õ��������ˮ��
            std::atomic<unsigned short> m_pktId = { 0 };

            /// �����ط���ʱ
            size_t              m_timeoutOnce = 10000u;
            /// ����ط�����
            size_t              m_maxResendTimes = 3;
        };
    };
};

#endif