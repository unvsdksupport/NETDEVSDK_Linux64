#include"mainwindow.h"
#include"ui_mainwindow.h"
#include"NetDEVSDK.h"
#include<QMessageBox>
#include<QByteArray>
#include<QDebug>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include <QTextStream>
#include"NetDEVSDK.h"


#include <QFile>
#include <QTextStream>

QTextStream cout(stdout, QIODevice::WriteOnly);
QString strout;
QString strDeviceList;

void STDCALL FaceCallBack_PF(IN LPVOID lpUserID,IN LPNETDEV_TMS_FACE_SNAPSHOT_PIC_INFO_S pstFaceSnapShotData,IN LPVOID lpUserData )
{

    QString filename;
    filename.sprintf("%s_%s.jpg",pstFaceSnapShotData->szRecordID,pstFaceSnapShotData->szPassTime);

    QFile file(filename);

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
		
    }
    file.write(pstFaceSnapShotData->pcPicBuff, pstFaceSnapShotData->udwPicBuffLen);
	file.close();
    return;
}

void STDCALL PassengerFlowStatisticCallBack_PF(IN LPVOID lpUserID,
                                               IN LPNETDEV_PASSENGER_FLOW_STATISTIC_DATA_S pstPassengerFlowData,
                                               IN LPVOID lpUserData)
{
    qDebug() << "ChlID:" << pstPassengerFlowData->dwChannelID << "enterNum:" << pstPassengerFlowData->dwEnterNum
             << "ExitNum:" << pstPassengerFlowData->dwExitNum << "TEnterNum:" << pstPassengerFlowData->dwTotalEnterNum
             << "TExitNum:" << pstPassengerFlowData->dwTotalExitNum << "Interval: " << pstPassengerFlowData->tInterval
             << "Time:" << pstPassengerFlowData->tReportTime << endl;
}

void STDCALL AlarmCallBack_PF(IN LPVOID  dwUserID,
                              IN INT32 dwChannelID,
                              IN NETDEV_ALARM_INFO_S stAlarmInfo,
                              IN LPVOID lpBuf,
                              IN INT32  dwBufLen,
                              IN LPVOID dwUserData
                              )
{

    QString strtemp;
    if (NETDEV_ALARM_DISK_ABNORMAL<= stAlarmInfo.dwAlarmType && NETDEV_ALARM_DISK_STORAGE_IS_FULL >= stAlarmInfo.dwAlarmType)
    {
        strtemp.sprintf("Alarm Disk SlotIndex:%02d  ",stAlarmInfo.wIndex);
    }
    else if (NETDEV_ALARM_DISK_RAID_DISABLED<= stAlarmInfo.dwAlarmType && NETDEV_ALARM_DISK_RAID_RECOVERED >= stAlarmInfo.dwAlarmType)
    {
        strtemp.sprintf("Alarm RAIDIndex:%02d  ",stAlarmInfo.wIndex);
    }
    else
    {
        strtemp.sprintf("Alarm ChannelID:%02d  ",stAlarmInfo.dwChannelID);
    }
    strout += strtemp;

    if (NETDEV_ALARM_INPUT_SWITCH == stAlarmInfo.dwAlarmType)
    {
        strtemp.sprintf("Alarm source name: %s", stAlarmInfo.pszName);
        strout += strtemp;
    }

    strtemp.sprintf("Alarm time:%d  ",stAlarmInfo.tAlarmTime);
    tm *pTime = localtime((time_t*)&stAlarmInfo.tAlarmTime);
    strtemp.sprintf("%d/%02d/%02d %02d:%02d:%02d  ",pTime->tm_year + 1900,pTime->tm_mon+1,pTime->tm_mday,pTime->tm_hour,pTime->tm_min,pTime->tm_sec);
    strout += strtemp;

    switch(stAlarmInfo.dwAlarmType)
    {
    case NETDEV_ALARM_MOVE_DETECT:
        strout +="Motion detection alarm";
        break;
    case NETDEV_ALARM_MOVE_DETECT_RECOVER:
        strout +="Motion detection alarm recover";
        break;
    case NETDEV_ALARM_VIDEO_TAMPER_DETECT:
        strout +="Tampering detection alarm";
        break;
    case NETDEV_ALARM_VIDEO_TAMPER_RECOVER:
        strout +="Tampering detection alarm recover";
        break;
    case NETDEV_ALARM_INPUT_SWITCH:
        strout +="Boolean input alarm";
        break;
    case NETDEV_ALARM_INPUT_SWITCH_RECOVER:
        strout +="Boolean input alarm recover";
        break;
    case NETDEV_ALARM_VIDEO_LOST:
        strout +="Video loss alarm";
        break;
    case NETDEV_ALARM_VIDEO_LOST_RECOVER:
        strout +="Video loss alarm recover";
        break;
    case NETDEV_ALARM_REPORT_DEV_ONLINE:
        strout +="Device online";
        break;
    case NETDEV_ALARM_REPORT_DEV_OFFLINE:
        strout +="Device offline";
        break;
    case NETDEV_ALARM_REPORT_DEV_DELETE_CHL:
        strout +="Delete channel";
        break;
    case NETDEV_ALARM_DISK_OFFLINE:
        strout +="Disk offline";
        break;
    case NETDEV_ALARM_DISK_ONLINE:
        strout +="Disk online";
        break;
    case NETDEV_ALARM_DISK_ABNORMAL:
        strout +="Disk abnormal";
        break;
    case NETDEV_ALARM_DISK_ABNORMAL_RECOVER:
        strout +="Disk abnormal recover";
        break;
    case NETDEV_ALARM_DISK_STORAGE_WILL_FULL:
        strout +="Disk storage will full";
        break;
    case NETDEV_ALARM_DISK_STORAGE_WILL_FULL_RECOVER:
        strout +="Disk storage will full recover";
        break;
    case NETDEV_ALARM_DISK_STORAGE_IS_FULL:
        strout +="Disk storage is full";
        break;
    case NETDEV_ALARM_DISK_STORAGE_IS_FULL_RECOVER:
        strout +="Disk storage is full recover";
        break;
    case NETDEV_ALARM_DISK_RAID_DISABLED:
        strout +="RAID disabled";
        break;
    case NETDEV_ALARM_DISK_RAID_DISABLED_RECOVER:
        strout +="RAID disabled recover";
        break;
    case NETDEV_ALARM_DISK_RAID_DEGRADED:
        strout +="RAID degraded";
        break;
    case NETDEV_ALARM_DISK_RAID_DEGRADED_RECOVER:
        strout +="RAID degraded recover";
        break;
    case NETDEV_ALARM_DISK_RAID_RECOVERED:
        strout +="RAID recovered";
        break;    
    default:
        QString strAlarmType;
        strAlarmType.sprintf("AlarmType:%02d", stAlarmInfo.dwAlarmType);
        strout += strAlarmType;
        break;
    }	
    strout += "\n";
}

