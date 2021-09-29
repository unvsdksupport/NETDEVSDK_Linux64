#include "dlgdownloadinfo.h"
#include "ui_dlgdownloadinfo.h"

DlgDownloadInfo::DlgDownloadInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDownloadInfo)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);

    QStringList header;
    header <<"BeginTime" <<"EndTime"<<"Porcess"<<"Path";
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

DlgDownloadInfo::~DlgDownloadInfo()
{
    delete ui;
}
