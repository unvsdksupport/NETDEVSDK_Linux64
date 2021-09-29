#include "devicemanager.h"

DeviceManager::DeviceManager(QObject* parent):
    QObject(parent)
{
    m_strIpAddress = "";
    m_dwPort = 80;
    m_strUserName = "admin";
    m_strPassword = "123456";
    m_eDevType = NETDEMO_DEVICE_IPC_OR_NVR;
    m_lpLoginHandle = NULL;

    memset(&m_stCapacityList,0,sizeof(m_stCapacityList));

    m_dwPersonSubID = -1;
    m_dwStructSubID = -1;
    m_dwPlateRecogSubID = -1;
}

DeviceManager::~DeviceManager()
{
    logout();
}

/******************* public *******************/
/* 登录 */
bool DeviceManager::login()
{
    NETDEV_DEVICE_LOGIN_INFO_S stDevLoginInfo = {0};
    NETDEV_SELOG_INFO_S stSELogInfo = {0};
    strncpy(stDevLoginInfo.szIPAddr,m_strIpAddress.toStdString().c_str(),sizeof(stDevLoginInfo.szIPAddr));
    strncpy(stDevLoginInfo.szUserName,m_strUserName.toStdString().c_str(),sizeof(stDevLoginInfo.szUserName));
    strncpy(stDevLoginInfo.szPassword,m_strPassword.toStdString().c_str(),sizeof(stDevLoginInfo.szPassword));
    stDevLoginInfo.dwPort = m_dwPort;

    if(NETDEMO_DEVICE_VMS == m_eDevType)
    {
        stDevLoginInfo.dwLoginProto = NETDEV_LOGIN_PROTO_PRIVATE;
    }
    else
    {
        stDevLoginInfo.dwLoginProto = NETDEV_LOGIN_PROTO_ONVIF;
    }

    m_lpLoginHandle = NETDEV_Login_V30(&stDevLoginInfo,&stSELogInfo);
    if(NULL == m_lpLoginHandle)
    {
        qCritical() << "Login Fail:"  << m_strIpAddress;
        return false;
    }

    qInfo() << "Login Success:"  << m_strIpAddress;

    findDeviceDetailInfo();

    BOOL bRet = NETDEV_GetPersonLibCapacity(m_lpLoginHandle,5,&m_stCapacityList);
    if(FALSE == bRet)
    {
        qCritical() << "NETDEV_GetPersonLibCapacity fail:" << NETDEV_GetLastError();
    }

    return true;
}

/* 登出设备 */
bool DeviceManager::logout()
{
    if(NULL != m_lpLoginHandle)
    {
        /* 关闭所有实况,取消告警订阅 */
        for(int i = 0; i < m_IPCNVRChannelInfoList.size(); i++)
        {
            stopRealPlay(m_IPCNVRChannelInfoList[i].stBaseInfo.dwChannelID);
            unSubFaceSnapshotAlarm(m_IPCNVRChannelInfoList[i].stBaseInfo.dwChannelID);
        }

        for(int i = 0; i < m_VMSSubDeviceInfoList.size(); i++)
        {
            for(int j = 0; j < m_VMSSubDeviceInfoList[i].VMSChannelInfoList.size(); j++)
            {
                stopRealPlay(m_VMSSubDeviceInfoList[i].VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.dwChannelID);
                unSubFaceSnapshotAlarm(m_VMSSubDeviceInfoList[i].VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.dwChannelID);
                unSubPlateSnapshotAlarm(m_VMSSubDeviceInfoList[i].VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.dwChannelID);
            }
        }

        unSubFaceRecognitionAlarm();
        unSubPlateRecognitionAlarm();

        if(TRUE != NETDEV_Logout(m_lpLoginHandle))
        {
            return false;
        }
    }

    return true;
}

/* 获取IPC/NVR通道信息 */
QList<NETDEMO_IPCNVR_CHANNEL_INFO_S> DeviceManager::getIPCNVRChannelInfoList()
{
    return m_IPCNVRChannelInfoList;
}

/* 获取VMS组织信息 */
QList<NETDEV_ORG_INFO_S> DeviceManager::getVMSOrgInfoList()
{
    return m_VMSOrgInfoList;
}

/* 获取VMS子设备信息 */
QList<NETDEMO_VMS_SUB_DEVICE_INFO_S> DeviceManager::getVMSSubDeviceInfoList()
{
    return m_VMSSubDeviceInfoList;
}

/* 获取设备人脸库能力集 */
NETDEV_PERSON_LIB_CAP_LIST_S DeviceManager::getetPersonLibCapacity()
{
    return m_stCapacityList;
}

/* 查询人员库信息 */
QList<NETDEV_LIB_INFO_S> DeviceManager::getPersonLibInfo()
{
    QList<NETDEV_LIB_INFO_S> stPersonLibInfoList;

    LPVOID lpFindPersonLibHandle = NETDEV_FindPersonLibList(m_lpLoginHandle);

    BOOL bRet = TRUE;
    while(NULL !=lpFindPersonLibHandle && TRUE == bRet)
    {
        NETDEV_LIB_INFO_S stPersonLibInfo = {0};
        bRet = NETDEV_FindNextPersonLibInfo(lpFindPersonLibHandle, &stPersonLibInfo);
        if(FALSE == bRet)
        {
            NETDEV_FindClosePersonLibList(lpFindPersonLibHandle);
            break;
        }

        stPersonLibInfoList.append(stPersonLibInfo);
    }

    return stPersonLibInfoList;
}

