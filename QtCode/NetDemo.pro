#-------------------------------------------------
#
# Project created by QtCreator 2015-08-15T17:22:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetDemo
TEMPLATE = app
DESTDIR = ../bin/


SOURCES += main.cpp\
        mainwindow.cpp \
    dlgdownloadinfo.cpp

INCLUDEPATH += ../include/

HEADERS  += mainwindow.h \
        ../include/NetDEVSDK.h \
    dlgdownloadinfo.h

FORMS    += mainwindow.ui \
    dlgdownloadinfo.ui

DEFINES += LINUX

LIBS+=-L../lib/

LIBS+=-lNetDEVSDK




