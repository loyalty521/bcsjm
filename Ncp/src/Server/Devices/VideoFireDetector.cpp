
#include "../DeviceImpl.h"
#include "VideoFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        static inline ErrorCode PktToAlarmPicture(std::shared_ptr<Packet> pkt, std::vector<std::pair<unsigned char, std::vector<char>>> & pictures) //unsigned char & format, std::vector<char> & buffer)
        {
			auto head = pkt->GetHead();

            if (pkt && (head->method == Method::Reply || head->method == Method::Upload))
            {
                if (pkt->GetHead()->appBytes <= 5)
                {
                    return ErrorCode::InvalidData;
                }
                else
                {
                    size_t sz = 0, len = 0;
                    unsigned char format;
                    const char * ptr = pkt->GetObjects(sz);

                    while(sz > 5 + len)
                    {
                        sz -= 5 + len;

                        memcpy(&len, ptr, 4);
                        //len = *(_UNALIGNED_<uint32_t> *)ptr;    
                        ptr += 4;
                        format = *(unsigned char *)ptr++;

                        pictures.emplace_back(format, std::vector<char>(ptr, ptr + len));
                        ptr += len;
                    }

                    return ErrorCode::OK;
                }
            }

            ErrorCode ec = ReqError(pkt);
            return ec == ErrorCode::OK ? ErrorCode::InvalidData : ec;                
        }
        ErrorCode VideoFireDetector::GetAlarmPicture(std::vector<std::pair<unsigned char, std::vector<char>>> & pictures, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::AlarmPicture, m_sn.type), timeout);

            return PktToAlarmPicture(pkt, pictures);            
        }
        ErrorCode VideoFireDetector::GetAlarmPicture(std::function<void(const std::vector<std::pair<unsigned char, std::vector<char>>> &, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Read, Resource::SystemTime, m_sn.type), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { 
                        std::vector<std::pair<unsigned char, std::vector<char>>> pictures;

                        ErrorCode ec = PktToAlarmPicture(pkt, pictures); 
                        callback(pictures, ec); 
                    }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;             
        }

        static inline ErrorCode PktToAlgList(std::shared_ptr<Packet> pkt, std::vector<VideoFireDetectorAlgItemRead> & algs)
        {
            if (pkt && pkt->GetHead()->method == Method::Reply)
            {
                size_t sz = 0;
                const unsigned char * ptr = (const unsigned char *)pkt->GetObjects(sz), * end = ptr + sz;
                while (ptr < end)
                {
                    VideoFireDetectorAlgItemRead alg;

                    if (ptr + 13 >= end)
                    {
                        break;
                    }
                    alg.name.assign((const char *)ptr, 4);
                    memcpy(&alg.version, ptr + 4, sizeof(Version));// = *(_UNALIGNED_<Version> *)(ptr + 4);
                    alg.videoChannel = ptr[8];
                    alg.sensibility = ptr[9];
                    alg.sensibilityUBound = ptr[10];
                    alg.sensibilityDefault = ptr[11];
                    sz = ptr[12];

                    ptr += 13;
                    if (ptr + sz > end)
                    {
                        break;
                    }

                    alg.description.assign((const char *)ptr, sz);

                    algs.push_back(alg);
                }
                return ErrorCode::OK;
            }

            ErrorCode ec = ReqError(pkt);
            return ec == ErrorCode::OK ? ErrorCode::InvalidData : ec;             
        }
        ErrorCode VideoFireDetector::GetAlgList(std::vector<VideoFireDetectorAlgItemRead> & algs, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::AlgorithmList, m_sn.type), timeout);

            return PktToAlgList(pkt, algs);          
        }
        ErrorCode VideoFireDetector::GetAlgList(std::function<void(const std::vector<VideoFireDetectorAlgItemRead> &, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Read, Resource::AlgorithmList, m_sn.type), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { 
                        std::vector<VideoFireDetectorAlgItemRead> algs;

                        ErrorCode ec = PktToAlgList(pkt, algs); 
                        callback(algs, ec); 
                    }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;           
        }

        ErrorCode VideoFireDetector::SetAlgList(const std::vector<VideoFireDetectorAlgItemWrite> & algs, size_t timeout)
        {
            CHECK_IMPL();

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Write, Resource::AlgorithmList, m_sn.type, algs), 
                timeout);

            return ReqError(pkt);            
        }
        ErrorCode VideoFireDetector::SetAlgList(const std::vector<VideoFireDetectorAlgItemWrite> & algs, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Write, Resource::AlgorithmList, m_sn.type, algs), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;            
        }

#pragma pack(push)
#pragma pack(1)
        struct AlgInfo
        {
            char name[4];
            unsigned char sensibility;
        };
