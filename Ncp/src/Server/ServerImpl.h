////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file ServerImpl.h
/// @brief NcpЭ������ʵ���ඨ���ļ�
///
/// ����NcpЭ������ʵ����
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

#ifndef _NCP_SERVER_SERVERIMPL_H
#define _NCP_SERVER_SERVERIMPL_H

// ֻ����Ncp������Ŀ�а������ļ����ڲ�ͷ�ļ���

#include <set>

#include "IoService.h"
#include "TcpServer.h"

#include "DeviceImpl.h"
#include "Device.h"
#include "Server.h"

namespace Ncp
{
    namespace Server
    {
        /// ������ʵ����
        class ServerImpl : public IoService::TcpServer
        {
        public:
            /**
             * @brief ���캯��
             * @param [in] server ����������
             * @param [in] addr               ���ؼ�����ַ
             * @param [in] timeoutOnce        �����ط���ʱ�����룩
             * @param [in] maxResendTimes     ����ط�����
             * @param [in] activeTimeoutFirst ���μ��ʱʱ�䣨���룩
             * @param [in] activeTimeoutNext  �������ʱʱ�䣨���룩
             * @param [in] maxActivateTimes   ��󼤻����
             */
            ServerImpl(Server & server, const Ncp::Address & addr, 
                size_t timeoutOnce, 
                size_t maxResendTimes, 
                size_t activeTimeoutFirst, 
                size_t activeTimeoutNext, 
                size_t maxActivateTimes) :
                IoService::TcpServer({addr.IP, addr.Port}),
                m_server(server),
                m_timeoutOnce(timeoutOnce),
                m_maxResendTimes(maxResendTimes),
                m_activeTimeoutFirst(activeTimeoutFirst),
                m_activeTimeoutNext(activeTimeoutNext),
                m_maxActivateTimes(maxActivateTimes)
            {
            }

            /// ��������
            ~ServerImpl()
            {
                if (!m_deleting)    
                {
                    m_deleting = true;
                    Close();
                }
            }

            /**
             * @brief ��������
			 * @param [in] maxConcurrentAccepts ��󲢷����ո���
             * @return �����ɹ�����ture�����򷵻�false
             */
			virtual bool Start(size_t maxConcurrentAccepts = 500u) override final
            {
                std::lock_guard<mutex_type> lock(m_lock);
                return IoService::Start() && TcpServer::Start(maxConcurrentAccepts);
            }

            /// ֹͣ����
            void Stop();

            /**
             * @brief �豸ע�ᴦ����
             * @param [in] rs   ע��ṹ��
             * @param [in] addr �豸�����ַ
             * @return �����豸ָ��
             */
            Device * OnDeviceRegisted(const RegisterStruct & rs, const Ncp::Address & addr)
            {
                return m_server.OnAccept(rs.sn, addr);
            }

            /**
             * @brief �豸ע��ʧ�ܴ�����
             * @param [in] rs   ע��ṹ��
             * @param [in] addr �豸�����ַ
             * @param [in] ec   ������
             * @note ���ܵĴ����룺UnknownDeviceType(δ֪�豸����)��VersionNotSupport��Э��汾��֧�֣���SerialNumberExists�����к��Ѵ��ڣ�
             */
            void OnDeviceRegistFail(const RegisterStruct & rs, const Ncp::Address & addr, ErrorCode ec)
            {
                m_server.OnAcceptBadRegister(rs.sn, rs.ncpVersion, addr, ec);
            }

            /**
             * @brief ���ܵ����Ӵ�������δע�ᣩ
             * @param [in] client ���ܵ����׽���
             */
            virtual bool OnAcceptReady(std::shared_ptr<TcpSocket> client) override final;

            /**
             * @brief ���ܵ����Ӵ�������δע�ᣩ
             * @param [in] addrRemote Զ�̵�ַ
             * @return ����һ��TcpSocket����
             */
            virtual std::shared_ptr<TcpSocket> OnAccept(const Address & addrRemote) override final;

            /**
             * @brief �豸�Ͽ�������
             * @param [in] deviceImpl �豸ʵ��
             */
            void OnDeviceDisconnected(std::shared_ptr<DeviceImpl> deviceImpl);

		private:
			/// �رշ����
			virtual void Close() override final;

        private:
            /// �������
            Server &    m_server;
            /// �����ط���ʱ
            size_t      m_timeoutOnce = 0;
            /// ����ط�����
            size_t      m_maxResendTimes = 0;
            /// ���μ��ʱ
            size_t      m_activeTimeoutFirst = 0;
            /// �������ʱ
            size_t      m_activeTimeoutNext = 0;
            /// ��󼤻����
            size_t      m_maxActivateTimes = 0;
            /// ����ֹͣ���
            bool        m_stopping = false;

            /// �豸�б�
            std::set<std::shared_ptr<DeviceImpl>> m_devices;
        };
    };
};

#endif