#include "virtualledtext.h"
#include <QPainter>
#include <QPen>

VirtualLEDText::VirtualLEDText(QWidget *parent):
    QLabel(parent)
{
    setFocusPolicy(Qt::ClickFocus);

    m_strHexBackgroundColor = "000000";
    m_strHexFontColor = "FF0000";
    m_alpha = 255;
    m_alignmentHFlag = Qt::AlignHCenter;
    m_alignmentVFlag = Qt::AlignVCenter;
}

VirtualLEDText::~VirtualLEDText()
{

}

void VirtualLEDText::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    QColor color = QColor(QString("#") + m_strHexBackgroundColor);
    color.setAlpha(m_alpha);
    painter.setBrush(color);// 设置画刷颜色

    painter.drawRect(0,0, width(),height());

    painter.setFont(m_font);
    painter.setPen(QPen(QColor(QString("#") + m_strHexFontColor)));

    //  设置绘制文字的开始位置，也就是将文字往左移动多少
    //rc.setLeft(rc.left() - left);

    //  如果文字已经显示到末尾，则再添加一遍文字，做出循环滚动的效果
    QString strText = text();
    //if(timerId >= 0)
    //strText += strSpace + text();

    qDebug() << "VirtualLEDText::paintEvent:" << width() << "-" << height();
    //  绘制文字
    painter.drawText(0,0, width(),height(), m_alignmentHFlag|m_alignmentVFlag, strText);
}

void VirtualLEDText::focusInEvent(QFocusEvent *event)
{
    emit selectedSignal();
}

/* 取消当前窗口的选中状态 */
void VirtualLEDText::clearSelectedStatus()
{

}

/* 设置颜色 */
void VirtualLEDText::setColor(QString strHexBackgroundColor, QString strHexFontColor)
{
    m_strHexBackgroundColor = strHexBackgroundColor;
    m_strHexFontColor = strHexFontColor;
}

/* 设置字体、大小、间距 */
void VirtualLEDText::setFontTypeSizeSpacing(NETDEV_XW_LED_FONT_FORMAT_E Typeface, int dwPointSize, int dwFontGap)
{
    switch (Typeface)
    {
        case NETDEV_LED_FONT_SONG:/* 宋体 */
        {
            m_strFontFamily = "SimSun";
            break;
        }
        case NETDEV_LED_FONT_FORMAT_SIMHEI: /* 黑体 */
        {
            m_strFontFamily = "SimHei";
            break;
        }
        case NETDEV_LED_FONT_FORMAT_KAITI: /* 楷体 */
        {
            m_strFontFamily = "KaiTi";
            break;
        }
        case NETDEV_LED_FONT_FORMAT_Arial: /* Arial */
        {
            m_strFontFamily = "Arial";
            break;
        }
        default:
            m_strFontFamily = "SimSun";
            break;
    }

    m_font.setFamily(m_strFontFamily);
    m_font.setPixelSize(dwPointSize);
    m_font.setLetterSpacing(QFont::AbsoluteSpacing,dwFontGap);

    update();
}

/* 设置透明度 */
void VirtualLEDText::setTransparency(int  dwTransparency)
{
    if(0 == dwTransparency)/* 不透明 */
    {
        m_alpha = 255;
    }
    else if(50 == dwTransparency)
    {
        m_alpha = 128;/* 半透明 */
    }
    else if(100 == dwTransparency)
    {
        m_alpha = 0;/* 全透明 */
    }

    update();
}

/* 设置对齐方式 */
void VirtualLEDText::setAlignment(NETDEV_XW_HORIZONTAL_ALIGN_MODE_E eHAlignMode, NETDEV_XW_VERTICAL_ALIGN_MODE_E eVAlignMode)
{
    if(NETDEV_HORIZONTAL_ALIGN_LEFT == eHAlignMode)/* 左对齐 */
    {
        m_alignmentHFlag = Qt::AlignLeft;
    }
    else if(NETDEV_HORIZONTAL_ALIGN_MIDDLE == eHAlignMode)/* 居中对齐 */
    {
        m_alignmentHFlag = Qt::AlignHCenter;
    }
    else if(NETDEV_HORIZONTAL_ALIGN_RIGHT == eHAlignMode)/* 右对齐 */
    {
        m_alignmentHFlag = Qt::AlignRight;
    }

    if(NETDEV_VERTICAL_ALIGN_TOP == eVAlignMode)/* 上对齐 */
    {
        m_alignmentVFlag = Qt::AlignTop;
    }
    else if(NETDEV_VERTICAL_ALIGN_MIDDLE == eVAlignMode)/* 居中对齐 */
    {
        m_alignmentVFlag = Qt::AlignVCenter;
    }
    else if(NETDEV_VERTICAL_ALIGN_BOTTOM == eVAlignMode)/* 下对齐 */
    {
        m_alignmentVFlag = Qt::AlignBottom;
    }

    update();
}

/* 设置字体滚动模式 */
void VirtualLEDText::setScrollMode(NETDEV_XW_LED_SCROLLING_MODE_E  eScrollMode, int  dwScrollRateSec)
{
    /* 界面暂未实现字体滚动 */
}
