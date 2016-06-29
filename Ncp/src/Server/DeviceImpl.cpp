////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file DeviceImpl.cpp
/// @brief Ncp协议服务端设备实现类实现文件
///
/// 实现Ncp协议服务端设备实现类
///
/// @version 1.0
/// @author  李守红
/// @date    2013年12月1日
///
///
/// 修订说明：最初版本
///
////////////////////////////////////////////////////////////////////////

#include <map>
#include <string>
#include <queue>
#include <algorithm>

#include <cassert>

#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "../Ncp.h"
#include "../NcpImpl.hpp"

#include "IoService.h"
#include "TcpSocket.h"

#include "Device.h"
#include "Server.h"

#include "ServerImpl.h"
#include "DeviceImpl.h"

namespace Ncp
{
    namespace Server
    {
        void DeviceImpl::Close()
        {
            std::lock_guard<mutex_type> lock(m_lock);

			m_timeoutState.Cancel();

            m_registed = false;
            m_registerRecv.Clear();
            m_inputPkt.reset();

            TcpSocket::Close();
        }

        void DeviceImpl::OnConnected()
        {
            std::lock_guard<mutex_type> lock(m_lock);

            BeginAutoRead();

            // 建立连接后，必须在指定时间内注册成功，否则断开
			m_timeoutState = IoService::Invoke(std::bind(&DeviceImpl::HandleRegisterTimeout, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this())),
                                        std::chrono::milliseconds(30000));
        }

        void DeviceImpl::OnDisconnected()
        {
            std::lock_guard<mutex_type> lock(m_lock);

			m_timeoutState.Cancel();

            if (m_device != nullptr)
            {
                m_device->OnDisconnected();
                m_device = nullptr;
            }

            for (auto v : m_requestsById)
            {
                v.second->Wake();
            }

            m_requestsById.clear();
			m_inputsExecutedById.clear();

            m_serverImpl.OnDeviceDisconnected(std::dynamic_pointer_cast<DeviceImpl>(shared_from_this()));
        }

        /// 注册超时处理函数
        void DeviceImpl::HandleRegisterTimeout()
        {
            std::lock_guard<mutex_type> lock(m_lock);

            if (!m_registed)
            {
                Close();
            }
        }

