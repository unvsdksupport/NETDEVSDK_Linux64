#ifndef SDKHEADER_H
#define SDKHEADER_H
#include <QString>
#include <QList>
#include <QByteArray>

/*
 * 文件信息
*/
struct NETDEMO_FILE_INFO_S
{
    QString   szName;   /* 文件名称 范围[1, 16]*/
    unsigned int udwSize;                 /* 文件大小[pcdata或通过szurl获取到的图片大小] 单位:字节 范围:[0, 1M(1048576)] */
    unsigned int dwFileType;              /* 文件类型，详见枚举值：NETDEV_FILE_TYPE_E */
    unsigned int udwLastChange;           /* 最后修改时间，UTC时间，单位为s */
    QByteArray   pcData;                 /* 文件数据 Base64 需根据udwSize 动态申请内存*/
    QString   szUrl;   /* 图片URL，长度范围[0,256] */
};

/*
 * 车牌告警布控信息
*/
struct NETDEMO_MONITOR_ALARM_INFO_S
{
    unsigned int udwMonitorReason;                          /* 布控原因类型 */
    unsigned int udwMonitorAlarmType;                       /* 布控告警类型 0：匹配告警 1：不匹配告警 */
    unsigned int udwMemberID;                               /* 车辆成员ID */
};

/*
 * 车牌信息
*/
struct NETDEMO_PLATE_ATTR_INFO_S
{
    QString     szPlateNo;                        /* 车牌号 */
    unsigned int   udwColor;                                        /* 车牌颜色 参见NETDEV_PLATE_COLOR_E */
    unsigned int   udwType;                                         /* 车牌类型，参见NETDEV_PLATE_TYPE_E */
};

/*
 * 车辆属性信息
 */
struct NETDEMO_VEH_ATTR_S
{
    unsigned int udwType;                                       /* 车辆类型 详见 NETDEV_VEHICLE_TYPE_E */
    unsigned int udwColor;                                      /* 车身颜色 详见 NETDEV_PLATE_COLOR_E */
    unsigned int udwSpeedUnit;                                  /* 车辆速度单位 0：公里/每小时 1：英里/每小时 */
    float  fSpeedValue;                                   /* 车辆速度 */
    unsigned int udwSpeedType;                                  /* 结构化场景中的机动车车辆速度类型 详见 NETDEV_SPEED_TYPE_E */
    QString   szVehicleBrand;                 /* 车辆车标编码（自行编码) */
    unsigned int udwImageDirection;                             /* 结构化场景中的机动车在画面坐标系中的行驶方向 详见 NETDEV_IMAGE_DIRECTION_E */
};

/*
 * 车辆识别记录信息
*/
struct NETDEMO_VEHICLE_RECORD_INFO_S
{
    unsigned int   udwRecordID;                                       /* 车辆识别记录ID */
    unsigned int   udwChannelID;                                      /* 通道ID，抓拍推送时有效 */
    unsigned int   udwPassingTime;                                    /* 过车时间，UTC格式，单位秒*/
    QString     szChannelName;                     /* 卡口相机名称 */
    NETDEMO_PLATE_ATTR_INFO_S    stPlateAttr;                    /* 车牌抓拍信息 */
    NETDEMO_VEH_ATTR_S           stVehAttr;                      /* 车辆抓拍信息 */
    NETDEMO_FILE_INFO_S          stPlateImage;                   /* 车牌抓拍图片 图片加密后大小不超过1M*/
    NETDEMO_FILE_INFO_S          stVehicleImage;                 /* 车辆抓拍图片 结构化查询时携带 图片加密后大小不超过1M*/
    NETDEMO_FILE_INFO_S          stPanoImage;                    /* 全景图 结构化查询时携带 仅携带图片URL和size，图片数据需要通过/LAPI/V1.0/System/Picture接口获取*/
    NETDEMO_MONITOR_ALARM_INFO_S stMonitorAlarmInfo;             /* 车牌告警布控信息 */
};

/*
 * 车辆比对报警信息
*/
struct NETDEMO_VEHICLE_EVENT_INFO_S
{
    unsigned int udwID;                                              /* 通知记录ID */
    unsigned int udwTimestamp;                                       /* 通知上报时间，UTC格式，单位秒 */
    unsigned int udwNotificationType;                                /* 通知类型 详见 NETDEV_NOTIFICATION_TYPE_E*/
    unsigned int udwVehicleInfoNum;                                  /* 车辆信息数目 [0, 1] */
    QList<NETDEMO_VEHICLE_RECORD_INFO_S> stVehicleRecordInfoList;       /* 车辆信息列表 */
};

/**
* @struct tagstNETDEVVehRecognitionEvent
* @brief 车辆识别事件
* @attention
*/
struct NETDEMO_VEH_RECOGNITION_EVENT_S
{
    QString   szReference;                        /* 订阅者描述信息 */
    unsigned int udwSrcID;                                           /* 告警源ID */
    NETDEMO_VEHICLE_EVENT_INFO_S stVehicleEventInfo;            /* 车辆比对报警信息 需动态申请内存 */
};

