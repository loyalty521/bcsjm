////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file Server.cpp
/// @brief Ncp协议服务端类实现文件
///
/// 实现Ncp协议服务端类
///
/// @version 1.0
/// @author  李守红
/// @date    2013年12月1日
///
///
/// 修订说明：最初版本
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