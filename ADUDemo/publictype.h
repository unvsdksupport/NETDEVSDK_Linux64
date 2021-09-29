#ifndef PUBLICTYPE_H
#define PUBLICTYPE_H
#include "../include/NetDEVSDK.h"
#include <QString>
#include <QList>
#include <QDebug>

/* 设备类型 */
typedef enum tagNETDEMODeviceType
{
    NETDEMO_DEVICE_TYPE_ENCODER         = 0,        /* 编码设备 */
    NETDEMO_DEVICE_TYPE_DECODER         = 1,        /* 解码设备 */

    NETDEMO_DEVICE_TYPE_INVALID         = 0xFFFF    /* 无效值  Invalid value */
}NETDEMO_DEVICE_TYPE_E;

/* 电视墙显示类型 */
typedef enum tagNETDEMOTVWallShowType
{
    NETDEMO_TVWALL_SHOW_TYPE_WINDOW         = 0,        /* 窗口 */
    NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED         = 1,        /* 虚拟LED */

    NETDEMO_TVWALL_SHOW_TYPE_INVALID         = 0xFFFF    /* 无效值  Invalid value */
}NETDEMO_TVWALL_SHOW_TYPE_E;

/* 视频源添加方式 */
typedef enum tagNETDEMOVideoSourceAddType
{
    NETDEMO_VIDEO_SOURCE_ADD_SDK         = 0,        /* SDK */
    NETDEMO_VIDEO_SOURCE_ADD_MANUAL         = 1,        /* 手动 */

    NETDEMO_VIDEO_SOURCE_ADD_INVALID         = 0xFFFF    /* 无效值  Invalid value */
}NETDEMO_VIDEO_SOURCE_ADD_TYPE_E;

/* 被动解码任务号 */
typedef struct tagNETDEMOPassiveTaskNOInfo
{
    LPVOID lpLoginHandle; /* 设备句柄 */
    UINT32 udwTaskNO;     /* 任务号 */
}NETDEMO_PASSIVE_TASKNO_INFO_S;

/* 电视墙列表QListWidget单元项数据 */
typedef struct tagNETDEMOTVWallItemData
{
    int dwDecoderIndex; /* 设备索引 */
    UINT32 udwTVWallID;     /* 电视墙ID */
}NETDEMO_TVWALL_ITEM_DATA_S;
Q_DECLARE_METATYPE(NETDEMO_TVWALL_ITEM_DATA_S)

/* 设备列表MTreeWidget单元项数据 */
typedef struct tagNETDEMODeviceItemData
{
    int dwDeviceIndex;     /* 设备索引 */
    UINT32 udwChannelID;    /* 通道ID */
}NETDEMO_DEVICE_ITEM_DATA_S;
Q_DECLARE_METATYPE(NETDEMO_DEVICE_ITEM_DATA_S)

/*设备登录信息*/
typedef struct tagNETDEMODeviceLoginInfo
{
    NETDEMO_DEVICE_TYPE_E eDeviceType;
    NETDEMO_VIDEO_SOURCE_ADD_TYPE_E eVideoSourceAddType;
    QString strIpAddress;
    short     dwPort;
    bool bIsAuth;
    QString strUserName;
    QString strPassword;
    QString strRtspAddress;
}NETDEMO_DEVICE_LOGIN_INFO_S;

/*电视墙显示单元信息*/
typedef struct tagNETDEMOTVWallDisplayUnitInfo
{
    bool bIsBind; /*是否绑定视频输出通道*/
    int dwRowIndex;/* 行列分布行索引 */
    int dwColumnIndex;/*行列分布列索引*/
    int dwScreenIDIndex;
    int startX;
    int startY;
    int width;
    int height;
    QString strDescText;
}NETDEMO_TVWALL_DISPLAY_UNIT_INFO_S;

/* 屏幕格式输出制式转换 */
typedef struct tagNETDEMOFormatMapping
{
    INT32 dwFormat;                       /* 屏幕输出制式 NETDEV_XW_SCREAN_FORMAT_E */
    QString  strFormat;                      /* 屏幕输出制式 */
}NETDEMO_FORMAT_MAPPING_S;

