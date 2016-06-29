////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file Server.cpp
/// @brief NcpЭ��������ʵ���ļ�
///
/// ʵ��NcpЭ��������
///
/// @version 1.0
/// @author  ���غ�
/// @date    2013��12��1��
///
///
/// �޶�˵��������汾
///
////////////////////////////////////////////////////////////////////////

#include "../Ncp.h"

#include "Server.h"
#include "ServerImpl.h"

namespace Ncp
{
    namespace Server
    {
        Server::Server(const Address & addr, 
                size_t timeoutOnce, 
                size_t maxResendTimes, 
                size_t activeTimeoutFirst, 
                size_t activeTimeoutNext, 
                size_t maxActivateTimes) :
            m_impl(new std::shared_ptr<ServerImpl>(new ServerImpl(*this, addr, timeoutOnce, maxResendTimes, activeTimeoutFirst, activeTimeoutNext, maxActivateTimes)))
        {
        }

        Server::~Server()
        {
            m_impl->get()->Stop();
            delete m_impl;
        }

        bool Server::Start()
        {
            return m_impl->get()->Start();
        }

        void Server::Stop()
        {
            m_impl->get()->Stop();
        }
    };
};