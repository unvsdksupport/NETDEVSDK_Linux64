#ifndef PUBLICTYPE_H
#define PUBLICTYPE_H
#include "../include/NetDEVSDK.h"
#include "sdkheader.h"
#include <QDebug>
#include "common.h"
#include <QMap>

/* 设备类型 */
typedef enum tagNETDEMODeviceType
{
    NETDEMO_DEVICE_IPC_OR_NVR               = 0,        /* IPC/NVR */
    NETDEMO_DEVICE_VMS                      = 1,        /* VMS */

    NETDEMO_DEVICE_TYPE_INVALID             = 0xFFFF    /* 无效值  Invalid value */
}NETDEMO_DEVICE_TYPE_E;

/* 设备树节点类型 */
typedef enum tagNETDEMODeviceTreeNodeType
{
    NETDEMO_DEVICE_TREE_NODE_DEVICE         = 0,        /* 顶层设备节点 */
    NETDEMO_DEVICE_TREE_NODE_ORG            = 1,        /* 组织节点 */
    NETDEMO_DEVICE_TREE_NODE_SUB_DEVICE     = 1,        /* 子设备节点 */
    NETDEMO_DEVICE_TREE_NODE_CHANNEL        = 1,        /* 通道节点 */

    NETDEMO_DEVICE_TREE_NODE_INVALID        = 0xFFFF    /* 无效值  Invalid value */
}NETDEMO_DEVICE_TREE_NODE_TYPE_E;

/* 人脸记录检索类型 */
typedef enum NETDEMOFindFaceAlarmRecordType
{
    NETDEMO_FIND_FACE_ALARM_RECORD_RECOGNITION = 0,     /* 人脸识别 */
    NETDEMO_FIND_FACE_ALARM_RECORD_SNAPSHOT = 1,        /* 人脸抓拍 */
}NETDEMO_FIND_FACE_ALARM_RECORD_TYPE_E;

/* 设备列表单元项数据 */
typedef struct tagNETDEMODeviceItemData
{
    NETDEMO_DEVICE_TYPE_E eDeviceType;                  /* 节点所属设备类型(IPC/NVR,VMS) */
    NETDEMO_DEVICE_TREE_NODE_TYPE_E eTreeNodeType;      /* 节点类型 */
    LPVOID lpLoginDevice;                               /* 根据登录句柄唯一标识一台设备，没有时为NULL */
    int dwOrgID;                                        /* 组织ID，一体机使用，没有时值为-1 */
    int dwSubDeviceID;                                  /* 子设备ID，一体机使用，没有时值为-1 */
    NETDEV_DEVICE_STATUS_E dwSubDeviceStatus;           /* 子设备在线状态，一体机使用 */
    int dwChannelID;                                    /* 通道ID, 没有时值为-1 */
    NETDEV_CHANNEL_STATUS_E eChannelStatus;             /* 通道在线状态 */
}NETDEMO_DEVICE_ITEM_DATA_S;
Q_DECLARE_METATYPE(NETDEMO_DEVICE_ITEM_DATA_S)

/* 库列表单元项数据(人脸库、车牌库) */
typedef struct tagNETDEMOLibItemData
{
    int dwLibID;                                        /* 库ID */
    QString libName;                                    /* 库名称 */
}NETDEMO_LIB_ITEM_DATA_S;
Q_DECLARE_METATYPE(NETDEMO_LIB_ITEM_DATA_S)

/* 车牌颜色枚举-字符串转换 */
static QMap<INT32, QString> gPlateColorMap = {
    {NETDEV_PLATE_COLOR_BLACK_E,"Black"},
    {NETDEV_PLATE_COLOR_WHITE_E,"White"},
    {NETDEV_PLATE_COLOR_GRAY_E,"Gray"},
    {NETDEV_PLATE_COLOR_RED_E,"Red"},
    {NETDEV_PLATE_COLOR_BLUE_E,"Blue"},
    {NETDEV_PLATE_COLOR_YELLOW_E,"Yellow"},
    {NETDEV_PLATE_COLOR_ORANGE_E,"Orange"},
    {NETDEV_PLATE_COLOR_BROWN_E,"Brown"},
    {NETDEV_PLATE_COLOR_GREEN_E,"Green"},
    {NETDEV_PLATE_COLOR_PURPLE_E,"Purple"},
    {NETDEV_PLATE_COLOR_CYAN_E,"Cyan"},
    {NETDEV_PLATE_COLOR_PINK_E,"Pink"},
    {NETDEV_PLATE_COLOR_TRANSPARENT_E,"Transparent"},
    {NETDEV_PLATE_COLOR_SILVERYWHITE_E,"Silvery White"},
    {NETDEV_PLATE_COLOR_DARK_E,"Dark"},
    {NETDEV_PLATE_COLOR_LIGHT_E,"Light"},
    {NETDEV_PLATE_COLOR_COLOURLESS,"No Color"},
    {NETDEV_PLATE_COLOR_YELLOWGREEN,"Yellow&Green"},
    {NETDEV_PLATE_COLOR_GRADUALGREEN,"Gradated Green"},
    {NETDEV_PLATE_COLOR_OTHER_E,"Other"}
};

