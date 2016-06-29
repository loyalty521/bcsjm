////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file Client.h
/// @brief Ncp客户端类定义文件
///
/// 定义Ncp客户端类
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
        /// Ncp客户端
        class NCP_API Client : public IoService::TcpClient
        {
        public:
            /**
             * @brief 构造函数
             * @param [in] addrRemote    服务器地址
             * @param [in] addrLocal     本地地址，建议设为{0, 0}
             * @param [in] softVersion   软件版本
             * @param [in] platVersion   平台版本
             * @param [in] sn            序列号
             * @param [in] activeTimeout 活动超时
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

            /// 关闭客户端
            virtual void Close() override;

            /**
             * @brief 设置序列号
             * @param [in] sn 新的序列号
             * @note 如果新序列号与旧序列号不同，且此时已与服务器建立连接，则断开重连
             */
            void SetSerialNumber(const SerialNumber & sn);

            /**
             * @brief 设置服务器地址
             * @param [in] addrRemote 新的服务器地址
             * @note 如果新的服务器地址与旧服务器地址不同，且此时已与服务器建立连接，则断开重连
             */
            void SetServerAddress(const Address & addrRemote);
            
            /**
             * @brief 执行一次上传
             * @param [in] resource 资源
             * @param [in] data     数据
             * @return 执行成功返回true，否则返回false
             */
            template <typename T>
            bool Upload(Ncp::Resource resource, T && data)
            {
                std::lock_guard<mutex_type> lock(m_lock);
                return Upload(Packet::Create(++m_pktId, Ncp::Method::Upload, resource, m_registerStruct.sn.type, std::forward<T>(data)));
            }

            /**
             * @brief 执行一次上传
             * @param [in] resource    资源
             * @param [in] data        数据
             * @param [in] objectCount 对象个数，如果为0，则为ArrSz个
             * @return 执行成功返回true，否则返回false
             */
            template <typename T, size_t ArrSz>
            bool Upload(Ncp::Resource resource, T (&data)[ArrSz], size_t objectCount = 0)
            {
                std::lock_guard<mutex_type> lock(m_lock);
                return Upload(Packet::Create(++m_pktId, Ncp::Method::Upload, resource, m_registerStruct.sn.type, data, objectCount));                
            }

            /**
             * @brief 执行一次上传
             * @param [in] resource    资源
             * @param [in] data        数据
             * @param [in] objectCount 对象个数，如果为0，则为data长度
             * @return 执行成功返回true，否则返回false
             */
            template <typename T>
            bool Upload(Ncp::Resource resource, std::vector<T> data, size_t objectCount = 0)
            {
               std::lock_guard<mutex_type> lock(m_lock);
                return Upload(Packet::Create(++m_pktId, Ncp::Method::Upload, resource, m_registerStruct.sn.type, data, objectCount));                 
            }

        protected:
            /// 连接处理函数
            virtual void OnConnected() override;

            /// 断开处理函数
			virtual void OnDisconnected() override;

            /// 注册完成处理函数
            virtual void OnRegisted();

            /// 连接失败处理函数
			virtual void OnConnectFailed() override;

            /**
             * @brief 数据到达处理函数
             * @param [in] buffer 读到的数据
             * @return 返回下次将要读取的字节数（0表示缺省缓冲区大小）
             */
            size_t OnRead(std::shared_ptr<const std::vector<char>> buffer) override;

            /**
             * @brief 添加数据
             * @param [in] ptr   数据指针
             * @param [in] bytes 数据长度
             */
            void PushData(const char * ptr, size_t bytes);         

            /// 分发数据请求
            void DispatchRequest();

            /// 已执行包超时处理函数
            void HandleExecutedTimeout(unsigned short pktId)
            {
                std::lock_guard<mutex_type> lock(m_lock);
                m_inputsExecutedById.erase(pktId);
            }

            /**
             * @brief 设置超时时间
             * @param [in] timeout 超时时间
             */
            void SetTimeout(size_t timeout);

            /// Ncp请求处理函数
            virtual void OnRequest(std::shared_ptr<const Packet> pkt);

            /**
             * @brief 发起一次上传（客户端全部采用异步上传）
             * @param [in] pkt 上传数据包
             * @return 执行成功返回true，否则返回false
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
            /// 超时定时器（连接服务器超时、连接活动超时）
            IoService::InvokeState    m_timerTimeout;
            
            /// 发送出去的注册结构体
            RegisterStruct      m_registerStruct;

            /// 接收到的注册回应包
            RegisterPacket      m_registerRecv;

            /// 是否已注册
            bool                m_registed = false;

            /// 当前输入包
            std::shared_ptr<Packet> m_inputPkt;

            /// 请求（主动上传）映射表
            std::map<unsigned short, std::shared_ptr<IoService::Request<Packet>>>  m_requestsById;

            /// 输入包映射表 
            std::map<unsigned short, std::shared_ptr<Packet>>   m_inputsById;

            /// 输入包队列
            std::queue<std::weak_ptr<Packet>>                   m_inputs;

            /// 已执行包映射表
            std::map<unsigned short, std::shared_ptr<Packet>>   m_inputsExecutedById;

            /// 活动超时时间（毫秒）
            size_t              m_activeTimeout = 60000u;

            /// 当前使用的请求包流水号
            std::atomic<unsigned short> m_pktId = { 0 };

            /// 单次重发超时
            size_t              m_timeoutOnce = 10000u;
            /// 最大重发次数
            size_t              m_maxResendTimes = 3;
        };
    };
};

#endif