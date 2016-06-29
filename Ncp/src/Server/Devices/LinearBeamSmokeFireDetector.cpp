
#include "../DeviceImpl.h"
#include "LinearBeamSmokeFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::LinearBeamSmokeFireDetector);

        const DeviceType * LinearBeamSmokeFireDetector::GetDeviceType()  const
        {
            return &__deviceType;
        }        
    };
};
