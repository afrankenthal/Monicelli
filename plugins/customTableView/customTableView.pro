#-------------------------------------------------
#
# Project created by QtCreator 2011-05-13T13:41:32
#
# To create this custom widget:
#
# > qmake
# > make
# > make install
#
#-------------------------------------------------

QMAKE_CFLAGS_RELEASE = -g
QMAKE_CXXFLAGS_RELEASE = -g

QT          += core gui

TARGET       = customTableView
TEMPLATE     = lib

INCLUDEPATH += ../../include

CONFIG      += qt warn_on release designer plugin

SOURCES     += customTableView.cpp       \
               customTableViewPlugin.cpp

HEADERS     += customTableView.h         \
               customTableViewPlugin.h

FORMS       += customTableView.ui

target.path  = ../libs

INSTALLS    += target
