#-------------------------------------------------
#
# Project created by QtCreator 2016-06-07T21:05:15
#
#-------------------------------------------------

QT       += core gui
QT       += network
CONFIG       += console
CONFIG +=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_MAC_SDK = macosx10.13

TARGET = appWindow
TEMPLATE = app
#LIBS += -L/usr/local/opt/boost/lib -lboost_thread-mt -lboost_filesystem -lboost_system
LIBS += -L/usr/local/Cellar/boost160/1.60.0/lib -lboost_thread-mt -lboost_filesystem -lboost_system

boostinclude="/usr/local/Cellar/boost160/1.60.0/include/"
INCLUDEPATH+=$$boostinclude

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
            generalheader.h

FORMS    += mainwindow.ui
