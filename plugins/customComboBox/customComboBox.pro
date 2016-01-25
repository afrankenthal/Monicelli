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

QMAKE_CFLAGS_RELEASE   = -std=c++0x
QMAKE_CXXFLAGS_RELEASE = -std=c++0x

QT          += core gui

TARGET       = customComboBox
TEMPLATE     = lib

INCLUDEPATH += ../../include

CONFIG      += qt warn_on release designer plugin

SOURCES     += customComboBox.cpp       \
               customComboBoxPlugin.cpp

HEADERS     += customComboBox.h         \
               customComboBoxPlugin.h

FORMS       += customComboBox.ui

target.path  = ../libs

INSTALLS    += target