        /**
            * @brief 添加数据
            * @param [in] ptr   数据指针
            * @param [in] bytes 数据长度
            */
        void DeviceImpl::PushData(const char * ptr, size_t bytes)
        {
            if (ptr == nullptr || bytes == 0)
            {
                return;
            }

            const char * end = nullptr;
            ErrorCode ec;

            // 判断是否未注册完成
            if (!m_registed)
            {
                if (m_registerRecv.Completed())
                {
                    m_registerRecv.Clear();
                }

                ec = m_registerRecv.Append(ptr, bytes, &end);

                if (ec == ErrorCode::OK)
                {
                    // 注册结构体校验
                    ec = m_registerRecv.GetStruct().Validate();
                    RegisterStruct & rs = m_registerRecv.GetStruct();

                    if (ec == ErrorCode::OK) // 校验成功
                    {
                        // 获取设备对象
                        Device * device = m_serverImpl.OnDeviceRegisted(rs, m_addrRemote);

                        if (device == nullptr)
                        {
                            m_serverImpl.OnDeviceRegistFail(rs, m_addrRemote, ErrorCode::CreateDeviceFailed);
                        }
                        else if (device->IsConnected())
                        {
                            printf("设备已连接！重复注册！！！！\n");
                            m_serverImpl.OnDeviceRegistFail(rs, m_addrRemote, ErrorCode::SerialNumberExists);
                        }
                        else
                        {
                            m_device = device;
                            m_device->m_sn = rs.sn;

                            m_device->m_ncpVersion = rs.ncpVersion;
                            m_device->m_softwareVersion = rs.softVersion;
                            m_device->m_platformVersion = rs.platVersion;

                            m_device->m_addr = m_addrRemote;

                            /*
                             此处存在访问冲突风险：用户线程可能与本线程出现m_device->m_impl的访问冲突
                             */
                            if (m_device->m_impl != nullptr)
                            {
                                auto impl = m_device->m_impl->lock();

                                if (impl && impl.get() != this)
                                {
                                    impl->Close();
                                }

                                auto p = m_device->m_impl;
                                m_device->m_impl = nullptr;
                                delete p;
                            }

                            m_device->m_impl = new std::weak_ptr<DeviceImpl>(std::dynamic_pointer_cast<DeviceImpl>(shared_from_this()));
                            m_device->OnConnected();

                            m_activateCount = 0;
                            m_registed = true;

                            m_registerRecv.GetStruct().Encrypt();

                            // 发送注册回应包
                            Write(m_registerRecv.GetBuffer());

                            // 设置激活超时定时器
							m_timeoutState = IoService::Invoke(std::bind(&DeviceImpl::HandleActiveTimeout, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this()), true),
                                                            std::chrono::milliseconds(m_activeTimeoutFirst));
                        }
                    }
                    else // 校验失败
                    {
                        m_serverImpl.OnDeviceRegistFail(rs, m_addrRemote, ec);
                        
                        printf("%s[%s:%d] validating register struct failed, error: %d %s\n", __FILE__, __func__, __LINE__, (int)ec, std::to_string<char>(ec).c_str());
                        
                        RegisterPacket pkt(std::move(m_registerRecv));

                        // 从下一个字节开始重新分析注册包    
                        PushData(&pkt.GetBuffer()->at(1), pkt.GetBuffer()->size() - 1);
                    }
                }
            }
            else
            {
                // 将数据添加到当前包
                if (!m_inputPkt)
                {
                    m_inputPkt.reset(new Packet());
                }

                ErrorCode ec = m_inputPkt->Append(ptr, bytes, &end);

                if (ec == ErrorCode::PacketNotComplete) // 未完整
                {
                    return;
                }
                else
                {
                    if (ec == ErrorCode::OK) // 已完整
                    {
						/***** 异常监测&修复 *****/
						if (!m_connected && m_fd != -1)
						{
							printf("Error detected! m_connected:%d m_fd:%d!\n", m_connected ? 1 : 0, m_fd);
							IoService::Invoke(std::bind(&DeviceImpl::Close, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this())));
						}
						/******************/

                        m_activateCount = 0;

                        // 设置激活超时定时器
						m_timeoutState = IoService::Invoke(std::bind(&DeviceImpl::HandleActiveTimeout, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this()), true),
							std::chrono::milliseconds(m_activeTimeoutFirst));

                        if (m_inputPkt->IsResponse()) // 是回应包
                        {
                            // 查找对应的请求包
                            auto it = m_requestsById.find(m_inputPkt->GetHead()->id);

                            if (it != m_requestsById.end())
                            {
                                it->second->Wake(m_inputPkt);
                                m_requestsById.erase(it);
                            }
                        }
                        else
                        {
                            /*
                                可能出现重复接收情况：
                                请求包已经接收到，但正在排队、处理，尚未发送回应包；
                                已经发送回应包，但对方尚未接收到，或者回应包丢失；
                                出现上述情况时，对方可能执行数据重发。为了防止再次处理发来的重发包，有必要设立已执行队列。
                                */
                            // 当前请求队列和已执行队列均不存在相同ID的请求，才认为是正确的请求
                            if (m_inputsById.find(m_inputPkt->GetHead()->id) == m_inputsById.end() &&
                                (!m_dispatchingPkt || m_dispatchingPkt->GetHead()->id != m_inputPkt->GetHead()->id) &&
                                m_inputsExecutedById.find(m_inputPkt->GetHead()->id) == m_inputsExecutedById.end())
                            {
                                m_inputsById.insert({m_inputPkt->GetHead()->id, m_inputPkt});
                                m_inputs.push(m_inputPkt);

                                // 提交一个分发事件
                                IoService::Invoke(std::bind(&DeviceImpl::DispatchRequestOnce, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this())));
                            }
                        }

