#include "mplaywidget.h"
#include <QtDebug>

MPlayWidget::MPlayWidget(QWidget *parent) : QWidget(parent)
{
    clearTVWallDisplayPage();

    m_pVirtualLEDCfgDlg = new VirtualLEDConfigDlg(this);
    connect(m_pVirtualLEDCfgDlg,SIGNAL(virtualLEDChangedSignal(NETDEV_XW_VIRTUAL_LED_INFO_S, bool)),this,SLOT(virtualLEDInfoChangedSlot(NETDEV_XW_VIRTUAL_LED_INFO_S, bool)));
}

void MPlayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);// 反走样
    painter.setPen(QPen(QColor(0, 160, 230), 2));// 设置画笔颜色、宽度
    painter.setBrush(QColor(27, 39, 56));// 设置画刷颜色

    painter.drawRect(event->rect());

    /* 鼠标右键按下移动时，不刷新电视墙显示单元与窗口 */
    if(m_bIsRightMousePressMove == true)
    {
        return;
    }

    if(m_stTVWallCfg.stScreenInfo.udwRowNum <= 0
            || m_stTVWallCfg.stScreenInfo.udwColNum <= 0
            || m_dwDecoderIndex < 0)
    {
        return;
    }

    /* 记录上一次界面电视墙显示单元总宽高，refreshTvWallDisplayUnitSize函数中会重新计算本次宽高 */
    m_dwLastTVWallWidgetWidth = m_dwTVWallWidgetWidth;
    m_dwLastTVWallWidgetHeight = m_dwTVWallWidgetHeight;
    /* 刷新并重新绘制显示单元 */
    refreshTvWallDisplayUnitSize();

    for(int i = 0; i < m_TVWallDisplayUnitList.size(); i++)
    {
        painter.drawRect(m_TVWallDisplayUnitList[i].startX,
                         m_TVWallDisplayUnitList[i].startY,
                         m_TVWallDisplayUnitList[i].width,
                         m_TVWallDisplayUnitList[i].height);

        painter.drawText(m_TVWallDisplayUnitList[i].startX,
                         m_TVWallDisplayUnitList[i].startY,
                         m_TVWallDisplayUnitList[i].width,
                         m_TVWallDisplayUnitList[i].height,
                         Qt::AlignCenter,
                        m_TVWallDisplayUnitList[i].strDescText);
    }

    /* 鼠标左键按下移动窗口时，不刷新窗口/虚拟LED位置及大小 */
    if(false == m_bIsLeftMousePressMove)
    {
        /* 刷新播放窗口/虚拟LED大小及位置 */
        refreshWndOrVirtualLEDSize();
    }
}

/* 窗口大小改变，刷新电视墙显示单元大小 */
void MPlayWidget::refreshTvWallDisplayUnitSize()
{
    m_dwTVWallWidgetWidth = 0;
    m_dwTVWallWidgetHeight = 0;
    for(int i = width(); i > 0; i--)
    {
        for(int j = height(); j > 0; j--)
        {
            if(i == (m_dwVideoOutFormatWidth * m_stTVWallCfg.stScreenInfo.udwColNum * j)/(m_dwVideoOutFormatHeight * m_stTVWallCfg.stScreenInfo.udwRowNum))
            {
                m_dwTVWallWidgetWidth = i;
                m_dwTVWallWidgetHeight = j;
                break;
            }
        }

        /* 判断是否已经拿到宽高 */
        if(m_dwTVWallWidgetWidth > 0)
        {
            break;
        }
    }

    int dwDisplayUnitWidth = m_dwTVWallWidgetWidth/m_stTVWallCfg.stScreenInfo.udwColNum;
    int dwDisplayUnitHeight = m_dwTVWallWidgetHeight/m_stTVWallCfg.stScreenInfo.udwRowNum;

    for(int i = 0; i < m_TVWallDisplayUnitList.size(); i++)
    {
        m_TVWallDisplayUnitList[i].startX = m_TVWallDisplayUnitList[i].dwColumnIndex * dwDisplayUnitWidth;
        m_TVWallDisplayUnitList[i].startY = dwDisplayUnitHeight * m_TVWallDisplayUnitList[i].dwRowIndex;
        m_TVWallDisplayUnitList[i].width = dwDisplayUnitWidth;
        m_TVWallDisplayUnitList[i].height = dwDisplayUnitHeight;
    }
}

/* 窗口大小改变，刷新播放窗口/虚拟LED大小及位置 */
void MPlayWidget::refreshWndOrVirtualLEDSize()
{
    if(NETDEMO_TVWALL_SHOW_TYPE_WINDOW == m_eShowType)/* 获取playWidget中所有播放窗口 */
    {
        QList<PlayWindow*> playWindowList = this->findChildren<PlayWindow*>();
        for(int i = 0; i < playWindowList.size(); i++)
        {
            PlayWindow* playWindow = playWindowList.at(i);
            NETDEV_XW_AREA_S stArea = playWindow->getWndInfo().stArea;
            convertDeviceTVwall2ScreenPos(stArea);
            playWindow->setGeometry(QRect(QPoint(stArea.stTopLeft.udwX,stArea.stTopLeft.udwY),QPoint(stArea.stBottomRight.udwX,stArea.stBottomRight.udwY)));
        }
    }
    else if(NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED == m_eShowType)/* 获取playWidget中所有虚拟LED */
    {
        QList<VirtualLED*> virtualLEDList = this->findChildren<VirtualLED*>();
        for(int i = 0; i < virtualLEDList.size(); i++)
        {
            VirtualLED* virtualLED = virtualLEDList.at(i);
            NETDEV_XW_AREA_S stArea = virtualLED->getVirtualLEDInfo().stArea;
            convertDeviceTVwall2ScreenPos(stArea);
            virtualLED->setGeometry(QRect(QPoint(stArea.stTopLeft.udwX,stArea.stTopLeft.udwY),QPoint(stArea.stBottomRight.udwX,stArea.stBottomRight.udwY)));

            UINT32 udwFontSize = 48;
            if(NETDEV_FONT_SIZE_SELF_ADAPTION == virtualLED->getVirtualLEDInfo().stFontInfo.udwSize)
            {
                udwFontSize = virtualLED->getVirtualLEDInfo().stFontInfo.udwReferFontSize;
            }
            else {
                udwFontSize = virtualLED->getVirtualLEDInfo().stFontInfo.udwSize;
            }
            convertFontSize2Client(udwFontSize);

            UINT32 udwGap = 0;
            if(NETDEV_LED_FONT_GAP_INVALID == virtualLED->getVirtualLEDInfo().stFontInfo.udwGap)
            {
                udwGap = virtualLED->getVirtualLEDInfo().stFontInfo.udwReferFontGap;
                convertFontSize2Client(udwGap);
            }
            else {
                udwGap = virtualLED->getVirtualLEDInfo().stFontInfo.udwSize;
                convertFontGap2Client(udwFontSize, udwGap);
            }

            /* 界面大小改变，刷新显示的字体大小及字间距 */
            if(m_dwLastTVWallWidgetWidth != m_dwTVWallWidgetWidth
               || m_dwLastTVWallWidgetHeight != m_dwTVWallWidgetHeight)
            {
                virtualLED->setClientFontSize(udwFontSize,udwGap);
            }
        }
    }
}

void MPlayWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton)
    {
        m_startPoint = event->pos();
    }

    QWidget::mousePressEvent(event);
}

void MPlayWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(-1 == m_dwDecoderIndex) /* -1表示还未关联电视墙，不能开窗 */
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    qDebug() << event->pos().x() << event->pos().y();

    /* 鼠标位置超出电视墙坐标范围 */
    if(false == mousePointhasValid(event->pos()))
    {
        return;
    }else
    {
        m_endPoint = event->pos();
    }

    m_bIsRightMousePressMove = false;
    m_bIsLeftMousePressMove = false;

    if(event->buttons() & Qt::RightButton)/* 右键开窗/LED */
    {
        m_bIsRightMousePressMove = true;
        /* 绘制矩形 */
        update(QRect(m_startPoint,m_endPoint));
    }
    else if(event->buttons() & Qt::LeftButton)/*左键控制窗口/LED移动*/
    {
        m_bIsLeftMousePressMove = true;

        if(NETDEMO_TVWALL_SHOW_TYPE_WINDOW == m_eShowType)/* 获取playWidget中所有播放窗口,判断当前选中的是哪个窗口 */
        {
            QList<PlayWindow*> playWindowList = this->findChildren<PlayWindow*>();
            for(int i = 0; i < playWindowList.size(); i++)
            {
                PlayWindow* playWindow = playWindowList.at(i);
                if(m_udwCurrentSelectedWndID == playWindow->getWndInfo().udwWndID)
                {
                    playWindow->setGeometry(QRect(event->pos(), playWindow->geometry().bottomRight() + (event->pos() - playWindow->geometry().topLeft())));
                    break;
                }
            }
        }
        else if(NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED == m_eShowType)/* 获取playWidget中所有虚拟LED,判断当前选中的是哪个虚拟LED */
        {
            QList<VirtualLED*> virtualLEDList = this->findChildren<VirtualLED*>();
            for(int i = 0; i < virtualLEDList.size(); i++)
            {
                VirtualLED* virtualLED = virtualLEDList.at(i);
                if(m_udwCurrentSelectedLEDID == virtualLED->getVirtualLEDInfo().udwLEDID)
                {
                    virtualLED->setGeometry(QRect(event->pos(), virtualLED->geometry().bottomRight() + (event->pos() - virtualLED->geometry().topLeft())));
                    break;
                }
            }
        }
    }
}

void MPlayWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(true == m_bIsRightMousePressMove)/*右键开窗*/
    {
        m_bIsRightMousePressMove = false;

        /* 转换窗口坐标 */
        NETDEV_XW_AREA_S stArea;
        stArea.stTopLeft.udwX = m_startPoint.x();
        stArea.stTopLeft.udwY = m_startPoint.y();
        stArea.stBottomRight.udwX = m_endPoint.x();
        stArea.stBottomRight.udwY = m_endPoint.y();
        convertScreen2DeviceTVwallPos(stArea);

        if(NETDEMO_TVWALL_SHOW_TYPE_WINDOW == m_eShowType)/*开窗*/
        {
            NETDEV_XW_SCENE_WND_INFO_S stWndInfo = {0};
            stWndInfo.udwPaneMod = NETDEV_XW_LAYOUT_TYPE_01;
            stWndInfo.stArea = stArea;
            createWnd(stWndInfo);
        }
        else if(NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED == m_eShowType)/*创建虚拟LED*/
        {
            NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo = {0};
            stVirtualLEDInfo.stArea = stArea;
            m_pVirtualLEDCfgDlg->setVirtualLEDInfo(stVirtualLEDInfo);
            m_pVirtualLEDCfgDlg->show();
        }

        /*刷新界面*/
        update();
    }
    else if(true == m_bIsLeftMousePressMove)/*左键窗口移动*/
    {
        m_bIsLeftMousePressMove = false;

        /* 修改坐标 */
        if(NETDEMO_TVWALL_SHOW_TYPE_WINDOW == m_eShowType)/* 获取playWidget中所有播放窗口,判断当前选中的是哪个窗口 */
        {
            QList<PlayWindow*> playWindowList = this->findChildren<PlayWindow*>();
            for(int i = 0; i < playWindowList.size(); i++)
            {
                PlayWindow* playWindow = playWindowList.at(i);
                if(m_udwCurrentSelectedWndID == playWindow->getWndInfo().udwWndID)
                {
                    NETDEV_XW_SCENE_WND_INFO_S stWndInfo = playWindow->getWndInfo();
                    /* 转换窗口坐标 */
                    NETDEV_XW_AREA_S stArea;
                    stArea.stTopLeft.udwX = playWindow->geometry().topLeft().x();
                    stArea.stTopLeft.udwY = playWindow->geometry().topLeft().y();
                    stArea.stBottomRight.udwX = playWindow->geometry().bottomRight().x();
                    stArea.stBottomRight.udwY = playWindow->geometry().bottomRight().y();
                    convertScreen2DeviceTVwallPos(stArea);
                    stWndInfo.stArea = stArea;
                    stWndInfo.udwModifyCmd = NETDEV_WND_AREA;
                    bool bRet = modifyWndInfo(stWndInfo);
                    if(true == bRet)
                    {
                        if(true == getWndInfo(&stWndInfo))
                        {
                            playWindow->setWndInfo(stWndInfo);
                        }
                    }
                    break;
                }
            }
        }
        else if(NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED == m_eShowType)/* 获取playWidget中所有虚拟LED,判断当前选中的是哪个虚拟LED */
        {
            QList<VirtualLED*> virtualLEDList = this->findChildren<VirtualLED*>();
            for(int i = 0; i < virtualLEDList.size(); i++)
            {
                VirtualLED* virtualLED = virtualLEDList.at(i);
                if(m_udwCurrentSelectedLEDID == virtualLED->getVirtualLEDInfo().udwLEDID)
                {
                    NETDEV_XW_VIRTUAL_LED_INFO_S stvirtualLEDInfo = virtualLED->getVirtualLEDInfo();
                    /* 转换坐标 */
                    NETDEV_XW_AREA_S stArea;
                    stArea.stTopLeft.udwX = virtualLED->geometry().topLeft().x();
                    stArea.stTopLeft.udwY = virtualLED->geometry().topLeft().y();
                    stArea.stBottomRight.udwX = virtualLED->geometry().bottomRight().x();
                    stArea.stBottomRight.udwY = virtualLED->geometry().bottomRight().y();
                    convertScreen2DeviceTVwallPos(stArea);
                    stvirtualLEDInfo.stArea = stArea;
                    stvirtualLEDInfo.udwModifyCmd = NETDEV_LED_AREA;
                    bool bRet = modifyVirtualLEDInfo(stvirtualLEDInfo);
                    if(true == bRet)
                    {
                        if(true == getVirtualLEDInfo(&stvirtualLEDInfo))
                        {
                            UINT32 udwFontSize = 48;
                            if(NETDEV_FONT_SIZE_SELF_ADAPTION == stvirtualLEDInfo.stFontInfo.udwSize)
                            {
                                udwFontSize = stvirtualLEDInfo.stFontInfo.udwReferFontSize;
                            }
                            else {
                                udwFontSize = stvirtualLEDInfo.stFontInfo.udwSize;
                            }
                            convertFontSize2Client(udwFontSize);

                            UINT32 udwGap = 0;
                            if(NETDEV_LED_FONT_GAP_INVALID == stvirtualLEDInfo.stFontInfo.udwGap)
                            {
                                udwGap = stvirtualLEDInfo.stFontInfo.udwReferFontGap;
                                convertFontSize2Client(udwGap);
                            }
                            else {
                                udwGap = stvirtualLEDInfo.stFontInfo.udwGap;
                                convertFontGap2Client(udwFontSize, udwGap);
                            }

                            virtualLED->setClientFontSize(udwFontSize,udwGap);
                            virtualLED->setVirtualLEDInfo(stvirtualLEDInfo);
                        }
                    }
                    break;
                }
            }
        }

        /*移动后刷新界面*/
        update();
    }
    else {
        event->ignore();
    }
}

/* 转换客户端界面屏幕坐标到电视墙坐标 */
void MPlayWidget::convertScreen2DeviceTVwallPos(NETDEV_XW_AREA_S &stArea)
{
    stArea.stTopLeft.udwX = stArea.stTopLeft.udwX * (10080 * m_stTVWallCfg.stScreenInfo.udwColNum) / m_dwTVWallWidgetWidth;
    stArea.stTopLeft.udwY = stArea.stTopLeft.udwY * (10080 * m_stTVWallCfg.stScreenInfo.udwRowNum) / m_dwTVWallWidgetHeight;
    stArea.stBottomRight.udwX = stArea.stBottomRight.udwX * (10080 * m_stTVWallCfg.stScreenInfo.udwColNum) / m_dwTVWallWidgetWidth;
    stArea.stBottomRight.udwY = stArea.stBottomRight.udwY * (10080 * m_stTVWallCfg.stScreenInfo.udwRowNum) / m_dwTVWallWidgetHeight;
}

/* 转换电视墙坐标到客户端界面屏幕坐标 */
void MPlayWidget::convertDeviceTVwall2ScreenPos(NETDEV_XW_AREA_S &stArea)
{
    stArea.stTopLeft.udwX = stArea.stTopLeft.udwX * m_dwTVWallWidgetWidth / (10080 * m_stTVWallCfg.stScreenInfo.udwColNum);
    stArea.stTopLeft.udwY = stArea.stTopLeft.udwY * m_dwTVWallWidgetHeight / (10080 * m_stTVWallCfg.stScreenInfo.udwRowNum);
    stArea.stBottomRight.udwX = stArea.stBottomRight.udwX * m_dwTVWallWidgetWidth / (10080 * m_stTVWallCfg.stScreenInfo.udwColNum);
    stArea.stBottomRight.udwY = stArea.stBottomRight.udwY * m_dwTVWallWidgetHeight / (10080 * m_stTVWallCfg.stScreenInfo.udwRowNum);
}

/* 转换设备上虚拟LED字体大小 0-10080 - 屏幕分辨率 */
void MPlayWidget::convertFontSize2Device(const NETDEV_XW_AREA_S stArea, UINT32 &dwFontSize)
{
    /* 0-10080 - 屏幕分辨率 */
    UINT32 dwHeightFontSize = (stArea.stBottomRight.udwY - stArea.stTopLeft.udwY) * m_dwVideoOutFormatHeight / 10080;
    UINT32 dwWidthFontSize = (stArea.stBottomRight.udwX - stArea.stTopLeft.udwX) * m_dwVideoOutFormatWidth / 10080;

    dwFontSize = dwHeightFontSize < dwWidthFontSize ? dwHeightFontSize : dwWidthFontSize;
    if(dwFontSize < 48)/* 最小字体大小48 */
    {
        dwFontSize = 48;
    }
    else if(dwFontSize > 1024)/* 最大字体大小1024 */
    {
        dwFontSize = 1024;
    }
}

