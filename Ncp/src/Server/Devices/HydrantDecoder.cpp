
#include "HydrantDecoder.h"
#include "../DeviceImpl.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::HydrantDecoder);

        const DeviceType * HydrantDecoder::GetDeviceType() const
        {
            return &__deviceType;
        }

        ErrorCode HydrantDecoder::HydrantCtrl(CtrlCode code, std::initializer_list<uint8_t> data, size_t timeout)
        {
            CHECK_IMPL();

            assert(data.size() <= 8);

            uint8_t buffer[9] = { 0 };

            buffer[0] = (uint8_t)code;
            std::copy(data.begin(), data.end(), buffer + 1);
 
            std::shared_ptr<Packet> pkt = impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::CtrlHydrant, m_sn.type, buffer, 1), timeout);

            return ReqError(pkt);
        }

        ErrorCode HydrantDecoder::HydrantCtrl(CtrlCode code, std::initializer_list<uint8_t> data, std::function<void(ErrorCode)> callback, size_t timeout)
        {
            CHECK_IMPL();

            assert(data.size() <= 8);

            uint8_t buffer[9] = { 0 };

            buffer[0] = (uint8_t)code;
            std::copy(data.begin(), data.end(), buffer + 1);

            return impl->SendRequest(
                Packet::Create(impl->GetNextId(), Method::Ctrl, Resource::CtrlHydrant, m_sn.type, buffer, 1),
                ReqCallback(callback, [callback](std::shared_ptr<Packet> pkt) { callback(ReqError(pkt)); }),
                timeout) ?
                ErrorCode::OK : ErrorCode::DeviceDisconnected;
        }
    };
};
