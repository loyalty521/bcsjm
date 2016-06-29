#pragma once
#ifndef _NCP_SERVER_DEVICES_JETTINGHYDRANTDECODER_H
#define _NCP_SERVER_DEVICES_JETTINGHYDRANTDECODER_H

#include "../../Ncp.h"
#include "../Device.h"

namespace Ncp
{
    namespace Server
    {
        /// 射流灭火装置解码器
        class NCP_API JettingHydrantDecoder: public Device
        {
        public:
            /**
             * @brief 获取设备状态（同步）
             * @param [out] status  设备状态
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetStatus(JettingHydrantDecoderStatus & status, size_t timeout = 0)
            {
                return Device::GetStatus<JettingHydrantDecoderStatus>(status, timeout);
            }
            /**
             * @brief 获取设备状态（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetStatus(std::function<void(const JettingHydrantDecoderStatus &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetStatus<JettingHydrantDecoderStatus>(callback, timeout);
            }

           /**
             * @brief 获取设备参数（同步）
             * @param [out] params  设备参数
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(JettingHydrantDecoderParameters & params, size_t timeout = 0)
            {
                return Device::GetParameters<JettingHydrantDecoderParameters>(params, timeout);
            }
            /**
             * @brief 获取设备参数（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(std::function<void(const JettingHydrantDecoderParameters &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetParameters<JettingHydrantDecoderParameters>(callback, timeout);
            }

        public:
            /// 获取设备类型
            const DeviceType * GetDeviceType() const; 
        };
    };
};

#endif