/* 转换客户端界面上虚拟LED字体大小 (设备电视墙显示屏总宽高(分辨率)) - (客户端界面电视墙总宽高) */
void MPlayWidget::convertFontSize2Client(UINT32 &dwFontSize)
{
    UINT32 dwHeightFontSize = dwFontSize * m_dwTVWallWidgetHeight / (m_dwVideoOutFormatHeight * m_stTVWallCfg.stScreenInfo.udwRowNum);
    UINT32 dwWidthFontSize = dwFontSize * m_dwTVWallWidgetWidth / (m_dwVideoOutFormatWidth * m_stTVWallCfg.stScreenInfo.udwColNum);
    dwFontSize = dwHeightFontSize < dwWidthFontSize ? dwHeightFontSize : dwWidthFontSize;
}

/* 转换设备上虚拟LED字间距 10080*n - 屏幕分辨率*n */
void MPlayWidget::convertFontGap2Device(const NETDEV_XW_AREA_S stArea, UINT32 dwFontSize, int dwCount, UINT32 &dwFontGap)
{
    /* 虚拟LED在显示屏上实际宽度(分辨率像素单位) */
    UINT32 dwWidth = (stArea.stBottomRight.udwX - stArea.stTopLeft.udwX) * m_dwVideoOutFormatWidth / 10080;

    if(dwWidth < dwFontSize * dwCount)
    {
        dwFontGap = 0;
        return;
    }

    if(dwCount > 1)
    {
        dwFontGap = (dwWidth - dwFontSize * dwCount) / (dwCount - 1);
    }
    else if(1 == dwCount){
        dwFontGap = dwWidth - dwFontSize;
    }
}

/* 转换客户端界面上虚拟LED字间距 */
void MPlayWidget::convertFontGap2Client(UINT32 dwClientFontSize, UINT32 &dwFontGap)
{
    for(int i = 0; i < sizeof(gaFontGapConvert)/sizeof(NETDEMO_LED_FONT_GAP_MAPPING_S); i++)
    {
        if(dwFontGap == gaFontGapConvert[i].eFontGap)
        {
            dwFontGap = (dwClientFontSize * gaFontGapConvert[i].dwRate) / 4;
            break;
        }
    }
}

/* 判断鼠标按下位置是否有效 */
bool MPlayWidget::mousePointhasValid(QPoint pos)
{
    if(pos.x() >= m_dwTVWallWidgetWidth ||
       pos.y() >= m_dwTVWallWidgetHeight ||
       pos.x() < 0 ||
            pos.y() < 0)
    {
        return false;
    }

    return true;
}

/* 开窗 */
void MPlayWidget::createWnd(NETDEV_XW_SCENE_WND_INFO_S stWndInfo)
{
    if(-1 == m_dwDecoderIndex)
    {
        return;
    }

    strncpy(stWndInfo.szWndName,QString::number(m_dwMaxWndIndex).toStdString().c_str(),sizeof(stWndInfo.szWndName) - 1);
    UINT32 udwLastChange = 0;
    BOOL bRet = NETDEV_XW_CreateWnd(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                    this->m_stTVWallCfg.udwTVWallID,
                                    &stWndInfo,
                                    &udwLastChange);
   if(FALSE == bRet)
   {
       qDebug() << "create window fail";
       return;
   }

    PlayWindow* playWindow = new PlayWindow(this);
    playWindow->setDecoderIndex(m_dwDecoderIndex);
    playWindow->setTVWallID(this->m_stTVWallCfg.udwTVWallID);
    playWindow->setWndInfo(stWndInfo);
    qDebug() << "m_startPoint:" << m_startPoint.x() << ":" << m_startPoint.y();
    playWindow->setGeometry(QRect(m_startPoint,m_endPoint));
    playWindow->show();

    /* 窗口被选中时，发射信号 */
    connect(playWindow, SIGNAL(selectedSignal(UINT32)),this,SLOT(playWindowSelectChangedSlot(UINT32)));
}

/* 当前选中窗口变更 */
void MPlayWidget::playWindowSelectChangedSlot(UINT32 udwWndID)
{
    this->m_udwCurrentSelectedWndID = udwWndID;

    /* 同步更新所有显示窗口,非当前选中窗口则取消选中状态 */
    QList<PlayWindow*> playWindowList = this->findChildren<PlayWindow*>();
    for(int i = 0; i < playWindowList.size(); i++)
    {
        PlayWindow* playWindow = playWindowList.at(i);
        if(m_udwCurrentSelectedWndID != playWindow->getWndInfo().udwWndID)
        {
            playWindow->clearSelectedStatus();
        }
    }
}

/* 当前选中虚拟LED变更 */
void MPlayWidget::virtualLEDSelectChangedSlot(UINT32 udwLEDID)
{
    this->m_udwCurrentSelectedLEDID = udwLEDID;

    /* 同步更新所有显示窗口,非当前选中窗口则取消选中状态 */
    QList<VirtualLED*> virtualLEDList = this->findChildren<VirtualLED*>();
    for(int i = 0; i < virtualLEDList.size(); i++)
    {
        VirtualLED* virtualLED = virtualLEDList.at(i);
        if(m_udwCurrentSelectedLEDID != virtualLED->getVirtualLEDInfo().udwLEDID)
        {
            virtualLED->clearSelectedStatus();
        }
    }
}

/* 修改虚拟LED信息 */
void MPlayWidget::onModifyVirtualLEDSlot(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo)
{
    m_pVirtualLEDCfgDlg->setVirtualLEDInfo(stVirtualLEDInfo,false);
    m_pVirtualLEDCfgDlg->show();
}

