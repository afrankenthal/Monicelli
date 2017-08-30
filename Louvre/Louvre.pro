#-------------------------------------------------
#
# Project created by QtCreator 2016-05-25T17:01:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET    = Louvre
TEMPLATE  = app


SOURCES  += src/main.cpp           \
            src/mainwindow.cpp     \
            src/manipulatepath.cpp

HEADERS  += inc/mainwindow.h       \
            inc/dfilesystemmodel.h \
            inc/ANSIColors.h       \
            inc/MessageTools.h     \
            inc/manipulatepath.h

FORMS    += ui/mainwindow.ui

DISTFILES +=

RESOURCES += \
    resources.qrc
