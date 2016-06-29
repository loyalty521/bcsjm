#pragma once
#ifndef _NCP_SERVER_DEVICES_HYDRANTDECODER_H
#define _NCP_SERVER_DEVICES_HYDRANTDECODER_H

#include <limits>

#include "../../Ncp.h"
#include "../Device.h"

#ifdef max
#undef max
#endif

namespace Ncp
{
    namespace Server
    {
        // 自动消防炮解码器
        class NCP_API HydrantDecoder: public Device
        {
        private:
            enum class CtrlCode
            {
                Stop = 0,           ///< 停止炮管动作和复合动作
                BarrelAction = 1,   ///< 炮管动作
                PeripheralCtrl = 2, ///< 附属设备控制
                ComplexAction = 3,  ///< 复合动作
                ModeSwitch = 4      ///< 工作模式切换
            };
        public:
            /**
             * @brief 获取设备状态（同步）
             * @param [out] status  设备状态
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetStatus(HydrantDecoderStatus & status, size_t timeout = 0)
            {
                return Device::GetStatus<HydrantDecoderStatus>(status, timeout);
            }
            /**
             * @brief 获取设备状态（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetStatus(std::function<void(const HydrantDecoderStatus &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetStatus<HydrantDecoderStatus>(callback, timeout);
            }

           /**
             * @brief 获取设备参数（同步）
             * @param [out] params  设备参数
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(HydrantDecoderParameters & params, size_t timeout = 0)
            {
                return Device::GetParameters<HydrantDecoderParameters>(params, timeout);
            }
            /**
             * @brief 获取设备参数（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(std::function<void(const HydrantDecoderParameters &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetParameters<HydrantDecoderParameters>(callback, timeout);
            }

           /**
             * @brief 设置设备参数（同步）
             * @param [in] params  设备参数
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetParameters(const HydrantDecoderParameters & params, size_t timeout = 0)
            {
                return Device::SetParameters<HydrantDecoderParameters>(params, timeout);
            }
            /**
             * @brief 设置设备参数（异步）
             * @param [in] params   设备参数
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetParameters(const HydrantDecoderParameters & params, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::SetParameters<HydrantDecoderParameters>(params, callback, timeout);
            }

            /**
             * @brief 控制炮管向上运动
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveUp(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x01 }, 
                    timeout);
            }
            /**
             * @brief 控制炮管向上运动
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveUp(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x01 },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向上运动指定角度
             * @param [in] angle   需要运动的角度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveUp(uint16_t angle, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x01, 0x02, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) }, 
                    timeout);
            }

            /**
             * @brief 控制炮管向上运动指定角度
             * @param [in] angle    需要运动的角度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveUp(uint16_t angle, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x01, 0x02, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) }, 
                    timeout);
            }

            /**
             * @brief 控制炮管向上运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveUp(std::chrono::duration<Rep_, Period_> duration, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x01, 0x03, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) }, 
                    timeout);
            }

            /**
             * @brief 控制炮管向上运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveUp(std::chrono::duration<Rep_, Period_> duration, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x01, 0x03, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) }, 
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向下运动
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveDown(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x02 }, 
                    timeout);
            }
            /**
             * @brief 控制炮管向下运动
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveDown(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x02 },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向下运动指定角度
             * @param [in] angle   需要运动的角度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveDown(uint16_t angle, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x02, 0x02, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) }, 
                    timeout);
            }

            /**
             * @brief 控制炮管向下运动指定角度
             * @param [in] angle    需要运动的角度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveDown(uint16_t angle, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x02, 0x02, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) }, 
                    timeout);
            }

            /**
             * @brief 控制炮管向下运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveDown(std::chrono::duration<Rep_, Period_> duration, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x02, 0x03, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) }, 
                    timeout);
            }

            /**
             * @brief 控制炮管向下运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveDown(std::chrono::duration<Rep_, Period_> duration, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x02, 0x03, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) }, 
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向左运动
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveLeft(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x04 }, 
                    timeout);
            }
            /**
             * @brief 控制炮管向左运动
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveLeft(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x04 },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向左运动指定角度
             * @param [in] angle   需要运动的角度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveLeft(uint16_t angle, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x04, 0x08, 0, 0, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) }, 
                    timeout);
            }

            /**
             * @brief 控制炮管向左运动指定角度
             * @param [in] angle    需要运动的角度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveLeft(uint16_t angle, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x04, 0x08, 0, 0, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向左运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveLeft(std::chrono::duration<Rep_, Period_> duration, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x04, 0x0c, 0, 0, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向左运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveLeft(std::chrono::duration<Rep_, Period_> duration, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x04, 0x0c, 0, 0, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向右运动
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveRight(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x08 }, 
                    timeout);
            }
            /**
             * @brief 控制炮管向右运动
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveRight(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x08 },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向右运动指定角度
             * @param [in] angle   需要运动的角度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveRight(uint16_t angle, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x08, 0x08, 0, 0, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向右运动指定角度
             * @param [in] angle    需要运动的角度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveRight(uint16_t angle, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x08, 0x08, 0, 0, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向右运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveRight(std::chrono::duration<Rep_, Period_> duration, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x08, 0x0c, 0, 0, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向右运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveRight(std::chrono::duration<Rep_, Period_> duration, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x08, 0x0c, 0, 0, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向柱状运动
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MovePole(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x10 }, 
                    timeout);
            }
            /**
             * @brief 控制炮管向柱状运动
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MovePole(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x10 },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向柱状运动指定刻度
             * @param [in] ticks   需要运动的刻度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MovePole(uint16_t ticks, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x10, 0x20, 0, 0, 0, 0, (uint8_t)(ticks >> 8), (uint8_t)(ticks & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向柱状运动指定刻度
             * @param [in] ticks    需要运动的刻度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MovePole(uint16_t ticks, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x10, 0x20, 0, 0, 0, 0, (uint8_t)(ticks >> 8), (uint8_t)(ticks & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向柱状运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MovePole(std::chrono::duration<Rep_, Period_> duration, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x10, 0x30, 0, 0, 0, 0, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向柱状运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MovePole(std::chrono::duration<Rep_, Period_> duration, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x10, 0x30, 0, 0, 0, 0, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向雾状运动
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveFog(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x20 }, 
                    timeout);
            }
            /**
             * @brief 控制炮管向雾状运动
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveFog(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x20 },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管向雾状运动指定刻度
             * @param [in] ticks   需要运动的刻度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveFog(uint16_t ticks, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x20, 0x20, 0, 0, 0, 0, (uint8_t)(ticks >> 8), (uint8_t)(ticks & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向雾状运动指定刻度
             * @param [in] ticks    需要运动的刻度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveFog(uint16_t ticks, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x20, 0x20, 0, 0, 0, 0, (uint8_t)(ticks >> 8), (uint8_t)(ticks & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向雾状运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveFog(std::chrono::duration<Rep_, Period_> duration, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x20, 0x30, 0, 0, 0, 0, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管向雾状运动指定时间
             * @param [in] duration 需要运动的时间，[0, 65535]毫秒
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            template <typename Rep_, typename Period_>
            ErrorCode MoveFog(std::chrono::duration<Rep_, Period_> duration, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                auto c = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                assert(c >= 0 && c < std::numeric_limits<uint16_t>::max());
                uint16_t ms = (uint16_t)c;

                return HydrantCtrl(CtrlCode::BarrelAction, 
                    { 0x20, 0x30, 0, 0, 0, 0, (uint8_t)(c >> 8), (uint8_t)(c & 0xff) },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管水平运动到指定角度
             * @param [in] angle   需要运动到的角度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveToH(uint16_t angle, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x04, 0x04, 0, 0, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管水平运动到指定角度
             * @param [in] angle    需要运动到的角度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveToH(uint16_t angle, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x04, 0x04, 0, 0, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管垂直运动到指定角度
             * @param [in] angle   需要运动到的角度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveToV(uint16_t angle, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x01, 0x01, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管垂直运动到指定角度
             * @param [in] angle    需要运动到的角度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveToV(uint16_t angle, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x01, 0x01, (uint8_t)(angle >> 8), (uint8_t)(angle & 0xff) },
                    callback, timeout);
            }

            /**
             * @brief 控制炮管运动到指定角度
             * @param [in] angleH   需要水平运动到的角度
             * @param [in] angleV   需要垂直运动到的角度
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveTo(uint16_t angleH, uint16_t angleV, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x05, 0x05, (uint8_t)(angleV >> 8), (uint8_t)(angleV & 0xff), (uint8_t)(angleH >> 8), (uint8_t)(angleH & 0xff) },
                    timeout);
            }

            /**
             * @brief 控制炮管运动到指定角度
             * @param [in] angleH   需要水平运动到的角度
             * @param [in] angleV   需要垂直运动到的角度
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode MoveTo(uint16_t angleH, uint16_t angleV, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::BarrelAction,
                    { 0x05, 0x05, (uint8_t)(angleV >> 8), (uint8_t)(angleV & 0xff), (uint8_t)(angleH >> 8), (uint8_t)(angleH & 0xff) },
                    callback, timeout);
            }

            /**
             * @brief 控制升降装置折叠（收缩/上升）
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode LiftFold(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::PeripheralCtrl,
                    { 0x20 },
                    timeout);
            }

            /**
             * @brief 控制升降装置折叠（收缩/上升）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode LiftFold(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::PeripheralCtrl,
                    { 0x20 },
                    callback, timeout);
            }

            /**
             * @brief 控制升降装置展开（下降）
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode LiftUnfold(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::PeripheralCtrl,
                    { 0x10 },
                    timeout);
            }

            /**
             * @brief 控制升降装置展开（下降）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode LiftUnfold(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::PeripheralCtrl,
                    { 0x10 },
                    callback, timeout);
            }

            /**
             * @brief 开阀
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode OpenValve(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::PeripheralCtrl,
                    { 0x01 },
                    timeout);
            }

            /**
             * @brief 开阀
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode OpenValve(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::PeripheralCtrl,
                    { 0x01 },
                    callback, timeout);
            }

            /**
             * @brief 关阀
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode CloseValve(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::PeripheralCtrl,
                    { 0x02 },
                    timeout);
            }

            /**
             * @brief 关阀
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode CloseValve(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::PeripheralCtrl,
                    { 0x02 },
                    callback, timeout);
            }

            /**
             * @brief 切换工作模式
             * @param [in] hydrantMode 工作模式，true=消防炮工作模式，false=探测器工作模式
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SwitchMode(bool hydrantMode, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::ModeSwitch,
                    { (uint8_t)(hydrantMode ? 0x00 : 0x01) },
                    timeout);
            }

            /**
             * @brief 切换工作模式
             * @param [in] hydrantMode 工作模式，true=消防炮工作模式，false=探测器工作模式
             * @param [in] callback 回调函数
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SwitchMode(bool hydrantMode, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::ModeSwitch,
                    { (uint8_t)(hydrantMode ? 0x00 : 0x01) },
                    callback, timeout);
            }

            /**
             * @brief 摆动
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Swing(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::ComplexAction,
                    { 0x01 },
                    timeout);
            }

            /**
             * @brief 摆动
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Swing(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::ComplexAction,
                    { 0x01 },
                    callback, timeout);
            }

            /**
             * @brief 扫描
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Scan(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::ComplexAction,
                    { 0x02 },
                    timeout);
            }

            /**
             * @brief 扫描
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Scan(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::ComplexAction,
                    { 0x02 },
                    callback, timeout);
            }

            /**
             * @brief 归位
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Return(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::ComplexAction,
                    { 0x04 },
                    timeout);
            }

            /**
             * @brief 归位
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Return(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::ComplexAction,
                    { 0x04 },
                    callback, timeout);
            }

            /**
             * @brief 停止
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Stop(size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::Stop,
                    {},
                    timeout);
            }

            /**
             * @brief 停止
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Stop(std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return HydrantCtrl(CtrlCode::Stop,
                    {},
                    callback, timeout);
            }

        private:
            /**
             * @brief 执行消防炮控制
             * @param [in] code    控制代码
             * @param [in] data    控制参数
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode HydrantCtrl(CtrlCode code, std::initializer_list<uint8_t> data, size_t timeout);

            /**
             * @brief 执行消防炮控制
             * @param [in] code     控制代码
             * @param [in] data     控制参数
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode HydrantCtrl(CtrlCode code, std::initializer_list<uint8_t> data, std::function<void(ErrorCode)> callback, size_t timeout);

        public:
            /// 获取设备类型
            const DeviceType * GetDeviceType() const;
        };
    };
};

#endif