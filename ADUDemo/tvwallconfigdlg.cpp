#include "tvwallconfigdlg.h"
#include "playscreen.h"
#include "playwindow.h"
#include <QScrollBar>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QHeaderView>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>

TVWallConfigDlg::TVWallConfigDlg(QWidget *parent) :
    QDialog(parent)
{
    m_bIsAddTVWall = true;

    setWindowTitle(QString("电视墙信息"));
    resize(900,600);
    setWindowFlags( windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);

    m_TVWallNameLineEdt = new QLineEdit();
    m_TVWallRowCountLineEdt = new QLineEdit("2");
    m_TVWallColumnCountLineEdt = new QLineEdit("3");
    m_SetTVWallFormatBtn = new QPushButton("Set");
    m_resolutionListCmb = new QComboBox();
    m_decoderListCmb = new QComboBox();
    m_decoderOutVideoChlTreeWgt = new MTreeWidget();
    m_TVWallVideoChannelTabWgt = new MTableWidget();
    m_OkBtn = new QPushButton("OK");
    m_cancelBtn = new QPushButton("Cancel");
    QLabel* tvWallNameLbel = new QLabel(QString("电视墙名称"));
    QLabel* tvWallFormatLbel = new QLabel(QString("电视墙规格"));
    QLabel* formatSpaceLbel = new QLabel(QString("X"));
    QLabel* resolutionLbel = new QLabel(QString("分辨率"));

    m_TVWallVideoChannelTabWgt->setDragDropMode(QAbstractItemView::DropOnly);
    m_TVWallVideoChannelTabWgt->verticalHeader()->setHidden(true);
    m_TVWallVideoChannelTabWgt->horizontalHeader()->setHidden(true);
    m_TVWallVideoChannelTabWgt->horizontalScrollBar()->setHidden(true);
    m_TVWallVideoChannelTabWgt->verticalScrollBar()->setHidden(true);
    m_TVWallVideoChannelTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    m_TVWallVideoChannelTabWgt->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应行高
    m_TVWallVideoChannelTabWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_TVWallVideoChannelTabWgt->setContextMenuPolicy(Qt::CustomContextMenu);

    m_decoderOutVideoChlTreeWgt->setDragEnabled(true);
    m_decoderOutVideoChlTreeWgt->setHeaderLabel("视频输出通道");
    m_decoderOutVideoChlTreeWgt->header()->setDefaultAlignment(Qt::AlignHCenter);
    m_decoderOutVideoChlTreeWgt->setMaximumWidth(300);

    m_resolutionListCmb->setMinimumWidth(200);

    /* 界面布局 */
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(tvWallNameLbel);
    topLayout->addWidget(m_TVWallNameLineEdt);
    topLayout->addStretch(1);
    topLayout->addWidget(tvWallFormatLbel);
    topLayout->addWidget(m_TVWallRowCountLineEdt);
    topLayout->addWidget(formatSpaceLbel);
    topLayout->addWidget(m_TVWallColumnCountLineEdt);
    topLayout->addWidget(m_SetTVWallFormatBtn);
    topLayout->addStretch(1);
    topLayout->addWidget(resolutionLbel);
    topLayout->addWidget(m_resolutionListCmb);
    topLayout->addStretch(1);

    QVBoxLayout* decoderListLayout = new QVBoxLayout();
    decoderListLayout->addWidget(m_decoderListCmb);
    decoderListLayout->addWidget(m_decoderOutVideoChlTreeWgt);

    QHBoxLayout* okCannelLayout = new QHBoxLayout();
    okCannelLayout->addStretch(1);
    okCannelLayout->addWidget(m_OkBtn);
    okCannelLayout->addWidget(m_cancelBtn);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addLayout(decoderListLayout);
    bottomLayout->addWidget(m_TVWallVideoChannelTabWgt);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addLayout(okCannelLayout);
    this->setLayout(mainLayout);

    /*信号槽*/
    connect(m_SetTVWallFormatBtn,SIGNAL(clicked()),this,SLOT(on_SetTVWallFormatBtn_clicked()));
    connect(m_OkBtn,SIGNAL(clicked()),this,SLOT(okSlot()));
    connect(m_cancelBtn,SIGNAL(clicked()),this,SLOT(cancelSlot()));
    connect(m_decoderListCmb,SIGNAL(currentIndexChanged(int)),this,SLOT(on_decoderListCmb_currentIndexChanged(int)));
    connect(m_TVWallVideoChannelTabWgt,SIGNAL(customContextMenuRequested ( const QPoint &)),this, SLOT(onTVWallVideoChlTabMenuSlot(const QPoint &)));

}

