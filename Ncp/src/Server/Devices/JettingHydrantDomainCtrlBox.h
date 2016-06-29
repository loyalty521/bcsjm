#pragma once
#ifndef _NCP_SERVER_DEVICES_JETTINGHYDRANTDOMAINCTRLBOX_H
#define _NCP_SERVER_DEVICES_JETTINGHYDRANTDOMAINCTRLBOX_H

#include "../../Ncp.h"
#include "../Device.h"

namespace Ncp
{
    namespace Server
    {
        /// 小炮现场控制箱
        class NCP_API JettingHydrantDomainCtrlBox: public Device
        {
        public:
           /**
             * @brief 获取设备状态（同步）
             * @param [out] status  设备状态
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetStatus(JettingHydrantDomainCtrlBoxStatus & status, size_t timeout = 0)
            {
                return Device::GetStatus<JettingHydrantDomainCtrlBoxStatus>(status, timeout);
            }
            /**
             * @brief 获取设备状态（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetStatus(std::function<void(const JettingHydrantDomainCtrlBoxStatus &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetStatus<JettingHydrantDomainCtrlBoxStatus>(callback, timeout);
            }

           /**
             * @brief 获取设备参数（同步）
             * @param [out] params  设备参数
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(JettingHydrantDomainCtrlBoxParameters & params, size_t timeout = 0)
            {
                return Device::GetParameters<JettingHydrantDomainCtrlBoxParameters>(params, timeout);
            }
            /**
             * @brief 获取设备参数（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(std::function<void(const JettingHydrantDomainCtrlBoxParameters &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetParameters<JettingHydrantDomainCtrlBoxParameters>(callback, timeout);
            }

        public:
            /// 获取设备类型
            const DeviceType * GetDeviceType() const;
        };
    };
};

#endif