/* 查询人脸库下成员信息 */
void DeviceManager::getPersonInfo(UINT32 udwPersonLibID,
                                  NETDEV_PERSON_QUERY_INFO_S& stQueryInfo,
                                  NETDEV_BATCH_OPERATE_BASIC_S& stQueryResultInfo,
                                  QList<NETDEV_PERSON_INFO_S>& faceLibMemberInfoList)
{
    LPVOID lpFindPersonHandle = NETDEV_FindPersonInfoList(m_lpLoginHandle, udwPersonLibID, &stQueryInfo,&stQueryResultInfo);
    if(NULL == lpFindPersonHandle)
    {
        return;
    }

    BOOL bRet = TRUE;
    while(TRUE == bRet)
    {
        NETDEV_PERSON_INFO_S stPersonInfo = {0};
        for(int i = 0; i < NETDEV_LEN_8;i++)
        {
            stPersonInfo.stImageInfo[i].stFileInfo.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];/* 注意释放内存 */
            memset(stPersonInfo.stImageInfo[i].stFileInfo.pcData,0,NETDEMO_SMALL_IMAGE_SIZE);
            stPersonInfo.stImageInfo[i].stFileInfo.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
        }

        bRet = NETDEV_FindNextPersonInfo(lpFindPersonHandle, &stPersonInfo);
        if(FALSE == bRet)
        {
            for(int i = 0; i < NETDEV_LEN_8;i++)
            {
                delete[] stPersonInfo.stImageInfo[i].stFileInfo.pcData;
            }
            NETDEV_FindClosePersonInfoList(lpFindPersonHandle);
            break;
        }

        faceLibMemberInfoList.append(stPersonInfo);
    }
}

/* 添加人脸库信息 */
bool DeviceManager::addFaceLibInfo(NETDEV_LIB_INFO_S& stPersonLibInfo)
{
    UINT32 udwID = 0;
    BOOL bRet = NETDEV_CreatePersonLibInfo(m_lpLoginHandle, &stPersonLibInfo, &udwID);
    if(FALSE == bRet)
    {
        return false;
    }

    stPersonLibInfo.udwID = udwID;
    return true;
}

