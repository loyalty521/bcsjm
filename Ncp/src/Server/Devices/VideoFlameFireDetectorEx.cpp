
#include "VideoFlameFireDetectorEx.h"
#include "../DeviceImpl.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::VideoFlameFireDetectorEx);

        const DeviceType * VideoFlameFireDetectorEx::GetDeviceType()  const
        {
            return &__deviceType;
        }
    };
};
