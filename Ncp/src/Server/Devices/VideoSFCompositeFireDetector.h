#pragma once
#ifndef _NCP_SERVER_DEVICES_VIDEOSFCOMPOSITEFIREDETECTOR_H
#define _NCP_SERVER_DEVICES_VIDEOSFCOMPOSITEFIREDETECTOR_H


#include "../../Ncp.h"
#include "../Device.h"
#include "VideoFireDetector.h"

namespace Ncp
{
    namespace Server
    {
        /// 图像型烟火复合火灾探测器
        class NCP_API VideoSFCompositeFireDetector : public virtual VideoFireDetector
        {
        public:
            /**
             * @brief 获取探测器参数（同步）
             * @param [out] params  探测器参数
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(VideoSFCompositeFireDetectorParameters & params, size_t timeout = 0)
            {
                return Device::GetParameters<VideoSFCompositeFireDetectorParameters>(params, timeout);
            }
            /**
             * @brief 获取探测器参数（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(std::function<void(const VideoSFCompositeFireDetectorParameters &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetParameters<VideoSFCompositeFireDetectorParameters>(callback, timeout);
            }
            
            /**
             * @brief 设置探测器参数（同步）
             * @param [in] params  探测器参数
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetParameters(const VideoSFCompositeFireDetectorParameters & params, size_t timeout = 0)
            {
                return Device::SetParameters<VideoSFCompositeFireDetectorParameters>(params, timeout);
            }
            /**
             * @brief 设置探测器参数（异步）
             * @param [in] params   探测器参数
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetParameters(const VideoSFCompositeFireDetectorParameters & params, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::SetParameters<VideoSFCompositeFireDetectorParameters>(params, callback, timeout);
            }

        public:
            /// 获取设备类型
            virtual const DeviceType * GetDeviceType() const;
        };
    };
};

#endif