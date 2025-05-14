#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    _isAppLoading(true)
{
    ui->setupUi(this);

    SysAudio::getInstance().init();
}

void Widget::showEvent(QShowEvent *)
{
    int volumeFront = SysAudio::getInstance().getPartVolume("Front");
    ui->hSlider_Front->setValue(volumeFront);
    ui->lbl_VolFront->setText(QString::number(volumeFront));

    int volumeRear = SysAudio::getInstance().getPartVolume("Rear");
    ui->hSlider_Rear->setValue(volumeRear);
    ui->lbl_VolRear->setText(QString::number(volumeRear));

    int volumeSub = SysAudio::getInstance().getPartVolume("Subwoofer");
    ui->hSlider_Sub->setValue(volumeSub);
    ui->lbl_VolSub->setText(QString::number(volumeSub));

    int volumeCenter = SysAudio::getInstance().getPartVolume("Center");
    ui->hSlider_Center->setValue(volumeCenter);
    ui->lbl_VolCenter->setText(QString::number(volumeCenter));

    int volumeSide = SysAudio::getInstance().getPartVolume("Side");
    ui->hSlider_Side->setValue(volumeSide);
    ui->lbl_VolSide->setText(QString::number(volumeSide));

    // if volume = slider-value slot slider_valueChanged don't call!
    // this flag fix twice set volume on load app (when get-volume != slider-value)
    _isAppLoading = false;
}

void Widget::on_hSlider_Front_valueChanged(int value)
{
    if(_isAppLoading)
    {
        return;
    }

    qDebug() << Q_FUNC_INFO;

    SysAudio::getInstance().setPartVolume("Front", value);
    ui->lbl_VolFront->setText(QString::number(value));
}

void Widget::on_hSlider_Rear_valueChanged(int value)
{
    if(_isAppLoading)
    {
        return;
    }

    qDebug() << Q_FUNC_INFO;

    SysAudio::getInstance().setPartVolume("Rear", value);
    ui->lbl_VolRear->setText(QString::number(value));
}

void Widget::on_hSlider_Sub_valueChanged(int value)
{
    if(_isAppLoading)
    {
        return;   
    }

    qDebug() << Q_FUNC_INFO;

    SysAudio::getInstance().setPartVolume("Subwoofer", value);
    ui->lbl_VolSub->setText(QString::number(value));
}

void Widget::on_hSlider_Center_valueChanged(int value)
{
    if(_isAppLoading)
    {
        return;
    }

    qDebug() << Q_FUNC_INFO;

    SysAudio::getInstance().setPartVolume("Center", value);
    ui->lbl_VolCenter->setText(QString::number(value));
}

void Widget::on_hSlider_Side_valueChanged(int value)
{
    if(_isAppLoading)
    {
        return;
    }

    qDebug() << Q_FUNC_INFO;

    SysAudio::getInstance().setPartVolume("Side", value);
    ui->lbl_VolSide->setText(QString::number(value));
}

Widget::~Widget()
{
    delete ui;
}


