#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QMenu>
#include <QScrollBar>
#include <QDateTime>
#include <QMetaType>
#include <QImage>
#include <QMessageBox>
#include <QFileDialog>

/* 普通告警回调 */
void STDCALL cbAlarmMessCallBack_V30(IN LPVOID lpUserID,
                                        IN LPNETDEV_REPORT_INFO_S pstReportInfo,
                                        IN LPVOID    lpBuf,
                                        IN INT32     dwBufLen,
                                        IN LPVOID    lpUserData)
{

}

/* 结构化回调 */
void STDCALL cbAlarmStructCallBack(IN LPVOID lpUserID,
                                    IN LPNETDEV_STRUCT_ALARM_INFO_S pstAlarmInfo,
                                    IN LPNETDEV_STRUCT_DATA_INFO_S pstAlarmData,
                                    IN LPVOID    lpUserData)
{
    MainWidget* pMainWidget = (MainWidget*)lpUserData;
    /* 直接操作UI可能会崩溃，目前仅用于测试数据是否正常 */
    //emit pMainWidget->showStructInfoSignal(lpUserID, pstAlarmInfo, pstAlarmData);
    pMainWidget->showStructInfoSlot(lpUserID,pstAlarmInfo, pstAlarmData);
}

/* 人脸识别回调 */
void STDCALL cbAlarmPersonCallBack(IN LPVOID lpUserID,
                                 IN LPNETDEV_PERSON_EVENT_INFO_S pstAlarmData,
                                 IN LPVOID    lpUserData)
{
    MainWidget* pMainWidget = (MainWidget*)lpUserData;

    /* 备份数据，避免接口返回数据释放，注意delete内存 */
    LPNETDEV_PERSON_EVENT_INFO_S pstAlarmDataTemp = new NETDEV_PERSON_EVENT_INFO_S;
    memcpy(pstAlarmDataTemp,pstAlarmData,sizeof(NETDEV_PERSON_EVENT_INFO_S));

    for(int i = 0; i < pstAlarmDataTemp->udwFaceInfoNum; i++)
    {
        if(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.udwFeatureNum > 0)
        {
            pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.pstFeatureInfo = new NETDEV_FEATURE_INFO_S[pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.udwFeatureNum];
            memcpy(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.pstFeatureInfo,
                   pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.pstFeatureInfo,
                   sizeof(NETDEV_FEATURE_INFO_S) * pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.udwFeatureNum);
        }

        pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stFaceImage.pcData = new CHAR[pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stFaceImage.udwSize];
        memcpy(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stFaceImage.pcData,
               pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stFaceImage.pcData,
               sizeof(CHAR) * pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stFaceImage.udwSize);

        if (pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.udwSize > 0
                && pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.pcData != NULL)
        {
            pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.pcData = new CHAR[pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.udwSize];
            memcpy(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.pcData,
                   pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.pcData,
                   sizeof(CHAR) * pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.udwSize);
        }
        else {
            QString strPicUrl = QString(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.szUrl);
            if (true != strPicUrl.isEmpty())
            {
                /* 调用NETDEV_GetSystemPicture获取全景图数据，demo因为不显示全景图，此处未调用 */
            }
        }

        if(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwCustomNum > 0)
        {
            pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.pstCustomValueList = new NETDEV_CUSTOM_VALUE_S[pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwCustomNum];
            memcpy(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.pstCustomValueList,
                   pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.pstCustomValueList,
                   sizeof(NETDEV_CUSTOM_VALUE_S) * pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwCustomNum);
        }

        for(int j = 0; j < pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwImageNum;j++)
        {
           pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stImageInfo[j].stFileInfo.pcData
                   = new CHAR[pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stImageInfo[j].stFileInfo.udwSize];
           memcpy(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stImageInfo[j].stFileInfo.pcData,
                  pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stImageInfo[j].stFileInfo.pcData,
                  sizeof(CHAR) * pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stImageInfo[j].stFileInfo.udwSize);
        }

        if(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwTimeTemplateNum > 0)
        {
            pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.pstTimeTemplateList
                    = new NETDEV_PERSON_TIME_TEMPLATE_INFO_S[pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwTimeTemplateNum];
            memcpy(pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.pstTimeTemplateList,
                   pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.pstTimeTemplateList,
                   sizeof(NETDEV_PERSON_TIME_TEMPLATE_INFO_S) * pstAlarmDataTemp->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwTimeTemplateNum);
        }
    }

    emit pMainWidget->showRecognitionInfoSignal(lpUserID, pstAlarmDataTemp);
}

/* 车牌识别回调 */
void STDCALL cbVehicleAlarmMessCallBack(IN LPVOID lpUserID,
                                        IN LPNETDEV_VEH_RECOGNITION_EVENT_S pstVehicleAlarmInfo,
                                        IN LPVOID lpBuf,
                                        IN INT32 dwBufLen,
                                        IN LPVOID lpUserData)
{
    MainWidget* pMainWidget = (MainWidget*)lpUserData;

    /* 注意在槽函数中delete */
    NETDEMO_VEH_RECOGNITION_EVENT_S* pVehRecognitionEvent = new NETDEMO_VEH_RECOGNITION_EVENT_S();

    pVehRecognitionEvent->udwSrcID = pstVehicleAlarmInfo->udwSrcID;
    pVehRecognitionEvent->szReference = QString(pstVehicleAlarmInfo->szReference);
    pVehRecognitionEvent->stVehicleEventInfo.udwID = pstVehicleAlarmInfo->stVehicleEventInfo.udwID;
    pVehRecognitionEvent->stVehicleEventInfo.udwTimestamp = pstVehicleAlarmInfo->stVehicleEventInfo.udwTimestamp;
    pVehRecognitionEvent->stVehicleEventInfo.udwVehicleInfoNum = pstVehicleAlarmInfo->stVehicleEventInfo.udwVehicleInfoNum;
    pVehRecognitionEvent->stVehicleEventInfo.udwNotificationType = pstVehicleAlarmInfo->stVehicleEventInfo.udwNotificationType;
    for(int i = 0; i < pVehRecognitionEvent->stVehicleEventInfo.udwVehicleInfoNum; i++)
    {
        NETDEV_VEHICLE_RECORD_INFO_S stVehicleRecordInfoTemp = pstVehicleAlarmInfo->stVehicleEventInfo.pstVehicleRecordInfo[i];
        NETDEMO_VEHICLE_RECORD_INFO_S vehicleRecordInfo;
        vehicleRecordInfo.udwRecordID = stVehicleRecordInfoTemp.udwRecordID;
        vehicleRecordInfo.udwChannelID = stVehicleRecordInfoTemp.udwChannelID;
        vehicleRecordInfo.udwPassingTime = stVehicleRecordInfoTemp.udwPassingTime;
        vehicleRecordInfo.szChannelName = QString(stVehicleRecordInfoTemp.szChannelName);

        vehicleRecordInfo.stPlateAttr.udwType = stVehicleRecordInfoTemp.stPlateAttr.udwType;
        vehicleRecordInfo.stPlateAttr.udwColor = stVehicleRecordInfoTemp.stPlateAttr.udwColor;
        vehicleRecordInfo.stPlateAttr.szPlateNo = QString(stVehicleRecordInfoTemp.stPlateAttr.szPlateNo);

        vehicleRecordInfo.stVehAttr.udwType = stVehicleRecordInfoTemp.stVehAttr.udwType;
        vehicleRecordInfo.stVehAttr.udwColor = stVehicleRecordInfoTemp.stVehAttr.udwColor;
        vehicleRecordInfo.stVehAttr.fSpeedValue = stVehicleRecordInfoTemp.stVehAttr.fSpeedValue;
        vehicleRecordInfo.stVehAttr.udwSpeedType = stVehicleRecordInfoTemp.stVehAttr.udwSpeedType;
        vehicleRecordInfo.stVehAttr.udwSpeedUnit = stVehicleRecordInfoTemp.stVehAttr.udwSpeedUnit;
        vehicleRecordInfo.stVehAttr.udwImageDirection = stVehicleRecordInfoTemp.stVehAttr.udwImageDirection;
        vehicleRecordInfo.stVehAttr.szVehicleBrand = QString(stVehicleRecordInfoTemp.stVehAttr.szVehicleBrand);

        vehicleRecordInfo.stPanoImage.szUrl = QString(stVehicleRecordInfoTemp.stPanoImage.szUrl);
        vehicleRecordInfo.stPanoImage.szName = QString(stVehicleRecordInfoTemp.stPanoImage.szName);
        vehicleRecordInfo.stPanoImage.dwFileType = stVehicleRecordInfoTemp.stPanoImage.dwFileType;
        vehicleRecordInfo.stPanoImage.udwLastChange = stVehicleRecordInfoTemp.stPanoImage.udwLastChange;

        if(stVehicleRecordInfoTemp.stPanoImage.udwSize > 0
                && stVehicleRecordInfoTemp.stPanoImage.pcData != NULL)
        {
            vehicleRecordInfo.stPanoImage.udwSize = stVehicleRecordInfoTemp.stPanoImage.udwSize;
            vehicleRecordInfo.stPanoImage.pcData.append(stVehicleRecordInfoTemp.stPanoImage.pcData,stVehicleRecordInfoTemp.stPanoImage.udwSize);
        }

        vehicleRecordInfo.stVehicleImage.szUrl = QString(stVehicleRecordInfoTemp.stVehicleImage.szUrl);
        vehicleRecordInfo.stVehicleImage.szName = QString(stVehicleRecordInfoTemp.stVehicleImage.szName);
        vehicleRecordInfo.stVehicleImage.dwFileType = stVehicleRecordInfoTemp.stVehicleImage.dwFileType;
        vehicleRecordInfo.stVehicleImage.udwLastChange = stVehicleRecordInfoTemp.stVehicleImage.udwLastChange;
        if(stVehicleRecordInfoTemp.stVehicleImage.udwSize > 0
                && stVehicleRecordInfoTemp.stVehicleImage.pcData != NULL)
        {
            vehicleRecordInfo.stVehicleImage.udwSize = stVehicleRecordInfoTemp.stVehicleImage.udwSize;
            vehicleRecordInfo.stVehicleImage.pcData.append(stVehicleRecordInfoTemp.stVehicleImage.pcData,stVehicleRecordInfoTemp.stVehicleImage.udwSize);
        }

        vehicleRecordInfo.stPlateImage.szUrl = QString(stVehicleRecordInfoTemp.stPlateImage.szUrl);
        vehicleRecordInfo.stPlateImage.szName = QString(stVehicleRecordInfoTemp.stPlateImage.szName);
        vehicleRecordInfo.stPlateImage.dwFileType = stVehicleRecordInfoTemp.stPlateImage.dwFileType;
        vehicleRecordInfo.stPlateImage.udwLastChange = stVehicleRecordInfoTemp.stPlateImage.udwLastChange;
        if(stVehicleRecordInfoTemp.stPlateImage.udwSize > 0
                && stVehicleRecordInfoTemp.stPlateImage.pcData != NULL)
        {
            vehicleRecordInfo.stPlateImage.udwSize = stVehicleRecordInfoTemp.stPlateImage.udwSize;
            vehicleRecordInfo.stPlateImage.pcData.append(stVehicleRecordInfoTemp.stPlateImage.pcData,stVehicleRecordInfoTemp.stPlateImage.udwSize);
        }

        vehicleRecordInfo.stMonitorAlarmInfo.udwMemberID = stVehicleRecordInfoTemp.stMonitorAlarmInfo.udwMemberID;
        vehicleRecordInfo.stMonitorAlarmInfo.udwMonitorReason = stVehicleRecordInfoTemp.stMonitorAlarmInfo.udwMonitorReason;
        vehicleRecordInfo.stMonitorAlarmInfo.udwMonitorAlarmType = stVehicleRecordInfoTemp.stMonitorAlarmInfo.udwMonitorAlarmType;

        pVehRecognitionEvent->stVehicleEventInfo.stVehicleRecordInfoList.append(vehicleRecordInfo);
    }

    emit pMainWidget->showPlateRecogAlarmInfoSignal(lpUserID, pVehRecognitionEvent);
}

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    qRegisterMetaType<LPNETDEV_STRUCT_ALARM_INFO_S>("LPNETDEV_STRUCT_ALARM_INFO_S");
    qRegisterMetaType<LPNETDEV_STRUCT_DATA_INFO_S>("LPNETDEV_STRUCT_DATA_INFO_S");
    qRegisterMetaType<LPNETDEV_PERSON_EVENT_INFO_S>("LPNETDEV_PERSON_EVENT_INFO_S");
    qRegisterMetaType<LPVOID>("LPVOID");
    qRegisterMetaType<NETDEMO_VEH_RECOGNITION_EVENT_S>("NETDEMO_VEH_RECOGNITION_EVENT_S");

    ui->faceSnapRecordListWgt->setViewMode(QListView::IconMode);
    ui->faceSnapRecordListWgt->setFlow(QListView::TopToBottom);
    ui->faceSnapRecordListWgt->setWrapping(false);
    ui->faceSnapRecordListWgt->setResizeMode(QListView::Adjust);
    ui->faceSnapRecordListWgt->horizontalScrollBar()->setDisabled(true);
    ui->faceSnapRecordListWgt->setMovement(QListView::Static);
    ui->faceSnapRecordListWgt->setIconSize(QSize(206,201));

    ui->faceAlarmRecordTabWgt->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->faceAlarmRecordTabWgt->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->faceAlarmRecordTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->faceAlarmRecordTabWgt->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应行高
    ui->faceAlarmRecordTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->facePlayScreenLab->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->faceSnapshotPicLab->setScaledContents(true);
    ui->personLibPicLab->setScaledContents(true);

    ui->faceLibTreeWgt->setColumnCount(1);
    ui->faceLibTreeWgt->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->faceLibTreeWgt->header()->setHidden(true);

    ui->personInfoTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->personInfoTabWgt->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->personInfoTabWgt->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->personInfoTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->personInfoTabWgt->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->personInfoTabWgt->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->faceMonitorInfoTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    //ui->faceMonitorInfoTabWgt->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应行高
    ui->faceMonitorInfoTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->faceMonitorInfoTabWgt->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->faceMonitorInfoTabWgt->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->faceSearchRecordInfoTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->faceSearchRecordInfoTabWgt->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->faceSearchRecordInfoTabWgt->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->faceSearchRecordInfoTabWgt->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->faceSearchRecordInfoTabWgt->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->faceSearchRecordInfoTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->faceSearchRecordInfoTabWgt->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->faceSearchRecordInfoTabWgt->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->plateSnapRecordListWgt->setViewMode(QListView::IconMode);
    ui->plateSnapRecordListWgt->setFlow(QListView::TopToBottom);
    ui->plateSnapRecordListWgt->setWrapping(false);
    ui->plateSnapRecordListWgt->setResizeMode(QListView::Adjust);
    ui->plateSnapRecordListWgt->horizontalScrollBar()->setDisabled(true);
    ui->plateSnapRecordListWgt->setMovement(QListView::Static);
    ui->plateSnapRecordListWgt->setIconSize(QSize(206,100));

    ui->plateAlarmRecordTabWgt->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->plateAlarmRecordTabWgt->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->plateAlarmRecordTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->plateAlarmRecordTabWgt->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应行高
    ui->plateAlarmRecordTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QDateTime curDateTime = QDateTime::currentDateTime();
    curDateTime.setTime(QTime(0,0,0));
    ui->faceAlarmSearchBeginTimeDTEdit->setDateTime(curDateTime);
    curDateTime.setTime(QTime(23,59,59));
    ui->faceAlarmSearchEndTimeDTEdit->setDateTime(curDateTime);

    ui->plateSearchRecordInfoTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->plateSearchRecordInfoTabWgt->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->plateSearchRecordInfoTabWgt->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->plateSearchRecordInfoTabWgt->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->plateSearchRecordInfoTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->plateSearchRecordInfoTabWgt->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->plateSearchRecordInfoTabWgt->setSelectionMode(QAbstractItemView::SingleSelection);

    curDateTime.setTime(QTime(0,0,0));
    ui->plateAlarmSearchBeginTimeDTEdit->setDateTime(curDateTime);
    curDateTime.setTime(QTime(23,59,59));
    ui->plateAlarmSearchEndTimeDTEdit->setDateTime(curDateTime);

    ui->plateMonitorTaskInfoTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->plateMonitorTaskInfoTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->plateMonitorTaskInfoTabWgt->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->plateMonitorTaskInfoTabWgt->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->vehicleInfoTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->vehicleInfoTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->vehicleInfoTabWgt->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->vehicleInfoTabWgt->setSelectionMode(QAbstractItemView::SingleSelection);

    m_dwFindFaceLibMemberOffset = 0;
    m_dwFindFaceAlarmRecordOffset = 0;
    m_dwFindVehicleLibMemberOffset = 0;
    m_dwFindVehicleAlarmRecordOffset = 0;
    m_faceImagePath = "";

    NETDEV_Init();

    /* 信号槽 */
    connect(MessageLog::instance(),SIGNAL(message(const QString&, const QString&)), this, SLOT(outputMegToTextBrower(const QString&,const QString&)));
    connect(this,SIGNAL(showRecognitionInfoSignal(LPVOID,LPNETDEV_PERSON_EVENT_INFO_S)),this,SLOT(showRecognitionInfoSlot(LPVOID, LPNETDEV_PERSON_EVENT_INFO_S)));
    connect(this,SIGNAL(showStructInfoSignal(LPVOID,LPNETDEV_STRUCT_ALARM_INFO_S,LPNETDEV_STRUCT_DATA_INFO_S)),this,SLOT(showStructInfoSlot(LPVOID, LPNETDEV_STRUCT_ALARM_INFO_S,LPNETDEV_STRUCT_DATA_INFO_S)));
    connect(this,SIGNAL(showPlateRecogAlarmInfoSignal(LPVOID,NETDEMO_VEH_RECOGNITION_EVENT_S*)),this,SLOT(showPlateRecogAlarmInfoSlot(LPVOID, NETDEMO_VEH_RECOGNITION_EVENT_S*)));
    connect(ui->faceTabWidget,SIGNAL(currentChanged(int)),this,SLOT(onFaceTabWgtCurChangedSlot(int)));
    connect(ui->faceLibTreeWgt,SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onClickFaceLibItemSlot(QTreeWidgetItem*,int)));
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::closeEvent (QCloseEvent* e)
{
    if( QMessageBox::question(this,
                             tr("Quit"),
                             tr("Are you sure to quit this application?"),
                              QMessageBox::Yes, QMessageBox::No )
                   == QMessageBox::Yes){
        e->accept();

        /*此处放置关闭demo前需要进行的操作*/
        QList<DeviceManager*> deviceListTemp = m_DeviceList;
        for(int i = 0; i < deviceListTemp.size(); i++)
        {
            this->logout(deviceListTemp[i]);
        }
        qDebug()<<"ok";
    }
    else
    {
      e->ignore();
    }
}

