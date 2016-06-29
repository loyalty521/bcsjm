 ////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file Client.cpp
/// @brief Ncp客户端类实现文件
///
/// 实现Ncp客户端类
///
/// @version 1.0
/// @author  李守红
/// @date    2013年12月1日
///
///
/// 修订说明：最初版本
///
////////////////////////////////////////////////////////////////////////

#include "Client.h"

static std::atomic<unsigned int> connected_count = {0};
static std::atomic<unsigned int> registed_count = {0};

namespace Ncp
{
    namespace Client
    {
        Client::Client(const Address & addrRemote, const Address & addrLocal, 
            Version softVersion, Version platVersion, const SerialNumber & sn, size_t activeTimeout, 
                size_t timeoutOnce, size_t maxResendTimes) :
            IoService::TcpClient(addrRemote, addrLocal),
            m_activeTimeout(activeTimeout),
            m_timeoutOnce(timeoutOnce),
            m_maxResendTimes(maxResendTimes)
        {
            // 填充注册包
            m_registerStruct.magicHead = RegisterStruct::MAGIC_HEAD;
            m_registerStruct.magicTail = RegisterStruct::MAGIC_TAIL;
            m_registerStruct.softVersion = softVersion;
            m_registerStruct.platVersion = platVersion;
            m_registerStruct.ncpVersion = GetVersion();
            m_registerStruct.sn = sn;
        }

        void Client::SetSerialNumber(const SerialNumber & sn)
        {
            std::lock_guard<mutex_type> lock(m_lock);
                
            if (sn != m_registerStruct.sn)
            {
                m_registerStruct.sn = sn;

                if (m_connected)
                {
                    Close(); //将触发重新连接
                }
            }
        }

        void Client::SetServerAddress(const Address & addrRemote)
        {
            std::lock_guard<mutex_type> lock(m_lock);

            if (addrRemote != m_addrRemote)
            {
                m_addrRemote = addrRemote;

                if (IsConnected())
                {
                    Close();
                }
            }
        }
        void Client::SetTimeout(size_t timeout)
        {
			m_timerTimeout = IoService::Invoke(std::bind(&Client::Close, std::dynamic_pointer_cast<Client>(shared_from_this())),
                std::chrono::milliseconds(timeout));   
        }

        void Client::Close()
        {
            std::lock_guard<mutex_type> lock(m_lock);

            m_registed = false;
            m_registerRecv.Clear();

            TcpClient::Close();
        }

        void Client::OnRequest(std::shared_ptr<const Packet> pkt)
        {
            std::lock_guard<mutex_type> lock(m_lock);

            auto pktResponse = Packet::Create(pkt, ErrorCode::OperateNotSupport);

            if (pktResponse)
            {
                Write(pktResponse->GetBuffer());
            }
        }

        void Client::OnConnectFailed()
        {
			TcpClient::OnConnectFailed();
        }


        void Client::OnConnected()
        {
            std::lock_guard<mutex_type> lock(m_lock);

            if (BeginAutoRead())
            { 
                // 填充注册结构体加密字段
                srand((unsigned int)time(nullptr));
                for (unsigned int i = 0; i < sizeof(m_registerStruct.encrypting); ++i)
                {
                    m_registerStruct.encrypting[i] = rand() % 256;
                }
            
                m_registed = false;
                m_registerRecv.Clear();

                std::shared_ptr<std::vector<char>> buffer(new std::vector<char>((char *)&m_registerStruct, (char *)(&m_registerStruct + 1)));

                std::cout << m_registerStruct.sn.to_string() << std::endl;
                Write(buffer);
                //printf("the sn is %s\n",m_registerStruct.sn);
                m_registerStruct.Encrypt();
            }
            else
            {
                //printf("failed!\n");    
            }
            //printf("%s[%s:%d]\n", __FILE__, __func__, __LINE__);
            SetTimeout(m_activeTimeout);
            //printf("%s[%s:%d]\n", __FILE__, __func__, __LINE__);

			TcpClient::OnConnected();
        }

        void Client::OnDisconnected()
        {
            std::lock_guard<mutex_type> lock(m_lock); 

            m_timerTimeout.Cancel();
			m_requestsById.clear();
			m_inputPkt.reset();
			std::queue<std::weak_ptr<Packet>>().swap(m_inputs);
			m_inputsById.clear();
			m_inputsExecutedById.clear();
			m_registed = false;
			m_registerRecv.Clear();

			TcpClient::OnDisconnected();
        }

        void Client::OnRegisted()
        {
            //printf("OnRegisted:%u\n", ++registed_count);
        }

