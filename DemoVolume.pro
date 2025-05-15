#-------------------------------------------------
#
# Project created by QtCreator 2016-08-14T15:28:53
#
#-------------------------------------------------

QT       += core gui

win32:CONFIG(debug, debug|release): QT += testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DemoVolume
TEMPLATE = app

# WINDOWSSDKDIR = C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0
INCLUDEPATH += "$WINDOWSSDKDIR/um/x64"
LIBS += Ole32.lib Winmm.lib


SOURCES += main.cpp\
        sysaudio.cpp \
        widget.cpp

HEADERS  += widget.h \
    sysaudio.h

FORMS    += widget.ui
