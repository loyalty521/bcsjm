////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file ServerImpl.h
/// @brief Ncp协议服务端实现类定义文件
///
/// 定义Ncp协议服务端实现类
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

#ifndef _NCP_SERVER_SERVERIMPL_H
#define _NCP_SERVER_SERVERIMPL_H

// 只能在Ncp所在项目中包含此文件（内部头文件）

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
        /// 服务器实现类
        class ServerImpl : public IoService::TcpServer
        {
        public:
            /**
             * @brief 构造函数
             * @param [in] server 服务器对象
             * @param [in] addr               本地监听地址
             * @param [in] timeoutOnce        单次重发超时（毫秒）
             * @param [in] maxResendTimes     最大重发次数
             * @param [in] activeTimeoutFirst 初次激活超时时间（毫秒）
             * @param [in] activeTimeoutNext  后续激活超时时间（毫秒）
             * @param [in] maxActivateTimes   最大激活次数
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

            /// 析构函数
            ~ServerImpl()
            {
                if (!m_deleting)    
                {
                    m_deleting = true;
                    Close();
                }
            }

            /**
             * @brief 启动服务
			 * @param [in] maxConcurrentAccepts 最大并发接收个数
             * @return 启动成功返回ture，否则返回false
             */
			virtual bool Start(size_t maxConcurrentAccepts = 500u) override final
            {
                std::lock_guard<mutex_type> lock(m_lock);
                return IoService::Start() && TcpServer::Start(maxConcurrentAccepts);
            }

            /// 停止服务
            void Stop();

            /**
             * @brief 设备注册处理函数
             * @param [in] rs   注册结构体
             * @param [in] addr 设备网络地址
             * @return 返回设备指针
             */
            Device * OnDeviceRegisted(const RegisterStruct & rs, const Ncp::Address & addr)
            {
                return m_server.OnAccept(rs.sn, addr);
            }

            /**
             * @brief 设备注册失败处理函数
             * @param [in] rs   注册结构体
             * @param [in] addr 设备网络地址
             * @param [in] ec   错误码
             * @note 可能的错误码：UnknownDeviceType(未知设备类型)，VersionNotSupport（协议版本不支持），SerialNumberExists（序列号已存在）
             */
            void OnDeviceRegistFail(const RegisterStruct & rs, const Ncp::Address & addr, ErrorCode ec)
            {
                m_server.OnAcceptBadRegister(rs.sn, rs.ncpVersion, addr, ec);
            }

            /**
             * @brief 接受到连接处理函数（未注册）
             * @param [in] client 接受到的套接字
             */
            virtual bool OnAcceptReady(std::shared_ptr<TcpSocket> client) override final;

            /**
             * @brief 接受到连接处理函数（未注册）
             * @param [in] addrRemote 远程地址
             * @return 返回一个TcpSocket对象
             */
            virtual std::shared_ptr<TcpSocket> OnAccept(const Address & addrRemote) override final;

            /**
             * @brief 设备断开处理函数
             * @param [in] deviceImpl 设备实现
             */
            void OnDeviceDisconnected(std::shared_ptr<DeviceImpl> deviceImpl);

		private:
			/// 关闭服务端
			virtual void Close() override final;

        private:
            /// 服务对象
            Server &    m_server;
            /// 单次重发超时
            size_t      m_timeoutOnce = 0;
            /// 最大重发次数
            size_t      m_maxResendTimes = 0;
            /// 初次激活超时
            size_t      m_activeTimeoutFirst = 0;
            /// 后续激活超时
            size_t      m_activeTimeoutNext = 0;
            /// 最大激活次数
            size_t      m_maxActivateTimes = 0;
            /// 正在停止标记
            bool        m_stopping = false;

            /// 设备列表
            std::set<std::shared_ptr<DeviceImpl>> m_devices;
        };
    };
};

#endif