#include "adudemo.h"
#include "publictype.h"

#include <QMouseEvent>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include "playscreen.h"

AduDemo::AduDemo(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("ADUDemo");
    resize(1000, 650);

//    QPalette pal(palette());
//    pal.setColor(QPalette::Background, QColor(50, 50, 50));
//    setAutoFillBackground(true);
//    setPalette(pal);

    NETDEV_Init();

    m_pTVWallConfigDlg = new TVWallConfigDlg(this);
    m_pAddDeviceDlg = new AddDeviceDlg(this);

    m_videoSourceTreeWgt = new MTreeWidget();
    m_TVWallOptionListWgt = new QListWidget();
    m_PlayPannelWgt = new MPlayWidget();
    m_pWndVirtualLEDTabWgt = new QTabWidget();
    m_AddDecoderBtn = new QPushButton(QString("添加"));
    m_AddVideoSourceBtn = new QPushButton(QString("添加"));
    m_delDecoderBtn = new QPushButton(QString("删除"));
    m_videoSourceAddTypeCmb = new QComboBox();
    m_decoderListCmb = new QComboBox();

    m_videoSourceAddTypeCmb->addItem(tr("SDK获取"));
    m_videoSourceAddTypeCmb->addItem(tr("手动配置"));

    m_TVWallOptionListWgt->setFlow(QListView::LeftToRight);//横向显示
    m_TVWallOptionListWgt->setViewMode(QListView::IconMode);
    m_TVWallOptionListWgt->setDragDropMode(QAbstractItemView::NoDragDrop);
    m_TVWallOptionListWgt->setMaximumHeight(40);
    m_TVWallOptionListWgt->horizontalScrollBar()->setHidden(true);
    m_TVWallOptionListWgt->verticalScrollBar()->setHidden(true);
    m_TVWallOptionListWgt->setContextMenuPolicy(Qt::CustomContextMenu);
    m_TVWallOptionListWgt->setStyleSheet("QListWidget{border:1px solid gray; color:black; }"
                                         "QListWidget::Item{background:lightgray; }"
                                         "QListWidget::Item:hover{background:yellow; }"
                                         "QListWidget::item:selected{background:skyblue; color:red; }"
                                         "QListWidget::item:selected:!active{border-width:0px; background:lightgreen; }"
                                         );

    m_videoSourceTreeWgt->setColumnCount(1);
    m_videoSourceTreeWgt->setHeaderLabel("视频源信息");
    m_videoSourceTreeWgt->header()->setDefaultAlignment(Qt::AlignHCenter);
    m_videoSourceTreeWgt->setDefaultDropAction(Qt::MoveAction);
    m_videoSourceTreeWgt->setDragDropMode(QAbstractItemView::DragOnly);

    QListWidgetItem *item = new QListWidgetItem("Add TVWall");
    item->setSizeHint(QSize(80,35));
    m_TVWallOptionListWgt->addItem(item);

    m_pWndVirtualLEDTabWgt->setMaximumHeight(100);

    /* 界面布局 */
    QGroupBox* decoderGBx = new QGroupBox(QString("解码器"));
    decoderGBx->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    QGridLayout* decoderLayout = new QGridLayout();
    decoderLayout->addWidget(m_decoderListCmb,0,0,1,2);
    decoderLayout->addWidget(m_AddDecoderBtn,1,0,1,1);
    decoderLayout->addWidget(m_delDecoderBtn,1,1,1,1);
    decoderGBx->setLayout(decoderLayout);

    QGroupBox* encoderGBx = new QGroupBox(QString("视频源"));
    encoderGBx->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    QGridLayout* encoderLayout = new QGridLayout();
    encoderLayout->addWidget(m_videoSourceAddTypeCmb,0,0,1,1);
    encoderLayout->addWidget(m_AddVideoSourceBtn,0,1,1,1);
    encoderLayout->addWidget(m_videoSourceTreeWgt,1,0,1,2);
    encoderGBx->setLayout(encoderLayout);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(decoderGBx);
    leftLayout->addWidget(encoderGBx);

    QWidget* playControlWgt = new QWidget();
    playControlWgt->setMaximumHeight(20);
    QComboBox* splitScreenCmb = new QComboBox();
    splitScreenCmb->addItem(tr("1分屏"));
    splitScreenCmb->addItem(tr("4分屏"));
    splitScreenCmb->addItem(tr("9分屏"));
    splitScreenCmb->addItem(tr("16分屏"));

    QHBoxLayout* playControlLayout = new QHBoxLayout();
    playControlLayout->setMargin(0);
    playControlLayout->setSpacing(0);
    playControlLayout->addWidget(splitScreenCmb);
    playControlWgt->setLayout(playControlLayout);

    m_pWndVirtualLEDTabWgt->addTab(playControlWgt,tr("窗口"));
    QLabel * la = new QLabel("测试");
    m_pWndVirtualLEDTabWgt->addTab(la,tr("虚拟LED"));

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_TVWallOptionListWgt);
    rightLayout->addWidget(m_PlayPannelWgt);
    rightLayout->addWidget(m_pWndVirtualLEDTabWgt);

    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    this->setLayout(mainLayout);

    /* 信号槽 */
    connect(m_pWndVirtualLEDTabWgt,SIGNAL(currentChanged(int)),this,SLOT(onWndVirtualLEDTabWgtCurChangedSlot(int)));
    connect(splitScreenCmb,SIGNAL(currentIndexChanged(int)),this,SLOT(onSplitScreenCmbCurIndexChangedSlot(int)));
    connect(m_AddDecoderBtn, SIGNAL(clicked()), this, SLOT(on_AddDecoderBtn_clicked()));
    connect(m_delDecoderBtn, SIGNAL(clicked()), this, SLOT(on_delDecoderBtn_clicked()));
    connect(m_AddVideoSourceBtn, SIGNAL(clicked()), this, SLOT(on_AddVideoSourceBtn_clicked()));
    connect(m_TVWallOptionListWgt, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotClickedTvWallOptionListWgtItem(QListWidgetItem*)));
    connect(m_TVWallOptionListWgt,SIGNAL(customContextMenuRequested (const QPoint&)), this, SLOT(onTVWallOptionListContextMenuSlot(const QPoint&)));
    connect(m_pAddDeviceDlg,SIGNAL(addDeviceSignal(NETDEMO_DEVICE_LOGIN_INFO_S)),this,SLOT(AddDeviceSlot(NETDEMO_DEVICE_LOGIN_INFO_S)));
    connect(m_pTVWallConfigDlg,SIGNAL(updateTVWallSignal(int, bool, NETDEV_XW_TVWALL_CFG_S)),this,SLOT(updateTVWallSlot(int, bool, NETDEV_XW_TVWALL_CFG_S)));
}

