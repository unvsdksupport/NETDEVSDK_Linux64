#ifndef PlayScreen_H
#define PlayScreen_H
#include "publictype.h"
#include <QLabel>

class PlayScreen: public QLabel
{
    Q_OBJECT
public:
    explicit PlayScreen(QWidget *parent = nullptr);
    ~PlayScreen();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void selectedSignal(int dwPannelID);/*播放分屏被选中*/
    void mouseDoubleClickSignal(int dwPannelID);/* 双击事件 */

private slots:
    void onContextMenuSlot(const QPoint& pt);/* 右键菜单 */

    void onChangeDecodeSlot();/*切换主被动解码方式*/
    void onStartPreviewSlot();/*关闭预览*/
    void onStopPreviewSlot();/* 关闭预览 */
    void onStopDecodeSlot();/* 停止解码 */

public:
    void setDecoderIndex(int dwDecoderIndex);
    void setDisplayID(NETDEV_XW_DISPLAYER_ID_S stDisplayID);
    NETDEV_XW_DISPLAYER_ID_S getDisplayID();
    void setStreamInfo(NETDEV_XW_STREAM_INFO_S streamInfo);
    void clearSelectedStatus();/* 取消当前窗口的选中状态 */

private:
    void startLiveBind();

    void activeDecode(bool bIsStart);/* 主动解码 */
    void passiveDecode(bool bIsStart);/* 被动解码 */

private:
    NETDEV_XW_DISPLAYER_ID_S m_stDisplayID;/* 解码器设备ID(电视墙ID、窗口ID、分屏序号) */
    int m_dwDecoderIndex; /* 解码器设备索引 */
    int m_dwEncoderIndex; /* 视频源设备索引 */
    int m_dwVideoSourceChanID;/* 视频源通道ID,ID为0表示设备是手动配置的 */
    NETDEV_XW_STREAM_INFO_S m_streamInfo;/* 业务流信息 */
    bool m_bIsLiveBind; /* 当前分屏是否已bind业务流 */
    LPVOID m_lpPreviewHandle; /* 预览句柄 */
    bool m_bIsActiveDecode; /* 是否为主动解码 */
    NETDEMO_PASSIVE_TASKNO_INFO_S m_stPassiveTaskNoInfo;/*被动解码任务号*/
};

#endif // PlayScreen_H