/* 日志输出 */
void MainWidget::outputMegToTextBrower(const QString& messageType, const QString& msg)
{
    QString messageTextColor = "black";
    if("[Debug]" == messageType)
    {
        messageTextColor = "gray";
    }
    else if("[Info]" == messageType)
    {
        messageTextColor = "black";
    }
    else if("[Warning]" == messageType)
    {
        messageTextColor = "orange";
    }
    else if("[Critical]" == messageType || "[Fatal:]" == messageType)
    {
        messageTextColor = "red";
    }

    QString htmlMegText = QString("<font style='font-size:16px; background-color:white; color:%2;'> %1 </font>").arg(msg).arg(messageTextColor);
    ui->logTextBrowser->append(htmlMegText);
}

/* 登录设备 */
void MainWidget::on_loginBtn_clicked()
{

    DeviceManager* pDeviceManager = new DeviceManager();

    pDeviceManager->m_strIpAddress = ui->IPlineEdit->text();
    pDeviceManager->m_dwPort = ui->portLineEdit->text().toShort();
    pDeviceManager->m_strUserName = ui->UserNameLineEdit->text();
    pDeviceManager->m_strPassword = ui->pwdLineEdit->text();
    pDeviceManager->m_eDevType = (NETDEMO_DEVICE_TYPE_E)ui->loginDeviceTypeCmbBox->currentIndex();

    bool bRet = pDeviceManager->login();
    if(false == bRet)
    {
        return;
    }

    m_DeviceList.append(pDeviceManager);

    /* 添加到设备列表中 */
    QTreeWidgetItem* deviceItem = new QTreeWidgetItem();
    deviceItem->setText(0, pDeviceManager->m_strIpAddress);
    ui->realTimeVideoChlTreeWgt->addTopLevelItem(deviceItem);

    if(NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        NETDEMO_DEVICE_ITEM_DATA_S stItemData = {
            NETDEMO_DEVICE_TYPE_INVALID,NETDEMO_DEVICE_TREE_NODE_INVALID,NULL,-1,-1,NETDEV_DEV_STATUS_OFFLINE,-1,NETDEV_CHL_STATUS_OFFLINE
        };

        stItemData.eDeviceType = NETDEMO_DEVICE_IPC_OR_NVR;
        stItemData.lpLoginDevice = pDeviceManager->m_lpLoginHandle;
        stItemData.eTreeNodeType = NETDEMO_DEVICE_TREE_NODE_DEVICE;
        deviceItem->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));

        for(int i = 0; i < pDeviceManager->getIPCNVRChannelInfoList().size(); i++)
        {
            QTreeWidgetItem* item_sub = new QTreeWidgetItem();
            NETDEMO_DEVICE_ITEM_DATA_S stItemData = {
                NETDEMO_DEVICE_TYPE_INVALID,NETDEMO_DEVICE_TREE_NODE_INVALID,NULL,-1,-1,NETDEV_DEV_STATUS_OFFLINE,-1,NETDEV_CHL_STATUS_OFFLINE
            };

            stItemData.eDeviceType = NETDEMO_DEVICE_IPC_OR_NVR;
            stItemData.lpLoginDevice = pDeviceManager->m_lpLoginHandle;
            stItemData.eTreeNodeType = NETDEMO_DEVICE_TREE_NODE_CHANNEL;
            stItemData.dwChannelID = pDeviceManager->getIPCNVRChannelInfoList()[i].stBaseInfo.dwChannelID;
            stItemData.eChannelStatus = pDeviceManager->getIPCNVRChannelInfoList()[i].stBaseInfo.enStatus;
            item_sub->setText(0, QString("chl" + QString::number(stItemData.dwChannelID) + "_" + pDeviceManager->getIPCNVRChannelInfoList()[i].stBaseInfo.szChnName));
            item_sub->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
            deviceItem->addChild(item_sub);
        }
    }
    else
    {
        /* 顶层设备节点设置用户数据 */
        NETDEMO_DEVICE_ITEM_DATA_S stItemData = {
            NETDEMO_DEVICE_TYPE_INVALID,NETDEMO_DEVICE_TREE_NODE_INVALID,NULL,-1,-1,NETDEV_DEV_STATUS_OFFLINE,-1,NETDEV_CHL_STATUS_OFFLINE
        };

        stItemData.eDeviceType = NETDEMO_DEVICE_VMS;
        stItemData.lpLoginDevice = pDeviceManager->m_lpLoginHandle;
        stItemData.eTreeNodeType = NETDEMO_DEVICE_TREE_NODE_DEVICE;
        deviceItem->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));

        /* 组织节点设置用户数据 */
        QList<NETDEV_ORG_INFO_S> VMSOrgInfoList = pDeviceManager->getVMSOrgInfoList();
        QTreeWidgetItem* orgItemList = new QTreeWidgetItem[VMSOrgInfoList.size()];
        for(int i = 0; i < VMSOrgInfoList.size(); i++)
        {
            NETDEMO_DEVICE_ITEM_DATA_S stItemData = {
                NETDEMO_DEVICE_TYPE_INVALID,NETDEMO_DEVICE_TREE_NODE_INVALID,NULL,-1,-1,NETDEV_DEV_STATUS_OFFLINE,-1,NETDEV_CHL_STATUS_OFFLINE
            };

            stItemData.eDeviceType = NETDEMO_DEVICE_VMS;
            stItemData.lpLoginDevice = pDeviceManager->m_lpLoginHandle;
            stItemData.eTreeNodeType = NETDEMO_DEVICE_TREE_NODE_ORG;
            stItemData.dwOrgID = VMSOrgInfoList[i].dwOrgID;
            (orgItemList+i)->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
            (orgItemList+i)->setText(0, QString(VMSOrgInfoList[i].szNodeName));
        }

        /* 子设备节点设置用户数据 */
        QList<NETDEMO_VMS_SUB_DEVICE_INFO_S> VMSSubDeviceInfoList = pDeviceManager->getVMSSubDeviceInfoList();
        QTreeWidgetItem* subDeviceItemList = new QTreeWidgetItem[VMSSubDeviceInfoList.size()];
        for(int i = 0; i < VMSSubDeviceInfoList.size(); i++)
        {
            NETDEMO_DEVICE_ITEM_DATA_S stItemData = {
                NETDEMO_DEVICE_TYPE_INVALID,NETDEMO_DEVICE_TREE_NODE_INVALID,NULL,-1,-1,NETDEV_DEV_STATUS_OFFLINE,-1,NETDEV_CHL_STATUS_OFFLINE
            };

            stItemData.eDeviceType = NETDEMO_DEVICE_VMS;
            stItemData.lpLoginDevice = pDeviceManager->m_lpLoginHandle;
            stItemData.eTreeNodeType = NETDEMO_DEVICE_TREE_NODE_SUB_DEVICE;
            stItemData.dwOrgID = VMSSubDeviceInfoList[i].stBaseInfo.dwOrgID;
            stItemData.dwSubDeviceID = VMSSubDeviceInfoList[i].stBaseInfo.dwDevID;
            stItemData.dwSubDeviceStatus = (NETDEV_DEVICE_STATUS_E)VMSSubDeviceInfoList[i].stBaseInfo.dwDevStatus;
            (subDeviceItemList+i)->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
            (subDeviceItemList+i)->setText(0, QString(VMSSubDeviceInfoList[i].stBaseInfo.szDevName));

            /* 通道节点设置用户数据 */
            QList<NETDEMO_VMS_CHANNEL_INFO_S> VMSChannelInfoList = VMSSubDeviceInfoList[i].VMSChannelInfoList;
            QTreeWidgetItem* channelItemList = new QTreeWidgetItem[VMSChannelInfoList.size()];
            for(int j = 0; j < VMSChannelInfoList.size(); j++)
            {
                NETDEMO_DEVICE_ITEM_DATA_S stItemData = {
                    NETDEMO_DEVICE_TYPE_INVALID,NETDEMO_DEVICE_TREE_NODE_INVALID,NULL,-1,-1,NETDEV_DEV_STATUS_OFFLINE,-1,NETDEV_CHL_STATUS_OFFLINE
                };

                stItemData.eDeviceType = NETDEMO_DEVICE_VMS;
                stItemData.lpLoginDevice = pDeviceManager->m_lpLoginHandle;
                stItemData.eTreeNodeType = NETDEMO_DEVICE_TREE_NODE_SUB_DEVICE;
                stItemData.dwOrgID = VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.dwOrgID;
                stItemData.dwSubDeviceID = VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.dwDevID;
                stItemData.dwSubDeviceStatus = NETDEV_DEV_STATUS_ONLINE;
                stItemData.dwChannelID = VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.dwChannelID;
                stItemData.eChannelStatus = (NETDEV_CHANNEL_STATUS_E)VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.dwChnStatus;
                (channelItemList+j)->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
                (channelItemList+j)->setText(0, QString(VMSChannelInfoList[j].stBaseInfo.stChnBaseInfo.szChnName));

                (subDeviceItemList+i)->addChild((channelItemList+j));
            }
        }

        QTreeWidgetItem* rootOrgItem = NULL;
        for(int i = 0; i < VMSOrgInfoList.size(); i++)
        {
            /* 标记根组织 */
            if(0 == VMSOrgInfoList[i].dwParentID)
            {
                rootOrgItem = (orgItemList+i);
            }

            /* 组织下所属下级组织 */
            for(int j = 0; j < VMSOrgInfoList.size(); j++)
            {
                if(VMSOrgInfoList[i].dwOrgID == VMSOrgInfoList[j].dwParentID)
                {
                    (orgItemList+i)->addChild((orgItemList+j));
                }
            }

            /* 组织下所属子设备 */
            for(int k = 0; k < VMSSubDeviceInfoList.size(); k++)
            {
                QVariant variant = (subDeviceItemList+k)->data(0,Qt::UserRole);  // 获取用户数据
                NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();
                if(stItemData.dwOrgID == VMSOrgInfoList[i].dwOrgID)
                {
                    (orgItemList+i)->addChild((subDeviceItemList+k));
                }
            }
        }

        deviceItem->addChild(rootOrgItem);
    }

    /* 获取人员库信息 */
    findAllPersonLibInfo();
}

/* 登出设备 */
void MainWidget::on_logoutBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    logout(pDeviceManager);
}

/* 登出指定设备 */
void MainWidget::logout(DeviceManager* pDeviceManager)
{
    pDeviceManager->logout();

    /* 删除节点信息 */
    int topLevelItemCount = ui->realTimeVideoChlTreeWgt->topLevelItemCount();
    for(int i = 0; i < topLevelItemCount; i++)
    {
        QVariant variant = ui->realTimeVideoChlTreeWgt->topLevelItem(i)->data(0,Qt::UserRole);  // 获取用户数据
        NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();
        ;
        if(pDeviceManager->m_lpLoginHandle == stItemData.lpLoginDevice)
        {
            ui->realTimeVideoChlTreeWgt->takeTopLevelItem(i);
            break;
        }
    }

    m_DeviceList.removeOne(pDeviceManager);
}

/* 开启实况 */
void MainWidget::startRealPlay(int dwChannelID, LPVOID hPlayWnd)
{
    stopRealPlay(dwChannelID);

    NETDEV_PREVIEWINFO_S stPreviewInfo = {0};
    stPreviewInfo.hPlayWnd = hPlayWnd;
    stPreviewInfo.dwChannelID = dwChannelID;
    stPreviewInfo.dwStreamType = NETDEV_LIVE_STREAM_INDEX_MAIN;
    stPreviewInfo.dwLinkMode = NETDEV_TRANSPROTOCAL_RTPTCP;
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    bool bRet = pDeviceManager->startRealPlay(stPreviewInfo);
    if(false == bRet)
    {
        qCritical() << "NETDEV_RealPlay Fail";
        return;
    }

    qInfo() << "NETDEV_RealPlay success";
}

/* 停止实况 */
void MainWidget::stopRealPlay(int dwChannelID)
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    bool bRet = pDeviceManager->stopRealPlay(dwChannelID);
    if(false == bRet)
    {
        qCritical() << "NETDEV_StopRealPlay Fail";
        return;
    }

    qInfo() << "NETDEV_StopRealPlay success";
}

