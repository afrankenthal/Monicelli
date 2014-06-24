#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T11:24:46
#
#-------------------------------------------------

QT          += core gui

TARGET       = useCustomComboBox
TEMPLATE     = app

INCLUDEPATH += ../customComboBox \
               ../customLineEdit

SOURCES     += main.cpp          \
               mainwindow.cpp

HEADERS     += mainwindow.h      \
               ../customComboBox/customComboBox.h  \
               ../customLineEdit/customLineEdit.h
               
LIBS        += -L../plugins      \
               -lcustomComboBox  \
               -lcustomLineEdit

FORMS       += mainwindow.ui