TVWallConfigDlg::~TVWallConfigDlg()
{

}

void TVWallConfigDlg::init()
{
    m_decoderListCmb->setEnabled(true);
    m_TVWallRowCountLineEdt->setReadOnly(false);
    m_TVWallColumnCountLineEdt->setReadOnly(false);
    m_SetTVWallFormatBtn->setEnabled(true);

    this->m_bIsAddTVWall = true;/* 默认为新建电视墙 */

    m_decoderListCmb->clear();
    m_decoderOutVideoChlTreeWgt->clear();
    m_TVWallVideoChannelTabWgt->clear();

    m_decoderListCmb->setEnabled(true);

    for(int i = 0; i < gDecoderList.size(); i++)
    {
        if(NULL != gDecoderList[i].lpLoginHandle)
        {
            m_decoderListCmb->addItem(QString(gDecoderList[i].stDeviceBaseInfo.szDevModel) + "_" + gDecoderList[i].stDeviceLoginInfo.strIpAddress);
        }
    }

    /* 设置电视墙行列显示单元 */
    on_SetTVWallFormatBtn_clicked();
}

/* 电视墙配置修改信息 */
void TVWallConfigDlg::TVWallModifyInfo(int dwDecoderIndex, NETDEV_XW_TVWALL_CFG_S stTVWallCfg)
{
    this->m_bIsAddTVWall = false;
    m_stTVWallCfg = stTVWallCfg;


    m_decoderListCmb->clear();
    m_decoderOutVideoChlTreeWgt->clear();
    m_TVWallVideoChannelTabWgt->clear();

    if(NULL != gDecoderList[dwDecoderIndex].lpLoginHandle)
    {
        m_decoderListCmb->addItem(QString(gDecoderList[dwDecoderIndex].stDeviceBaseInfo.szDevModel) + "_" + gDecoderList[dwDecoderIndex].stDeviceLoginInfo.strIpAddress);
    }

    m_decoderListCmb->setEnabled(false);

    /* 显示电视墙信息 */
    /* 电视墙名称 */
    m_TVWallNameLineEdt->setText(QString(stTVWallCfg.szName));
    /* 电视墙显示单元行列数，不允许修改 */
    m_TVWallRowCountLineEdt->setText(QString::number(stTVWallCfg.stScreenInfo.udwRowNum));
    m_TVWallColumnCountLineEdt->setText(QString::number(stTVWallCfg.stScreenInfo.udwColNum));
    m_TVWallRowCountLineEdt->setReadOnly(true);
    m_TVWallColumnCountLineEdt->setReadOnly(true);
    m_SetTVWallFormatBtn->setEnabled(false);
    /* 电视墙分辨率 */
    INT32 udwMapSize = sizeof(gaFormat)/sizeof(NETDEMO_FORMAT_MAPPING_S);
    for(int j = 0; j < udwMapSize; j++)
    {
        if(stTVWallCfg.stScreenInfo.udwFormat == gaFormat[j].dwFormat)
        {
            m_resolutionListCmb->setCurrentText(gaFormat[j].strFormat);
            break;
        }
    }

    /* 电视墙显示单元信息 */
    on_SetTVWallFormatBtn_clicked();
    for(int rowIndex = 0; rowIndex < m_TVWallVideoChannelTabWgt->rowCount(); rowIndex++)
    {
        for(int columnIndex = 0; columnIndex < m_TVWallVideoChannelTabWgt->columnCount(); columnIndex++)
        {
            QTableWidgetItem* item = m_TVWallVideoChannelTabWgt->item(rowIndex,columnIndex);
            for(int dwVideoOutIndex = 0; dwVideoOutIndex < stTVWallCfg.udwVideoOutNum; dwVideoOutIndex++)
            {
                int dwTVWallScreenIDIndex = rowIndex * m_TVWallVideoChannelTabWgt->columnCount() + columnIndex + 1;
                if(dwTVWallScreenIDIndex == stTVWallCfg.astVideoOut[dwVideoOutIndex].udwScreenID)
                {
                    item->setText(QString::number(stTVWallCfg.astVideoOut[dwVideoOutIndex].udwVideoOutID));
                    NETDEMO_DEVICE_ITEM_DATA_S stItemData;
                    stItemData.dwDeviceIndex = dwDecoderIndex;
                    stItemData.udwChannelID = stTVWallCfg.astVideoOut[dwVideoOutIndex].udwVideoOutID;
                    item->setData(Qt::UserRole,QVariant::fromValue(stItemData));
                    item->setTextAlignment(Qt::AlignCenter);

                    break;
                }
            }
        }
    }
}

