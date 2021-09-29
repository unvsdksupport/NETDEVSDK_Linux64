#ifndef MPLAYWIDGET_H
#define MPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMdiArea>
#include "publictype.h"
#include "playwindow.h"
#include "virtualled.h"
#include "virtualledconfigdlg.h"

class MPlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MPlayWidget(QWidget *parent = nullptr);

protected:
    //void dragEnterEvent(QDragEnterEvent *event) override;
    //void dragLeaveEvent(QDragLeaveEvent *event) override;
    //void dragMoveEvent(QDragMoveEvent *event) override;
    //void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public:
    void setDecoderIndex(int dwDecoderIndex);/*设置解码器设备索引*/
    void clearTVWallDisplayPage();/* 清理现有电视墙显示单元及窗口 */
    void setWindowSplitScreen(int dwSplitScreenNumber);/* 设置窗口分屏 */
    void setTVWallInfo(NETDEV_XW_TVWALL_CFG_S stTVWallCfg);/*设置电视墙信息*/
    void deleteAllWndInfo();/* 删除电视墙所有窗口 */
    void deleteAllVirtualLEDInfo();/* 删除电视墙所有虚拟LED */
    void setShowType(NETDEMO_TVWALL_SHOW_TYPE_E eShowType);/* 电视墙展示窗口还是展示虚拟LED */


private:
    void refreshTvWallDisplayUnitSize();
    void refreshWndOrVirtualLEDSize();

    void createWnd(NETDEV_XW_SCENE_WND_INFO_S stWndInfo);/* 开窗 */
    bool modifyWndInfo(NETDEV_XW_SCENE_WND_INFO_S stWndInfo);/* 修改窗口信息 */
    bool getWndInfo(LPNETDEV_XW_SCENE_WND_INFO_S pstWndInfo);/* 获取窗口信息 */
    void createVirtualLED(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo);/* 创建虚拟LED */
    bool getVirtualLEDInfo(LPNETDEV_XW_VIRTUAL_LED_INFO_S pVirtualLEDInfo);/* 获取虚拟LED信息 */
    bool modifyVirtualLEDInfo(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo);/* 修改虚拟LED信息 */
    void convertScreen2DeviceTVwallPos(NETDEV_XW_AREA_S &stArea);/* 转换客户端界面屏幕坐标到电视墙坐标 */
    void convertDeviceTVwall2ScreenPos(NETDEV_XW_AREA_S &stArea);/* 转换电视墙坐标到客户端界面屏幕坐标 */
    void convertFontSize2Device(const NETDEV_XW_AREA_S stArea, UINT32 &dwFontSize);/* 转换设备上虚拟LED字体大小 0-10080 - 屏幕分辨率 */
    void convertFontSize2Client(UINT32 &dwFontSize);/* 转换客户端界面上虚拟LED字体大小 (设备电视墙显示屏总宽高(分辨率)) - (客户端界面电视墙总宽高) */
    void convertFontGap2Device(const NETDEV_XW_AREA_S stArea, UINT32 dwFontSize, int dwCount, UINT32 &dwFontGap);/* 转换设备上虚拟LED字间距 10080*n - 屏幕分辨率*n */
    void convertFontGap2Client(UINT32 dwClientFontSize, UINT32 &dwFontGap);/* 转换客户端界面上虚拟LED字间距 */
    bool mousePointhasValid(QPoint pos);/* 判断鼠标位置是否有效 */

    void showAllTVWallWnd(UINT32 udwTVWallID);/* 获取并显示电视墙窗口信息 */
    void getStreamInfo(UINT32 udwTVWallID);/* 获取电视墙关联业务流信息 */
    void showAllTVWallVirtualLED(UINT32 udwTVWallID);/* 获取并显示电视墙虚拟LED信息 */

signals:

public slots:

private slots:
    void playWindowSelectChangedSlot(UINT32 udwWndID);/*当前选中窗口变更*/
    void virtualLEDSelectChangedSlot(UINT32 udwLEDID);/* 当前选中虚拟LED变更 */
    void virtualLEDInfoChangedSlot(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo, bool bIsAdd);/* 虚拟LED信息变更 */
    void onModifyVirtualLEDSlot(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo);/* 修改虚拟LED信息 */

private:
    int m_dwVideoOutFormatWidth; /* 视频输出制式分辨率-宽 */
    int m_dwVideoOutFormatHeight; /* 视频输出制式分辨率-高 */
    int m_dwTVWallWidgetWidth = 0; /* 电视墙控件宽度 */
    int m_dwTVWallWidgetHeight = 0;/* 电视墙控件高度 */
    int m_dwLastTVWallWidgetWidth = 0; /* 上一次电视墙控件宽度 */
    int m_dwLastTVWallWidgetHeight = 0;/* 上一次电视墙控件高度 */
    int m_dwDecoderIndex; /* 解码器索引 */
    int m_dwMaxWndIndex; /* 电视墙下窗口当前最大索引 */
    UINT32 m_udwCurrentSelectedWndID;/* 当前选中的窗口ID */
    UINT32 m_udwCurrentSelectedLEDID;/* 当前选中的虚拟LED ID */
    NETDEV_XW_TVWALL_CFG_S m_stTVWallCfg;
    QPoint m_startPoint; /* 鼠标右键按下位置 */
    QPoint m_endPoint;  /* 鼠标右键释放位置 */
    bool m_bIsRightMousePressMove; /* 鼠标右键按下移动标志 */
    bool m_bIsLeftMousePressMove; /* 鼠标左键按下移动标志 */
    QList<NETDEMO_TVWALL_DISPLAY_UNIT_INFO_S> m_TVWallDisplayUnitList;/* 电视墙显示单元信息 */
    QList<NETDEV_XW_STREAM_INFO_S> m_streamInfoList;/* 业务流信息 */
    NETDEMO_TVWALL_SHOW_TYPE_E m_eShowType;/* 电视墙展示窗口还是展示虚拟LED */
    VirtualLEDConfigDlg* m_pVirtualLEDCfgDlg;/* 虚拟LED配置信息框 */
};

#endif // MPLAYWIDGET_H
