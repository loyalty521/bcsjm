////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file DeviceImpl.h
/// @brief Ncp协议服务端设备实现类定义文件
///
/// 定义Ncp协议服务端设备实现类
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

#ifndef _NCP_SERVER_DEVICEIMPL_H
#define _NCP_SERVER_DEVICEIMPL_H

// 只能在Ncp所在项目中包含此文件（内部头文件）

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
        /// 设备实现类
        class DeviceImpl : public IoService::TcpSocket
        {
            friend class Device;
        public:
            /**
             * @brief 构造函数
             * @param [in] serverImpl         服务实现对象
             * @param [in] socket             套接字
             * @param [in] timeoutOnce        单次重发超时（毫秒）
             * @param [in] maxResendTimes     最大重发次数
             * @param [in] activeTimeoutFirst 初次激活超时时间（毫秒）
             * @param [in] activeTimeoutNext  后续激活超时时间（毫秒）
             * @param [in] maxActivateTimes   最大激活次数
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

            /// 虚析构
            virtual ~DeviceImpl() 
            {
                if (!m_deleting)    
                {
                    m_deleting = true;
                    Close();
                }            
            }

        public:
            /// 断开操作
            virtual void Close() override final;

            /// 判断是否已经连接（并注册成功）
            virtual bool IsConnected() const override final
            {
                std::lock_guard<mutex_type> lock(m_lock);
                return TcpSocket::IsConnected() && m_registed;
            }

        protected:
            /// 建立连接处理函数（不包含注册）
            virtual void OnConnected() override final;

            /// 连接断开处理函数
            virtual void OnDisconnected() override final;

            /// 注册超时处理函数
            void HandleRegisterTimeout();

            /**
             * @brief 添加数据
             * @param [in] ptr   数据指针
             * @param [in] bytes 数据长度
             */
            void PushData(const char * ptr, size_t bytes);

            /**
             * @brief 数据接收处理函数
             * @param [in] buffer 接收到的数据缓冲区
             * @return 返回下次将要读取的字节数（0表示缺省缓冲区大小）
             */
            size_t OnRead(std::shared_ptr<const std::vector<char>> buffer);

            /**
             * @brief 激活超时回调函数
             * @param [in] isFirst 是否是初次超时
             */
            void HandleActiveTimeout(bool isFirst);

        public:
            /**
             * @brief 取消某个请求
             */
            void CancelRequest(unsigned short id, bool callback = false);

            /**
             * @brief 发起一个异步请求
             * @param [in] pkt      请求数据包
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒数），如果为0，则使用缺省超时（单次重发超时*（最大重发次数+1））
             */
            bool SendRequest(std::shared_ptr<Packet> pkt, std::function<void(std::shared_ptr<Packet>)> callback, size_t timeout);

            /**
             * @brief 发起一个同步请求对象
             * @param [in] pkt      请求数据包
             * @param [in] timeout  超时时间（毫秒数），如果为0，则使用缺省超时（单次重发超时*（最大重发次数+1））
             */
            std::shared_ptr<Packet> SendRequest(std::shared_ptr<Packet> pkt, size_t timeout = 0u);

            /**
             * @brief 发送一个回应包
             * @param [in] pkt 回应包
             * @return 发送成功返回true，否则返回false
             */
            bool SendResponse(std::shared_ptr<Packet> pkt);
            
            /// 分发一次数据请求
            void DispatchRequestOnce();

            /// 已执行包超时处理函数
            void HandleExecutedTimeout(unsigned short pktId);

            /// 获取下一个可用的数据包流水号
            unsigned short GetNextId()
            {
                return ++m_pktId;
            }

        protected:
			/// 自动激活、超时延时调用
			IoService::InvokeState					m_timeoutState;

            /// 服务端实现对象
            ServerImpl &                            m_serverImpl;

            /// 具体设备指针
            Device *                                m_device = nullptr;

            /// 注册结构体
            RegisterPacket                          m_registerRecv;

            /// 当前正在接收包
            std::shared_ptr<Packet>                 m_inputPkt;

            /// 当前正在分发的请求包
            std::shared_ptr<Packet>                 m_dispatchingPkt;

            /// 单次重发超时（毫秒）
            size_t                                  m_timeoutOnce = 0;
            /// 最大重发次数
            size_t                                  m_maxResendTimes = 0;
            /// 初次激活超时（毫秒）
            size_t                                  m_activeTimeoutFirst = 0;
            /// 后续激活超时（毫秒）
            size_t                                  m_activeTimeoutNext = 0;
            /// 最大激活次数
            size_t                                  m_maxActivateTimes = 0;
            
            /// 是否已经注册
            bool                                    m_registed = false;

            // 激活包
            std::shared_ptr<Packet>                 m_pktActivate;
            
            /// 当前使用的请求包流水号
            std::atomic<unsigned short>             m_pktId = 0;

            /// 激活次数
            size_t                                  m_activateCount = 0;

            /// 接收到的请求包队列
            std::queue<std::weak_ptr<Packet>>                   m_inputs;
            /// ID索引表
            std::map<unsigned short, std::shared_ptr<Packet>>   m_inputsById;

            //已执行的接收到的请求包
            std::map<unsigned short, std::shared_ptr<Packet>>   m_inputsExecutedById;

            //发出去的请求包
            std::map<unsigned short, std::shared_ptr<IoService::Request<Packet>>>  m_requestsById;
        };
    };
};

#endif