/* 显示人脸识别信息 */
void MainWidget::showRecognitionInfoSlot(LPVOID lpUserID, LPNETDEV_PERSON_EVENT_INFO_S pstAlarmData)
{
    if(NULL == pstAlarmData)
    {
        qCritical() << "showRecognitionInfo fail";
        return;
    }

    qDebug() << "showRecognitionInfo success";

    DeviceManager* pDeviceManager = getCurSelectedDevice(lpUserID);
    if(NULL == pDeviceManager)
    {
        return;
    }

    ui->faceAlarmRecordTabWgt->insertRow(0);

    /* 记录时间 */
    QTableWidgetItem* itemTime = new QTableWidgetItem(QDateTime::fromTime_t(pstAlarmData->udwTimestamp).toString("yyyy-MM-dd HH:mm:ss"));

    for(int i = 0; i < pstAlarmData->udwFaceInfoNum; i++)
    {
        /*姓名*/
        QTableWidgetItem* itemName = new QTableWidgetItem(tr(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.szPersonName));

        /* 匹配状态 */
        QString status;
        if(NETDEV_TYPE_FACE_PASS_COM_SUCCESS == pstAlarmData->stCtrlFaceInfo[i].udwType)
        {
            status = "Match";
        }
        else {
            status = "Not Match";
        }

        QTableWidgetItem* itemStatus = new QTableWidgetItem(status);

        /* 证件号,仅显示身份证 */
        QTableWidgetItem* pItemIdentification = NULL;
        for(int IdentificationIndex = 0;
            IdentificationIndex < pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwIdentificationNum;
            IdentificationIndex++)
        {
            if(NETDEV_CERTIFICATE_TYPE_ID ==
                    pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stIdentificationInfo[IdentificationIndex].udwType)
            {

                       pItemIdentification = new QTableWidgetItem(tr(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stIdentificationInfo[IdentificationIndex].szNumber));
                break;
            }
        }

        /*相似度*/
        QTableWidgetItem* itemSimilarity = new QTableWidgetItem(QString::number(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.udwSimilarity) + "%");

        /* 报警源 */
        DeviceManager* pDeviceManager = getCurSelectedDevice();
        if(NULL == pDeviceManager)
        {
            return;
        }
        QTableWidgetItem* itemAlarmSource = new QTableWidgetItem(pDeviceManager->m_strIpAddress + "_"
                                                          + tr(pstAlarmData->stCtrlFaceInfo[i].szChlName) + "_"
                                                          + QString::number(pstAlarmData->stCtrlFaceInfo[i].udwChannelID));

        itemTime->setTextAlignment(Qt::AlignCenter);
        itemName->setTextAlignment(Qt::AlignCenter);
        itemSimilarity->setTextAlignment(Qt::AlignCenter);
        itemTime->setTextAlignment(Qt::AlignCenter);
        itemAlarmSource->setTextAlignment(Qt::AlignCenter);

        ui->faceAlarmRecordTabWgt->setItem(0, 0, itemTime);
        ui->faceAlarmRecordTabWgt->setItem(0, 1, itemName);
        if(NULL != pItemIdentification)
        {
            ui->faceAlarmRecordTabWgt->setItem(0, 2, pItemIdentification);
        }

        ui->faceAlarmRecordTabWgt->setItem(0, 3, itemStatus);
        ui->faceAlarmRecordTabWgt->setItem(0, 4, itemSimilarity);
        ui->faceAlarmRecordTabWgt->setItem(0, 5, itemAlarmSource);

        /*比对信息*/
        QImage imageSnapshotPic;
        imageSnapshotPic.loadFromData(QByteArray(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stFaceImage.pcData,
                             pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stFaceImage.udwSize));

        ui->faceSnapshotPicLab->setPixmap(QPixmap::fromImage(imageSnapshotPic).scaled(ui->faceSnapshotPicLab->width(), ui->faceSnapshotPicLab->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        QImage imagePersonLibPic;
        if(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwImageNum > 0)
        {
            imagePersonLibPic.loadFromData(QByteArray(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stImageInfo[0].stFileInfo.pcData,
                                 pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stImageInfo[0].stFileInfo.udwSize));
            ui->personLibPicLab->setPixmap(QPixmap::fromImage(imagePersonLibPic).scaled(ui->personLibPicLab->width(), ui->personLibPicLab->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            ui->personLibPicLab->clear();
            ui->personLibPicLab->setText(tr("未匹配"));
        }

        ui->similarityLab->setText(QString::number(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.udwSimilarity) + "%");
    }

    /* 释放内存 */
    for(int i = 0; i < pstAlarmData->udwFaceInfoNum; i++)
    {
        if(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.udwFeatureNum > 0)
        {
            delete[] pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.pstFeatureInfo;
        }

        delete[] pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stFaceImage.pcData;

        if (pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.udwSize > 0
                && pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.pcData != NULL)
        {
            delete[] pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPanoImage.pcData;
        }

        if(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwCustomNum > 0)
        {
            delete[] pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.pstCustomValueList;
        }

        for(int j = 0; j < pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwImageNum;j++)
        {
           delete[] pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.stImageInfo[j].stFileInfo.pcData;

        }

        if(pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.udwTimeTemplateNum > 0)
        {
            delete[] pstAlarmData->stCtrlFaceInfo[i].stCompareInfo.stPersonInfo.pstTimeTemplateList;

        }
    }

    delete pstAlarmData;
}


/*显示结构化信息*/
void MainWidget::showStructInfoSlot(LPVOID lpUserID, LPNETDEV_STRUCT_ALARM_INFO_S pstAlarmInfo, LPNETDEV_STRUCT_DATA_INFO_S pstAlarmData)
{
    if( NULL == pstAlarmData->pstImageInfo ||
            0 >= pstAlarmData->udwImageNum)
    {
        qCritical() << "showStructInfo fail";
        return;
    }

    qDebug() << "showStructInfo";

    DeviceManager* pDeviceManager = getCurSelectedDevice(lpUserID);
    if(NULL == pDeviceManager)
    {
        return;
    }

    /*人脸信息 */
    if(NULL != pstAlarmData->stObjectInfo.pstFaceInfo &&
            0 < pstAlarmData->stObjectInfo.udwFaceNum)
    {
        for(UINT32 i = 0; i < pstAlarmData->stObjectInfo.udwFaceNum; i++)
        {
            for(UINT32 j = 0;j < pstAlarmData->udwImageNum; j++)
            {
                QString fileName = pDeviceManager->m_strIpAddress + "_"
                                    + tr(pstAlarmInfo->szSrcName) + "_"
                                    + QString::number(pstAlarmInfo->udwTimeStamp) + "_"
                                    + QString::number(pstAlarmData->stObjectInfo.pstFaceInfo[i].udwFaceDoforPersonID) + "_"
                                    + QString::number(pstAlarmData->stObjectInfo.pstFaceInfo[i].udwFaceID)
                                    + "_face";

                /* 大图 */
                if(pstAlarmData->stObjectInfo.pstFaceInfo[i].udwLargePicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {

                    fileName += "_L.jpg";
                }
                /* 小图 */
                else if(pstAlarmData->stObjectInfo.pstFaceInfo[i].udwSmallPicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    fileName += "_S.jpg";
                }
                else{
                    continue;
                }

                /* 存储图片 */
                QFile file(fileName);
                if(!file.open(QIODevice::WriteOnly))
                {
                    qCritical() << "Can't open file for writing";
                    continue;
                }

                file.write(pstAlarmData->pstImageInfo[j].pszData, pstAlarmData->pstImageInfo[j].udwSize);
                file.close();

                /* 当前Demo仅展示人脸图片信息 */
                if(pstAlarmData->stObjectInfo.pstFaceInfo[i].udwSmallPicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    QListWidgetItem *imageItem = new QListWidgetItem;
                    imageItem->setIcon(QIcon(fileName));
                    imageItem->setText(QDateTime::fromTime_t(pstAlarmInfo->udwTimeStamp).toString("yyyy-MM-dd HH:mm:ss"));
                    imageItem->setSizeHint(QSize(206,222));
                    ui->faceSnapRecordListWgt->insertItem(0,imageItem);
                }
            }
        }
    }

    /* 人员信息 */
    if(NULL != pstAlarmData->stObjectInfo.udwPersonNum &&
            0 <pstAlarmData->stObjectInfo.udwPersonNum)
    {
        for(UINT32 i = 0; i < pstAlarmData->stObjectInfo.udwPersonNum; i++)
        {
            for(UINT32 j = 0;j < pstAlarmData->udwImageNum; j++)
            {
                QString fileName = pDeviceManager->m_strIpAddress + "_"
                                    + tr(pstAlarmInfo->szSrcName) + "_"
                                    + QString::number(pstAlarmInfo->udwTimeStamp) + "_"
                                    + QString::number(pstAlarmData->stObjectInfo.pstPersonInfo[i].udwPersonID) + "_"
                                    + QString::number(pstAlarmData->stObjectInfo.pstPersonInfo[i].udwPersonDoforFaceID)
                                    + "_person";
                /* 大图 */
                if(pstAlarmData->stObjectInfo.pstPersonInfo[i].udwLargePicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    fileName += "_L.jpg";
                }
                /* 小图 */
                else if(pstAlarmData->stObjectInfo.pstPersonInfo[i].udwSmallPicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    fileName += "_S.jpg";
                }
                else{
                    continue;
                }

                /* 存储图片 */
                QFile file(fileName);
                if(!file.open(QIODevice::WriteOnly))
                {
                    qCritical() << "Can't open file for writing";
                    continue;
                }
                file.write(pstAlarmData->pstImageInfo[j].pszData, pstAlarmData->pstImageInfo[j].udwSize);
                file.close();
            }
        }
    }

    /* 非机动车信息 */
    if(NULL != pstAlarmData->stObjectInfo.pstNonMotorVehInfo &&
            0 <pstAlarmData->stObjectInfo.udwNonMotorVehNum)
    {
        for(UINT32 i = 0; i < pstAlarmData->stObjectInfo.udwNonMotorVehNum; i++)
        {
            for(UINT32 j = 0;j < pstAlarmData->udwImageNum; j++)
            {
                QString fileName = pDeviceManager->m_strIpAddress + "_"
                                    + tr(pstAlarmInfo->szSrcName) + "_"
                                    + QString::number(pstAlarmInfo->udwTimeStamp) + "_"
                                    + QString::number(pstAlarmData->stObjectInfo.pstNonMotorVehInfo[i].udwID)
                                    + "_NonMotor";
                /* 大图 */
                if(pstAlarmData->stObjectInfo.pstNonMotorVehInfo[i].udwLargePicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    fileName += "_L.jpg";
                }
                /* 小图 */
                else if(pstAlarmData->stObjectInfo.pstNonMotorVehInfo[i].udwSmallPicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    fileName += "_S.jpg";
                }
                else{
                    continue;
                }

                /* 存储图片 */
                QFile file(fileName);
                if(!file.open(QIODevice::WriteOnly))
                {
                    qCritical() << "Can't open file for writing";
                    continue;
                }

                file.write(pstAlarmData->pstImageInfo[j].pszData, pstAlarmData->pstImageInfo[j].udwSize);
                file.close();
            }
        }
    }

    /* 车辆信息 */
    if(NULL != pstAlarmData->stObjectInfo.pstVehInfo &&
            0 <pstAlarmData->stObjectInfo.udwVehicleNum)
    {
        for(UINT32 i = 0; i < pstAlarmData->stObjectInfo.udwVehicleNum; i++)
        {
            for(UINT32 j = 0;j < pstAlarmData->udwImageNum; j++)
            {
                /* 大图 */
                if(pstAlarmData->stObjectInfo.pstVehInfo[i].udwLargePicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    if (pstAlarmData->pstImageInfo[j].udwSize > 0)
                    {
                        QString strPicUrl = tr(pstAlarmData->pstImageInfo[j].szUrl);
                        QByteArray imageData;
                        if (true != strPicUrl.isEmpty())
                        {
                            bool bRet = pDeviceManager->getImageDataByUrl(strPicUrl, (int)pstAlarmData->pstImageInfo[j].udwSize, imageData);
                            if (true == bRet)
                            {

                            }
                        }
                        else
                        {
                            imageData.append(pstAlarmData->pstImageInfo[j].pszData,(int)pstAlarmData->pstImageInfo[j].udwSize);
                        }
                    }
                }
                /* 小图 */
                else if(pstAlarmData->stObjectInfo.pstVehInfo[i].udwSmallPicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    if (pstAlarmData->pstImageInfo[j].udwSize > 0 &&
                            pstAlarmData->pstImageInfo[j].pszData != NULL)
                    {
                        QByteArray imageData;
                        imageData.append(pstAlarmData->pstImageInfo[j].pszData,(int)pstAlarmData->pstImageInfo[j].udwSize);
                    }
                }
                /* 车牌图 */
                else if(pstAlarmData->stObjectInfo.pstVehInfo[i].udwPlatePicAttachIndex == pstAlarmData->pstImageInfo[j].udwIndex)
                {
                    if (pstAlarmData->pstImageInfo[j].udwSize > 0 &&
                            pstAlarmData->pstImageInfo[j].pszData != NULL)
                    {
                        QByteArray imageData;
                        imageData.append(pstAlarmData->pstImageInfo[j].pszData,(int)pstAlarmData->pstImageInfo[j].udwSize);
                        /* 当前Demo仅展示车牌图片信息 */
                        QListWidgetItem *imageItem = new QListWidgetItem;
                        QImage imagePic;
                        imagePic.loadFromData(imageData);
                        imageItem->setIcon(QIcon(QPixmap::fromImage(imagePic).scaled(190, 80, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
                        imageItem->setText(QDateTime::fromTime_t(pstAlarmInfo->udwTimeStamp).toString("yyyy-MM-dd HH:mm:ss"));
                        imageItem->setSizeHint(QSize(190,100));
                        ui->plateSnapRecordListWgt->insertItem(0,imageItem);
                    }
                }
                else{
                    continue;
                }
            }
        }
    }
}

/*显示车牌识别上报信息*/
void MainWidget::showPlateRecogAlarmInfoSlot(LPVOID lpUserID, NETDEMO_VEH_RECOGNITION_EVENT_S* pStVehicleAlarmInfo)
{
    qDebug() << "showPlateRecogAlarmInfoSlot";
    DeviceManager* pDeviceManager = getCurSelectedDevice(lpUserID);
    if(NULL == pDeviceManager)
    {
        return;
    }

    QList<NETDEMO_VEHICLE_RECORD_INFO_S> stVehicleRecordInfoList = pStVehicleAlarmInfo->stVehicleEventInfo.stVehicleRecordInfoList;
    if (stVehicleRecordInfoList.size() > 0)
    {
        QString strPlateNo = stVehicleRecordInfoList[0].stPlateAttr.szPlateNo;
        QString strSnapshotTime = QDateTime::fromTime_t(stVehicleRecordInfoList[0].udwPassingTime).toString("yyyy-MM-dd HH:mm:ss");
        QString strPlateColor = gPlateColorMap.value(stVehicleRecordInfoList[0].stPlateAttr.udwColor);
        QString strMatch;
        QString strPlateType = gPlateTypeMap.value(stVehicleRecordInfoList[0].stPlateAttr.udwType);
        QString strVehicleColor = gPlateColorMap.value(stVehicleRecordInfoList[0].stVehAttr.udwColor);

        if (0 == stVehicleRecordInfoList[0].stMonitorAlarmInfo.udwMonitorAlarmType)
        {
            strMatch = "Match Alarm";
        }
        else
        {
            strMatch = "Not Match Alarm";
        }

        QTableWidgetItem* itemTime = new QTableWidgetItem(strPlateNo);
        QTableWidgetItem* itemSnapshotTime = new QTableWidgetItem(strSnapshotTime);
        QTableWidgetItem* itemPlateColor = new QTableWidgetItem(strPlateColor);
        QTableWidgetItem* itemMatch = new QTableWidgetItem(strMatch);
        QTableWidgetItem* itemPlateType = new QTableWidgetItem(strPlateType);
        QTableWidgetItem* itemVehicleColor = new QTableWidgetItem(strVehicleColor);

        itemTime->setTextAlignment(Qt::AlignCenter);
        itemSnapshotTime->setTextAlignment(Qt::AlignCenter);
        itemPlateColor->setTextAlignment(Qt::AlignCenter);
        itemMatch->setTextAlignment(Qt::AlignCenter);
        itemPlateType->setTextAlignment(Qt::AlignCenter);
        itemVehicleColor->setTextAlignment(Qt::AlignCenter);

        ui->plateAlarmRecordTabWgt->insertRow(0);
        ui->plateAlarmRecordTabWgt->setItem(0, 0, itemTime);
        ui->plateAlarmRecordTabWgt->setItem(0, 1, itemSnapshotTime);
        ui->plateAlarmRecordTabWgt->setItem(0, 2, itemPlateColor);
        ui->plateAlarmRecordTabWgt->setItem(0, 3, itemMatch);
        ui->plateAlarmRecordTabWgt->setItem(0, 4, itemPlateType);
        ui->plateAlarmRecordTabWgt->setItem(0, 5, itemVehicleColor);

        ui->plateNoLab->setText(strPlateNo);

        /* Vehicle picture */
        if (stVehicleRecordInfoList[0].stVehicleImage.udwSize > 0)
        {
            QImage imagePic;
            imagePic.loadFromData(stVehicleRecordInfoList[0].stVehicleImage.pcData);
        }

        /* plate picture */
        if (stVehicleRecordInfoList[0].stPlateImage.udwSize > 0)
        {
            QImage imagePic;
            imagePic.loadFromData(stVehicleRecordInfoList[0].stPlateImage.pcData);
            ui->plateSnapshotPicLab->setPixmap(QPixmap::fromImage(imagePic).scaled(ui->plateSnapshotPicLab->width(),
                                                                                       ui->plateSnapshotPicLab->height(),
                                                                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        /* Panoramic Image */
        if (stVehicleRecordInfoList[0].stPanoImage.udwSize > 0
                && stVehicleRecordInfoList[0].stPanoImage.pcData.size() > 0)
        {
            QImage imagePic;
            imagePic.loadFromData(stVehicleRecordInfoList[0].stPanoImage.pcData);
            ui->plateSnapshotPanoPicLab->setPixmap(QPixmap::fromImage(imagePic).scaled(ui->plateSnapshotPanoPicLab->width(),
                                                                                       ui->plateSnapshotPanoPicLab->height(),
                                                                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            NETDEV_FILE_INFO_S stFileInfo = {0};
            stFileInfo.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
            stFileInfo.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
            if(true == pDeviceManager->getPlatePanoImageInfo(stVehicleRecordInfoList[0].udwRecordID,stFileInfo))
            {
                QImage panoImage;
                panoImage.loadFromData(QByteArray(stFileInfo.pcData, stFileInfo.udwSize));
                ui->plateSnapshotPanoPicLab->setPixmap(QPixmap::fromImage(panoImage).scaled(ui->plateSnapshotPanoPicLab->width(),
                                                                              ui->plateSnapshotPanoPicLab->height(),
                                                                              Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            }

            delete[] stFileInfo.pcData;
        }
    }

    delete pStVehicleAlarmInfo;
}

/* 人脸选项切换 */
void MainWidget::onFaceTabWgtCurChangedSlot(int index)
{
    switch (index)
    {
        case 0:/* 实时监控 */
        {
            break;
        }
        case 1: /* 人脸库管理 */
        {
            break;
        }
        case 2:/* 布控任务 */
        {
            break;
        }
        default:
            break;
    }
}

/* 查询所有人员库信息 */
void MainWidget::findAllPersonLibInfo()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    clearPersonInfoTabWgt();
    ui->faceLibTreeWgt->clear();
    ui->faceMonitorObjectCmbBox->clear();

    QList<NETDEV_LIB_INFO_S> stPersonLibInfoList = pDeviceManager->getPersonLibInfo();

    for(int i = 0; i < stPersonLibInfoList.size(); i++)
    {
        /* 界面显示 */
        QTreeWidgetItem* item = new QTreeWidgetItem();
        NETDEMO_LIB_ITEM_DATA_S stItemData;
        stItemData.dwLibID = stPersonLibInfoList[i].udwID;
        stItemData.libName = QString(stPersonLibInfoList[i].szName);
        item->setText(0, QString(stPersonLibInfoList[i].szName) + "("
                      + QString::number(stPersonLibInfoList[i].udwMemberNum) + "/"
                      + QString::number(stPersonLibInfoList[i].udwFaceNum) + ")");
        item->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
        ui->faceLibTreeWgt->addTopLevelItem(item);

        ui->faceMonitorObjectCmbBox->addItem(QString(stPersonLibInfoList[i].szName));
        ui->faceMonitorObjectCmbBox->setItemData(i,QVariant::fromValue(stItemData),Qt::UserRole);
    }
}

/* 获取当前选中的设备，如果没有指定句柄，就获取当前设备树选中的设备 */
DeviceManager* MainWidget::getCurSelectedDevice(LPVOID lpLoginDevice)
{
    if(NULL == lpLoginDevice)
    {
        QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
        if(NULL == item)
        {
            QMessageBox::warning(this, tr("Demo"),tr("please select a device"));
            return NULL;
        }

        QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
        NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();
        lpLoginDevice = stItemData.lpLoginDevice;
    }


    for(int i = 0; i < m_DeviceList.size(); i++)
    {
        if(lpLoginDevice == m_DeviceList[i]->m_lpLoginHandle)
        {
            return m_DeviceList[i];
        }
    }

    return NULL;
}

/* 单击人脸库项槽 */
void MainWidget::onClickFaceLibItemSlot(QTreeWidgetItem* item,int column)
{
    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
    ui->faceLibNameLineEdit->setText(stItemData.libName);
    m_dwFindFaceLibMemberOffset = 0;
    findPersonInfo(stItemData.dwLibID, m_dwFindFaceLibMemberOffset);
}

/* 清除当前显示的人员信息 */
void MainWidget::clearPersonInfoTabWgt()
{
    int rowCount = ui->personInfoTabWgt->rowCount();
    for(int i = 0;i < rowCount;i++)
    {
        ui->personInfoTabWgt->removeRow(0);
    }

    for(int i = 0; i < m_faceLibMemberInfoList.size(); i++)
    {
        /*释放内存*/
        for(int j = 0; j < NETDEV_LEN_8;j++)
        {
            delete[] m_faceLibMemberInfoList[i].stImageInfo[j].stFileInfo.pcData;
        }
    }
    m_faceLibMemberInfoList.clear();
}

/* 清除人脸布控缓存信息 */
void MainWidget::clearFaceMonitorInfoTabWgt()
{
    int rowCount = ui->faceMonitorInfoTabWgt->rowCount();
    for(int i = 0;i < rowCount;i++)
    {
        ui->faceMonitorInfoTabWgt->removeRow(0);
    }

    for(int i = 0; i < m_faceMonitorTaskInfoList.size(); i++)
    {
        delete[] m_faceMonitorTaskInfoList[i].stMonitorRuleInfo.pudwMonitorChlIDList;
        delete[] m_faceMonitorTaskInfoList[i].pstLinkStrategyList;
    }
    m_faceMonitorTaskInfoList.clear();
}

/* 查询人脸库人员信息 */
void MainWidget::findPersonInfo(UINT32 udwPersonLibID, int dwOffset)
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    clearPersonInfoTabWgt();

    NETDEV_PERSON_QUERY_INFO_S stQueryInfo = {0};
    stQueryInfo.udwLimit = 6;/* 每次查询6条成员记录 */
    stQueryInfo.udwOffset = dwOffset;
    NETDEV_BATCH_OPERATE_BASIC_S stQueryResultInfo = {0};
    pDeviceManager->getPersonInfo(udwPersonLibID, stQueryInfo,stQueryResultInfo,m_faceLibMemberInfoList);
    for(int i = 0; i < m_faceLibMemberInfoList.size(); i++)
    {
        /* 界面展示 */
        showPersonInfo(m_faceLibMemberInfoList[i]);
    }
}

void MainWidget::showPersonInfo(NETDEV_PERSON_INFO_S &stPersonInfo)
{
    int rowCount = ui->personInfoTabWgt->rowCount();
    ui->personInfoTabWgt->insertRow(rowCount);

    QString strGender;
    if(NETDEV_GENDER_TYPE_MAN == stPersonInfo.udwGender)
    {
        strGender = "man";
    }
    else if(NETDEV_GENDER_TYPE_WOMAN == stPersonInfo.udwGender)
    {
        strGender = "woman";
    }
    else {
        strGender = "unknown";
    }

    QString strIdentificationType;
    switch (stPersonInfo.stIdentificationInfo[0].udwType) {
        case NETDEV_CERTIFICATE_TYPE_ID:
        {
            strIdentificationType = "ID";
            break;
        }
        case NETDEV_CERTIFICATE_TYPE_IC:
        {
            strIdentificationType = "IC";
            break;
        }
        case NETDEV_CERTIFICATE_TYPE_PASSPORT:
        {
            strIdentificationType = "Passport";
            break;
        }
        case NETDEV_CERTIFICATE_TYPE_DRIVING_LICENSE:
        {
            strIdentificationType = "Driving license";
            break;
        }
        case NETDEV_CERTIFICATE_TYPE_OTHER:
        {
            strIdentificationType = "other";
            break;
        }
        default:
            strIdentificationType = "unknown";
            break;
    }

    QTableWidgetItem* itemPersonID = new QTableWidgetItem(QString::number(stPersonInfo.udwPersonID));
    QTableWidgetItem* itemPersonName = new QTableWidgetItem(tr(stPersonInfo.szPersonName));
    QTableWidgetItem* itemGender = new QTableWidgetItem(strGender);
    QTableWidgetItem* itemBirthday = new QTableWidgetItem(tr(stPersonInfo.szBirthday));
    QTableWidgetItem* itemNation = new QTableWidgetItem(tr(stPersonInfo.stRegionInfo.szNation));
    QTableWidgetItem* itemProvince = new QTableWidgetItem(tr(stPersonInfo.stRegionInfo.szProvince));
    QTableWidgetItem* itemCity = new QTableWidgetItem(tr(stPersonInfo.stRegionInfo.szCity));
    QTableWidgetItem* itemIdentificationType = new QTableWidgetItem(strIdentificationType);
    QTableWidgetItem* itemIdentification = new QTableWidgetItem(tr(stPersonInfo.stIdentificationInfo[0].szNumber));

    itemPersonID->setTextAlignment(Qt::AlignCenter);
    itemPersonName->setTextAlignment(Qt::AlignCenter);
    itemGender->setTextAlignment(Qt::AlignCenter);
    itemBirthday->setTextAlignment(Qt::AlignCenter);
    itemNation->setTextAlignment(Qt::AlignCenter);
    itemProvince->setTextAlignment(Qt::AlignCenter);
    itemCity->setTextAlignment(Qt::AlignCenter);
    itemIdentificationType->setTextAlignment(Qt::AlignCenter);
    itemIdentification->setTextAlignment(Qt::AlignCenter);

    QLabel* pImageLab = new QLabel();
    pImageLab->resize(111,120);
    QImage imagePersonLibPic;
    if(stPersonInfo.udwImageNum > 0)
    {
        imagePersonLibPic.loadFromData(QByteArray(stPersonInfo.stImageInfo[0].stFileInfo.pcData,
                             stPersonInfo.stImageInfo[0].stFileInfo.udwSize));
        pImageLab->setPixmap(QPixmap::fromImage(imagePersonLibPic).scaled(pImageLab->width(), pImageLab->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    ui->personInfoTabWgt->setCellWidget(rowCount,0,pImageLab);
    ui->personInfoTabWgt->setItem(rowCount, 1, itemPersonID);
    ui->personInfoTabWgt->setItem(rowCount, 2, itemPersonName);
    ui->personInfoTabWgt->setItem(rowCount, 3, itemGender);
    ui->personInfoTabWgt->setItem(rowCount, 4, itemBirthday);
    ui->personInfoTabWgt->setItem(rowCount, 5, itemNation);
    ui->personInfoTabWgt->setItem(rowCount, 6, itemProvince);
    ui->personInfoTabWgt->setItem(rowCount, 7, itemCity);
    ui->personInfoTabWgt->setItem(rowCount, 8, itemIdentificationType);
    ui->personInfoTabWgt->setItem(rowCount, 9, itemIdentification);
}

/* 显示人脸布控任务信息 */
void MainWidget::showFaceMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo)
{
    int rowCount = ui->faceMonitorInfoTabWgt->rowCount();
    ui->faceMonitorInfoTabWgt->insertRow(rowCount);

    QString strStatus = "";
    if (FALSE == stMonitorInfo.stMonitorRuleInfo.bEnabled)
    {
        strStatus = "Disable";
    }
    else
    {
        strStatus = "Enable";
    }

    QString strLibName = "";
    for(int i = 0; i < ui->faceMonitorObjectCmbBox->count(); i++)
    {
        QVariant variant = ui->faceMonitorObjectCmbBox->itemData(i,Qt::UserRole);  // 获取用户数据
        NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
        if(stItemData.dwLibID == stMonitorInfo.stMonitorRuleInfo.audwLibList[0])
        {
            strLibName = stItemData.libName;
            break;
        }
    }

    QTableWidgetItem* itemID = new QTableWidgetItem(QString::number(stMonitorInfo.udwID));
    QTableWidgetItem* itemTaskName = new QTableWidgetItem(tr(stMonitorInfo.stMonitorRuleInfo.szName));
    QTableWidgetItem* itemszReason = new QTableWidgetItem(tr(stMonitorInfo.stMonitorRuleInfo.szReason));
    QTableWidgetItem* itemAlarmThreshold = new QTableWidgetItem(QString::number(stMonitorInfo.stMonitorRuleInfo.udwMultipleValue));
    QTableWidgetItem* itemMonitoringObject = new QTableWidgetItem(strLibName);
    QTableWidgetItem* itemStatus = new QTableWidgetItem(strStatus);

    itemID->setTextAlignment(Qt::AlignCenter);
    itemTaskName->setTextAlignment(Qt::AlignCenter);
    itemszReason->setTextAlignment(Qt::AlignCenter);
    itemAlarmThreshold->setTextAlignment(Qt::AlignCenter);
    itemMonitoringObject->setTextAlignment(Qt::AlignCenter);
    itemStatus->setTextAlignment(Qt::AlignCenter);

    ui->faceMonitorInfoTabWgt->setItem(rowCount, 0, itemID);
    ui->faceMonitorInfoTabWgt->setItem(rowCount, 1, itemTaskName);
    ui->faceMonitorInfoTabWgt->setItem(rowCount, 2, itemszReason);
    ui->faceMonitorInfoTabWgt->setItem(rowCount, 3, itemAlarmThreshold);
    ui->faceMonitorInfoTabWgt->setItem(rowCount, 4, itemMonitoringObject);
    ui->faceMonitorInfoTabWgt->setItem(rowCount, 5, itemStatus);
}

/* 上一页人员信息 */
void MainWidget::on_previousPageFaceLibMemberBtn_clicked()
{
    QTreeWidgetItem* item = ui->faceLibTreeWgt->currentItem();
    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    if(m_dwFindFaceLibMemberOffset > NETDEMO_FIND_LIB_MEM_COUNT)
    {
        m_dwFindFaceLibMemberOffset -= NETDEMO_FIND_LIB_MEM_COUNT;
    }
    else {
        m_dwFindFaceLibMemberOffset = 0;
    }

    findPersonInfo(stItemData.dwLibID, m_dwFindFaceLibMemberOffset);
}

/* 下一页人员信息 */
void MainWidget::on_nextPageFaceLibMemberBtn_clicked()
{
    QTreeWidgetItem* item = ui->faceLibTreeWgt->currentItem();
    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    m_dwFindFaceLibMemberOffset += NETDEMO_FIND_LIB_MEM_COUNT;

    findPersonInfo(stItemData.dwLibID, m_dwFindFaceLibMemberOffset);
}

/* 添加人脸库 */
void MainWidget::on_addFaceLibBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType
            &&  (ui->faceLibTreeWgt->topLevelItemCount() >= pDeviceManager->getetPersonLibCapacity().udwMaxLibNum))
    {
        qWarning() << "The value for maximum lib size is invalid or out of range.";
        return;
    }

    NETDEV_LIB_INFO_S stPersonLibInfo = {0};

    if(ui->faceLibNameLineEdit->text().isEmpty())
    {
        /* 找一个没有被使用过的库名字 */
        int dwlibIndex = 0;
        while(true)
        {
            QString strLibName = tr("新人脸库");
            strLibName += QString::number(dwlibIndex+1);
            int subItemIndex = 0;
            for(; subItemIndex < ui->faceLibTreeWgt->topLevelItemCount(); subItemIndex++)
            {
                if(strLibName == ui->faceLibTreeWgt->topLevelItem(subItemIndex)->text(0).split("(")[0])
                {
                    break;
                }
            }

            if(subItemIndex == ui->faceLibTreeWgt->topLevelItemCount())
            {
                strncpy(stPersonLibInfo.szName,strLibName.toStdString().c_str(),sizeof(stPersonLibInfo.szName));
                break;
            }

            dwlibIndex++;
        }
    }
    else {
        strncpy(stPersonLibInfo.szName,ui->faceLibNameLineEdit->text().toStdString().c_str(),sizeof(stPersonLibInfo.szName));
    }

    bool bRet = pDeviceManager->addFaceLibInfo(stPersonLibInfo);
    if(false == bRet)
    {
        qCritical() << "NETDEV_CreatePersonLibInfo fail";
        return;
    }

    findAllPersonLibInfo();
}

/* 修改人脸库 */
void MainWidget::on_modifyFaceLibBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    QTreeWidgetItem* curItem = ui->faceLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    NETDEV_LIB_INFO_S stPersonLibInfo = {0};
    stPersonLibInfo.udwID = stItemData.dwLibID;
    strncpy(stPersonLibInfo.szName,ui->faceLibNameLineEdit->text().toStdString().c_str(),sizeof(stPersonLibInfo.szName));

    NETDEV_PERSON_LIB_LIST_S stPersonLibList = {0};
    stPersonLibList.udwNum = 1;
    stPersonLibList.pstLibInfo = new NETDEV_LIB_INFO_S[stPersonLibList.udwNum];
    stPersonLibList.pstLibInfo[0] = stPersonLibInfo;
    bool bRet = pDeviceManager->modifyFaceLibInfo(stPersonLibList);
    if(false == bRet)
    {
        qCritical() << "NETDEV_ModifyPersonLibInfo fail:" << NETDEV_GetLastError();
    }
    else {
        on_refreshFaceLibBtn_clicked();
    }

    delete[] stPersonLibList.pstLibInfo;
}

/* 删除人脸库 */
void MainWidget::on_delFaceLibBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    QTreeWidgetItem* curItem = ui->faceLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
    NETDEV_DELETE_DB_FLAG_INFO_S stFlagInfo = {0};
    stFlagInfo.bIsDeleteMember = TRUE;
    bool bRet = pDeviceManager->deleteFaceLibInfo(stItemData.dwLibID,stFlagInfo);
    if(false == bRet)
    {
        qCritical() << "NETDEV_DeletePersonLibInfo fail" << NETDEV_GetLastError();
    }
    else {
        findAllPersonLibInfo();
    }
}

/* 刷新人脸库信息 */
void MainWidget::on_refreshFaceLibBtn_clicked()
{
    /* 获取人员库信息 */
    findAllPersonLibInfo();
}

/* 添加人员信息 */
void MainWidget::on_addPersonBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    QTreeWidgetItem* curItem = ui->faceLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        return;
    }

    if(true == m_faceImagePath.isEmpty())/* 没有选择人脸图片 */
    {
       return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    NETDEV_PERSON_INFO_LIST_S stPersonInfoList = {0};

    stPersonInfoList.udwNum = 1;/* 添加一个人 */
    stPersonInfoList.pstPersonInfo = new NETDEV_PERSON_INFO_S[stPersonInfoList.udwNum];
    memset(stPersonInfoList.pstPersonInfo,0,sizeof(NETDEV_PERSON_INFO_S) * stPersonInfoList.udwNum);
    strncpy(stPersonInfoList.pstPersonInfo[0].szPersonName,
            ui->personNameLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].szPersonName));
    strncpy(stPersonInfoList.pstPersonInfo[0].szBirthday,
            ui->personBirthdayLineEdit->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].szBirthday));
    stPersonInfoList.pstPersonInfo[0].udwGender = ui->personGenderCmbBox->currentIndex();
    strncpy(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szNation,
            ui->personNationLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szNation));
    strncpy(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szProvince,
            ui->personProvinceLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szProvince));
    strncpy(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szCity,
            ui->personCityLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szCity));
    stPersonInfoList.pstPersonInfo[0].udwIdentificationNum = 1;/* 一个证件 */
    stPersonInfoList.pstPersonInfo[0].stIdentificationInfo[0].udwType = ui->personIdentificationTypeCmbBox->currentIndex();
    strncpy(stPersonInfoList.pstPersonInfo[0].stIdentificationInfo[0].szNumber,
            ui->personIdentificationLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].stIdentificationInfo[0].szNumber));

    {
        QByteArray fileData;
        Common::getFileData(m_faceImagePath,fileData);
        QFileInfo fi = QFileInfo(m_faceImagePath);
        strncpy(stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.szName,
                fi.fileName().toStdString().c_str(),
                sizeof (stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.szName));
        stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.dwFileType = NETDEV_TYPE_FILE;
        stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.udwSize = fileData.size();
        stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.pcData = new CHAR[fileData.size()];
        memcpy(stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.pcData,
                fileData.toStdString().c_str(),
                fileData.size());
        stPersonInfoList.pstPersonInfo[0].udwImageNum = 1;
    }

    NETDEV_PERSON_RESULT_LIST_S stPersonResultList = {0};
    stPersonResultList.udwNum = stPersonInfoList.udwNum;
    stPersonResultList.pstPersonList = new NETDEV_PERSON_LIST_S[stPersonResultList.udwNum];
    memset(stPersonResultList.pstPersonList,0,sizeof(NETDEV_PERSON_LIST_S) * stPersonResultList.udwNum);

    bool bRet = pDeviceManager->addPersonInfo(stItemData.dwLibID, stPersonInfoList, stPersonResultList);
    if(false == bRet)
    {
        qCritical() << "add Person Info fail";
    }
    
    delete[] stPersonResultList.pstPersonList;
    delete[] stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.pcData;
    delete[] stPersonInfoList.pstPersonInfo;
    findPersonInfo(stItemData.dwLibID, m_dwFindFaceLibMemberOffset);
}

/* 修改人员信息 */
void MainWidget::on_modifyPersonBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    QTreeWidgetItem* curItem = ui->faceLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        return;
    }

    int dwCurRowIndex = ui->personInfoTabWgt->currentRow();
    if(dwCurRowIndex < 0)
    {
        return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    NETDEV_PERSON_INFO_LIST_S stPersonInfoList = {0};

    stPersonInfoList.udwNum = 1;/* 修改一个人 */
    stPersonInfoList.pstPersonInfo = &m_faceLibMemberInfoList[dwCurRowIndex];

    strncpy(stPersonInfoList.pstPersonInfo[0].szPersonName,
            ui->personNameLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].szPersonName));
    strncpy(stPersonInfoList.pstPersonInfo[0].szBirthday,
            ui->personBirthdayLineEdit->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].szBirthday));
    stPersonInfoList.pstPersonInfo[0].udwGender = ui->personGenderCmbBox->currentIndex();
    strncpy(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szNation,
            ui->personNationLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szNation));
    strncpy(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szProvince,
            ui->personProvinceLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szProvince));
    strncpy(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szCity,
            ui->personCityLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].stRegionInfo.szCity));
    stPersonInfoList.pstPersonInfo[0].stIdentificationInfo[0].udwType = ui->personIdentificationTypeCmbBox->currentIndex();
    strncpy(stPersonInfoList.pstPersonInfo[0].stIdentificationInfo[0].szNumber,
            ui->personIdentificationLineEdt->text().toStdString().c_str(),
            sizeof(stPersonInfoList.pstPersonInfo[0].stIdentificationInfo[0].szNumber));

    if(false == m_faceImagePath.isEmpty())/* 选择了新图片才修改 */
    {
        if(0 == stPersonInfoList.pstPersonInfo[0].udwImageNum)/* 原来没有图片 */
        {
            stPersonInfoList.pstPersonInfo[0].udwImageNum = 1;
        }

        QByteArray fileData;
        Common::getFileData(m_faceImagePath,fileData);
        QFileInfo fi = QFileInfo(m_faceImagePath);
        strncpy(stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.szName,
                fi.fileName().toStdString().c_str(),
                sizeof (stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.szName));
        stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.dwFileType = NETDEV_TYPE_FILE;
        stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.udwSize = fileData.size();
        memcpy(stPersonInfoList.pstPersonInfo[0].stImageInfo[0].stFileInfo.pcData,
                fileData.toStdString().c_str(),
                fileData.size());
    }

    NETDEV_PERSON_RESULT_LIST_S stPersonResultList = {0};
    stPersonResultList.udwNum = stPersonInfoList.udwNum;
    stPersonResultList.pstPersonList = new NETDEV_PERSON_LIST_S[stPersonResultList.udwNum];
    memset(stPersonResultList.pstPersonList,0,sizeof(NETDEV_PERSON_LIST_S) * stPersonResultList.udwNum);

    bool bRet = pDeviceManager->modifyPersonInfo(stItemData.dwLibID, stPersonInfoList, stPersonResultList);
    if(false == bRet)
    {
        qCritical() << "modify Person Info fail";
    }

    delete[] stPersonResultList.pstPersonList;
    findPersonInfo(stItemData.dwLibID, m_dwFindFaceLibMemberOffset);
}

