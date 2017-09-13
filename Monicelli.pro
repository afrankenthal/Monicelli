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

# include("$(ROOTINC)/rootcint.pri")

QMAKE_CFLAGS_RELEASE   = $(CPLUSPLUSFLAGS)
QMAKE_CXXFLAGS_RELEASE = $(CPLUSPLUSFLAGS)

target.path          = ./
sources.path         = ./

QT                  += widgets

HEADERS              = include/aligner.h                         	      		   \
                       include/ANSIColors.h                      	      		   \
                       include/beamSimulator.h                   	      		   \
                       include/calibrationLoader.h               	      		   \
                       include/canvaswidget.h                    	      		   \
                       include/clusterizer.h                     	      		   \
                       include/Detector.h                        	      		   \
                       include/detectortabwidget.h               	      		   \
                       include/detectorw.h                       	      		   \
                       include/DOMTreeErrorReporter.hpp          	      		   \
                       include/Event.h                           	      		   \
                       include/EventHeader.h                     	      		   \
                       include/fileEater.h                       	      		   \
                       include/fitter.h                          	      		   \
                       include/Geometry.h                        	      		   \
                       include/geometryLoader.h                  	      		   \
                       include/GeometryParameters.h              	      		   \
                       include/histogramPaths.h                  	      		   \
                       include/HManager.h                        	      		   \
                       include/hnavigator.h                      	      		   \
                       include/hTreeBrowser.h                    	      		   \
                       include/KalmanPlaneInfo.h                 	      		   \
                       include/maintabs.h                        	      		   \
                       include/mainwindow.h                      	      		   \
                       include/mdichild.h                        	      		   \
                       include/mdisubwindow.h                    	      		   \
                       include/MessageTools.h                    	      		   \
                       include/msg.h                             	      		   \
                       include/multiProcess.h                    	      		   \
                       include/newNonStandardTabDialog.h         	      		   \
                       include/nonstandardpitch.h                	      		   \
                       include/process.h                         	      		   \
                       include/qrootcanvas.h                     	      		   \
                       include/remNonStandardTabDialog.h         	      		   \
                       include/ROC.h                             	      		   \
                       include/roctabwidget.h                    	      		   \
                       include/rocwidget.h                       	      		   \
                       include/serialIdDialog.h                  	      		   \
                       include/subProcess.h                      	      		   \
                       include/subMultiProcess.h                 	      		   \
                       include/stationgb.h                       	      		   \
                       include/stationtabwidget.h                	      		   \
                       include/threader.h                        	      		   \
                       include/trackFinder.h                     	      		   \
                       include/trackFitter.h                     	      		   \
                       include/verbosity.h                       	      		   \
                       include/xmlcreationdialog.h               	      		   \
                       include/xmleditor.h                       	      		   \
                       include/xmlParser.h                       	      		   \
                       plugins/customCheckBox/customCheckBox.h   	      		   \
                       plugins/customComboBox/customComboBox.h   	      		   \
                       plugins/customLineEdit/customLineEdit.h   	      		   \
                       plugins/customSpinBox/customSpinBox.h     	      		   \
                       plugins/customTableView/customTableView.h \
    include/settingsmanager.h

