#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    _isAppLoading(true)
{
    ui->setupUi(this);

    connect(ui->hsldChannelFront, SIGNAL(valueChanged(int)), ui->lbVolumeFront, SLOT(setNum(int)));
    //connect(ui->hsldChannelFront, SIGNAL(valueChanged(int)), this, SLOT(handleVolumeValueChanged(int, ui->lbChannelFront->text()))); // warning: Signature is not normalized. [clazy-connect-not-normalized]
    connect(ui->hsldChannelFront, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelFront->value(), ui->lbChannelFront->text()); });

    connect(ui->hsldChannelRear, SIGNAL(valueChanged(int)), ui->lbVolumeRear, SLOT(setNum(int)));
    connect(ui->hsldChannelRear, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelRear->value(), ui->lbChannelRear->text()); });

    connect(ui->hsldChannelSub, SIGNAL(valueChanged(int)), ui->lbVolumeSub, SLOT(setNum(int)));
    connect(ui->hsldChannelSub, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelSub->value(), ui->lbChannelSub->text()); });

    connect(ui->hsldChannelCenter, SIGNAL(valueChanged(int)), ui->lbVolumeCenter, SLOT(setNum(int)));
    connect(ui->hsldChannelCenter, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelCenter->value(), ui->lbChannelCenter->text()); });

    connect(ui->hsldChannelSide, SIGNAL(valueChanged(int)), ui->lbVolumeSide, SLOT(setNum(int)));
    connect(ui->hsldChannelSide, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelSide->value(), ui->lbChannelSide->text()); });

    SysAudio::getInstance().init();
}

void Widget::showEvent(QShowEvent *)
{
    int volumeFront = SysAudio::getInstance().getPartVolume(ui->lbChannelFront->text());
    ui->hsldChannelFront->setValue(volumeFront);
    ui->lbVolumeFront->setText(QString::number(volumeFront));

    int volumeRear = SysAudio::getInstance().getPartVolume(ui->lbChannelRear->text());
    ui->hsldChannelRear->setValue(volumeRear);
    ui->lbVolumeRear->setText(QString::number(volumeRear));

    int volumeSub = SysAudio::getInstance().getPartVolume(ui->lbChannelSub->text());
    ui->hsldChannelSub->setValue(volumeSub);
    ui->lbVolumeSub->setText(QString::number(volumeSub));

    int volumeCenter = SysAudio::getInstance().getPartVolume(ui->lbChannelCenter->text());
    ui->hsldChannelCenter->setValue(volumeCenter);
    ui->lbVolumeCenter->setText(QString::number(volumeCenter));

    int volumeSide = SysAudio::getInstance().getPartVolume(ui->lbChannelSide->text());
    ui->hsldChannelSide->setValue(volumeSide);
    ui->lbVolumeSide->setText(QString::number(volumeSide));

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

    if(!SysAudio::getInstance().setPartVolume(channelName, volume))
    {
        qDebug() << "ERROR!: Can't change volume for the" << channelName << "channel to" << volume << Q_FUNC_INFO;
        return;
    }

    qDebug() << "-> Volume changed for channel:" << channelName << volume;
}

Widget::~Widget()
{
    delete ui;
}