/* 修改人脸库信息 */
bool DeviceManager::modifyFaceLibInfo(NETDEV_PERSON_LIB_LIST_S& stPersonLibList)
{
    BOOL bRet = NETDEV_ModifyPersonLibInfo(m_lpLoginHandle,&stPersonLibList);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 删除人脸库信息 */
bool DeviceManager::deleteFaceLibInfo(UINT32 udwPersonLibID,
                                      NETDEV_DELETE_DB_FLAG_INFO_S& stFlagInfo)
{
    BOOL bRet = NETDEV_DeletePersonLibInfo(m_lpLoginHandle,udwPersonLibID, &stFlagInfo);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 添加人员信息 */
bool DeviceManager::addPersonInfo(UINT32 udwPersonLibID,
                                  NETDEV_PERSON_INFO_LIST_S& stPersonInfoList,
                                  NETDEV_PERSON_RESULT_LIST_S& stPersonResultList)
{
    BOOL bRet = NETDEV_AddPersonInfo(m_lpLoginHandle,udwPersonLibID,&stPersonInfoList,&stPersonResultList);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 修改人员信息 */
bool DeviceManager::modifyPersonInfo(UINT32 udwPersonLibID,
                                     NETDEV_PERSON_INFO_LIST_S& stPersonInfoList,
                                     NETDEV_PERSON_RESULT_LIST_S& stPersonResultList)
{
    BOOL bRet = NETDEV_ModifyPersonInfo(m_lpLoginHandle,udwPersonLibID,&stPersonInfoList,&stPersonResultList);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 删除人员信息 */
bool DeviceManager::deletePersonInfo(UINT32 udwPersonLibID, UINT32 udwPersonID)
{
    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        UINT32 udwLastChange = 0;
        BOOL bRet = NETDEV_DeletePersonInfo(m_lpLoginHandle,udwPersonLibID, udwPersonID,udwLastChange);
        if(FALSE == bRet)
        {
            return false;
        }
    }
    else
    {
        NETDEV_BATCH_OPERATE_MEMBER_LIST_S stIDList = {0};
        stIDList.udwMemberNum = 1;
        stIDList.pstMemberIDList = new UINT32[stIDList.udwMemberNum];
        stIDList.pstMemberIDList[0] = udwPersonID;

        NETDEV_BATCH_OPERATOR_LIST_S stResutList = {0};
        stResutList.udwNum = stIDList.udwMemberNum;
        stResutList.pstBatchList = new NETDEV_BATCH_OPERATOR_INFO_S[stResutList.udwNum];
        memset(stResutList.pstBatchList,0,sizeof(NETDEV_BATCH_OPERATOR_INFO_S) * stResutList.udwNum);
        BOOL bRet = NETDEV_DeletePersonInfoList(m_lpLoginHandle,udwPersonLibID,&stIDList,&stResutList);
        if(FALSE == bRet)
        {
            delete[] stIDList.pstMemberIDList;
            delete[] stResutList.pstBatchList;
            return false;
        }

        delete[] stIDList.pstMemberIDList;
        delete[] stResutList.pstBatchList;
    }
	
	return true;
}

/* 获取人脸布控任务信息 */
bool DeviceManager::getFaceMonitorTaskInfo(UINT32 udwChannelID,
                                           NETDEV_MONITOR_QUERY_INFO_S& stQueryInfo,
                                           QList<NETDEV_MONITION_INFO_S>& faceMonitorInfoList)
{
    BOOL bRet = 0;
    LPVOID lpPersonMonitorHandle = NETDEV_FindPersonMonitorList(m_lpLoginHandle, udwChannelID, &stQueryInfo);
    if (NULL == lpPersonMonitorHandle)
    {
        return false;
    }
    else
    {
        while (true)
        {
            NETDEV_MONITION_INFO_S stMonitorInfo = {0};

            stMonitorInfo.stMonitorRuleInfo.udwChannelNum = 1;
            stMonitorInfo.stMonitorRuleInfo.pudwMonitorChlIDList = new UINT32[stMonitorInfo.stMonitorRuleInfo.udwChannelNum];

            stMonitorInfo.udwLinkStrategyNum = 10;
            stMonitorInfo.pstLinkStrategyList = new NETDEV_LINKAGE_STRATEGY_S[stMonitorInfo.udwLinkStrategyNum];

            bRet = NETDEV_FindNextPersonMonitorInfo(lpPersonMonitorHandle, &stMonitorInfo);
            if (FALSE == bRet)
            {
                delete[] stMonitorInfo.stMonitorRuleInfo.pudwMonitorChlIDList;
                delete[] stMonitorInfo.pstLinkStrategyList;
                NETDEV_FindClosePersonMonitorList(lpPersonMonitorHandle);
                break;
            }
            else
            {
                NETDEV_GetPersonMonitorRuleInfo(m_lpLoginHandle, &stMonitorInfo);
                faceMonitorInfoList.append(stMonitorInfo);
            }
        }
    }

    return true;
}

/* 添加人脸布控任务信息 */
bool DeviceManager::addFaceMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo, NETDEV_MONITOR_RESULT_INFO_S& stMonitorResultInfo)
{
    BOOL bRet = NETDEV_AddPersonMonitorInfo(m_lpLoginHandle, &stMonitorInfo, &stMonitorResultInfo);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 修改人脸布控任务信息 */
bool DeviceManager::modifyFaceMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo)
{
    BOOL bRet = NETDEV_SetPersonMonitorRuleInfo(m_lpLoginHandle, &stMonitorInfo);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 删除人脸布控任务信息 */
bool DeviceManager::deleteFaceMonitorTaskInfo(NETDEV_BATCH_OPERATOR_LIST_S& stBatchList)
{
    BOOL bRet = TRUE;
    bRet = NETDEV_BatchDeletePersonMonitorInfo(m_lpLoginHandle, &stBatchList);
    if (FALSE == bRet)
    {
        qCritical() << " deleteFaceMonitorTaskInfo fail";
        return false;
    }

    return true;
}

/* 获取告警记录人脸全景图信息 */
bool DeviceManager::getFacePanoImageInfo(UINT32 recordID, UINT32 recordType, NETDEV_FILE_INFO_S& stFileInfo)
{
    if(FALSE == NETDEV_GetFaceRecordImageInfo(m_lpLoginHandle, recordID, recordType, &stFileInfo))
    {
        return false;
    }

    return true;
}

/* 人脸识别-人脸抓拍记录检索,注意，内存需要在外部释放 */
bool DeviceManager::searchFaceRecordInfo(NETDEV_ALARM_LOG_COND_LIST_S stFindCond,
                                         QList<NETDEV_FACE_RECORD_SNAPSHOT_INFO_S>& faceSearchRecordInfoList)
{
    NETDEV_SMART_ALARM_LOG_RESULT_INFO_S  stResultInfo = {0};
    LPVOID lpFindHandle = NETDEV_FindFaceRecordDetailList(m_lpLoginHandle, &stFindCond, &stResultInfo);
    if (NULL != lpFindHandle)
    {
        while (true)
        {
            NETDEV_FACE_RECORD_SNAPSHOT_INFO_S stRecordInfo = {0};
            stRecordInfo.stCompareInfo.stMemberInfo.stMemberSemiInfo.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
            stRecordInfo.stCompareInfo.stMemberInfo.stMemberSemiInfo.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
            stRecordInfo.stCompareInfo.stMemberInfo.stMemberImageInfo.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
            stRecordInfo.stCompareInfo.stMemberInfo.stMemberImageInfo.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
            stRecordInfo.stCompareInfo.stSnapshotImage.stBigImage.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
            stRecordInfo.stCompareInfo.stSnapshotImage.stBigImage.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
            stRecordInfo.stCompareInfo.stSnapshotImage.stSmallImage.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
            stRecordInfo.stCompareInfo.stSnapshotImage.stSmallImage.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
            BOOL bRet = NETDEV_FindNextFaceRecordDetail(lpFindHandle, &stRecordInfo);
            if (FALSE == bRet)
            {
                delete stRecordInfo.stCompareInfo.stMemberInfo.stMemberSemiInfo.pcData;
                delete stRecordInfo.stCompareInfo.stMemberInfo.stMemberImageInfo.pcData;
                delete stRecordInfo.stCompareInfo.stSnapshotImage.stBigImage.pcData;
                delete stRecordInfo.stCompareInfo.stSnapshotImage.stSmallImage.pcData;
                break;
            }
            else
            {
                faceSearchRecordInfoList.append(stRecordInfo);
            }
        }
    }
    else {
        return false;
    }

    NETDEV_FindCloseFaceRecordDetail(lpFindHandle);

    return true;
}

/* 车牌识别-抓拍记录检索,注意，内存需要在外部释放 */
bool DeviceManager::searchPlateRecordInfo(NETDEV_ALARM_LOG_COND_LIST_S& stFindCond,
                                         QList<NETDEV_VEHICLE_RECORD_INFO_S>& plateSearchRecordInfoList)
{
    NETDEV_SMART_ALARM_LOG_RESULT_INFO_S stResultInfo = {0};
    LPVOID lpFindHandle = NETDEV_FindVehicleRecordInfoList(m_lpLoginHandle, &stFindCond, &stResultInfo);
    if (NULL != lpFindHandle)
    {
        while (true)
        {
            NETDEV_VEHICLE_RECORD_INFO_S stVehicleRecordInfo = {0};
            stVehicleRecordInfo.stPlateImage.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
            stVehicleRecordInfo.stPlateImage.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
            stVehicleRecordInfo.stVehicleImage.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
            stVehicleRecordInfo.stVehicleImage.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
            stVehicleRecordInfo.stPanoImage.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
            stVehicleRecordInfo.stPanoImage.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
            BOOL bRet = NETDEV_FindNextVehicleRecordInfo(lpFindHandle, &stVehicleRecordInfo);
            if (FALSE == bRet)
            {
                delete stVehicleRecordInfo.stPlateImage.pcData;
                delete stVehicleRecordInfo.stVehicleImage.pcData;
                delete stVehicleRecordInfo.stPanoImage.pcData;
                break;
            }
            else
            {
                plateSearchRecordInfoList.append(stVehicleRecordInfo);
            }
        }
    }
    else {
        return false;
    }

    NETDEV_FindCloseVehicleRecordList(lpFindHandle);

    return true;
}

/* 获取告警记录车牌全景图信息 */
bool DeviceManager::getPlatePanoImageInfo(UINT32 recordID, NETDEV_FILE_INFO_S& stFileInfo)
{
    if(FALSE == NETDEV_GetVehicleRecordImageInfo(m_lpLoginHandle, recordID, &stFileInfo))
    {
        return false;
    }

    return true;
}

/* 打开实况 */
bool DeviceManager::startRealPlay(NETDEV_PREVIEWINFO_S& stPreviewInfo)
{
    LPVOID lpPlayHandle = NETDEV_RealPlay(m_lpLoginHandle, &stPreviewInfo, NULL,NULL);
    if(NULL == lpPlayHandle)
    {
        return false;
    }

    /*开启元数据*/
    NETDEV_SetIVAEnable(lpPlayHandle,TRUE);

    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        NETDEMO_IPCNVR_CHANNEL_INFO_S* pchannelInfo = NULL;
        getIPCNVRChannelInfoFromID(stPreviewInfo.dwChannelID,&pchannelInfo);
        if(NULL != pchannelInfo)
        {
            pchannelInfo->playHandle = lpPlayHandle;
        }
    }
    else
    {
        NETDEMO_VMS_CHANNEL_INFO_S* pchannelInfo = NULL;
        getVMSChannelInfoFromID(stPreviewInfo.dwChannelID,&pchannelInfo);
        if(NULL != pchannelInfo)
        {
            pchannelInfo->playHandle = lpPlayHandle;
        }
    }


    return true;
}

/* 关闭实况 */
bool DeviceManager::stopRealPlay(int dwChannelID)
{
    LPVOID lpPlayHandle = NULL;
    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        NETDEMO_IPCNVR_CHANNEL_INFO_S* pchannelInfo = NULL;
        getIPCNVRChannelInfoFromID(dwChannelID,&pchannelInfo);
        if(NULL != pchannelInfo)
        {
            lpPlayHandle = pchannelInfo->playHandle;
            pchannelInfo->playHandle = NULL;
        }
    }
    else
    {
        NETDEMO_VMS_CHANNEL_INFO_S* pchannelInfo = NULL;
        getVMSChannelInfoFromID(dwChannelID,&pchannelInfo);
        if(NULL != pchannelInfo)
        {
            lpPlayHandle = pchannelInfo->playHandle;
            pchannelInfo->playHandle = NULL;
        }
    }

    if(NULL != lpPlayHandle)
    {
        if(true == NETDEV_StopRealPlay(lpPlayHandle))
        {
            return true;
        }
    }

    return false;
}

/*
 * 订阅人脸识别比对告警
*/
bool DeviceManager::subFaceRecognitionAlarm(NETDEV_PersonAlarmMessCallBack_PF cbAlarmPersonCallBack,
                                            NETDEV_AlarmMessCallBack_PF_V30 cbAlarmMessCallBack,
                                            LPVOID lpUserData)
{     
    /* 设置人脸告警回调 */
    BOOL bRet = NETDEV_SetPersonAlarmCallBack(m_lpLoginHandle, cbAlarmPersonCallBack, lpUserData);
    if(FALSE == bRet)
    {
        return false;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        if(-1 == m_dwPersonSubID)
        {
            NETDEV_LAPI_SUB_INFO_S stSubInfo = {0};
            stSubInfo.udwType = 0x10;/* 人脸识别 */
            stSubInfo.udwLibIDNum = 0xffff;/*订阅所有的库*/
            NETDEV_SUBSCRIBE_SUCC_INFO_S stSubSuccInfo = {0};
            bRet = NETDEV_SubscibeLapiAlarm(m_lpLoginHandle, &stSubInfo, &stSubSuccInfo);
            if(FALSE == bRet)
            {
                return false;
            }

            m_dwPersonSubID = stSubSuccInfo.udwID;
            return true;
        }
    }
    else
    {
        bRet = NETDEV_SetAlarmCallBack_V30(m_lpLoginHandle, cbAlarmMessCallBack, lpUserData);
        if (FALSE == bRet)
        {
            return false;
        }
    }
}

/*
 * 取消订阅人脸识别比对告警
*/
bool DeviceManager::unSubFaceRecognitionAlarm()
{
    /* 设置人脸告警回调 */
    BOOL bRet = NETDEV_SetPersonAlarmCallBack(m_lpLoginHandle, NULL, m_lpLoginHandle);
    if(FALSE == bRet)
    {
        return false;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        if(-1 != m_dwPersonSubID)
        {
            bRet = NETDEV_UnSubLapiAlarm(m_lpLoginHandle, m_dwPersonSubID);
            if(FALSE == bRet)
            {
                return false;
            }

            m_dwPersonSubID = -1;
        }
    }
    else
    {
        /* 引用计数判断是否要取消告警 */
    }

    return true;
}

/* 订阅人脸抓拍告警 */
bool DeviceManager::subFaceSnapshotAlarm(int dwChannelID,
                                         NETDEV_StructAlarmMessCallBack_PF cbAlarmStructCallBack,
                                         NETDEV_AlarmMessCallBack_PF_V30 cbAlarmMessCallBack,
                                         LPVOID lpUserData)
{
    BOOL bRet = NETDEV_SetStructAlarmCallBack(m_lpLoginHandle, cbAlarmStructCallBack, lpUserData);
    if(FALSE == bRet)
    {
        return false;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        if (-1 == m_dwStructSubID)
        {
            NETDEV_LAPI_SUB_INFO_S stSubInfo = {0};
            stSubInfo.udwType = 0x20;
            stSubInfo.udwLibIDNum = 0xffff;

            NETDEV_SUBSCRIBE_SUCC_INFO_S stSubSuccInfo = {0};
            bRet = NETDEV_SubscibeLapiAlarm(m_lpLoginHandle, &stSubInfo, &stSubSuccInfo);
            if (FALSE == bRet)
            {
                return false;
            }
            else
            {
                m_dwStructSubID = stSubSuccInfo.udwID;
            }
        }
    }
    else
    {
        NETDEMO_VMS_CHANNEL_INFO_S* pchannelInfo = NULL;
        getVMSChannelInfoFromID(dwChannelID,&pchannelInfo);
        if(-1 != pchannelInfo->dwFaceSnapshotSubID)
        {
            return true;
        }

        NETDEV_SUBSCRIBE_SMART_INFO_S stSubscribeInfo = {0};
        stSubscribeInfo.udwNum = 1;

        stSubscribeInfo.pudwSmartType = new UINT32[stSubscribeInfo.udwNum];
        stSubscribeInfo.pudwSmartType[0] = NETDEV_SMART_ALARM_TYPE_FACE_SNAP;

        NETDEV_SMART_INFO_S stSmartInfo = {0};
        stSmartInfo.dwChannelID = dwChannelID;

        bRet = NETDEV_SubscribeSmart(m_lpLoginHandle, &stSubscribeInfo, &stSmartInfo);
        if (FALSE == bRet)
        {
            delete[] stSubscribeInfo.pudwSmartType;
            return false;
        }
        else
        {
            pchannelInfo->dwFaceSnapshotSubID = stSmartInfo.udwSubscribeID;
            delete[] stSubscribeInfo.pudwSmartType;
        }
    }

    return true;
}

/* 取消订阅人脸抓拍告警 */
bool DeviceManager::unSubFaceSnapshotAlarm(int dwChannelID)
{
    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        if(-1 != m_dwStructSubID)
        {
            BOOL bRet = NETDEV_SetStructAlarmCallBack(m_lpLoginHandle, NULL, m_lpLoginHandle);
            if(FALSE == bRet)
            {
                return false;
            }

            bRet = NETDEV_UnSubLapiAlarm(m_lpLoginHandle, m_dwStructSubID);
            if(FALSE == bRet)
            {
                return false;
            }

            m_dwStructSubID = -1;
        }
    }
    else
    {
        NETDEMO_VMS_CHANNEL_INFO_S* pchannelInfo = NULL;
        getVMSChannelInfoFromID(dwChannelID,&pchannelInfo);

        if(-1 != pchannelInfo->dwFaceSnapshotSubID)
        {
            NETDEV_SMART_INFO_S stSmartInfo = {0};
            stSmartInfo.dwChannelID = dwChannelID;
            stSmartInfo.udwSubscribeID = pchannelInfo->dwFaceSnapshotSubID;
            BOOL bRet = NETDEV_UnsubscribeSmart(m_lpLoginHandle, &stSmartInfo);
            if (FALSE == bRet)
            {
                return false;
            }

            pchannelInfo->dwFaceSnapshotSubID = -1;
        }
    }

    return true;
}

