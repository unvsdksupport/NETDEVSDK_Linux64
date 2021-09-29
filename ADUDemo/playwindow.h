#ifndef PlayWindow_H
#define PlayWindow_H

#include <QWidget>
#include "publictype.h"

class QGridLayout;

class PlayWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PlayWindow(QWidget *parent = nullptr);

signals:
    void selectedSignal(UINT32 udwWndID);/* 窗口被选中时发送信号 */

private slots:
    void onPlayScreenSelectedSlot(int dwPannelID);
    void onCloseWindowSlot();

    void onPlayScreenDoubleClickSlot(int dwPannelID);

public:
    int getDecoderIndex();
    void setDecoderIndex(int dwDecoderIndex);
    int getTVWallID();
    void setTVWallID(int dwTVWallID);
    void setWndInfo(NETDEV_XW_SCENE_WND_INFO_S stSceneWndInfo);
    NETDEV_XW_SCENE_WND_INFO_S getWndInfo();
    void appendStreamInfo(NETDEV_XW_STREAM_INFO_S stXWStreamInfo);/* 添加窗口关联的业务流信息 */
    void clearStreamInfo();/* 清理窗口关联业务流信息 */
    bool deleteWnd();/* 删除电视墙窗口 */
    void clearSelectedStatus();/* 取消当前窗口的选中状态 */
    bool modifyPlayScreenZoom(NETDEV_XW_SCREEN_ZOOM_INFO_S stScreenZoomInfo);/* 修改播放分屏放大状态 */

private:
    void setSplitScreenNumber(int dwSplitScreenNumber);

private:
    QGridLayout* m_playLayout;

private:
    int m_dwTVWallID;
    int m_dwDecoderIndex;
    NETDEV_XW_SCENE_WND_INFO_S m_stSceneWndInfo;
    QList<NETDEV_XW_STREAM_INFO_S> m_streamInfoList;/* 业务流信息 */
    QWidget* m_titleBarWIdget;/*标题栏*/
    int m_dwCurrentSelectedScreenID;/* 当前选中的分屏序号 */
    bool m_bIsZoomOn; /* 分屏处于放大状态 */
};

#endif // PlayWindow_H
