////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file Device.cpp
/// @brief Ncp协议服务端设备基类实现文件
///
/// 实现Ncp协议服务端设备基类
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
#include "../NcpImpl.hpp"

#include "Device.h"
#include "Server.h"
#include "ServerImpl.h"

namespace Ncp
{
    namespace Server
    {
        Device::~Device()
        {
			Disconnect();
        }

        std::shared_ptr<DeviceImpl> Device::QueryImpl() const
        {
            if (m_impl)
            {
                return m_impl->lock();
            }

            return nullptr;
        }

        bool Device::IsConnected() const
        {
            if (m_impl == nullptr)
            {
                return false;
            }

            std::shared_ptr<DeviceImpl> impl = m_impl->lock();
            return impl && impl->IsConnected();
        }

        void Device::Disconnect()
        {
            if (m_impl)
            {
                auto impl = m_impl->lock();

                if (impl)
                {
                    printf("Close:Disconnect\n");
                    impl->Close();
                }
                
                delete m_impl;
				m_impl = nullptr;
            }
        }

        // 从仅包含时间的数据包中取出时间（带错误处理）
        static inline ErrorCode PktToTime(std::shared_ptr<Packet> pkt, std::array<unsigned char, 6> & time)
        {
            if (pkt && pkt->GetHead()->method == Method::Reply)
            {
                if (pkt->GetHead()->appBytes != 6)
                {
                    return ErrorCode::InvalidData;
                }
                else
                {
                    std::copy(pkt->GetObjects(), pkt->GetObjects() + 6, time.begin());

                    return ErrorCode::OK;
                }
            }

            ErrorCode ec = ReqError(pkt);
            return ec == ErrorCode::OK ? ErrorCode::InvalidData : ec;            
        }

        ErrorCode Device::GetDeviceTime(std::array<unsigned char, 6> & time, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::SystemTime, m_sn.type), timeout);