void TVWallConfigDlg::switchCurrentDecoder(int dwDecoderIndex)
{
    if(dwDecoderIndex < 0)
    {
        return;
    }

    m_decoderOutVideoChlTreeWgt->clear();
    m_resolutionListCmb->clear();

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QString(gDecoderList[dwDecoderIndex].stDeviceBaseInfo.szDevModel) + "_" + gDecoderList[dwDecoderIndex].stDeviceLoginInfo.strIpAddress);
    m_decoderOutVideoChlTreeWgt->addTopLevelItem(item);

    /* 视频输出通道 */
    for(int j = 0; j < gDecoderList[dwDecoderIndex].stChannelsList.udwVideoOutNum; j++)
    {
        QTreeWidgetItem* item_sub = new QTreeWidgetItem();
        NETDEMO_DEVICE_ITEM_DATA_S stItemData;
        stItemData.dwDeviceIndex = dwDecoderIndex;
        stItemData.udwChannelID = gDecoderList[dwDecoderIndex].stChannelsList.pstVideoOut[j].dwChannelID;
        item_sub->setData(0,Qt::UserRole,QVariant::fromValue(stItemData));
        item_sub->setText(0, QString("channel_" + QString::number(stItemData.udwChannelID)));
        item->addChild(item_sub);
    }

    /* 大屏输出制式 */
    for(int i = 0; i < gDecoderList[dwDecoderIndex].stCapInfo.stVideoFormats.udwSize; i++)
    {
        int dwVideoFormat = gDecoderList[dwDecoderIndex].stCapInfo.stVideoFormats.adwFormatList[i];
        UINT32 udwMapSize = sizeof(gaFormat)/sizeof(NETDEMO_FORMAT_MAPPING_S);
        for(int j = 0; j < udwMapSize; j++)
        {
            if(dwVideoFormat == gaFormat[j].dwFormat)
            {
                m_resolutionListCmb->addItem(gaFormat[j].strFormat);
            }
        }
    }
}

