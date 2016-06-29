
#include "../DeviceImpl.h"
#include "VideoFlameFireDetector.h"
namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::VideoFlameFireDetector);

        const DeviceType * VideoFlameFireDetector::GetDeviceType()  const
        {
            return &__deviceType;
        }
    };
};
