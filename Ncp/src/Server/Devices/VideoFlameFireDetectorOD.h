#pragma once
#ifndef _NCP_SERVER_VIDEOFLAMEFIREDETECTOROD_H
#define _NCP_SERVER_VIDEOFLAMEFIREDETECTOROD_H

#include "../../Ncp.h"
#include "../Device.h"
#include "VideoFlameFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        /// 室外型图像型火焰火灾探测器
        class NCP_API VideoFlameFireDetectorOD : public VideoFlameFireDetector
        {
        public:
            /// 获取设备类型
            const DeviceType * GetDeviceType() const;
        };
    };
};

#endif