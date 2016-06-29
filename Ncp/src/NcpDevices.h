////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, 合肥科大立安安全技术股份有限公司
/// All rights reserved.
///
/// @file NcpDevices.h
/// @brief Ncp协议设备相关结构和操作的定义文件
///
/// 定义Ncp协议设备相关结构和操作
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

#ifndef _NCP_NCPDEVICES_H
#define _NCP_NCPDEVICES_H

#include <string>

namespace Ncp
{
#pragma pack(push)
#pragma pack(1)
    /// 状态
    struct VideoFireDetectorStatus
    {
        bool debug : 1;                 ///< 调试
        bool alarm : 1;                 ///< 报警
        bool fault : 1;                 ///< 故障
        bool disable : 1;               ///< 屏蔽
        bool preAlarm : 1;              ///< 预警
        bool flameAlarm : 1;            ///< 火焰报警
        bool smokeAlarm : 1;            ///< 烟雾报警
        bool tempAlarm : 1;             ///< 温度报警
        bool videoChannel1NoInput : 1;  ///< 通道1视频缺失
        bool videoChannel2NoInput : 1;  ///< 通道2视频缺失
        bool videoQualityPoor : 1;      ///< 视频质量太差
        bool noSenders : 1;             ///< 未设置发射器
        bool senderShield : 1;          ///< 发射器遮挡

        bool inputRelay1 : 1;           ///< 输入继电器1状态（闭合：true，打开：false）
        bool inputRelay2 : 1;           ///< 输入继电器2状态（闭合：true，打开：false）

        bool __bitpadding : 1;          ///< 填充位
        char __bytepadding[2];          ///< 填充字节

        VideoFireDetectorStatus()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 参数
    struct VideoFireDetectorParameters
    {
        bool osdLogo : 8;       ///< 是否在输出视频中叠加公司LOGO
        bool osdDateTime : 8;   ///< 是否在输出视频中叠加日期时间
        bool osdLocation : 8;   ///< 是否在输出视频中叠加物理位置
        bool osdAlarm : 8;      ///< 是否在输出视频中叠加报警信息
        unsigned int h264Bitrate;   ///< H264目标码率，[1,000,000-10,000,000]（1M-10Mbps）
        unsigned char videoDisplayChannel;  ///< 视频显示通道（0-不输出模拟视频，1-输出通道1视频，2-输出通道2视频）

        VideoFireDetectorParameters()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 算法项（查询算法列表时返回的算法信息）
    struct VideoFireDetectorAlgItemRead
    {
        std::string     name;                   ///< 算法名称（UTF-8编码）
        Version         version;                ///< 算法版本
        unsigned char   videoChannel = 0;       ///< 使用的视频通道（0-不启用，1-使用通道1视频，2-使用通道2视频）
        unsigned char   sensibility = 0;        ///< 当前灵敏度，取值范围[0，灵敏度上限]，其中0为自定义灵敏度
        unsigned char   sensibilityUBound = 0;  ///< 灵敏度上限
        unsigned char   sensibilityDefault = 0; ///< 缺省灵敏度
        std::string     description;            ///< 算法描述（UTF-8编码）
    };

    /// 算法项（设置算法列表时使用）
    struct VideoFireDetectorAlgItemWrite
    {
        std::string     name;               ///< 算法名称（UTF-8编码）
        unsigned char   videoChannel = 0;   ///< 视频通道（0-不启用，1-使用通道1视频，2-使用通道2视频）
        unsigned char   sensibility = 0;    ///< 灵敏度，取值范围[0，灵敏度上限]，其中0为自定义灵敏度
    };

    /// 状态
    struct VideoSFCompositeFireDetectorParameters : VideoFireDetectorParameters
    {
        unsigned char algParallel = 0;  ///< 算法调度模式（0-同时运行，1-逐帧切换，2-周期切换）
        unsigned char alarmMode = 0;    ///< 报警模式（0-其一报警即报警 1-两者均报警即报警）
    };

    /// 自动消防炮解码器状态
    struct HydrantDecoderStatus
    {
        bool debug : 1;                 ///< 调试
        bool alarm : 1;                 ///< 报警
        bool fault : 1;                 ///< 故障
        bool disable : 1;               ///< 屏蔽
        bool preAlarm : 1;              ///< 预警
        bool targeting : 1;             ///< 正在定位
        bool targetSuccess : 1;         ///< 定位成功
        bool targetFail : 1;            ///< 定位失败

        bool leftLimit : 1;             ///< 左极限
        bool rightLimit : 1;            ///< 右极限
        bool upLimit : 1;               ///< 上极限
        bool downLimit : 1;             ///< 下极限
        bool leftRightMoving : 1;       ///< 左右运动
        bool upDownMoving : 1;          ///< 上下运动
        bool poleLimit : 1;             ///< 柱极限
        bool fogLimit : 1;              ///< 雾极限

