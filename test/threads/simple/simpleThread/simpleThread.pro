#-------------------------------------------------
#
# Project created by QtCreator 2011-10-18T10:38:46
#
#-------------------------------------------------

QT          += core gui

TARGET       = simpleThread
TEMPLATE     = app

INCLUDEPATH += ../../../../include

SOURCES     += main.cpp         \
               MainWindow.cpp   \
               threader.cpp \
    processB.cpp \
    processA.cpp

HEADERS     += MainWindow.h     \
               threaderClass.h  \
               MessageTools.h \
    processA.h \
    process.h \
    processB.h

FORMS       += MainWindow.ui
