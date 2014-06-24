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

TARGET       = customCheckBox
TEMPLATE     = lib

INCLUDEPATH += ../../include

CONFIG      += qt warn_on release designer plugin

SOURCES     += customCheckBox.cpp       \
               customCheckBoxPlugin.cpp

HEADERS     += customCheckBox.h         \
               customCheckBoxPlugin.h

FORMS       += customCheckBox.ui

target.path  = ../libs

INSTALLS    += target
