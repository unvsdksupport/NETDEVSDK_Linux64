#include "mtablewidget.h"
#include "publictype.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>

MTableWidget::MTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    setAcceptDrops(true);
}

void MTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("custom/video_output_channelID"))
        {
            event->acceptProposedAction();
        }
        else
        {
            event->ignore();
            QTableWidget::dragEnterEvent(event);
        }
}

void MTableWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QTableWidget::dragLeaveEvent(event);
}

void MTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasFormat("custom/video_output_channelID"))
    {
        QTableWidgetItem* itemOver = itemAt(event->pos());
        if(itemOver == NULL)
        {
        }
        else
        {
            // accept
            event->acceptProposedAction();
            return;
        }
    }

    event->ignore();
    QTableWidget::dragMoveEvent(event);
}

void MTableWidget::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat("custom/video_output_channelID"))
    {
        QTableWidgetItem* itemOver = itemAt(event->pos());
        if(itemOver != NULL)
        {
            // 提取drag中携带的信息
            QByteArray data = event->mimeData()->data("custom/video_output_channelID");
            QDataStream dataStream(&data, QIODevice::ReadOnly);
            QString strDeviceIndex;
            QString strChannelID;
            dataStream >> strDeviceIndex >> strChannelID;
            itemOver->setText(strChannelID);
            NETDEMO_DEVICE_ITEM_DATA_S stItemData;
            stItemData.dwDeviceIndex = strDeviceIndex.toInt();
            stItemData.udwChannelID = strChannelID.toUInt();
            itemOver->setData(Qt::UserRole,QVariant::fromValue(stItemData));
            qDebug() << strDeviceIndex << strChannelID;
            event->acceptProposedAction();
            return;
        }
    }

    event->ignore();
    QTableWidget::dropEvent(event);
}
