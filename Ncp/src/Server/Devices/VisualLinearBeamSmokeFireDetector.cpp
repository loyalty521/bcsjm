
#include "../DeviceImpl.h"
#include "VisualLinearBeamSmokeFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::VisualLinearBeamSmokeFireDetector);

        const DeviceType * VisualLinearBeamSmokeFireDetector::GetDeviceType()  const
        {
            return &__deviceType;
        }         
    };
};
