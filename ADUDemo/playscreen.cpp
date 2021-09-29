#include "playscreen.h"
#include "publictype.h"
#include <QDebug>
#include <QMouseEvent>
#include <QMimeData>
#include <QMenu>
#include <QTcpSocket>
#include <QHostAddress>

void STDCALL SOURCE_DATA_CALLBACK_PF(LPVOID lpPlayHandle,
                                         const BYTE    *pucBuffer,
                                         INT32         dwBufSize,
                                         INT32         dwMediaDataType,
                                         LPVOID        lpUserParam
                                                       )
{
    NETDEV_PASSIVE_SEND_S stSendData;
    memset(&stSendData, 0, sizeof(NETDEV_PASSIVE_SEND_S));
    stSendData.pszBuffer = new CHAR[dwBufSize];
        memcpy(stSendData.pszBuffer, pucBuffer, dwBufSize);
        stSendData.dwBufSize = dwBufSize;
        stSendData.dwTransType = NETDEV_TRANS_PROTOCOL_TCP;

        NETDEMO_PASSIVE_TASKNO_INFO_S *pstPassiveTaskNoInfo = (NETDEMO_PASSIVE_TASKNO_INFO_S *)lpUserParam;
        BOOL bRet = NETDEV_SendPassiveData(pstPassiveTaskNoInfo->lpLoginHandle, pstPassiveTaskNoInfo->udwTaskNO, &stSendData);
        if (FALSE == bRet)
        {
            qDebug() << "Send Data Failed.";
        }
        delete[] stSendData.pszBuffer;
        return;
}

PlayScreen::PlayScreen(QWidget *parent):
    QLabel(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAcceptDrops(true);
    setFocusPolicy(Qt::ClickFocus);
    setContextMenuPolicy(Qt::CustomContextMenu);

    m_dwDecoderIndex = -1;
    m_dwEncoderIndex = -1;
    m_bIsLiveBind = false;
    m_lpPreviewHandle = NULL;
    m_bIsActiveDecode = true; /* 默认主动解码 */
    memset(&m_stPassiveTaskNoInfo,0,sizeof(m_stPassiveTaskNoInfo));

    /* 已绑定实况，改变屏幕颜色 */
    if(true == m_bIsLiveBind)
    {
        setStyleSheet("background-color:rgb(33,57,94);border: 1px solid black;");
    }
    else {
        setStyleSheet("background-color:rgb(50,50,50);border: 1px solid black;");
    }

    /* 信号槽 */
    connect(this,SIGNAL(customContextMenuRequested (const QPoint&)), this, SLOT(onContextMenuSlot(const QPoint&)));

}

PlayScreen::~PlayScreen()
{
    /* 关闭预览，避免析构后还在调用视频回调接口 */
    onStopPreviewSlot();
    /* 如果是被动解码，分屏析构就关闭被动解码 */
    if(false == m_bIsActiveDecode)/*被动解码*/
    {
        passiveDecode(false);
    }

    m_bIsActiveDecode = true;/* 默认为主动解码 */
}

void PlayScreen::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("custom/video_output_channelID"))
        {
            event->acceptProposedAction();
        }
        else
        {
            event->ignore();
            QLabel::dragEnterEvent(event);
        }
}

void PlayScreen::dragLeaveEvent(QDragLeaveEvent *event)
{
    QLabel::dragLeaveEvent(event);
}

void PlayScreen::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasFormat("custom/video_output_channelID"))
    {
        // accept
        event->acceptProposedAction();
        return;
    }

    event->ignore();
    QLabel::dragMoveEvent(event);
}