            return PktToTime(pkt, time);
        }
        ErrorCode Device::GetDeviceTime(std::function<void(const std::array<unsigned char, 6> & time, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Read, Resource::SystemTime, m_sn.type), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { 
                        std::array<unsigned char, 6> time; 
                        ErrorCode ec = PktToTime(pkt, time); 
                        callback(time, ec); 
                    }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;            
        }

        ErrorCode Device::SetDeviceTime(size_t timeout)
        {
            time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm _tm;
            localtime_s(&_tm, &tt);

            return SetDeviceTime(std::array<unsigned char, 6>{{_tm.tm_year - 100, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec}}, timeout);
        }
        ErrorCode Device::SetDeviceTime(std::function<void(ErrorCode)> callback, size_t timeout)
        {
            time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm _tm;
            localtime_s(&_tm, &tt);

            return SetDeviceTime(std::array<unsigned char, 6>{{_tm.tm_year - 100, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec}}, callback, timeout);
        }

        ErrorCode Device::SetDeviceTime(const std::array<unsigned char, 6> & time, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Write, Resource::SystemTime, m_sn.type, std::vector<unsigned char>{ time.begin(), time.end() }, 1), 
                timeout);

            return ReqError(pkt);
        }
        ErrorCode Device::SetDeviceTime(const std::array<unsigned char, 6> & time, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Write, Resource::SystemTime, m_sn.type, std::vector<unsigned char>{ time.begin(), time.end() }, 1), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        // 从含有模拟量（类型1字节、值4字节）中取出数据
        static inline ErrorCode PktToAnalog(std::shared_ptr<Packet> pkt, unsigned char & type, unsigned int & analogValue)
        {
            if (pkt && pkt->GetHead()->method == Method::Reply)
            {
                if (pkt->GetHead()->appBytes != 4)
                {
                    return ErrorCode::InvalidData;
                }
                else
                {
                    type = ((unsigned char *)pkt->GetObjects())[0];
                    memcpy(&analogValue, pkt->GetObjects() + 1, 4);
                    //analogValue = *(_UNALIGNED_<unsigned int> *)(pkt->GetObjects() + 1);

                    return ErrorCode::OK;
                }
            }

            ErrorCode ec = ReqError(pkt);
            return ec == ErrorCode::OK ? ErrorCode::InvalidData : ec;            
        }

        static inline ErrorCode PktToAnalog(std::shared_ptr<Packet> pkt, unsigned int & analogValue)
        {
            unsigned char type;
            return PktToAnalog(pkt, type, analogValue);
        }

        ErrorCode Device::GetAnalogValue(unsigned char type, unsigned int & value, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::AnalogValue, m_sn.type, type), timeout);

            return PktToAnalog(pkt, value);            
        }

        ErrorCode Device::GetAnalogValue(unsigned char type, std::function<void(unsigned int, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Read, Resource::AnalogValue, m_sn.type, type), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { 
                        unsigned int value; 
                        ErrorCode ec = PktToAnalog(pkt, value); 
                        callback(value, ec); 
                    }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;              
        }

        // 从含有物理位置的数据包中取出数据
        static inline ErrorCode PktToLocation(std::shared_ptr<Packet> pkt, std::wstring & location)
        {
            if (pkt && (pkt->GetHead()->method == Method::Reply || pkt->GetHead()->method == Method::Upload))
            {
                if (pkt->GetHead()->appBytes < 2)
                {
                    return ErrorCode::InvalidData;
                }
                else
                {
                    unsigned short length;
                    memcpy(&length, pkt->GetObjects(), 2);

                    if (pkt->GetHead()->appBytes != 2 + length)
                    {
                        return ErrorCode::InvalidData;
                    }

                    // 计算转换后需要的缓冲区长度
                    int bufferSize = MultiByteToWideChar(936, 0, pkt->GetObjects() + 2, length, NULL, 0);

                    std::vector<wchar_t> buffer(bufferSize, 0);
                    MultiByteToWideChar(936, 0, pkt->GetObjects() + 2, length, &buffer[0], bufferSize);

                    location.clear();
                    location.insert(location.begin(), buffer.begin(), buffer.end());
                    return ErrorCode::OK;
                }
            }

            ErrorCode ec = ReqError(pkt);
            return ec == ErrorCode::OK ? ErrorCode::InvalidData : ec;
        }

        ErrorCode Device::GetLocation(std::wstring & location, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::Location, m_sn.type), timeout);

            return PktToLocation(pkt, location);
        }

        ErrorCode Device::GetLocation(std::function<void(const std::wstring &, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::Location, m_sn.type),
                ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) {
                std::wstring value;
                ErrorCode ec = PktToLocation(pkt, value);
                callback(value, ec);
            }),
                timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        ErrorCode Device::SetLocation(const std::wstring & location, size_t timeout)
        {
            CHECK_IMPL();

            // 计算转换为GB18030需要的缓冲区长度
            int length = WideCharToMultiByte(936, 0, location.c_str(), location.size(), NULL, 0, NULL, NULL);
            
            // 分配会用数据包缓冲区
            auto req = Packet::Create(impl->GetNextId(), Method::Write, Resource::Location, m_sn.type, nullptr, 2 + length + 2, 1);

            // 编码转换
            BOOL usedDefault = FALSE;
            CHAR defaultChar = ' ';
            WideCharToMultiByte(936, 0, location.c_str(), location.size(), req->GetObjects() + 2, length, &defaultChar, &usedDefault);

            unsigned short size = (unsigned short)(length + 2);
            memcpy(req->GetObjects(), &size, 2);
            memset(req->GetObjects() + size, 0, 2);

            auto pkt = impl->SendRequest(req, timeout);
            return ReqError(pkt);
        }

        ErrorCode Device::SetLocation(const std::wstring & location, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            // 计算转换为GB18030需要的缓冲区长度
            int length = WideCharToMultiByte(936, 0, location.c_str(), location.size(), NULL, 0, NULL, NULL);

            // 分配会用数据包缓冲区
            auto req = Packet::Create(impl->GetNextId(), Method::Write, Resource::Location, m_sn.type, nullptr, 2 + length + 2, 1);

            // 编码转换
            BOOL usedDefault = FALSE;
            CHAR defaultChar = ' ';
            WideCharToMultiByte(936, 0, location.c_str(), location.size(), req->GetObjects() + 2, length, &defaultChar, &usedDefault);

            unsigned short size = (unsigned short)(length + 2);
            memcpy(req->GetObjects(), &size, 2);
            memset(req->GetObjects() + size, 0, 2);

            return impl->SendRequest(
                req,
                ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }),
                timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        ErrorCode Device::ReadSingleBufferNoParam(Resource resource, char * ptr, size_t sz, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, resource, m_sn.type), timeout);

            if (pkt && pkt->GetHead()->method == Method::Reply)
            {
                if (pkt->GetHead()->appBytes != sz)
                {
                    return ErrorCode::InvalidData;
                }
                else
                {
                    memcpy(ptr, pkt->GetObjects(), sz);
                    return ErrorCode::OK;
                }
            }

            ErrorCode ec = ReqError(pkt);
            return ec == ErrorCode::OK ? ErrorCode::InvalidData : ec;    
        }
        ErrorCode Device::ReadSingleBufferNoParam(Resource resource, size_t sz, std::function<void(const char *, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Read, resource, m_sn.type), 
                    ReqCallback(callback, [callback, sz](std::shared_ptr<Packet> pkt) { 
                        if (pkt && pkt->GetHead()->method == Method::Reply)
                        {
                            if (pkt->GetHead()->appBytes != sz)
                            {
                                callback(nullptr, ErrorCode::InvalidData);
                            }
                            else
                            {
                                callback(pkt->GetObjects(), ErrorCode::InvalidData);
                            }
                        }
                        else
                        {
                            ErrorCode ec = ReqError(pkt);    
                            callback(nullptr, ec == ErrorCode::OK ? ErrorCode::InvalidData : ec);
                        }
                    }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;                 
        }

        ErrorCode Device::WriteSingleBufferNoParam(Resource resource, const char * ptr, size_t sz, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Write, resource, m_sn.type, ptr, sz, 1), 
                timeout);

            return ReqError(pkt);
        }
        ErrorCode Device::WriteSingleBufferNoParam(Resource resource, const char * ptr, size_t sz, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Write, resource, m_sn.type, ptr, sz, 1), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;        
        }

        static inline ErrorCode PktToOperationRecord(std::shared_ptr<Packet> pkt, std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> & records, bool &hasMore)
        {
            //GB 18030-2005编码
            if (pkt && pkt->GetHead()->method == Method::Reply)
            {
                if (pkt->GetHead()->appBytes < 1)
                {
                    return ErrorCode::InvalidData;
                }
                else
                {
                    hasMore = (*pkt->GetObjects() == 1);

                    size_t sz = 0;
                    const char * ptr = pkt->GetObjects(sz) + 1, * end = ptr + sz - 1;
                    
                    std::array<unsigned char, 6> time;
                    unsigned char len = 0;

                    while (ptr + 7 <= end)
                    {
                        std::copy(ptr, ptr + 6, time.begin()); // 前6个字节为时间信息
                        len = ptr[6];

                        ptr += 7;
                        if (ptr + len >= end)
                        {
                            break;
                        }

                        records.push_back({time, std::vector<char>(ptr, ptr + len)});
                    }

                    return ErrorCode::OK;
                }
            }

            ErrorCode ec = ReqError(pkt);
            return ec == ErrorCode::OK ? ErrorCode::InvalidData : ec;  
        }
        ErrorCode Device::GetOperationRecord(const std::array<unsigned char, 6> & timeBegin, const std::array<unsigned char, 6> & timeEnd, std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> & records, bool & hasMore, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::OperationRecord, m_sn.type, std::vector<std::array<unsigned char, 6>>{timeBegin, timeEnd}), timeout);

            return PktToOperationRecord(pkt, records, hasMore);   
        }
        ErrorCode Device::GetOperationRecord(const std::array<unsigned char, 6> & timeBegin, const std::array<unsigned char, 6> & timeEnd, std::function<void(const std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> &, bool, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Read, Resource::OperationRecord, m_sn.type, std::vector<std::array<unsigned char, 6>>{timeBegin, timeEnd}), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { 
                        std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> records;
                        bool hasMore = false;
                        ErrorCode ec = PktToOperationRecord(pkt, records, hasMore); 
                        callback(records, hasMore, ec); 
                    }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;  
        }

        ErrorCode Device::GetOperationRecord(std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> & records, bool & hasMore, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::OperationRecord, m_sn.type), timeout);

            return PktToOperationRecord(pkt, records, hasMore);        
        }
        ErrorCode Device::GetOperationRecord(std::function<void(const std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> &, bool, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Read, Resource::OperationRecord, m_sn.type), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { 
                        std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> records;
                        bool hasMore = false;
                        ErrorCode ec = PktToOperationRecord(pkt, records, hasMore); 
                        callback(records, hasMore, ec); 
                    }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;          
        }

        ErrorCode Device::CtrlRelay(unsigned char idx, unsigned char isClose, unsigned char seconds, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::CtrlRelay, m_sn.type, std::array<unsigned char, 3>{{idx, isClose, seconds}}), timeout);

            return ReqError(pkt);
        }
        ErrorCode Device::CtrlRelay(unsigned char idx, unsigned char isClose, unsigned char seconds, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::CtrlRelay, m_sn.type, std::array<unsigned char, 3>{{idx, isClose, seconds}}), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        ErrorCode Device::CtrlRelay(std::vector<std::array<unsigned char, 3>> ctrls, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::CtrlRelay, m_sn.type, ctrls), timeout);

            return ReqError(pkt);
        }
        ErrorCode Device::CtrlRelay(std::vector<std::array<unsigned char, 3>> ctrls, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::CtrlRelay, m_sn.type, ctrls), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        ErrorCode Device::Disable(bool isDisable, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::Disable, m_sn.type, (unsigned char)(isDisable ? 1 : 0)), timeout);

            return ReqError(pkt);
        }
        ErrorCode Device::Disable(bool isDisable, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::Disable, m_sn.type, (unsigned char)(isDisable ? 1 : 0)), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        ErrorCode Device::SetDefaultParameters(size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::SetDefaultParameters, m_sn.type), timeout);

            return ReqError(pkt);
        }
        ErrorCode Device::SetDefaultParameters(std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::SetDefaultParameters, m_sn.type), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        ErrorCode Device::Reset(size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::Reset, m_sn.type), timeout);

            return ReqError(pkt);
        }
        ErrorCode Device::Reset(std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::Reset, m_sn.type), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        ErrorCode Device::Reboot(size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::Reboot, m_sn.type), timeout);

            return ReqError(pkt);
        }
        ErrorCode Device::Reboot(std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::Reboot, m_sn.type), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }

        void Device::UploadDispatch(std::shared_ptr<Packet> pkt)
        {
            switch (pkt->GetHead()->resource)
            {
            case Resource::Location:
                if (pkt->GetHead()->method == Method::Upload)
                {
                    std::wstring location;

                    if (PktToLocation(pkt, location) == ErrorCode::OK)
                    {
                        OnUploadLocation(location);
                    }
                }
                break;
            case Resource::AnalogValue:
                if (pkt->GetHead()->method == Method::Upload)
                {
                    unsigned char type;
                    unsigned int analogValue;

                    if (PktToAnalog(pkt, type, analogValue) == ErrorCode::OK)
                    {
                        OnUploadAnalog(type, analogValue);
                    }
                }
                break;

            default:
                break;
            }
        }
    };
};