/* 删除人员信息 */
void MainWidget::on_delPersonBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    QTreeWidgetItem* curItem = ui->faceLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    int dwCurRowIndex = ui->personInfoTabWgt->currentRow();
    if(m_faceLibMemberInfoList.size() > 0 && dwCurRowIndex >= 0)
    {
        bool bRet = pDeviceManager->deletePersonInfo(stItemData.dwLibID, m_faceLibMemberInfoList[dwCurRowIndex].udwPersonID);
        if(true == bRet)
        {
            m_dwFindFaceLibMemberOffset = 0;
            findPersonInfo(stItemData.dwLibID, m_dwFindFaceLibMemberOffset);
        }
        else {
            qCritical() << "NETDEV_DeletePersonInfo fail" << NETDEV_GetLastError();
        }
    }
}

void MainWidget::on_faceStartRealPlayBtn_clicked()
{
    QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
    if(NULL == item)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a channel"));
        return;
    }
    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select an online channel"));
        return;
    }

    startRealPlay(stItemData.dwChannelID,(LPVOID)ui->facePlayScreenLab->winId());
}

void MainWidget::on_faceStopRealPlayBtn_clicked()
{
    QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
    if(NULL == item)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a channel"));
        return;
    }

    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select an online channel"));
        return;
    }

    stopRealPlay(stItemData.dwChannelID);

    ui->facePlayScreenLab->setStyleSheet("background-color:black");
}

