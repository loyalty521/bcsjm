////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file Device.h
/// @brief NcpЭ�������豸���ඨ���ļ�
///
/// ����NcpЭ�������豸����
///
/// @version 1.0
/// @author  ���غ�
/// @date    2013��12��1��
///
///
/// �޶�˵��������汾
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

        /// NCP������豸����
        class NCP_API Device
        {
            friend class DeviceImpl;
            friend class ServerImpl;

        public:
            /// ����ȱʡ���캯��
            Device() = default;

            /// copy���캯��
            Device(const Device & device) :
                m_sn(device.m_sn),
                m_addr(device.m_addr),
                m_ncpVersion(device.m_ncpVersion),
                m_softwareVersion(device.m_softwareVersion),
                m_platformVersion(device.m_platformVersion)
            {
            }

            /// copy��ֵ������
            Device & operator = (const Device & device)
            {
                if (this != &device)
                {
                    /// �豸���ͱ�����ͬ
                    assert(GetDeviceType() == device.GetDeviceType());

                    m_sn = device.m_sn;
                    m_addr = device.m_addr;
                    m_ncpVersion = device.m_ncpVersion;
                    m_softwareVersion = device.m_softwareVersion;
                    m_platformVersion = device.m_platformVersion;
                }

                return *this;
            }

            /// ������
            virtual ~Device();

        public:
            /**
             * @brief ��ȡ�豸ʱ�䣨ͬ����
             * @param [out] time    �豸ʱ��
             * @param [in]  timeout ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note time�е�6���ֽڰ�������˳�����α�ʾ��[0, 255]����[1-12]����[1-31]��ʱ[0-23]����[0-59]����[0-59]���������ʾ��2000�꿪ʼ����������
             */
            ErrorCode GetDeviceTime(std::array<unsigned char, 6> & time, size_t timeout = 0);

            /**
             * @brief ��ȡ�豸ʱ�䣨�첽��
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �ص�������һ��������ʾ�첽��ȡ�����豸ʱ�䣬�ṹ����GetDeviceTime:��ȡ�豸ʱ�䣨ͬ������
             */
            ErrorCode GetDeviceTime(std::function<void(const std::array<unsigned char, 6> &, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief �����豸ʱ��Ϊ��ǰϵͳʱ�䣨ͬ����
             * @param [in] timeout ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode SetDeviceTime(size_t timeout = 0);

            /**
             * @brief �����豸ʱ��Ϊ��ǰϵͳʱ�䣨�첽��
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode SetDeviceTime(std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief �����豸ʱ�䣨ͬ����
             * @param [out] time    Ҫ���õ�ʱ��
             * @param [in]  timeout ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note time�ṹ����GetDeviceTime:��ȡ�豸ʱ�䣨ͬ������
             */
            ErrorCode SetDeviceTime(const std::array<unsigned char, 6> & time, size_t timeout = 0);

            /**
             * @brief �����豸ʱ�䣨�첽��
             * @param [out] time     Ҫ���õ�ʱ��
             * @param [in]  callback �ص�����
             * @param [in]  timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note time�ṹ����GetDeviceTime:��ȡ�豸ʱ�䣨ͬ������
             */
            ErrorCode SetDeviceTime(const std::array<unsigned char, 6> & time, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ��ȡģ����ֵ��ͬ����
             * @param [in]  type    ģ��������
             * @param [out] value   ģ����ֵ
             * @param [in]  timeout ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����豸Ӧͨ���˺���ʵ�־���ģ�������͵Ĺ��ܽӿ�
             */
            ErrorCode GetAnalogValue(unsigned char type, unsigned int & value, size_t timeout = 0);

            /**
             * @brief ��ȡģ����ֵ���첽��
             * @param [in] type     ģ��������
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����豸Ӧͨ���˺���ʵ�־���ģ�������͵Ĺ��ܽӿ�
             */
            ErrorCode GetAnalogValue(unsigned char type, std::function<void(unsigned int, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ��ȡ�豸״̬��ͬ����
             * @param [out] status  �豸״̬
             * @param [in]  timeout ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����豸Ӧͨ���˺���ʵ�־����豸�������͵Ĺ��ܽӿ�
             */
            template <typename T>
            ErrorCode GetStatus(T & status, size_t timeout = 0)
            {
                return ReadSingleBufferNoParam(Resource::Status, (char *)&status, sizeof(T), timeout);
            }

            /**
             * @brief ��ȡ�豸״̬���첽��
             * @param [out] callback �ص�����
             * @param [in]  timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����豸Ӧͨ���˺���ʵ�־����豸�������͵Ĺ��ܽӿ�
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
             * @brief ��ȡ�豸������ͬ����
             * @param [out] parameters �豸����
             * @param [in]  timeout    ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����豸Ӧͨ���˺���ʵ�־����豸�������͵Ĺ��ܽӿ�
             */
            template <typename T>
            ErrorCode GetParameters(T & parameters, size_t timeout = 0)
            {
                return ReadSingleBufferNoParam(Resource::Parameters, (char *)&parameters, sizeof(T), timeout);
            }

            /**
             * @brief ��ȡ�豸�������첽��
             * @param [out] callback �ص�����
             * @param [in]  timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����豸Ӧͨ���˺���ʵ�־����豸�������͵Ĺ��ܽӿ�
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
             * @brief ��ȡ����λ��
             * @param [out] location ��ȡ��������λ��
             * @param [in]  timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note locationΪGB18030-2005����
             */
            ErrorCode GetLocation(std::wstring & location, size_t timeout = 0);

            /**
             * @brief ��ȡ����λ��
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note callback�Ĳ���Ϊ����λ�ã�����ΪGB18030-2005
             */
            ErrorCode GetLocation(std::function<void(const std::wstring &, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ��������λ��
             * @param [in] location ����λ��
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note locationΪGB18030-2005����
             */
            ErrorCode SetLocation(const std::wstring & location, size_t timeout = 0);

            /**
             * @brief ��������λ��
             * @param [in] location ����λ��
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note locationΪGB18030-2005����
             */
            ErrorCode SetLocation(const std::wstring & location, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief �����豸������ͬ����
             * @param [out] parameters �豸����
             * @param [in]  timeout    ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����豸Ӧͨ���˺���ʵ�־����豸�������͵Ĺ��ܽӿ�
             */
            template <typename T>
            ErrorCode SetParameters(const T & parameters, size_t timeout = 0)
            {
                return WriteSingleBufferNoParam(Resource::Parameters, (const char *)&parameters, sizeof(T), timeout);
            }

            /**
             * @brief �����豸�������첽��
             * @param [out] parameters �豸����
             * @param [in]  callback   �ص�����
             * @param [in]  timeout    ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����豸Ӧͨ���˺���ʵ�־����豸�������͵Ĺ��ܽӿ�
             */
            template <typename T>
            ErrorCode SetParameters(const T & settings, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return WriteSingleBufferNoParam(Resource::Parameters, (const char *)&settings, sizeof(T), callback, timeout);
            }

            /**
             * @brief ��ȡ�豸ָ��ʱ�䷶Χ�ڵ����м�¼��ͬ����
             * @param [in]  timeBegin ��ʼʱ��
             * @param [in]  timeEnd   ����ʱ��
             * @param [out] records   �豸���м�¼
             * @param [out] hasMore   �Ƿ��и����¼
             * @param [in]  timeout   ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note ��ʼʱ��ͽ���ʱ��ṹ����GetDeviceTime:��ȡ�豸ʱ�䣨ͬ������
             * @note ÿ����¼������������Ա����¼ʱ�䣨�ṹ����GetDeviceTime:��ȡ�豸ʱ�䣨ͬ���������ͼ�¼���ݣ�GB18030���룬 ����ҳ54946��
             */
            ErrorCode GetOperationRecord(const std::array<unsigned char, 6> & timeBegin, const std::array<unsigned char, 6> & timeEnd, 
                                         std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> & records, bool & hasMore, size_t timeout = 0);

            /**
             * @brief ��ȡ�豸ָ��ʱ�䷶Χ�ڵ����м�¼���첽��
             * @param [in]  timeBegin ��ʼʱ��
             * @param [in]  timeEnd   ����ʱ��
             * @param [out] callback  �ص���������������Ϊ���м�¼���Ƿ��и����¼������ţ�
             * @param [in]  timeout   ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note ��ʼʱ��ͽ���ʱ��ṹ����GetDeviceTime:��ȡ�豸ʱ�䣨ͬ������
             * @note ��¼�ṹ����GetOperationRecord:��ȡ�豸ָ��ʱ�䷶Χ�ڵ����м�¼��ͬ������
             */
            ErrorCode GetOperationRecord(const std::array<unsigned char, 6> & timeBegin, const std::array<unsigned char, 6> & timeEnd, 
                                         std::function<void(const std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> &, bool, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ��ȡ�豸�������м�¼��ͬ����
             * @param [out] records   �豸���м�¼
             * @param [out] hasMore   �Ƿ��и����¼
             * @param [in]  timeout   ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note ��¼�ṹ����GetOperationRecord:��ȡ�豸ָ��ʱ�䷶Χ�ڵ����м�¼��ͬ������
             */
            ErrorCode GetOperationRecord(std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> & records, bool & hasMore, size_t timeout = 0);

            /**
             * @brief ��ȡ�豸�������м�¼���첽��
             * @param [out] callback  �ص���������������Ϊ���м�¼���Ƿ��и����¼������ţ�
             * @param [in]  timeout   ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note ��¼�ṹ����GetOperationRecord:��ȡ�豸ָ��ʱ�䷶Χ�ڵ����м�¼��ͬ������
             */
            ErrorCode GetOperationRecord(std::function<void(const std::vector<std::pair<std::array<unsigned char, 6>, std::vector<char>>> &, bool, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ���Ƶ����̵���������ͬ����
             * @param [in] idx     �̵������
             * @param [in] isClose ���ƶ�����1-�պϣ�0-��
             * @param [in] seconds ��������ʱ��
             * @param [in] timeout ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����������ʱ��Ϊ0�����ʾ��ִ�ж������ֿ��ƣ���ִ��ָ����������������seconds��֮�󣬻ָ�����֮ǰ��״̬������ڱ���ʱ���ڽ��յ��µĿ���ָ������֮ǰ�ı��ֿ���
             */
            ErrorCode CtrlRelay(unsigned char idx, unsigned char isClose, unsigned char seconds = 0, size_t timeout = 0);

            /**
             * @brief ���Ƶ����̵����������첽��
             * @param [in] idx      �̵������
             * @param [in] isClose  ���ƶ�����1-�պϣ�0-��
             * @param [in] seconds  ��������ʱ��
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note ���ڡ��������֡��μ���CtrlRelay:���Ƶ����̵���������ͬ������
             */
            ErrorCode CtrlRelay(unsigned char idx, unsigned char isClose, unsigned char seconds, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ���ƶ���̵���������ͬ����
             * @param [in] ctrls   �̵������ƶ���
             * @param [in] timeout ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note ÿ���̵������ֶ�������3���ֶΣ���������˳������Ϊ�̵�����š����ƶ����Ͷ�������ʱ�䣬���庬��μ���CtrlRelay:���Ƶ����̵���������ͬ�����е�idx/isClose/seconds������
             */
            ErrorCode CtrlRelay(std::vector<std::array<unsigned char, 3>> ctrls, size_t timeout = 0);

            /**
             * @brief ���ƶ���̵����������첽��
             * @param [in] ctrls    �̵������ƶ���
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �̵������ƶ���������CtrlRelay:���ƶ���̵���������ͬ������
             */
            ErrorCode CtrlRelay(std::vector<std::array<unsigned char, 3>> ctrls, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ����/��������豸��ͬ����
             * @param [in] isDisable ����ָʾ��true-���Σ�false-�������
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note �����ε��豸����������Ȼ�ᷢ��ע�ᣬ���ҿ��Խ��ܿ���ָ�������ִ���κ�״̬�ϴ�
             */
            ErrorCode Disable(bool isDisable, size_t timeout = 0);

            /**
             * @brief ����/��������豸���첽��
             * @param [in] isDisable ����ָʾ��true-���Σ�false-�������
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode Disable(bool isDisable, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief �ָ��豸ȱʡ������ͬ����
             * @param [in] timeout ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             * @note ����ָ��������
             */
            ErrorCode SetDefaultParameters(size_t timeout = 0);

            /**
             * @brief �ָ��豸ȱʡ�������첽��
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode SetDefaultParameters(std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ��λ�豸��ͬ����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode Reset(size_t timeout = 0);

            /**
             * @brief ��λ�豸���첽��
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode Reset(std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief �����豸��ͬ����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode Reboot(size_t timeout = 0);

            /**
             * @brief �����豸���첽��
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode Reboot(std::function<void(ErrorCode)> callback, size_t timeout = 0);
            
        private:
            /**
             * @brief ִ���޲����������ѯ��ͬ����
             * @param [in] resource ����ѯ����Դ
             * @param [in] ptr      ������
             * @param [in] sz       ��������С
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode ReadSingleBufferNoParam(Resource resource, char * ptr, size_t sz, size_t timeout = 0);

            /**
             * @brief ִ���޲����������ѯ���첽��
             * @param [in] resource ����ѯ����Դ
             * @param [in] sz       ��������С
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode ReadSingleBufferNoParam(Resource resource, size_t sz, std::function<void(const char *, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief ִ���޲������������ã�ͬ����
             * @param [in] resource �����õ���Դ
             * @param [in] ptr      ������
             * @param [in] sz       ��������С
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode WriteSingleBufferNoParam(Resource resource, const char * ptr, size_t sz, size_t timeout = 0);

            /**
             * @brief ִ���޲������������ã��첽��
             * @param [in] resource �����õ���Դ
             * @param [in] ptr      ������
             * @param [in] sz       ��������С
             * @param [in] callback �ص�����
             * @param [in] timeout  ��ʱʱ�䣨���룩�����Ϊ0����ʹ��ȱʡ��ʱʱ�䣬�������ط���ʱ*������ط�����+1��
             * @return ���ش�����
             */
            ErrorCode WriteSingleBufferNoParam(Resource resource, const char * ptr, size_t sz, std::function<void(ErrorCode)> callback, size_t timeout = 0);
        
        public:
            /// ��ȡ�豸���к�
            const SerialNumber & GetSerialNumber() const { return m_sn; }
            /// ��ȡ�豸�����ַ
            const Address & GetAddress() const { return m_addr; }
            /// ��ȡ�豸NCPЭ��汾
            const Version & GetNcpVersion() const { return m_ncpVersion; }
            /// ��ȡ�豸����汾
            const Version & GetSoftwareVersion() const { return m_softwareVersion; }
            /// ��ȡ�豸ƽ̨�汾
            const Version & GetPlatformVersion() const { return m_platformVersion; }

            /// �ж��豸�Ƿ������Ӳ�ע��ɹ�
            bool IsConnected() const;

            /// �Ͽ��豸����
            void Disconnect();

            /// ��ȡ�豸����
            virtual const DeviceType * GetDeviceType() const = 0;

        protected:
            /**
             * @brief �豸�����ϴ����ݰ�������
             * @param [in] pkt �����ϴ����ݰ�
             * @note ע�⣬�˴������������ϴ������͵����ݰ���ͬ�������򼤻��豸�����͵�״̬��ѯ��������״̬��Ӧ��������������״̬���ϴ���
             */
            virtual void OnUpload(std::shared_ptr<Packet> pkt) {};

            /**
             * @brief ģ�����ϴ�������
             * @param [in] type  ģ��������
             * @param [in] value ģ����ֵ
             */
            virtual void OnUploadAnalog(unsigned char type, unsigned int value) {};

            /**
             * @brief ����λ���ϴ�����
             * @param [in] location ����λ��
             * @note locationΪGB18030-2005����
             */
            virtual void OnUploadLocation(const std::wstring & location) {}

            /**
             * @brief �豸�Ͽ�������
             */
            virtual void OnDisconnected() 
            { 
            }

            /**
             * @brief �豸���Ӳ�ע��ɹ�������
             */
            virtual void OnConnected() 
            {
				//printf("%s[%s:%d]\n", __FILE__, __func__, __LINE__);
            }

        private:
            /**
             * @brief �豸�����ϴ����ݰ��ַ�����
             * @param [in] pkt �����ϴ����ݰ�
             * @note ע�⣬�˴������������ϴ������͵����ݰ���ͬ�������򼤻��豸�����͵�״̬��ѯ��������״̬��Ӧ��������������״̬���ϴ���
             */
            void UploadDispatch(std::shared_ptr<Packet> pkt);

        protected: // ���豸ע��ɹ�������������DeviceImpl��䣻����豸����ͨ�������л����죬��Ӧ�ɷ����л�ִ������䣨���߲���䣬�������ȥ���Ļ���
            /// �豸���к�
            SerialNumber    m_sn;
            /// �豸��ַ
            Address         m_addr;

            /// �豸NCP�汾
            Version         m_ncpVersion;
            /// �豸����汾
            Version         m_softwareVersion;
            /// �豸ƽ̨�汾
            Version         m_platformVersion;

        protected: // ��������Ӧ����NCPЭ���ڲ�ʹ��
            /// ��ȡDeviceImplָ��
            std::shared_ptr<DeviceImpl> QueryImpl() const;

            /// ����ʵ��
            std::weak_ptr<DeviceImpl> *    m_impl = nullptr;
        };
    };

};

#endif