AduDemo::~AduDemo()
{

}

void AduDemo::mousePressEvent(QMouseEvent *event)
{
//    if (event->button()==Qt::LeftButton)
//    {
//        mouse_press = true;
//        move_point = event->pos();
//    }
}

void AduDemo::mouseReleaseEvent(QMouseEvent *event)
{
//    Q_UNUSED(event);
//    mouse_press = false;
}

void AduDemo::mouseMoveEvent(QMouseEvent *event)
{
//    if (mouse_press)
//    {
//        QPoint move_pos = event->globalPos();
//        this->move(move_pos - move_point);
//    }
}

void AduDemo::closeEvent (QCloseEvent* e)
{
    if( QMessageBox::question(this,
                             tr("Quit"),
                             tr("Are you sure to quit this application?"),
                              QMessageBox::Yes, QMessageBox::No )
                   == QMessageBox::Yes){
        e->accept();

        /* 退出前，清理当前电视墙窗口，确保析构时被动解码被关闭 */
        m_PlayPannelWgt->clearTVWallDisplayPage();
        qDebug()<<"ok";
    }
    else
    {
      e->ignore();
    }
}

void AduDemo::onWndVirtualLEDTabWgtCurChangedSlot(int dwCurrentIndex)
{
    int dwCurIndex = m_TVWallOptionListWgt->currentRow();
    int dwCount = m_TVWallOptionListWgt->count();
    if(dwCurIndex == dwCount -1) /* 选择的是添加电视墙 */
    {
        return;
    }
    else /* 选择的是已有电视墙 */
    {
        /*切换展示当前电视墙*/
        switchShowTVWall(dwCurIndex);
    }
}