void STDCALL NETDEV_DISCOVERY_CB(IN LPNETDEV_DISCOVERY_DEVINFO_S pstDevInfo, void* lParam)
{
    strDeviceList += pstDevInfo->szDevAddr;
    strDeviceList += "\n";
    return;
}

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
    m_lpDevHandle = NULL ;
    m_lpPlayHandle = NULL ;
    m_lpDownloadHandle = NULL;
    m_DlgDownlaodInfo = new DlgDownloadInfo();
    m_timer = new QTimer(this);
    m_lastDownLoadTime = 0;
    m_SameTimeCount = 0;
    m_LoginDeviceType = 0;

    connect(m_timer,SIGNAL(timeout()),this, SLOT(updateDownLoadProcess()));
    ui->setupUi(this) ;
    m_ulSession = 0;

    //ui->Login->setStyleSheet("background-color:rgb(255,255,255);") ;
    //ui->Logout->setStyleSheet("background-color:rgb(255,255,255);") ;

    ui->LIVE->setStyleSheet("background-color:rgb(200,200,200);") ;
    ui->groupBox->setStyleSheet("background-color:rgb(200,200,200);") ;
    ui->PTZ->setStyleSheet("background-color:rgb(200,200,200);");

    ui->IpAddr->setStyleSheet("background-color:rgb(255,255,255);") ;
    ui->UserName->setStyleSheet("background-color:rgb(255,255,255);") ;
    ui->PassWord->setStyleSheet("background-color:rgb(255,255,255);") ;

    m_nTimerId = startTimer(500);
    Init() ;
}

MainWindow::~MainWindow()
{
    Uninit();
    if(m_nTimerId != 0)
    {
        killTimer(m_nTimerId);
    }
    if(NULL != ui)
    {
        delete ui;
        ui = NULL;
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if("" != strout)
    {
        ui->AlarmInfo->textCursor().insertText(strout);
        strout = "";
    }

    if("" != strDeviceList)
    {
        ui->DeviceList->textCursor().insertText(strDeviceList);
        strDeviceList = "";
    }
}



BOOL MainWindow::Init()
{
    BOOL bRet = NETDEV_Init() ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("HELLO") ;
    }

    m_ChannelID = 1 ;
    m_lpPlayHandle = NULL;
    m_lpDevHandle = NULL;
    return TRUE ;
}

void MainWindow::on_Login_clicked()
{
    NETDEV_DEVICE_INFO_S stDevInfo = {0} ;
    memset(&stDevInfo, 0, sizeof(NETDEV_DEVICE_INFO_S)) ;
    m_strDevIP   = ui->IpAddr->text() ;
    m_strUser    = ui->UserName->text() ;
    m_strPwd     = ui->PassWord->text() ;

    m_LoginDeviceType = ui->loginDeviceTypeCmb->currentIndex();

    if(NULL != m_lpDevHandle)
    {
        NETDEV_Logout(m_lpDevHandle) ;
    }

    QByteArray mid_var_Ip   = m_strDevIP.toLatin1() ;
    CHAR* szDevIP = mid_var_Ip.data() ;
    QByteArray mid_var_User = m_strUser.toLatin1() ;
    CHAR* szUser = mid_var_User.data() ;
    QByteArray mid_var_Pwd  = m_strPwd.toLatin1() ;
    CHAR* szPwd   = mid_var_Pwd.data() ;

    NETDEV_DEVICE_LOGIN_INFO_S stDevLoginInfo = {0};
    NETDEV_SELOG_INFO_S stSELogInfo = {0};
    strncpy(stDevLoginInfo.szIPAddr, szDevIP, sizeof(stDevLoginInfo.szIPAddr) - 1);
    strncpy(stDevLoginInfo.szUserName, szUser, sizeof(stDevLoginInfo.szUserName) - 1);
    strncpy(stDevLoginInfo.szPassword, mid_var_Pwd, sizeof(stDevLoginInfo.szPassword) - 1);
    stDevLoginInfo.dwPort = 80;
    /*接入协议*/
    if(0 == m_LoginDeviceType) /* IPC/NVR */
    {
        stDevLoginInfo.dwLoginProto = NETDEV_LOGIN_PROTO_ONVIF;
    }
    else /* VMS */
    {
        stDevLoginInfo.dwLoginProto = NETDEV_LOGIN_PROTO_PRIVATE;
    }
    m_lpDevHandle = NETDEV_Login_V30(&stDevLoginInfo, &stSELogInfo) ;
    cout << szUser << szPwd <<endl;//
    if(NULL == m_lpDevHandle)
    {
        QMessageBox *msg_box = new QMessageBox();
        int iRet = NETDEV_GetLastError();
        QString strError = QString("%1").arg(iRet);
        msg_box->setText("login failed" + strError);
        msg_box->show() ;
        return ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox();
        msg_box->setText("Login succeed");
        msg_box->show() ;
    }


    NETDEV_SetAlarmCallBack(m_lpDevHandle, AlarmCallBack_PF, (LPVOID)this);
    NETDEV_SetFaceSnapshotCallBack(m_lpDevHandle, FaceCallBack_PF,(LPVOID)this);
    NETDEV_SetPassengerFlowStatisticCallBack(m_lpDevHandle, PassengerFlowStatisticCallBack_PF,(LPVOID)this);

    if(0 == m_LoginDeviceType) /* IPC/NVR */
    {
        INT32 dwCount = 64 ;
        NETDEV_VIDEO_CHL_DETAIL_INFO_S pstVideioChlList[64] = {0} ;
        BOOL bRet = NETDEV_QueryVideoChlDetailList(m_lpDevHandle,&dwCount,pstVideioChlList) ;
        if(TRUE == bRet)
        {
            ui->ChannelList->clear() ;
            for(INT32 i = 0; i < dwCount; i++)
            {
                QString strtemp("Channel %1") ;
                QString strout = strtemp.arg(i+1) ;
                ui->ChannelList->addItem(strout) ;
            }
        }
    }
    else /* VMS */
    {
        LPVOID lpDevFindHandle = NETDEV_FindDevList(m_lpDevHandle, NETDEV_DTYPE_MAIN_ENCODE);
        if(NULL == lpDevFindHandle)
        {
            QMessageBox *msg_box = new QMessageBox();
            msg_box->setText("Find device list fail");
            msg_box->show() ;
            return;
        }

        ui->ChannelList->clear();

        NETDEV_DEV_BASIC_INFO_S stDevInfo = {0};
        while(TRUE == NETDEV_FindNextDevInfo(lpDevFindHandle, &stDevInfo))
        {
            LPVOID  lpChnFindHandle = NETDEV_FindDevChnList(m_lpDevHandle, stDevInfo.dwDevID, NETDEV_CHN_TYPE_ENCODE);
            if(NULL == lpChnFindHandle)
            {
                QMessageBox *msg_box = new QMessageBox();
                msg_box->setText("Find device channel list fail");
                msg_box->show();
                continue;
            }
            else
            {
                NETDEV_DEV_CHN_ENCODE_INFO_S stDevChnEncodeInfo = {0};
                INT32 dwBytesReturned;
                while(TRUE == NETDEV_FindNextDevChn(lpChnFindHandle, &stDevChnEncodeInfo,sizeof(NETDEV_DEV_CHN_ENCODE_INFO_S),&dwBytesReturned))
                {
                    QString strtemp("%1_Channel_%2");
                    QString strout = strtemp.arg(stDevInfo.szDevName).arg(stDevChnEncodeInfo.stChnBaseInfo.dwChannelID) ;
                    ui->ChannelList->addItem(strout) ;
                }
                NETDEV_FindCloseDevChn(lpChnFindHandle);
            }
        }
        NETDEV_FindCloseDevInfo(lpDevFindHandle);
    }

    ui->ChannelList->setCurrentIndex(0);
    if(0 == m_LoginDeviceType)
    {
        m_ChannelID = ui->ChannelList->currentIndex() + 1;
    }
    else
    {
        m_ChannelID = ui->ChannelList->currentText().mid(ui->ChannelList->currentText().lastIndexOf("_") + 1).toInt();
    }
    return ;

}

