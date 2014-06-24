#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T09:49:47
#
#-------------------------------------------------

QT         += core gui

TARGET      = customComboBox
TEMPLATE    = lib

CONFIG     += qt warn_on release designer plugin

SOURCES    += customComboBox.cpp       \
              customComboBoxPlugin.cpp

HEADERS    += customComboBox.h         \
              customComboBoxPlugin.h

FORMS      += customComboBox.ui        \
              customcombobox.ui

target.path = ../libs

INSTALLS   += target
