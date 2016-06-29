#pragma once
#ifndef _NCP_SERVER_VIDEOSFCOMPOSITEFIREDETECTOREX_H
#define _NCP_SERVER_VIDEOSFCOMPOSITEFIREDETECTOREX_H

#include "../../Ncp.h"
#include "../Device.h"
#include "VideoSFCompositeFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        /// 防爆型图像型烟火复合火灾探测器
        class NCP_API VideoSFCompositeFireDetectorEx : public VideoSFCompositeFireDetector
        {
        public:
            /// 获取设备类型
            const DeviceType * GetDeviceType() const;
        };
    };
};

#endif