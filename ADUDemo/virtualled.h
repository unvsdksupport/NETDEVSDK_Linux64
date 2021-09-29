#ifndef VIRTUALLED_H
#define VIRTUALLED_H

#include <QWidget>
#include <QLabel>
#include "publictype.h"
#include "virtualledtext.h"

class VirtualLED: public QWidget
{
    Q_OBJECT
public:
    explicit VirtualLED(QWidget *parent = nullptr);

public:
    int getDecoderIndex();
    void setDecoderIndex(int dwDecoderIndex);

    int getTVWallID();
    void setTVWallID(int dwTVWallID);
    void setVirtualLEDInfo(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo);
    NETDEV_XW_VIRTUAL_LED_INFO_S getVirtualLEDInfo();
    void clearSelectedStatus();/* 取消当前窗口的选中状态 */
    void setClientFontSize(UINT32 udwClientFontSize, UINT32 udwClientFontGap);/* 设置客户端界面显示字体大小、间距 */

private:
    bool deleteVirtualLED();/* 关闭虚拟LED */

private slots:
    void onCloseWindowSlot();/* 关闭窗口 */
    void onLEDTextSelectedSlot();/* LED Label被选中 */
    void onModifyVirtualLEDSlot(); /* 修改虚拟LED */

signals:
    void selectedSignal(UINT32 udwVirtualID);/* 窗口被选中时发送信号 */
    void modifyVirtualLEDSignal(NETDEV_XW_VIRTUAL_LED_INFO_S m_stVirtualLEDInfo);/* 修改虚拟LED信号 */

private:
    QWidget* m_titleBarWIdget;/*标题栏*/
    VirtualLEDText* m_pLEDText;

    int m_dwTVWallID;
    int m_dwDecoderIndex;
    UINT32 m_udwClientFontSize;/* 客户端界面显示字体大小 */
    UINT32 m_udwClientFontGap;/* 客户端界面显示字间距 */
    NETDEV_XW_VIRTUAL_LED_INFO_S m_stVirtualLEDInfo;
};

#endif // VIRTUALLED_H
