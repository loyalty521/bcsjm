////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2013, �Ϸʿƴ�������ȫ�����ɷ����޹�˾
/// All rights reserved.
///
/// @file NcpDevices.h
/// @brief NcpЭ���豸��ؽṹ�Ͳ����Ķ����ļ�
///
/// ����NcpЭ���豸��ؽṹ�Ͳ���
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

#ifndef _NCP_NCPDEVICES_H
#define _NCP_NCPDEVICES_H

#include <string>

namespace Ncp
{
#pragma pack(push)
#pragma pack(1)
    /// ״̬
    struct VideoFireDetectorStatus
    {
        bool debug : 1;                 ///< ����
        bool alarm : 1;                 ///< ����
        bool fault : 1;                 ///< ����
        bool disable : 1;               ///< ����
        bool preAlarm : 1;              ///< Ԥ��
        bool flameAlarm : 1;            ///< ���汨��
        bool smokeAlarm : 1;            ///< ������
        bool tempAlarm : 1;             ///< �¶ȱ���
        bool videoChannel1NoInput : 1;  ///< ͨ��1��Ƶȱʧ
        bool videoChannel2NoInput : 1;  ///< ͨ��2��Ƶȱʧ
        bool videoQualityPoor : 1;      ///< ��Ƶ����̫��
        bool noSenders : 1;             ///< δ���÷�����
        bool senderShield : 1;          ///< �������ڵ�

        bool inputRelay1 : 1;           ///< ����̵���1״̬���պϣ�true���򿪣�false��
        bool inputRelay2 : 1;           ///< ����̵���2״̬���պϣ�true���򿪣�false��

        bool __bitpadding : 1;          ///< ���λ
        char __bytepadding[2];          ///< ����ֽ�

        VideoFireDetectorStatus()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// ����
    struct VideoFireDetectorParameters
    {
        bool osdLogo : 8;       ///< �Ƿ��������Ƶ�е��ӹ�˾LOGO
        bool osdDateTime : 8;   ///< �Ƿ��������Ƶ�е�������ʱ��
        bool osdLocation : 8;   ///< �Ƿ��������Ƶ�е�������λ��
        bool osdAlarm : 8;      ///< �Ƿ��������Ƶ�е��ӱ�����Ϣ
        unsigned int h264Bitrate;   ///< H264Ŀ�����ʣ�[1,000,000-10,000,000]��1M-10Mbps��
        unsigned char videoDisplayChannel;  ///< ��Ƶ��ʾͨ����0-�����ģ����Ƶ��1-���ͨ��1��Ƶ��2-���ͨ��2��Ƶ��

        VideoFireDetectorParameters()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// �㷨���ѯ�㷨�б�ʱ���ص��㷨��Ϣ��
    struct VideoFireDetectorAlgItemRead
    {
        std::string     name;                   ///< �㷨���ƣ�UTF-8���룩
        Version         version;                ///< �㷨�汾
        unsigned char   videoChannel = 0;       ///< ʹ�õ���Ƶͨ����0-�����ã�1-ʹ��ͨ��1��Ƶ��2-ʹ��ͨ��2��Ƶ��
        unsigned char   sensibility = 0;        ///< ��ǰ�����ȣ�ȡֵ��Χ[0������������]������0Ϊ�Զ���������
        unsigned char   sensibilityUBound = 0;  ///< ����������
        unsigned char   sensibilityDefault = 0; ///< ȱʡ������
        std::string     description;            ///< �㷨������UTF-8���룩
    };

    /// �㷨������㷨�б�ʱʹ�ã�
    struct VideoFireDetectorAlgItemWrite
    {
        std::string     name;               ///< �㷨���ƣ�UTF-8���룩
        unsigned char   videoChannel = 0;   ///< ��Ƶͨ����0-�����ã�1-ʹ��ͨ��1��Ƶ��2-ʹ��ͨ��2��Ƶ��
        unsigned char   sensibility = 0;    ///< �����ȣ�ȡֵ��Χ[0������������]������0Ϊ�Զ���������
    };

    /// ״̬
    struct VideoSFCompositeFireDetectorParameters : VideoFireDetectorParameters
    {
        unsigned char algParallel = 0;  ///< �㷨����ģʽ��0-ͬʱ���У�1-��֡�л���2-�����л���
        unsigned char alarmMode = 0;    ///< ����ģʽ��0-��һ���������� 1-���߾�������������
    };

    /// �Զ������ڽ�����״̬
    struct HydrantDecoderStatus
    {
        bool debug : 1;                 ///< ����
        bool alarm : 1;                 ///< ����
        bool fault : 1;                 ///< ����
        bool disable : 1;               ///< ����
        bool preAlarm : 1;              ///< Ԥ��
        bool targeting : 1;             ///< ���ڶ�λ
        bool targetSuccess : 1;         ///< ��λ�ɹ�
        bool targetFail : 1;            ///< ��λʧ��

