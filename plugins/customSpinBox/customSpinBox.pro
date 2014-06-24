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

QT          += core gui

TARGET       = customSpinBox
TEMPLATE     = lib

INCLUDEPATH += ../../include

CONFIG      += qt warn_on release designer plugin

SOURCES     += customSpinBox.cpp       \
               customSpinBoxPlugin.cpp

HEADERS     += customSpinBox.h         \
               customSpinBoxPlugin.h

FORMS       += customSpinBox.ui

target.path  = ../libs

INSTALLS    += target
