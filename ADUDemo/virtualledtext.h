#ifndef VIRTUALLEDTEXT_H
#define VIRTUALLEDTEXT_H

#include <QLabel>
#include "publictype.h"

class VirtualLEDText : public QLabel
{
    Q_OBJECT
public:
    explicit VirtualLEDText(QWidget *parent = nullptr);
    ~VirtualLEDText();

protected:
    void paintEvent(QPaintEvent* event) override;

public:
    void clearSelectedStatus();/* 取消选中状态 */

    void setColor(QString strHexBackgroundColor, QString strHexFontColor);/* 设置颜色 */
    void setFontTypeSizeSpacing(NETDEV_XW_LED_FONT_FORMAT_E Typeface, int dwPointSize, int dwFontGap);/* 设置字体、大小、间距 */
    void setTransparency(int  dwTransparency);/* 设置透明度 */
    void setAlignment(NETDEV_XW_HORIZONTAL_ALIGN_MODE_E eHAlignMode, NETDEV_XW_VERTICAL_ALIGN_MODE_E eVAlignMode);/* 设置对齐方式 */
    void setScrollMode(NETDEV_XW_LED_SCROLLING_MODE_E  eScrollMode, int  dwScrollRateSec);/* 设置字体滚动模式 */

protected:
    void focusInEvent(QFocusEvent *event) override;

signals:
    void selectedSignal();/*被选中*/

private:
    QFont m_font;/* 字体 */
    QString m_strFontFamily;/*字体类型*/
    QString m_strHexBackgroundColor;/* 背景颜色 */
    QString m_strHexFontColor;/* 字体颜色 */
    int m_alpha;/* 透明度 */
    Qt::AlignmentFlag m_alignmentHFlag; /* 水平对齐方式 */
    Qt::AlignmentFlag m_alignmentVFlag; /* 垂直对齐方式 */
};

#endif // VIRTUALLEDTEXT_H
