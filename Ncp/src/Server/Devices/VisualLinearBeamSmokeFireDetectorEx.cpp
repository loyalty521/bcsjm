
#include "VisualLinearBeamSmokeFireDetectorEx.h"
#include "../DeviceImpl.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::VisualLinearBeamSmokeFireDetectorEx);

        const DeviceType * VisualLinearBeamSmokeFireDetectorEx::GetDeviceType()  const
        {
            return &__deviceType;
        }         
    };
};