/* 订阅车牌识别比对告警 */
bool DeviceManager::subPlateRecognitionAlarm(NETDEV_AlarmMessCallBack_PF_V30 cbAlarmMessCallBack,
                                             NETDEV_VehicleAlarmMessCallBack_PF cbVehicleAlarmMessCallBack,
                                             LPVOID lpUserData)
{
    BOOL bRet = NETDEV_SetVehicleAlarmCallBack(m_lpLoginHandle, cbVehicleAlarmMessCallBack, lpUserData);
    if (FALSE == bRet)
    {
        return false;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        if (-1 == m_dwPlateRecogSubID)
        {
            NETDEV_LAPI_SUB_INFO_S stSubInfo = {0};
            stSubInfo.udwType = 64;
            stSubInfo.udwLibIDNum = 0xffff;

            NETDEV_SUBSCRIBE_SUCC_INFO_S stSubSuccInfo = {0};
            bRet = NETDEV_SubscibeLapiAlarm(m_lpLoginHandle, &stSubInfo, &stSubSuccInfo);
            if (FALSE == bRet)
            {
                return false;
            }
            else
            {
                m_dwPlateRecogSubID = stSubSuccInfo.udwID;
            }
        }
        else
        {
            return true;
        }
    }
    else
    {
        bRet = NETDEV_SetAlarmCallBack_V30(m_lpLoginHandle, cbAlarmMessCallBack, lpUserData);
        if (FALSE == bRet)
        {
            return false;
        }
    }

    return true;
}

