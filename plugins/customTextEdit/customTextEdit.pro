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

QT          += core gui

TARGET       = customTextEdit
TEMPLATE     = lib

INCLUDEPATH += ../../include

CONFIG      += qt warn_on release designer plugin

SOURCES     += customTextEdit.cpp       \
               customTextEditPlugin.cpp

HEADERS     += customTextEdit.h         \
               customTextEditPlugin.h

FORMS       += customTextEdit.ui

target.path  = ../libs

INSTALLS    += target