void PlayScreen::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat("custom/video_output_channelID"))
    {
        // 提取drag中携带的信息
        QByteArray data = event->mimeData()->data("custom/video_output_channelID");
        QDataStream dataStream(&data, QIODevice::ReadOnly);
        QString strDeviceIndex;
        QString strChannelID;
        dataStream >> strDeviceIndex >> strChannelID;
        m_dwEncoderIndex = strDeviceIndex.toInt();
        m_dwVideoSourceChanID = strChannelID.toInt();
        this->setText(strChannelID);
        event->acceptProposedAction();

        /* 播放分屏绑定实况 */
        startLiveBind();

        return;
    }

    event->ignore();
    QLabel::dropEvent(event);
}

void PlayScreen::focusInEvent(QFocusEvent *event)
{
    if(false == m_bIsLiveBind)
    {
        setStyleSheet("background-color:rgb(50,50,50);border: 2px solid yellow;");
    }
    else
    {
        setStyleSheet("background-color:rgb(33,57,94);border: 2px solid yellow;");
    }

    emit selectedSignal(m_stDisplayID.udwPaneID);
}

void PlayScreen::mouseDoubleClickEvent(QMouseEvent *event){
    emit mouseDoubleClickSignal(m_stDisplayID.udwPaneID);
}

/* 取消当前窗口的选中状态 */
void PlayScreen::clearSelectedStatus()
{
    if(false == m_bIsLiveBind)
    {
        setStyleSheet("background-color:rgb(50,50,50);border: 1px solid black;");
    }
    else
    {
        setStyleSheet("background-color:rgb(33,57,94);border: 1px solid black;");
    }
}

void PlayScreen::onContextMenuSlot(const QPoint& pt)
{
    //构造右键菜单
    QMenu *pMenu = new QMenu(this);
    QAction *startPreviewAction = new QAction(tr("开启预览"), this);
    QAction *stopPreviewAction = new QAction(tr("关闭预览"), this);
    QAction *stopDecodeAction = new QAction(tr("停止解码"), this);
    QAction *changePassiveDecodeAction = new QAction(tr("切换到被动解码"), this);
    QAction *changeActiveDecodeAction = new QAction(tr("切换到主动解码"), this);

    connect(startPreviewAction,SIGNAL(triggered()),this,SLOT(onStartPreviewSlot()));
    connect(stopPreviewAction,SIGNAL(triggered()),this,SLOT(onStopPreviewSlot()));
    connect(changePassiveDecodeAction,SIGNAL(triggered()),this,SLOT(onChangeDecodeSlot()));
    connect(changeActiveDecodeAction,SIGNAL(triggered()),this,SLOT(onChangeDecodeSlot()));
    connect(stopDecodeAction,SIGNAL(triggered()),this,SLOT(onStopDecodeSlot()));

    if(true == m_bIsLiveBind)
    {
        /* 是否开启预览 */
        if(NULL != m_lpPreviewHandle)
        {
            pMenu->addAction(stopPreviewAction);
        }
        else
        {
            pMenu->addAction(startPreviewAction);
        }

        /* 解码类型(主动解码、被动解码) */
        if(true == m_bIsActiveDecode)
        {
            pMenu->addAction(changePassiveDecodeAction);
        }
        else
        {
            pMenu->addAction(changeActiveDecodeAction);
        }


        pMenu->addAction(stopDecodeAction);
    }

    //显示菜单
    pMenu->exec(mapToGlobal(pt));

    delete pMenu;
    delete stopDecodeAction;
    delete startPreviewAction;
    delete stopPreviewAction;
    delete changePassiveDecodeAction;
    delete changeActiveDecodeAction;
}

/* 开始解码 */
void PlayScreen::startLiveBind()
{
    if(true == m_bIsLiveBind)
    {
        return;
    }

    if(true == m_bIsActiveDecode)/* 主动解码 */
    {
        activeDecode(true);
    }
    else {
        /* 被动解码 */
        passiveDecode(true);
    }

    /* 已绑定实况，改变屏幕颜色 */
    if(true == m_bIsLiveBind)
    {
        setStyleSheet("background-color:rgb(33,57,94);border: 1px solid black;");
    }
    else {
        setStyleSheet("background-color:rgb(50,50,50);border: 1px solid black;");
    }
}

