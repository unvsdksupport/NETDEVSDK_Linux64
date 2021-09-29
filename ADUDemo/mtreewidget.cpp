#include "mtreewidget.h"
#include "publictype.h"
#include <QTreeWidgetItem>
#include <QMimeData>
#include <QDrag>

MTreeWidget::MTreeWidget(QWidget *parent)
: QTreeWidget(parent)
{
    setDragEnabled(true);
    setDropIndicatorShown(true);
}

void MTreeWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
    QTreeWidgetItem *item = currentItem();
    if(NULL == item->parent())
    {
        return;
    }

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QVariant variant = item->data(0,Qt::UserRole);  // 获取用户数据
    NETDEMO_DEVICE_ITEM_DATA_S stItemData = variant.value<NETDEMO_DEVICE_ITEM_DATA_S>();

    QString strDeviceIndex = QString::number(stItemData.dwDeviceIndex);
    QString strChannelID = QString::number(stItemData.udwChannelID);

    dataStream << strDeviceIndex << strChannelID;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MTreeWidget::MimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    drag->exec();
}
