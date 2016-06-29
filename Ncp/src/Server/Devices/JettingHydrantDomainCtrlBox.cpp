
#include "JettingHydrantDomainCtrlBox.h"
#include "../DeviceImpl.h"

namespace Ncp
{
    namespace Server
    {
        static DeviceType __deviceType = DeviceType::FromId(DeviceTypeId::JettingHydrantDomainCtrlBox);

        const DeviceType * JettingHydrantDomainCtrlBox::GetDeviceType() const
        {
            return &__deviceType;
        }
    };
};