/* 虚拟LED信息变更 */
void MPlayWidget::virtualLEDInfoChangedSlot(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo, bool bIsAdd)
{
    qDebug() << "font count:" << QString(stVirtualLEDInfo.szText).length();
    UINT32 udwFontSize = 0;
    if(NETDEV_FONT_SIZE_SELF_ADAPTION == stVirtualLEDInfo.stFontInfo.udwSize)/* 字大小自适应 */
    {
        convertFontSize2Device(stVirtualLEDInfo.stArea,stVirtualLEDInfo.stFontInfo.udwReferFontSize);
        udwFontSize = stVirtualLEDInfo.stFontInfo.udwReferFontSize;
    }
    else {
        udwFontSize = stVirtualLEDInfo.stFontInfo.udwSize;
    }

    if(NETDEV_LED_FONT_GAP_INVALID == stVirtualLEDInfo.stFontInfo.udwGap)/*字间距自适应*/
    {
        convertFontGap2Device(stVirtualLEDInfo.stArea,
                              udwFontSize,
                               QString(stVirtualLEDInfo.szText).length(),
                               stVirtualLEDInfo.stFontInfo.udwReferFontGap);
    }

    if(true == bIsAdd)/* 创建虚拟LED */
    {
        createVirtualLED(stVirtualLEDInfo);
    }
    else/* 修改虚拟LED */
    {
        if(true == modifyVirtualLEDInfo(stVirtualLEDInfo))
        {
            QList<VirtualLED*> virtualLEDList = this->findChildren<VirtualLED*>();
            for(int i = 0; i < virtualLEDList.size(); i++)
            {
                VirtualLED* virtualLED = virtualLEDList.at(i);
                if(m_udwCurrentSelectedLEDID == virtualLED->getVirtualLEDInfo().udwLEDID)
                {
                    if(true == getVirtualLEDInfo(&stVirtualLEDInfo))
                    {
                        UINT32 udwFontSize = 48;
                        if(NETDEV_FONT_SIZE_SELF_ADAPTION == stVirtualLEDInfo.stFontInfo.udwSize)
                        {
                            udwFontSize = stVirtualLEDInfo.stFontInfo.udwReferFontSize;
                        }
                        else {
                            udwFontSize = stVirtualLEDInfo.stFontInfo.udwSize;
                        }
                        convertFontSize2Client(udwFontSize);

                        UINT32 udwGap = 0;
                        if(NETDEV_LED_FONT_GAP_INVALID == stVirtualLEDInfo.stFontInfo.udwGap)
                        {
                            udwGap = stVirtualLEDInfo.stFontInfo.udwReferFontGap;
                            convertFontSize2Client(udwGap);
                        }
                        else {
                            udwGap = stVirtualLEDInfo.stFontInfo.udwGap;
                            convertFontGap2Client(udwFontSize, udwGap);
                        }

                        virtualLED->setClientFontSize(udwFontSize,udwGap);
                        virtualLED->setVirtualLEDInfo(stVirtualLEDInfo);
                    }
                    break;
                }
            }
        }
    }
}

/* 获取窗口信息 */
bool MPlayWidget::getWndInfo(LPNETDEV_XW_SCENE_WND_INFO_S pstWndInfo)
{
    if(-1 == m_dwDecoderIndex)
    {
        return false;
    }

    BOOL bRet = NETDEV_XW_GetWnd(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                    this->m_stTVWallCfg.udwTVWallID,
                                    pstWndInfo);
   if(FALSE == bRet)
   {
       qDebug() << "get window info fail";
       return false;
   }

   return true;
}

/* 修改窗口信息 */
bool MPlayWidget::modifyWndInfo(NETDEV_XW_SCENE_WND_INFO_S stWndInfo)
{
    if(-1 == m_dwDecoderIndex)
    {
        return false;
    }

    UINT32 udwLastChange = 0;
    BOOL bRet = NETDEV_XW_ModifyWnd(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                    this->m_stTVWallCfg.udwTVWallID,
                                    &stWndInfo,
                                    &udwLastChange);
   if(FALSE == bRet)
   {
       qDebug() << "modify window fail";
       return false;
   }

   return true;
}

/* 创建虚拟LED */
void MPlayWidget::createVirtualLED(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo)
{
    if(-1 == m_dwDecoderIndex)
    {
        return;
    }

    UINT32 udwLastChange = 0;
    BOOL bRet = NETDEV_XW_CreateVirtualLED(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                    this->m_stTVWallCfg.udwTVWallID,
                                    &stVirtualLEDInfo,
                                    &udwLastChange);
   if(FALSE == bRet)
   {
       qDebug() << "create Virtual LED fail";
       return;
   }

    VirtualLED* virtualLED = new VirtualLED(this);
    virtualLED->setDecoderIndex(m_dwDecoderIndex);
    virtualLED->setTVWallID(this->m_stTVWallCfg.udwTVWallID);
    UINT32 udwFontSize = 48;
    if(NETDEV_FONT_SIZE_SELF_ADAPTION == stVirtualLEDInfo.stFontInfo.udwSize)
    {
        udwFontSize = stVirtualLEDInfo.stFontInfo.udwReferFontSize;
    }
    else {
        udwFontSize = stVirtualLEDInfo.stFontInfo.udwSize;
    }
    convertFontSize2Client(udwFontSize);

    UINT32 udwGap = 0;
    if(NETDEV_LED_FONT_GAP_INVALID == stVirtualLEDInfo.stFontInfo.udwGap)
    {
        udwGap = stVirtualLEDInfo.stFontInfo.udwReferFontGap;
        convertFontSize2Client(udwGap);
    }
    else {
        udwGap = stVirtualLEDInfo.stFontInfo.udwGap;
        convertFontGap2Client(udwFontSize, udwGap);
    }

    virtualLED->setClientFontSize(udwFontSize,udwGap);
    virtualLED->setVirtualLEDInfo(stVirtualLEDInfo);
    virtualLED->setGeometry(QRect(m_startPoint,m_endPoint));
    virtualLED->show();

    /* 窗口被选中时，发射信号 */
    connect(virtualLED, SIGNAL(selectedSignal(UINT32)),this,SLOT(virtualLEDSelectChangedSlot(UINT32)));
    /* 虚拟LED修改时，发射信号 */
    connect(virtualLED, SIGNAL(modifyVirtualLEDSignal(NETDEV_XW_VIRTUAL_LED_INFO_S)),this,SLOT(onModifyVirtualLEDSlot(NETDEV_XW_VIRTUAL_LED_INFO_S)));
}