SOURCES              = main.cpp                                  	      		   \
                       src/aligner.cpp                           	      		   \
                       src/beamSimulator.cpp                     	      		   \
                       src/calibrationLoader.cpp                 	      		   \
                       src/canvaswidget.cpp                      	      		   \
                       src/clusterizer.cpp                       	      		   \
                       src/Detector.cpp                          	      		   \
                       src/detectortabwidget.cpp                 	      		   \
                       src/detectorw.cpp                         	      		   \
                       src/DOMTreeErrorReporter.cpp              	      		   \
                       src/Event.cpp                             	      		   \
                       src/EventHeader.cpp                       	      		   \
                       src/fileEater.cpp                         	      		   \
                       src/fitter.cpp                            	      		   \
                       src/Geometry.cpp                          	      		   \
                       src/geometryLoader.cpp                    	      		   \
                       src/GeometryParameters.cpp                	      		   \
                       src/HManager.cpp                          	      		   \
                       src/hnavigator.cpp                        	      		   \
                       src/hTreeBrowser.cpp                      	      		   \
                       src/KalmanPlaneInfo.cpp                   	      		   \
                       src/mainwindow.cpp                        	      		   \
                       src/maintabs.cpp                          	      		   \
                       src/mdichild.cpp                          	                   \
                       src/mdisubwindow.cpp                      	      		   \
                       src/newNonStandardTabDialog.cpp           	      		   \
                       src/nonstandardpitch.cpp                  	      		   \
                       src/remNonStandardTabDialog.cpp           	      		   \
                       src/ROC.cpp                               	      		   \
                       src/roctabwidget.cpp                      	      		   \
                       src/rocwidget.cpp                         	      		   \
                       src/serialIdDialog.cpp                    	      		   \
                       src/stationgb.cpp                         	      		   \
                       src/stationtabwidget.cpp                  	      		   \
                       src/qrootcanvas.cpp                       	      		   \
                       src/trackFinder.cpp                       	      		   \
                       src/trackFitter.cpp                       	      		   \
                       src/threader.cpp                          	      		   \
                       src/xmlcreationdialog.cpp                 	      		   \
                       src/xmleditor.cpp                         	      		   \
                       src/xmlParser.cpp                         	      		   \
                       tmp/EventDict.C                           	      		   \
                       tmp/EventHeaderDict.C                                               \
                       plugins/customCheckBox/customCheckBox.cpp                           \
                       plugins/customComboBox/customComboBox.cpp                           \
                       plugins/customLineEdit/customLineEdit.cpp                           \
                       plugins/customSpinBox/customSpinBox.cpp                             \
                       plugins/customTableView/customTableView.cpp \
    src/settingsmanager.cpp


RESOURCES            = Monicelli.qrc

sources.files        = $$SOURCES                                 	      		   \
                       $$HEADERS                                 	      		   \
                       $$RESOURCES                               	      		   \
                       $$FORMS                                   	      		   \
                       Monicelli.pro                             	      		   \
                       images                                  
                                                               
DEFINES              = USE_QT

INCLUDEPATH	    += ./include				 	      		   \
                       ./uiFiles                                                           \
		       -pthread $(ROOTINC)   		         	      		   \
		       $(QTDIR)/include/QtXml			 	      		   \
		       $(XERCESCINC)				 	      		   \
		       $(BOOSTINC)

INCLUDEPATH	    += plugins/customCheckBox			 	      		   \
		       plugins/customComboBox			 	      		   \
		       plugins/customLineEdit			 	      		   \
		       plugins/customSpinBox			 	      		   \
		       plugins/customTextEdit			 	      		   \
		       plugins/customTableView
		       
LIBS                += -L$(BOOSTLIB)                             	      		   \
                       -lboost_regex                             	      		   \
                       -lboost_system                            	      		   \
                       -lboost_filesystem

LIBS                += -L$(ROOTLIB)                              	      		   \
                       -lCore                                                              \
                       -lRIO                                                               \
                       -lNet                                                               \
                       -lHist                                                              \
                       -lGraf                                                              \
                       -lGraf3d                                                            \
                       -lGpad                                                              \
                       -lTree                                                              \
                       -lRint                                                              \
                       -lPostscript                                                        \
                       -lMatrix                                                            \
                       -lPhysics                                                           \
                       -lMathCore                                                          \
                       -lThread

LIBS                += -Lplugins/libs                            	      		   \
                       -lcustomCheckBox                          	      		   \
                       -lcustomComboBox                          	      		   \
                       -lcustomLineEdit                          	      		   \
                       -lcustomSpinBox                           	      		   \
                       -lcustomTextEdit                          	      		   \
                       -lcustomTableView

LIBS                += -L$(XERCESCLIB)                           	      		   \
                       -lxerces-c

LIBS                += -L$(QTDIR)                                                          \
                       -lQt5Widgets                                                        \
                       -lQt5Gui                                                            \
                       -lQt5Xml                                                            \
                       -lQt5Core                                                           \
                       -lQt5OpenGL
                       