void TVWallConfigDlg::on_SetTVWallFormatBtn_clicked()
{
     m_TVWallVideoChannelTabWgt->clear();

    int dwRowCount = m_TVWallRowCountLineEdt->text().toInt();
    int dwColumnCount = m_TVWallColumnCountLineEdt->text().toInt();

    m_TVWallVideoChannelTabWgt->setRowCount(dwRowCount);
    m_TVWallVideoChannelTabWgt->setColumnCount(dwColumnCount);

    for(int rowIndex = 0; rowIndex < dwRowCount; rowIndex++)
    {
        for(int columnIndex = 0; columnIndex < dwColumnCount; columnIndex++)
        {
            /* 显示单元ID从左至右，从上至下，从1开始递增 */
            int dwTVWallScreenIDIndex = rowIndex * dwColumnCount + columnIndex + 1;
            QTableWidgetItem* item = new QTableWidgetItem(QString("未绑定") + QString::number(dwTVWallScreenIDIndex));
            NETDEMO_DEVICE_ITEM_DATA_S stItemData;
            stItemData.dwDeviceIndex = -1;/*默认未绑定输出通道的显示单元设备索引为-1*/
            stItemData.udwChannelID = 0;/*默认未绑定输出通道的显示单元通道ID为0*/
            item->setData(Qt::UserRole,QVariant::fromValue(stItemData));
            item->setTextAlignment(Qt::AlignCenter);
            m_TVWallVideoChannelTabWgt->setItem(rowIndex, columnIndex, item);
            item->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void TVWallConfigDlg::okSlot()
{
    NETDEV_XW_TVWALL_CFG_S stTVWallCfg = {0};

    if(false == m_bIsAddTVWall)
    {
        stTVWallCfg = m_stTVWallCfg;/* 界面没有修改的信息保持原样 */
    }

    int dwDecoderIndex = m_decoderListCmb->currentIndex();
    if(dwDecoderIndex < 0)
    {
        return;
    }

    /* 电视墙名称、行显示单元数、列显示单元数、屏幕输出制式 */
    strncpy(stTVWallCfg.szName,m_TVWallNameLineEdt->text().toStdString().c_str(),sizeof(stTVWallCfg.szName) -1);
    stTVWallCfg.stScreenInfo.udwRowNum = m_TVWallRowCountLineEdt->text().toUInt();
    stTVWallCfg.stScreenInfo.udwColNum = m_TVWallColumnCountLineEdt->text().toUInt();
    INT32 udwMapSize = sizeof(gaFormat)/sizeof(NETDEMO_FORMAT_MAPPING_S);
    for(int j = 0; j < udwMapSize; j++)
    {
        if(m_resolutionListCmb->currentText() == gaFormat[j].strFormat)
        {
            stTVWallCfg.stScreenInfo.udwFormat = gaFormat[j].dwFormat;
            break;
        }
    }

    /*视频输出通道与电视墙显示单元绑定*/
    int videoOutRowCount = m_TVWallVideoChannelTabWgt->rowCount();
    int videoOutColumnCount = m_TVWallVideoChannelTabWgt->columnCount();
    uint dwVideoOutIndex = 0;
    for(int videoOutRowIndex = 0; videoOutRowIndex < videoOutRowCount; videoOutRowIndex++)
    {
        for(int videoOutColumnIndex = 0; videoOutColumnIndex < videoOutColumnCount; videoOutColumnIndex++)
        {
            QTableWidgetItem* item = m_TVWallVideoChannelTabWgt->item(videoOutRowIndex,videoOutColumnIndex);
            QVariant variant = item->data(Qt::UserRole);  // 获取用户数据
            NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();
            if(-1 != stItemData.dwDeviceIndex)
            {
                uint dwScreenIDIndex = videoOutRowIndex * videoOutColumnCount + videoOutColumnIndex + 1;
                QString strVideoOutputChannelID = QString::number(stItemData.udwChannelID);
                stTVWallCfg.astVideoOut[dwVideoOutIndex].udwScreenID = dwScreenIDIndex;
                stTVWallCfg.astVideoOut[dwVideoOutIndex].udwVideoOutID = strVideoOutputChannelID.toUInt();
                dwVideoOutIndex++;
            }
        }
    }

    stTVWallCfg.udwVideoOutNum = dwVideoOutIndex;

    emit updateTVWallSignal(dwDecoderIndex, m_bIsAddTVWall,stTVWallCfg);
    close();
}

void TVWallConfigDlg::cancelSlot()
{
    close();
}

void TVWallConfigDlg::on_decoderListCmb_currentIndexChanged(int index)
{
    switchCurrentDecoder(index);
}

void TVWallConfigDlg::onDelVideoOutChlBindSlot()
{
    int dwCurRowIndex = m_TVWallVideoChannelTabWgt->currentRow();
    int dwCurColumnIndex = m_TVWallVideoChannelTabWgt->currentColumn();
    int dwColumnCount = m_TVWallVideoChannelTabWgt->columnCount();

    int dwScreenIDIndex = dwCurRowIndex * dwColumnCount + dwCurColumnIndex + 1;
    QTableWidgetItem* item = m_TVWallVideoChannelTabWgt->currentItem();
    QVariant variant = item->data(Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();
    if(-1 != stItemData.dwDeviceIndex)
    {
        item->setText(QString("未绑定") + QString::number(dwScreenIDIndex));
        NETDEMO_DEVICE_ITEM_DATA_S stItemData;
        stItemData.dwDeviceIndex = -1;/*默认未绑定输出通道的显示单元设备索引为-1*/
        stItemData.udwChannelID = 0;/*默认未绑定输出通道的显示单元通道ID为0*/
        item->setData(Qt::UserRole,QVariant::fromValue(stItemData));
        qDebug() << stItemData.udwChannelID << stItemData.dwDeviceIndex;

    }
}

void TVWallConfigDlg::onTVWallVideoChlTabMenuSlot(const QPoint& pos)
{
    QTableWidgetItem* item = m_TVWallVideoChannelTabWgt->currentItem();
    QVariant variant = item->data(Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();
    if(-1 == stItemData.dwDeviceIndex)
    {
        return;
    }

    //构造右键菜单
    QMenu *pMenu = new QMenu(this);
    QAction *delBindAction = new QAction(tr("解除绑定"), this);

    connect(delBindAction,SIGNAL(triggered()),this,SLOT(onDelVideoOutChlBindSlot()));

    pMenu->addAction(delBindAction);

    //显示菜单
    pMenu->exec(QCursor::pos());

    delete pMenu;
    delete delBindAction;
}