/* 获取虚拟LED信息 */
bool MPlayWidget::getVirtualLEDInfo(LPNETDEV_XW_VIRTUAL_LED_INFO_S pVirtualLEDInfo)
{
    if(-1 == m_dwDecoderIndex)
    {
        return false;
    }

    BOOL bRet = NETDEV_XW_GetVirtualLED(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                    this->m_stTVWallCfg.udwTVWallID,
                                    pVirtualLEDInfo);
   if(FALSE == bRet)
   {
       qDebug() << "get Virtual LED Info fail";
       return false;
   }

   return true;
}

/* 修改虚拟LED信息 */
bool MPlayWidget::modifyVirtualLEDInfo(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo)
{
    if(-1 == m_dwDecoderIndex)
    {
        return false;
    }

    UINT32 udwLastChange = 0;
    BOOL bRet = NETDEV_XW_SetVirtualLED(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                    this->m_stTVWallCfg.udwTVWallID,
                                    &stVirtualLEDInfo,
                                    &udwLastChange);
   if(FALSE == bRet)
   {
       qDebug() << "modify Virtual LED Info fail";
       return false;
   }

   return true;
}

/*设置解码器设备索引*/
void MPlayWidget::setDecoderIndex(int dwDecoderIndex)
{
    m_dwDecoderIndex = dwDecoderIndex;
}

/* 清理现有电视墙显示单元及窗口 */
void MPlayWidget::clearTVWallDisplayPage()
{
    m_dwDecoderIndex = -1; /* 使用该值表示是否有电视墙 */
    m_udwCurrentSelectedWndID = 0;
    m_udwCurrentSelectedLEDID = 0;
    memset(&m_stTVWallCfg, 0, sizeof(m_stTVWallCfg));

    m_dwMaxWndIndex = 1;
    m_bIsRightMousePressMove = false;
    m_bIsLeftMousePressMove = false;


    /* 清理电视墙显示单元 */
    m_TVWallDisplayUnitList.clear();

    if(NETDEMO_TVWALL_SHOW_TYPE_WINDOW == m_eShowType)/* 清理窗口 */
    {
        QList<PlayWindow*> playWindowList = this->findChildren<PlayWindow*>();
        for(int i = 0; i < playWindowList.size(); i++)
        {
              delete (PlayWindow*)playWindowList.at(i);
        }

        /* 清理电视墙关联业务流缓存 */
        m_streamInfoList.clear();
    }
    else if(NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED == m_eShowType)/* 清理虚拟LED */
    {
        QList<VirtualLED*> virtualLEDList = this->findChildren<VirtualLED*>();
        for(int i = 0; i < virtualLEDList.size(); i++)
        {
              delete (VirtualLED*)virtualLEDList.at(i);
        }
    }


    m_eShowType = NETDEMO_TVWALL_SHOW_TYPE_WINDOW;/* 默认显示窗口 */

    update();
}

/* 电视墙展示窗口还是展示虚拟LED */
void MPlayWidget::setShowType(NETDEMO_TVWALL_SHOW_TYPE_E eShowType)
{
    m_eShowType = eShowType;
}

/* 设置电视墙信息 */
void MPlayWidget::setTVWallInfo(NETDEV_XW_TVWALL_CFG_S stTVWallCfg)
{
    if(-1 == m_dwDecoderIndex)
    {
        return;
    }

    this->m_stTVWallCfg = stTVWallCfg;

    /* 解析显示单元分辨率信息 */
    for(int i = 0; i < sizeof(gaFormat)/sizeof(NETDEMO_FORMAT_MAPPING_S);i++)
    {
        if(gaFormat[i].dwFormat == m_stTVWallCfg.stScreenInfo.udwFormat)
        {
            m_dwVideoOutFormatWidth = gaFormat[i].strFormat.section('x',0,0).toInt();
            m_dwVideoOutFormatHeight = gaFormat[i].strFormat.section('x',1).section('@',0,0).toInt();
            break;
        }
    }

    /* 电视墙显示单元信息 */
    int dwScreenIDIndex = 1;
    for(int dwRowIndex = 0; dwRowIndex < m_stTVWallCfg.stScreenInfo.udwRowNum; dwRowIndex++)
    {
        for(int dwColumnIndex = 0; dwColumnIndex < m_stTVWallCfg.stScreenInfo.udwColNum; dwColumnIndex++)
        {
            NETDEMO_TVWALL_DISPLAY_UNIT_INFO_S stTVWallDisplayUnitInfo;
            stTVWallDisplayUnitInfo.dwRowIndex = dwRowIndex;
            stTVWallDisplayUnitInfo.dwColumnIndex = dwColumnIndex;
            stTVWallDisplayUnitInfo.dwScreenIDIndex = dwScreenIDIndex;

            /* 显示单元描述信息 */
            int dwVideoOutIndex = 0;
            for(; dwVideoOutIndex < stTVWallCfg.udwVideoOutNum;dwVideoOutIndex++)
            {
                if(dwScreenIDIndex == stTVWallCfg.astVideoOut[dwVideoOutIndex].udwScreenID)
                {
                    stTVWallDisplayUnitInfo.bIsBind = true;
                    stTVWallDisplayUnitInfo.strDescText = QString(gDecoderList[m_dwDecoderIndex].stDeviceBaseInfo.szDevModel)+"_"+
                            gDecoderList[m_dwDecoderIndex].stDeviceLoginInfo.strIpAddress+":"+
                            QString::number(m_stTVWallCfg.astVideoOut[dwVideoOutIndex].udwScreenID) + ":"+
                            QString::number(m_stTVWallCfg.astVideoOut[dwVideoOutIndex].udwVideoOutID);
                    break;
                }
            }

            if(dwVideoOutIndex == stTVWallCfg.udwVideoOutNum)
            {
                stTVWallDisplayUnitInfo.bIsBind = false;
                stTVWallDisplayUnitInfo.strDescText = QString("未绑定");
            }

            dwScreenIDIndex++;

            m_TVWallDisplayUnitList.append(stTVWallDisplayUnitInfo);
        }
    }

    refreshTvWallDisplayUnitSize();
    if(NETDEMO_TVWALL_SHOW_TYPE_WINDOW == m_eShowType)
    {
        getStreamInfo(this->m_stTVWallCfg.udwTVWallID);
        showAllTVWallWnd(this->m_stTVWallCfg.udwTVWallID);
    }
    else if(NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED == m_eShowType)
    {
        showAllTVWallVirtualLED(this->m_stTVWallCfg.udwTVWallID);
    }

    update();
}