void AduDemo::slotClickedTvWallOptionListWgtItem(QListWidgetItem* item)
{
    int dwCount = m_TVWallOptionListWgt->count();
    int dwCurIndex = m_TVWallOptionListWgt->currentRow();
    if(dwCurIndex == dwCount -1) /* 选择的是添加电视墙 */
    {
        if(nullptr == m_pTVWallConfigDlg)
        {
            m_pTVWallConfigDlg = new TVWallConfigDlg();
        }

        m_pTVWallConfigDlg->init();
        m_pTVWallConfigDlg->show();
    }
    else /* 选择的是已有电视墙 */
    {
        /*切换展示当前电视墙*/
        switchShowTVWall(dwCurIndex);
    }
}

/* 修改电视墙信息 */
void AduDemo::onModifyTVWallConfigSlot()
{
    int dwCurIndex = m_TVWallOptionListWgt->currentRow();

    QListWidgetItem* item = m_TVWallOptionListWgt->item(dwCurIndex);
    if(NULL == item)
    {
        return;
    }
    QVariant variant = item->data(Qt::UserRole);  // 获取用户数据
    NETDEMO_TVWALL_ITEM_DATA_S stItemData = variant.value<NETDEMO_TVWALL_ITEM_DATA_S>();

    LPNETDEV_XW_TVWALL_CFG_S pstTVWallCfg = getTVWallInfo(stItemData.dwDecoderIndex,stItemData.udwTVWallID);
    if(NULL == pstTVWallCfg)
    {
        return;
    }

    m_pTVWallConfigDlg->TVWallModifyInfo(stItemData.dwDecoderIndex,*pstTVWallCfg);
    m_pTVWallConfigDlg->show();
}

/* 删除电视墙信息 */
void AduDemo::onDelTVWallConfigSlot()
{
    /* 删除电视墙 */
    QVariant variant = m_TVWallOptionListWgt->currentItem()->data(Qt::UserRole);  // 获取用户数据
    NETDEMO_TVWALL_ITEM_DATA_S stItemData = variant.value<NETDEMO_TVWALL_ITEM_DATA_S>();
    UINT32 udwLastChange = 0;
    BOOL bRet = NETDEV_XW_DeleteTVWallCfg(gDecoderList[stItemData.dwDecoderIndex].lpLoginHandle,
                                            stItemData.udwTVWallID,
                                            &udwLastChange);
    if(FALSE == bRet)
    {
        return;
    }

    qDebug() << "del TVWall success";

    /* 从界面列表中移除 */
    int dwDelRow = m_TVWallOptionListWgt->currentRow();
    QListWidgetItem* pDelItem = m_TVWallOptionListWgt->takeItem(dwDelRow);
    if(NULL != pDelItem)
    {
        delete pDelItem;
    }

    /* 从电视墙缓存中移除 */
    removeTVWallInfo(stItemData.dwDecoderIndex,stItemData.udwTVWallID);

    /* 如果还有下一项，切换到下一项展示，如果没有下一项，清空电视墙显示界面 */
    int dwCount = m_TVWallOptionListWgt->count();
    int dwCurIndex = m_TVWallOptionListWgt->currentRow();
    if(dwCurIndex == dwCount -1) /* 当前项是添加电视墙,说明已经没有下一项 */
    {
        m_PlayPannelWgt->clearTVWallDisplayPage();/*清空电视墙显示界面*/
    }
    else /* 选择的是已有电视墙 */
    {
        /*切换展示当前电视墙*/
        switchShowTVWall(dwCurIndex);
    }

}

