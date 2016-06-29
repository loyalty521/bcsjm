
#include "VideoFlameFireDetectorOD.h"
#include "../DeviceImpl.h"


namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::VideoFlameFireDetectorOD);

        const DeviceType * VideoFlameFireDetectorOD::GetDeviceType()  const
        {
            return &__deviceType;
        }
    };
};
