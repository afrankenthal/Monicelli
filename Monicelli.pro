#/*===============================================================================
# * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
# * 
# * Copyright (C) 2014 
# *
# * Authors:
# *
# * Dario Menasce      (INFN) 
# * Luigi Moroni       (INFN)
# * Jennifer Ngadiuba  (INFN)
# * Stefano Terzo      (INFN)
# * Lorenzo Uplegger   (FNAL)
# * Luigi Vigani       (INFN)
# *
# * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
# *
# * This program is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ================================================================================*/

include("$(ROOTSYS)/include/rootcint.pri")
QMAKE_CFLAGS_RELEASE   = -std=c++11
QMAKE_CXXFLAGS_RELEASE = -std=c++11

target.path          = ./
sources.path         = ./

HEADERS              = include/canvaswidget.h                    \
                       include/detectortabwidget.h               \
                       include/detectorw.h                       \
                       include/hnavigator.h                      \
                       include/maintabs.h                        \
                       include/mainwindow.h                      \
                       include/mdichild.h                        \
                       include/newNonStandardTabDialog.h         \
                       include/nonstandardpitch.h                \
                       include/remNonStandardTabDialog.h         \
                       include/roctabwidget.h                    \
                       include/rocwidget.h                       \
                       include/serialIdDialog.h                  \
                       include/stationgb.h                       \
                       include/stationtabwidget.h                \
                       include/xmlcreationdialog.h               \
                       include/xmleditor.h                       \
                       include/aligner.h                         \
                       include/ANSIColors.h                      \
                       include/beamSimulator.h                   \
                       include/calibrationLoader.h               \
                       include/clusterizer.h                     \
                       include/Detector.h                        \
                       include/DOMTreeErrorReporter.hpp          \
                       include/Event.h                           \
                       include/EventHeader.h                     \
                       include/fileEater.h                       \
                       include/fitter.h                          \
                       include/HManager.h                        \
                       include/hTreeBrowser.h                    \
                       include/Geometry.h                        \
                       include/geometryLoader.h                  \
                       include/mdisubwindow.h                    \
                       include/MessageTools.h                    \
                       include/msg.h                             \
                       include/ROC.h                             \
                       include/trackFinder.h                     \
                       include/trackFitter.h                     \
                       include/verbosity.h                       \
                       include/xmlParser.h                       \
                       include/process.h                         \
                       include/threader.h                        \
                       include/subProcess.h                      \
                       include/histogramPaths.h                  \
                       include/multiProcess.h                    \
                       include/subMultiProcess.h                 \
                       include/GeometryParameters.h              \
                       plugins/customCheckBox/customCheckBox.h   \
                       plugins/customComboBox/customComboBox.h   \
                       plugins/customLineEdit/customLineEdit.h   \
                       plugins/customSpinBox/customSpinBox.h     \
                       plugins/customTableView/customTableView.h \
                       tmp/EventDict.h                           \
                       tmp/EventHeaderDict.h                     \                      
                       include/qrootcanvas.h

                                                                     
SOURCES              = main.cpp                                  \
                       src/canvaswidget.cpp                      \
                       src/detectortabwidget.cpp                 \
                       src/detectorw.cpp                         \
                       src/hnavigator.cpp                        \
                       src/mainwindow.cpp                        \
                       src/maintabs.cpp                          \
                       src/mdichild.cpp                          \	 		
                       src/newNonStandardTabDialog.cpp           \
                       src/nonstandardpitch.cpp                  \
                       src/remNonStandardTabDialog.cpp           \
                       src/roctabwidget.cpp                      \
                       src/rocwidget.cpp                         \
                       src/serialIdDialog.cpp                    \
                       src/stationgb.cpp                         \
                       src/stationtabwidget.cpp                  \
                       src/xmlcreationdialog.cpp                 \
                       src/xmleditor.cpp                         \
                       src/aligner.cpp                           \
                       src/beamSimulator.cpp                     \
                       src/calibrationLoader.cpp                 \
                       src/clusterizer.cpp                       \
                       src/Detector.cpp                          \
                       src/DOMTreeErrorReporter.cpp              \
                       src/Event.cpp                             \
                       src/EventHeader.cpp                       \
                       src/fileEater.cpp                         \
                       src/fitter.cpp                            \
                       src/Geometry.cpp                          \
                       src/geometryLoader.cpp                    \
                       src/mdisubwindow.cpp                      \
                       src/HManager.cpp                          \
                       src/hTreeBrowser.cpp                      \
                       src/ROC.cpp                               \
                       src/trackFinder.cpp                       \
                       src/trackFitter.cpp                       \
                       src/xmlParser.cpp                         \
                       src/threader.cpp                          \
                       src/GeometryParameters.cpp                \
                       tmp/EventDict.C                           \
                       tmp/EventHeaderDict.C                     \                      
                       src/qrootcanvas.cpp