/* 显示电视墙虚拟LED信息 */
void MPlayWidget::showAllTVWallVirtualLED(UINT32 udwTVWallID)
{
    /* 获取虚拟LED配置信息 */
    LPVOID lpFindWndHandle = NETDEV_XW_FindVirtualLEDList(gDecoderList[m_dwDecoderIndex].lpLoginHandle,udwTVWallID);
    if(NULL != lpFindWndHandle)
    {
        while(true)
        {
            NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo = {0};
            BOOL bRet = NETDEV_XW_FindNextVirtualLED(lpFindWndHandle,&stVirtualLEDInfo);
            if(FALSE == bRet)
            {
                break;
            }

            /* 显示窗口 */
            NETDEV_XW_AREA_S stArea = stVirtualLEDInfo.stArea;
            convertDeviceTVwall2ScreenPos(stArea);
            VirtualLED* virtualLED = new VirtualLED(this);
            virtualLED->setDecoderIndex(m_dwDecoderIndex);
            virtualLED->setTVWallID(udwTVWallID);
            UINT32 udwFontSize = 48;
            if(NETDEV_FONT_SIZE_SELF_ADAPTION == stVirtualLEDInfo.stFontInfo.udwSize)
            {
                udwFontSize = stVirtualLEDInfo.stFontInfo.udwReferFontSize;
            }
            else {
                udwFontSize = stVirtualLEDInfo.stFontInfo.udwSize;
            }
            convertFontSize2Client(udwFontSize);

            UINT32 udwGap = 0;
            if(NETDEV_LED_FONT_GAP_INVALID == stVirtualLEDInfo.stFontInfo.udwGap)
            {
                udwGap = stVirtualLEDInfo.stFontInfo.udwReferFontGap;
                convertFontSize2Client(udwGap);
            }
            else {
                udwGap = stVirtualLEDInfo.stFontInfo.udwGap;
                convertFontGap2Client(udwFontSize, udwGap);
            }

            virtualLED->setClientFontSize(udwFontSize, udwGap);
            virtualLED->setVirtualLEDInfo(stVirtualLEDInfo);
            virtualLED->setGeometry(QRect(QPoint(stArea.stTopLeft.udwX,stArea.stTopLeft.udwY),QPoint(stArea.stBottomRight.udwX,stArea.stBottomRight.udwY)));
            virtualLED->show();

            /* 虚拟LED窗口被选中时，发射信号 */
            connect(virtualLED, SIGNAL(selectedSignal(UINT32)),this,SLOT(virtualLEDSelectChangedSlot(UINT32)));
            /* 虚拟LED修改时，发射信号 */
            connect(virtualLED, SIGNAL(modifyVirtualLEDSignal(NETDEV_XW_VIRTUAL_LED_INFO_S)),this,SLOT(onModifyVirtualLEDSlot(NETDEV_XW_VIRTUAL_LED_INFO_S)));
        }

        NETDEV_XW_FindCloseVirtualLED(lpFindWndHandle);
    }
}

/* 显示电视墙关联窗口信息 */
void MPlayWidget::showAllTVWallWnd(UINT32 udwTVWallID)
{
    /* 获取窗口配置信息 */
    LPVOID lpFindWndHandle = NETDEV_XW_FindWndList(gDecoderList[m_dwDecoderIndex].lpLoginHandle,udwTVWallID);
    if(NULL != lpFindWndHandle)
    {
        while(true)
        {
            NETDEV_XW_SCENE_WND_INFO_S stWndInfo = {0};
            BOOL bRet = NETDEV_XW_FindNextWnd(lpFindWndHandle,&stWndInfo);
            if(FALSE == bRet)
            {
                break;
            }

            /* 显示窗口 */
            NETDEV_XW_AREA_S stArea = stWndInfo.stArea;
            convertDeviceTVwall2ScreenPos(stArea);
            PlayWindow* playWindow = new PlayWindow(this);
            playWindow->setDecoderIndex(m_dwDecoderIndex);
            playWindow->setTVWallID(udwTVWallID);
            /* 设置该窗口关联的业务流 */
            playWindow->clearStreamInfo();
            for(int i = 0; i < m_streamInfoList.size(); i++)
            {
                if(stWndInfo.udwWndID == m_streamInfoList[i].stStreamInfoDest.udwWndID)
                {
                    playWindow->appendStreamInfo(m_streamInfoList[i]);
                }
            }

            playWindow->setWndInfo(stWndInfo);
            playWindow->setGeometry(QRect(QPoint(stArea.stTopLeft.udwX,stArea.stTopLeft.udwY),QPoint(stArea.stBottomRight.udwX,stArea.stBottomRight.udwY)));
            playWindow->show();

            /* 窗口被选中时，发射信号 */
            connect(playWindow, SIGNAL(selectedSignal(UINT32)),this,SLOT(playWindowSelectChangedSlot(UINT32)));
        }

        NETDEV_XW_FindCloseWnd(lpFindWndHandle);
    }
}

