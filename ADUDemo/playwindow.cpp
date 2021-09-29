#include "playwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include <QDebug>
#include "playscreen.h"

PlayWindow::PlayWindow(QWidget *parent) : QWidget(parent)
{
    memset(&m_stSceneWndInfo, 0, sizeof(m_stSceneWndInfo));

    m_dwCurrentSelectedScreenID = -1; /* -1表示没有分屏被选中 */
    m_bIsZoomOn = false;/* 默认分屏没有放大 */

    /*窗口标题栏*/
    m_titleBarWIdget = new QWidget();
    QPushButton* closeBtn = new QPushButton();
    m_titleBarWIdget->setMaximumHeight(16);
    closeBtn->setFixedSize(16,16);
    closeBtn->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton)));
    m_titleBarWIdget->setStyleSheet("background-color:rgb(25,25,25);");
    closeBtn->setStyleSheet("background-color:white;");

    QHBoxLayout* titleBarLayout = new QHBoxLayout();
    titleBarLayout->setSpacing(0);
    titleBarLayout->setMargin(0);
    titleBarLayout->setAlignment(Qt::AlignRight);
    titleBarLayout->addWidget(closeBtn);
    m_titleBarWIdget->setLayout(titleBarLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    m_playLayout = new QGridLayout();
    m_playLayout->setSpacing(0);
    m_playLayout->setMargin(0);

    m_titleBarWIdget->hide();/* 默认隐藏标题栏 */

    mainLayout->addWidget(m_titleBarWIdget);
    mainLayout->addLayout(m_playLayout);
    setLayout(mainLayout);

    /* 信号槽 */
    connect(closeBtn,SIGNAL(clicked()),this,SLOT(onCloseWindowSlot()));
}

void PlayWindow::setSplitScreenNumber(int dwSplitScreenNumber)
{
    /* 当前demo的切换分屏处理方式会导致客户端预览和被动解码被关闭，切换分屏处理思路：由少变多，原来的不变，只新增不够的，
	由多变少，保留两者前面重复的，去除最后多余的部分 */
    QLayoutItem * child;
    while(NULL != (child = m_playLayout->takeAt(0)))
    {
        if(NULL != child->widget())
        {
            delete child->widget();/* 确保playScreen被析构 */
        }

        delete child;
    }

    int dwSplitScreenIndex = 0;
    for(int dwRowIndex = 0; dwRowIndex < dwSplitScreenNumber; dwRowIndex++)
    {
        for(int dwColumnIndex = 0; dwColumnIndex < dwSplitScreenNumber; dwColumnIndex++)
        {
            PlayScreen* playScreen = new PlayScreen();
            playScreen->setDecoderIndex(m_dwDecoderIndex);
            NETDEV_XW_DISPLAYER_ID_S stDisplayID;
            stDisplayID.udwTVWallID = m_dwTVWallID;
            stDisplayID.udwWndID = m_stSceneWndInfo.udwWndID;
            stDisplayID.udwPaneID = dwSplitScreenIndex;
            playScreen->setDisplayID(stDisplayID);
            for(int i = 0; i < m_streamInfoList.size(); i++)
            {
                if(stDisplayID.udwPaneID == m_streamInfoList[i].stStreamInfoDest.udwPaneID)
                {
                    playScreen->setStreamInfo(m_streamInfoList[i]);
                    break;
                }
            }

            /*PlayScreen获得焦点后窗口允许修改,显示标题栏*/
            connect(playScreen,SIGNAL(selectedSignal(int)),this,SLOT(onPlayScreenSelectedSlot(int)));

            /* 双击分屏放大 */
            connect(playScreen,SIGNAL(mouseDoubleClickSignal(int)),this,SLOT(onPlayScreenDoubleClickSlot(int)));

            m_playLayout->addWidget(playScreen, dwRowIndex, dwColumnIndex);
            dwSplitScreenIndex++;
        }
    }
}

void PlayWindow::onPlayScreenSelectedSlot(int dwPannelID)
{
    m_dwCurrentSelectedScreenID = dwPannelID;
    m_titleBarWIdget->show();
    emit selectedSignal(m_stSceneWndInfo.udwWndID);

    /* 取消当前窗口下非选中分屏的选中状态 */
    for (int i = 0; i < m_playLayout->count(); ++i)
    {
        PlayScreen* playScreen = (PlayScreen*)m_playLayout->itemAt(i)->widget();
        if(m_dwCurrentSelectedScreenID != playScreen->getDisplayID().udwPaneID)
        {
            playScreen->clearSelectedStatus();
        }
    }
}

