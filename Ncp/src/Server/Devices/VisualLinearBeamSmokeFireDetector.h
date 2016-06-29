#pragma once
#ifndef _NCP_SERVER_DEVICES_VISUALLINEARBEAMSMOKEFIREDETECTOR_H
#define _NCP_SERVER_DEVICES_VISUALLINEARBEAMSMOKEFIREDETECTOR_H

#include "../../Ncp.h"
#include "../Device.h"
#include "LinearBeamSmokeFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        /// 可视化线型光束感烟火灾探测器
        class NCP_API VisualLinearBeamSmokeFireDetector : public LinearBeamSmokeFireDetector
        {
        public:
            /// 获取设备类型
            virtual const DeviceType * GetDeviceType() const;
        };
    };
};

#endif