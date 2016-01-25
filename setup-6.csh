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


# The following script sets up the environment to run qt and Monicelli on local machines.
# Once you have fetched this copy from the cvs repository, make a copy (name it something
# like mySetupQt.csh) and modify the copy to suit your needs. Please do NOT put bac in cvs  
# a modified copy of the original setupQt.csh if you have by mistake accidentally edited it.
#
# Before running Monicelli, source YOUR own, PRIVATE **copy** of this file:

# scl enable devtoolset-2 tcsh

alias do       'make -f MakefileAll '
alias cleanAll 'make -f MakefileAll clean'

#setenv PATH            /opt/rh/python27/root/usr/bin$:${PATH}
#setenv LD_LIBRARY_PATH /opt/rh/python27/root/usr/lib64
#setenv MANPATH         /opt/rh/python27/root/usr/share/man
#setenv XDG_DATA_DIRS   /opt/rh/python27/root/usr/share
#setenv PKG_CONFIG_PATH /opt/rh/python27/root/usr/lib64/pkgconfig

#setenv BASEDATADIR     /user/gr1/e831/menasce/MonicelliNew/data/2015_10_October
setenv BASEDATADIR     /data/TestBeam/2015_10_October

#===============   Personalized configuration on this computer   ==============================================
if(${HOSTNAME} == "rulinux04.dhcp.fnal.gov") then

  #===== Local directories
  setenv MonicelliDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   /home/uplegger/Programming/MonicelliOutput
  setenv Monicelli_XML_Dir         ${BASEDATADIR}/Geometries
  
  #===== Location of the ROOT components
  setenv ROOTSYS		   /opt/local/root

  #===== Location of the Qt components
  setenv QTDIR  		   /opt/local/qt
  setenv QTCREATORDIR		   /opt/local/qtcreator

  #===== Location of the BOOST components
  setenv BOOSTINC		   /usr/include
  setenv BOOSTLIB		   /usr/lib64

  #===== Location of the XERCES components
  setenv XERCESCINC		   /opt/local/xerces/include
  setenv XERCESCLIB		   /opt/local/xerces/lib
  
  setenv LD_LIBRARY_PATH           /usr/lib64
  
else if(${HOSTNAME} == "hal9000.mib.infn.it") then

  echo "In hal9000, newest version"
  setenv PATH /opt/rh/devtoolset-2/root/usr/bin:/bin:/usr/bin:/usr/local/bin:/usr/printmi/bin
  #===== Local directories
  setenv MonicelliDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   ${BASEDATADIR}/MonicelliOutput
  setenv Monicelli_XML_Dir	   ${BASEDATADIR}/Geometries
  
  #===== Location of the ROOT components
  setenv ROOTSYS		   /user/service/root-6.04.10/

  #===== Location of the Qt components
  setenv QTDIR  		   /usr/local/Trolltech/Qt-4.8.5
  setenv QTCREATORDIR		   /opt/local/qtcreator-2.7.1/bin

  #===== Location of the BOOST components
  setenv BOOSTINC		   /usr/local/include/boost
  setenv BOOSTLIB		   /usr/local/lib

  #===== Location of the XERCES components
  setenv XERCESCINC		   /opt/local/xerces/include
  setenv XERCESCLIB		   /opt/local/xerces/lib
  
#  setenv LD_LIBRARY_PATH	   
  
  alias qtcreator ${QTCREATORDIR}/qtcreator
    
endif

#===== Final PATH definitions
setenv PATH		$ROOTSYS/bin:$QTDIR/bin:$QTCREATORDIR/bin:$PATH
setenv LD_LIBRARY_PATH  $ROOTSYS/lib:${BOOSTLIB}:$QTDIR/lib:$XERCESCLIB/:$MonicelliDir/plugins/libs/:$MonicelliDir/test/customWidgets/libs:$LD_LIBRARY_PATH