/* 取消订阅车牌识别比对告警 */
bool DeviceManager::unSubPlateRecognitionAlarm()
{
    NETDEV_SetVehicleAlarmCallBack(m_lpLoginHandle, NULL, m_lpLoginHandle);

    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        if (-1 != m_dwPlateRecogSubID)
        {
            BOOL bRet = NETDEV_UnSubLapiAlarm(m_lpLoginHandle, m_dwPlateRecogSubID);
            if (FALSE == bRet)
            {
                return false;
            }

            m_dwPlateRecogSubID = -1;
        }
    }
    else {
        /* 引用计数判断是否要取消告警 */
    }

    return true;
}

/* 订阅车牌抓拍告警 */
bool DeviceManager::subPlateSnapshotAlarm(int dwChannelID,
                                          NETDEV_StructAlarmMessCallBack_PF cbAlarmStructCallBack,
                                          NETDEV_AlarmMessCallBack_PF_V30 cbAlarmMessCallBack,
                                          LPVOID lpUserData)
{
    BOOL bRet = NETDEV_SetStructAlarmCallBack(m_lpLoginHandle, cbAlarmStructCallBack, lpUserData);
    if (FALSE == bRet)
    {
        return false;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        if (-1 == m_dwStructSubID)
        {
            NETDEV_LAPI_SUB_INFO_S stSubInfo = {0};
            stSubInfo.udwType = 0x20;
            stSubInfo.udwLibIDNum = 0xffff;

            NETDEV_SUBSCRIBE_SUCC_INFO_S stSubSuccInfo = {0};
            bRet = NETDEV_SubscibeLapiAlarm(m_lpLoginHandle, &stSubInfo, &stSubSuccInfo);
            if (FALSE == bRet)
            {
                return false;
            }
            else
            {
                m_dwStructSubID = stSubSuccInfo.udwID;
            }
        }
    }
    else
    {
        NETDEMO_VMS_CHANNEL_INFO_S* pchannelInfo = NULL;
        getVMSChannelInfoFromID(dwChannelID,&pchannelInfo);
        if(-1 != pchannelInfo->dwVehicleSnapshotSubID)
        {
            return true;
        }

        NETDEV_SUBSCRIBE_SMART_INFO_S stSubscribeInfo = {0};
        stSubscribeInfo.udwNum = 1;

        stSubscribeInfo.pudwSmartType = new UINT32[stSubscribeInfo.udwNum];
        stSubscribeInfo.pudwSmartType[0] = NETDEV_SMART_ALARM_TYPE_VEHICLE_SNAP;

        NETDEV_SMART_INFO_S stSmartInfo = {0};
        stSmartInfo.dwChannelID = dwChannelID;

        bRet = NETDEV_SubscribeSmart(m_lpLoginHandle, &stSubscribeInfo, &stSmartInfo);
        if (FALSE == bRet)
        {
            delete[] stSubscribeInfo.pudwSmartType;
            return false;
        }
        else
        {
            pchannelInfo->dwVehicleSnapshotSubID = stSmartInfo.udwSubscribeID;
            delete[] stSubscribeInfo.pudwSmartType;
        }
    }

    return true;
}

