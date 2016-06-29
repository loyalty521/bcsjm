#pragma once
#ifndef _NCP_SERVER_DEIVCES_VIDEOFLAMEFIREDETECTOR_H
#define _NCP_SERVER_DEIVCES_VIDEOFLAMEFIREDETECTOR_H

#include "../../Ncp.h"
#include "../Device.h"
#include "VideoFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        /// 图像型火焰火灾探测器
        class NCP_API VideoFlameFireDetector : public virtual VideoFireDetector
        {
        public:
            /// 获取设备类型
            virtual const DeviceType * GetDeviceType() const;
        };
    };
};

#endif