/* 停止解码 */
void PlayScreen::onStopDecodeSlot()
{
    if(false == m_bIsLiveBind)
    {
        return;
    }

    if(true == m_bIsActiveDecode)/*主动解码*/
    {
        activeDecode(false);
    }else
    {
        /*被动解码*/
        passiveDecode(false);
    }

    /* 已绑定实况，改变屏幕颜色 */
    if(true == m_bIsLiveBind)
    {
        setStyleSheet("background-color:rgb(33,57,94);border: 1px solid black;");
    }
    else {
        setStyleSheet("background-color:rgb(50,50,50);border: 1px solid black;");
    }
}

/* 主动解码 */
void PlayScreen::activeDecode(bool bIsStart)
{
    if(true == bIsStart)/* 开始主动解码 */
    {
        /* 获取视频源rtsp播放地址 */
        if(-1 == m_dwEncoderIndex)/* 没有视频源，无法获取URL */
        {
            return;
        }

        BOOL bIsAuth = TRUE;/* 视频源是否需要鉴权,默认需要鉴权 */
        BOOL bRet = FALSE;
        CHAR szStreamUrl[512] = {0};
        if(0 == m_dwVideoSourceChanID)/* 手动配置视频源 */
        {
            strncpy(szStreamUrl,gEncoderList[m_dwEncoderIndex].stDeviceLoginInfo.strRtspAddress.toStdString().c_str(),sizeof(szStreamUrl) -1);
            if(false == gEncoderList[m_dwEncoderIndex].stDeviceLoginInfo.bIsAuth)
            {
                bIsAuth = FALSE;
            }
        }
        else/* SDK获取视频源 */
        {
            bIsAuth = TRUE; /* 通过SDK添加的方式目前指定必须鉴权 */
            bRet = NETDEV_GetStreamUrl(gEncoderList[m_dwEncoderIndex].lpLoginHandle,
                                m_dwVideoSourceChanID,
                                NETDEV_LIVE_STREAM_INDEX_MAIN,
                                szStreamUrl);
            if(FALSE == bRet)
            {
                qDebug() << "get stream url fail";
                return;
            }
        }

        UINT32 udwLastChange = 0;
        NETDEV_XW_DISPLAYER_ID_S stDisplayerID = m_stDisplayID;
        NETDEV_XW_VIDEO_SOURCE_S stVideoSource;
        memset(&stVideoSource,0,sizeof(stVideoSource));
        stVideoSource.stAuthentication.bIsNeedAuthentication = bIsAuth;
        if(TRUE == bIsAuth)
        {
            strncpy(stVideoSource.stAuthentication.szUserName,
                    gEncoderList[m_dwEncoderIndex].stDeviceLoginInfo.strUserName.toStdString().c_str(),
                    sizeof(stVideoSource.stAuthentication.szUserName) -1);
            strncpy(stVideoSource.stAuthentication.szPassword,
                    gEncoderList[m_dwEncoderIndex].stDeviceLoginInfo.strPassword.toStdString().c_str(),
                    sizeof(stVideoSource.stAuthentication.szPassword) -1);
        }
        strncpy(stVideoSource.szRealplayURL,szStreamUrl,sizeof(stVideoSource.szRealplayURL) -1);
        stVideoSource.udwStreamID = NETDEV_LIVE_STREAM_INDEX_MAIN;
        stVideoSource.stDisplayerID = m_stDisplayID;
        stVideoSource.udwType = NETDEV_TYPE_NORMAL;
        stVideoSource.udwTransProtocol = NETDEV_TRANS_PROTOCOL_TCP;
        stVideoSource.udwMulticast = NETDEV_STREAM_TYPE_RTP_UNICAST;
        stVideoSource.udwVideoInID = 1;/* 需计算得到，先临时给值 */
        bRet = NETDEV_XW_CreateLiveBind(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                 &stDisplayerID,
                                 &stVideoSource,
                                 &udwLastChange);

        if(FALSE == bRet)
        {
            qDebug() << "Create Live Bind fail";
            return;
        }

        m_bIsLiveBind = true;
    }
    else/* 停止主动解码 */
    {
        if(true == m_bIsLiveBind)
        {
            /* 停止预览 */
            onStopPreviewSlot();

            UINT32 udwLastChange = 0;
            BOOL bRet = NETDEV_XW_DeleteLiveBind(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                              &m_stDisplayID,
                                              &udwLastChange);
            if(FALSE == bRet)
            {
                qDebug() << "delete Live Bind fail";
                return;
            }

            m_bIsLiveBind = false;
        }
    }
}

