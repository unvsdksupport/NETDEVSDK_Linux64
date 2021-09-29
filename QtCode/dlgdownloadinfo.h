#ifndef DLGDOWNLOADINFO_H
#define DLGDOWNLOADINFO_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class DlgDownloadInfo;
}

class DlgDownloadInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDownloadInfo(QWidget *parent = 0);
    ~DlgDownloadInfo();

private:
    Ui::DlgDownloadInfo *ui;
};

#endif // DLGDOWNLOADINFO_H
