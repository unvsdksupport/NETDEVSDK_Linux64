#ifndef MTABLEWIDGET_H
#define MTABLEWIDGET_H

#include <QTableWidget>

class MTableWidget: public QTableWidget
{
    Q_OBJECT
public:
    MTableWidget(QWidget *parent = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // MTABLEWIDGET_H
