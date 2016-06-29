////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file Device.h
/// @brief Ncp协议服务端设备基类定义文件
///
/// 定义Ncp协议服务端设备基类
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

#ifndef _NCP_SERVER_DEVICE_H
#define _NCP_SERVER_DEVICE_H

#include <array>
#include <vector>
#include <memory>
#include <functional>

namespace Ncp
{
    class Packet;

    namespace Server
    {
        class DeviceImpl;
        class ServerImpl;

        /// NCP服务端设备基类
        class NCP_API Device
        {
            friend class DeviceImpl;
            friend class ServerImpl;

        public:
            /// 生成缺省构造函数
            Device() = default;

            /// copy构造函数
            Device(const Device & device) :
                m_sn(device.m_sn),
                m_addr(device.m_addr),
                m_ncpVersion(device.m_ncpVersion),
                m_softwareVersion(device.m_softwareVersion),
                m_platformVersion(device.m_platformVersion)
            {
            }

            /// copy赋值操作符
            Device & operator = (const Device & device)
            {
                if (this != &device)
                {
                    /// 设备类型必须相同
                    assert(GetDeviceType() == device.GetDeviceType());

                    m_sn = device.m_sn;
                    m_addr = device.m_addr;
                    m_ncpVersion = device.m_ncpVersion;
                    m_softwareVersion = device.m_softwareVersion;
                    m_platformVersion = device.m_platformVersion;
                }

                return *this;
            }

            /// 虚析构
            virtual ~Device();

        public:
            /**
             * @brief 获取设备时间（同步）
             * @param [out] time    设备时间
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note time中的6个字节按照索引顺序依次表示年[0, 255]、月[1-12]、日[1-31]、时[0-23]、分[0-59]、秒[0-59]，其中年表示从2000年开始经历的年数
             */
            ErrorCode GetDeviceTime(std::array<unsigned char, 6> & time, size_t timeout = 0);