/**
* @struct tagNETDEVRegionInfo
* @brief 成员地区信息
* @attention 无 None
*/
struct NETDEMO_REGION_INFO_S
{
    QString szNation;          /* 国籍，范围[1-63] */
    QString szProvince;        /* 省份，范围[1-63] */
    QString szCity;            /* 城市，范围[1-63] */
};

/**
* @enum tagNETDEVPersonTimeTemplateInfo
* @brief 时间模板相关信息
* @attention 无 None
*/
struct NETDEMO_PERSON_TIME_TEMPLATE_INFO_S
{
    unsigned int udwBeginTime;    /* 时间模板生效起始时间 若未配置，填写0 */
    unsigned int udwEndTime;      /* 时间模板生效结束时间 若未配置，填写4294967295(0xFFFFFFFF)*/
    unsigned int udwIndex;        /* 时间模板索引 */
};

/**
* @struct tagNETDEVIdentificationInfo
* @brief 成员证件信息
* @attention 无 None
*/
struct NETDEMO_IDENTIFICATION_INFO_S
{
    unsigned int udwType;                     /* 证件类型 详情参见枚举 NETDEV_ID_TYPE_E*/
    QString   szNumber;     /* 证件号，范围:[1, 20] */
};

/**
* @struct tagNETDEVImageInfo
* @brief 人脸图片信息列表
* @attention 无 None
*/
typedef struct NETDEMO_IMAGE_INFO_S
{
    unsigned int udwFaceID;                   /* 人脸照片ID */
    NETDEMO_FILE_INFO_S stFileInfo;      /* 照片信息 */
};

/**
 * @struct tagNETDEVCustomValue
 * @brief 自定义属性信息
 * @attention 无 None
 */
typedef struct NETDEMO_CUSTOM_VALUE_S
{
    unsigned int  udwID;                                         /* 自定义属性名称序号 从0开始*/
    QString    szValue;        /* 自定义属性值 范围[1,63]*/
    unsigned int  udwModelStatus;                                /* 建模状态，即将人脸图片转为半结构化数据的状态。0：未建模 1：已建模 2：建模失败 */
};

/**
 * @struct tagNETDEVStaffInfo
 * @brief 员工信息
 * @attention 无 None
 */
typedef struct NETDEMO_STAFF_INFO_S
{
    QString   szNumber;                       /* 人员编号 字符串长度范围[1, 16] */
    QString   szBirthday;                     /* 出生日期 字符串长度范围[1,31] */
    unsigned int udwDeptID;                                     /* 部门ID */
    QString   szDeptName;                    /* 部门名称 添加时可不携带 字符串长度范围[1, 64] */
};

/**
 * @struct tagNETDEVVisitorInfo
 * @brief 访客信息
 * @attention 无 None
 */
struct NETDEMO_VISITOR_INFO_S
{
    unsigned int udwVisitorCount;                               /* 访客人数 */
    QString   szCompany;                     /* 访客单位 字符串长度范围[1, 64] */
    unsigned int udwIntervieweeID;                              /* 被访者ID */
};

/**
* @struct tagNETDEVPersonInfo
* @brief 人员信息
* @attention 无 None
*/
struct NETDEMO_PERSON_INFO_S
{
    unsigned int                                  udwPersonID;                         /* 人员ID */
    unsigned int                                  udwLastChange;                       /* 人员信息最后修改时间 */
    QString                                    szPersonName;        /* 人员名字 范围:[1, 63] */
    unsigned int                                  udwGender;                           /* 成员性别 详情参见枚举NETDEV_GENDER_TYPE_E*/
    QString                                    szBirthday;           /* 成员出生日期，格式YYYYMMDD，范围[1,31] */
    NETDEMO_REGION_INFO_S                    stRegionInfo;                        /* 成员地区信息 */
    unsigned int                                  udwTimeTemplateNum;                  /* 时间模板数量 */
    QList<NETDEMO_PERSON_TIME_TEMPLATE_INFO_S>    stTimeTemplateList;                 /* 时间模板相关信息 需动态分配 */
    unsigned int                                  udwIdentificationNum;                /* 证件信息个数 范围:[0, 6]*/
    NETDEMO_IDENTIFICATION_INFO_S            stIdentificationInfo;  /* 成员证件信息 */
    unsigned int                                  udwImageNum;                         /* 人脸图片个数 范围:[0, 6] */
    NETDEMO_IMAGE_INFO_S                     stImageInfo;           /* 人脸图片信息列表 */
    unsigned int                                  udwReqSeq;                           /* 请求数据序列号，此字段会在返回结果中待会，仅在批量添加中携带该字段 */
    bool                                    bIsMonitored;                        /* 是否已布控，获取时必带，仅VMS支持 */
    unsigned int                                  udwBelongLibNum;                     /* 所属人员库数量，仅VMS支持 */
    unsigned int                                 *pudwBelongLibList;                   /* 所属人员库ID列表，需动态分配内存，仅VMS支持 */
    unsigned int                                  udwCustomNum;                        /* 自定义属性值数量，最多5个，仅VMS支持 */
    QList<NETDEMO_CUSTOM_VALUE_S>                 stCustomValueList;                  /* 自定义属性值列表，需动态分配内存，当Num为0时可以不填 */
    QString                                    szTelephone;          /* 联系电话 字符串长度[1,64] */
    QString                                    szAddress;           /* 地址 字符串长度范围[1, 64] */
    unsigned int                                  udwCardNum;                          /* 门禁卡个数 取值范围[0,6],Get时携带 */
    unsigned int                                  udwFingerprintNum;                   /* 指纹个数，取值范围[0,10] */
    unsigned int                                  udwType;                             /* 人员类型 0：员工 1：访客 2：陌生人 */
    NETDEMO_STAFF_INFO_S                     stStaff;                             /* 员工信息 Type为员工时必填 */
    NETDEMO_VISITOR_INFO_S                   stVisitor;                           /* 访客信息 Type为访客时必填 */
    QString                                    szDesc;              /* 备注信息 字符串长度范围[1, 128] */
};