/* 取消订阅车牌抓拍告警 */
bool DeviceManager::unSubPlateSnapshotAlarm(int dwChannelID)
{
    if(NETDEMO_DEVICE_IPC_OR_NVR == m_eDevType)
    {
        if (-1 != m_dwStructSubID)
        {
            BOOL bRet = NETDEV_UnSubLapiAlarm(m_lpLoginHandle, m_dwStructSubID);
            if (FALSE == bRet)
            {
                return false;
            }

            m_dwStructSubID = -1;
        }
    }
    else
    {
        NETDEMO_VMS_CHANNEL_INFO_S* pchannelInfo = NULL;
        getVMSChannelInfoFromID(dwChannelID,&pchannelInfo);

        if(-1 != pchannelInfo->dwVehicleSnapshotSubID)
        {
            NETDEV_SMART_INFO_S stSmartInfo = {0};
            stSmartInfo.dwChannelID = dwChannelID;
            stSmartInfo.udwSubscribeID = pchannelInfo->dwVehicleSnapshotSubID;
            BOOL bRet = NETDEV_UnsubscribeSmart(m_lpLoginHandle, &stSmartInfo);
            if (FALSE == bRet)
            {
                return false;
            }

            pchannelInfo->dwVehicleSnapshotSubID = -1;
        }
    }
    return true;
}

/* 查找车牌布控信息 */
bool DeviceManager::getVehicleMonitorTaskInfo(QList<NETDEV_MONITION_INFO_S>& m_vehicleMonitorTaskInfoList)
{
    LPVOID lpVehicleMonitorHandle = NETDEV_FindVehicleMonitorList(m_lpLoginHandle);
    if (NULL == lpVehicleMonitorHandle)
    {
        return false;
    }
    else
    {
        while (true)
        {
            NETDEV_MONITION_INFO_S stMonitorInfo = {0};

            stMonitorInfo.stMonitorRuleInfo.udwChannelNum = 1;
            stMonitorInfo.stMonitorRuleInfo.pudwMonitorChlIDList = new UINT32[stMonitorInfo.stMonitorRuleInfo.udwChannelNum];

            stMonitorInfo.udwLinkStrategyNum = 10;
            stMonitorInfo.pstLinkStrategyList = new NETDEV_LINKAGE_STRATEGY_S[stMonitorInfo.udwLinkStrategyNum];

            BOOL bRet = NETDEV_FindNextVehicleMonitorInfo(lpVehicleMonitorHandle, &stMonitorInfo);
            if (FALSE == bRet)
            {
                delete[] stMonitorInfo.stMonitorRuleInfo.pudwMonitorChlIDList;
                delete[] stMonitorInfo.pstLinkStrategyList;
                break;
            }
            else
            {
                NETDEV_GetVehicleMonitorInfo(m_lpLoginHandle, stMonitorInfo.udwID, &stMonitorInfo.stMonitorRuleInfo);
                m_vehicleMonitorTaskInfoList.append(stMonitorInfo);
            }
        }

        NETDEV_FindCloseVehicleMonitorList(lpVehicleMonitorHandle);

        return true;
    }
}

