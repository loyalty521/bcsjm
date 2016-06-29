#pragma once
#ifndef _NCP_SERVER_LINEARBEAMSMOKEFIREDETECTOREX_H
#define _NCP_SERVER_LINEARBEAMSMOKEFIREDETECTOREX_H

#include "../../Ncp.h"
#include "../Device.h"
#include "LinearBeamSmokeFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        /// 防爆型线型光束感烟火灾探测器
        class NCP_API LinearBeamSmokeFireDetectorEx : public LinearBeamSmokeFireDetector
        {
        public:
            /// 获取设备类型
            const DeviceType * GetDeviceType() const;
        };
    };
};

#endif