/* 被动解码 */
void PlayScreen::passiveDecode(bool bIsStart)
{
    BOOL bRet = FALSE;
    if(true == bIsStart)/* 开始被动解码 */
    {
        if(-1 == m_dwEncoderIndex)/* Demo目前是通过视频源获取码流，发送给解码器，如果没有视频源，无法解码 */
        {
            return;
        }

        /*获取本地IP地址,被动解码需要使用*/
        QString IP;
        QTcpSocket* tcpsocket = new QTcpSocket();
        tcpsocket->connectToHost(QString(gDecoderList[m_dwDecoderIndex].stDeviceLoginInfo.strIpAddress),
                                 gDecoderList[m_dwDecoderIndex].stDeviceLoginInfo.dwPort);
        if(true == tcpsocket->waitForConnected(5000))
        {
            IP = tcpsocket->localAddress().toString();
            tcpsocket->disconnectFromHost();
            delete tcpsocket;
        }
        else
        {
            tcpsocket->disconnectFromHost();
            delete tcpsocket;
            return;
        }

        NETDEV_XW_VIDEO_SOURCE_S stVideoSource;
        memset(&stVideoSource,0,sizeof(stVideoSource));
        stVideoSource.stAuthentication.bIsNeedAuthentication = FALSE;
        stVideoSource.udwStreamID = NETDEV_LIVE_STREAM_INDEX_MAIN;
        stVideoSource.stDisplayerID = m_stDisplayID;
        stVideoSource.udwType = NETDEV_TYPE_NORMAL;
        stVideoSource.udwTransProtocol = NETDEV_TRANS_PROTOCOL_TCP;/* 使用TCP协议 */
        stVideoSource.udwMulticast = NETDEV_STREAM_TYPE_RTP_UNICAST;
        stVideoSource.udwDecodeType = NETDEV_DECODE_TYPE_SERVER;/* 指定解码器作为服务器 */
        stVideoSource.udwVideoInID = 1;/* 需计算得到，先临时给值 */
        if(NETDEV_TRANS_PROTOCOL_TCP == stVideoSource.udwTransProtocol)
        {
            QString streamURL = "tcp://";
            streamURL+=IP + ":" + QString::number(10002);
            strncpy(stVideoSource.szRealplayURL,streamURL.toStdString().c_str(),sizeof(stVideoSource.szRealplayURL) -1);
        }

        memset(&m_stPassiveTaskNoInfo,0,sizeof(m_stPassiveTaskNoInfo));
        bRet = NETDEV_StartPassiveDecode(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                         &m_stDisplayID,
                                         &stVideoSource,
                                         &m_stPassiveTaskNoInfo.udwTaskNO);
        if(FALSE == bRet)
        {
            qDebug() << "start passive Decode fail";
            return;
        }

        m_bIsLiveBind = true;

        m_stPassiveTaskNoInfo.lpLoginHandle = gDecoderList[m_dwDecoderIndex].lpLoginHandle;

        /* 开启预览，获取视频流数据，作为被动解码视频源数据发送 */
        onStartPreviewSlot();
    }
    else/* 停止被动解码 */
    {
        /* 停止预览 */
        onStopPreviewSlot();

        /* 停止解码 */
        bRet = NETDEV_StopPassiveDecode(gDecoderList[m_dwDecoderIndex].lpLoginHandle,m_stPassiveTaskNoInfo.udwTaskNO);
        if(FALSE == bRet)
        {
            qDebug() << "stop passive Decode fail";
            return;
        }

        m_bIsLiveBind = false;
    }
}

