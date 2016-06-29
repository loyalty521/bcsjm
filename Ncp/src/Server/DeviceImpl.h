////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file DeviceImpl.h
/// @brief NcpЭ�������豸ʵ���ඨ���ļ�
///
/// ����NcpЭ�������豸ʵ����
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

#ifndef _NCP_SERVER_DEVICEIMPL_H
#define _NCP_SERVER_DEVICEIMPL_H

// ֻ����Ncp������Ŀ�а������ļ����ڲ�ͷ�ļ���

#include <map>
#include <queue>
#include <memory>

#include <mutex>
#include <atomic>

#include "../Ncp.h"
#include "../NcpImpl.hpp"

#include "IoService.h"
#include "TcpSocket.h"
#include "Request.h"

class Device;
class Server;
class ServerImpl;

namespace Ncp
{
    namespace Server
    {
        /// �豸ʵ����
        class DeviceImpl : public IoService::TcpSocket
        {
            friend class Device;
        public:
            /**
             * @brief ���캯��
             * @param [in] serverImpl         ����ʵ�ֶ���
             * @param [in] socket             �׽���
             * @param [in] timeoutOnce        �����ط���ʱ�����룩
             * @param [in] maxResendTimes     ����ط�����
             * @param [in] activeTimeoutFirst ���μ��ʱʱ�䣨���룩
             * @param [in] activeTimeoutNext  �������ʱʱ�䣨���룩
             * @param [in] maxActivateTimes   ��󼤻����
             */
            DeviceImpl(ServerImpl & serverImpl, 
                   size_t timeoutOnce, 
                   size_t maxResendTimes, 
                   size_t activeTimeoutFirst, 
                   size_t activeTimeoutNext, 
                   size_t maxActivateTimes) :
                m_serverImpl(serverImpl),
                m_timeoutOnce(timeoutOnce),
                m_maxResendTimes(maxResendTimes),
                m_activeTimeoutFirst(activeTimeoutFirst),
                m_activeTimeoutNext(activeTimeoutNext),
                m_maxActivateTimes(maxActivateTimes)
            {}

            /// ������
            virtual ~DeviceImpl() 
            {
                if (!m_deleting)    
                {
                    m_deleting = true;
                    Close();
                }            
            }

        public:
            /// �Ͽ�����
            virtual void Close() override final;

            /// �ж��Ƿ��Ѿ����ӣ���ע��ɹ���
            virtual bool IsConnected() const override final
            {
                std::lock_guard<mutex_type> lock(m_lock);
                return TcpSocket::IsConnected() && m_registed;
            }

        protected:
            /// �������Ӵ�������������ע�ᣩ
            virtual void OnConnected() override final;

            /// ���ӶϿ�������
            virtual void OnDisconnected() override final;

            /// ע�ᳬʱ������
            void HandleRegisterTimeout();

            /**
             * @brief �������
             * @param [in] ptr   ����ָ��
             * @param [in] bytes ���ݳ���
             */
            void PushData(const char * ptr, size_t bytes);

            /**
             * @brief ���ݽ��մ�����
             * @param [in] buffer ���յ������ݻ�����
             * @return �����´ν�Ҫ��ȡ���ֽ�����0��ʾȱʡ��������С��
             */
            size_t OnRead(std::shared_ptr<const std::vector<char>> buffer);

            /**
             * @brief ���ʱ�ص�����
             * @param [in] isFirst �Ƿ��ǳ��γ�ʱ
             */
            void HandleActiveTimeout(bool isFirst);

        public:
            /**
             * @brief ȡ��ĳ������
             */
            void CancelRequest(unsigned short id, bool callback = false);

            /**
             * @brief ����һ���첽����
             * @param [in] pkt      �������ݰ�
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨�������������Ϊ0����ʹ��ȱʡ��ʱ�������ط���ʱ*������ط�����+1����
             */
            bool SendRequest(std::shared_ptr<Packet> pkt, std::function<void(std::shared_ptr<Packet>)> callback, size_t timeout);

            /**
             * @brief ����һ��ͬ���������
             * @param [in] pkt      �������ݰ�
             * @param [in] timeout  ��ʱʱ�䣨�������������Ϊ0����ʹ��ȱʡ��ʱ�������ط���ʱ*������ط�����+1����
             */
            std::shared_ptr<Packet> SendRequest(std::shared_ptr<Packet> pkt, size_t timeout = 0u);

            /**
             * @brief ����һ����Ӧ��
             * @param [in] pkt ��Ӧ��
             * @return ���ͳɹ�����true�����򷵻�false
             */
            bool SendResponse(std::shared_ptr<Packet> pkt);
            
            /// �ַ�һ����������
            void DispatchRequestOnce();

            /// ��ִ�а���ʱ������
            void HandleExecutedTimeout(unsigned short pktId);

            /// ��ȡ��һ�����õ����ݰ���ˮ��
            unsigned short GetNextId()
            {
                return ++m_pktId;
            }

        protected:
			/// �Զ������ʱ��ʱ����
			IoService::InvokeState					m_timeoutState;

            /// �����ʵ�ֶ���
            ServerImpl &                            m_serverImpl;

            /// �����豸ָ��
            Device *                                m_device = nullptr;

            /// ע��ṹ��
            RegisterPacket                          m_registerRecv;

            /// ��ǰ���ڽ��հ�
            std::shared_ptr<Packet>                 m_inputPkt;

            /// ��ǰ���ڷַ��������
            std::shared_ptr<Packet>                 m_dispatchingPkt;

            /// �����ط���ʱ�����룩
            size_t                                  m_timeoutOnce = 0;
            /// ����ط�����
            size_t                                  m_maxResendTimes = 0;
            /// ���μ��ʱ�����룩
            size_t                                  m_activeTimeoutFirst = 0;
            /// �������ʱ�����룩
            size_t                                  m_activeTimeoutNext = 0;
            /// ��󼤻����
            size_t                                  m_maxActivateTimes = 0;
            
            /// �Ƿ��Ѿ�ע��
            bool                                    m_registed = false;

            // �����
            std::shared_ptr<Packet>                 m_pktActivate;
            
            /// ��ǰʹ�õ��������ˮ��
            std::atomic<unsigned short>             m_pktId = 0;

            /// �������
            size_t                                  m_activateCount = 0;

            /// ���յ������������
            std::queue<std::weak_ptr<Packet>>                   m_inputs;
            /// ID������
            std::map<unsigned short, std::shared_ptr<Packet>>   m_inputsById;

            //��ִ�еĽ��յ��������
            std::map<unsigned short, std::shared_ptr<Packet>>   m_inputsExecutedById;

            //����ȥ�������
            std::map<unsigned short, std::shared_ptr<IoService::Request<Packet>>>  m_requestsById;
        };
    };
};

#endif