////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file DeviceImpl.cpp
/// @brief NcpЭ�������豸ʵ����ʵ���ļ�
///
/// ʵ��NcpЭ�������豸ʵ����
///
/// @version 1.0
/// @author  ���غ�
/// @date    2013��12��1��
///
///
/// �޶�˵��������汾
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

            // �������Ӻ󣬱�����ָ��ʱ����ע��ɹ�������Ͽ�
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

        /// ע�ᳬʱ������
        void DeviceImpl::HandleRegisterTimeout()
        {
            std::lock_guard<mutex_type> lock(m_lock);

            if (!m_registed)
            {
                Close();
            }
        }

        /**
            * @brief �������
            * @param [in] ptr   ����ָ��
            * @param [in] bytes ���ݳ���
            */
        void DeviceImpl::PushData(const char * ptr, size_t bytes)
        {
            if (ptr == nullptr || bytes == 0)
            {
                return;
            }

            const char * end = nullptr;
            ErrorCode ec;

            // �ж��Ƿ�δע�����
            if (!m_registed)
            {
                if (m_registerRecv.Completed())
                {
                    m_registerRecv.Clear();
                }

                ec = m_registerRecv.Append(ptr, bytes, &end);

                if (ec == ErrorCode::OK)
                {
                    // ע��ṹ��У��
                    ec = m_registerRecv.GetStruct().Validate();
                    RegisterStruct & rs = m_registerRecv.GetStruct();

                    if (ec == ErrorCode::OK) // У��ɹ�
                    {
                        // ��ȡ�豸����
                        Device * device = m_serverImpl.OnDeviceRegisted(rs, m_addrRemote);

                        if (device == nullptr)
                        {
                            m_serverImpl.OnDeviceRegistFail(rs, m_addrRemote, ErrorCode::CreateDeviceFailed);
                        }
                        else if (device->IsConnected())
                        {
                            printf("�豸�����ӣ��ظ�ע�ᣡ������\n");
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
                             �˴����ڷ��ʳ�ͻ���գ��û��߳̿����뱾�̳߳���m_device->m_impl�ķ��ʳ�ͻ
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

                            // ����ע���Ӧ��
                            Write(m_registerRecv.GetBuffer());

                            // ���ü��ʱ��ʱ��
							m_timeoutState = IoService::Invoke(std::bind(&DeviceImpl::HandleActiveTimeout, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this()), true),
                                                            std::chrono::milliseconds(m_activeTimeoutFirst));
                        }
                    }
                    else // У��ʧ��
                    {
                        m_serverImpl.OnDeviceRegistFail(rs, m_addrRemote, ec);
                        
                        printf("%s[%s:%d] validating register struct failed, error: %d %s\n", __FILE__, __func__, __LINE__, (int)ec, std::to_string<char>(ec).c_str());
                        
                        RegisterPacket pkt(std::move(m_registerRecv));

                        // ����һ���ֽڿ�ʼ���·���ע���    
                        PushData(&pkt.GetBuffer()->at(1), pkt.GetBuffer()->size() - 1);
                    }
                }
            }
            else
            {
                // ��������ӵ���ǰ��
                if (!m_inputPkt)
                {
                    m_inputPkt.reset(new Packet());
                }

                ErrorCode ec = m_inputPkt->Append(ptr, bytes, &end);

                if (ec == ErrorCode::PacketNotComplete) // δ����
                {
                    return;
                }
                else
                {
                    if (ec == ErrorCode::OK) // ������
                    {
						/***** �쳣���&�޸� *****/
						if (!m_connected && m_fd != -1)
						{
							printf("Error detected! m_connected:%d m_fd:%d!\n", m_connected ? 1 : 0, m_fd);
							IoService::Invoke(std::bind(&DeviceImpl::Close, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this())));
						}
						/******************/

                        m_activateCount = 0;

                        // ���ü��ʱ��ʱ��
						m_timeoutState = IoService::Invoke(std::bind(&DeviceImpl::HandleActiveTimeout, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this()), true),
							std::chrono::milliseconds(m_activeTimeoutFirst));

                        if (m_inputPkt->IsResponse()) // �ǻ�Ӧ��
                        {
                            // ���Ҷ�Ӧ�������
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
                                ���ܳ����ظ����������
                                ������Ѿ����յ����������Ŷӡ�������δ���ͻ�Ӧ����
                                �Ѿ����ͻ�Ӧ�������Է���δ���յ������߻�Ӧ����ʧ��
                                �����������ʱ���Է�����ִ�������ط���Ϊ�˷�ֹ�ٴδ��������ط������б�Ҫ������ִ�ж��С�
                                */
                            // ��ǰ������к���ִ�ж��о���������ͬID�����󣬲���Ϊ����ȷ������
                            if (m_inputsById.find(m_inputPkt->GetHead()->id) == m_inputsById.end() &&
                                (!m_dispatchingPkt || m_dispatchingPkt->GetHead()->id != m_inputPkt->GetHead()->id) &&
                                m_inputsExecutedById.find(m_inputPkt->GetHead()->id) == m_inputsExecutedById.end())
                            {
                                m_inputsById.insert({m_inputPkt->GetHead()->id, m_inputPkt});
                                m_inputs.push(m_inputPkt);

                                // �ύһ���ַ��¼�
                                IoService::Invoke(std::bind(&DeviceImpl::DispatchRequestOnce, std::dynamic_pointer_cast<DeviceImpl>(shared_from_this())));
                            }
                        }

                        // �����ǰ��
                        m_inputPkt.reset();
                    }

                    /*
                      ���Append���صĴ����벻��OK��PacketNotComplete�����ʾ�����ݰ�У�����Ӧ���ӵڶ����ֽڿ�ʼ���·������ݰ�
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
                // �Ͽ�����
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
            * @brief ���ʱ�ص�����
            * @param [in] isFirst �Ƿ��ǳ��γ�ʱ
            */
        void DeviceImpl::HandleActiveTimeout(bool isFirst)
        {
            std::lock_guard<mutex_type> lock(m_lock);

            if (m_device == NULL)
            {
                return;
            }

            // ���α��Ӧ�뼤�����һ��
            if (isFirst != (m_activateCount == 0))
            {
                return;
            }

            if (m_activateCount >= m_maxActivateTimes) // �������������Ͽ�
            {
                printf("Close:ActivateCount !!!\n");
                Close();
            }
            else
            {
                ++m_activateCount;

                // ׼�������
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
                    // ���ͼ������ݰ�
                    Write(m_pktActivate->GetBuffer());

                    // ���ú�����ʱ
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
            
            // �첽���ú�����thisָ��Ӧ��ʹ������ָ��
            auto _this = std::dynamic_pointer_cast<DeviceImpl>(shared_from_this());
			std::shared_ptr<IoService::Request<Packet>> req(new IoService::Request<Packet>(shared_from_this(), pkt, timeout, [_this, callback, id](std::shared_ptr<Packet> response){ 
				if (!response) //���û���յ���Ӧ��������б���ɾ���������
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
			
			if (!ret) //���һֱû���յ���Ӧ�����ڴ˴�ɾ�������
            {
                std::lock_guard<mutex_type> lock(m_lock);
                m_requestsById.erase(pkt->GetHead()->id);
            } 

			return ret;
        }

        /**
            * @brief ����һ����Ӧ��
            * @param [in] pkt ��Ӧ��
            * @return ���ͳɹ�����true�����򷵻�false
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
            
        /// �ַ�һ����������
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

        /// ��ִ�а���ʱ������
        void DeviceImpl::HandleExecutedTimeout(unsigned short pktId)
        {
            std::lock_guard<mutex_type> lock(m_lock);
            m_inputsExecutedById.erase(pktId);
        }

    };
};
