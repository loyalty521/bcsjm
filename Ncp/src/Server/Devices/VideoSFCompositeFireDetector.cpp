
#include "../DeviceImpl.h"
#include "VideoSFCompositeFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::VideoSFCompositeFireDetector);

        const DeviceType * VideoSFCompositeFireDetector::GetDeviceType()  const
        {
            return &__deviceType;
        }
    };
};
