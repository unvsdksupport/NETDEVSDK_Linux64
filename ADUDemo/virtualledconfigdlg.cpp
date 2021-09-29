#include "virtualledconfigdlg.h"
#include "ui_virtualledconfigdlg.h"
#include <QColorDialog>
#include <QRgb>

VirtualLEDConfigDlg::VirtualLEDConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VirtualLEDConfigDlg)
{
    ui->setupUi(this);

    m_bIsAddLED = true;
    memset(&m_stVirtualLEDInfo,0,sizeof(NETDEV_XW_VIRTUAL_LED_INFO_S));
}

VirtualLEDConfigDlg::~VirtualLEDConfigDlg()
{
    delete ui;
}

/* 设置虚拟LED信息 */
void VirtualLEDConfigDlg::setVirtualLEDInfo(NETDEV_XW_VIRTUAL_LED_INFO_S stVirtualLEDInfo, bool bIsAdd)
{
    m_bIsAddLED = bIsAdd;
    m_stVirtualLEDInfo = stVirtualLEDInfo;
    /* 显示LED信息 */
    if(true == m_bIsAddLED)
    {
        /* 初始化一个默认颜色 */
        bool ok;
        m_stVirtualLEDInfo.stBackground.udwBaseColor = QString("0x000000").toInt(&ok,16);
        m_stVirtualLEDInfo.stFontInfo.udwColor = QString("0xFF0000").toInt(&ok,16);
    }
    else {
        ui->contentLineEdt->setText(QString(stVirtualLEDInfo.szText));
        ui->enableCmbBox->setCurrentIndex(m_stVirtualLEDInfo.udwEnable);
        if(100 == m_stVirtualLEDInfo.stBackground.udwTransparency)
        {
            ui->transparencyCmbBox->setCurrentIndex(0);/* 全透明 */
        }
        else {
            ui->transparencyCmbBox->setCurrentIndex(1);/* 不透明 */
        }

        ui->typefaceCmbBox->setCurrentIndex(m_stVirtualLEDInfo.stFontInfo.udwFont);
        if(NETDEV_FONT_SIZE_SELF_ADAPTION == m_stVirtualLEDInfo.stFontInfo.udwSize)/*自适应*/
        {
            ui->fontSizeCmbBox->setCurrentIndex(0);
        }
        else
        {
            ui->fontSizeCmbBox->setCurrentIndex(1);
             ui->fontSizeSlider->setValue(m_stVirtualLEDInfo.stFontInfo.udwSize);
        }

        /*字体间距*/
        for(int i = 0; i < sizeof(gaFontGapConvert)/sizeof(NETDEMO_LED_FONT_GAP_MAPPING_S); i++)
        {
            if(m_stVirtualLEDInfo.stFontInfo.udwGap == gaFontGapConvert[i].eFontGap)
            {
                ui->fontGapCmbBox->setCurrentIndex(gaFontGapConvert[i].dwFontGapIndex);
                break;
            }
        }

        ui->horizontalAlignCmbBox->setCurrentIndex(m_stVirtualLEDInfo.stFontInfo.udwHorizontalAlign);
        ui->verticalAlignCmbBox->setCurrentIndex(m_stVirtualLEDInfo.stFontInfo.udwVerticalAlign);
        ui->ScrollModeCmbBox->setCurrentIndex(m_stVirtualLEDInfo.stFontInfo.udwScrollMode);
        ui->scrollRateSlider->setValue(m_stVirtualLEDInfo.stFontInfo.udwScrollRateSec);
        ui->enableCmbBox->setCurrentIndex(m_stVirtualLEDInfo.udwEnable);
    }
}

void VirtualLEDConfigDlg::on_fontColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(Qt::red,this,tr("字体颜色"));
    if(!color.isValid())
    {
        return;
    }

    QString strHexColor = QString("%1%2%3").arg(color.red(),2,16,QLatin1Char('0'))
            .arg(color.green(),2,16,QLatin1Char('0'))
            .arg(color.blue(),2,16,QLatin1Char('0'));

    bool ok;
    qDebug()<<"select color valus:"<<strHexColor << ":" << strHexColor.toInt(&ok,16);
    m_stVirtualLEDInfo.stFontInfo.udwColor = strHexColor.toInt(&ok,16);
    ui->fontColorBtn->setStyleSheet("background-color:#"+strHexColor);
}