void MainWindow::on_Logout_clicked()
{
    if(NULL == m_lpDevHandle)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("DEVICE DOESNOT EXIST") ;
        msg_box->show() ;
        return ;
    }

    BOOL bRet = NETDEV_Logout(m_lpDevHandle) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Logout failed") ;
        msg_box->show() ;
        return ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Logout succeeded") ;
        msg_box->show() ;
    }
    ui->ChannelList->clear() ;
    return ;
}

void MainWindow::on_GET_STREAM_clicked()
{
    NETDEV_VIDEO_STREAM_INFO_S stStreamInfo ;
    memset(&stStreamInfo,0,sizeof(NETDEV_VIDEO_STREAM_INFO_S)) ;
    stStreamInfo.enStreamType = NETDEV_LIVE_STREAM_INDEX_MAIN ;

    INT32 dwBytesReturned = 0 ;

    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle,m_ChannelID,NETDEV_GET_STREAMCFG,&stStreamInfo,sizeof(NETDEV_VIDEO_STREAM_INFO_S),&dwBytesReturned) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get Video stream failed") ;
        msg_box->show() ;
    }
    else
    {
        QString str_mid("Stream Type \t %1 \n"
            "Enable \t %2 \n"
            "Bit rate \t %3 \n"
            "Frame rate \t %4 \n"
            "Resolution \t %5 * %6 \n"
            "Format \t %7 \n"
            "I Frame Interval \t %8 \n"
            ) ;
        QString str_streamInfo = str_mid.arg(NETDEV_LIVE_STREAM_INDEX_MAIN)
            .arg(stStreamInfo.bEnableFlag)
            .arg(stStreamInfo.dwBitRate)
            .arg(stStreamInfo.dwFrameRate)
            .arg(stStreamInfo.dwWidth)
            .arg(stStreamInfo.dwHeight)
            .arg(stStreamInfo.enCodeType)
            .arg(stStreamInfo.dwGop) ;
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(str_streamInfo) ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_SET_STREAM_clicked()
{
    NETDEV_VIDEO_STREAM_INFO_S stStreamInfo ;
    memset(&stStreamInfo,0,sizeof(NETDEV_VIDEO_STREAM_INFO_S)) ;
    stStreamInfo.enStreamType = NETDEV_LIVE_STREAM_INDEX_MAIN ;

    INT32 dwBytesReturned = 0 ;

    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle,m_ChannelID,NETDEV_GET_STREAMCFG,&stStreamInfo,sizeof(NETDEV_VIDEO_STREAM_INFO_S),&dwBytesReturned) ;
    if(TRUE != bRet)
    {

        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set stream failed") ;
        msg_box->show() ;
        return ;
    }

    stStreamInfo.bEnableFlag = TRUE ;
    stStreamInfo.dwFrameRate = 20 ;
    stStreamInfo.dwBitRate = 4096 ;
    stStreamInfo.dwGop = 24 ;

    bRet = NETDEV_SetDevConfig(m_lpDevHandle,m_ChannelID,NETDEV_SET_STREAMCFG,&stStreamInfo,sizeof(NETDEV_VIDEO_STREAM_INFO_S)) ;
    qDebug("%d",bRet);
    if(TRUE == bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set stream succeed") ;
        msg_box->show() ;
    }
    else
    {
        int iRet = NETDEV_GetLastError();
        QString strError = QString("%1").arg(iRet);
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set stream failed  " + strError) ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_get_imaInfo_clicked()
{
    NETDEV_IMAGE_SETTING_S stImageInfo ;
    memset(&stImageInfo,0,sizeof(NETDEV_IMAGE_SETTING_S)) ;

    INT32 dwBytesReturned ;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle,m_ChannelID,NETDEV_GET_IMAGECFG,&stImageInfo,sizeof(NETDEV_IMAGE_SETTING_S),&dwBytesReturned) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get image setting failed") ;
        msg_box->show() ;
    }
    else
    {
        QString strimaInfo ;
        QString strmidvar("Brightness \t %1\n"
            "ColorSaturation \t %2\n"
            "Contrast \t %3\n"
            "Sharpness \t %4\n");
        strimaInfo = strmidvar.arg(stImageInfo.dwBrightness)
            .arg(stImageInfo.dwSaturation)
            .arg(stImageInfo.dwContrast)
            .arg(stImageInfo.dwSharpness) ;

        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strimaInfo) ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_SET_IMAGE_INFO_clicked()
{
    NETDEV_IMAGE_SETTING_S stImageInfo ;
    memset(&stImageInfo,0,sizeof(NETDEV_IMAGE_SETTING_S)) ;

    stImageInfo.dwContrast = 100 ;
    stImageInfo.dwBrightness = 110 ;
    stImageInfo.dwSaturation = 120 ;
    stImageInfo.dwSharpness = 130 ;

    BOOL bRet = NETDEV_SetDevConfig(m_lpDevHandle,m_ChannelID,NETDEV_SET_IMAGECFG,&stImageInfo,sizeof(NETDEV_IMAGE_SETTING_S)) ;
    if(TRUE == bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set image succeed") ;
        msg_box->show() ;
        return ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set image failed") ;
        msg_box->show() ;
    }
    return ;

}

void MainWindow::on_NORTH_WEST_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_LEFTUP,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set left-up failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_UP_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_TILTUP,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set tilt-up failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_NORTH_EAST_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_RIGHTUP,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set right_up failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_LEFT_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_PANLEFT,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set pan-left failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_STOP_clicked()
{
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_ALLSTOP,0) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set all-stop failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_RIGHT_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_PANRIGHT,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set pan-right failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_SOUTH_WEST_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_LEFTDOWN,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set left-down failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_DOWN_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_TILTDOWN,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set titl-down failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_SOUT_EAST_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_RIGHTDOWN,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set right-down failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_FOCUS_IN_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_ZOOMTELE,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set zoom-tele failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_FOCUS_OUT_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_ZOOMWIDE,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set zoom-wide failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_Version_clicked()
{
    INT32 iVersion = 0 ;
    iVersion = NETDEV_GetSDKVersion() ;

    INT32 iVerMain = (iVersion & 0xFFFF0000)>>16 ;
    INT32 iVerSub  = (iVersion & 0x0000FFFF) ;

    QString str_mid("SDK Version:%1.%2\n ") ;
    QString str_verInfo = str_mid.arg(iVerMain).arg(iVerSub) ;


    QMessageBox *msg_box = new QMessageBox() ;
    msg_box->setText(str_verInfo) ;
    msg_box->show() ;
    return ;
}


void MainWindow::on_ChannelList_activated(const QString &arg1)
{
    if(0 == m_LoginDeviceType)
    {
        m_ChannelID = ui->ChannelList->currentIndex() + 1;
    }
    else
    {
        m_ChannelID = ui->ChannelList->currentText().mid(ui->ChannelList->currentText().lastIndexOf("_") + 1).toInt();
    }
}

void MainWindow::on_GetStreamURL_clicked()
{
    char szStreamUrl[512] = {0};
    BOOL bRet = NETDEV_GetStreamUrl(m_lpDevHandle, m_ChannelID, NETDEV_LIVE_STREAM_INDEX_MAIN, szStreamUrl);
    if(TRUE != bRet)
    {
        strcpy(szStreamUrl, "Get Stream Url failed");
    }
    QMessageBox *msg_box = new QMessageBox() ;
    msg_box->setText(szStreamUrl) ;
    msg_box->show() ;
    return ;
}

//调用此接口需要先查询录像，调用接口on_CommonQuery_clicked
void MainWindow::on_GetReplayURL_clicked()
{
    char szStreamUrl[512] = {0};
    BOOL bRet = NETDEV_GetReplayUrl(m_lpDevHandle, m_ChannelID, NETDEV_LIVE_STREAM_INDEX_MAIN, szStreamUrl);
    if(TRUE != bRet)
    {
        strcpy(szStreamUrl, "Get Replay URL failed");
    }
    QMessageBox *msg_box = new QMessageBox() ;
    msg_box->setText(szStreamUrl) ;
    msg_box->show() ;
    return ;
}

long MainWindow::GetTick(char *str_time)  
{
    struct tm stm;  
    int iY, iM, iD, iH, iMin, iS;  
    memset(&stm,0,sizeof(stm));  
    iM = atoi(str_time);  
    iD = atoi(str_time+3);
    iY = atoi(str_time+6);
    iH = atoi(str_time+11);  
    iMin = atoi(str_time+14);  
    iS = atoi(str_time+17);  

    stm.tm_year=iY-1900;  
    stm.tm_mon=iM-1;  
    stm.tm_mday=iD;  
    stm.tm_hour=iH;  
    stm.tm_min=iMin;  
    stm.tm_sec=iS;   

    return mktime(&stm);  
}  

void MainWindow::on_CommonQuery_clicked()
{
    NETDEV_FILECOND_S stFileCond = {0};
    stFileCond.dwChannelID = m_ChannelID;
    LPVOID dwFileHandle = 0;

    /* UTC time. The found period of time is 24 hours from current system time. */

    CHAR* szBeginTime= ui->BeginTime->text().toLatin1().data();
    stFileCond.tBeginTime = GetTick(szBeginTime);

    CHAR* szEndTime= ui->EndTime->text().toLatin1().data();
    stFileCond.tEndTime = GetTick(szEndTime);
    if(stFileCond.tBeginTime >= stFileCond.tEndTime)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("BeginTime >= EndTime") ;
        msg_box->show() ;
        return;
    }

    dwFileHandle = NETDEV_FindFile(m_lpDevHandle,&stFileCond);
    if(NULL == dwFileHandle)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("FindFile failed") ;
        msg_box->show() ;
        return;
    }
    else
    {
        NETDEV_FINDDATA_S stVodFile = {0};
        for(; ; )
        {
            BOOL bRet = NETDEV_FindNextFile(dwFileHandle, &stVodFile);
            if(TRUE != bRet)
            {
                break;
            }
            else
            {
                QString strBeginLocalTime, strEndLocalTime;
                {
                    tm *pTime = localtime((time_t*)&stVodFile.tBeginTime);
                    strBeginLocalTime.sprintf("%d/%02d/%02d %02d:%02d:%02d",pTime->tm_year + 1900,pTime->tm_mon+1,pTime->tm_mday,
                        pTime->tm_hour,pTime->tm_min,pTime->tm_sec);
                }
                {
                    tm *pTime = localtime((time_t*)&stVodFile.tEndTime);
                    strEndLocalTime.sprintf("%d/%02d/%02d %02d:%02d:%02d",pTime->tm_year + 1900,pTime->tm_mon+1,pTime->tm_mday,
                        pTime->tm_hour,pTime->tm_min,pTime->tm_sec);
                }

                QString strOutInfo;
                strOutInfo = strOutInfo + strBeginLocalTime + "---" + strEndLocalTime + "\n";
                QString strTime;
                strTime += strOutInfo;
                cout << strTime <<endl;//
            }
        }
    }
    NETDEV_FindClose(dwFileHandle);
    return;
}