/*切换主被动解码方式*/
void PlayScreen::onChangeDecodeSlot()
{
    if(-1 == m_dwEncoderIndex
       || 0 == m_dwVideoSourceChanID)/* Demo目前是通过视频源获取解码流，如果没有视频源，无法解码
                                        默认启动的是主动解码，如果通道ID为0不允许切换到被动解码*/
    {
        return;
    }

    onStopDecodeSlot();
    m_bIsActiveDecode = !m_bIsActiveDecode;
    startLiveBind();
}

/* 开启预览 */
void PlayScreen::onStartPreviewSlot()
{
    if(-1 == m_dwEncoderIndex
            || m_lpPreviewHandle != NULL
            || 0 == m_dwVideoSourceChanID )/* 预览已开启或者设备是手动配置的则不开启预览 */
    {
        return;
    }

    NETDEV_PREVIEWINFO_S stPreviewInfo = {0};
    stPreviewInfo.hPlayWnd = (LPVOID)this->winId();
    stPreviewInfo.dwChannelID = m_dwVideoSourceChanID;
    stPreviewInfo.dwStreamType = NETDEV_LIVE_STREAM_INDEX_MAIN;
    stPreviewInfo.dwLinkMode = NETDEV_TRANSPROTOCAL_RTPTCP;

    if(true == m_bIsActiveDecode)/* 主动解码不设置码流回调 */
    {
        m_lpPreviewHandle = NETDEV_RealPlay(gEncoderList[m_dwEncoderIndex].lpLoginHandle,
                        &stPreviewInfo,
                        NULL,
                        NULL);
    }
    else/* 被动解码需要设置码流回调取视频流数据 */
    {
        m_lpPreviewHandle = NETDEV_RealPlay(gEncoderList[m_dwEncoderIndex].lpLoginHandle,
                        &stPreviewInfo,
                        SOURCE_DATA_CALLBACK_PF,
                        &m_stPassiveTaskNoInfo);
    }

    if(NULL == m_lpPreviewHandle)
    {
        qDebug() << "start Preview fail";
    }
}

/* 关闭预览 */
void PlayScreen::onStopPreviewSlot()
{
    if(m_lpPreviewHandle != NULL)
    {
        BOOL bRet = NETDEV_StopRealPlay(m_lpPreviewHandle);
        if(FALSE == bRet)
        {
            qDebug() << "stop Preview fail";
            return;
        }

        m_lpPreviewHandle = NULL;
    }

    update();
}

void PlayScreen::setDecoderIndex(int dwDecoderIndex)
{
    m_dwDecoderIndex = dwDecoderIndex;
}

void PlayScreen::setDisplayID(NETDEV_XW_DISPLAYER_ID_S stDisplayID)
{
    m_stDisplayID = stDisplayID;
}

NETDEV_XW_DISPLAYER_ID_S PlayScreen::getDisplayID()
{
    return m_stDisplayID;
}

void PlayScreen::setStreamInfo(NETDEV_XW_STREAM_INFO_S streamInfo)
{
    m_streamInfo = m_streamInfo;
    m_bIsLiveBind = true;

    /* 已绑定实况，改变屏幕颜色 */
    if(true == m_bIsLiveBind)
    {
        setStyleSheet("background-color:rgb(33,57,94);border: 1px solid black;");
    }
    else {
        setStyleSheet("background-color:rgb(50,50,50);border: 1px solid black;");
    }
}