/* 屏幕输出制式分辨率 */
static NETDEMO_FORMAT_MAPPING_S gaFormat[] = \
{
    {NETDEV_VIDEO_FORMAT_720P24, "1280x720@24"},\
    {NETDEV_VIDEO_FORMAT_720P25, "1280x720@25"},\
    {NETDEV_VIDEO_FORMAT_720P30, "1280x720@30"},\
    {NETDEV_VIDEO_FORMAT_720P50, "1280x720@50"},\
    {NETDEV_VIDEO_FORMAT_720P60, "1280x720@60"},\
    {NETDEV_VIDEO_FORMAT_1080P24, "1920x1080@24"},\
    {NETDEV_VIDEO_FORMAT_1080P25, "1920x1080@25"},\
    {NETDEV_VIDEO_FORMAT_1080P30, "1920x1080@30"},\
    {NETDEV_VIDEO_FORMAT_1080P50, "1920x1080@50"},\
    {NETDEV_VIDEO_FORMAT_1080P60, "1920x1080@60"},\
    {NETDEV_VIDEO_FORMAT_XGA60, "1024x768@60"},\
    {NETDEV_VIDEO_FORMAT_SXGA60, "1280x1024@60"},\
    {NETDEV_VIDEO_FORMAT_UXGA60, "1600x1200@60"},\
    {NETDEV_VIDEO_FORMAT_WXGAP60HZ, "1366x768@60"},\
    {NETDEV_VIDEO_FORMAT_WSXGA60HZ, "1440x900@60"},\
    {NETDEV_VIDEO_FORMAT_4K30, "3840x2160@30"},\
    {NETDEV_VIDEO_FORMAT_4K60, "3840x2160@60"},\
    {NETDEV_VIDEO_FORMAT_MUXGA60HZ, "1920x1200@60"}\
};

/* 虚拟LED字体间距界面选择与SDK枚举值转换 */
typedef struct tagNETDEMOLEDFontGapMapping
{
    INT32 dwFontGapIndex;                       /* 界面下拉框索引 */
    NETDEV_XW_LED_FONT_GAP_E  eFontGap;         /* SDK枚举值 */
    int dwRate;      /* 对应的4分之n比例系数 */
}NETDEMO_LED_FONT_GAP_MAPPING_S;

/* 虚拟LED字体间距 */
static NETDEMO_LED_FONT_GAP_MAPPING_S gaFontGapConvert[] = \
{
    {0, NETDEV_LED_FONT_GAP_INVALID, 0},\
    {1, NETDEV_LED_FONT_GAP_NORMAL, 0},\
    {2, NETDEV_LED_FONT_GAP_FIVE, 1},\
    {3, NETDEV_LED_FONT_GAP_ONE, 2},\
    {4, NETDEV_LED_FONT_GAP_SIX, 3},\
    {5, NETDEV_LED_FONT_GAP_TWO, 4},\
    {6, NETDEV_LED_FONT_GAP_SEVEN, 5},\
    {7, NETDEV_LED_FONT_GAP_THREE, 6},\
    {8, NETDEV_LED_FONT_GAP_EIGHT, 7},\
    {9, NETDEV_LED_FONT_GAP_FOUR, 8},\
    {10, NETDEV_LED_FONT_GAP_NINE, 10}
};

/* 解码器信息 */
typedef struct tagNETDEMODecoderInfo
{
    LPVOID lpLoginHandle;
    NETDEMO_DEVICE_LOGIN_INFO_S stDeviceLoginInfo;
    NETDEV_DEVICE_BASICINFO_S stDeviceBaseInfo;
    NETDEV_XW_CAP_INFO_S stCapInfo;
    NETDEV_XW_CHANNELS_LIST_S stChannelsList;
    QList<NETDEV_XW_TVWALL_CFG_S> stTVWallCfgList;
}NETDEMO_DECODER_INFO_S;

/* 编码设备信息 */
typedef struct tagNETDEMOEncoderInfo
{
    LPVOID lpLoginHandle;
    NETDEMO_DEVICE_LOGIN_INFO_S stDeviceLoginInfo;
    NETDEV_DEVICE_BASICINFO_S stDeviceBaseInfo;
    QList<NETDEV_VIDEO_CHL_DETAIL_INFO_EX_S> stVideoChlInfoList;
}NETDEMO_ENCODER_INFO_S;

extern QList<NETDEMO_DECODER_INFO_S> gDecoderList;
extern QList<NETDEMO_ENCODER_INFO_S> gEncoderList;

#endif // PUBLICTYPE_H
