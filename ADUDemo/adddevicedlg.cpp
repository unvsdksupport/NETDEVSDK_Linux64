#include "adddevicedlg.h"
#include "ui_adddevicedlg.h"

QList<NETDEMO_DECODER_INFO_S> gDecoderList;
QList<NETDEMO_ENCODER_INFO_S> gEncoderList;

AddDeviceDlg::AddDeviceDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDeviceDlg)
{
    ui->setupUi(this);

    m_eDeviceType = NETDEMO_DEVICE_TYPE_DECODER;
    m_eVideoSourceAddType = NETDEMO_VIDEO_SOURCE_ADD_SDK;
    //setWindowFlags( windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
}

AddDeviceDlg::~AddDeviceDlg()
{
    delete ui;
}

void AddDeviceDlg::setDeviceType(NETDEMO_DEVICE_TYPE_E eDeviceType, NETDEMO_VIDEO_SOURCE_ADD_TYPE_E eAddType)
{
    m_eDeviceType = eDeviceType;
    m_eVideoSourceAddType = eAddType;

    if(NETDEMO_DEVICE_TYPE_DECODER == eDeviceType ||
            NETDEMO_VIDEO_SOURCE_ADD_SDK == eAddType)/* SDK */
    {
        ui->rtspLab->setHidden(true);
       ui->rtspAddressLineEdt->setHidden(true);
       ui->ipLab->setHidden(false);
       ui->IPAddressLineEdt->setHidden(false);
       ui->portLab->setHidden(false);
       ui->portLineEdit->setHidden(false);
       ui->authLab->setHidden(true);
       ui->authOnRadBtn->setHidden(true);
       ui->authOffRadBtn->setHidden(true);
    }
    else if(NETDEMO_VIDEO_SOURCE_ADD_MANUAL == eAddType)/* Manual */
    {
        ui->rtspLab->setHidden(false);
        ui->rtspAddressLineEdt->setHidden(false);
        ui->ipLab->setHidden(true);
        ui->IPAddressLineEdt->setHidden(true);
        ui->portLab->setHidden(true);
        ui->portLineEdit->setHidden(true);
        ui->authLab->setHidden(false);
        ui->authOnRadBtn->setHidden(false);
        ui->authOffRadBtn->setHidden(false);
    }
}

void AddDeviceDlg::on_buttonBox_accepted()
{
    NETDEMO_DEVICE_LOGIN_INFO_S stDeviceLoginInfo;
    stDeviceLoginInfo.eDeviceType = m_eDeviceType;
    stDeviceLoginInfo.eVideoSourceAddType = m_eVideoSourceAddType;
    stDeviceLoginInfo.strIpAddress = ui->IPAddressLineEdt->text();
    stDeviceLoginInfo.dwPort = ui->portLineEdit->text().toShort();
    stDeviceLoginInfo.strUserName = ui->usernameLineEdit->text();
    stDeviceLoginInfo.strPassword = ui->PwdLineEdit->text();
    stDeviceLoginInfo.strRtspAddress = ui->rtspAddressLineEdt->text();
    stDeviceLoginInfo.bIsAuth = ui->authOnRadBtn->isChecked();

    qDebug() << stDeviceLoginInfo.bIsAuth << endl;

    emit addDeviceSignal(stDeviceLoginInfo);
}

void AddDeviceDlg::on_buttonBox_rejected()
{

}
