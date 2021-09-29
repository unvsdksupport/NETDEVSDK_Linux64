#ifndef ADUDEMO_H
#define ADUDEMO_H

#include <QWidget>
#include <QMouseEvent>
#include <QToolButton>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>

#include "tvwallconfigdlg.h"
#include "adddevicedlg.h"
#include "mtreewidget.h"
#include "mplaywidget.h"

class QListWidgetItem;

class AduDemo : public QWidget
{
    Q_OBJECT

public:
    explicit AduDemo(QWidget *parent = nullptr);
    ~AduDemo();

public:
    QPoint move_point;//移动的距离
    bool mouse_press;//鼠标按下

protected:
    virtual void mousePressEvent(QMouseEvent *event);//鼠标按下事件
    virtual void mouseReleaseEvent(QMouseEvent *event);//鼠标释放事件
    virtual void mouseMoveEvent(QMouseEvent *event);//鼠标移动事件

    virtual void closeEvent(QCloseEvent* e);

private:
    void AddEncoder(LPVOID lpLoginHandle, NETDEMO_DEVICE_LOGIN_INFO_S stDeviceInfo);
    void AddDecoder(LPVOID lpLoginHandle, NETDEMO_DEVICE_LOGIN_INFO_S stDeviceInfo);
    LPNETDEV_XW_TVWALL_CFG_S getTVWallInfo(int dwDecoderIndex, UINT32 udwTVWallID);
    void removeTVWallInfo(int dwDecoderIndex, UINT32 udwTVWallID);
    void switchShowTVWall(int dwTVWallOptionListCurRow);

private slots:
    void slotClickedTvWallOptionListWgtItem(QListWidgetItem* item);
    void on_AddDecoderBtn_clicked();
    void on_delDecoderBtn_clicked();/* 删除解码器 */
    void on_AddVideoSourceBtn_clicked();
    void updateTVWallSlot(int dwDecoderIndex, bool bIsAddTVWall, NETDEV_XW_TVWALL_CFG_S stTVWallCfg);
    void onTVWallOptionListContextMenuSlot(const QPoint&);
    void onModifyTVWallConfigSlot();
    void onDelTVWallConfigSlot();
    void onSplitScreenCmbCurIndexChangedSlot(int SplitIndex);
    void onWndVirtualLEDTabWgtCurChangedSlot(int dwCurrentIndex);

public slots:
    void AddDeviceSlot(NETDEMO_DEVICE_LOGIN_INFO_S stDeviceLoginInfo);

private:
    TVWallConfigDlg* m_pTVWallConfigDlg;
    AddDeviceDlg* m_pAddDeviceDlg;
    MTreeWidget* m_videoSourceTreeWgt;
    QListWidget* m_TVWallOptionListWgt;
    MPlayWidget* m_PlayPannelWgt;
    QPushButton* m_AddDecoderBtn;
    QPushButton* m_delDecoderBtn;
    QPushButton* m_AddVideoSourceBtn;
    QComboBox* m_decoderListCmb;
    QComboBox* m_videoSourceAddTypeCmb;
    QTabWidget* m_pWndVirtualLEDTabWgt;

};

#endif // ADUDEMO_H
