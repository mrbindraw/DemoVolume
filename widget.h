#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QMessageBox>

#include "sysaudio.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void handleVolumeValueChanged(int volume, const QString &channelName);

private:
    Ui::Widget *ui;

protected:
    void showEvent(QShowEvent *);
};

#endif // WIDGET_H