void VirtualLEDConfigDlg::on_backgroundColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(Qt::black,this,tr("背景颜色"));
    if(!color.isValid())
    {
        return;
    }

    QString strHexColor = QString("%1%2%3").arg(color.red(),2,16,QLatin1Char('0'))
            .arg(color.green(),2,16,QLatin1Char('0'))
            .arg(color.blue(),2,16,QLatin1Char('0'));

    bool ok;
    qDebug()<<"select color valus:"<<strHexColor << ":" << strHexColor.toInt(&ok,16);
    m_stVirtualLEDInfo.stBackground.udwBaseColor = strHexColor.toInt(&ok,16);

    ui->backgroundColorBtn->setStyleSheet("background-color:#"+strHexColor);
}



void VirtualLEDConfigDlg::on_buttonBox_accepted()
{
    if(ui->contentLineEdt->text().isEmpty())
    {
        qDebug()<<"content is Empty" << endl;
        return;
    }
    strncpy(m_stVirtualLEDInfo.szText,ui->contentLineEdt->text().toStdString().c_str(),sizeof(m_stVirtualLEDInfo.szText) -1);
    m_stVirtualLEDInfo.udwType = NETDEV_LED_TYPE_STRING;
    m_stVirtualLEDInfo.udwEnable = ui->enableCmbBox->currentIndex();
    if(0 == ui->transparencyCmbBox->currentIndex())
    {
        m_stVirtualLEDInfo.stBackground.udwTransparency = 100;/* 全透明 */
    }
    else {
        m_stVirtualLEDInfo.stBackground.udwTransparency = 0;/* 不透明 */
    }

    m_stVirtualLEDInfo.stFontInfo.udwFont = ui->typefaceCmbBox->currentIndex();
    if(0 == ui->fontSizeCmbBox->currentIndex())/*自适应*/
    {
        m_stVirtualLEDInfo.stFontInfo.udwSize = NETDEV_FONT_SIZE_SELF_ADAPTION;
        /* 需计算参考字体大小，目前是在信号virtualLEDChangedSignal对应的槽函数中计算。 */
        //m_stVirtualLEDInfo.stFontInfo.udwReferFontSize;
    }
    else
    {
        m_stVirtualLEDInfo.stFontInfo.udwSize = ui->fontSizeSlider->value();
        m_stVirtualLEDInfo.stFontInfo.udwReferFontSize = 0;
    }

    /*字体间距*/
    for(int i = 0; i < sizeof(gaFontGapConvert)/sizeof(NETDEMO_LED_FONT_GAP_MAPPING_S); i++)
    {
        if(ui->fontGapCmbBox->currentIndex() == gaFontGapConvert[i].dwFontGapIndex)
        {
            m_stVirtualLEDInfo.stFontInfo.udwGap = gaFontGapConvert[i].eFontGap;
            break;
        }
    }
    if(NETDEV_LED_FONT_GAP_INVALID == m_stVirtualLEDInfo.stFontInfo.udwGap)/* 自适应字间距 */
    {
        /*计算参考字间距*/
        m_stVirtualLEDInfo.stFontInfo.udwReferFontGap = 0;/*先默认赋值为0,在MPlayWidget中计算*/
    }

    m_stVirtualLEDInfo.stFontInfo.udwHorizontalAlign = ui->horizontalAlignCmbBox->currentIndex();
    m_stVirtualLEDInfo.stFontInfo.udwVerticalAlign = ui->verticalAlignCmbBox->currentIndex();
    m_stVirtualLEDInfo.stFontInfo.udwScrollMode = ui->ScrollModeCmbBox->currentIndex();
    m_stVirtualLEDInfo.stFontInfo.udwScrollRateSec = ui->scrollRateSlider->value();

    /* 设置修改类型 */
    if(false == m_bIsAddLED)
    {
        m_stVirtualLEDInfo.udwModifyCmd = NETDEV_LED_ENABLE|NETDEV_LED_BACKGROUND|NETDEV_LED_FONTINFO|NETDEV_LED_TEXT;
    }

    emit virtualLEDChangedSignal(m_stVirtualLEDInfo,m_bIsAddLED);
}

void VirtualLEDConfigDlg::on_buttonBox_rejected()
{

}

void VirtualLEDConfigDlg::on_scrollRateSlider_valueChanged(int value)
{
    ui->ScrollRateLab->setText(QString::number(value)+ "倍速");
}

void VirtualLEDConfigDlg::on_fontSizeSlider_valueChanged(int value)
{
    ui->fontSizeLab->setText(QString::number(value));
}