/* 订阅人脸识别告警 */
void MainWidget::on_SubFaceRecognitionBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    bool bRet = pDeviceManager->subFaceRecognitionAlarm(cbAlarmPersonCallBack,cbAlarmMessCallBack_V30,this);
    if(false == bRet)
    {
        qCritical() << "subFaceRecognitionAlarm Fail";
    }

    qInfo() << "subFaceRecognitionAlarm success";
}

/* 取消订阅人脸识别告警 */
void MainWidget::on_unSubFaceRecognitionBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    bool bRet = pDeviceManager->unSubFaceRecognitionAlarm();
    if(false == bRet)
    {
        qCritical() << "unSubscibeLapiAlarm Fail";
    }

    qInfo() << "unSubscibeLapiAlarm success";
}

/* 订阅人脸抓拍 */
void MainWidget::on_SubFaceSnapshotBtn_clicked()
{
    QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
    if(NULL == item)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a device"));
        return;
    }

    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(NETDEMO_DEVICE_VMS == stItemData.eDeviceType
            && (-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus))
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select an online channel"));
        return;
    }

    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    /* 订阅智能告警(结构化) */
    bool bRet = pDeviceManager->subFaceSnapshotAlarm(stItemData.dwChannelID, cbAlarmStructCallBack,cbAlarmMessCallBack_V30,this);
    if(FALSE == bRet)
    {
        qCritical() << "on_SubFaceSnapshotBtn_clicked Fail";
        return;
    }

    qInfo() << "on_SubFaceSnapshotBtn_clicked success";
}

/* 取消人脸抓拍订阅 */
void MainWidget::on_unSubFaceSnapshotBtn_clicked()
{
    QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
    if(NULL == item)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a device"));
        return;
    }

    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(NETDEMO_DEVICE_VMS == stItemData.eDeviceType
            && (-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus))
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select an online channel"));
        return;
    }

    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    bool bRet = pDeviceManager->unSubFaceSnapshotAlarm(stItemData.dwChannelID);
    if(false == bRet)
    {
        qCritical() << "on_unSubFaceSnapshotBtn_clicked Fail";
    }
    else {
        qInfo() << "on_unSubFaceSnapshotBtn_clicked success";
    }

}

void MainWidget::on_personFaceImageBtn_clicked()
{
    QString strFilePath = QFileDialog::getOpenFileName(this,tr("Open Binary File"),".",tr("Images (*.jpg)"));
    qDebug() << strFilePath << endl;
    if(strFilePath.isEmpty())
    {
        return;
    }

    /* 读取图片 */
    QByteArray fileData;
    if(false == Common::getFileData(strFilePath,fileData))
    {
        qCritical() << "read Image error";
        return;
    }

    m_faceImagePath = strFilePath;

    QImage imageSnapshotPic;
    imageSnapshotPic.loadFromData(fileData);

    ui->personFaceImageBtn->setIcon(QIcon(QPixmap::fromImage(imageSnapshotPic).scaled(ui->personFaceImageBtn->width(), ui->personFaceImageBtn->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    ui->personFaceImageBtn->setIconSize(QSize(ui->personFaceImageBtn->width(),ui->personFaceImageBtn->height()));
}

/* 选中指定人员后显示人员详细信息 */
void MainWidget::on_personInfoTabWgt_itemClicked(QTableWidgetItem *item)
{
    m_faceImagePath = "";

    QList<QTableWidgetItem*> items = ui->personInfoTabWgt->selectedItems();
    UINT32 personID = 0;
    if(items.count() > 0)
    {
        personID = items[0]->text().toUInt();
    }

    for(int i = 0; i < m_faceLibMemberInfoList.size(); i++)
    {
        if(personID == m_faceLibMemberInfoList[i].udwPersonID)
        {
            if(m_faceLibMemberInfoList[i].udwImageNum > 0)
            {
                QByteArray fileData(m_faceLibMemberInfoList[i].stImageInfo[0].stFileInfo.pcData,m_faceLibMemberInfoList[i].stImageInfo[0].stFileInfo.udwSize);
                QImage imageSnapshotPic;
                imageSnapshotPic.loadFromData(fileData);

                ui->personFaceImageBtn->setIcon(QIcon(QPixmap::fromImage(imageSnapshotPic).scaled(ui->personFaceImageBtn->width(), ui->personFaceImageBtn->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
                ui->personFaceImageBtn->setIconSize(QSize(ui->personFaceImageBtn->width(),ui->personFaceImageBtn->height()));
            }

            ui->personNameLineEdt->setText(QString(m_faceLibMemberInfoList[i].szPersonName));
            ui->personCityLineEdt->setText(QString(m_faceLibMemberInfoList[i].stRegionInfo.szCity));
            ui->personNationLineEdt->setText(QString(m_faceLibMemberInfoList[i].stRegionInfo.szNation));
            ui->personProvinceLineEdt->setText(QString(m_faceLibMemberInfoList[i].stRegionInfo.szProvince));
            ui->personBirthdayLineEdit->setText(QString(m_faceLibMemberInfoList[i].szBirthday));
            ui->personGenderCmbBox->setCurrentIndex(m_faceLibMemberInfoList[i].udwGender);

            if(m_faceLibMemberInfoList[i].udwIdentificationNum > 0)
            {
                ui->personIdentificationTypeCmbBox->setCurrentIndex(m_faceLibMemberInfoList[i].stIdentificationInfo[0].udwType);
                ui->personIdentificationLineEdt->setText(QString(m_faceLibMemberInfoList[i].stIdentificationInfo[0].szNumber));
            }

            break;
        }
    }
}

/* 查询人脸布控信息 */
void MainWidget::findFaceMonitorTaskInfo()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    clearFaceMonitorInfoTabWgt();

    QVariant variant = ui->realTimeVideoChlTreeWgt->currentItem()->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(NETDEMO_DEVICE_IPC_OR_NVR == stItemData.eDeviceType
            && (-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus))
    {
        QMessageBox::warning(this, tr("Demo"),tr("IPC/NVR: Please select an online channel"));
        return;
    }

    NETDEV_MONITOR_QUERY_INFO_S stQueryInfo = {0};
    stQueryInfo.udwLimit = 20;

    bool bRet = pDeviceManager->getFaceMonitorTaskInfo(stItemData.dwChannelID,stQueryInfo,m_faceMonitorTaskInfoList);
    if (false == bRet)
    {
        qCritical() << " getFaceMonitorTaskInfo fail ";
        return;
    }
    else
    {
        findAllPersonLibInfo();
        for(int i = 0; i < m_faceMonitorTaskInfoList.size(); i++)
        {
            showFaceMonitorTaskInfo(m_faceMonitorTaskInfoList[i]);
        }
    }
}

/* 查询布控信息 */
void MainWidget::on_findFaceMonitorTaskBtn_clicked()
{
    findFaceMonitorTaskInfo();
}

/* 选中指定布控后显示布控详细信息 */
void MainWidget::on_faceMonitorInfoTabWgt_itemClicked(QTableWidgetItem *item)
{
    QList<QTableWidgetItem*> items = ui->faceMonitorInfoTabWgt->selectedItems();
    UINT32 monitorID = 0;
    if(items.count() > 0)
    {
        monitorID = items[0]->text().toUInt();
    }

    for(int i = 0; i < m_faceMonitorTaskInfoList.size(); i++)
    {
        if(monitorID == m_faceMonitorTaskInfoList[i].udwID)
        {
            ui->faceMonitorTaskNameLineEdit->setText(QString(m_faceMonitorTaskInfoList[i].stMonitorRuleInfo.szName));
            ui->faceMonitorRemarksLineEdt->setText(QString(m_faceMonitorTaskInfoList[i].stMonitorRuleInfo.szReason));
            ui->faceMonitorAlarmThresholdSpinBox->setValue(m_faceMonitorTaskInfoList[i].stMonitorRuleInfo.udwMultipleValue);
            ui->faceMonitorTypeCmbBox->setCurrentIndex(m_faceMonitorTaskInfoList[i].stMonitorRuleInfo.udwMonitorType);
            for(int j = 0; j < ui->faceMonitorObjectCmbBox->count(); j++)
            {
                QVariant variant = ui->faceMonitorObjectCmbBox->itemData(j,Qt::UserRole);  // 获取用户数据
                NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
                if(stItemData.dwLibID == m_faceMonitorTaskInfoList[i].stMonitorRuleInfo.audwLibList[0])
                {
                    ui->faceMonitorObjectCmbBox->setCurrentIndex(j);
                    break;
                }
            }
            break;
        }
    }
}

/* 添加人脸布控 */
void MainWidget::on_addFaceMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    QVariant variant = ui->realTimeVideoChlTreeWgt->currentItem()->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();
    if(NETDEMO_DEVICE_IPC_OR_NVR == stItemData.eDeviceType
            && (-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus))
    {
        QMessageBox::warning(this, tr("Demo"),tr("IPC/NVR: Please select an online channel"));
        return;
    }

    if(NETDEMO_DEVICE_VMS == stItemData.eDeviceType
            && (-1 == stItemData.dwSubDeviceID || NETDEV_DEV_STATUS_ONLINE != stItemData.dwSubDeviceStatus))
    {
        QMessageBox::warning(this, tr("Demo"),tr("VMS: Please select an online sub device"));
        return;
    }

    NETDEV_MONITION_INFO_S stMonitorInfo = {0};
    stMonitorInfo.stMonitorRuleInfo.bEnabled = 1;
    strncpy(stMonitorInfo.stMonitorRuleInfo.szName,
            ui->faceMonitorTaskNameLineEdit->text().toStdString().c_str(),
            sizeof(stMonitorInfo.stMonitorRuleInfo.szName));
    strncpy(stMonitorInfo.stMonitorRuleInfo.szReason,
            ui->faceMonitorRemarksLineEdt->text().toStdString().c_str(),
            sizeof(stMonitorInfo.stMonitorRuleInfo.szReason));
    stMonitorInfo.stMonitorRuleInfo.udwMonitorType = ui->faceMonitorTypeCmbBox->currentIndex();

    stMonitorInfo.stMonitorRuleInfo.udwLibNum = 1;
    QVariant variant2 = ui->faceMonitorObjectCmbBox->itemData(ui->faceMonitorObjectCmbBox->currentIndex(),Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stLibItemData = variant2.value<NETDEMO_LIB_ITEM_DATA_S>();
    stMonitorInfo.stMonitorRuleInfo.audwLibList[0] = stLibItemData.dwLibID;
    if (NETDEMO_DEVICE_IPC_OR_NVR == stItemData.eDeviceType)
    {
        stMonitorInfo.stMonitorRuleInfo.udwMultipleValue = ui->faceMonitorAlarmThresholdSpinBox->value();
        stMonitorInfo.stMonitorRuleInfo.udwChannelNum = 1;
        stMonitorInfo.stMonitorRuleInfo.pudwMonitorChlIDList = new UINT32[stMonitorInfo.stMonitorRuleInfo.udwChannelNum];
        stMonitorInfo.stMonitorRuleInfo.pudwMonitorChlIDList[0] = stItemData.dwChannelID;

        stMonitorInfo.stWeekPlan.bEnabled = 1;
        stMonitorInfo.stWeekPlan.udwDayNum = 8;
        for (int i = 0; i < stMonitorInfo.stWeekPlan.udwDayNum; i++)
        {
            stMonitorInfo.stWeekPlan.astDayPlan[i].udwIndex = i + 1;
            stMonitorInfo.stWeekPlan.astDayPlan[i].udwSectionNum = 1;
            for (int j = 0; j < stMonitorInfo.stWeekPlan.astDayPlan[i].udwSectionNum; j++)
            {
                strncpy(stMonitorInfo.stWeekPlan.astDayPlan[i].astTimeSection[j].szBeginTime,
                        "00:00:00",
                        sizeof(stMonitorInfo.stWeekPlan.astDayPlan[i].astTimeSection[j].szBeginTime));
                strncpy(stMonitorInfo.stWeekPlan.astDayPlan[i].astTimeSection[j].szEndTime,
                        "23:59:59",
                        sizeof(stMonitorInfo.stWeekPlan.astDayPlan[i].astTimeSection[j].szEndTime));
                stMonitorInfo.stWeekPlan.astDayPlan[i].astTimeSection[j].udArmingType = NETDEV_ARMING_TYPE_TIMING;
            }
        }
    }
    else
    {
        stMonitorInfo.stMonitorRuleInfo.udwDevNum = 1;
        stMonitorInfo.stMonitorRuleInfo.audwMonitorDevIDList[0] = stItemData.dwSubDeviceID;
    }

    NETDEV_MONITOR_RESULT_INFO_S stMonitorResultInfo = {0};
    stMonitorResultInfo.udwChannelNum = 1;
    stMonitorResultInfo.pstMonitorChlInfos = new NETDEV_MONITION_CHL_INFO_S[stMonitorResultInfo.udwChannelNum];
    memset(stMonitorResultInfo.pstMonitorChlInfos,0,sizeof(NETDEV_MONITION_CHL_INFO_S) * stMonitorResultInfo.udwChannelNum);

    bool bRet = pDeviceManager->addFaceMonitorTaskInfo(stMonitorInfo, stMonitorResultInfo);
    if(false == bRet)
    {
        qCritical() << "on_addFaceMonitorTaskBtn_clicked fail";
    }
    else
    {
        qInfo() << "on_addFaceMonitorTaskBtn_clicked success";
        findFaceMonitorTaskInfo();
    }

    delete[] stMonitorResultInfo.pstMonitorChlInfos;
    if (NETDEMO_DEVICE_IPC_OR_NVR == stItemData.eDeviceType)
    {
        delete[] stMonitorInfo.stMonitorRuleInfo.pudwMonitorChlIDList;
    }
}

/* 修改人脸布控 */
void MainWidget::on_modifyFaceMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    int curRow = ui->faceMonitorInfoTabWgt->currentRow();
    if(curRow < 0)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Please select a monitoring Task"));
        return;
    }

    NETDEV_MONITION_INFO_S stMonitorInfo = m_faceMonitorTaskInfoList[curRow];
    strncpy(stMonitorInfo.stMonitorRuleInfo.szName,
            ui->faceMonitorTaskNameLineEdit->text().toStdString().c_str(),
            sizeof(stMonitorInfo.stMonitorRuleInfo.szName));
    strncpy(stMonitorInfo.stMonitorRuleInfo.szReason,
            ui->faceMonitorRemarksLineEdt->text().toStdString().c_str(),
            sizeof(stMonitorInfo.stMonitorRuleInfo.szReason));
    stMonitorInfo.stMonitorRuleInfo.udwMonitorType = ui->faceMonitorTypeCmbBox->currentIndex();
    QVariant variant2 = ui->faceMonitorObjectCmbBox->itemData(ui->faceMonitorObjectCmbBox->currentIndex(),Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stLibItemData = variant2.value<NETDEMO_LIB_ITEM_DATA_S>();
    stMonitorInfo.stMonitorRuleInfo.audwLibList[0] = stLibItemData.dwLibID;

    if (NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        stMonitorInfo.stMonitorRuleInfo.udwMultipleValue = ui->faceMonitorAlarmThresholdSpinBox->value();
    }

    bool bRet = pDeviceManager->modifyFaceMonitorTaskInfo(stMonitorInfo);
    if (false == bRet)
    {
        qCritical() << "on_modifyFaceMonitorTaskBtn_clicked fail";
    }
    else
    {
        qInfo() << "on_modifyFaceMonitorTaskBtn_clicked success";
        findFaceMonitorTaskInfo();
    }
}

/* 删除人脸布控 */
void MainWidget::on_delFaceMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    NETDEV_BATCH_OPERATOR_LIST_S stBatchList = {0};
    stBatchList.udwNum = 1;
    stBatchList.pstBatchList = new NETDEV_BATCH_OPERATOR_INFO_S[stBatchList.udwNum];
    memset(stBatchList.pstBatchList,0,sizeof(NETDEV_BATCH_OPERATOR_INFO_S) * stBatchList.udwNum);

    QList<QTableWidgetItem*> items = ui->faceMonitorInfoTabWgt->selectedItems();
    UINT32 monitorID = 0;
    if(items.count() == 0)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Please select a monitoring Task"));
        return;
    }
    monitorID = items[0]->text().toUInt();
    stBatchList.pstBatchList[0].udwID = monitorID;

    bool bRet = pDeviceManager->deleteFaceMonitorTaskInfo(stBatchList);
    if(false == bRet)
    {
        qCritical() << "on_delFaceMonitorTaskBtn_clicked fail";
    }
    else
    {
        qInfo() << "on_delFaceMonitorTaskBtn_clicked success";
        findFaceMonitorTaskInfo();
    }

    delete[] stBatchList.pstBatchList;
}

/* 启用人脸布控 */
void MainWidget::on_enableFaceMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    int curRow = ui->faceMonitorInfoTabWgt->currentRow();
    if(curRow < 0)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Please select a monitoring Task"));
        return;
    }

    NETDEV_MONITION_INFO_S stMonitorInfo = m_faceMonitorTaskInfoList[curRow];
    stMonitorInfo.stMonitorRuleInfo.bEnabled = TRUE;
    bool bRet = pDeviceManager->modifyFaceMonitorTaskInfo(stMonitorInfo);
    if (false == bRet)
    {
        qCritical() << "on_enableFaceMonitorTaskBtn_clicked fail";
    }
    else
    {
        qInfo() << "on_enableFaceMonitorTaskBtn_clicked success";
        findFaceMonitorTaskInfo();
    }
}

