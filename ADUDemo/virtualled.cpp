#include "virtualled.h"
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include <QHBoxLayout>

VirtualLED::VirtualLED(QWidget *parent) : QWidget(parent)
{
    m_dwTVWallID = -1;
    m_dwDecoderIndex = -1;
    memset(&m_stVirtualLEDInfo,0,sizeof(NETDEV_XW_VIRTUAL_LED_INFO_S));

    m_pLEDText = new VirtualLEDText();

    /*窗口标题栏*/
    m_titleBarWIdget = new QWidget();
    QPushButton* closeBtn = new QPushButton();
    m_titleBarWIdget->setMaximumHeight(16);
    closeBtn->setFixedSize(16,16);
    closeBtn->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton)));
    m_titleBarWIdget->setStyleSheet("background-color:rgb(25,25,25);");
    closeBtn->setStyleSheet("background-color:white;");

    QPushButton* modifyBtn = new QPushButton();
    modifyBtn->setFixedSize(16,16);
    modifyBtn->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogResetButton)));
    modifyBtn->setStyleSheet("background-color:white;");

    QHBoxLayout* titleBarLayout = new QHBoxLayout();
    titleBarLayout->setSpacing(0);
    titleBarLayout->setMargin(0);
    titleBarLayout->setAlignment(Qt::AlignRight);
    titleBarLayout->addWidget(modifyBtn);
    titleBarLayout->addWidget(closeBtn);
    m_titleBarWIdget->setLayout(titleBarLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    m_titleBarWIdget->hide();/* 默认隐藏标题栏 */

    mainLayout->addWidget(m_titleBarWIdget);
    mainLayout->addWidget(m_pLEDText);
    setLayout(mainLayout);

    /* 信号槽 */
    connect(closeBtn,SIGNAL(clicked()),this,SLOT(onCloseWindowSlot()));
    connect(modifyBtn,SIGNAL(clicked()),this,SLOT(onModifyVirtualLEDSlot()));
    connect(m_pLEDText,SIGNAL(selectedSignal()),this,SLOT(onLEDTextSelectedSlot()));
}

int VirtualLED::getDecoderIndex()
{
    return this->m_dwDecoderIndex;
}
void VirtualLED::setDecoderIndex(int dwDecoderIndex)
{
    this->m_dwDecoderIndex = dwDecoderIndex;
}

int VirtualLED::getTVWallID()
{
    return this->m_dwTVWallID;
}
void VirtualLED::setTVWallID(int dwTVWallID)
{
    this->m_dwTVWallID = dwTVWallID;
}

/* 设置客户端界面显示字体大小、间距 */
void VirtualLED::setClientFontSize(UINT32 udwClientFontSize, UINT32 udwClientFontGap)
{
    m_udwClientFontSize = udwClientFontSize;
    m_udwClientFontGap = udwClientFontGap;
    m_pLEDText->setFontTypeSizeSpacing((NETDEV_XW_LED_FONT_FORMAT_E)m_stVirtualLEDInfo.stFontInfo.udwFont,m_udwClientFontSize, m_udwClientFontGap);
}

void VirtualLED::setVirtualLEDInfo(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo)
{
    m_stVirtualLEDInfo = stVirtualLEDInfo;

    /*设置颜色，如果虚拟LED未开启，则显示灰底灰字*/
    QString strHexBackgroundColor;
    QString strHexFontColor;
    if(TRUE == stVirtualLEDInfo.udwEnable)
    {
        strHexBackgroundColor = QString("%1").arg(stVirtualLEDInfo.stBackground.udwBaseColor,6,16,QLatin1Char('0'));
        strHexFontColor = QString("%1").arg(stVirtualLEDInfo.stFontInfo.udwColor,6,16,QLatin1Char('0'));
    }
    else {
        strHexBackgroundColor = "323842";
        strHexFontColor = "6a6a6a";
    }

    m_pLEDText->setText(QString(tr(stVirtualLEDInfo.szText)));
    m_pLEDText->setColor(strHexBackgroundColor, strHexFontColor);
    m_pLEDText->setAlignment((NETDEV_XW_HORIZONTAL_ALIGN_MODE_E)stVirtualLEDInfo.stFontInfo.udwHorizontalAlign,
                             (NETDEV_XW_VERTICAL_ALIGN_MODE_E)stVirtualLEDInfo.stFontInfo.udwVerticalAlign);
    m_pLEDText->setFontTypeSizeSpacing((NETDEV_XW_LED_FONT_FORMAT_E)stVirtualLEDInfo.stFontInfo.udwFont,m_udwClientFontSize,m_udwClientFontGap);
    m_pLEDText->setTransparency(stVirtualLEDInfo.stBackground.udwTransparency);
    m_pLEDText->setScrollMode((NETDEV_XW_LED_SCROLLING_MODE_E)stVirtualLEDInfo.stFontInfo.udwScrollMode, stVirtualLEDInfo.stFontInfo.udwScrollRateSec);

    /* 显示文本 */
    if(stVirtualLEDInfo.stFontInfo.udwGap);

}

NETDEV_XW_VIRTUAL_LED_INFO_S VirtualLED::getVirtualLEDInfo()
{
    return m_stVirtualLEDInfo;
}

/* 取消当前窗口的选中状态 */
void VirtualLED::clearSelectedStatus()
{
    m_titleBarWIdget->hide();
   m_pLEDText->clearSelectedStatus();
}

void VirtualLED::onModifyVirtualLEDSlot()
{
    emit modifyVirtualLEDSignal(m_stVirtualLEDInfo);
}

void VirtualLED::onCloseWindowSlot()
{
    /* 关闭窗口 */
    if(true == deleteVirtualLED())
    {
        setAttribute(Qt::WA_DeleteOnClose);/* 确保窗口关闭会销毁对象 */
        close();
    }
}

/* LED Label被选中 */
void VirtualLED::onLEDTextSelectedSlot()
{
    m_titleBarWIdget->show();
    emit selectedSignal(m_stVirtualLEDInfo.udwLEDID);
}

/* 关闭虚拟LED */
bool VirtualLED::deleteVirtualLED()
{
    UINT32 udwLastChange = 0;
    BOOL bRet = NETDEV_XW_DeleteOneVirtualLED(gDecoderList[m_dwDecoderIndex].lpLoginHandle,
                                    m_dwTVWallID,
                                    m_stVirtualLEDInfo.udwLEDID,
                                    &udwLastChange);

    if(FALSE == bRet)
    {
        qDebug() << "delete Virtual LED fail";
        return false;
    }

    return true;
}
