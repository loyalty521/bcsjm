////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file Server.h
/// @brief NcpЭ�������ඨ���ļ�
///
/// ����NcpЭ��������
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

#ifndef _NCP_SERVER_SERVER_H
#define _NCP_SERVER_SERVER_H

#include "../Ncp.h"
#include "Device.h"

namespace Ncp
{
    class Packet;

    namespace Server
    {
        /// NCP����������
        class NCP_API Server
        {
            friend class ServerImpl;

        public:
            /**
             * @brief ���캯��
             * @param [in] addr               ���ؼ�����ַ
             * @param [in] timeoutOnce        �����ط���ʱ�����룩
             * @param [in] maxResendTimes     ����ط�����
             * @param [in] activeTimeoutFirst ���μ��ʱʱ�䣨���룩
             * @param [in] activeTimeoutNext  �������ʱʱ�䣨���룩
             * @param [in] maxActivateTimes   ��󼤻����
             */
            Server(const Address & addr = { 0, 1111 }, 
                   size_t timeoutOnce = 10000, 
                   size_t maxResendTimes = 3, 
                   size_t activeTimeoutFirst = 30000, 
                   size_t activeTimeoutNext = 10000, 
                   size_t maxActivateTimes = 3);

            /// ��������
            virtual ~Server();

            /**
             * @brief ��������
             * @return �����ɹ�����true�����򷵻�false
             */
            bool Start();

            /**
             * @brief ֹͣ����
             */
            void Stop();

            /**
             * @brief �豸ע��ɹ�������
             * @param [in] sn   �豸���к�
             * @param [in] addr �豸�����ַ
             * @return �����豸ָ��
             * @note ����Ӧʵ�ִ˽ӿڣ��������кź������ַ�����豸����
             */
            virtual Device * OnAccept(const SerialNumber & sn, const Address & addr) { return nullptr; };

            /**
             * @brief �豸ע��ʧ�ܴ�����
             * @param [in] sn         �豸���к�
             * @param [in] ncpVersion �Է�NCP�汾
             * @param [in] addr       �Է������ַ
             * @param [in] err        ������
             * @note �����ʵ�ִ˽ӿ��Ա�չʾ������Ϣ
             */
            virtual void OnAcceptBadRegister(const SerialNumber & sn, const Version & ncpVersion, const Ncp::Address & addr, ErrorCode err) {};

        private:
            /// ��ֹcopy����
            Server(const Server&) = delete;
            /// ��ֹcopy��ֵ
            Server & operator = (const Server&) = delete;

        private:
            /// �ڲ�ʵ��ָ��
            std::shared_ptr<ServerImpl> *    m_impl = nullptr;
        };
    };
};

#include "Devices/HydrantDecoder.h"
#include "Devices/JettingHydrantDecoder.h"
#include "Devices/JettingHydrantDomainCtrlBox.h"
#include "Devices/VideoFlameFireDetector.h"
#include "Devices/VideoFlameFireDetectorOD.h"
#include "Devices/VideoFlameFireDetectorEx.h"
#include "Devices/VideoSFCompositeFireDetector.h"
#include "Devices/VideoSFCompositeFireDetectorOD.h"
#include "Devices/VideoSFCompositeFireDetectorEx.h"
#include "Devices/LinearBeamSmokeFireDetector.h"
#include "Devices/LinearBeamSmokeFireDetectorEx.h"
#include "Devices/VisualLinearBeamSmokeFireDetector.h"
#include "Devices/VisualLinearBeamSmokeFireDetectorEx.h"

#endif