void AduDemo::onTVWallOptionListContextMenuSlot(const QPoint& pt)
{
    QListWidgetItem* plistItem = m_TVWallOptionListWgt->itemAt(pt);

    if(NULL == plistItem)
    {
        return;
    }

    int dwCount = m_TVWallOptionListWgt->count();
    int dwCurIndex = m_TVWallOptionListWgt->currentRow();
    if(dwCurIndex == dwCount -1)
    {
        return;
    }

    //构造右键菜单
    QMenu *pMenu = new QMenu(this);
    QAction *modifyTVWallConfigAction = new QAction(tr("修改电视墙"), this);
    QAction *delTVWallConfigAction = new QAction(tr("删除电视墙"), this);
    pMenu->addAction(modifyTVWallConfigAction);
    pMenu->addAction(delTVWallConfigAction);

    connect(modifyTVWallConfigAction,SIGNAL(triggered()),this,SLOT(onModifyTVWallConfigSlot()));
    connect(delTVWallConfigAction,SIGNAL(triggered()),this,SLOT(onDelTVWallConfigSlot()));

    //显示菜单
    pMenu->exec(m_TVWallOptionListWgt->mapToGlobal(pt));

    delete pMenu;
    delete modifyTVWallConfigAction;
    delete delTVWallConfigAction;
}

void AduDemo::on_AddDecoderBtn_clicked()
{
    if(nullptr == m_pAddDeviceDlg)
    {
        m_pAddDeviceDlg = new AddDeviceDlg();
    }

    m_pAddDeviceDlg->setDeviceType(NETDEMO_DEVICE_TYPE_DECODER);
    m_pAddDeviceDlg->show();
}

/* 删除解码器 */
void AduDemo::on_delDecoderBtn_clicked()
{
    int decoderIndex = m_decoderListCmb->currentIndex();
    if(decoderIndex < 0)
    {
        return;
    }

    /* 释放内存 */
    if(nullptr != gDecoderList[decoderIndex].stCapInfo.stSupport.stPaneType.pudwPaneType)
    {
        delete[] gDecoderList[decoderIndex].stCapInfo.stSupport.stPaneType.pudwPaneType;
    }

    if(nullptr != gDecoderList[decoderIndex].stChannelsList.pstAudioIn)
    {
        delete[] gDecoderList[decoderIndex].stChannelsList.pstAudioIn;
    }

    if(nullptr != gDecoderList[decoderIndex].stChannelsList.pstVideoIn)
    {
        delete[] gDecoderList[decoderIndex].stChannelsList.pstVideoIn;
    }

    if(nullptr != gDecoderList[decoderIndex].stChannelsList.pstAudioOut)
    {
        delete[] gDecoderList[decoderIndex].stChannelsList.pstAudioOut;
    }

    if(nullptr != gDecoderList[decoderIndex].stChannelsList.pstVideoOut)
    {
        delete[] gDecoderList[decoderIndex].stChannelsList.pstVideoOut;
    }

    /* 登出设备 */
    NETDEV_Logout(gDecoderList[decoderIndex].lpLoginHandle);
    gDecoderList.removeAt(decoderIndex);
    m_decoderListCmb->clear();

    /* 界面移除电视墙相关信息 */
    int count = m_TVWallOptionListWgt->count();
    for(int i = 0; i < count - 1; i++)
    {
        QListWidgetItem* pDelItem = m_TVWallOptionListWgt->takeItem(0);
        if(NULL != pDelItem)
        {
            delete pDelItem;
        }
    }

    /*清空电视墙显示界面*/
    m_PlayPannelWgt->clearTVWallDisplayPage();
}

void AduDemo::on_AddVideoSourceBtn_clicked()
{
    if(nullptr == m_pAddDeviceDlg)
    {
        m_pAddDeviceDlg = new AddDeviceDlg();
    }

    m_pAddDeviceDlg->setDeviceType(NETDEMO_DEVICE_TYPE_ENCODER, (NETDEMO_VIDEO_SOURCE_ADD_TYPE_E)m_videoSourceAddTypeCmb->currentIndex());
    m_pAddDeviceDlg->show();
}

