#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T15:14:37
#
#-------------------------------------------------

QT          += core gui

TARGET       = customLineEdit
TEMPLATE     = lib

CONFIG      += qt warn_on release designer plugin

SOURCES     += customLineEdit.cpp       \
               customLineEditPlugin.cpp

HEADERS     += customLineEdit.h         \
               customLineEditPlugin.h

FORMS       += customLineEdit.ui

target.path  = ../libs

INSTALLS    += target