INSTALLS            += target sources

mkdirtmp.target      = $$OUT_PWD/tmp
mkdirtmp.commands    = $(MKDIR) $$OUT_PWD/tmp

header.depends       = include/EventHeader.h

header.target        = tmp/EventHeaderDict.C

ROOTVERSION          = $$(ROOTVER)

contains(ROOTVERSION, "FIVE") {
 message("'[1;33mSetting up Makefile for ROOT5[0;0m")  
 header.commands     = @echo "'[1;33m------ ROOT5 header dictionary[0;0m'" &&    \
                       rootcint -f tmp/EventHeaderDict.C                                   \
                                -c include/EventHeader.h+                       &&         \
                       @echo "'[1;33m------ ROOT5 header dictionary done[0;0m'"
} else {  
 message("[1;33mSetting up Makefile for ROOT6[0;0m")
 header.commands     = @echo "'[1;33m------ ROOT6 header dictionary[0;0m'" &&    \
                       rootcint -f tmp/EventHeaderDict.C      		   	     	   \
                                -c include/EventHeader.h+     		        &&         \
                       cp tmp/*.pcm .                                           
}

trees.depends        = mkdirtmp                                                            \
                       include/Event.h                        		   	     	   \
                       include/Geometry.h                     		   	     	   \
                       include/Detector.h                     		   	     	   \
                       include/ROC.h

trees.target         = tmp/EventDict.C

contains(ROOTVERSION, "FIVE")                                                              \
{
 trees.commands      = @echo "'[1;33m------ ROOT5 full dictionary -[0;0m'" &&    \
                       rootcint -f tmp/EventDict.C               	      		   \
                                -c include/Event.h+              	      		   \
                                   include/Geometry.h+           	      		   \
                                   include/Detector.h+           	      		   \
                                   include/ROC.h+
} else {
 trees.commands      = @echo "'[1;33m------ ROOT6 full dictionary -[0;0m'" &&    \
                       rootcint -f tmp/EventDict.C            		   	     	   \
                                -c include/Event.h+           		   	     	   \
                                   include/Geometry.h+        		   	     	   \
                                   include/Detector.h+        		   	     	   \
                                   include/ROC.h+             		   	     &&    \
                                   cp tmp/*.pcm .
}

QMAKE_EXTRA_TARGETS += mkdirtmp
QMAKE_EXTRA_TARGETS += trees
QMAKE_EXTRA_TARGETS += header

FORMS               += ui/canvaswidget.ui                        			   \
                       ui/maintabs.ui                            			   \
                       ui/hnavigator.ui                          			   \
                       ui/xmleditor.ui                           			   \
                       ui/stationtabwidget.ui                    			   \
                       ui/detectortabwidget.ui                   			   \
                       ui/roctabwidget.ui                        			   \
                       ui/detectorw.ui                           			   \
                       ui/rocwidget.ui                           			   \
                       ui/stationgb.ui                           			   \
                       ui/xmlcreationdialog.ui                   			   \
                       ui/nonstandardpitch.ui                    			   \
                       ui/newNonStandardTabDialog.ui             			   \
                       ui/remNonStandardTabDialog.ui             			   \
                       ui/serialIdDialog.ui                      			   \
                       ui/geometryParameters.ui

SUBDIRS             += plugins/customComboBox                    			   \
                       plugins/customLineEdit                    			   \
                       plugins/customSpinBox                     			   \
                       plugins/customCheckBox                    			   \
                       plugins/customTableView

MOC_DIR             += mocFiles
UI_DIR              += uiFiles
OBJECTS_DIR         += objFiles

QT                  *= xml

DEPENDPATH          += . src include

extraclean.commands  = rm -rf Makefile                           			   \
                       *.pcm                                     			   \
                       tmp/*                                     			   \
                       Express/MonicelliExpress                  			   \
		       plugins/*/moc_*.cpp                       			   \
		       plugins/*/ui_*.h                          			   \
                       MakefileExpress

distclean.depends    = extraclean

QMAKE_EXTRA_TARGETS += distclean extraclean 