void AduDemo::AddDeviceSlot(NETDEMO_DEVICE_LOGIN_INFO_S stDeviceLoginInfo)
{
    if(NETDEMO_DEVICE_TYPE_DECODER == stDeviceLoginInfo.eDeviceType
            && gDecoderList.size() > 0)/* demo仅允许登录一台解码器 */
    {
        QMessageBox::warning(this,"ADUDemo","demo仅允许登录一台解码器");
        return;
    }

    qDebug() << stDeviceLoginInfo.strIpAddress << stDeviceLoginInfo.dwPort
             << stDeviceLoginInfo.strUserName << stDeviceLoginInfo.strPassword;

    LPVOID lpLoginHandle = NULL;
    if(NETDEMO_VIDEO_SOURCE_ADD_MANUAL != stDeviceLoginInfo.eVideoSourceAddType)
    {
        NETDEV_DEVICE_INFO_S stDeviceInfo = {0};
        lpLoginHandle = NETDEV_Login(stDeviceLoginInfo.strIpAddress.toLatin1().data(),
                     stDeviceLoginInfo.dwPort,
                     stDeviceLoginInfo.strUserName.toLatin1().data(),
                     stDeviceLoginInfo.strPassword.toLatin1().data(),
                     &stDeviceInfo);
        if(nullptr == lpLoginHandle)
        {
            qDebug() << "Login Fail";
            return;
        }
    }

    qDebug() << "Login Success";



    if(NETDEMO_DEVICE_TYPE_ENCODER == stDeviceLoginInfo.eDeviceType)
    {
        AddEncoder(lpLoginHandle, stDeviceLoginInfo);
    }
    else
    {
        AddDecoder(lpLoginHandle, stDeviceLoginInfo);
    }
}

void AduDemo::AddEncoder(LPVOID lpLoginHandle, NETDEMO_DEVICE_LOGIN_INFO_S stDeviceInfo)
{
    NETDEMO_ENCODER_INFO_S stEncoderInfo;
    stEncoderInfo.lpLoginHandle = NULL;

    if(NETDEMO_VIDEO_SOURCE_ADD_SDK == stDeviceInfo.eVideoSourceAddType)/* SDK添加 */
    {
        NETDEV_DEVICE_BASICINFO_S stDeviceBaseInfo = {0};
        int dwBytesReturned = 0;
        BOOL bRet = NETDEV_GetDevConfig(lpLoginHandle,0,NETDEV_GET_DEVICECFG,&stDeviceBaseInfo,
                            sizeof(NETDEV_DEVICE_BASICINFO_S),&dwBytesReturned);
        if(FALSE == bRet)
        {
            qDebug() << "get Device Info Fail";
        }

        int dwCount = 128;
        NETDEV_VIDEO_CHL_DETAIL_INFO_EX_S astVideoChnDetailInfoExList[128] = {0};
        bRet = NETDEV_QueryVideoChlDetailListEx(lpLoginHandle,&dwCount,astVideoChnDetailInfoExList);
        if(FALSE == bRet)
        {
            qDebug() << "Query video channel Info Fail";
        }

        for(int i = 0; i < dwCount; i++)
        {
            stEncoderInfo.stVideoChlInfoList.append(astVideoChnDetailInfoExList[i]);
        }

        stEncoderInfo.lpLoginHandle = lpLoginHandle;
        stEncoderInfo.stDeviceLoginInfo = stDeviceInfo;
        stEncoderInfo.stDeviceBaseInfo = stDeviceBaseInfo;
        gEncoderList.append(stEncoderInfo);

        /* 添加到设备列表中 */
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, QString(stDeviceBaseInfo.szDeviceName)+ "_" + stDeviceInfo.strIpAddress);
        m_videoSourceTreeWgt->addTopLevelItem(item);

        for(int i = 0; i < stEncoderInfo.stVideoChlInfoList.size(); i++)
        {
            QTreeWidgetItem* item_sub = new QTreeWidgetItem();
            NETDEMO_DEVICE_ITEM_DATA_S stItemData;
            stItemData.dwDeviceIndex = gEncoderList.size() - 1;
            stItemData.udwChannelID = stEncoderInfo.stVideoChlInfoList[i].dwChannelID;
            item_sub->setText(0, QString(stEncoderInfo.stVideoChlInfoList[i].szChnName) + "_Channel_" + QString::number(stItemData.udwChannelID));
            item_sub->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
            item->addChild(item_sub);
        }
    }
    else/* 手动配置 */
    {
        stEncoderInfo.stDeviceLoginInfo = stDeviceInfo;
        gEncoderList.append(stEncoderInfo);

        /* 添加到设备列表中 */
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, QString("customDevice_")+ "_" + QString::number(gEncoderList.size()));
        m_videoSourceTreeWgt->addTopLevelItem(item);

        QTreeWidgetItem* item_sub = new QTreeWidgetItem();
        NETDEMO_DEVICE_ITEM_DATA_S stItemData;
        stItemData.dwDeviceIndex = gEncoderList.size() - 1;
        stItemData.udwChannelID = 0;/* 手动配置的通道ID都赋值为0，用以区分是手动还是SDK获取 */
        item_sub->setText(0, stDeviceInfo.strRtspAddress);
        item_sub->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
        item->addChild(item_sub);
    }
}

