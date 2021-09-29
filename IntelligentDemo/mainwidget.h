#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "publictype.h"
#include "devicemanager.h"
#include "messagelog.h"
#include <QTableWidgetItem>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

protected:
    void closeEvent(QCloseEvent* e);

public slots:
    void showRecognitionInfoSlot(LPVOID lpUserID,LPNETDEV_PERSON_EVENT_INFO_S pstAlarmData);/* 显示人脸识别信息 */
    void showStructInfoSlot(LPVOID lpUserID,LPNETDEV_STRUCT_ALARM_INFO_S pstAlarmInfo, LPNETDEV_STRUCT_DATA_INFO_S pstAlarmData);/*界面显示结构化信息*/
    void showPlateRecogAlarmInfoSlot(LPVOID lpUserID, NETDEMO_VEH_RECOGNITION_EVENT_S* pStVehicleAlarmInfo);/*显示车牌识别上报信息*/

    void onFaceTabWgtCurChangedSlot(int index);

    void onClickFaceLibItemSlot(QTreeWidgetItem* item,int column);
    /* 日志输出 */
    void outputMegToTextBrower(const QString& messageType, const QString& msg);

public:

private:
    void startRealPlay(int dwChannelID, LPVOID hPlayWnd);
    void stopRealPlay(int dwChannelID);

    void findAllPersonLibInfo();/* 查询所有人员库信息 */
    void findPersonInfo(UINT32 udwPersonLibID, int dwOffset);/* 查询人脸库人员信息 */
    void showPersonInfo(NETDEV_PERSON_INFO_S &stPersonInfo);/*界面显示人脸库成员信息*/
    void clearPersonInfoTabWgt();/* 清除当前显示的人员信息 */
    DeviceManager* getCurSelectedDevice(LPVOID lpLoginDevice = NULL);/* 获取当前选中的设备 */
    void logout(DeviceManager* pDeviceManager); /* 登出指定设备 */
    void findFaceMonitorTaskInfo();/* 查询人脸布控信息 */
    void clearFaceMonitorInfoTabWgt();/* 清除人脸布控缓存信息 */
    void showFaceMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo);/*界面显示人脸布控任务信息*/
    void findFaceAlarmRecordInfo(UINT32 udwOffset);/* 人脸识别_人脸抓拍检索 */
    void showFaceAlarmRecordInfo(NETDEV_FACE_RECORD_SNAPSHOT_INFO_S& stRecordInfo);/* 显示人脸识别_人脸抓拍检索结果 */
    void findVehicleAlarmRecordInfo(UINT32 udwOffset);/* 车牌识别_抓拍检索 */
    void showPlateAlarmRecordInfo(NETDEV_VEHICLE_RECORD_INFO_S& stRecordInfo);/* 显示车牌识别_车牌抓拍检索结果 */
    void clearVehicleMonitorInfoTabWgt();/* 清除车辆布控缓存信息 */
    void showVehicleMonitorTaskInfo(NETDEV_MONITION_INFO_S& stMonitorInfo);/* 显示车辆布控任务信息 */
    void findVehicleMonitorTaskInfo();/*查询车辆布控任务信息*/
    void findVehicleLibInfo();/* 查询车辆库信息 */
    void clearVehicleInfoTabWgt();/* 清除当前显示的车辆信息 */
    void showVehicleInfo(NETDEV_VEHICLE_DETAIL_INFO_S &stVehicleInfo);/* 界面显示车辆信息 */
    void findVehicleInfo(UINT32 udwVehicleLibID, int dwOffset);/* 查询人脸库人员信息 */

signals:
    void showRecognitionInfoSignal(LPVOID lpUserID, LPNETDEV_PERSON_EVENT_INFO_S);/* 显示人脸识别信息信号 */
    void showStructInfoSignal(LPVOID lpUserID, LPNETDEV_STRUCT_ALARM_INFO_S, LPNETDEV_STRUCT_DATA_INFO_S);/*显示结构化信息信号*/
    void showPlateRecogAlarmInfoSignal(LPVOID lpUserID, NETDEMO_VEH_RECOGNITION_EVENT_S* pStVehicleAlarmInfo);/*显示车牌识别上报信息信号*/