/* 双击播放分屏放大至最大 */
void PlayWindow::onPlayScreenDoubleClickSlot(int dwPannelID)
{
    m_dwCurrentSelectedScreenID = dwPannelID;
    m_titleBarWIdget->show();
    emit selectedSignal(m_stSceneWndInfo.udwWndID);

    NETDEV_XW_SCREEN_ZOOM_INFO_S stScreenZoomInfo = {0};
    stScreenZoomInfo.uOperatedType = NETDEV_XW_SCREEN_ZOOM_TYPE_PANE;
    stScreenZoomInfo.uSplitScreenIndex = dwPannelID;
    if(m_bIsZoomOn == false)
    {
        stScreenZoomInfo.uOperatedMode = NETDEV_XW_SCREEN_ZOOM_MODE_ON;
    }
    else
    {
        stScreenZoomInfo.uOperatedMode = NETDEV_XW_SCREEN_ZOOM_MODE_OFF;
    }
    if(false == modifyPlayScreenZoom(stScreenZoomInfo))
    {
        return;
    }

    for (int i = 0; i < m_playLayout->count(); ++i)
    {
        PlayScreen* playScreen = (PlayScreen*)m_playLayout->itemAt(i)->widget();
        if(m_dwCurrentSelectedScreenID != playScreen->getDisplayID().udwPaneID)
        {
            if(m_bIsZoomOn == false)
            {
                playScreen->setHidden(true);/* 隐藏当前窗口下非选中状态的分屏 */
            }
            else
            {
                playScreen->setHidden(false);
            }
        }
    }

    m_bIsZoomOn = !m_bIsZoomOn;
}


/* 取消当前窗口的选中状态 */
void PlayWindow::clearSelectedStatus()
{
    m_titleBarWIdget->hide();
    /* 取消当前窗口下所有分屏的选中状态 */
    for (int i = 0; i < m_playLayout->count(); ++i)
    {
        PlayScreen* playScreen = (PlayScreen*)m_playLayout->itemAt(i)->widget();
        playScreen->clearSelectedStatus();
    }
}

/* 修改播放分屏放大状态 */
bool PlayWindow::modifyPlayScreenZoom(NETDEV_XW_SCREEN_ZOOM_INFO_S stScreenZoomInfo)
{

   BOOL bRet = NETDEV_XW_ZoomScreen(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                        m_dwTVWallID,
                        m_stSceneWndInfo.udwWndID,
                        &stScreenZoomInfo);
   if(FALSE == bRet)
   {
       qDebug() << "NETDEV_XW_ZoomScreen fail";
       return false;
   }

   return true;
}

void PlayWindow::onCloseWindowSlot()
{
    /* 关闭窗口 */
    if(true == deleteWnd())
    {
        setAttribute(Qt::WA_DeleteOnClose);/* 确保窗口关闭会销毁对象 */
        close();
    }
}

/* 关闭窗口 */
bool PlayWindow::deleteWnd()
{
    UINT32 udwLastChange = 0;
    BOOL bRet = NETDEV_XW_DeleteWnd(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                    m_dwTVWallID,
                                    m_stSceneWndInfo.udwWndID,
                                    &udwLastChange);

    if(FALSE == bRet)
    {
        qDebug() << "delete window fail";
        return false;
    }

    return true;
}

int PlayWindow::getDecoderIndex()
{
    return this->m_dwDecoderIndex;
}
void PlayWindow::setDecoderIndex(int dwDecoderIndex)
{
    this->m_dwDecoderIndex = dwDecoderIndex;
}

int PlayWindow::getTVWallID()
{
    return this->m_dwTVWallID;
}
void PlayWindow::setTVWallID(int dwTVWallID)
{
    this->m_dwTVWallID = dwTVWallID;
}

void PlayWindow::setWndInfo(NETDEV_XW_SCENE_WND_INFO_S stSceneWndInfo)
{
    m_bIsZoomOn = false;/* 播放分屏的放大状态默认为不放大 */

    if(m_stSceneWndInfo.udwPaneMod == stSceneWndInfo.udwPaneMod)
    {
        m_stSceneWndInfo = stSceneWndInfo;
    }
    else {
        m_stSceneWndInfo = stSceneWndInfo;
        /* 设置分屏模式，demo仅实现基本的1、4、9、16分屏 */
        if(NETDEV_XW_LAYOUT_TYPE_01 == m_stSceneWndInfo.udwPaneMod)
        {
            setSplitScreenNumber(1);
        }
        else if(NETDEV_XW_LAYOUT_TYPE_04 == m_stSceneWndInfo.udwPaneMod)
        {
            setSplitScreenNumber(2);
        }
        else if(NETDEV_XW_LAYOUT_TYPE_09 == m_stSceneWndInfo.udwPaneMod)
        {
            setSplitScreenNumber(3);
        }
        else if(NETDEV_XW_LAYOUT_TYPE_16 == m_stSceneWndInfo.udwPaneMod)
        {
            setSplitScreenNumber(4);
        }
    }
}

NETDEV_XW_SCENE_WND_INFO_S PlayWindow::getWndInfo()
{
    return m_stSceneWndInfo;
}

/* 清理窗口关联业务流信息 */
void PlayWindow::clearStreamInfo()
{
   m_streamInfoList.clear();
}

/* 添加窗口关联的业务流信息 */
void PlayWindow::appendStreamInfo(NETDEV_XW_STREAM_INFO_S stXWStreamInfo)
{
    m_streamInfoList.append(stXWStreamInfo);
}