void AduDemo::AddDecoder(LPVOID lpLoginHandle, NETDEMO_DEVICE_LOGIN_INFO_S stDeviceInfo)
{
    NETDEMO_DECODER_INFO_S stDecoderInfo;

    /*获取设备基本信息*/
    NETDEV_DEVICE_BASICINFO_S stDeviceBaseInfo = {0};
    int dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(lpLoginHandle,0,NETDEV_GET_DEVICECFG,&stDeviceBaseInfo,
                        sizeof(NETDEV_DEVICE_BASICINFO_S),&dwBytesReturned);
    if(FALSE == bRet)
    {
        qDebug() << "get Decoder Info Fail";
    }

    /* 获取能力集 */
    NETDEV_XW_CAP_INFO_S stCapInfo = {0};
    stCapInfo.stSupport.stPaneType.udwSize = 32;
    stCapInfo.stSupport.stPaneType.pudwPaneType = new UINT32[stCapInfo.stSupport.stPaneType.udwSize];//delete[]
    bRet = NETDEV_XW_GetCapability(lpLoginHandle, &stCapInfo);
    if(FALSE == bRet)
    {
        qDebug() << "get Decoder Capability Info Fail";
    }

    /* 获取解码器通道信息 */
    NETDEV_XW_CHANNELS_NUM_S stChannelsNum = {0};
    bRet = NETDEV_XW_GetChannelsNum(lpLoginHandle, &stChannelsNum);

    NETDEV_XW_CHANNELS_LIST_S stChannelsList = {0};
    stChannelsList.udwAudioInNum = stChannelsNum.udwAudioInNum;
    if(stChannelsList.udwAudioInNum > 0)
    {
        stChannelsList.pstAudioIn = new NETDEV_XW_AUDIO_IN_INFO_S[stChannelsList.udwAudioInNum];//delete[]
    }

    stChannelsList.udwAudioOutNum = stChannelsNum.udwAudioOutNum;
    if(stChannelsList.udwAudioOutNum > 0)
    {
        stChannelsList.pstAudioOut = new NETDEV_XW_AUDIO_OUT_INFO_S[stChannelsList.udwAudioOutNum];//delete[]
    }
    stChannelsList.udwVideoInNum = stChannelsNum.udwVideoInNum;
    if(stChannelsList.udwVideoInNum > 0)
    {
        stChannelsList.pstVideoIn = new NETDEV_XW_VIDEO_IN_INFO_S[stChannelsList.udwVideoInNum];//delete[]
    }
    stChannelsList.udwVideoOutNum = stChannelsNum.udwVideoOutNum;
    if(stChannelsList.udwVideoOutNum > 0)
    {
        stChannelsList.pstVideoOut = new NETDEV_XW_VIDEO_OUT_INFO_S[stChannelsList.udwVideoOutNum];//delete[]
    }
    bRet = NETDEV_XW_GetChannelsInfoList(lpLoginHandle,&stChannelsList);
    if(FALSE == bRet)
    {
        qDebug() << "get Decoder Channel Info Fail";
    }

    /* 获取电视墙配置信息 */
    LPVOID lpFindTVWALLHandle = NETDEV_XW_FindTVWallCfgList(lpLoginHandle);
    if(NULL != lpFindTVWALLHandle)
    {
        while(true)
        {
            NETDEV_XW_TVWALL_CFG_S stTVWallCfg = {0};
            bRet = NETDEV_XW_FindNextTVWallCfg(lpFindTVWALLHandle,&stTVWallCfg);
            if(FALSE == bRet)
            {
                break;
            }

            stDecoderInfo.stTVWallCfgList.append(stTVWallCfg);
        }

        NETDEV_XW_FindCloseTVWallCfg(lpFindTVWALLHandle);
    }

    //NETDEV_XW_FindStreamList
    //NETDEV_XW_FindNextStreamInfo
    //NETDEV_XW_FindCloseStreamInfo

    stDecoderInfo.lpLoginHandle = lpLoginHandle;
    stDecoderInfo.stDeviceLoginInfo = stDeviceInfo;
    stDecoderInfo.stDeviceBaseInfo = stDeviceBaseInfo;
    stDecoderInfo.stCapInfo = stCapInfo;
    stDecoderInfo.stChannelsList = stChannelsList;
    gDecoderList.append(stDecoderInfo);

    m_decoderListCmb->addItem(QString(stDeviceBaseInfo.szDevModel) + "_" + stDeviceInfo.strIpAddress);

    /* 添加到界面电视墙列表中 */
    for(int i = 0; i < gDecoderList[gDecoderList.size() - 1].stTVWallCfgList.size(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString(gDecoderList[gDecoderList.size() - 1].stTVWallCfgList[i].szName));
        NETDEMO_TVWALL_ITEM_DATA_S stItemData;
        stItemData.dwDecoderIndex = gDecoderList.size() - 1;
        stItemData.udwTVWallID = gDecoderList[gDecoderList.size() - 1].stTVWallCfgList[i].udwTVWallID;
        item->setData(Qt::UserRole,QVariant::fromValue(stItemData));
        item->setSizeHint(QSize(80,35));
        m_TVWallOptionListWgt->insertItem(0,item);
        m_TVWallOptionListWgt->setCurrentRow(0);
    }

    /* 切换当前展示的电视墙 */
    switchShowTVWall(0);
}

