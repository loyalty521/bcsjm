#pragma once
#ifndef _NCP_SERVER_DEVICES_VIDEOFIREDETECTOR_H
#define _NCP_SERVER_DEVICES_VIDEOFIREDETECTOR_H

#include "../../Ncp.h"
#include "../Device.h"

#include <array>
#include <vector>

namespace Ncp
{
    namespace Server
    {
        /// 图像型火灾探测器
        class NCP_API VideoFireDetector : public Device
        {
        public:
            /**
             * @brief 获取最后一次报警时的报警图片（同步）
             * @param [out] pictures 报警图片，每项包含图片格式和图片数据缓冲区两个字段
             * @param [in]  timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 图片格式（1：格式YUV422PSEMI，分辨率720*576，压缩格式ZIP，其余值尚未定义）
             * @note 若从探测器开机至今从未报警，则返回ExecuteFailed错误码
             */
            ErrorCode GetAlarmPicture(std::vector<std::pair<unsigned char, std::vector<char>>> & pictures, size_t timeout = 0);

            /**
             * @brief 获取最后一次报警时的报警图片（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             * @note 若从探测器开机至今从未报警，则返回ExecuteFailed错误码
             * @note 回调函数参数依次为图片列表、错误号，具体含义见“GetAlarmPicture:获取最后一次报警时的报警图片（同步）”中的pictures参数
             */
            ErrorCode GetAlarmPicture(std::function<void(const std::vector<std::pair<unsigned char, std::vector<char>>> &, ErrorCode)> callback, size_t timeout = 0);


            /**
             * @brief 查询所有可用算法列表（同步）
             * @param [out] algs 算法列表
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetAlgList(std::vector<VideoFireDetectorAlgItemRead> & algs, size_t timeout = 0);

            /**
             * @brief 查询所有可用算法列表（同步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetAlgList(std::function<void(const std::vector<VideoFireDetectorAlgItemRead> &, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 设置算法列表（同步）
             * @param [in] algs 算法列表
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetAlgList(const std::vector<VideoFireDetectorAlgItemWrite> & algs, size_t timeout = 0);
            
            /**
             * @brief 设置算法列表（异步）
             * @param [in] algs 算法列表
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetAlgList(const std::vector<VideoFireDetectorAlgItemWrite> & algs, std::function<void(ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 获取探测器状态（同步）
             * @param [out] status  探测器状态
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetStatus(VideoFireDetectorStatus & status, size_t timeout = 0)
            {
                return Device::GetStatus<VideoFireDetectorStatus>(status, timeout);
            }
            /**
             * @brief 获取探测器状态（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetStatus(std::function<void(const VideoFireDetectorStatus &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetStatus<VideoFireDetectorStatus>(callback, timeout);
            }

            /**
             * @brief 获取探测器参数（同步）
             * @param [out] params  探测器参数
             * @param [in]  timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(VideoFireDetectorParameters & params, size_t timeout = 0)
            {
                return Device::GetParameters<VideoFireDetectorParameters>(params, timeout);
            }
            /**
             * @brief 获取探测器参数（异步）
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetParameters(std::function<void(const VideoFireDetectorParameters &, ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::GetParameters<VideoFireDetectorParameters>(callback, timeout);
            }
            
            /**
             * @brief 设置探测器参数（同步）
             * @param [in] params  探测器参数
             * @param [in] timeout 超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetParameters(const VideoFireDetectorParameters & params, size_t timeout = 0)
            {
                return Device::SetParameters<VideoFireDetectorParameters>(params, timeout);
            }
            /**
             * @brief 设置探测器参数（异步）
             * @param [in] params   探测器参数
             * @param [in] callback 回调函数
             * @param [in] timeout  超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetParameters(const VideoFireDetectorParameters & params, std::function<void(ErrorCode)> callback, size_t timeout = 0)
            {
                return Device::SetParameters<VideoFireDetectorParameters>(params, callback, timeout);
            }

            /**
             * @brief 获取算法参数（同步）
             * @param [in]  name        算法名称（UTF-8编码）
             * @param [in]  sensibility 灵敏度，取值范围[0，灵敏度上限]，其中0为自定义灵敏度
             * @param [out] paramsJson  算法参数，（JSON格式，UTF-8）编码
             * @param [in]  timeout     超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetAlgParameters(const std::string & name, unsigned char sensibility, std::string & paramsJson, size_t timeout = 0);

            /**
             * @brief 获取算法参数（异步）
             * @param [in] name        算法名称（UTF-8编码）
             * @param [in] sensibility 灵敏度，取值范围[0，灵敏度上限]，其中0为自定义灵敏度
             * @param [in] callback    回调函数
             * @param [in] timeout     超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode GetAlgParameters(const std::string & name, unsigned char sensibility, std::function<void(const std::string &, ErrorCode)> callback, size_t timeout = 0);

            /**
             * @brief 设置算法参数（同步）
             * @param [in] name        算法名称（UTF-8编码）
             * @param [in] sensibility 灵敏度，取值范围[0，灵敏度上限]，其中0为自定义灵敏度
             * @param [in] paramsJson  算法参数，（JSON格式，UTF-8）编码
             * @param [in] timeout     超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetAlgParameters(const std::string & name, unsigned char sensibility, const std::string & paramsJson, size_t timeout = 0);

            /**
             * @brief 设置算法参数（异步）
             * @param [in] name        算法名称（UTF-8编码）
             * @param [in] sensibility 灵敏度，取值范围[0，灵敏度上限]，其中0为自定义灵敏度
             * @param [in] paramsJson  算法参数，（JSON格式，UTF-8）编码
             * @param [in] callback    回调函数
             * @param [in] timeout     超时时间（毫秒），如果为0，则使用缺省超时时间，即单次重发超时*（最大重发次数+1）
             * @return 返回错误码
             */
            ErrorCode SetAlgParameters(const std::string & name, unsigned char sensibility, const std::string & paramsJson, std::function<void(ErrorCode)> callback, size_t timeout = 0);

        protected:
            /**
             * @brief 报警图片上传处理函数
             * @param [in] pictures 报警图片
             * @note pictures中每项包含图片格式和图片数据缓冲区两个字段
             * @note 图片格式（1：格式YUV422PSEMI，分辨率720*576，压缩格式ZIP，其余值尚未定义）
             */
            virtual void OnUploadAlarmPicture(const std::vector<std::pair<unsigned char, std::vector<char>>> & pictures) {};

            /**
             * @brief 设备状态上传处理函数
             * @param [in] status 设备状态
             */
            virtual void OnUploadStatus(const VideoFireDetectorStatus & status) {};

        private:
            /// 数据包上传处理函数
            virtual void OnUpload(std::shared_ptr<Packet> pkt);
        };
    };
};

#endif