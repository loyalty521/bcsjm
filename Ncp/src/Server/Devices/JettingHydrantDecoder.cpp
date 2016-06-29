
#include "JettingHydrantDecoder.h"
#include "../DeviceImpl.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::JettingHydrantDecoder);

        const DeviceType * JettingHydrantDecoder::GetDeviceType()  const
        {
            return &__deviceType;
        }
    };
};