        bool waterStream : 1;           ///< 有无水流
        bool sceneOrRemote : 1;         ///< 现场远程，0=现场，1=远程
        bool liftFoldLimit : 1;         ///< 升降装置收缩极限
        bool liftUnfoldLimit : 1;       ///< 升降装置展开极限
        bool liftFolding : 1;           ///< 升降装置正在收缩
        bool liftUnfolding : 1;         ///< 升降装置正在展开
        bool mode : 1;                  ///< 工作模式，0=消防炮工作模式，1=探测器工作模式
        bool poleFogMoving : 1;         ///< 柱雾运行

        bool valveOpenLimit : 1;        ///< 阀开极限
        bool valveCloseLimit : 1;       ///< 阀关极限
        bool valveOpening : 1;          ///< 阀开运行
        bool valveClosing : 1;          ///< 阀关运行
        bool signalValveOpenLimit : 1;  ///< 信号阀开极限
        bool signalValveCloseLimit : 1; ///< 信号阀关极限
		bool signalValveOpening : 1;	///< 信号阀开运行
		bool signalValveClosing : 1;	///< 信号阀关运行

        bool valveType : 1;             ///< 阀类型，0=电动阀，1=电磁阀

        uint8_t __bitpadding1 : 7;      ///< 填充位
        uint8_t __bytepadding1;         ///< 填充字节

        bool videoQualityPoor : 1;      ///< 视频质量差
        bool colorVideoLost : 1;        ///< 彩色视频丢失
		bool irVideoLost : 1;			///< 红外视频丢失
        bool liftFault : 1;             ///< 升降装置故障
        bool valveFault : 1;            ///< 阀故障
		bool hydrantFault : 1;          ///< 灭火装置故障（指机械故障）
		bool ncpconnected : 1;          ///<NCP已连接
		bool AbnormalAngle : 1;         ///< 角度异常

		bool AngleFault : 1;            ///< 角度板故障
		bool MotorFault : 1;            ///< 炮电机故障
		bool DriverCanFault : 1;        ///< 驱动板通信故障
		uint8_t __bitpaddign2 : 5;      ///< 填充位

        uint16_t horizontalAngle;        ///< 水平角度
        uint16_t verticalAngle;         ///< 垂直角度
        uint16_t poleFogTicks;          ///< 柱雾刻度

		uint16_t horizonalAngle;        ///< 柱雾刻度		

		bool swinging : 1;				///< 正在摆动
		bool signalValve : 1;           ///< 有信号阀
		bool hasLift : 1;               ///< 有升降装置
		uint8_t __bitpadding3 : 5;      ///< 填充位

		HydrantDecoderStatus() ///20150508
		{
			memset(this, 0, sizeof(*this));
		}
    };

    /// 自动消防炮解码器参数
    struct HydrantDecoderParameters
    {
        HydrantDecoderParameters()
        {
            osdLogo = false;
            osdDateTime = false;
			osdLocation = false;
            osdAlarm = false;            
            h264Bitrate = 1500000;
            hasLift = false;
            hasDetector = 0;
            setupHeight = 10.0f;

            horizontalAngleV = 0.0f;
            horizontalFiringRange = 42.0f;
            locatorAngle = 0.0f;

            horizontalTargetPixel = 360;
            verticalTargetPixel = 288;
            targetError = 4;

            horizontalSwingScope = 0.0f;
            verticalSwingStep = 0.0f;
            verticalSwingScope = 0.0f;

            algSensibilityWhenScan = 1;
            algSensibilityWhenDetect = 1;
            canCommunicationTimeout = 200;
            canCommunicationTimeoutMaxTimes = 3;

            autoFogConvertAngle = -30.0f;
			autoFogConvertEnable = 1;
			autoFogConvertTime = 2500;
            returnPresetAngleH = 0.0f;
            returnPresetAngleV = 0.0f;
            logicalLimitAngleH = 0.0f;
            logicalLimitAngleV = 0.0f;
            
            detectPresetAngleH = 0.0f;
            detectPresetAngleV = -30.0f;
            liftPresetAngleH = 0.0f;
            liftPresetAngleV = 0.0f;

            liftPresetAngleErrorH = 1.0;
            liftPresetAngleErrorV = 1.0;

            verticalScanStepAngle = 13.0f;
            angleH = 16.0f;
            angleV = 16.0f;

            swingMode = SwingMode::HVStep;
            modeSwitchTime = 600;
			swingTime = 300;
            osdTarget = false;
			osdAngle = false; 
			signalValve = false;
			hasLift = false;
			fogScope = 20;
        }


        bool osdLogo : 8;       ///< 是否在输出视频中叠加公司LOGO
        bool osdDateTime : 8;   ///< 是否在输出视频中叠加日期时间
        bool osdLocation : 8;   ///< 是否在输出视频中叠加物理位置
        bool osdAlarm : 8;      ///< 是否在输出视频中叠加报警信息
        uint32_t h264Bitrate;   ///< H264目标码率，[1,000,000-10,000,000]（缺省1500000，1M-10Mbps）

