#ifndef VIRTUALLEDCONFIGDLG_H
#define VIRTUALLEDCONFIGDLG_H

#include <QDialog>
#include "publictype.h"

namespace Ui {
class VirtualLEDConfigDlg;
}

class VirtualLEDConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit VirtualLEDConfigDlg(QWidget *parent = nullptr);
    ~VirtualLEDConfigDlg();

signals:
    void virtualLEDChangedSignal(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo, bool bIsAdd);

public:
    void setVirtualLEDInfo(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo, bool bIsAdd = true);

private slots:
    void on_fontColorBtn_clicked();

    void on_backgroundColorBtn_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_scrollRateSlider_valueChanged(int value);

    void on_fontSizeSlider_valueChanged(int value);

private:
    Ui::VirtualLEDConfigDlg *ui;

    bool m_bIsAddLED;
    NETDEV_XW_VIRTUAL_LED_INFO_S m_stVirtualLEDInfo;
};

#endif // VIRTUALLEDCONFIGDLG_H
