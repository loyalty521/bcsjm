#pragma once
#ifndef _NCP_SERVER_DEVICES_LINEARBEAMSMOKEFIREDETECTOR_H
#define _NCP_SERVER_DEVICES_LINEARBEAMSMOKEFIREDETECTOR_H

#include "../../Ncp.h"
#include "../Device.h"
#include "VideoFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        /// 线型光束感烟火灾探测器
        class NCP_API LinearBeamSmokeFireDetector : public virtual VideoFireDetector
        {
        public:
            /// 获取设备类型
            virtual const DeviceType * GetDeviceType() const;
        };
    };
};

#endif