                        // 清除当前包
                        m_inputPkt.reset();
                    }

                    /*
                      如果Append返回的错误码不是OK和PacketNotComplete，则表示该数据包校验错误，应当从第二个字节开始重新分析数据包
                     */
                    else if (m_inputPkt->GetSize())
                    {
                        std::shared_ptr<Packet> pktTmp = m_inputPkt;

                        m_inputPkt.reset();
                        PushData(&pktTmp->GetBuffer()->at(1), pktTmp->GetSize() - 1);
                    }
                }  
            }

            if (end != nullptr)
            {
                PushData(end, bytes - (end - ptr));
            }
        }

        size_t DeviceImpl::OnRead(std::shared_ptr<const std::vector<char>> buffer)
        {
            std::lock_guard<mutex_type> lock(m_lock);

            if (!buffer || buffer->empty())
            {
                // 断开连接
                printf("[%p]Close:OnRead read failed:%p\n", this, buffer.get());
                Close();
            }
            else
            {
                PushData(&buffer->at(0), buffer->size());
            }

            return TcpSocket::OnRead(buffer);
        }

        /**
            * @brief 激活超时回调函数
            * @param [in] isFirst 是否是初次超时
            */
        void DeviceImpl::HandleActiveTimeout(bool isFirst)
        {
            std::lock_guard<mutex_type> lock(m_lock);

            if (m_device == NULL)
            {
                return;
            }

            // 初次标记应与激活次数一致
            if (isFirst != (m_activateCount == 0))
            {
                return;
            }

            if (m_activateCount >= m_maxActivateTimes) // 超过激活次数则断开
            {
                printf("Close:ActivateCount !!!\n");
                Close();
            }
            else
            {
                ++m_activateCount;

                // 准备激活包
                if (!m_pktActivate)
                {
                    m_pktActivate = Packet::Create(++m_pktId, Method::Read, Resource::Status, m_device->GetDeviceType()->GetId());
                }
                else
                {
                    m_pktActivate->GetHead()->id = ++m_pktId;
                }

                if (IsConnected())
                {
                    // 发送激活数据包
                    Write(m_pktActivate->GetBuffer());

                    // 设置后续超时
					m_timeoutState = IoService::Invoke(std::bind(&DeviceImpl::HandleActiveTimeout, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this()), false),
						std::chrono::milliseconds(m_activeTimeoutNext));
                }
            }
        }

        void DeviceImpl::CancelRequest(unsigned short id, bool callback)
        {
        }


        bool DeviceImpl::SendRequest(std::shared_ptr<Packet> pkt, std::function<void(std::shared_ptr<Packet>)> callback, size_t timeout)
        {
            std::lock_guard<mutex_type> lock(m_lock);
			
			if (!IsConnected())
			{
				return false;
			}

			auto id = pkt->GetHead()->id;
            
            // 异步调用函数的this指针应当使用智能指针
            auto _this = std::dynamic_pointer_cast<DeviceImpl>(shared_from_this());
			std::shared_ptr<IoService::Request<Packet>> req(new IoService::Request<Packet>(shared_from_this(), pkt, timeout, [_this, callback, id](std::shared_ptr<Packet> response){ 
				if (!response) //如果没有收到回应则从请求列表中删除该请求包
				{
					std::lock_guard<mutex_type> lock(_this->m_lock);
					_this->m_requestsById.erase(id);
				}

				if (callback)
				{
					callback(response);
				}
			}));

            if (!req->Send(m_timeoutOnce, m_maxResendTimes))
            {
                return false;
            }

            m_requestsById.insert({pkt->GetHead()->id, req});
            return true;
        }

        std::shared_ptr<Packet> DeviceImpl::SendRequest(std::shared_ptr<Packet> pkt, size_t timeout)
        {
			std::shared_ptr<IoService::Request<Packet>> req(new IoService::Request<Packet>(shared_from_this(), pkt, timeout));
            
			if (!IsConnected())
			{
				return false;
			}

            {
                std::lock_guard<mutex_type> lock(m_lock);

                if (!req->Send(m_timeoutOnce, m_maxResendTimes))
                {
                    return nullptr;
                }

                m_requestsById.insert({pkt->GetHead()->id, req});
            } 

            auto ret = req->Wait();
			
			if (!ret) //如果一直没有收到回应，则在此处删除请求包
            {
                std::lock_guard<mutex_type> lock(m_lock);
                m_requestsById.erase(pkt->GetHead()->id);
            } 

			return ret;
        }

        /**
            * @brief 发送一个回应包
            * @param [in] pkt 回应包
            * @return 发送成功返回true，否则返回false
            */
        bool DeviceImpl::SendResponse(std::shared_ptr<Packet> pkt)
        {
            assert(pkt->IsResponse());

            if (IsConnected())
            {
                return Write(pkt->GetBuffer(), nullptr);
            }

            return false;
        }
            
        /// 分发一次数据请求
        void DeviceImpl::DispatchRequestOnce()
        {
            std::lock_guard<mutex_type> lock(m_lock);

            while(!m_inputs.empty())
            {
                m_dispatchingPkt = m_inputs.front().lock();

                m_inputs.pop();

                if (m_dispatchingPkt)
                {
                    if (m_device)
                    {
                        m_device->OnUpload(m_dispatchingPkt);
                        m_device->UploadDispatch(m_dispatchingPkt);
                    }

                    m_inputsById.erase(m_dispatchingPkt->GetHead()->id);

                    m_inputsExecutedById.insert({ m_dispatchingPkt->GetHead()->id, m_dispatchingPkt} );

                    IoService::Invoke(std::bind(&DeviceImpl::HandleExecutedTimeout, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this()), m_dispatchingPkt->GetHead()->id),
                                        std::chrono::milliseconds((m_maxResendTimes + 1) * m_timeoutOnce));
                        
                    m_dispatchingPkt.reset();

                    if (!m_inputs.empty())
                    {
                        IoService::Invoke(std::bind(&DeviceImpl::DispatchRequestOnce, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this())));
                    }
                    break;
                }
            }
        }

        /// 已执行包超时处理函数
        void DeviceImpl::HandleExecutedTimeout(unsigned short pktId)
        {
            std::lock_guard<mutex_type> lock(m_lock);
            m_inputsExecutedById.erase(pktId);
        }

    };
};
