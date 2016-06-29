
#include "LinearBeamSmokeFireDetectorEx.h"
#include "../DeviceImpl.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::LinearBeamSmokeFireDetectorEx);

        const DeviceType * LinearBeamSmokeFireDetectorEx::GetDeviceType()  const
        {
            return &__deviceType;
        } 
    };
};