        size_t Client::OnRead(std::shared_ptr<const std::vector<char>> buffer)
        {
            std::lock_guard<mutex_type> lock(m_lock);

            //printf("buffer: %p ", buffer.get());

            //if (buffer)
            //{
            //    printf("%u\n", buffer->size());
            //}
            //else
            //{
            //    printf("\n");
            //}

            if (!buffer || buffer->empty())
            {
                Close();
            }
            else
            {
                PushData(&buffer->at(0), buffer->size());
            }

            return 0;
        }

        void Client::PushData(const char * ptr, size_t bytes)
        {
            if (ptr == nullptr || bytes == 0)
            {
                return;
            }

            const char * end = nullptr;
            ErrorCode ec;

            //printf("m_registed: %d m_registerStructRecvSize:%u\n", m_registed, m_registerStructRecvSize);
            // 判断是否注册完成
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
                    ec = m_registerRecv.GetStruct().Validate(m_registerStruct);

                    if (ec == ErrorCode::OK) // 校验成功
                    {
                        m_registed = true;
                        //printf("%s[%s:%d] Ncp Registed!\n", __FILE__, __func__, __LINE__);

                        OnRegisted();
                        SetTimeout(m_activeTimeout);
                    }
                    else // 校验失败
                    {
                        printf("%s[%s:%d] InvalidRegisterStruct!!!! ec:%d\n", __FILE__, __func__, __LINE__, ec);

                        IoService::Invoke(std::bind(&Client::Close, std::dynamic_pointer_cast<Client>(shared_from_this()))); 
                        bytes = 0;

                        return;
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

                ec = m_inputPkt->Append(ptr, bytes, &end);

                if (ec == ErrorCode::PacketNotComplete) // 未完整
                {
                    return;
                }
                else
                {
                    if (ec == ErrorCode::OK) // 已完整
                    {
                        //printf("%s[%s:%d] pkt received! method:%d resource:%d\n", __FILE__, __func__, __LINE__, (int)m_inputPkt->GetHead()->method, (int)m_inputPkt->GetHead()->resource);
                        
                        // 设置超时定时器
                        SetTimeout(m_activeTimeout);

                        if (m_inputPkt->IsResponse()) // 是回应包
                        {
                            //printf("%s[%s:%d] received a response!\n", __FILE__, __func__, __LINE__);
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
                            //printf("%s[%s:%d] received a request!\n", __FILE__, __func__, __LINE__);

                            /*
                             可能出现重复接收情况：
                             请求包已经接收到，但正在排队、处理，尚未发送回应包；
                             已经发送回应包，但对方尚未接收到，或者回应包丢失；
                             出现上述情况时，对方可能执行数据重发。为了防止再次处理发来的重发包，有必要设立已执行队列。
                             */
                            // 当前请求队列和已执行队列均不存在相同ID的请求，才认为是正确的请求
                            if (m_inputsById.find(m_inputPkt->GetHead()->id) == m_inputsById.end() &&
                                m_inputsExecutedById.find(m_inputPkt->GetHead()->id) == m_inputsExecutedById.end())
                            {
                                //printf("%s[%s:%d] push it into m_inputs!\n", __FILE__, __func__, __LINE__);

                                m_inputsById.insert({m_inputPkt->GetHead()->id, m_inputPkt});
                                m_inputs.push(m_inputPkt);

                                // 提交一个分发事件
                                IoService::Invoke(std::bind(&Client::DispatchRequest, std::dynamic_pointer_cast<Client>(shared_from_this())));
                            }
                        }

                        // 清除当前包
                        m_inputPkt.reset();
                    }
                    //如果Append返回的错误码不是OK和PacketNotComplete，则表示该数据包校验错误，应当从第二个字节开始重新分析数据包
                    else if (m_inputPkt->GetSize() > 0)
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

        // 分发一次数据请求
        void Client::DispatchRequest()
        {   
            std::shared_ptr<Ncp::Packet> pkt;

            do
            {
                pkt.reset();

                {
                    std::lock_guard<mutex_type> lock(m_lock);

                    if (m_inputs.empty())
                    {
                        return;
                    }

                    pkt = m_inputs.front().lock();
                    m_inputs.pop();
                }

                if (pkt)
                {
                    OnRequest(pkt);

                    std::lock_guard<mutex_type> lock(m_lock);
					auto head = pkt->GetHead();

                    m_inputsById.erase(head->id);
                    m_inputsExecutedById.insert({ head->id, pkt} );

                    IoService::Invoke(std::bind(&Client::HandleExecutedTimeout, std::dynamic_pointer_cast<Client>(shared_from_this()), head->id),
                                        std::chrono::seconds(60));
                }
            } while(pkt);
        }    
    }
}