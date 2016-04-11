#-------------------------------------------------
#
# Project created by QtCreator 2016-03-04T14:45:57
#
#-------------------------------------------------

QMAKE_CFLAGS_RELEASE   = $(CPLUSPLUSFLAGS)
QMAKE_CXXFLAGS_RELEASE = $(CPLUSPLUSFLAGS)

QT                    += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET                 = walden
TEMPLATE               = app


SOURCES               += main.cpp             \
                         src/mainwindow.cpp   \
                         src/qrootcanvas.cpp  \
                         src/extractor.cpp    \
                         src/treeitem.cpp     \
                         src/treemodel.cpp    \
                         src/indent.cpp

HEADERS               += inc/mainwindow.h     \
                         inc/qrootcanvas.h    \
                         inc/extractor.h      \
                         inc/helpers.h        \
                         inc/treeitem.h       \
                         inc/treemodel.h      \
                         inc/indent.h         \
                         inc/ANSIColors.h

INCLUDEPATH	      += ./inc                \
                         uiFiles              \
                         $(ROOTINC)

LIBS                  += -L$(ROOTLIB)         \
                         -lCore               \
                         -lRIO                \
                         -lNet		      \
                         -lHist 	      \
                         -lGraf 	      \
                         -lGraf3d	      \
                         -lGpad 	      \
                         -lTree 	      \
                         -lRint 	      \
                         -lPostscript	      \
                         -lMatrix	      \
                         -lPhysics	      \
                         -lMathCore	      \
                         -lThread	      \
                         -pthread	      \
                         -lm		      \
                         -ldl		      \
                         -rdynamic

FORMS                 += uiFiles/mainwindow.ui

MOC_DIR               += mocFiles
UI_DIR                += uiFiles
OBJECTS_DIR           += objFiles

