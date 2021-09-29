#ifndef MLISTWIDGET_H
#define MLISTWIDGET_H

#include <QTreeWidget>

class MTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit MTreeWidget(QWidget *parent = 0);

    static QString MimeType() { return QStringLiteral("custom/video_output_channelID"); }

protected:
    void startDrag(Qt::DropActions supportedActions) override;
};

#endif // MLISTWIDGET_H
