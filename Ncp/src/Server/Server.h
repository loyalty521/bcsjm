////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file Server.h
/// @brief Ncp协议服务端类定义文件
///
/// 定义Ncp协议服务端类
///
/// @version 1.0
/// @author  李守红
/// @date    2013年12月1日
///
///
/// 修订说明：最初版本
///
////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _NCP_SERVER_SERVER_H
#define _NCP_SERVER_SERVER_H

#include "../Ncp.h"
#include "Device.h"

namespace Ncp
{
    class Packet;

    namespace Server
    {
        /// NCP服务器对象
        class NCP_API Server
        {
            friend class ServerImpl;

        public:
            /**
             * @brief 构造函数
             * @param [in] addr               本地监听地址
             * @param [in] timeoutOnce        单次重发超时（毫秒）
             * @param [in] maxResendTimes     最大重发次数
             * @param [in] activeTimeoutFirst 初次激活超时时间（毫秒）
             * @param [in] activeTimeoutNext  后续激活超时时间（毫秒）
             * @param [in] maxActivateTimes   最大激活次数
             */
            Server(const Address & addr = { 0, 1111 }, 
                   size_t timeoutOnce = 10000, 
                   size_t maxResendTimes = 3, 
                   size_t activeTimeoutFirst = 30000, 
                   size_t activeTimeoutNext = 10000, 
                   size_t maxActivateTimes = 3);

            /// 析构函数
            virtual ~Server();

            /**
             * @brief 启动服务
             * @return 启动成功返回true，否则返回false
             */
            bool Start();

            /**
             * @brief 停止服务
             */
            void Stop();

            /**
             * @brief 设备注册成功处理函数
             * @param [in] sn   设备序列号
             * @param [in] addr 设备网络地址
             * @return 返回设备指针
             * @note 子类应实现此接口（根据序列号和网络地址构建设备对象）
             */
            virtual Device * OnAccept(const SerialNumber & sn, const Address & addr) { return nullptr; };

            /**
             * @brief 设备注册失败处理函数
             * @param [in] sn         设备序列号
             * @param [in] ncpVersion 对方NCP版本
             * @param [in] addr       对方网络地址
             * @param [in] err        错误码
             * @note 子类可实现此接口以便展示错误信息
             */
            virtual void OnAcceptBadRegister(const SerialNumber & sn, const Version & ncpVersion, const Ncp::Address & addr, ErrorCode err) {};

        private:
            /// 禁止copy构造
            Server(const Server&) = delete;
            /// 禁止copy赋值
            Server & operator = (const Server&) = delete;

        private:
            /// 内部实现指针
            std::shared_ptr<ServerImpl> *    m_impl = nullptr;
        };
    };
};

#include "Devices/HydrantDecoder.h"
#include "Devices/JettingHydrantDecoder.h"
#include "Devices/JettingHydrantDomainCtrlBox.h"
#include "Devices/VideoFlameFireDetector.h"
#include "Devices/VideoFlameFireDetectorOD.h"
#include "Devices/VideoFlameFireDetectorEx.h"
#include "Devices/VideoSFCompositeFireDetector.h"
#include "Devices/VideoSFCompositeFireDetectorOD.h"
#include "Devices/VideoSFCompositeFireDetectorEx.h"
#include "Devices/LinearBeamSmokeFireDetector.h"
#include "Devices/LinearBeamSmokeFireDetectorEx.h"
#include "Devices/VisualLinearBeamSmokeFireDetector.h"
#include "Devices/VisualLinearBeamSmokeFireDetectorEx.h"

#endif