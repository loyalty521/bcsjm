
#include "../DeviceImpl.h"
#include "VideoSFCompositeFireDetectorOD.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::VideoSFCompositeFireDetectorOD);

        const DeviceType * VideoSFCompositeFireDetectorOD::GetDeviceType()  const
        {
            return &__deviceType;
        }
    };
};