/* 停用人脸布控 */
void MainWidget::on_disableFaceMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    int curRow = ui->faceMonitorInfoTabWgt->currentRow();
    if(curRow < 0)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Please select a monitoring Task"));
        return;
    }

    NETDEV_MONITION_INFO_S stMonitorInfo = m_faceMonitorTaskInfoList[curRow];
    stMonitorInfo.stMonitorRuleInfo.bEnabled = FALSE;
    bool bRet = pDeviceManager->modifyFaceMonitorTaskInfo(stMonitorInfo);
    if (false == bRet)
    {
        qCritical() << "on_disableFaceMonitorTaskBtn_clicked fail";
    }
    else
    {
        qInfo() << "on_disableFaceMonitorTaskBtn_clicked success";
        findFaceMonitorTaskInfo();
    }
}

/* 显示人脸识别_人脸抓拍检索结果 */
void MainWidget::showFaceAlarmRecordInfo(NETDEV_FACE_RECORD_SNAPSHOT_INFO_S& stRecordInfo)
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    int rowCount = ui->faceSearchRecordInfoTabWgt->rowCount();
    ui->faceSearchRecordInfoTabWgt->insertRow(rowCount);

    /* 抓拍人脸全景图 */
    QLabel* pPanoImageLab = new QLabel();
    pPanoImageLab->resize(211,120);
    QImage panoImage;
    if(NETDEMO_DEVICE_VMS == pDeviceManager->m_eDevType)
    {
        NETDEV_FILE_INFO_S stFileInfo = {0};
        stFileInfo.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
        stFileInfo.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
        if(true == pDeviceManager->getFacePanoImageInfo(stRecordInfo.udwRecordID, stRecordInfo.udwRecordType,stFileInfo))
        {
            panoImage.loadFromData(QByteArray(stFileInfo.pcData, stFileInfo.udwSize));
            pPanoImageLab->setPixmap(QPixmap::fromImage(panoImage).scaled(pPanoImageLab->width(),
                                                                          pPanoImageLab->height(),
                                                                          Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        delete[] stFileInfo.pcData;
    }
    else
    {
        panoImage.loadFromData(QByteArray(stRecordInfo.stCompareInfo.stSnapshotImage.stBigImage.pcData,
                                          stRecordInfo.stCompareInfo.stSnapshotImage.stBigImage.udwSize));
        pPanoImageLab->setPixmap(QPixmap::fromImage(panoImage).scaled(pPanoImageLab->width(),
                                                                      pPanoImageLab->height(),
                                                                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    /* 抓拍人脸小图 */
    QLabel* pSnapshotImageLab = new QLabel();
    pSnapshotImageLab->resize(111,120);
    QImage snapshotImage;
    snapshotImage.loadFromData(QByteArray(stRecordInfo.stCompareInfo.stSnapshotImage.stSmallImage.pcData,
                                          stRecordInfo.stCompareInfo.stSnapshotImage.stSmallImage.udwSize));
    pSnapshotImageLab->setPixmap(QPixmap::fromImage(snapshotImage).scaled(pSnapshotImageLab->width(),
                                                                          pSnapshotImageLab->height(),
                                                                          Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    /* 人脸库小图,仅比对成功时给值 */
    QLabel* plibFaceImageLab = new QLabel();
    plibFaceImageLab->resize(111,120);
    QImage libFaceImage;
    if(NETDEV_TYPE_FACE_PASS_COM_SUCCESS == stRecordInfo.udwRecordType)
    {
        libFaceImage.loadFromData(QByteArray(stRecordInfo.stCompareInfo.stMemberInfo.stMemberImageInfo.pcData,
                                             stRecordInfo.stCompareInfo.stMemberInfo.stMemberImageInfo.udwSize));
        plibFaceImageLab->setPixmap(QPixmap::fromImage(libFaceImage).scaled(plibFaceImageLab->width(),
                                                                            plibFaceImageLab->height(),
                                                                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    /* 抓拍时间 */
    QTableWidgetItem* itemTime = new QTableWidgetItem(QDateTime::fromTime_t(stRecordInfo.udwPassTime).toString("yyyy-MM-dd HH:mm:ss"));
    /* 告警源 */
    QTableWidgetItem* itemAlarmSource = new QTableWidgetItem(QString(stRecordInfo.szChannelName));
    /* 姓名 */
    QTableWidgetItem* itemName = new QTableWidgetItem(QString(stRecordInfo.stCompareInfo.stMemberInfo.szMemberName));
    /* 证件号 */
    QTableWidgetItem* itemIDNumber = new QTableWidgetItem(QString(stRecordInfo.stCompareInfo.stMemberInfo.stMemberIDInfo.szNumber));
    /* 相似度 */
    QTableWidgetItem* itemSimi = new QTableWidgetItem(QString::number(stRecordInfo.stCompareInfo.udwSimilarity) + "%");

    itemTime->setTextAlignment(Qt::AlignCenter);
    itemAlarmSource->setTextAlignment(Qt::AlignCenter);
    itemName->setTextAlignment(Qt::AlignCenter);
    itemIDNumber->setTextAlignment(Qt::AlignCenter);
    itemSimi->setTextAlignment(Qt::AlignCenter);

    ui->faceSearchRecordInfoTabWgt->setCellWidget(rowCount,0,pPanoImageLab);
    ui->faceSearchRecordInfoTabWgt->setCellWidget(rowCount,1,pSnapshotImageLab);
    ui->faceSearchRecordInfoTabWgt->setCellWidget(rowCount,2,plibFaceImageLab);
    ui->faceSearchRecordInfoTabWgt->setItem(rowCount, 3, itemTime);
    ui->faceSearchRecordInfoTabWgt->setItem(rowCount, 4, itemAlarmSource);
    ui->faceSearchRecordInfoTabWgt->setItem(rowCount, 5, itemName);
    ui->faceSearchRecordInfoTabWgt->setItem(rowCount, 6, itemIDNumber);
    ui->faceSearchRecordInfoTabWgt->setItem(rowCount, 7, itemSimi);
}

/* 人脸识别_人脸抓拍检索 */
void MainWidget::findFaceAlarmRecordInfo(UINT32 udwOffset)
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    int rowCount = ui->faceSearchRecordInfoTabWgt->rowCount();
    for(int i = 0;i < rowCount;i++)
    {
        ui->faceSearchRecordInfoTabWgt->removeRow(0);
    }

    NETDEV_ALARM_LOG_COND_LIST_S stFindCond = {0};
    stFindCond.dwPageRow = NETDEMO_FIND_SMART_ALARM_RECORD_COUNT;
    stFindCond.dwFirstRow = (int)udwOffset;
    stFindCond.dwCondSize = 0;

    stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_TIME;
    stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_NO_LESS;
    QString strBeginTime = QString::number(ui->faceAlarmSearchBeginTimeDTEdit->dateTime().toTime_t());
    strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
            strBeginTime.toStdString().c_str(),
            sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
    stFindCond.dwCondSize++;

    stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_TIME;
    stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_NO_GREATER;
    QString strEndTime = QString::number(ui->faceAlarmSearchEndTimeDTEdit->dateTime().toTime_t());
    strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
            strEndTime.toStdString().c_str(),
            sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
    stFindCond.dwCondSize++;

    stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_TIME;
    stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_DESC_ORDER;
    stFindCond.dwCondSize++;

    if (NETDEMO_FIND_FACE_ALARM_RECORD_RECOGNITION == ui->faceRecordSearchTypeCmbBox->currentIndex())
    {
        stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_ALARMTYPE;
        stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_EQUAL;
        if (0 == ui->faceAlarmTypeCmbBox->currentIndex())/* 匹配告警 */
        {
            strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                    QString::number(NETDEV_ALARM_SMART_FACE_MATCH_LIST).toStdString().c_str(),
                    sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
        }
        else
        {
            strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                    QString::number(NETDEV_ALARM_SMART_FACE_MISMATCH_LIST).toStdString().c_str(),
                    sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
        }
        stFindCond.dwCondSize += 1;
    }
    else
    {
        if (false == ui->faceAlarmSourceLineEdit->text().isEmpty())
        {
            stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_ALARMSRCNAME;
            stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_DIM_QUERY;
            strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                    ui->faceAlarmSourceLineEdit->text().toStdString().c_str(),
                    sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
            stFindCond.dwCondSize += 1;
        }
    }

    QList<NETDEV_FACE_RECORD_SNAPSHOT_INFO_S> faceSearchRecordInfoList;

    bool bRet = pDeviceManager->searchFaceRecordInfo(stFindCond,faceSearchRecordInfoList);
    if(false == bRet)
    {
        qCritical() << "searchFaceRecordInfo fail";
        return;
    }

    /* 显示人脸识别-抓拍检索记录信息 */
    for(int i = 0; i < faceSearchRecordInfoList.size(); i++)
    {
        showFaceAlarmRecordInfo(faceSearchRecordInfoList[i]);

        delete faceSearchRecordInfoList[i].stCompareInfo.stMemberInfo.stMemberSemiInfo.pcData;
        delete faceSearchRecordInfoList[i].stCompareInfo.stMemberInfo.stMemberImageInfo.pcData;
        delete faceSearchRecordInfoList[i].stCompareInfo.stSnapshotImage.stBigImage.pcData;
        delete faceSearchRecordInfoList[i].stCompareInfo.stSnapshotImage.stSmallImage.pcData;
    }
}

/* 人脸识别-抓拍记录检索 */
void MainWidget::on_faceRecogRecordSerachBtn_clicked()
{
    m_dwFindFaceAlarmRecordOffset = 0;
    findFaceAlarmRecordInfo(m_dwFindFaceAlarmRecordOffset);
}

/* 人脸识别-抓拍记录检索，上一页 */
void MainWidget::on_faceRecogRecordPrieviousBtn_clicked()
{
    if (m_dwFindFaceAlarmRecordOffset > NETDEMO_FIND_SMART_ALARM_RECORD_COUNT)
    {
        m_dwFindFaceAlarmRecordOffset -= NETDEMO_FIND_SMART_ALARM_RECORD_COUNT;
    }
    else
    {
        m_dwFindFaceAlarmRecordOffset = 0;
    }

    findFaceAlarmRecordInfo(m_dwFindFaceAlarmRecordOffset);
}

/* 人脸识别-抓拍记录检索，下一页 */
void MainWidget::on_faceRecogRecordNextBtn_clicked()
{
    m_dwFindFaceAlarmRecordOffset += NETDEMO_FIND_SMART_ALARM_RECORD_COUNT;
    findFaceAlarmRecordInfo(m_dwFindFaceAlarmRecordOffset);
}

/* 显示车牌识别_车牌抓拍检索结果 */
void MainWidget::showPlateAlarmRecordInfo(NETDEV_VEHICLE_RECORD_INFO_S& stRecordInfo)
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    int rowCount = ui->plateSearchRecordInfoTabWgt->rowCount();
    ui->plateSearchRecordInfoTabWgt->insertRow(rowCount);

    /* 全景图 */
    QLabel* pPanoImageLab = new QLabel();
    pPanoImageLab->resize(211,120);
    QImage panoImage;
    if(NETDEMO_DEVICE_VMS == pDeviceManager->m_eDevType)
    {
        NETDEV_FILE_INFO_S stFileInfo = {0};
        stFileInfo.pcData = new CHAR[NETDEMO_SMALL_IMAGE_SIZE];
        stFileInfo.udwSize = NETDEMO_SMALL_IMAGE_SIZE;
        if(true == pDeviceManager->getPlatePanoImageInfo(stRecordInfo.udwRecordID,stFileInfo))
        {
            panoImage.loadFromData(QByteArray(stFileInfo.pcData, stFileInfo.udwSize));
            pPanoImageLab->setPixmap(QPixmap::fromImage(panoImage).scaled(pPanoImageLab->width(),
                                                                          pPanoImageLab->height(),
                                                                          Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        delete[] stFileInfo.pcData;
    }
    else
    {
        panoImage.loadFromData(QByteArray(stRecordInfo.stPanoImage.pcData,
                                          stRecordInfo.stPanoImage.udwSize));
        pPanoImageLab->setPixmap(QPixmap::fromImage(panoImage).scaled(pPanoImageLab->width(),
                                                                      pPanoImageLab->height(),
                                                                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    /* 车牌图 */
    QLabel* pPlateImageLab = new QLabel();
    pPlateImageLab->resize(120,65);
    QImage plateImage;
    plateImage.loadFromData(QByteArray(stRecordInfo.stPlateImage.pcData,
                                       stRecordInfo.stPlateImage.udwSize));
    pPlateImageLab->setPixmap(QPixmap::fromImage(plateImage).scaled(pPlateImageLab->width(),
                                                                       pPlateImageLab->height(),
                                                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    /* 车牌号 */
    QTableWidgetItem* itemPlateNo = new QTableWidgetItem(QString(stRecordInfo.stPlateAttr.szPlateNo));
    /* 抓拍时间 */
    QTableWidgetItem* itemTime = new QTableWidgetItem(QDateTime::fromTime_t(stRecordInfo.udwPassingTime).toString("yyyy-MM-dd HH:mm:ss"));
    /* 车牌颜色 */
    QTableWidgetItem* itemPlateColor = new QTableWidgetItem(gPlateColorMap.value(stRecordInfo.stPlateAttr.udwColor));
    /* 告警源 */
    QTableWidgetItem* itemAlarmSource = new QTableWidgetItem(QString(stRecordInfo.szChannelName));

    itemTime->setTextAlignment(Qt::AlignCenter);
    itemAlarmSource->setTextAlignment(Qt::AlignCenter);
    itemPlateNo->setTextAlignment(Qt::AlignCenter);
    itemPlateColor->setTextAlignment(Qt::AlignCenter);

    ui->plateSearchRecordInfoTabWgt->setCellWidget(rowCount,0,pPanoImageLab);
    ui->plateSearchRecordInfoTabWgt->setCellWidget(rowCount,1,pPlateImageLab);
    ui->plateSearchRecordInfoTabWgt->setItem(rowCount, 2, itemPlateNo);
    ui->plateSearchRecordInfoTabWgt->setItem(rowCount, 3, itemTime);
    ui->plateSearchRecordInfoTabWgt->setItem(rowCount, 4, itemPlateColor);
    ui->plateSearchRecordInfoTabWgt->setItem(rowCount, 5, itemAlarmSource);
}

/* 车牌识别_抓拍检索 */
void MainWidget::findVehicleAlarmRecordInfo(UINT32 udwOffset)
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    int rowCount = ui->plateSearchRecordInfoTabWgt->rowCount();
    for(int i = 0;i < rowCount;i++)
    {
        ui->plateSearchRecordInfoTabWgt->removeRow(0);
    }

    NETDEV_ALARM_LOG_COND_LIST_S stFindCond = {0};
    stFindCond.dwPageRow = NETDEMO_FIND_SMART_ALARM_RECORD_COUNT;
    stFindCond.dwFirstRow = (int)udwOffset;
    stFindCond.dwCondSize = 0;

    stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_VEH_DATA_TYPE;
    stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_DIM_QUERY;
    strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
            "0",sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
    stFindCond.dwCondSize++;

    stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_TIME;
    stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_NO_LESS;
    QString strBeginTime = QString::number(ui->plateAlarmSearchBeginTimeDTEdit->dateTime().toTime_t());
    strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
            strBeginTime.toStdString().c_str(),
            sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
    stFindCond.dwCondSize++;

    stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_TIME;
    stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_NO_GREATER;
    QString strEndTime = QString::number(ui->plateAlarmSearchEndTimeDTEdit->dateTime().toTime_t());
    strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
            strEndTime.toStdString().c_str(),
            sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
    stFindCond.dwCondSize++;

    stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_TIME;
    stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_DESC_ORDER;
    stFindCond.dwCondSize++;

    if (false == ui->plateSearchCameraNameLineEdit->text().isEmpty())
    {
        stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_CHNNAME;
        stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_DIM_QUERY;
        strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                ui->plateSearchCameraNameLineEdit->text().toStdString().c_str(),
                sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
        stFindCond.dwCondSize += 1;
    }

    if (false == ui->plateSearchPlateNoLineEdit->text().isEmpty())
    {
        stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_PLATE_NUM;
        stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_DIM_QUERY;
        strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                ui->plateSearchPlateNoLineEdit->text().toStdString().c_str(),
                sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
        stFindCond.dwCondSize += 1;
    }

    if ("All" != ui->plateSearchPlateColorCmbBox->currentText())
    {
        stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_PLATE_COLOR;
        stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_EQUAL;
        strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                QString::number(gPlateColorMap.key(ui->plateSearchPlateColorCmbBox->currentText())).toStdString().c_str(),
                sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
        stFindCond.dwCondSize += 1;
    }

    if ("All" != ui->plateSearchVehicleColorCmbBox->currentText())
    {
        stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_VEHICLE_COLOR;
        stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_EQUAL;
        strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                QString::number(gPlateColorMap.key(ui->plateSearchVehicleColorCmbBox->currentText())).toStdString().c_str(),
                sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
        stFindCond.dwCondSize += 1;
    }

    if (NETDEMO_FIND_PLATE_ALARM_RECORD_RECOGNITION == ui->plateSearchPlateColorCmbBox->currentIndex())/* 车牌识别 */
    {
        stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_ALARMTYPE;
        stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_EQUAL;
        if (0 == ui->plateAlarmTypeCmbBox->currentIndex())/* Match Alarm */
        {
            strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                    QString::number(NETDEV_ALARM_SMART_VEHICLE_MATCH_LIST).toStdString().c_str(),
                    sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
        }
        else
        {
            strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                    QString::number(NETDEV_ALARM_SMART_VEHICLE_MISMATCH_LIST).toStdString().c_str(),
                    sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
        }
        stFindCond.dwCondSize += 1;

        if ("All" != ui->plateSearchMonitorCauseCmbBox->currentText()
                && 0 == ui->plateAlarmTypeCmbBox->currentIndex())/* Match Alarm */
        {
            stFindCond.astCondition[stFindCond.dwCondSize].dwQueryType = NETDEV_QUERYCOND_MONITOY_REASON;
            stFindCond.astCondition[stFindCond.dwCondSize].dwLogicFlag = NETDEV_QUERYCOND_LOGIC_EQUAL;
            strncpy(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData,
                    QString::number(ui->plateSearchMonitorCauseCmbBox->currentIndex()).toStdString().c_str(),
                    sizeof(stFindCond.astCondition[stFindCond.dwCondSize].szConditionData));
            stFindCond.dwCondSize += 1;
        }
    }

    QList<NETDEV_VEHICLE_RECORD_INFO_S> plateSearchRecordInfoList;

    bool bRet = pDeviceManager->searchPlateRecordInfo(stFindCond,plateSearchRecordInfoList);
    if(false == bRet)
    {
        qCritical() << "searchPlateRecordInfo fail";
        return;
    }

    /* 显示车牌识别-抓拍检索记录信息 */
    for(int i = 0; i < plateSearchRecordInfoList.size(); i++)
    {
        showPlateAlarmRecordInfo(plateSearchRecordInfoList[i]);

        delete plateSearchRecordInfoList[i].stPlateImage.pcData;
        delete plateSearchRecordInfoList[i].stVehicleImage.pcData;
        delete plateSearchRecordInfoList[i].stPanoImage.pcData;
    }
}

/* 车牌识别-抓拍记录检索 */
void MainWidget::on_plateRecogRecordSerachBtn_clicked()
{
    m_dwFindVehicleAlarmRecordOffset = 0;
    findVehicleAlarmRecordInfo(m_dwFindVehicleAlarmRecordOffset);
}

/* 车牌识别-抓拍记录检索，上一页 */
void MainWidget::on_plateRecogRecordPreviousSerachBtn_clicked()
{
    if (m_dwFindVehicleAlarmRecordOffset > NETDEMO_FIND_SMART_ALARM_RECORD_COUNT)
    {
        m_dwFindVehicleAlarmRecordOffset -= NETDEMO_FIND_SMART_ALARM_RECORD_COUNT;
    }
    else
    {
        m_dwFindVehicleAlarmRecordOffset = 0;
    }

    findVehicleAlarmRecordInfo(m_dwFindVehicleAlarmRecordOffset);
}

/* 车牌识别-抓拍记录检索，下一页 */
void MainWidget::on_plateRecogRecordNextSerachBtn_clicked()
{
    m_dwFindVehicleAlarmRecordOffset += NETDEMO_FIND_SMART_ALARM_RECORD_COUNT;
    findVehicleAlarmRecordInfo(m_dwFindVehicleAlarmRecordOffset);
}

/* 清除车辆布控缓存信息 */
void MainWidget::clearVehicleMonitorInfoTabWgt()
{
    int rowCount = ui->plateMonitorTaskInfoTabWgt->rowCount();
    for(int i = 0;i < rowCount;i++)
    {
        ui->plateMonitorTaskInfoTabWgt->removeRow(0);
    }

    for(int i = 0; i < m_vehicleMonitorTaskInfoList.size(); i++)
    {
        delete[] m_vehicleMonitorTaskInfoList[i].stMonitorRuleInfo.pudwMonitorChlIDList;
        delete[] m_vehicleMonitorTaskInfoList[i].pstLinkStrategyList;
    }
    m_vehicleMonitorTaskInfoList.clear();
}

/* 显示人脸布控任务信息 */
void MainWidget::showVehicleMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo)
{
    int rowCount = ui->plateMonitorTaskInfoTabWgt->rowCount();
    ui->plateMonitorTaskInfoTabWgt->insertRow(rowCount);

    QString strStatus = "";
    if (FALSE == stMonitorInfo.stMonitorRuleInfo.bEnabled)
    {
        strStatus = "Disable";
    }
    else
    {
        strStatus = "Enable";
    }

    QString strAlarmType = "";
    if (0 == stMonitorInfo.stMonitorRuleInfo.udwMonitorType)
    {
        strAlarmType = "Match Alarm";
    }
    else
    {
        strAlarmType = "Not Match Alarm";
    }

    QString strLibName = "";
    for(int i = 0; i < ui->plateMonitorObjectCmbBox->count(); i++)
    {
        QVariant variant = ui->plateMonitorObjectCmbBox->itemData(i,Qt::UserRole);  // 获取用户数据
        NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
        if(stItemData.dwLibID == stMonitorInfo.stMonitorRuleInfo.audwLibList[0])
        {
            strLibName = stItemData.libName;
            break;
        }
    }

    QTableWidgetItem* itemID = new QTableWidgetItem(QString::number(stMonitorInfo.udwID));
    QTableWidgetItem* itemTaskName = new QTableWidgetItem(tr(stMonitorInfo.stMonitorRuleInfo.szName));
    QTableWidgetItem* itemszReason = new QTableWidgetItem(tr(stMonitorInfo.stMonitorRuleInfo.szReason));
    QTableWidgetItem* itemAlarmType = new QTableWidgetItem(strAlarmType);
    QTableWidgetItem* itemMonitoringObject = new QTableWidgetItem(strLibName);
    QTableWidgetItem* itemStatus = new QTableWidgetItem(strStatus);

    itemID->setTextAlignment(Qt::AlignCenter);
    itemTaskName->setTextAlignment(Qt::AlignCenter);
    itemszReason->setTextAlignment(Qt::AlignCenter);
    itemAlarmType->setTextAlignment(Qt::AlignCenter);
    itemMonitoringObject->setTextAlignment(Qt::AlignCenter);
    itemStatus->setTextAlignment(Qt::AlignCenter);

    ui->plateMonitorTaskInfoTabWgt->setItem(rowCount, 0, itemID);
    ui->plateMonitorTaskInfoTabWgt->setItem(rowCount, 1, itemTaskName);
    ui->plateMonitorTaskInfoTabWgt->setItem(rowCount, 2, itemszReason);
    ui->plateMonitorTaskInfoTabWgt->setItem(rowCount, 3, itemAlarmType);
    ui->plateMonitorTaskInfoTabWgt->setItem(rowCount, 4, itemMonitoringObject);
    ui->plateMonitorTaskInfoTabWgt->setItem(rowCount, 5, itemStatus);
}

/*查询车辆布控任务信息*/
void MainWidget::findVehicleMonitorTaskInfo()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    clearVehicleMonitorInfoTabWgt();

    QVariant variant = ui->realTimeVideoChlTreeWgt->currentItem()->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(NETDEMO_DEVICE_IPC_OR_NVR == stItemData.eDeviceType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("IPC/NVR: Not Supported"));
        return;
    }

    bool bRet = pDeviceManager->getVehicleMonitorTaskInfo(m_vehicleMonitorTaskInfoList);
    if (false == bRet)
    {
        qCritical() << " getVehicleMonitorTaskInfo fail ";
        return;
    }
    else
    {
        findVehicleLibInfo();
        for(int i = 0; i < m_vehicleMonitorTaskInfoList.size(); i++)
        {
            showVehicleMonitorTaskInfo(m_vehicleMonitorTaskInfoList[i]);
        }
    }
}

/* 查询车牌布控任务 */
void MainWidget::on_searchPlateMonitorTaskBtn_clicked()
{
    findVehicleMonitorTaskInfo();
}

/* 添加车牌布控任务 */
void MainWidget::on_addPlateMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if (NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("IPC/NVR: Not Supported"));
        return;
    }

    NETDEV_MONITION_INFO_S stMonitorInfo = {0};
    stMonitorInfo.stMonitorRuleInfo.bEnabled = 1;
    strncpy(stMonitorInfo.stMonitorRuleInfo.szName,
            ui->plateMonitorTaskNameLineEdit->text().toStdString().c_str(),
            sizeof(stMonitorInfo.stMonitorRuleInfo.szName));
    strncpy(stMonitorInfo.stMonitorRuleInfo.szReason,
            ui->plateMonitorRemarksLineEdit->text().toStdString().c_str(),
            sizeof(stMonitorInfo.stMonitorRuleInfo.szReason));
    stMonitorInfo.stMonitorRuleInfo.udwMonitorType = ui->plateMonitorAlarmTypeCmbBox->currentIndex();
    stMonitorInfo.stMonitorRuleInfo.udwMonitorReason = ui->plateMonitorCauseCmbBox->currentIndex();
    stMonitorInfo.stMonitorRuleInfo.udwLibNum = 1;
    QVariant variant = ui->plateMonitorObjectCmbBox->itemData(ui->plateMonitorObjectCmbBox->currentIndex(),Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stLibItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
    stMonitorInfo.stMonitorRuleInfo.audwLibList[0] = stLibItemData.dwLibID;

    bool bRet = pDeviceManager->addVehicleMonitorTaskInfo(stMonitorInfo);
    if (false == bRet)
    {
        qCritical() << "addVehicleMonitorTaskInfo fail";
    }
    else {
        findVehicleMonitorTaskInfo();
    }
}

/* 修改车牌布控任务 */
void MainWidget::on_modifyPlateMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if (NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("IPC/NVR: Not Supported"));
        return;
    }

    int curRow = ui->plateMonitorTaskInfoTabWgt->currentRow();
    if(curRow < 0)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a monitoring task"));
        return;
    }

    NETDEV_MONITION_INFO_S stMonitorInfo = m_vehicleMonitorTaskInfoList[curRow];
    strncpy(stMonitorInfo.stMonitorRuleInfo.szName,
            ui->plateMonitorTaskNameLineEdit->text().toStdString().c_str(),
            sizeof(stMonitorInfo.stMonitorRuleInfo.szName));
    strncpy(stMonitorInfo.stMonitorRuleInfo.szReason,
            ui->plateMonitorRemarksLineEdit->text().toStdString().c_str(),
            sizeof(stMonitorInfo.stMonitorRuleInfo.szReason));
    stMonitorInfo.stMonitorRuleInfo.udwMonitorType = ui->plateMonitorAlarmTypeCmbBox->currentIndex();
    stMonitorInfo.stMonitorRuleInfo.udwMonitorReason = ui->plateMonitorCauseCmbBox->currentIndex();
    stMonitorInfo.stMonitorRuleInfo.udwLibNum = 1;
    QVariant variant = ui->plateMonitorObjectCmbBox->itemData(ui->plateMonitorObjectCmbBox->currentIndex(),Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stLibItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
    stMonitorInfo.stMonitorRuleInfo.audwLibList[0] = stLibItemData.dwLibID;

    bool bRet = pDeviceManager->modifyVehicleMonitorTaskInfo(stMonitorInfo);
    if (false == bRet)
    {
        qCritical() << "modifyVehicleMonitorTaskInfo fail";
    }
    else {
        findVehicleMonitorTaskInfo();
    }
}

/* 删除车牌布控任务 */
void MainWidget::on_deletePlateMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if (NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("IPC/NVR: Not Supported"));
        return;
    }

    int curRow = ui->plateMonitorTaskInfoTabWgt->currentRow();
    if(curRow < 0)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a monitoring task"));
        return;
    }

    UINT32 udwMonitorTaskID = m_vehicleMonitorTaskInfoList[curRow].udwID;
    NETDEV_BATCH_OPERATOR_LIST_S stBatchList = {0};
    stBatchList.udwNum = 1;
    stBatchList.pstBatchList = new NETDEV_BATCH_OPERATOR_INFO_S[stBatchList.udwNum];
    memset(stBatchList.pstBatchList,0, sizeof(NETDEV_BATCH_OPERATOR_INFO_S) * stBatchList.udwNum);
    stBatchList.pstBatchList[0].udwID = udwMonitorTaskID;
    bool bRet = pDeviceManager->deleteVehicleMonitorTaskInfo(stBatchList);
    if (false == bRet)
    {
        qCritical() << "deleteVehicleMonitorTaskInfo fail";
    }
    else{
        findVehicleMonitorTaskInfo();
    }

    delete[] stBatchList.pstBatchList;
}

/* 启用车牌布控任务 */
void MainWidget::on_enablePlateMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if (NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Not Supported"));
        return;
    }

    int curRow = ui->plateMonitorTaskInfoTabWgt->currentRow();
    if(curRow < 0)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a monitoring task"));
        return;
    }

    NETDEV_MONITION_INFO_S stMonitorInfo = m_vehicleMonitorTaskInfoList[curRow];
    stMonitorInfo.stMonitorRuleInfo.bEnabled = TRUE;

    bool bRet = pDeviceManager->modifyVehicleMonitorTaskInfo(stMonitorInfo);
    if (false == bRet)
    {
        qCritical() << "modifyVehicleMonitorTaskInfo fail";
    }
    else {
        findVehicleMonitorTaskInfo();
    }
}

/* 停用车牌布控任务 */
void MainWidget::on_disablePlateMonitorTaskBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if (NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Not Supported"));
        return;
    }

    int curRow = ui->plateMonitorTaskInfoTabWgt->currentRow();
    if(curRow < 0)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a monitoring task"));
        return;
    }

    NETDEV_MONITION_INFO_S stMonitorInfo = m_vehicleMonitorTaskInfoList[curRow];
    stMonitorInfo.stMonitorRuleInfo.bEnabled = FALSE;

    bool bRet = pDeviceManager->modifyVehicleMonitorTaskInfo(stMonitorInfo);
    if (false == bRet)
    {
        qCritical() << "modifyVehicleMonitorTaskInfo fail";
    }
    else {
        findVehicleMonitorTaskInfo();
    }
}

/* 开启车辆实况 */
void MainWidget::on_vehicleStartRealPlayBtn_clicked()
{
    QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
    if(NULL == item)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a channel"));
        return;
    }

    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select an online channel"));
        return;
    }

    startRealPlay(stItemData.dwChannelID,(LPVOID)ui->platePlayScreenLab->winId());
}

