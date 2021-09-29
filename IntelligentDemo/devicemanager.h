#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include "publictype.h"

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceManager(QObject* parent = 0);
    ~DeviceManager();

public:
    /* 登录设备 */
    bool login();
    /* 登出设备 */
    bool logout();

    /* 获取IPC/NVR通道信息 */
    QList<NETDEMO_IPCNVR_CHANNEL_INFO_S> getIPCNVRChannelInfoList();
    /* 获取VMS组织信息 */
    QList<NETDEV_ORG_INFO_S> getVMSOrgInfoList();
    /* 获取VMS子设备信息 */
    QList<NETDEMO_VMS_SUB_DEVICE_INFO_S> getVMSSubDeviceInfoList();

    /* 获取设备人脸库能力集 */
    NETDEV_PERSON_LIB_CAP_LIST_S getetPersonLibCapacity();
    /* 查询人员库信息 */
    QList<NETDEV_LIB_INFO_S> getPersonLibInfo();
    /* 添加人脸库信息 */
    bool addFaceLibInfo(NETDEV_LIB_INFO_S& stPersonLibInfo);
    /* 修改人脸库信息 */
    bool modifyFaceLibInfo(NETDEV_PERSON_LIB_LIST_S& stPersonLibList);
    /* 删除人脸库信息 */
    bool deleteFaceLibInfo(UINT32 udwPersonLibID, NETDEV_DELETE_DB_FLAG_INFO_S& stFlagInfo);
    /* 查询人脸库下成员信息 */
    void getPersonInfo(UINT32 udwPersonLibID,
                       NETDEV_PERSON_QUERY_INFO_S& stQueryInfo,
                       NETDEV_BATCH_OPERATE_BASIC_S& stQueryResultInfo,
                       QList<NETDEV_PERSON_INFO_S>& faceLibMemberInfoList);
    /* 添加人员信息 */
    bool addPersonInfo(UINT32 udwPersonLibID,
                       NETDEV_PERSON_INFO_LIST_S& stPersonInfoList,
                       NETDEV_PERSON_RESULT_LIST_S& stPersonResultList);
    /* 修改人员信息 */
    bool modifyPersonInfo(UINT32 udwPersonLibID,
                          NETDEV_PERSON_INFO_LIST_S& stPersonInfoList,
                          NETDEV_PERSON_RESULT_LIST_S& stPersonResultList);
    /* 删除人员信息 */
    bool deletePersonInfo(UINT32 udwPersonLibID, UINT32 udwPersonID);
    /* 获取人脸布控任务信息 */
    bool getFaceMonitorTaskInfo(UINT32 dwChannelID,
                                NETDEV_MONITOR_QUERY_INFO_S& stQueryInfo,
                                QList<NETDEV_MONITION_INFO_S>& faceMonitorInfoList);
    /* 添加人脸布控任务信息 */
    bool addFaceMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo, NETDEV_MONITOR_RESULT_INFO_S& stMonitorResultInfo);
    /* 修改人脸布控任务信息 */
    bool modifyFaceMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo);
    /* 删除人脸布控任务信息 */
    bool deleteFaceMonitorTaskInfo(NETDEV_BATCH_OPERATOR_LIST_S& stBatchList);
    /* 人脸识别-人脸抓拍记录检索 */
    bool searchFaceRecordInfo(NETDEV_ALARM_LOG_COND_LIST_S stFindCond,
                              QList<NETDEV_FACE_RECORD_SNAPSHOT_INFO_S>& faceSearchRecordInfoList);
    /* 获取告警记录人脸全景图信息 */
    bool getFacePanoImageInfo(UINT32 recordID, UINT32 recordType, NETDEV_FILE_INFO_S& stFileInfo);

    /* 车牌识别-抓拍记录检索,注意，内存需要在外部释放 */
    bool searchPlateRecordInfo(NETDEV_ALARM_LOG_COND_LIST_S& stFindCond,
                               QList<NETDEV_VEHICLE_RECORD_INFO_S>& plateSearchRecordInfoList);
    /* 获取告警记录车牌全景图信息 */
    bool getPlatePanoImageInfo(UINT32 recordID, NETDEV_FILE_INFO_S& stFileInfo);
    /* 通过URL获取图片数据 */
    bool getImageDataByUrl(QString& url, int size, QByteArray& data);
    /* 查找车牌布控信息 */
    bool getVehicleMonitorTaskInfo(QList<NETDEV_MONITION_INFO_S>& m_vehicleMonitorTaskInfoList);
    /* 添加车辆布控任务信息 */
    bool addVehicleMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo);
    /* 修改车辆布控任务信息 */
    bool modifyVehicleMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo);
    /* 删除车辆布控任务信息 */
    bool deleteVehicleMonitorTaskInfo(NETDEV_BATCH_OPERATOR_LIST_S& stBatchList);
    /* 查询车辆库信息 */
    bool getVehicleLibInfo(QList<NETDEV_LIB_INFO_S>& stLibInfoList);
    /* 添加车辆库信息 */
    bool addVehicleLibInfo(NETDEV_LIB_INFO_S& stLibInfo);
    /* 修改车辆库信息 */
    bool modifyVehicleLibInfo(NETDEV_PERSON_LIB_LIST_S& stLibList);
    /* 删除车辆库信息 */
    bool deleteVehicleLibInfo(UINT32 udwLibID, NETDEV_DELETE_DB_FLAG_INFO_S& stFlagInfo);
    /* 查询车辆库下车辆信息 */
    bool getVehicleInfo(UINT32 udwVehicleLibID,
                        NETDEV_PERSON_QUERY_INFO_S& stQueryInfo,
                        NETDEV_BATCH_OPERATE_BASIC_S& stQueryResultInfo,
                        QList<NETDEV_VEHICLE_DETAIL_INFO_S>& vehicleLibMemberInfoList);
    /* 添加车辆信息 */
    bool addVehicleInfo(UINT32 udwVehicleLibID,
                        NETDEV_VEHICLE_INFO_LIST_S& stVehicleInfoList,
                        NETDEV_BATCH_OPERATOR_LIST_S& stResultList);
    /* 修改车辆信息 */
    bool modifyVehicleInfo(UINT32 udwVehicleLibID,
                           NETDEV_VEHICLE_INFO_LIST_S& stVehicleInfoList,
                           NETDEV_BATCH_OPERATOR_LIST_S& stResultList);
    /* 删除车辆信息 */
    bool deleteVehicleInfo(UINT32 udwVehicleLibID,
                           NETDEV_VEHICLE_INFO_LIST_S& stVehicleInfoList,
                           NETDEV_BATCH_OPERATOR_LIST_S& stResutList);

    /* 打开实况 */
    bool startRealPlay(NETDEV_PREVIEWINFO_S& stPreviewInfo);
    /* 关闭实况 */
    bool stopRealPlay(int dwChannelID);

    /* 订阅人脸识别比对告警 */
    bool subFaceRecognitionAlarm(NETDEV_PersonAlarmMessCallBack_PF cbAlarmPersonCallBack,
                                 NETDEV_AlarmMessCallBack_PF_V30 cbAlarmMessCallBack,
                                 LPVOID lpUserData);
    /* 取消订阅人脸识别比对告警 */
    bool unSubFaceRecognitionAlarm();
    /* 订阅人脸抓拍告警 */
    bool subFaceSnapshotAlarm(int dwChannelID,
                              NETDEV_StructAlarmMessCallBack_PF cbAlarmStructCallBack,
                              NETDEV_AlarmMessCallBack_PF_V30 cbAlarmMessCallBack,
                              LPVOID lpUserData);
    /* 取消订阅人脸抓拍告警 */
    bool unSubFaceSnapshotAlarm(int dwChannelID);

    /* 订阅车牌识别比对告警 */
    bool subPlateRecognitionAlarm(NETDEV_AlarmMessCallBack_PF_V30 cbAlarmMessCallBack,
                                  NETDEV_VehicleAlarmMessCallBack_PF cbVehicleAlarmMessCallBack,
                                  LPVOID lpUserData);
    /* 取消订阅车牌识别比对告警 */
    bool unSubPlateRecognitionAlarm();
    /* 订阅车牌抓拍告警 */
    bool subPlateSnapshotAlarm(int dwChannelID,
                               NETDEV_StructAlarmMessCallBack_PF cbAlarmStructCallBack,
                               NETDEV_AlarmMessCallBack_PF_V30 cbAlarmMessCallBack,
                               LPVOID lpUserData);
    /* 取消订阅车牌抓拍告警 */
    bool unSubPlateSnapshotAlarm(int dwChannelID);