        uint16_t modeSwitchTime;            ///< 工作模式自动切换时间,0=不自动切换,[0, 3600]（单位秒）
		uint16_t swingTime;                 ///< 摆动时间,[0, 600]（单位秒）
        uint16_t hasLift;                   ///< 1=配有升降装置，0=无升降装置
        uint16_t hasDetector;               ///< 1=附带探测功能，0=无探测功能

        float setupHeight;                  ///< 安装高度（单位米）
        float horizontalAngleV;              ///< 炮管水平时的垂直角度
        float horizontalFiringRange;         ///< 水平射程（单位米）
        float locatorAngle;                 ///< 定位器夹角

        uint16_t horizontalTargetPixel;      ///< 水平靶心像素
        uint16_t verticalTargetPixel;       ///< 垂直靶心像素
        uint32_t targetError;               ///< 定位误差（精度）

        float horizontalSwingScope;          ///< 水平摆动范围
        float verticalSwingStep;            ///< 垂直摆动步长
        float verticalSwingScope;           ///< 垂直摆动范围

        uint8_t  algSensibilityWhenScan;    ///< 扫描时算法灵敏度
        uint8_t  algSensibilityWhenDetect;  ///< 探测时算法灵敏度
        uint16_t canCommunicationTimeout;   ///< CAN通信超时时间
        uint32_t canCommunicationTimeoutMaxTimes;   ///< CAN通信最大超时次数

        float    autoFogConvertAngle;       ///< 柱雾转换角度
		uint16_t autoFogConvertEnable;      ///< 柱雾转换使能
		uint16_t autoFogConvertTime;        ///< 柱雾转换时间（毫秒）
        float returnPresetAngleH;           ///< 归位预置位水平角度
        float returnPresetAngleV;           ///< 归位预置位垂直角度
        float logicalLimitAngleH;           ///< 逻辑极限位水平角度
        float logicalLimitAngleV;           ///< 逻辑极限位垂直角度
        float detectPresetAngleH;           ///< 探测预置位水平角度
        float detectPresetAngleV;           ///< 探测预置位垂直角度
        float liftPresetAngleH;             ///< 升降预置位水平角度（收缩时，炮管水平角度）
        float liftPresetAngleV;             ///< 升降预置位垂直角度（收缩时，炮管垂直角度）
        float liftPresetAngleErrorH;        ///< 升降装置预置位水平允许误差
        float liftPresetAngleErrorV;        ///< 升降装置预置位垂直允许误差

        float verticalScanStepAngle;        ///< 垂直扫描步进角度
        float angleH;           ///< 运动水平角度
        float angleV;           ///< 运动垂直角度
        bool osdTarget : 8;     ///< 是否在输出视频中叠加靶心图标
        bool osdAngle : 8;     ///< 是否在输出视频中叠加实时角度
		uint16_t signalValve;  ///< 1=配有信号阀，0=无信号阀
		float fogScope;         ///< 雾化范围（单位米）

        /// 自动消防炮摆动模式
        enum class SwingMode
        {
            HVStep = 0,   ///< 水平垂直逐步摆动（弓字形）
        };

        SwingMode swingMode;                ///< 摆动模式	
    };


    /// 射流灭火装置区域控制箱状态
    struct JettingHydrantDomainCtrlBoxStatus
    {
    };
    
    /// 射流灭火装置区域控制箱参数
    struct JettingHydrantDomainCtrlBoxParameters
    {
    };

    /// 射流灭火装置解码器状态定义
    struct JettingHydrantDecoderStatus
    {
        bool debug : 1;         ///< 调试
        bool alarm : 1;         ///< 报警
        bool fault : 1;         ///< 故障
        bool disable : 1;       ///< 屏蔽
        bool preAlarm : 1;      ///< 预警
        bool locating : 1;      ///< 正在定位
        bool locateSuccess : 1; ///< 定位成功
        bool locateCancel : 1;  ///< 定位取消
        bool openLimit : 1;     ///< 阀开极限
        bool closeLimit : 1;    ///< 阀关极限
        bool moving : 1;        ///< 炮管运动
        bool leftLimit : 1;     ///< 炮管左极限
        bool rightLimit : 1;    ///< 炮管右极限
        bool upLimit : 1;       ///< 炮管上极限
        bool downLimit : 1;     ///< 炮管下极限
        unsigned char fogPole : 1; ///< 柱雾，0-柱状 1-雾状
        bool waterStream : 1;   ///< 有水流
        bool remoteScene : 1;   ///< 现场远程，0-现场 1-远程
        bool contractLimit : 1; ///< 收缩极限
        bool expandLimit : 1;   ///< 展开极限
        bool contracting : 1;   ///< 正在收缩
        bool expanding : 1;     ///< 正在展开
        bool liftDead : 1;      ///< 升降装置卡死

        bool inputRelay1 : 1;   ///< 输入继电器1状态（闭合：true，打开：false）
        bool inputRelay2 : 1;   ///< 输入继电器2状态（闭合：true，打开：false）

        bool __bitpadding : 7;  ///< 填充位

        JettingHydrantDecoderStatus()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 射流灭火装置解码器参数
    struct JettingHydrantDecoderParameters
    {
    };
#pragma pack(pop)

}

#endif