void MainWindow::Uninit()
{
    NETDEV_Cleanup();
}

void MainWindow::on_StartFlow_clicked()
{
    if(NULL != m_lpPlayHandle)
    {
        NETDEV_StopRealPlay(m_lpPlayHandle);
        m_lpPlayHandle = NULL;
    }

       NETDEV_PREVIEWINFO_S stNetInfo = {0};

       stNetInfo.dwChannelID = m_ChannelID;
       stNetInfo.hPlayWnd = (void *)ui->widget->winId();
       stNetInfo.dwStreamType = NETDEV_LIVE_STREAM_INDEX_MAIN;
       stNetInfo.dwLinkMode = NETDEV_TRANSPROTOCAL_RTPTCP;
       m_lpPlayHandle = NETDEV_RealPlay(m_lpDevHandle, &stNetInfo, NULL, 0);
       if(NULL == m_lpPlayHandle)
       {
           QMessageBox *msg_box = new QMessageBox() ;
           msg_box->setText("RealPlay failed") ;
           msg_box->show() ;
       }
}

void MainWindow::on_StopFlow_clicked()
{
    if(NULL != m_lpPlayHandle)
    {
        NETDEV_StopRealPlay(m_lpPlayHandle);
        m_lpPlayHandle = NULL;
    }
}

void MainWindow::on_StartReplayFlow_clicked()
{
    /* 需要获取开始结束时间*/
    CHAR* szBeginTime= ui->BeginTime->text().toLatin1().data();
    long ulStartTime = GetTick(szBeginTime);

    CHAR* szEndTime= ui->EndTime->text().toLatin1().data();
    long ulEndTime  = GetTick(szEndTime);

    if(NULL != m_lpPlayHandle)
    {
        NETDEV_StopPlayBack(m_lpPlayHandle);
        m_lpPlayHandle = NULL;
    }

    NETDEV_PLAYBACKCOND_S stPlayBackByTimeInfo = {0};
    stPlayBackByTimeInfo.dwChannelID = m_ChannelID;
    stPlayBackByTimeInfo.hPlayWnd = (void *)ui->widget->winId();
    stPlayBackByTimeInfo.dwLinkMode = NETDEV_TRANSPROTOCAL_RTPTCP;
	stPlayBackByTimeInfo.dwPlaySpeed = NETDEV_PLAY_STATUS_1_FORWARD;
    stPlayBackByTimeInfo.tBeginTime = ulStartTime;
    stPlayBackByTimeInfo.tEndTime = ulEndTime;
    m_lpPlayHandle = NETDEV_PlayBackByTime(m_lpDevHandle, &stPlayBackByTimeInfo);
    if(NULL == m_lpPlayHandle)
    {
       QMessageBox *msg_box = new QMessageBox() ;
       msg_box->setText("PlayBack failed") ;
       msg_box->show() ;
    }
}

