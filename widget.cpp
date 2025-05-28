#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    SysAudio::getInstance().init();

    // KSNODETYPE_SUM https://learn.microsoft.com/en-us/windows-hardware/drivers/audio/ksnodetype-sum
    if(!SysAudio::getInstance().isPartExist("Sum"))
    {
        QMessageBox::critical(this, "ERROR!", "Make sure the default playback device is 'Speakers' (mmsys.cpl), otherwise\nthe audio playback device is not configured or does not support the surround sound system.");
        return;
    }

    connect(ui->hsldChannelFront, SIGNAL(valueChanged(int)), ui->lbVolumeFront, SLOT(setNum(int)));
    connect(ui->hsldChannelFront, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelFront->value(), ui->lbChannelFront->text()); });

    connect(ui->hsldChannelRear, SIGNAL(valueChanged(int)), ui->lbVolumeRear, SLOT(setNum(int)));
    connect(ui->hsldChannelRear, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelRear->value(), ui->lbChannelRear->text()); });

    connect(ui->hsldChannelSub, SIGNAL(valueChanged(int)), ui->lbVolumeSub, SLOT(setNum(int)));
    connect(ui->hsldChannelSub, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelSub->value(), ui->lbChannelSub->text()); });

    connect(ui->hsldChannelCenter, SIGNAL(valueChanged(int)), ui->lbVolumeCenter, SLOT(setNum(int)));
    connect(ui->hsldChannelCenter, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelCenter->value(), ui->lbChannelCenter->text()); });

    connect(ui->hsldChannelSide, SIGNAL(valueChanged(int)), ui->lbVolumeSide, SLOT(setNum(int)));
    connect(ui->hsldChannelSide, &QSlider::valueChanged, this, [this](){ handleVolumeValueChanged(ui->hsldChannelSide->value(), ui->lbChannelSide->text()); });
}

void Widget::showEvent(QShowEvent *)
{
    int volumeFront = SysAudio::getInstance().getPartVolume(ui->lbChannelFront->text());
    ui->hsldChannelFront->setValue(volumeFront);

    int volumeRear = SysAudio::getInstance().getPartVolume(ui->lbChannelRear->text());
    ui->hsldChannelRear->setValue(volumeRear);

    int volumeSub = SysAudio::getInstance().getPartVolume(ui->lbChannelSub->text());
    ui->hsldChannelSub->setValue(volumeSub);

    int volumeCenter = SysAudio::getInstance().getPartVolume(ui->lbChannelCenter->text());
    ui->hsldChannelCenter->setValue(volumeCenter);

    int volumeSide = SysAudio::getInstance().getPartVolume(ui->lbChannelSide->text());
    ui->hsldChannelSide->setValue(volumeSide);
}

void Widget::handleVolumeValueChanged(int volume, const QString &channelName)
{
    if(!SysAudio::getInstance().setPartVolume(channelName, volume))
    {
        return;
    }

    qDebug() << "-> Volume changed for channel:" << channelName << volume;
}

Widget::~Widget()
{
    delete ui;
}