/* 删除电视墙所有窗口 */
void MPlayWidget::deleteAllWndInfo()
{
    /* 先查找,再批量删除窗口 */
    QList<UINT32> udwWndIDList;
    LPVOID lpFindWndHandle = NETDEV_XW_FindWndList(gDecoderList[m_dwDecoderIndex].lpLoginHandle,m_stTVWallCfg.udwTVWallID);
    if(NULL != lpFindWndHandle)
    {
        while(true)
        {
            NETDEV_XW_SCENE_WND_INFO_S stWndInfo = {0};
            BOOL bRet = NETDEV_XW_FindNextWnd(lpFindWndHandle,&stWndInfo);
            if(FALSE == bRet)
            {
                break;
            }

            udwWndIDList.append(stWndInfo.udwWndID);
        }

        NETDEV_XW_FindCloseWnd(lpFindWndHandle);
    }

    if(udwWndIDList.size() == 0)
    {
        return;
    }

    NETDEV_XW_BATCH_RESULT_LIST_S stWinInfoList = {0};
    stWinInfoList.udwSize = udwWndIDList.size();
    stWinInfoList.pstResultInfo = new NETDEV_XW_BATCH_RESULT_WND_S[stWinInfoList.udwSize];
    for(int i = 0; i< stWinInfoList.udwSize; i++)
    {
        stWinInfoList.pstResultInfo[i].udwWinID = udwWndIDList[i];
    }
    BOOL bRet = NETDEV_XW_DeleteBatchWnd(gDecoderList[m_dwDecoderIndex].lpLoginHandle,10,m_stTVWallCfg.udwTVWallID,&stWinInfoList);
    if(FALSE == bRet)
    {
        qDebug() << "Delete all Wnd fail";
        return;
    }

    /* 清理窗口 */
    if(NETDEMO_TVWALL_SHOW_TYPE_WINDOW == m_eShowType)
    {
        QList<PlayWindow*> playWindowList = this->findChildren<PlayWindow*>();
        for(int i = 0; i < playWindowList.size(); i++)
        {
              delete (PlayWindow*)playWindowList.at(i);
        }

        /* 清理电视墙关联业务流 */
        m_streamInfoList.clear();
    }
}

/* 删除电视墙所有虚拟LED */
void MPlayWidget::deleteAllVirtualLEDInfo()
{
    /* 先查找,再逐个删除虚拟LED */
    LPVOID lpFindWndHandle = NETDEV_XW_FindVirtualLEDList(gDecoderList[m_dwDecoderIndex].lpLoginHandle,m_stTVWallCfg.udwTVWallID);
    if(NULL != lpFindWndHandle)
    {
        while(true)
        {
            NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo = {0};
            BOOL bRet = NETDEV_XW_FindNextVirtualLED(lpFindWndHandle,&stVirtualLEDInfo);
            if(FALSE == bRet)
            {
                break;
            }

            UINT32 udwLastChange = 0;
            bRet = NETDEV_XW_DeleteOneVirtualLED(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                           m_stTVWallCfg.udwTVWallID,
                                           stVirtualLEDInfo.udwLEDID,
                                           &udwLastChange);
            if(FALSE == bRet)
            {
                qDebug() << "Delete virtual LED fail," << stVirtualLEDInfo.udwLEDID;
            }
        }

        NETDEV_XW_FindCloseVirtualLED(lpFindWndHandle);
    }

    /* 清理虚拟LED */
    if(NETDEMO_TVWALL_SHOW_TYPE_VIRTUAL_LED == m_eShowType)
    {
        QList<VirtualLED*> virtualLEDList = this->findChildren<VirtualLED*>();
        for(int i = 0; i < virtualLEDList.size(); i++)
        {
              delete (VirtualLED*)virtualLEDList.at(i);
        }
    }
}

/* 设置窗口分屏 */
void MPlayWidget::setWindowSplitScreen(int dwSplitScreenNumber)
{
    QList<PlayWindow*> playWindowList = this->findChildren<PlayWindow*>();
    for(int i = 0; i < playWindowList.size(); i++)
    {
        PlayWindow* playWindow = playWindowList.at(i);
        if(m_udwCurrentSelectedWndID == playWindow->getWndInfo().udwWndID)
        {
            /*修改窗口分屏*/
            NETDEV_XW_SCENE_WND_INFO_S stWndInfo = playWindow->getWndInfo();
            stWndInfo.udwModifyCmd = NETDEV_WND_PANE_MOD;
            if(1 == dwSplitScreenNumber)
            {
                stWndInfo.udwPaneMod = NETDEV_XW_LAYOUT_TYPE_01;
            }
            else if(2 == dwSplitScreenNumber){
                stWndInfo.udwPaneMod = NETDEV_XW_LAYOUT_TYPE_04;
            }
            else if(3 == dwSplitScreenNumber){
                stWndInfo.udwPaneMod = NETDEV_XW_LAYOUT_TYPE_09;
            }
            else if(4 == dwSplitScreenNumber){
                stWndInfo.udwPaneMod = NETDEV_XW_LAYOUT_TYPE_16;
            }

            bool bRet = this->modifyWndInfo(stWndInfo);
            if(true == bRet)
            {
                /* 更新窗口业务流信息 */
                getStreamInfo(this->m_stTVWallCfg.udwTVWallID);

                /* 更新每个窗口关联的业务流 */
                playWindow->clearStreamInfo();
                for(int i = 0; i < m_streamInfoList.size(); i++)
                {
                    if(playWindow->getWndInfo().udwWndID == m_streamInfoList[i].stStreamInfoDest.udwWndID)
                    {
                        playWindow->appendStreamInfo(m_streamInfoList[i]);
                    }
                }
                playWindow->setWndInfo(stWndInfo);
            }

            break;
        }
    }
}

/* 获取电视墙关联业务流信息 */
void MPlayWidget::getStreamInfo(UINT32 udwTVWallID)
{
    if(-1 == m_dwDecoderIndex)
    {
        return;
    }

    m_streamInfoList.clear();
    LPVOID lpFindStreamHandle = NETDEV_XW_FindStreamList(gDecoderList[m_dwDecoderIndex].lpLoginHandle,udwTVWallID);
    if(NULL != lpFindStreamHandle)
    {
        while(true)
        {
            NETDEV_XW_STREAM_INFO_S stXWStreamInfo = {0};
            BOOL bRet = NETDEV_XW_FindNextStreamInfo(lpFindStreamHandle,&stXWStreamInfo);
            if(FALSE == bRet)
            {
                break;
            }

            m_streamInfoList.append(stXWStreamInfo);
        }

        NETDEV_XW_FindCloseStreamInfo(lpFindStreamHandle);
    }
}