/* 新建或修改电视墙配置时触发该接口 */
void AduDemo::updateTVWallSlot(int dwDecoderIndex, bool bIsAddTVWall, NETDEV_XW_TVWALL_CFG_S stTVWallCfg)
{
    if(true == bIsAddTVWall) /*新建电视墙*/
    {
        UINT32 udwLastChange = 0;
        BOOL bRet = NETDEV_XW_CreateTVWallCfg(gDecoderList[dwDecoderIndex].lpLoginHandle,&stTVWallCfg,&udwLastChange);
        if(FALSE == bRet)
        {
            qDebug() << "create TVWall Fail,error:" << NETDEV_GetLastError();
            return;
        }

        /* 添加到电视墙列表中 */
        gDecoderList[dwDecoderIndex].stTVWallCfgList.append(stTVWallCfg);

        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString(stTVWallCfg.szName));
        NETDEMO_TVWALL_ITEM_DATA_S stItemData;
        stItemData.dwDecoderIndex = dwDecoderIndex;
        stItemData.udwTVWallID = stTVWallCfg.udwTVWallID;
        item->setData(Qt::UserRole,QVariant::fromValue(stItemData));
        item->setSizeHint(QSize(80,35));
        m_TVWallOptionListWgt->insertItem(0,item);
        m_TVWallOptionListWgt->setCurrentRow(0);

        /* 切换当前展示的电视墙 */
        switchShowTVWall(0);
    }
    else/*修改电视墙*/
    {
        /*删除电视墙所有窗口*/
        m_PlayPannelWgt->deleteAllWndInfo();
        /*删除电视墙所有虚拟LED*/
        m_PlayPannelWgt->deleteAllVirtualLEDInfo();

        /* 修改电视墙 */
        UINT32 udwLastChange = 0;
        BOOL bRet = NETDEV_XW_ModifyTVWallCfg(gDecoderList[dwDecoderIndex].lpLoginHandle,&stTVWallCfg,&udwLastChange);
        if(FALSE == bRet)
        {
            qDebug() << "modify TVWall Fail,error:" << NETDEV_GetLastError();
            return;
        }

        /* 修改完电视墙信息后，应该重新获取一次 */
        bRet = NETDEV_XW_GetTVWallCfg(gDecoderList[dwDecoderIndex].lpLoginHandle,
                                      stTVWallCfg.udwTVWallID,
                                      &stTVWallCfg);
        if(FALSE == bRet)
        {
            qDebug() << "get TVWall info Fail,error:" << NETDEV_GetLastError();
            //return;
        }

        /* 获取成功后，更新电视墙缓存 */
        for(int i = 0; i < gDecoderList[dwDecoderIndex].stTVWallCfgList.size(); i++)
        {
            if(stTVWallCfg.udwTVWallID == gDecoderList[dwDecoderIndex].stTVWallCfgList[i].udwTVWallID)
            {
                gDecoderList[dwDecoderIndex].stTVWallCfgList[i] = stTVWallCfg;
                break;
            }
        }

        /* 切换当前展示的电视墙 */
        m_TVWallOptionListWgt->currentItem()->setText(QString(stTVWallCfg.szName));
        int dwCurIndex = m_TVWallOptionListWgt->currentRow();
        switchShowTVWall(dwCurIndex);
    }
}

