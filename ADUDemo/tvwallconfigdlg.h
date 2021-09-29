#ifndef TVWALLCONFIGDLG_H
#define TVWALLCONFIGDLG_H

#include <QDialog>
#include "publictype.h"
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include "mtreewidget.h"
#include "mtablewidget.h"

class TVWallConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TVWallConfigDlg(QWidget *parent = nullptr);
    ~TVWallConfigDlg();

public:
    void init();
    void TVWallModifyInfo(int dwDecoderIndex, NETDEV_XW_TVWALL_CFG_S stTVWallCfg);/* 电视墙配置修改信息 */

private:
    void switchCurrentDecoder(int dwDecoderIndex);

private slots:
    void on_SetTVWallFormatBtn_clicked();
    void okSlot();
    void cancelSlot();
    void on_decoderListCmb_currentIndexChanged(int index);
    void onTVWallVideoChlTabMenuSlot(const QPoint&);
    void onDelVideoOutChlBindSlot();

signals:
    void updateTVWallSignal(int dwDecoderIndex, bool bIsAddTVWall, NETDEV_XW_TVWALL_CFG_S stTVWallCfg);/* 发送电视墙变更成功信号 */

private:
    bool m_bIsAddTVWall;
    NETDEV_XW_TVWALL_CFG_S m_stTVWallCfg;

    QLineEdit* m_TVWallNameLineEdt;
    QLineEdit* m_TVWallRowCountLineEdt;
    QLineEdit* m_TVWallColumnCountLineEdt;
    QPushButton* m_SetTVWallFormatBtn;
    QComboBox* m_resolutionListCmb;
    QComboBox* m_decoderListCmb;
    MTreeWidget* m_decoderOutVideoChlTreeWgt;
    MTableWidget* m_TVWallVideoChannelTabWgt;
    QPushButton* m_OkBtn;
    QPushButton* m_cancelBtn;
};

#endif // TVWALLCONFIGDLG_H