RESOURCES            = Monicelli.qrc

sources.files        = $$SOURCES                                 \
                       $$HEADERS                                 \
                       $$RESOURCES                               \
                       $$FORMS                                   \
                       Monicelli.pro                             \
                       images                                  
                                                               
DEFINES              = USE_QT

INCLUDEPATH	    += ./include				 \
		       -pthread $(ROOTSYS)/include		 \
		       $(QTDIR)/include/QtXml			 \
		       $(XERCESCINC)				 \
		       $(BOOSTINC)

INCLUDEPATH	    += plugins/customCheckBox			 \
		       plugins/customComboBox			 \
		       plugins/customLineEdit			 \
		       plugins/customSpinBox			 \
		       plugins/customTextEdit			 \
		       plugins/customTableView
		       
LIBS                += -L$(ROOTSYS)/lib                          \
               

LIBS                += -L$(BOOSTLIB)                             \
                       -lboost_regex                             \
                       -lboost_system                            \
                       -lboost_filesystem

LIBS                += -Lplugins/libs                            \
                       -lcustomCheckBox                          \
                       -lcustomComboBox                          \
                       -lcustomLineEdit                          \
                       -lcustomSpinBox                           \
                       -lcustomTextEdit                          \
                       -lcustomTableView

LIBS                += -L$(XERCESCLIB)                           \
                       -lxerces-c

INSTALLS            += target sources

header.depends       = include/EventHeader.h

header.target        = tmp/EventHeaderDict.C
header.commands      = rootcint -f tmp/EventHeaderDict.C         \
				-c include/EventHeader.h+ &&     \
		       ls -la tmp/*.pcm &&                       \
		       cp tmp/*.pcm .

trees.depends        = include/Event.h                           \
                       include/Geometry.h                        \
                       include/Detector.h                        \
                       include/ROC.h

trees.target         = tmp/EventDict.C
trees.commands       = rootcint -f tmp/EventDict.C               \
                                -c include/Event.h+              \
                                   include/Geometry.h+           \
                                   include/Detector.h+           \
                                   include/ROC.h+ &&             \
		       ls -la tmp/*.pcm &&                       \
		       cp tmp/*.pcm .

QMAKE_EXTRA_TARGETS += trees
QMAKE_EXTRA_TARGETS += header

FORMS               += ui/canvaswidget.ui                        \
                       ui/maintabs.ui                            \
                       ui/hnavigator.ui                          \
                       ui/xmleditor.ui                           \
                       ui/stationtabwidget.ui                    \
                       ui/detectortabwidget.ui                   \
                       ui/roctabwidget.ui                        \
                       ui/detectorw.ui                           \
                       ui/rocwidget.ui                           \
                       ui/stationgb.ui                           \
                       ui/xmlcreationdialog.ui                   \
                       ui/nonstandardpitch.ui                    \
                       ui/newNonStandardTabDialog.ui             \
                       ui/remNonStandardTabDialog.ui             \
                       ui/serialIdDialog.ui                      \
                       ui/geometryParameters.ui

SUBDIRS             += plugins/customComboBox                    \
                       plugins/customLineEdit                    \
                       plugins/customSpinBox                     \
                       plugins/customCheckBox                    \
                       plugins/customTableView

MOC_DIR             += mocFiles
UI_DIR              += uiFiles
OBJECTS_DIR         += objFiles

QT                  *= xml

DEPENDPATH          += . src include

extraclean.commands  = rm -rf Makefile *.pcm
distclean.depends    = extraclean
QMAKE_EXTRA_TARGETS += distclean extraclean
