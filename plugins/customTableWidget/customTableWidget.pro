#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T15:14:37
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

TARGET       = customTableWidget
TEMPLATE     = lib

INCLUDEPATH += ../../include

CONFIG      += qt warn_on release designer plugin

SOURCES     += customTableWidget.cpp       \
               customTableWidgetPlugin.cpp

HEADERS     += customTableWidget.h         \
               customTableWidgetPlugin.h

FORMS       += customTableWidget.ui

target.path  = ../libs

INSTALLS    += target
