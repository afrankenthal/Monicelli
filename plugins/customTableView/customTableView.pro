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

QMAKE_CFLAGS_RELEASE   = -std=c++11
QMAKE_CXXFLAGS_RELEASE = -std=c++11

QT                    += core gui designer

TARGET                 = customTableView
TEMPLATE               = lib

INCLUDEPATH           += ../../include

CONFIG                += qt warn_on release plugin

SOURCES               += customTableView.cpp                \
                         customTableViewPlugin.cpp

HEADERS               += customTableView.h                  \
                         customTableViewPlugin.h

FORMS                 += customTableView.ui

target.path            = ../libs

INSTALLS              += target

extraclean.commands    = rm -rf Makefile                    \
                         plugins/customTableView/moc_*      \
                         plugins/customTableView/Makefile   \
                         plugins/customTableView/ui_*       \

distclean.depends      = extraclean