#pragma pack(pop)
        static inline ErrorCode PktToAlgParameters(std::shared_ptr<Packet> pkt, std::string & paramsJson)
        {
            if (pkt && pkt->GetHead()->method == Method::Reply)
            {
                if (pkt->GetHead()->appBytes < 7)
                {
                    return ErrorCode::InvalidData;
                }
                else
                {
                    size_t sz = 0;
                    const unsigned char * ptr = (const unsigned char *)pkt->GetObjects(sz), * end = ptr + sz;
                  
                    //name.assign((const char *)ptr, 4);
                    //sensibility = ptr[4];

                    sz = ptr[5] | (ptr[6] << 8);
                    //memcpy(&sz = *(_UNALIGNED_<unsigned short> *)(ptr + 5);

                    ptr += 5;

                    if (ptr + sz > end)
                    {
                        return ErrorCode::InvalidData;
                    }

                    paramsJson.assign((const char *)ptr, sz);
                }
                return ErrorCode::OK;
            }

            ErrorCode ec = ReqError(pkt);
            return ec == ErrorCode::OK ? ErrorCode::InvalidData : ec;                
        }
        
        ErrorCode VideoFireDetector::GetAlgParameters(const std::string & name, unsigned char sensibility, std::string & paramsJson, size_t timeout)
        {
            CHECK_IMPL();

            assert(name.size() == 4);

            AlgInfo alg = { { name.at(0), name.at(1), name.at(2), name.at(3) }, sensibility };

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Read, Resource::AlgorithmParameters, m_sn.type, alg), timeout);

            return PktToAlgParameters(pkt, paramsJson);
        }
        ErrorCode VideoFireDetector::GetAlgParameters(const std::string & name, unsigned char sensibility, std::function<void(const std::string &, ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            assert(name.size() == 4);

            AlgInfo alg = { { name.at(0), name.at(1), name.at(2), name.at(3) }, sensibility };

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Read, Resource::AlgorithmParameters, m_sn.type, alg), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { 
                        std::vector<VideoFireDetectorAlgItemRead> algs;
                        std::string paramsJson;
                        
                        ErrorCode ec = PktToAlgParameters(pkt, paramsJson); 
                        callback(paramsJson, ec); 
                    }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;             
        }

        ErrorCode VideoFireDetector::SetAlgParameters(const std::string & name, unsigned char sensibility, const std::string & paramsJson, size_t timeout)
        {
            CHECK_IMPL();

            assert(name.size() == 4);
            assert(paramsJson.size() < 65535);

            std::vector<unsigned char> buffer(4 + 1 + 2 + paramsJson.size());

            std::copy(name.begin(), name.end(), buffer.begin());
            buffer[4] = sensibility;
            buffer[5] = paramsJson.size() & 0xff;
            buffer[6] = (paramsJson.size() >> 8) & 0xff;
            //*(_UNALIGNED_<unsigned short> *)(&buffer[5]) = paramsJson.size() & 0xffff;

            std::copy(paramsJson.begin(), paramsJson.end(), buffer.begin() + 7);

            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Write, Resource::AlgorithmList, m_sn.type, buffer, 1), 
                timeout);

            return ReqError(pkt); 
        }
        ErrorCode VideoFireDetector::SetAlgParameters(const std::string & name, unsigned char sensibility, const std::string & paramsJson, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            assert(name.size() == 4);
            assert(paramsJson.size() < 65535);

            std::vector<unsigned char> buffer(4 + 1 + 2 + paramsJson.size());

            std::copy(name.begin(), name.end(), buffer.begin());
            buffer[4] = sensibility;
            buffer[5] = paramsJson.size() & 0xff;
            buffer[6] = (paramsJson.size() >> 8) & 0xff;
            //*(_UNALIGNED_<unsigned short> *)(&buffer[5]) = paramsJson.size() & 0xffff;
            std::copy(paramsJson.begin(), paramsJson.end(), buffer.begin() + 7);

            return impl->SendRequest(
                    Packet::Create(impl->GetNextId(), Method::Write, Resource::AlgorithmList, m_sn.type, buffer, 1), 
                    ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }), 
                    timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;            
        }

        void VideoFireDetector::OnUpload(std::shared_ptr<Packet> pkt)
        {
            std::shared_ptr<DeviceImpl> impl = QueryImpl();
            ErrorCode ec = ErrorCode::OK;
			auto head = pkt->GetHead();

			if (pkt && head->method == Method::Upload || head->method == Method::Reply)
            {
				switch (head->resource)
                {
                    case Resource::Status:
						if (head->appBytes == sizeof(VideoFireDetectorStatus))
                        {
                            if (!pkt->IsResponse() && impl && impl->IsConnected())
                            {
                                impl->SendResponse(Packet::Create(pkt));
                            }

                            VideoFireDetectorStatus status;
                            memcpy(&status, pkt->GetObjects(), sizeof(status));
                            OnUploadStatus(status);
                        }
                        else
                        {
                            if (impl && impl->IsConnected())
                            {
                                ec = ErrorCode::InvalidObjectData;
                            }
                        }
                        break;

                    case Resource::AlarmPicture:
                    {
                        std::vector<std::pair<unsigned char, std::vector<char>>> pictures;

                        if (PktToAlarmPicture(pkt, pictures) == ErrorCode::OK)
                        {
                            if (impl && impl->IsConnected())
                            {
                                impl->SendResponse(Packet::Create(pkt));
                            }

                            OnUploadAlarmPicture(pictures);
                        }
                        else
                        {
                            ec = ErrorCode::InvalidObjectData;
                        }
                    }
                        break;
                    default:
                        Device::OnUpload(pkt);
                        return;
                }
            }

            if (impl && impl->IsConnected())
            {
                impl->SendResponse(Packet::Create(pkt, ec));
            }
        }
    };
};