private:
    /* 查询设备详细信息(组织，子设备，通道) */
    bool findDeviceDetailInfo();
    /* 根据通道ID获取通道信息 */
    bool getIPCNVRChannelInfoFromID(int channelID, NETDEMO_IPCNVR_CHANNEL_INFO_S** pChannelInfo);
    /* 根据通道ID获取通道信息 */
    bool getVMSChannelInfoFromID(int channelID, NETDEMO_VMS_CHANNEL_INFO_S** pChannelInfo);

public:
    QString m_strIpAddress;
    short     m_dwPort;
    QString m_strUserName;
    QString m_strPassword;
    NETDEMO_DEVICE_TYPE_E m_eDevType;
    LPVOID m_lpLoginHandle;

private:
    QList<NETDEMO_IPCNVR_CHANNEL_INFO_S> m_IPCNVRChannelInfoList;/* IPC/NVR通道信息 */
    QList<NETDEV_ORG_INFO_S> m_VMSOrgInfoList;/* 一体机组织信息 */
    QList<NETDEMO_VMS_SUB_DEVICE_INFO_S> m_VMSSubDeviceInfoList;/* 一体机子设备信息 */

    NETDEV_PERSON_LIB_CAP_LIST_S m_stCapacityList;/* 人脸库能力集信息 */

    INT32 m_dwPersonSubID;/* 人脸识别告警订阅ID-NVR/IPC使用 */
    INT32 m_dwStructSubID; /* 人脸抓拍告警订阅ID-NVR/IPC使用 */
    INT32 m_dwPlateRecogSubID;/* 车辆识别告警订阅ID-NVR/IPC使用 */
};

#endif // DEVICEMANAGER_H
