#-------------------------------------------------
#
# Project created by QtCreator 2011-04-06T09:59:01
#
#-------------------------------------------------

QT           += core gui

target.path   = ./
sources.path  = ./

TARGET        = xmlgui
TEMPLATE      = app

INCLUDEPATH  += $(MonicelliDir)/test/xml/xmlgui  \
                -pthread -m64                    \
                $(ROOTSYS)/include               \
                $(QTDIR)/include/QtXml           \
                $(BOOSTINC)/include/boost/

LIBS         += -L$(BOOSTLIB)                    \
                -lboost_regex                    \
                -lboost_filesystem


INSTALLS     += target sources

LIBS         += -L$(XERCESCLIB)                  \
                 -lxerces-c

SOURCES      += main.cpp       \
                mainwindow.cpp \
    tabwidget.cpp \
    formpage1.cpp

HEADERS      += MessageTools.h \
                mainwindow.h   \
    tabwidget.h \
    formpage1.h

FORMS        += mainwindow.ui \
    tabwidget.ui \
    formpage1.ui