/* 车牌类型枚举-字符串转换 */
static QMap<INT32, QString> gPlateTypeMap = {
    {NETDEV_PLATE_TYPE_BIG_CAR_E,"Large Vehicle Plate"},
    {NETDEV_PLATE_TYPE_MINI_CAR_E,"Small Vehicle Plate"},
    {NETDEV_PLATE_TYPE_EMBASSY_CAR_E,"Embassy Vehicle Plate"},
    {NETDEV_PLATE_TYPE_CONSULATE_CAR_E,"Consulate Vehicle Plate"},
    {NETDEV_PLATE_TYPE_OVERSEAS_CAR_E,"Overseas Vehicle Plate"},
    {NETDEV_PLATE_TYPE_FOREIGN_CAR_E,"Foreign Vehicle Plate"},
    {NETDEV_PLATE_TYPE_COMMON_MOTORBIKE_E,"Common Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_HANDINESS_MOTORBIKE_E,"Light Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_EMBASSY_MOTORBIKE_E,"Embassy Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_CONSULATE_MOTORBIKE_E,"Consulate Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_OVERSEAS_MOTORBIKE_E,"Overseas Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_FOREIGN_MOTORBIKE_E,"Foreign Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_LOW_SPEED_CAR_E,"Low Speed Vehicle Plate"},
    {NETDEV_PLATE_TYPE_TRACTOR_E,"Tractor Plate"},
    {NETDEV_PLATE_TYPE_TRAILER_E,"Trailer Plate"},
    {NETDEV_PLATE_TYPE_COACH_CAR_E,"Coach Vehicle Plate"},
    {NETDEV_PLATE_TYPE_COACH_MOTORBIKE_E,"Coach Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_TEMPORARY_ENTRY_CAR_E,"Temporary Entry Vehicle Plate"},
    {NETDEV_PLATE_TYPE_TEMPORARY_ENTRY_MOTORBIKE_E,"Temporary Entry Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_TEMPORARY_DRIVING_E,"Temporary Vehicle Plate"},
    {NETDEV_PLATE_TYPE_POLICE_CAR_E,"Police Vehicle Plate"},
    {NETDEV_PLATE_TYPE_POLICE_MOTORBIKE_E,"Police Motorcycle Plate"},
    {NETDEV_PLATE_TYPE_HONGKONG_ENTRY_EXIT_E,"Border Crossing Vehicle (Hong Kong} Plate"},
    {NETDEV_PLATE_TYPE_MACAO_ENTRY_EXIT_E,"Border Crossing Vehicle (Macau} Plate"},
    {NETDEV_PLATE_TYPE_ARMED_POLICE_E,"Armed Police Plate"},
    {NETDEV_PLATE_TYPE_ARMY_E,"Military Plate"},
    {NETDEV_PLATE_TYPE_OTHER_E,"Other"}
};

/* 车牌记录检索类型 */
typedef enum NETDEMOFindPlateAlarmRecordType
{
    NETDEMO_FIND_PLATE_ALARM_RECORD_RECOGNITION = 0,     /* 车牌识别 */
    NETDEMO_FIND_PLATE_ALARM_RECORD_SNAPSHOT = 1,        /* 车牌抓拍 */
}NETDEMO_FIND_PLATE_ALARM_RECORD_TYPE_E;

/* IPC/NVR通道信息 */

typedef struct tagNETDEMOIPCNVRChannelInfo
{
    LPVOID playHandle; /* 播放句柄 */
    NETDEV_VIDEO_CHL_DETAIL_INFO_EX_S stBaseInfo; /* 通道基本信息 */
}NETDEMO_IPCNVR_CHANNEL_INFO_S;

/* 一体机通道信息 */
typedef struct tagNETDEMOVMSChannelInfo
{
    LPVOID playHandle; /* 播放句柄 */
    int   dwFaceSnapshotSubID; /* 人脸抓拍订阅ID,默认值-1 */
    int   dwVehicleSnapshotSubID; /* 车牌抓拍订阅ID,默认值-1 */
    NETDEV_DEV_CHN_ENCODE_INFO_S stBaseInfo; /* 通道基本信息 */
}NETDEMO_VMS_CHANNEL_INFO_S;

/* 一体机子设备信息 */
typedef struct tagNETDEMOVMSSUbDeviceInfo
{
    NETDEV_DEV_BASIC_INFO_S stBaseInfo; /* 子设备信息 */
    QList<NETDEMO_VMS_CHANNEL_INFO_S> VMSChannelInfoList;/* 子设备下编码通道信息 */
}NETDEMO_VMS_SUB_DEVICE_INFO_S;

const int NETDEMO_FIND_LIB_MEM_COUNT = 6;
const int NETDEMO_FIND_SMART_ALARM_RECORD_COUNT = 16;
const int NETDEMO_SMALL_IMAGE_SIZE = 1024 * 1024;

#endif // PUBLICTYPE_H
