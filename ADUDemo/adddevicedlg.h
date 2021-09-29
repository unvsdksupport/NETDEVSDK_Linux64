#ifndef ADDDEVICEDLG_H
#define ADDDEVICEDLG_H

#include <QDialog>

#include "publictype.h"

namespace Ui {
class AddDeviceDlg;
}

class AddDeviceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddDeviceDlg(QWidget *parent = nullptr);
    ~AddDeviceDlg();

private:
    Ui::AddDeviceDlg *ui;

public:
    void setDeviceType(NETDEMO_DEVICE_TYPE_E eDeviceType,NETDEMO_VIDEO_SOURCE_ADD_TYPE_E eAddType = NETDEMO_VIDEO_SOURCE_ADD_SDK);

signals:
    void addDeviceSignal(NETDEMO_DEVICE_LOGIN_INFO_S stDeviceLoginInfo);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    NETDEMO_DEVICE_TYPE_E m_eDeviceType;
    NETDEMO_VIDEO_SOURCE_ADD_TYPE_E m_eVideoSourceAddType;
};

#endif // ADDDEVICEDLG_H