/* 添加车辆布控任务信息 */
bool DeviceManager::addVehicleMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo)
{
    BOOL bRet = NETDEV_AddVehicleMonitorInfo(m_lpLoginHandle, &stMonitorInfo);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 修改车辆布控任务信息 */
bool DeviceManager::modifyVehicleMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo)
{
    BOOL bRet = NETDEV_SetVehicleMonitorInfo(m_lpLoginHandle, stMonitorInfo.udwID, &stMonitorInfo.stMonitorRuleInfo);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 删除车辆布控任务信息 */
bool DeviceManager::deleteVehicleMonitorTaskInfo(NETDEV_BATCH_OPERATOR_LIST_S& stBatchList)
{
    BOOL bRet = TRUE;
    bRet = NETDEV_DeleteVehicleMonitorInfo(m_lpLoginHandle, &stBatchList);
    if (FALSE == bRet)
    {
        qCritical() << " deleteVehicleMonitorTaskInfo fail";
        return false;
    }

    return true;
}

/* 查询车辆库信息 */
bool DeviceManager::getVehicleLibInfo(QList<NETDEV_LIB_INFO_S>& stLibInfoList)
{
    LPVOID lpFindLibHandle = NETDEV_FindVehicleLibList(m_lpLoginHandle);
    if(NULL == lpFindLibHandle)
    {
        return false;
    }

    BOOL bRet = TRUE;
    while(TRUE == bRet)
    {
        NETDEV_LIB_INFO_S stLibInfo = {0};
        bRet = NETDEV_FindNextVehicleLibInfo(lpFindLibHandle, &stLibInfo);
        if(FALSE == bRet)
        {
            NETDEV_FindCloseVehicleLibList(lpFindLibHandle);
            break;
        }

        stLibInfoList.append(stLibInfo);
    }

    return true;
}

/* 添加车辆库信息 */
bool DeviceManager::addVehicleLibInfo(NETDEV_LIB_INFO_S& stLibInfo)
{
    BOOL bRet = NETDEV_AddVehicleLibInfo(m_lpLoginHandle, &stLibInfo);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 修改车辆库信息 */
bool DeviceManager::modifyVehicleLibInfo(NETDEV_PERSON_LIB_LIST_S& stLibList)
{
    BOOL bRet = NETDEV_ModifyVehicleLibInfo(m_lpLoginHandle,&stLibList);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 删除车辆库信息 */
bool DeviceManager::deleteVehicleLibInfo(UINT32 udwLibID, NETDEV_DELETE_DB_FLAG_INFO_S& stFlagInfo)
{
    BOOL bRet = NETDEV_DeleteVehicleLibInfo(m_lpLoginHandle,udwLibID, &stFlagInfo);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 查询车辆库下车辆信息 */
bool DeviceManager::getVehicleInfo(UINT32 udwVehicleLibID,
                                  NETDEV_PERSON_QUERY_INFO_S& stQueryInfo,
                                  NETDEV_BATCH_OPERATE_BASIC_S& stQueryResultInfo,
                                  QList<NETDEV_VEHICLE_DETAIL_INFO_S>& vehicleLibMemberInfoList)
{
    LPVOID lpFindHandle = NETDEV_FindVehicleMemberDetailList(m_lpLoginHandle, udwVehicleLibID, &stQueryInfo,&stQueryResultInfo);
    if(NULL == lpFindHandle)
    {
        return false;
    }

    BOOL bRet = TRUE;
    while(TRUE == bRet)
    {
        NETDEV_VEHICLE_DETAIL_INFO_S stVehicleInfo = {0};
        for(int i = 0; i < NETDEV_LEN_8;i++)
        {
            stVehicleInfo.stVehicleAttr.stVehicleImage.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];/* 注意释放内存 */
            memset(stVehicleInfo.stVehicleAttr.stVehicleImage.pcData,0,NETDEMO_SMALL_IMAGE_SIZE);
            stVehicleInfo.stVehicleAttr.stVehicleImage.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
        }

        bRet = NETDEV_FindNextVehicleMemberDetail(lpFindHandle, &stVehicleInfo);
        if(FALSE == bRet)
        {
            delete[] stVehicleInfo.stVehicleAttr.stVehicleImage.pcData;
            NETDEV_FindCloseVehicleMemberDetail(lpFindHandle);
            break;
        }

        vehicleLibMemberInfoList.append(stVehicleInfo);
    }

    return true;
}

/* 添加车辆信息 */
bool DeviceManager::addVehicleInfo(UINT32 udwVehicleLibID,
                                  NETDEV_VEHICLE_INFO_LIST_S& stVehicleInfoList,
                                  NETDEV_BATCH_OPERATOR_LIST_S& stResultList)
{
    BOOL bRet = NETDEV_AddVehicleMemberList(m_lpLoginHandle,udwVehicleLibID,&stVehicleInfoList,&stResultList);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 修改车辆信息 */
bool DeviceManager::modifyVehicleInfo(UINT32 udwVehicleLibID,
                                     NETDEV_VEHICLE_INFO_LIST_S& stVehicleInfoList,
                                     NETDEV_BATCH_OPERATOR_LIST_S& stResultList)
{
    BOOL bRet = NETDEV_ModifyVehicleMemberInfo(m_lpLoginHandle,udwVehicleLibID,&stVehicleInfoList,&stResultList);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 删除车辆信息 */
bool DeviceManager::deleteVehicleInfo(UINT32 udwVehicleLibID,
                                      NETDEV_VEHICLE_INFO_LIST_S& stVehicleInfoList,
                                      NETDEV_BATCH_OPERATOR_LIST_S& stResutList)
{
    BOOL bRet = NETDEV_DelVehicleMemberList(m_lpLoginHandle,0, &stVehicleInfoList, &stResutList);
    if(FALSE == bRet)
    {
        return false;
    }

    return true;
}

/* 通过URL获取图片数据 */
bool DeviceManager::getImageDataByUrl(QString& url, int size, QByteArray& data)
{
    CHAR* imageData = new CHAR[size];
    BOOL bRet = NETDEV_GetSystemPicture(m_lpLoginHandle, (CHAR*)url.toStdString().c_str(), size, imageData);
    if (FALSE == bRet)
    {
        delete[] imageData;
        return false;
    }

    data.append(imageData, size);
    delete[] imageData;
    return true;
}

/* ************* private **************** */

/* 查询设备详细信息(组织，子设备，通道) */
bool DeviceManager::findDeviceDetailInfo()
{
    if(NETDEMO_DEVICE_VMS == m_eDevType)
    {
        NETDEV_ORG_FIND_COND_S stFindCond = {0};
        stFindCond.udwRootOrgID = 0;
        LPVOID lpFindOrgHandle = NETDEV_FindOrgInfoList(m_lpLoginHandle, &stFindCond);
        if (NULL == lpFindOrgHandle)
        {
            qDebug() << "find org fail";
            return false;
        }

        while (true)
        {
            NETDEV_ORG_INFO_S stOrgInfo = {0};
            BOOL bRet = NETDEV_FindNextOrgInfo(lpFindOrgHandle, &stOrgInfo);
            if (FALSE == bRet)
            {
                break;
            }

            m_VMSOrgInfoList.append(stOrgInfo);
        }

        NETDEV_FindCloseOrgInfo(lpFindOrgHandle);

        /* 查询子设备，当前仅查询普通编码设备、智能编码设备、门禁设备 */
        NETDEV_DEVICE_MAIN_TYPE_E eSubDeviceTypeList[3] = {NETDEV_DTYPE_MAIN_ENCODE,NETDEV_DTYPE_MAIN_BAYONET,NETDEV_DTYPE_MAIN_ACS};
        for (int i = 0; i < sizeof (eSubDeviceTypeList)/sizeof (NETDEV_DEVICE_MAIN_TYPE_E); i++)
        {
            LPVOID lpFindDevHandle = NETDEV_FindDevList(m_lpLoginHandle, eSubDeviceTypeList[i]);
            if (NULL == lpFindDevHandle)
            {
                qWarning() << "find sub Device fail, type:" << eSubDeviceTypeList[i];
                continue;
            }

            while (true)
            {
                NETDEMO_VMS_SUB_DEVICE_INFO_S stVmsSubDeviceInfo = {0};
                BOOL bRet = NETDEV_FindNextDevInfo(lpFindDevHandle, &stVmsSubDeviceInfo.stBaseInfo);
                if (FALSE == bRet)
                {
                    break;
                }

                int dwChlType = NETDEV_CHN_TYPE_ENCODE;
                LPVOID lpFindDevChnHandle = NETDEV_FindDevChnList(m_lpLoginHandle, stVmsSubDeviceInfo.stBaseInfo.dwDevID, dwChlType);
                if (NULL == lpFindDevChnHandle)
                {
                    qWarning() << "find sub Device channel fail, subDevice:" << stVmsSubDeviceInfo.stBaseInfo.szDevName;
                    continue;
                }

                while (true)
                {
                    int dwBytesReturned = 0;
                    NETDEMO_VMS_CHANNEL_INFO_S stVMSChannelInfo = {0};
                    stVMSChannelInfo.playHandle = NULL;
                    stVMSChannelInfo.dwFaceSnapshotSubID = -1;
                    stVMSChannelInfo.dwVehicleSnapshotSubID = -1;
                    bRet = NETDEV_FindNextDevChn(lpFindDevChnHandle, &stVMSChannelInfo.stBaseInfo, sizeof(NETDEV_DEV_CHN_ENCODE_INFO_S), &dwBytesReturned);
                    if (FALSE == bRet)
                    {
                        break;
                    }

                    stVmsSubDeviceInfo.VMSChannelInfoList.append(stVMSChannelInfo);
                }

                NETDEV_FindCloseDevChn(lpFindDevChnHandle);

                m_VMSSubDeviceInfoList.append(stVmsSubDeviceInfo);
            }

            NETDEV_FindCloseDevInfo(lpFindDevHandle);
        }
    }
    else
    {
        /*获取视频源通道信息*/
        int dwCount = 128;
        NETDEV_VIDEO_CHL_DETAIL_INFO_EX_S astVideoChnDetailInfoExList[128] = {0};
        BOOL bRet = NETDEV_QueryVideoChlDetailListEx(m_lpLoginHandle,&dwCount,astVideoChnDetailInfoExList);
        if(FALSE == bRet)
        {
            qCritical() << "Query video channel Info Fail";
            return false;
        }

        for(int i = 0; i < dwCount; i++)
        {
            NETDEMO_IPCNVR_CHANNEL_INFO_S stIPCNVRChannelInfo;
            stIPCNVRChannelInfo.playHandle = NULL;
            stIPCNVRChannelInfo.stBaseInfo = astVideoChnDetailInfoExList[i];
            m_IPCNVRChannelInfoList.append(stIPCNVRChannelInfo);
        }

        NETDEV_FindDevChlBasicInfoList(m_lpLoginHandle);
    }
}

/* 根据通道ID获取通道信息 */
bool DeviceManager::getIPCNVRChannelInfoFromID(int channelID, NETDEMO_IPCNVR_CHANNEL_INFO_S** pChannelInfo)
{
    for(int i = 0; i < m_IPCNVRChannelInfoList.size(); i++)
    {
        if(channelID == m_IPCNVRChannelInfoList[i].stBaseInfo.dwChannelID)
        {
            *pChannelInfo = &m_IPCNVRChannelInfoList[i];
            return true;
        }
    }

    return false;
}

/* 根据通道ID获取通道信息 */
bool DeviceManager::getVMSChannelInfoFromID(int channelID, NETDEMO_VMS_CHANNEL_INFO_S** pChannelInfo)
{
    for(int i = 0; i < m_VMSSubDeviceInfoList.size(); i++)
    {
        for(int j = 0; j < m_VMSSubDeviceInfoList[i].VMSChannelInfoList.size(); j++)
        {
            if(channelID == m_VMSSubDeviceInfoList[i].VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.dwChannelID)
            {
                *pChannelInfo = &m_VMSSubDeviceInfoList[i].VMSChannelInfoList[j];
                return true;
            }
        }
    }

    return false;
}
