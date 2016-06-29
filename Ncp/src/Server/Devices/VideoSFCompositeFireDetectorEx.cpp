

#include "VideoSFCompositeFireDetectorEx.h"
#include "../DeviceImpl.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::VideoSFCompositeFireDetectorEx);

        const DeviceType * VideoSFCompositeFireDetectorEx::GetDeviceType()  const
        {
            return &__deviceType;
        }
    };
};
