#pragma once
#ifndef _NCP_SERVER_DEVICES_VISUALLINEARBEAMSMOKEFIREDETECTOREX_H
#define _NCP_SERVER_DEVICES_VISUALLINEARBEAMSMOKEFIREDETECTOREX_H

#include "../../Ncp.h"
#include "../Device.h"
#include "VisualLinearBeamSmokeFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        /// 防爆型可视化线型光束感烟火灾探测器
        class NCP_API VisualLinearBeamSmokeFireDetectorEx : public VisualLinearBeamSmokeFireDetector
        {
        public:
            /// 获取设备类型
            const DeviceType * GetDeviceType() const;
        };
    };
};

#endif