            /**
             * @brief 获取设备时间（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 回调函数第一个参数表示异步获取到的设备时间，结构见“GetDeviceTime:获取设备时间（同步）”
             */
            ErrorCode GetDeviceTime(std::function<void(const std::array<unsigned char, 6> &, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 设置设备时间为当前系统时间（同步）
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetDeviceTime(size_t timeout = 0);

            /**
             * @brief 设置设备时间为当前系统时间（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetDeviceTime(std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 设置设备时间（同步）
             * @param [out] time    要设置的时间
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note time结构见“GetDeviceTime:获取设备时间（同步）”
             */
            ErrorCode SetDeviceTime(const std::array<unsigned char, 6> & time, size_t timeout = 0);

            /**
             * @brief 设置设备时间（异步）
             * @param [out] time     要设置的时间
             * @param [in]  callback 回调函数
             * @param [in]  timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note time结构见“GetDeviceTime:获取设备时间（同步）”
             */
            ErrorCode SetDeviceTime(const std::array<unsigned char, 6> & time, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 获取模拟量值（同步）
             * @param [in]  type    模拟量类型
             * @param [out] value   模拟量值
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 具体设备应通过此函数实现具体模拟量类型的功能接口
             */
            ErrorCode GetAnalogValue(unsigned char type, unsigned int & value, size_t timeout = 0);

            /**
             * @brief 获取模拟量值（异步）
             * @param [in] type     模拟量类型
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 具体设备应通过此函数实现具体模拟量类型的功能接口
             */
            ErrorCode GetAnalogValue(unsigned char type, std::function<void(unsigned int, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 获取设备状态（同步）
             * @param [out] status  设备状态
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 具体设备应通过此函数实现具体设备参数类型的功能接口
             */
            template <typename T>
            ErrorCode GetStatus(T & status, size_t timeout = 0)
            {
                return ReadSingleBufferNoParam(Resource::Status, (char *)&status, sizeof(T), timeout);
            }

            /**
             * @brief 获取设备状态（异步）
             * @param [out] callback 回调函数
             * @param [in]  timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 具体设备应通过此函数实现具体设备参数类型的功能接口
             */
            template <typename T>
            ErrorCode GetStatus(std::function<void(const T&, ErrorCode)> callback, size_t timeout = 0)
            {
                return ReadSingleBufferNoParam(Resource::Status, sizeof(T), 
                                                [callback](const char * ptr, ErrorCode ec) {
                                                    T value;
                                                    memcpy(&value, ptr, sizeof(T));
                                                    callback(value, ec);
                                                }, timeout);
            }

            /**
             * @brief 获取设备参数（同步）
             * @param [out] parameters 设备参数
             * @param [in]  timeout    超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 具体设备应通过此函数实现具体设备参数类型的功能接口
             */
            template <typename T>
            ErrorCode GetParameters(T & parameters, size_t timeout = 0)
            {
                return ReadSingleBufferNoParam(Resource::Parameters, (char *)&parameters, sizeof(T), timeout);
            }

            /**
             * @brief 获取设备参数（异步）
             * @param [out] callback 回调函数
             * @param [in]  timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 具体设备应通过此函数实现具体设备参数类型的功能接口
             */
            template <typename T>
            ErrorCode GetParameters(std::function<void(const T&, ErrorCode)> callback, size_t timeout = 0)
            {
                return ReadSingleBufferNoParam(Resource::Parameters, sizeof(T), 
                                                [callback](const char * ptr, ErrorCode ec) {
                                                    T value;
                                                    memcpy(&value, ptr, sizeof(T));
                                                    callback(value, ec);
                                                }, timeout);
            }

            /**
             * @brief 获取物理位置
             * @param [out] location 获取到的物理位置
             * @param [in]  timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note location为GB18030-2005编码
             */
            ErrorCode GetLocation(std::wstring & location, size_t timeout = 0);

            /**
             * @brief 获取物理位置
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note callback的参数为物理位置，编码为GB18030-2005
             */
            ErrorCode GetLocation(std::function<void(const std::wstring &, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 设置物理位置
             * @param [in] location 物理位置
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note location为GB18030-2005编码
             */
            ErrorCode SetLocation(const std::wstring & location, size_t timeout = 0);

            /**
             * @brief 设置物理位置
             * @param [in] location 物理位置
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note location为GB18030-2005编码
             */
            ErrorCode SetLocation(const std::wstring & location, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 设置设备参数（同步）
             * @param [out] parameters 设备参数
             * @param [in]  timeout    超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 具体设备应通过此函数实现具体设备参数类型的功能接口
             */
            template <typename T>
            ErrorCode SetParameters(const T & parameters, size_t timeout = 0)
            {
                return WriteSingleBufferNoParam(Resource::Parameters, (const char *)&parameters, sizeof(T), timeout);
            }

            /**
             * @brief 设置设备参数（异步）
             * @param [out] parameters 设备参数
             * @param [in]  callback   回调函数
             * @param [in]  timeout    超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 具体设备应通过此函数实现具体设备参数类型的功能接口
             */
            template <typename T>
            ErrorCode SetParameters(const T & settings, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return WriteSingleBufferNoParam(Resource::Parameters, (const char *)&settings, sizeof(T), callback, timeout);
            }

            /**
             * @brief 获取设备指定时间范围内的运行记录（同步）
             * @param [in]  timeBegin 起始时间
             * @param [in]  timeEnd   结束时间
             * @param [out] records   设备运行记录
             * @param [out] hasMore   是否还有更多记录
             * @param [in]  timeout   超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 起始时间和结束时间结构见“GetDeviceTime:获取设备时间（同步）”
             * @note 每条记录包含两个个成员：记录时间（结构见“GetDeviceTime:获取设备时间（同步）”）和记录内容（GB18030编码， 代码页54946）
             */
            ErrorCode GetOperationRecord(const std::array<unsigned char, 6> & timeBegin, const std::array<unsigned char, 6> & timeEnd, 
                                         std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> & records, bool & hasMore, size_t timeout = 0);

            /**
             * @brief 获取设备指定时间范围内的运行记录（异步）
             * @param [in]  timeBegin 起始时间
             * @param [in]  timeEnd   结束时间
             * @param [out] callback  回调函数（参数依次为运行记录、是否还有更多记录、错误号）
             * @param [in]  timeout   超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 起始时间和结束时间结构见“GetDeviceTime:获取设备时间（同步）”
             * @note 记录结构见“GetOperationRecord:获取设备指定时间范围内的运行记录（同步）”
             */
            ErrorCode GetOperationRecord(const std::array<unsigned char, 6> & timeBegin, const std::array<unsigned char, 6> & timeEnd, 
                                         std::function<void(const std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> &, bool, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 获取设备所有运行记录（同步）
             * @param [out] records   设备运行记录
             * @param [out] hasMore   是否还有更多记录
             * @param [in]  timeout   超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 记录结构见“GetOperationRecord:获取设备指定时间范围内的运行记录（同步）”
             */
            ErrorCode GetOperationRecord(std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> & records, bool & hasMore, size_t timeout = 0);

            /**
             * @brief 获取设备所有运行记录（异步）
             * @param [out] callback  回调函数（参数依次为运行记录、是否还有更多记录、错误号）
             * @param [in]  timeout   超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 记录结构见“GetOperationRecord:获取设备指定时间范围内的运行记录（同步）”
             */
            ErrorCode GetOperationRecord(std::function<void(const std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> &, bool, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 控制单个继电器动作（同步）
             * @param [in] idx     继电器编号
             * @param [in] isClose 控制动作：1-闭合，0-打开
             * @param [in] seconds 动作保持时间
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 如果动作保持时间为0，则表示不执行动作保持控制（仅执行指定动作），否则在seconds秒之后，恢复控制之前的状态；如果在保持时间内接收到新的控制指令，则放弃之前的保持控制
             */
            ErrorCode CtrlRelay(unsigned char idx, unsigned char isClose, unsigned char seconds = 0, size_t timeout = 0);

            /**
             * @brief 控制单个继电器动作（异步）
             * @param [in] idx      继电器编号
             * @param [in] isClose  控制动作：1-闭合，0-打开
             * @param [in] seconds  动作保持时间
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 关于“动作保持”参见“CtrlRelay:控制单个继电器动作（同步）”
             */
            ErrorCode CtrlRelay(unsigned char idx, unsigned char isClose, unsigned char seconds, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 控制多个继电器动作（同步）
             * @param [in] ctrls   继电器控制动作
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 每个继电器保持动作包含3个字段，按照索引顺序依次为继电器编号、控制动作和动作保持时间，具体含义参见“CtrlRelay:控制单个继电器动作（同步）中的idx/isClose/seconds参数”
             */
            ErrorCode CtrlRelay(std::vector<std::array<unsigned char, 3>> ctrls, size_t timeout = 0);

            /**
             * @brief 控制多个继电器动作（异步）
             * @param [in] ctrls    继电器控制动作
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 继电器控制动作项含义见“CtrlRelay:控制多个继电器动作（同步）”
             */
            ErrorCode CtrlRelay(std::vector<std::array<unsigned char, 3>> ctrls, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 屏蔽/解除屏蔽设备（同步）
             * @param [in] isDisable 屏蔽指示：true-屏蔽，false-解除屏蔽
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 被屏蔽的设备在重启后仍然会发起注册，并且可以接受控制指令，但不会执行任何状态上传
             */
            ErrorCode Disable(bool isDisable, size_t timeout = 0);

            /**
             * @brief 屏蔽/解除屏蔽设备（异步）
             * @param [in] isDisable 屏蔽指示：true-屏蔽，false-解除屏蔽
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Disable(bool isDisable, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 恢复设备缺省参数（同步）
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 不会恢复网络参数
             */
            ErrorCode SetDefaultParameters(size_t timeout = 0);

            /**
             * @brief 恢复设备缺省参数（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetDefaultParameters(std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 复位设备（同步）
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Reset(size_t timeout = 0);

            /**
             * @brief 复位设备（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Reset(std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 重启设备（同步）
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Reboot(size_t timeout = 0);

            /**
             * @brief 重启设备（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode Reboot(std::function<void(ErrorCode)> callback, size_t timeout = 0);
            
        private:
            /**
             * @brief 执行无参数单缓冲查询（同步）
             * @param [in] resource 待查询的资源
             * @param [in] ptr      缓冲区
             * @param [in] sz       缓冲区大小
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode ReadSingleBufferNoParam(Resource resource, char * ptr, size_t sz, size_t timeout = 0);

            /**
             * @brief 执行无参数单缓冲查询（异步）
             * @param [in] resource 待查询的资源
             * @param [in] sz       缓冲区大小
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode ReadSingleBufferNoParam(Resource resource, size_t sz, std::function<void(const char *, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 执行无参数单缓冲设置（同步）
             * @param [in] resource 待设置的资源
             * @param [in] ptr      缓冲区
             * @param [in] sz       缓冲区大小
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode WriteSingleBufferNoParam(Resource resource, const char * ptr, size_t sz, size_t timeout = 0);

            /**
             * @brief 执行无参数单缓冲设置（异步）
             * @param [in] resource 待设置的资源
             * @param [in] ptr      缓冲区
             * @param [in] sz       缓冲区大小
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode WriteSingleBufferNoParam(Resource resource, const char * ptr, size_t sz, std::function<void(ErrorCode)> callback, size_t timeout = 0);
        
        public:
            /// 获取设备序列号
            const SerialNumber & GetSerialNumber() const { return m_sn; }
            /// 获取设备网络地址
            const Address & GetAddress() const { return m_addr; }
            /// 获取设备NCP协议版本
            const Version & GetNcpVersion() const { return m_ncpVersion; }
            /// 获取设备软件版本
            const Version & GetSoftwareVersion() const { return m_softwareVersion; }
            /// 获取设备平台版本
            const Version & GetPlatformVersion() const { return m_platformVersion; }

            /// 判断设备是否已连接并注册成功
            bool IsConnected() const;

            /// 断开设备连接
            void Disconnect();

            /// 获取设备类型
            virtual const DeviceType * GetDeviceType() const = 0;

        protected:
            /**
             * @brief 设备主动上传数据包处理函数
             * @param [in] pkt 主动上传数据包
             * @note 注意，此处不仅包括“上传”类型的数据包，同样包含因激活设备而发送的状态查询包引发的状态回应包，即包含冗余状态“上传”
             */
            virtual void OnUpload(std::shared_ptr<Packet> pkt) {};

            /**
             * @brief 模拟量上传处理函数
             * @param [in] type  模拟量类型
             * @param [in] value 模拟量值
             */
            virtual void OnUploadAnalog(unsigned char type, unsigned int value) {};

            /**
             * @brief 物理位置上传函数
             * @param [in] location 物理位置
             * @note location为GB18030-2005编码
             */
            virtual void OnUploadLocation(const std::wstring & location) {}

            /**
             * @brief 设备断开处理函数
             */
            virtual void OnDisconnected() 
            { 
            }

            /**
             * @brief 设备连接并注册成功处理函数
             */
            virtual void OnConnected() 
            {
				//printf("%s[%s:%d]\n", __FILE__, __func__, __LINE__);
            }

        private:
            /**
             * @brief 设备主动上传数据包分发函数
             * @param [in] pkt 主动上传数据包
             * @note 注意，此处不仅包括“上传”类型的数据包，同样包含因激活设备而发送的状态查询包引发的状态回应包，即包含冗余状态“上传”
             */
            void UploadDispatch(std::shared_ptr<Packet> pkt);

        protected: // 在设备注册成功后，以下内容由DeviceImpl填充；如果设备对象通过反序列化构造，则应由反序列化执行者填充（或者不填充，如果不会去读的话）
            /// 设备序列号
            SerialNumber    m_sn;
            /// 设备地址
            Address         m_addr;

            /// 设备NCP版本
            Version         m_ncpVersion;
            /// 设备软件版本
            Version         m_softwareVersion;
            /// 设备平台版本
            Version         m_platformVersion;

        protected: // 以下内容应仅由NCP协议内部使用
            /// 获取DeviceImpl指针
            std::shared_ptr<DeviceImpl> QueryImpl() const;

            /// 功能实现
            std::weak_ptr<DeviceImpl> *    m_impl = nullptr;
        };
    };

};

#endif