/* 停止车辆实况 */
void MainWidget::on_vehicleStopRealPlayBtn_clicked()
{
    QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
    if(NULL == item)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a channel"));
        return;
    }

    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select an online channel"));
        return;
    }

    stopRealPlay(stItemData.dwChannelID);

    ui->platePlayScreenLab->setStyleSheet("background-color:black");
}

/* 订阅车辆识别告警 */
void MainWidget::on_SubPlateRecognitionBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    bool bRet = pDeviceManager->subPlateRecognitionAlarm(cbAlarmMessCallBack_V30,cbVehicleAlarmMessCallBack,this);
    if(false == bRet)
    {
        qCritical() << "subPlateRecognitionAlarm Fail";
    }
    else {
        qInfo() << "subPlateRecognitionAlarm success";
    }
}

/* 取消订阅车辆识别告警 */
void MainWidget::on_unSubPlateRecognitionBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    bool bRet = pDeviceManager->unSubPlateRecognitionAlarm();
    if(false == bRet)
    {
        qCritical() << "unSubPlateRecognitionAlarm Fail";
    }
    else {
        qInfo() << "unSubPlateRecognitionAlarm success";
    }
}

/* 订阅车辆抓拍告警 */
void MainWidget::on_SubPlateSnapshotBtn_clicked()
{
    QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
    if(NULL == item)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a device"));
        return;
    }

    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(NETDEMO_DEVICE_VMS == stItemData.eDeviceType
            && (-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus))
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select an online channel"));
        return;
    }

    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    /* 订阅智能告警(结构化) */
    bool bRet = pDeviceManager->subPlateSnapshotAlarm(stItemData.dwChannelID, cbAlarmStructCallBack,cbAlarmMessCallBack_V30,this);
    if(FALSE == bRet)
    {
        qCritical() << "on_SubPlateSnapshotBtn_clicked Fail";
        return;
    }

    qInfo() << "on_SubPlateSnapshotBtn_clicked success";
}

/* 取消订阅车辆抓拍告警 */
void MainWidget::on_unSubPlateSnapshotBtn_clicked()
{
    QTreeWidgetItem *item = ui->realTimeVideoChlTreeWgt->currentItem();
    if(NULL == item)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a device"));
        return;
    }

    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    if(NETDEMO_DEVICE_VMS == stItemData.eDeviceType
            && (-1 == stItemData.dwChannelID || NETDEV_CHL_STATUS_ONLINE != stItemData.eChannelStatus))
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select an online channel"));
        return;
    }

    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    bool bRet = pDeviceManager->unSubPlateSnapshotAlarm(stItemData.dwChannelID);
    if(false == bRet)
    {
        qCritical() << "on_unSubPlateSnapshotBtn_clicked Fail";
    }
    else {
        qInfo() << "on_unSubPlateSnapshotBtn_clicked success";
    }
}

/* 添加车辆库信息 */
void MainWidget::on_addVehicleLibBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Not Supported"));
        return;
    }

    NETDEV_LIB_INFO_S stLibInfo = {0};

    if(ui->vehicleLibNameLineEdit->text().isEmpty())
    {
        /* 找一个没有被使用过的库名字 */
        int dwlibIndex = 0;
        while(true)
        {
            QString strLibName = tr("新车辆库");
            strLibName += QString::number(dwlibIndex+1);
            int subItemIndex = 0;
            for(; subItemIndex < ui->vehicleLibTreeWgt->topLevelItemCount(); subItemIndex++)
            {
                if(strLibName == ui->vehicleLibTreeWgt->topLevelItem(subItemIndex)->text(0).split("(")[0])
                {
                    break;
                }
            }

            if(subItemIndex == ui->vehicleLibTreeWgt->topLevelItemCount())
            {
                strncpy(stLibInfo.szName,strLibName.toStdString().c_str(),sizeof(stLibInfo.szName));
                break;
            }

            dwlibIndex++;
        }
    }
    else {
        strncpy(stLibInfo.szName,ui->vehicleLibNameLineEdit->text().toStdString().c_str(),sizeof(stLibInfo.szName));
    }

    bool bRet = pDeviceManager->addVehicleLibInfo(stLibInfo);
    if(false == bRet)
    {
        qCritical() << "addVehicleLibInfo fail";
        return;
    }

    /* 界面显示 */
    QTreeWidgetItem* item = new QTreeWidgetItem();
    NETDEMO_LIB_ITEM_DATA_S stItemData;
    stItemData.dwLibID = stLibInfo.udwID;
    stItemData.libName = QString(stLibInfo.szName);
    item->setText(0, QString(stLibInfo.szName) + "("
                      + QString::number(stLibInfo.udwPersonNum) + "/"
                      + QString::number(stLibInfo.udwFaceNum) + ")");
    item->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
    ui->vehicleLibTreeWgt->addTopLevelItem(item);
}

