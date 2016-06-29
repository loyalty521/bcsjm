////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file ServerImpl.cpp
/// @brief Ncp协议服务端实现类实现文件
///
/// 实现Ncp协议服务端实现类
///
/// @version 1.0
/// @author  李守红
/// @date    2013年12月1日
///
///
/// 修订说明：最初版本
///
////////////////////////////////////////////////////////////////////////

#include <set>
#include <cassert>

#include "../Ncp.h"
#include "../NcpImpl.hpp"

#include "IoService.h"
#include "TcpServer.h"

#include "Device.h"
#include "Server.h"

#include "ServerImpl.h"

namespace Ncp
{
    namespace Server
    {  
        void ServerImpl::Stop()
        { 
            Close();
            IoService::Stop();
        }

        // 关闭服务端
        void ServerImpl::Close()
        {
            std::lock_guard<mutex_type> lock(m_lock);

            // 释放所有的deviceImpl
            for (auto & v : m_devices)
            {
                IoService::Invoke(std::bind(&DeviceImpl::Close, v));        
            }

            m_devices.clear();

            TcpServer::Close();
        }

        bool ServerImpl::OnAcceptReady(std::shared_ptr<TcpSocket> client)
        {
            std::lock_guard<mutex_type> lock(m_lock);

            if (client)
            {
                m_devices.insert(std::dynamic_pointer_cast<DeviceImpl>(client));
				return true;
            }
			else
			{
				return false;
			}
        }

        std::shared_ptr<IoService::TcpSocket> ServerImpl::OnAccept(const Address & addrRemote) 
        { 
            std::lock_guard<mutex_type> lock(m_lock);

            std::shared_ptr<DeviceImpl> deviceImpl(new DeviceImpl(*this, 
                    m_timeoutOnce, m_maxResendTimes, m_activeTimeoutFirst, m_activeTimeoutNext, m_maxActivateTimes));

            //printf("%s[%s:%d] deviceImpl:%p-%p\n", __FILE__, __func__, __LINE__, deviceImpl.get(), (unsigned char *)deviceImpl.get() + sizeof(DeviceImpl));
            return deviceImpl;
        }

        void ServerImpl::OnDeviceDisconnected(std::shared_ptr<DeviceImpl> deviceImpl)
        {
            std::lock_guard<mutex_type> lock(m_lock);
            m_devices.erase(deviceImpl);
        }
    };
};