void MainWindow::on_StopReplayFlow_clicked()
{
    if(NULL != m_lpPlayHandle)
    {
        NETDEV_StopPlayBack(m_lpPlayHandle);
        m_lpPlayHandle = NULL;
    }
}

void MainWindow::on_FocusFar_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_FOCUSFAR,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set focus-far failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_FocusNear_clicked()
{
    INT32 dwSpeed = 5 ;
    BOOL bRet = NETDEV_PTZControl_Other(m_lpDevHandle,m_ChannelID,NETDEV_PTZ_FOCUSNEAR,dwSpeed) ;
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set focus-near failed") ;
        msg_box->show() ;
    }
    return ;
}

void MainWindow::on_setSystemTime_clicked()
{
    NETDEV_TIME_CFG_S stSystemTimeCfg;
    memset(&stSystemTimeCfg, 0, sizeof(NETDEV_TIME_CFG_S));

    BOOL bRet = NETDEV_GetSystemTimeCfg(m_lpDevHandle, &stSystemTimeCfg);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("set SystemTime NETDEV_GetSystemTimeCfg failed") ;
        msg_box->show() ;
        return;
    }

    stSystemTimeCfg.dwTimeZone = NETDEV_TIME_ZONE_E0800;
    stSystemTimeCfg.stTime.dwYear = 2016;
    stSystemTimeCfg.stTime.dwMonth = 1;
    stSystemTimeCfg.stTime.dwDay = 1;
    stSystemTimeCfg.stTime.dwHour = 0;
    stSystemTimeCfg.stTime.dwMinute = 0;
    stSystemTimeCfg.stTime.dwSecond = 0;

    bRet = NETDEV_SetSystemTimeCfg(m_lpDevHandle, &stSystemTimeCfg);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set SystemTime Cfg Failed") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set SystemTime Cfg Succeed") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetSystemTime_2_clicked()
{
    NETDEV_TIME_CFG_S stSystemTimeCfg;
    memset(&stSystemTimeCfg, 0, sizeof(NETDEV_TIME_CFG_S));

    BOOL bRet = NETDEV_GetSystemTimeCfg(m_lpDevHandle, &stSystemTimeCfg);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get SystemTime Cfg failed.") ;
        msg_box->show() ;
    }
    else
    {
        QString str_mid("time:%1/"
            "%2/"
            "%3  "
            "%4:"
            "%5:"
            "%6\n"
            ) ;
        QString strTime = str_mid.arg(stSystemTimeCfg.stTime.dwYear)
            .arg(stSystemTimeCfg.stTime.dwMonth)
            .arg(stSystemTimeCfg.stTime.dwDay)
            .arg(stSystemTimeCfg.stTime.dwHour)
            .arg(stSystemTimeCfg.stTime.dwMinute)
            .arg(stSystemTimeCfg.stTime.dwSecond);

        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strTime) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_setOSD_clicked()
{
    NETDEV_VIDEO_OSD_CFG_S stChannelOSDInfo;
    memset(&stChannelOSDInfo, 0, sizeof(NETDEV_VIDEO_OSD_CFG_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_OSDCFG, &stChannelOSDInfo, sizeof(NETDEV_VIDEO_OSD_CFG_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("setOSD NETDEV_GetDevConfig failed.") ;
        msg_box->show() ;
        return;
    }
    stChannelOSDInfo.stTimeOSD.bEnableFlag = TRUE;
    stChannelOSDInfo.stTimeOSD.stAreaScope.dwLocateX = 1000;
    stChannelOSDInfo.stTimeOSD.stAreaScope.dwLocateY = 1000;
    stChannelOSDInfo.stTimeOSD.udwTimeFormat = NETDEV_OSD_TIME_FORMAT_CAP_HHMMSS;
    /* name */
    stChannelOSDInfo.stNameOSD.bEnableFlag = TRUE;
    stChannelOSDInfo.stNameOSD.stAreaScope.dwLocateX = 2000;
    stChannelOSDInfo.stNameOSD.stAreaScope.dwLocateY = 8000;
    strncpy(stChannelOSDInfo.stNameOSD.szOSDText, "test name", NETDEV_LEN_64 - 1);
    /* text */
    stChannelOSDInfo.astTextOverlay[0].bEnableFlag = TRUE;
    stChannelOSDInfo.astTextOverlay[0].stAreaScope.dwLocateX = 5000;
    stChannelOSDInfo.astTextOverlay[0].stAreaScope.dwLocateY = 5000;
    stChannelOSDInfo.wTextNum = 1;
    strncpy(stChannelOSDInfo.astTextOverlay[0].szOSDText, "test text", NETDEV_LEN_64 - 1);
    bRet = NETDEV_SetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_SET_OSDCFG, &stChannelOSDInfo, sizeof(NETDEV_VIDEO_OSD_CFG_S));
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("setOSD  failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("setOSD  Succeed.") ;
        msg_box->show() ;
    }


    return;
}

void MainWindow::on_GetOSD_clicked()
{
    NETDEV_VIDEO_OSD_CFG_S stChannelOSDInfo;
    memset(&stChannelOSDInfo, 0, sizeof(NETDEV_VIDEO_OSD_CFG_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_OSDCFG, &stChannelOSDInfo, sizeof(NETDEV_VIDEO_OSD_CFG_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("GetOSD failed.") ;
        msg_box->show() ;
        return;
    }
    else
    {
        QString str_mid("Channel name OSD:\t %1 \n"
            "x = %2, \t y = %3 \n"
            "OSD text:\t %4 \n"
            "x = %5, \t y = %6 \n"
            ) ;
        QString strOut = str_mid.arg(stChannelOSDInfo.stNameOSD.szOSDText)
            .arg(stChannelOSDInfo.stNameOSD.stAreaScope.dwLocateX)
            .arg(stChannelOSDInfo.stNameOSD.stAreaScope.dwLocateY)
            .arg(stChannelOSDInfo.astTextOverlay[0].szOSDText)
            .arg(stChannelOSDInfo.astTextOverlay[0].stAreaScope.dwLocateX)
            .arg(stChannelOSDInfo.astTextOverlay[0].stAreaScope.dwLocateY);

        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_setPrivacyMask_clicked()
{
    NETDEV_PRIVACY_MASK_CFG_S stPrivacyMaskInfo;
    memset(&stPrivacyMaskInfo, 0, sizeof(NETDEV_PRIVACY_MASK_CFG_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_PRIVACYMASKCFG, &stPrivacyMaskInfo, sizeof(NETDEV_PRIVACY_MASK_CFG_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Privacymask Info NETDEV_GetDevConfig failed.") ;
        msg_box->show() ;
        return;
    }

    stPrivacyMaskInfo.astArea[0].bIsEanbled = TRUE;
    stPrivacyMaskInfo.astArea[0].dwBottomRightX = 6000;
    stPrivacyMaskInfo.astArea[0].dwBottomRightY = 6000;
    stPrivacyMaskInfo.astArea[0].dwTopLeftX = 2000;
    stPrivacyMaskInfo.astArea[0].dwTopLeftY = 2000;

    stPrivacyMaskInfo.astArea[1].bIsEanbled = TRUE;
    stPrivacyMaskInfo.astArea[1].dwBottomRightX = 8000;
    stPrivacyMaskInfo.astArea[1].dwBottomRightY = 8000;
    stPrivacyMaskInfo.astArea[1].dwTopLeftX = 5000;
    stPrivacyMaskInfo.astArea[1].dwTopLeftY = 5000;


    bRet = NETDEV_SetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_SET_PRIVACYMASKCFG, &stPrivacyMaskInfo, sizeof(NETDEV_PRIVACY_MASK_CFG_S));
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Privacymask Info failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Privacymask Info succeed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetPrivacyMask_clicked()
{
    NETDEV_PRIVACY_MASK_CFG_S stPrivacyMaskInfo;
    memset(&stPrivacyMaskInfo, 0, sizeof(NETDEV_PRIVACY_MASK_CFG_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_PRIVACYMASKCFG, &stPrivacyMaskInfo, sizeof(NETDEV_PRIVACY_MASK_CFG_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("GetPrivacyMask failed.") ;
        msg_box->show() ;
    }
    else
    {
        QString strOut;

        for(INT32 i = 0; i < stPrivacyMaskInfo.dwSize; i++)
        {
            QString str_mid("Area[%1]:\n"
                "TopLeftX: %2 \t TopLeftY: %3\n  BottomRightX: %4 BottomRightY: %5\n"
                ) ;
            QString strTemp = str_mid.arg(stPrivacyMaskInfo.astArea[i].dwIndex)
                .arg(stPrivacyMaskInfo.astArea[i].dwTopLeftX)
                .arg(stPrivacyMaskInfo.astArea[i].dwTopLeftY)
                .arg(stPrivacyMaskInfo.astArea[i].dwBottomRightX)
                .arg(stPrivacyMaskInfo.astArea[i].dwBottomRightY);
            strOut += strTemp;
        }



        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_DeletePrivacyMask_clicked()
{
    NETDEV_PRIVACY_MASK_CFG_S stPrivacyMaskInfo;
    memset(&stPrivacyMaskInfo, 0, sizeof(NETDEV_PRIVACY_MASK_CFG_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_PRIVACYMASKCFG, &stPrivacyMaskInfo, sizeof(NETDEV_PRIVACY_MASK_CFG_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("DeletePrivacyMask  NETDEV_GetDevConfig failed.") ;
        msg_box->show() ;
        return;
    }

    for(INT32 i = 0; i < stPrivacyMaskInfo.dwSize; i++)
    {
        bRet = NETDEV_SetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_DELETE_PRIVACYMASKCFG, &stPrivacyMaskInfo.astArea[i].dwIndex, sizeof(INT32));
        if(TRUE != bRet)
        {
            QMessageBox *msg_box = new QMessageBox() ;
            msg_box->setText("Delete Privacymask Info failed.") ;
            msg_box->show() ;
            return;
        }
    }

    QMessageBox *msg_box = new QMessageBox() ;
    msg_box->setText("Delete Privacymask Info succeed.") ;
    msg_box->show() ;
    return;
}

void MainWindow::on_setMotionAlarm_clicked()
{
    NETDEV_MOTION_ALARM_INFO_S stMotionAlarmInfo;
    memset(&stMotionAlarmInfo, 0, sizeof(NETDEV_MOTION_ALARM_INFO_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_MOTIONALARM, &stMotionAlarmInfo, sizeof(NETDEV_MOTION_ALARM_INFO_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Motion Alarm Info NETDEV_GetDevConfig failed.") ;
        msg_box->show() ;
        return;
    }

    stMotionAlarmInfo.dwSensitivity = 88;
    stMotionAlarmInfo.dwObjectSize = 88;
    stMotionAlarmInfo.dwHistory = 2;

    memset(&stMotionAlarmInfo.awScreenInfo, 0,sizeof(stMotionAlarmInfo.awScreenInfo));
    for(INT32 i = 0; i < 18; i++)
    {
        for(INT32 j = 0; j < 22; j++)
        {
            stMotionAlarmInfo.awScreenInfo[i][j] = 1;
        }
    }

    bRet = NETDEV_SetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_SET_MOTIONALARM, &stMotionAlarmInfo, sizeof(NETDEV_MOTION_ALARM_INFO_S));
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Motion Alarm Info failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Motion Alarm Info succeed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetMotionAlarm_clicked()
{
    NETDEV_MOTION_ALARM_INFO_S stMotionAlarmInfo;
    memset(&stMotionAlarmInfo, 0, sizeof(NETDEV_MOTION_ALARM_INFO_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_MOTIONALARM, &stMotionAlarmInfo, sizeof(NETDEV_MOTION_ALARM_INFO_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get Motion Alarm Info failed.") ;
        msg_box->show() ;
    }
    else
    {
        QString str_mid("Sensitivity: %1 \n"
            "Objection Size:  %2 \n"
            "History:  %3 \n"
            ) ;
        QString strOut = str_mid.arg(stMotionAlarmInfo.dwSensitivity)
            .arg(stMotionAlarmInfo.dwObjectSize)
            .arg(stMotionAlarmInfo.dwHistory);

        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_setAlarmOutput_clicked()
{
    NETDEV_ALARM_OUTPUT_LIST_S stAlarmOutputList;
    memset(&stAlarmOutputList, 0, sizeof(NETDEV_ALARM_OUTPUT_LIST_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_ALARM_OUTPUTCFG, &stAlarmOutputList, sizeof(NETDEV_ALARM_OUTPUT_LIST_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("set alarm output info NETDEV_GetDevConfig  failed.") ;
        msg_box->show() ;
        return;
    }

    NETDEV_ALARM_OUTPUT_INFO_S stAlarmOutputInfo;
    memset(&stAlarmOutputInfo, 0, sizeof(NETDEV_ALARM_OUTPUT_INFO_S));

    stAlarmOutputInfo = stAlarmOutputList.astAlarmOutputInfo[0];
    stAlarmOutputInfo.dwDurationSec = 15;

    bRet = NETDEV_SetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_SET_ALARM_OUTPUTCFG, &stAlarmOutputInfo, sizeof(NETDEV_ALARM_OUTPUT_INFO_S));
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set alarm output info failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set alarm output info succeed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetAlarmOutput_clicked()
{
    NETDEV_ALARM_OUTPUT_LIST_S stAlarmOutputList;
    memset(&stAlarmOutputList, 0, sizeof(NETDEV_ALARM_OUTPUT_LIST_S));

    INT32 dwBytesReturned =0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_ALARM_OUTPUTCFG, &stAlarmOutputList, sizeof(NETDEV_ALARM_OUTPUT_LIST_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get alarm output info failed.") ;
        msg_box->show() ;
        return;
    }
    else
    {
        QString strOut;
        for(INT32 i = 0; i < stAlarmOutputList.dwSize; i++)
        {
            QString str_mid("Name:\t %1 \n"
                "Channel id:\t %2 \n"
                "Status:\t %3 \n"
                "Duration:\t %4 \n"
                ) ;
            QString strTemp = str_mid.arg(stAlarmOutputList.astAlarmOutputInfo[i].szName)
                .arg(stAlarmOutputList.astAlarmOutputInfo[i].dwChancelId)
                .arg(stAlarmOutputList.astAlarmOutputInfo[i].enDefaultStatus)
                .arg(stAlarmOutputList.astAlarmOutputInfo[i].dwDurationSec);
            strOut += strTemp;
        }


        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_TriggerAlarmOutput_clicked()
{
    NETDEV_ALARM_OUTPUT_LIST_S stAlarmOutputList;
    memset(&stAlarmOutputList, 0, sizeof(NETDEV_ALARM_OUTPUT_LIST_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_ALARM_OUTPUTCFG, &stAlarmOutputList, sizeof(NETDEV_ALARM_OUTPUT_LIST_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Trigger alarm output info NETDEV_GetDevConfig failed.") ;
        msg_box->show() ;
        return;
    }

    NETDEV_TRIGGER_ALARM_OUTPUT_S stTriggerAlarmOutput;
    memset(&stTriggerAlarmOutput, 0, sizeof(NETDEV_TRIGGER_ALARM_OUTPUT_S));
    strncpy(stTriggerAlarmOutput.szName, stAlarmOutputList.astAlarmOutputInfo[0].szName, NETDEV_LEN_64 - 1);

    stTriggerAlarmOutput.enOutputState = NETDEV_BOOLEAN_STATUS_ACTIVE;

    bRet = NETDEV_SetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_TRIGGER_ALARM_OUTPUT, &stTriggerAlarmOutput, sizeof(NETDEV_TRIGGER_ALARM_OUTPUT_S));
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Trigger alarm output failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Trigger alarm output succeed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetAlarmInput_clicked()
{
    NETDEV_ALARM_INPUT_LIST_S stAlarmInputList;
    memset(&stAlarmInputList, 0, sizeof(NETDEV_ALARM_INPUT_LIST_S));

    INT32 dwBytesReturned =0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_ALARM_INPUTCFG, &stAlarmInputList, sizeof(NETDEV_ALARM_INPUT_LIST_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get Alarm Input failed.") ;
        msg_box->show() ;
        return;
    }
    else
    {
        QString strOut;
        for(INT32 i = 0; i < stAlarmInputList.dwSize; i++)
        {
            QString str_mid("name:\t %1 \n") ;
            QString strTemp = str_mid.arg(stAlarmInputList.astAlarmInputInfo[i].szName);
            strOut += strTemp;
        }
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_setNTP_clicked()
{
    NETDEV_SYSTEM_NTP_INFO_S stNTPInfo;
    memset(&stNTPInfo, 0, sizeof(NETDEV_SYSTEM_NTP_INFO_S));

    stNTPInfo.bSupportDHCP = 0;
    stNTPInfo.stAddr.eIPType = NETDEV_NETWORK_HOSTTYPE_IPV4;
    strncpy(stNTPInfo.stAddr.szIPAddr, "192.168.0.14", NETDEV_LEN_132 - 1);

    BOOL bRet = NETDEV_SetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_SET_NTPCFG, &stNTPInfo, sizeof(NETDEV_SYSTEM_NTP_INFO_S));
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set NTP Info failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set NTP Info succeed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetNTP_clicked()
{
    NETDEV_SYSTEM_NTP_INFO_S stNTPInfo;
    memset(&stNTPInfo, 0, sizeof(NETDEV_SYSTEM_NTP_INFO_S));

    /* Failed to return Get information when NTP is not enabled for the NVR. Please perform Set operation first. */
    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_NTPCFG, &stNTPInfo, sizeof(NETDEV_SYSTEM_NTP_INFO_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get NTP Info failed.") ;
        msg_box->show() ;
    }
    else
    {
        QString str_mid("Support DHCP:\t %1 \n"
            "IP Type:\t %2 \n"
            "NTP Server address:\t %3 \n"
            ) ;
        QString strOut = str_mid.arg(stNTPInfo.bSupportDHCP)
            .arg(stNTPInfo.stAddr.eIPType)
            .arg(stNTPInfo.stAddr.szIPAddr);


        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_setNetworkCfg_clicked()
{
    NETDEV_NETWORKCFG_S stNetworkSetcfg;
    memset(&stNetworkSetcfg, 0, sizeof(NETDEV_NETWORKCFG_S));

    INT32 dwBytesReturned =0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_NETWORKCFG, &stNetworkSetcfg, sizeof(NETDEV_NETWORKCFG_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get Network Interfaces failed.") ;
        msg_box->show() ;
        return;
    }

    strncpy(stNetworkSetcfg.szIPv4GateWay, "192.168.1.2", NETDEV_LEN_32  -1);


    /* IP or gateway must be modified in compliance with each other */
    bRet = NETDEV_SetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_SET_NETWORKCFG, &stNetworkSetcfg, sizeof(NETDEV_NETWORKCFG_S));
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Network Interfaces failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Network Interfaces succeed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetNetworkCfg_clicked()
{
    NETDEV_NETWORKCFG_S stNetworkcfg;
    memset(&stNetworkcfg, 0, sizeof(NETDEV_NETWORKCFG_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_NETWORKCFG, &stNetworkcfg, sizeof(NETDEV_NETWORKCFG_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get Network Interfaces failed.") ;
        msg_box->show() ;
    }
    else
    {
        QString str_mid("IPv4DHCP    \t %1 \n"
            "Ipv4Address \t %2 \n"
            "IPv4GateWay \t %3 \n"
            "IPv4Subnet Mask \t %4 \n"
            "MTU         \t %5 \n"
            ) ;
        QString strOut = str_mid.arg(stNetworkcfg.bIPv4DHCP)
            .arg(stNetworkcfg.szIpv4Address)
            .arg(stNetworkcfg.szIPv4GateWay)
            .arg(stNetworkcfg.szIPv4SubnetMask)
            .arg(stNetworkcfg.dwMTU);


        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_setDeviceName_clicked()
{
    CHAR pszDeviceName[32] = "uniview";
    BOOL bRet = NETDEV_ModifyDeviceName(m_lpDevHandle, pszDeviceName);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Device Info failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Device Info succeed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetDeviceName_clicked()
{
    NETDEV_DEVICE_BASICINFO_S stDeviceInfo;
    memset(&stDeviceInfo, 0, sizeof(NETDEV_DEVICE_BASICINFO_S));

    INT32 dwBytesReturned = 0;
    BOOL bRet = NETDEV_GetDevConfig(m_lpDevHandle, m_ChannelID, NETDEV_GET_DEVICECFG, &stDeviceInfo, sizeof(NETDEV_DEVICE_BASICINFO_S), &dwBytesReturned);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get Device Info failed.") ;
        msg_box->show() ;
        return;
    }

    else
    {
        QString str_mid("Device Name:  %1 \n") ;
        QString strOut = str_mid.arg(stDeviceInfo.szDeviceName);
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_GetNatStates_clicked()
{
    NETDEV_UPNP_NAT_STATE_S stState;
    memset(&stState, 0, sizeof(NETDEV_UPNP_NAT_STATE_S));

    BOOL bRet = NETDEV_GetUpnpNatState(m_lpDevHandle, &stState);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Get UpnpNatState Info failed.") ;
        msg_box->show() ;
    }
    else
    {
        QString strOut;
        for (INT32 i = 0; i < stState.dwSize;i++)
        {
            QString str_mid;
            switch(stState.astUpnpPort[i].eType)
            {
            case 0:
                {
                    str_mid = "Protocol: HTTP \n";
                }
                break;
            case 1:
                {
                    str_mid = "Protocol: HTTPS \n";
                }
                break;
            case 2:
                {
                    str_mid = "Protocol: RTSP \n";
                }
                break;
            default:
                break;
            }
            str_mid += "    Enable:%1 Port:%2 \n\n";
            QString strTemp = str_mid.arg(stState.astUpnpPort[i].bEnbale)
                .arg(stState.astUpnpPort[i].dwPort);

            strOut += strTemp;
        }
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText(strOut) ;
        msg_box->show() ;
    }
    return;
}

void MainWindow::on_SetLogPath_clicked()
{
    CHAR pszLogPath[NETDEV_LEN_260] = "/home/yangchaofeng/log/";
    BOOL bRet = NETDEV_SetLogPath(pszLogPath);
    if (TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Log Path failed.") ;
        msg_box->show() ;
    }
    else
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Set Log Path succeed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_Discovery_clicked()
{
    strDeviceList = "";
    ui->DeviceList->clear();
    QString strBeginIP = ui->BeginIP->text();
    QByteArray mid_var_begin_Ip   = strBeginIP.toLatin1() ;
    CHAR* szBeginIP = mid_var_begin_Ip.data() ;

    QString strEndIP = ui->EndIp->text();
    QByteArray mid_var_end_Ip   = strEndIP.toLatin1() ;
    CHAR* szEndIP = mid_var_end_Ip.data() ;


    NETDEV_SetDiscoveryCallBack(NETDEV_DISCOVERY_CB, this);

    BOOL bRet = NETDEV_Discovery(szBeginIP, szEndIP);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Discovery Failed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_AutoDiscovery_clicked()
{
    strDeviceList = "";
    ui->DeviceList->clear();
    NETDEV_SetDiscoveryCallBack(NETDEV_DISCOVERY_CB, this);

    BOOL bRet = NETDEV_Discovery("0.0.0.0", "0.0.0.0");
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("Discovery Failed.") ;
        msg_box->show() ;
    }

    return;
}

void MainWindow::on_snapshotBtn_clicked()
{
    CHAR pszLogPath[NETDEV_LEN_260] = "/home/filename";
    BOOL bRet = NETDEV_CaptureNoPreview(m_lpDevHandle,m_ChannelID,NETDEV_LIVE_STREAM_INDEX_MAIN,pszLogPath,NETDEV_PICTURE_JPG);
    if(TRUE != bRet)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        msg_box->setText("snapshot Failed.") ;
        msg_box->show() ;
    }

}
void MainWindow::on_startDownRecordBtn_clicked()
{
    /* 需要获取开始结束时间*/
    CHAR* szBeginTime= ui->BeginTime->text().toLatin1().data();
    long ulStartTime = GetTick(szBeginTime);

    CHAR* szEndTime= ui->EndTime->text().toLatin1().data();
    long ulEndTime  = GetTick(szEndTime);

    if(NULL != m_lpDownloadHandle)
    {
        NETDEV_StopGetFile(m_lpDownloadHandle);
        m_lpDownloadHandle = NULL;
    }

    NETDEV_PLAYBACKCOND_S stPlayBackByTimeInfo = {0};
    stPlayBackByTimeInfo.dwChannelID = m_ChannelID;
    stPlayBackByTimeInfo.hPlayWnd = NULL;
    stPlayBackByTimeInfo.dwLinkMode = NETDEV_TRANSPROTOCAL_RTPTCP;
    stPlayBackByTimeInfo.tBeginTime = ulStartTime;
    stPlayBackByTimeInfo.tEndTime = ulEndTime;
    stPlayBackByTimeInfo.dwDownloadSpeed = NETDEV_DOWNLOAD_SPEED_EIGHT;

    CHAR pszFileName[NETDEV_LEN_260] = "/home/recordFileName";

    QMessageBox *msg_box = new QMessageBox() ;

    m_lpDownloadHandle = NETDEV_GetFileByTime(m_lpDevHandle, &stPlayBackByTimeInfo,pszFileName,NETDEV_MEDIA_FILE_MP4);
    if(NULL == m_lpDownloadHandle)
    {
        msg_box->setText("start download record fail.");
    }
    else
    {
        msg_box->setText("start download record.");
        m_timer->start(1000);
    }

    msg_box->show() ;
}

void MainWindow::on_stopDownRecordBtn_clicked()
{
    if(NULL != m_lpDownloadHandle)
    {
        QMessageBox *msg_box = new QMessageBox() ;
        m_timer->stop();
        BOOL bRet = NETDEV_StopGetFile(m_lpDownloadHandle);
        if(TRUE != bRet)
        {
            msg_box->setText("stop download record fail.");
        }
        else
        {
            msg_box->setText("stop download OK.");
        }

        m_lpDownloadHandle = NULL;
        msg_box->show() ;
    }
}

void MainWindow::on_downloadInfoBtn_clicked()
{
    m_DlgDownlaodInfo->show();
}

void MainWindow::updateDownLoadProcess()
{
    INT64 curTime = 0;
    CHAR* szBeginTime= ui->BeginTime->text().toLatin1().data();
    long ulStartTime = GetTick(szBeginTime);

    CHAR* szEndTime= ui->EndTime->text().toLatin1().data();
    long ulEndTime  = GetTick(szEndTime);

    if(NULL != m_lpDownloadHandle)
    {
        BOOL bRet = NETDEV_PlayBackControl(m_lpDownloadHandle,NETDEV_PLAY_CTRL_GETPLAYTIME,&curTime);
        if(TRUE == bRet)
        {
            if(m_lastDownLoadTime == curTime)
            {
                m_SameTimeCount++;
            }
            else
            {
                m_lastDownLoadTime = curTime;
                m_SameTimeCount = 0;
            }

            qDebug() << "download process : " << (100 * (curTime - ulStartTime)) / (ulEndTime - ulStartTime) << "%" << endl;
            if(curTime >= ulEndTime || m_SameTimeCount >10)
            {
                m_SameTimeCount = 0;
                m_lastDownLoadTime = 0;
                on_stopDownRecordBtn_clicked();
            }
        }
    }
}