        bool leftLimit : 1;             ///< ����
        bool rightLimit : 1;            ///< �Ҽ���
        bool upLimit : 1;               ///< �ϼ���
        bool downLimit : 1;             ///< �¼���
        bool leftRightMoving : 1;       ///< �����˶�
        bool upDownMoving : 1;          ///< �����˶�
        bool poleLimit : 1;             ///< ������
        bool fogLimit : 1;              ///< ����

        bool waterStream : 1;           ///< ����ˮ��
        bool sceneOrRemote : 1;         ///< �ֳ�Զ�̣�0=�ֳ���1=Զ��
        bool liftFoldLimit : 1;         ///< ����װ����������
        bool liftUnfoldLimit : 1;       ///< ����װ��չ������
        bool liftFolding : 1;           ///< ����װ����������
        bool liftUnfolding : 1;         ///< ����װ������չ��
        bool mode : 1;                  ///< ����ģʽ��0=�����ڹ���ģʽ��1=̽��������ģʽ
        bool poleFogMoving : 1;         ///< ��������

        bool valveOpenLimit : 1;        ///< ��������
        bool valveCloseLimit : 1;       ///< ���ؼ���
        bool valveOpening : 1;          ///< ��������
        bool valveClosing : 1;          ///< ��������
        bool signalValveOpenLimit : 1;  ///< �źŷ�������
        bool signalValveCloseLimit : 1; ///< �źŷ��ؼ���
		bool signalValveOpening : 1;	///< �źŷ�������
		bool signalValveClosing : 1;	///< �źŷ�������

        bool valveType : 1;             ///< �����ͣ�0=�綯����1=��ŷ�

        uint8_t __bitpadding1 : 7;      ///< ���λ
        uint8_t __bytepadding1;         ///< ����ֽ�

        bool videoQualityPoor : 1;      ///< ��Ƶ������
        bool colorVideoLost : 1;        ///< ��ɫ��Ƶ��ʧ
		bool irVideoLost : 1;			///< ������Ƶ��ʧ
        bool liftFault : 1;             ///< ����װ�ù���
        bool valveFault : 1;            ///< ������
		bool hydrantFault : 1;          ///< ���װ�ù��ϣ�ָ��е���ϣ�
		bool ncpconnected : 1;          ///<NCP������
		bool AbnormalAngle : 1;         ///< �Ƕ��쳣

		bool AngleFault : 1;            ///< �ǶȰ����
		bool MotorFault : 1;            ///< �ڵ������
		bool DriverCanFault : 1;        ///< ������ͨ�Ź���
		uint8_t __bitpaddign2 : 5;      ///< ���λ

        uint16_t horizontalAngle;        ///< ˮƽ�Ƕ�
        uint16_t verticalAngle;         ///< ��ֱ�Ƕ�
        uint16_t poleFogTicks;          ///< ����̶�

		uint16_t horizonalAngle;        ///< ����̶�		

		bool swinging : 1;				///< ���ڰڶ�
		bool signalValve : 1;           ///< ���źŷ�
		bool hasLift : 1;               ///< ������װ��
		uint8_t __bitpadding3 : 5;      ///< ���λ

		HydrantDecoderStatus() ///20150508
		{
			memset(this, 0, sizeof(*this));
		}
    };

    /// �Զ������ڽ���������
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


        bool osdLogo : 8;       ///< �Ƿ��������Ƶ�е��ӹ�˾LOGO
        bool osdDateTime : 8;   ///< �Ƿ��������Ƶ�е�������ʱ��
        bool osdLocation : 8;   ///< �Ƿ��������Ƶ�е�������λ��
        bool osdAlarm : 8;      ///< �Ƿ��������Ƶ�е��ӱ�����Ϣ
        uint32_t h264Bitrate;   ///< H264Ŀ�����ʣ�[1,000,000-10,000,000]��ȱʡ1500000��1M-10Mbps��

        uint16_t modeSwitchTime;            ///< ����ģʽ�Զ��л�ʱ��,0=���Զ��л�,[0, 3600]����λ�룩
		uint16_t swingTime;                 ///< �ڶ�ʱ��,[0, 600]����λ�룩
        uint16_t hasLift;                   ///< 1=��������װ�ã�0=������װ��
        uint16_t hasDetector;               ///< 1=����̽�⹦�ܣ�0=��̽�⹦��

        float setupHeight;                  ///< ��װ�߶ȣ���λ�ף�
        float horizontalAngleV;              ///< �ڹ�ˮƽʱ�Ĵ�ֱ�Ƕ�
        float horizontalFiringRange;         ///< ˮƽ��̣���λ�ף�
        float locatorAngle;                 ///< ��λ���н�

        uint16_t horizontalTargetPixel;      ///< ˮƽ��������
        uint16_t verticalTargetPixel;       ///< ��ֱ��������
        uint32_t targetError;               ///< ��λ�����ȣ�

