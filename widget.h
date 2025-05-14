#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QtTest/QTest>

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
    void on_hSlider_Front_valueChanged(int value);

    void on_hSlider_Rear_valueChanged(int value);

    void on_hSlider_Sub_valueChanged(int value);

    void on_hSlider_Center_valueChanged(int value);

    void on_hSlider_Side_valueChanged(int value);

private:
    Ui::Widget *ui;

    bool _isAppLoading;

protected:
    void showEvent(QShowEvent *);
};

#endif // WIDGET_H