/* 修改车辆库信息 */
void MainWidget::on_modifyVehicleLibBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Not Supported"));
        return;
    }

    QTreeWidgetItem* curItem = ui->vehicleLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a vihicle lib"));
        return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    NETDEV_LIB_INFO_S stLibInfo = {0};
    stLibInfo.udwID = stItemData.dwLibID;
    strncpy(stLibInfo.szName,ui->vehicleLibNameLineEdit->text().toStdString().c_str(),sizeof(stLibInfo.szName));

    NETDEV_PERSON_LIB_LIST_S stLibList = {0};
    stLibList.udwNum = 1;
    stLibList.pstLibInfo = new NETDEV_LIB_INFO_S[stLibList.udwNum];
    stLibList.pstLibInfo[0] = stLibInfo;
    bool bRet = pDeviceManager->modifyVehicleLibInfo(stLibList);
    if(false == bRet)
    {
        qCritical() << "modifyVehicleLibInfo fail:" << NETDEV_GetLastError();
    }
    else {
        on_refreshVehicleLibBtn_clicked();
    }

    delete[] stLibList.pstLibInfo;
}

/* 删除车辆库信息 */
void MainWidget::on_deleteVehicleLibBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Not Supported"));
        return;
    }

    QTreeWidgetItem* curItem = ui->vehicleLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a vihicle lib"));
        return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
    NETDEV_DELETE_DB_FLAG_INFO_S stFlagInfo = {0};
    stFlagInfo.bIsDeleteMember = TRUE;
    bool bRet = pDeviceManager->deleteVehicleLibInfo(stItemData.dwLibID,stFlagInfo);
    if(false == bRet)
    {
        qCritical() << "deleteVehicleLibInfo fail" << NETDEV_GetLastError();
    }
    else {
        on_refreshVehicleLibBtn_clicked();
    }
}

/* 查询车辆库信息 */
void MainWidget::findVehicleLibInfo()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    ui->vehicleLibTreeWgt->clear();
    ui->plateMonitorObjectCmbBox->clear();

    QList<NETDEV_LIB_INFO_S> stLibInfoList;
    pDeviceManager->getVehicleLibInfo(stLibInfoList);

    for(int i = 0; i < stLibInfoList.size(); i++)
    {
        /* 界面显示 */
        QTreeWidgetItem* item = new QTreeWidgetItem();
        NETDEMO_LIB_ITEM_DATA_S stItemData;
        stItemData.dwLibID = stLibInfoList[i].udwID;
        stItemData.libName = QString(stLibInfoList[i].szName);
        item->setText(0, QString(stLibInfoList[i].szName) + "("
                      + QString::number(stLibInfoList[i].udwMemberNum) + ")");
        item->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
        ui->vehicleLibTreeWgt->addTopLevelItem(item);

        ui->plateMonitorObjectCmbBox->addItem(QString(stLibInfoList[i].szName));
        ui->plateMonitorObjectCmbBox->setItemData(i,QVariant::fromValue(stItemData),Qt::UserRole);
    }
}

/* 刷新(查询)车辆库信息 */
void MainWidget::on_refreshVehicleLibBtn_clicked()
{
    findVehicleLibInfo();
}

/* 添加车辆信息 */
void MainWidget::on_addVehicleBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Not Supported"));
        return;
    }

    QTreeWidgetItem* curItem = ui->vehicleLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a vihicle lib"));
        return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    NETDEV_VEHICLE_INFO_LIST_S stVehicleMemberList = {0};
    stVehicleMemberList.udwVehicleNum = 1;
    stVehicleMemberList.pstMemberInfoList = new NETDEV_VEHICLE_DETAIL_INFO_S[stVehicleMemberList.udwVehicleNum];
    memset(stVehicleMemberList.pstMemberInfoList,0,sizeof(NETDEV_VEHICLE_DETAIL_INFO_S) * stVehicleMemberList.udwVehicleNum);
    strncpy(stVehicleMemberList.pstMemberInfoList[0].stPlateAttr.szPlateNo,
            ui->plateNoLineEdit->text().toStdString().c_str(),
            sizeof(stVehicleMemberList.pstMemberInfoList[0].stPlateAttr.szPlateNo));
    stVehicleMemberList.pstMemberInfoList[0].stPlateAttr.udwColor = gPlateColorMap.key(ui->plateColorCmbBox->currentText());
    stVehicleMemberList.pstMemberInfoList[0].stPlateAttr.udwType = gPlateTypeMap.key(ui->plateTypeCmbBox->currentText());
    stVehicleMemberList.pstMemberInfoList[0].stVehicleAttr.udwColor = gPlateColorMap.key(ui->vehicleColorCmbBox->currentText());

    NETDEV_BATCH_OPERATOR_LIST_S stResultList = {0};
    stResultList.udwNum = stVehicleMemberList.udwVehicleNum;
    stResultList.pstBatchList = new NETDEV_BATCH_OPERATOR_INFO_S[stResultList.udwNum];

    bool bRet = pDeviceManager->addVehicleInfo(stItemData.dwLibID, stVehicleMemberList, stResultList);
    if (false == bRet)
    {
        qCritical() << "addVehicleInfo fail" << NETDEV_GetLastError();
    }
    else
    {
        findVehicleInfo(stItemData.dwLibID, 0);
    }

    delete[] stVehicleMemberList.pstMemberInfoList;
    delete[] stResultList.pstBatchList;

}

/* 修改车辆信息 */
void MainWidget::on_modifyVehicleBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Not Supported"));
        return;
    }

    QTreeWidgetItem* curItem = ui->vehicleLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a vihicle lib"));
        return;
    }

    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    NETDEV_VEHICLE_INFO_LIST_S stVehicleMemberList = {0};
    stVehicleMemberList.udwVehicleNum = 1;
    stVehicleMemberList.pstMemberInfoList = new NETDEV_VEHICLE_DETAIL_INFO_S[stVehicleMemberList.udwVehicleNum];
    stVehicleMemberList.pstMemberInfoList[0] = m_vehicleLibMemberInfoList[ui->vehicleInfoTabWgt->currentRow()];
    strncpy(stVehicleMemberList.pstMemberInfoList[0].stPlateAttr.szPlateNo,
            ui->plateNoLineEdit->text().toStdString().c_str(),
            sizeof(stVehicleMemberList.pstMemberInfoList[0].stPlateAttr.szPlateNo));
    stVehicleMemberList.pstMemberInfoList[0].stPlateAttr.udwColor = gPlateColorMap.key(ui->plateColorCmbBox->currentText());
    stVehicleMemberList.pstMemberInfoList[0].stPlateAttr.udwType = gPlateTypeMap.key(ui->plateTypeCmbBox->currentText());
    stVehicleMemberList.pstMemberInfoList[0].stVehicleAttr.udwColor = gPlateColorMap.key(ui->vehicleColorCmbBox->currentText());

    NETDEV_BATCH_OPERATOR_LIST_S stResultList = {0};
    stResultList.udwNum = stVehicleMemberList.udwVehicleNum;
    stResultList.pstBatchList = new NETDEV_BATCH_OPERATOR_INFO_S[stResultList.udwNum];

    bool bRet = pDeviceManager->modifyVehicleInfo(stItemData.dwLibID, stVehicleMemberList, stResultList);
    if (false == bRet)
    {
        qCritical() << "modifyVehicleInfo fail" << NETDEV_GetLastError();
    }
    else
    {
        findVehicleInfo(stItemData.dwLibID, 0);
    }

    delete[] stVehicleMemberList.pstMemberInfoList;
    delete[] stResultList.pstBatchList;
}

/* 删除车辆信息 */
void MainWidget::on_deleteVehicleBtn_clicked()
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    if(NETDEMO_DEVICE_IPC_OR_NVR == pDeviceManager->m_eDevType)
    {
        QMessageBox::warning(this, tr("Demo"),tr("Not Supported"));
        return;
    }

    QTreeWidgetItem* curItem = ui->vehicleLibTreeWgt->currentItem();
    if(nullptr == curItem)
    {
        QMessageBox::warning(this, tr("Demo"),tr("please select a vihicle lib"));
        return;
    }
    QVariant variant = curItem->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    NETDEV_VEHICLE_INFO_LIST_S stVehicleMemberList = {0};
    stVehicleMemberList.udwVehicleNum = 1;
    stVehicleMemberList.pstMemberInfoList = new NETDEV_VEHICLE_DETAIL_INFO_S[stVehicleMemberList.udwVehicleNum];
    stVehicleMemberList.pstMemberInfoList[0] = m_vehicleLibMemberInfoList[ui->vehicleInfoTabWgt->currentRow()];

    NETDEV_BATCH_OPERATOR_LIST_S stResutList = {0};
    stResutList.udwNum = stVehicleMemberList.udwVehicleNum;
    stResutList.pstBatchList = new NETDEV_BATCH_OPERATOR_INFO_S[stResutList.udwNum];

    bool bRet = pDeviceManager->deleteVehicleInfo(stItemData.dwLibID, stVehicleMemberList, stResutList);
    if (false == bRet)
    {
        qCritical() << "deleteVehicleLibInfo fail" << NETDEV_GetLastError();
    }
    else
    {
        findVehicleInfo(stItemData.dwLibID,0);
    }

    delete[] stVehicleMemberList.pstMemberInfoList;
    delete[] stResutList.pstBatchList;
}

/* 查找车辆信息，上一页 */
void MainWidget::on_searchVehiclePreviousBtn_clicked()
{
    QTreeWidgetItem* item = ui->vehicleLibTreeWgt->currentItem();
    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    if(m_dwFindVehicleLibMemberOffset > NETDEMO_FIND_LIB_MEM_COUNT)
    {
        m_dwFindVehicleLibMemberOffset -= NETDEMO_FIND_LIB_MEM_COUNT;
    }
    else {
        m_dwFindVehicleLibMemberOffset = 0;
    }

    findVehicleInfo(stItemData.dwLibID, m_dwFindVehicleLibMemberOffset);
}

/* 查找车辆信息，下一页 */
void MainWidget::on_searchVehicleNextBtn_clicked()
{
    QTreeWidgetItem* item = ui->vehicleLibTreeWgt->currentItem();
    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();

    m_dwFindVehicleLibMemberOffset += NETDEMO_FIND_LIB_MEM_COUNT;

    findVehicleInfo(stItemData.dwLibID, m_dwFindVehicleLibMemberOffset);
}

/* 选中指定车辆布控后显示布控详细信息 */
void MainWidget::on_plateMonitorTaskInfoTabWgt_itemClicked(QTableWidgetItem *item)
{
    QList<QTableWidgetItem*> items = ui->plateMonitorTaskInfoTabWgt->selectedItems();
    UINT32 monitorID = 0;
    if(items.count() > 0)
    {
        monitorID = items[0]->text().toUInt();
    }

    for(int i = 0; i < m_vehicleMonitorTaskInfoList.size(); i++)
    {
        if(monitorID == m_vehicleMonitorTaskInfoList[i].udwID)
        {
            ui->plateMonitorTaskNameLineEdit->setText(QString(m_vehicleMonitorTaskInfoList[i].stMonitorRuleInfo.szName));
            ui->plateMonitorRemarksLineEdit->setText(QString(m_vehicleMonitorTaskInfoList[i].stMonitorRuleInfo.szReason));
            ui->plateMonitorCauseCmbBox->setCurrentIndex(m_vehicleMonitorTaskInfoList[i].stMonitorRuleInfo.udwMonitorReason);
            ui->plateMonitorAlarmTypeCmbBox->setCurrentIndex(m_vehicleMonitorTaskInfoList[i].stMonitorRuleInfo.udwMonitorType);
            for(int j = 0; j < ui->plateMonitorObjectCmbBox->count(); j++)
            {
                QVariant variant = ui->plateMonitorObjectCmbBox->itemData(j,Qt::UserRole);  // 获取用户数据
                NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
                if(stItemData.dwLibID == m_vehicleMonitorTaskInfoList[i].stMonitorRuleInfo.audwLibList[0])
                {
                    ui->plateMonitorObjectCmbBox->setCurrentIndex(j);
                    break;
                }
            }
            break;
        }
    }
}

/* 清除当前显示的车辆信息 */
void MainWidget::clearVehicleInfoTabWgt()
{
    int rowCount = ui->vehicleInfoTabWgt->rowCount();
    for(int i = 0;i < rowCount;i++)
    {
        ui->vehicleInfoTabWgt->removeRow(0);
    }

    for(int i = 0; i < m_vehicleLibMemberInfoList.size(); i++)
    {
        /*释放内存*/
        delete[] m_vehicleLibMemberInfoList[i].stVehicleAttr.stVehicleImage.pcData;
    }
    m_vehicleLibMemberInfoList.clear();
}

/* 界面显示车辆信息 */
void MainWidget::showVehicleInfo(NETDEV_VEHICLE_DETAIL_INFO_S &stVehicleInfo)
{
    int rowCount = ui->vehicleInfoTabWgt->rowCount();
    ui->vehicleInfoTabWgt->insertRow(rowCount);

    QString strPlateColor;
    strPlateColor = gPlateColorMap.value(stVehicleInfo.stPlateAttr.udwColor);

    QString strPlateType;
    strPlateType = gPlateTypeMap.value(stVehicleInfo.stPlateAttr.udwType);

    QString strVehicleColor;
    strVehicleColor = gPlateColorMap.value(stVehicleInfo.stVehicleAttr.udwColor);

    QString strMonitorStatus;
    if(TRUE == stVehicleInfo.bIsMonitored)
    {
        strMonitorStatus = "In Use";
    }
    else {
        strMonitorStatus = "Not In Use";
    }

    QTableWidgetItem* itemID = new QTableWidgetItem(QString::number(stVehicleInfo.udwMemberID));
    QTableWidgetItem* itemPlateNo = new QTableWidgetItem(tr(stVehicleInfo.stPlateAttr.szPlateNo));
    QTableWidgetItem* itemPlateColor = new QTableWidgetItem(strPlateColor);
    QTableWidgetItem* itemPlateType = new QTableWidgetItem(strPlateType);
    QTableWidgetItem* itemVehicleColor = new QTableWidgetItem(strVehicleColor);
    QTableWidgetItem* itemMonitorStatus = new QTableWidgetItem(strMonitorStatus);

    itemID->setTextAlignment(Qt::AlignCenter);
    itemPlateNo->setTextAlignment(Qt::AlignCenter);
    itemPlateColor->setTextAlignment(Qt::AlignCenter);
    itemPlateType->setTextAlignment(Qt::AlignCenter);
    itemVehicleColor->setTextAlignment(Qt::AlignCenter);
    itemMonitorStatus->setTextAlignment(Qt::AlignCenter);



    ui->vehicleInfoTabWgt->setItem(rowCount, 0,itemID);
    ui->vehicleInfoTabWgt->setItem(rowCount, 1, itemPlateNo);
    ui->vehicleInfoTabWgt->setItem(rowCount, 2, itemPlateColor);
    ui->vehicleInfoTabWgt->setItem(rowCount, 3, itemPlateType);
    ui->vehicleInfoTabWgt->setItem(rowCount, 4, itemVehicleColor);
    ui->vehicleInfoTabWgt->setItem(rowCount, 5, itemMonitorStatus);
}

/* 查询人脸库人员信息 */
void MainWidget::findVehicleInfo(UINT32 udwVehicleLibID, int dwOffset)
{
    DeviceManager* pDeviceManager = getCurSelectedDevice();
    if(NULL == pDeviceManager)
    {
        return;
    }

    clearVehicleInfoTabWgt();

    NETDEV_PERSON_QUERY_INFO_S stQueryInfo = {0};
    stQueryInfo.udwLimit = 6;/* 每次查询6条成员记录 */
    stQueryInfo.udwOffset = dwOffset;
    NETDEV_BATCH_OPERATE_BASIC_S stQueryResultInfo = {0};
    pDeviceManager->getVehicleInfo(udwVehicleLibID, stQueryInfo,stQueryResultInfo,m_vehicleLibMemberInfoList);
    for(int i = 0; i < m_vehicleLibMemberInfoList.size(); i++)
    {
        /* 界面展示 */
        showVehicleInfo(m_vehicleLibMemberInfoList[i]);
    }
}

/* 单击车辆库项槽 */
void MainWidget::on_vehicleLibTreeWgt_itemClicked(QTreeWidgetItem *item, int column)
{
    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_LIB_ITEM_DATA_S stItemData = variant.value<NETDEMO_LIB_ITEM_DATA_S>();
    ui->vehicleLibNameLineEdit->setText(stItemData.libName);
    m_dwFindVehicleLibMemberOffset = 0;
    findVehicleInfo(stItemData.dwLibID, m_dwFindVehicleLibMemberOffset);
}

/* 选中指定车辆后显示其详细信息 */
void MainWidget::on_vehicleInfoTabWgt_itemClicked(QTableWidgetItem *item)
{
    QList<QTableWidgetItem*> items = ui->vehicleInfoTabWgt->selectedItems();
    UINT32 vehicleID = 0;
    if(items.count() > 0)
    {
        vehicleID = items[0]->text().toUInt();
    }

    for(int i = 0; i < m_vehicleLibMemberInfoList.size(); i++)
    {
        if(vehicleID == m_vehicleLibMemberInfoList[i].udwMemberID)
        {
            ui->plateNoLineEdit->setText(QString(m_vehicleLibMemberInfoList[i].stPlateAttr.szPlateNo));
            ui->plateColorCmbBox->setCurrentText(gPlateColorMap.value(m_vehicleLibMemberInfoList[i].stPlateAttr.udwColor));
            ui->plateTypeCmbBox->setCurrentText(gPlateTypeMap.value(m_vehicleLibMemberInfoList[i].stPlateAttr.udwType));
            ui->vehicleColorCmbBox->setCurrentText(gPlateColorMap.value(m_vehicleLibMemberInfoList[i].stVehicleAttr.udwColor));
            break;
        }
    }
}