        float horizontalSwingScope;          ///< ˮƽ�ڶ���Χ
        float verticalSwingStep;            ///< ��ֱ�ڶ�����
        float verticalSwingScope;           ///< ��ֱ�ڶ���Χ

        uint8_t  algSensibilityWhenScan;    ///< ɨ��ʱ�㷨������
        uint8_t  algSensibilityWhenDetect;  ///< ̽��ʱ�㷨������
        uint16_t canCommunicationTimeout;   ///< CANͨ�ų�ʱʱ��
        uint32_t canCommunicationTimeoutMaxTimes;   ///< CANͨ�����ʱ����

        float    autoFogConvertAngle;       ///< ����ת���Ƕ�
		uint16_t autoFogConvertEnable;      ///< ����ת��ʹ��
		uint16_t autoFogConvertTime;        ///< ����ת��ʱ�䣨���룩
        float returnPresetAngleH;           ///< ��λԤ��λˮƽ�Ƕ�
        float returnPresetAngleV;           ///< ��λԤ��λ��ֱ�Ƕ�
        float logicalLimitAngleH;           ///< �߼�����λˮƽ�Ƕ�
        float logicalLimitAngleV;           ///< �߼�����λ��ֱ�Ƕ�
        float detectPresetAngleH;           ///< ̽��Ԥ��λˮƽ�Ƕ�
        float detectPresetAngleV;           ///< ̽��Ԥ��λ��ֱ�Ƕ�
        float liftPresetAngleH;             ///< ����Ԥ��λˮƽ�Ƕȣ�����ʱ���ڹ�ˮƽ�Ƕȣ�
        float liftPresetAngleV;             ///< ����Ԥ��λ��ֱ�Ƕȣ�����ʱ���ڹܴ�ֱ�Ƕȣ�
        float liftPresetAngleErrorH;        ///< ����װ��Ԥ��λˮƽ�������
        float liftPresetAngleErrorV;        ///< ����װ��Ԥ��λ��ֱ�������

        float verticalScanStepAngle;        ///< ��ֱɨ�貽���Ƕ�
        float angleH;           ///< �˶�ˮƽ�Ƕ�
        float angleV;           ///< �˶���ֱ�Ƕ�
        bool osdTarget : 8;     ///< �Ƿ��������Ƶ�е��Ӱ���ͼ��
        bool osdAngle : 8;     ///< �Ƿ��������Ƶ�е���ʵʱ�Ƕ�
		uint16_t signalValve;  ///< 1=�����źŷ���0=���źŷ�
		float fogScope;         ///< ����Χ����λ�ף�

        /// �Զ������ڰڶ�ģʽ
        enum class SwingMode
        {
            HVStep = 0,   ///< ˮƽ��ֱ�𲽰ڶ��������Σ�
        };

        SwingMode swingMode;                ///< �ڶ�ģʽ	
    };


    /// �������װ�����������״̬
    struct JettingHydrantDomainCtrlBoxStatus
    {
    };
    
    /// �������װ��������������
    struct JettingHydrantDomainCtrlBoxParameters
    {
    };

    /// �������װ�ý�����״̬����
    struct JettingHydrantDecoderStatus
    {
        bool debug : 1;         ///< ����
        bool alarm : 1;         ///< ����
        bool fault : 1;         ///< ����
        bool disable : 1;       ///< ����
        bool preAlarm : 1;      ///< Ԥ��
        bool locating : 1;      ///< ���ڶ�λ
        bool locateSuccess : 1; ///< ��λ�ɹ�
        bool locateCancel : 1;  ///< ��λȡ��
        bool openLimit : 1;     ///< ��������
        bool closeLimit : 1;    ///< ���ؼ���
        bool moving : 1;        ///< �ڹ��˶�
        bool leftLimit : 1;     ///< �ڹ�����
        bool rightLimit : 1;    ///< �ڹ��Ҽ���
        bool upLimit : 1;       ///< �ڹ��ϼ���
        bool downLimit : 1;     ///< �ڹ��¼���
        unsigned char fogPole : 1; ///< ����0-��״ 1-��״
        bool waterStream : 1;   ///< ��ˮ��
        bool remoteScene : 1;   ///< �ֳ�Զ�̣�0-�ֳ� 1-Զ��
        bool contractLimit : 1; ///< ��������
        bool expandLimit : 1;   ///< չ������
        bool contracting : 1;   ///< ��������
        bool expanding : 1;     ///< ����չ��
        bool liftDead : 1;      ///< ����װ�ÿ���

        bool inputRelay1 : 1;   ///< ����̵���1״̬���պϣ�true���򿪣�false��
        bool inputRelay2 : 1;   ///< ����̵���2״̬���պϣ�true���򿪣�false��

        bool __bitpadding : 7;  ///< ���λ

        JettingHydrantDecoderStatus()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// �������װ�ý���������
    struct JettingHydrantDecoderParameters
    {
    };
#pragma pack(pop)

}

#endif