private:
    Ui::MainWidget *ui;
    QList<DeviceManager*> m_DeviceList;

    QStringList m_structSnapShotPicNameList; /* 存储结构化抓拍的图片名称 */
    QStringList m_faceRecogitionSnapShotPicNameList; /* 存储人脸识别比对抓拍的图片名称 */

    int m_dwFindFaceLibMemberOffset;/* 查询人脸库成员偏移量 */
    QList<NETDEV_PERSON_INFO_S> m_faceLibMemberInfoList;/* 人脸库成员信息，存储查询到的人员信息 */
    QList<NETDEV_MONITION_INFO_S> m_faceMonitorTaskInfoList;/* 人脸布控信息，存储查询到的人脸布控信息 */
    int m_dwFindVehicleLibMemberOffset;/* 查询车辆库成员偏移量 */
    QList<NETDEV_VEHICLE_DETAIL_INFO_S> m_vehicleLibMemberInfoList;/* 车辆库成员信息，存储查询到的车辆信息 */
    QList<NETDEV_MONITION_INFO_S> m_vehicleMonitorTaskInfoList;/* 车辆布控信息，存储查询到的人脸布控信息 */

    int m_dwFindFaceAlarmRecordOffset;/* 人脸抓拍-识别记录检索偏移量 */
    int m_dwFindVehicleAlarmRecordOffset;/* 车牌抓拍-识别记录检索偏移量 */

    QString m_faceImagePath; /* 人脸图片路径，添加人员时使用 */

private slots:
    void on_addFaceLibBtn_clicked();
    void on_modifyFaceLibBtn_clicked();
    void on_delFaceLibBtn_clicked();
    void on_refreshFaceLibBtn_clicked();
    void on_previousPageFaceLibMemberBtn_clicked();
    void on_nextPageFaceLibMemberBtn_clicked();
    void on_addPersonBtn_clicked();
    void on_modifyPersonBtn_clicked();
    void on_delPersonBtn_clicked();
    void on_faceStartRealPlayBtn_clicked();
    void on_faceStopRealPlayBtn_clicked();
    void on_SubFaceRecognitionBtn_clicked();
    void on_unSubFaceRecognitionBtn_clicked();
    void on_SubFaceSnapshotBtn_clicked();
    void on_unSubFaceSnapshotBtn_clicked();
    void on_personFaceImageBtn_clicked();
    void on_loginBtn_clicked();
    void on_logoutBtn_clicked();
    void on_personInfoTabWgt_itemClicked(QTableWidgetItem *item);
    void on_findFaceMonitorTaskBtn_clicked();
    void on_faceMonitorInfoTabWgt_itemClicked(QTableWidgetItem *item);
    void on_addFaceMonitorTaskBtn_clicked();
    void on_modifyFaceMonitorTaskBtn_clicked();
    void on_delFaceMonitorTaskBtn_clicked();
    void on_enableFaceMonitorTaskBtn_clicked();
    void on_disableFaceMonitorTaskBtn_clicked();
    void on_faceRecogRecordSerachBtn_clicked();
    void on_faceRecogRecordPrieviousBtn_clicked();
    void on_faceRecogRecordNextBtn_clicked();
    void on_plateRecogRecordSerachBtn_clicked();
    void on_plateRecogRecordPreviousSerachBtn_clicked();
    void on_plateRecogRecordNextSerachBtn_clicked();
    void on_searchPlateMonitorTaskBtn_clicked();
    void on_addPlateMonitorTaskBtn_clicked();
    void on_modifyPlateMonitorTaskBtn_clicked();
    void on_deletePlateMonitorTaskBtn_clicked();
    void on_enablePlateMonitorTaskBtn_clicked();
    void on_disablePlateMonitorTaskBtn_clicked();
    void on_vehicleStartRealPlayBtn_clicked();
    void on_vehicleStopRealPlayBtn_clicked();
    void on_SubPlateRecognitionBtn_clicked();
    void on_unSubPlateRecognitionBtn_clicked();
    void on_SubPlateSnapshotBtn_clicked();
    void on_unSubPlateSnapshotBtn_clicked();
    void on_addVehicleLibBtn_clicked();
    void on_modifyVehicleLibBtn_clicked();
    void on_deleteVehicleLibBtn_clicked();
    void on_refreshVehicleLibBtn_clicked();
    void on_addVehicleBtn_clicked();
    void on_modifyVehicleBtn_clicked();
    void on_deleteVehicleBtn_clicked();
    void on_searchVehiclePreviousBtn_clicked();
    void on_searchVehicleNextBtn_clicked();
    void on_plateMonitorTaskInfoTabWgt_itemClicked(QTableWidgetItem *item);
    void on_vehicleLibTreeWgt_itemClicked(QTreeWidgetItem *item, int column);
    void on_vehicleInfoTabWgt_itemClicked(QTableWidgetItem *item);
};

#endif // MAINWIDGET_H