/* 根据电视墙ID移除电视墙信息 */
void AduDemo::removeTVWallInfo(int dwDecoderIndex, UINT32 udwTVWallID)
{
    for(int i = 0; i < gDecoderList[dwDecoderIndex].stTVWallCfgList.size(); i++)
    {
        if(udwTVWallID == gDecoderList[dwDecoderIndex].stTVWallCfgList[i].udwTVWallID)
        {
            gDecoderList[dwDecoderIndex].stTVWallCfgList.takeAt(i);
            return;
        }
    }
}

/* 根据电视墙ID获取电视墙信息 */
LPNETDEV_XW_TVWALL_CFG_S AduDemo::getTVWallInfo(int dwDecoderIndex, UINT32 udwTVWallID)
{
    for(int i = 0; i < gDecoderList[dwDecoderIndex].stTVWallCfgList.size(); i++)
    {
        if(udwTVWallID == gDecoderList[dwDecoderIndex].stTVWallCfgList[i].udwTVWallID)
        {
            return &gDecoderList[dwDecoderIndex].stTVWallCfgList[i];
        }
    }

    return NULL;
}

/* 切换当前需要展示的电视墙 */
void AduDemo::switchShowTVWall(int dwTVWallOptionListCurRow)
{
    QListWidgetItem* item = m_TVWallOptionListWgt->item(dwTVWallOptionListCurRow);
    if(NULL == item)
    {
        return;
    }
    QVariant variant = item->data(Qt::UserRole);  // 获取用户数据
    NETDEMO_TVWALL_ITEM_DATA_S stItemData = variant.value<NETDEMO_TVWALL_ITEM_DATA_S>();

    LPNETDEV_XW_TVWALL_CFG_S pstTVWallCfg = getTVWallInfo(stItemData.dwDecoderIndex,stItemData.udwTVWallID);
    if(NULL == pstTVWallCfg)
    {
        return;
    }

    m_PlayPannelWgt->clearTVWallDisplayPage();
    m_PlayPannelWgt->setDecoderIndex(stItemData.dwDecoderIndex);
    if(0 == m_pWndVirtualLEDTabWgt->currentIndex())
    {
        m_PlayPannelWgt->setShowType(NETDEMO_TVWALL_SHOW_TYPE_WINDOW);
    }
    else if(1 == m_pWndVirtualLEDTabWgt->currentIndex())
    {
        m_PlayPannelWgt->setShowType(NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED);
    }
    m_PlayPannelWgt->setTVWallInfo(*pstTVWallCfg);
}

/* 分屏 */
void AduDemo::onSplitScreenCmbCurIndexChangedSlot(int SplitIndex)
{
    if(0 == m_pWndVirtualLEDTabWgt->currentIndex())
    {
        m_PlayPannelWgt->setWindowSplitScreen(SplitIndex+1);
    }
}