/**
 * @brief 人脸位置信息
 * @attention
 */
struct NETDEMO_FACE_POSITION_INFO_S
{
    int   dwTopLeftX;           /* 左上角X [0, 10000]  Upper left corner X [0, 10000]  */
    int   dwTopLeftY;           /* 左上角Y [0, 10000]  Upper left corner Y [0, 10000]  */
    int   dwBottomRightX;       /* 右下角X [0, 10000]  Lower right corner x [0, 10000] */
    int   dwBottomRightY;       /* 右下角Y [0, 10000]  Lower right corner y [0, 10000] */
};

/**
* @struct tagNETDEVFeatureInfo
* @brief 半结构化特征信息
* @attention 无 None
*/
struct NETDEMO_FEATURE_INFO_S
{
    QString    szFeatureVersion;        /* 人脸半结构化特征提取算法版本号 [0, 20] */
    QString    szFeature;    /* 基于人脸提取出来的特征信息 目前加密前512B */
};

/**
* @struct tagNETDEVPersonCompareInfo
* @brief 人脸 比对信息
* @attention 无 None
*/
struct NETDEMO_PERSON_COMPARE_INFO_S
{
    unsigned int                      udwSimilarity;                                  /* 相似度 */
    NETDEMO_PERSON_INFO_S        stPersonInfo;                                   /* 人脸库成员信息 */
    NETDEMO_FILE_INFO_S          stPanoImage;                                    /* 人脸大图  */
    NETDEMO_FILE_INFO_S          stFaceImage;                                    /* 人脸小图 */
    NETDEMO_FACE_POSITION_INFO_S stFaceArea;                                     /* 人脸区域坐标 */
    unsigned int                      udwCapSrc;                                      /* 采集来源 */
    unsigned int                      udwFeatureNum;                                  /* 半结构化特征数目 如果没有半结构化特征，可不带相关字段 PTS必带 */
    QList<NETDEMO_FEATURE_INFO_S>     stFeatureInfoList;                                 /* 半结构化特征列表 如果没有半结构化特征，可不带相关字段  PTS必带 */
};

/**
* @struct tagNETDEVFacePassRecordInfo
* @brief 人脸 通过记录信息
* @attention 无 None
*/
struct NETDEMO_FACE_PASS_RECORD_INFO_S
{
    unsigned int                              udwRecordID;                        /* 人脸通行记录ID */
    unsigned int                              udwType;                            /* 人脸通行类型，参见枚举NETDEV_FACE_PASS_RECORD_TYPE_E */
    long long                               tPassingTime;                       /* 过人时间，UTC格式，单位秒 */
    unsigned int                              udwChannelID;                       /* 通道ID */
    QString                                szChlName;          /* 抓拍通道名称，范围[1,63] */
    NETDEMO_PERSON_COMPARE_INFO_S        stCompareInfo;                      /* 比对信息 */
};

/**
* @struct tagNETDEVPersonEventInfo
* @brief 人员报警信息
* @attention 无 None
*/
struct NETDEMO_PERSON_EVENT_INFO_S
{
    unsigned int                          udwID;                                   /* 通知记录ID */
    unsigned int                          udwTimestamp;                            /* 通知上报时间 UTC格式，单位秒*/
    unsigned int                          udwNotificationType;                     /* 通知类型 0：实时通知1：历史通知 */
    unsigned int                          udwFaceInfoNum;                          /* 人脸信息数目 范围：[0, 1] */
    NETDEMO_FACE_PASS_RECORD_INFO_S  stCtrlFaceInfo[2];            /* 人脸信息列表，当采集信息没有人脸时，可不带FaceInfo相关字段 */
    unsigned int                          udwFinishFaceNum;                        /* 人脸结束数量 范围：[0, 40] */
    unsigned int                          audwFinishFaceList[40];       /* 人脸结束列表 */
};

#endif // SDKHEADER_H
