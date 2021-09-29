#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "NetDEVSDK.h"
#include <QMainWindow>
#include <QString>
#include <QComboBox>
#include "dlgdownloadinfo.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0) ;
    ~MainWindow() ;

private slots:
    BOOL Init() ;
    void Uninit();
    void on_Login_clicked() ;
    void on_Logout_clicked() ;
    void on_Version_clicked() ;
    void on_GET_STREAM_clicked() ;
    void on_get_imaInfo_clicked() ;
    void on_NORTH_WEST_clicked() ;
    void on_UP_clicked() ;
    void on_NORTH_EAST_clicked() ;
    void on_LEFT_clicked() ;
    void on_STOP_clicked() ;
    void on_RIGHT_clicked() ;
    void on_SOUTH_WEST_clicked() ;
    void on_DOWN_clicked() ;
    void on_SOUT_EAST_clicked() ;
    void on_FOCUS_IN_clicked() ;
    void on_FOCUS_OUT_clicked() ;
    void on_SET_STREAM_clicked();
    void on_SET_IMAGE_INFO_clicked();
    void on_ChannelList_activated(const QString &arg1);
    void on_GetStreamURL_clicked();
    void on_GetReplayURL_clicked();
    void on_CommonQuery_clicked();

    void on_StartFlow_clicked();

    void on_StopFlow_clicked();

    void on_StartReplayFlow_clicked();

    void on_StopReplayFlow_clicked();

    void on_FocusFar_clicked();

    void on_FocusNear_clicked();

    void on_setSystemTime_clicked();

    void on_GetSystemTime_2_clicked();

    void on_setOSD_clicked();

    void on_GetOSD_clicked();

    void on_setPrivacyMask_clicked();

    void on_GetPrivacyMask_clicked();

    void on_DeletePrivacyMask_clicked();

    void on_setMotionAlarm_clicked();

    void on_GetMotionAlarm_clicked();

    void on_setAlarmOutput_clicked();

    void on_GetAlarmOutput_clicked();

    void on_TriggerAlarmOutput_clicked();

    void on_GetAlarmInput_clicked();

    void on_setNTP_clicked();

    void on_GetNTP_clicked();

    void on_setNetworkCfg_clicked();

    void on_GetNetworkCfg_clicked();

    void on_setDeviceName_clicked();

    void on_GetDeviceName_clicked();

    void on_GetNatStates_clicked();

    void on_SetLogPath_clicked();

    void on_Discovery_clicked();

    void on_AutoDiscovery_clicked();

    void on_snapshotBtn_clicked();

    void on_startDownRecordBtn_clicked();

    void on_stopDownRecordBtn_clicked();

    void on_downloadInfoBtn_clicked();

    void updateDownLoadProcess();

protected:
    void timerEvent(QTimerEvent *event);
    int m_nTimerId;
	long GetTick(char *str_time);

public:
    DlgDownloadInfo * m_DlgDownlaodInfo;
    QTimer  * m_timer;
    INT64 m_lastDownLoadTime;
    int m_SameTimeCount;
    QComboBox *m_cBoxChlList ;
    QString m_strDevIP ;
    QString m_strUser ;
    QString m_strPwd ;
    int m_LoginDeviceType;
    LPVOID  m_lpDevHandle ;
    INT32   m_PresetID ;
    INT32   m_ChannelID ;
    INT32   m_CruiseID ;
    NETDEV_FINDDATA_S m_stVodFile ;
    Ui::MainWindow *ui ;
    LPVOID  m_lpPlayHandle ;
    LPVOID m_lpDownloadHandle;
    UINT32 m_ulSession;
};

#endif // MAINWINDOW_H
