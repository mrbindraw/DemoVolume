#-------------------------------------------------
#
# Project created by QtCreator 2016-08-14T15:28:53
#
#-------------------------------------------------

QT       += core gui multimedia testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DemoVolume
TEMPLATE = app

INCLUDEPATH += "$WINDOWSSDKDIR/Lib/winv6.3/um/x64"
LIBS += Ole32.lib Winmm.lib


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui
