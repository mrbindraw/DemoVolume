#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    _isAppLoading(true)
{
    ui->setupUi(this);

    connect(ui->hSlider_Front, SIGNAL(valueChanged(int)), ui->lbl_VolFront, SLOT(setNum(int)));
    //connect(ui->hSlider_Front, SIGNAL(valueChanged(int)), this, SLOT(handleVolumeValueChanged(int, ui->lbl_Front->text()))); // warning: Signature is not normalized. [clazy-connect-not-normalized]
    connect(ui->hSlider_Front, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hSlider_Front->value(), ui->lbl_Front->text()); });

    connect(ui->hSlider_Rear, SIGNAL(valueChanged(int)), ui->lbl_VolRear, SLOT(setNum(int)));
    connect(ui->hSlider_Rear, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hSlider_Rear->value(), ui->lbl_Rear->text()); });

    connect(ui->hSlider_Sub, SIGNAL(valueChanged(int)), ui->lbl_VolSub, SLOT(setNum(int)));
    connect(ui->hSlider_Sub, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hSlider_Sub->value(), ui->lbl_Sub->text()); });

    connect(ui->hSlider_Center, SIGNAL(valueChanged(int)), ui->lbl_VolCenter, SLOT(setNum(int)));
    connect(ui->hSlider_Center, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hSlider_Center->value(), ui->lbl_Center->text()); });

    connect(ui->hSlider_Side, SIGNAL(valueChanged(int)), ui->lbl_VolSide, SLOT(setNum(int)));
    connect(ui->hSlider_Side, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hSlider_Side->value(), ui->lbl_Side->text()); });

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

void Widget::handleVolumeValueChanged(int volume, const QString &channelName)
{
    if(_isAppLoading)
    {
        return;
    }

    qDebug() << Q_FUNC_INFO;
    SysAudio::getInstance().setPartVolume(channelName, volume);
}

Widget::~Widget()
{
    delete ui;
}


