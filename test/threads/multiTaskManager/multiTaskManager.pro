#-------------------------------------------------
#
# Project created by QtCreator 2011-03-10T10:18:54
#
#-------------------------------------------------

QT          += core gui

TARGET       = multiTaskManager
TEMPLATE     = app


SOURCES     += main.cpp        \
               mainwindow.cpp  \
               computation.cpp \
               parser.cpp      \
               builder.cpp     \
               aligner.cpp     \
               cProcess.cpp \
    util.cpp

HEADERS     += mainwindow.h    \
               computation.h   \
               parser.h        \
               cProcess.h      \
               builder.h       \
               aligner.h       \
               ANSIColors.h \
    parser.h \
    util.h

INCLUDEPATH += $(BOOSTINC)

LIBS        += -L$(BOOSTLIB) -lboost_regex

FORMS       += mainwindow.ui
