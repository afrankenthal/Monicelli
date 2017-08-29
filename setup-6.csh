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

# To run Monicelli do the following:

# scl enable devtoolset-2 tcsh
# source ~/.tcshrc
# source setup-6.csh
# ./Monicelli

alias do           'make -f MakefileAll'
alias cleanAll     'make -f MakefileAll clean'
alias distCleanAll 'make -f MakefileAll distclean'

#===============   Personalized configuration on this computer   ==============================================
if(${HOSTNAME} == "MAC") then

  echo "Setting up Monicelli for MAC"

  if(     ${USER} == "menasce") then
   setenv BASEDATADIR /Users/menasce/AnalysisTBF/data/TestBeamData_PlanarRadBatch01_PlanarRadBatch02_3DBatch01
  else if(${USER} == "dinardo") then
   setenv BASEDATADIR /raid2/data1/vtx1/dinardo/TestBeamFBKPlanarBeforeRad_Batch01
  else if(${USER} == "dzuolo") then
   setenv BASEDATADIR ../data/TestBeamFBKPlanarAfterRad_Batch01_02
  endif
  
#  setenv PATH /opt/rh/devtoolset-2/root/usr/bin:/bin:/usr/bin:/usr/local/bin:/usr/printmi/bin

  #===== Local directories
  setenv MonicelliDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   ${BASEDATADIR}/MonicelliOutput
  setenv Monicelli_XML_Dir	   ${BASEDATADIR}/Geometries
  
  #===== Location of the ROOT components
  setenv ROOTVER                   SIX
  setenv ROOTSYS		   /Applications/root_v6.04.02/
#  setenv ROOTSYS		   /Users/menasce/AnalysisTBF/root-v6.10.00.build
  setenv ROOTINC		   $ROOTSYS/include
  setenv ROOTLIB		   $ROOTSYS/lib

  #===== Location of the Qt components
  setenv QTDIR  		   /Applications/Qt5.9/5.9/clang_64
#  setenv QTDIR  		   /Users/menasce/AnalysisTBF/qt-v5.9.0

  setenv QTCREATORDIR		   '/Applications/Qt5.9/Qt\ Creator.app/Contents/MacOS/Qt\ Creator'
  setenv QTASSISTANTDIR            '/Applications/Qt5.9/5.9/clang_64/bin/Assistant.app/Contents/MacOS/'
  
  #===== Location of the BOOST components
  setenv BOOSTINC		   /Users/menasce/AnalysisTBF/boost_1_64_0
  setenv BOOSTLIB		   /Users/menasce/AnalysisTBF/boost_1_64_0/stage/lib

  #===== Location of the XERCES components
  setenv XERCESCINC		   /usr/local/include
  setenv XERCESCLIB		   /usr/local/lib
  
  #===== C++ flags
  setenv CPLUSPLUSFLAGS            "-std=c++11"

  setenv LD_LIBRARY_PATH	   ""

  alias  qt                        "${QTCREATORDIR}"
  alias  assistant                 "${QTASSISTANTDIR}/Assistant"
    
endif
#===============   Personalized configuration on this computer   ==============================================
if(${HOSTNAME} == "hal9000.mib.infn.it") then

  echo "Setting up for hal9000"
  if(     ${USER} == "menasce") then
#   setenv BASEDATADIR /raid1/menasce/AnalysisTBF/data/TestBeamData_PlanarRadBatch01_PlanarRadBatch02_3DBatch01
   setenv BASEDATADIR /raid1/menasce/AnalysisTBF/data/StripTelescopeAnalysis
  else if(${USER} == "dinardo") then
   setenv BASEDATADIR /raid2/data1/vtx1/dinardo/TestBeamFBKPlanarBeforeRad_Batch01
  else if(${USER} == "dzuolo") then
   #setenv BASEDATADIR /user/gr1/e831/dzuolo/data/TestBeamFBK_PlanarARadBatch02_3DBRadBatch01_PlanarBRadBatch03
   #setenv BASEDATADIR /user/gr1/e831/dzuolo/data/TestBeamData2015
   #setenv BASEDATADIR /user/gr1/e831/dzuolo/data/TestBeamFBKPlanarAfterRad_Batch01_02
   #setenv BASEDATADIR /user/gr1/e831/dzuolo/data/TestBeamFBKPlanarBeforeRad_Batch02
   setenv BASEDATADIR /user/gr1/e831/dzuolo/data/TestBeamData_February_2017
   #setenv BASEDATADIR /user/gr1/e831/dzuolo/StripTelescopeAnalysis
  endif
  
  setenv PATH /opt/rh/devtoolset-2/root/usr/bin:/bin:/usr/bin:/usr/local/bin:/usr/printmi/bin

  #===== Local directories
  setenv MonicelliDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   ${BASEDATADIR}/MonicelliOutput
  setenv Monicelli_XML_Dir	   ${BASEDATADIR}/Geometries
  
  #===== Location of the ROOT components
  setenv ROOTVER                   SIX
  setenv ROOTSYS		   /user/service/root-6.04.10/
  setenv ROOTINC		   $ROOTSYS/include/root
  setenv ROOTLIB		   $ROOTSYS/lib/root

  #===== Location of the Qt components
  setenv QTDIR  		   /user/service/Qt-5.6.2/5.6/gcc_64/
  setenv QTCREATORDIR		   /raid1/menasce/qt-creator-opensource-src-4.3.0
  setenv QTLIB                     ${QTDIR}/lib 
  setenv QTINC                     ${QTDIR}/include 

  #===== Location of the BOOST components
  setenv BOOSTINC		   /usr/local/include/boost
  setenv BOOSTLIB		   /usr/local/lib

  #===== Location of the XERCES components
  setenv XERCESCINC		   /opt/local/xerces/include
  setenv XERCESCLIB		   /opt/local/xerces/lib
  
  #===== Location of the FINDER components
  setenv FINDERHOME		   ${MonicelliDir}/finder
  
  #===== C++ flags
  setenv CPLUSPLUSFLAGS            "-std=c++11"

  setenv LD_LIBRARY_PATH	   
  
  unalias qtcreator
    
endif
#===============   Personalized configuration on this computer   ==============================================
if(${HOSTNAME} == "cip.mib.infn.it") then

  if(     ${USER} == "menasce") then
   setenv BASEDATADIR /raid1/menasce/AnalysisTBF/data/TestBeamData_PlanarRadBatch01_PlanarRadBatch02_3DBatch01
  else if(${USER} == "dinardo") then
   setenv BASEDATADIR /raid2/data1/vtx1/dinardo/TestBeamFBKPlanarBeforeRad_Batch01
  else if(${USER} == "dzuolo") then
   setenv BASEDATADIR ../data/TestBeamFBKPlanarAfterRad_Batch01_02
  endif
  
  setenv PATH /opt/rh/devtoolset-2/root/usr/bin:/bin:/usr/bin:/usr/local/bin:/usr/printmi/bin

  #===== Local directories
  setenv MonicelliDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   ${BASEDATADIR}/MonicelliOutput
  setenv Monicelli_XML_Dir	   ${BASEDATADIR}/Geometries
  
  #===== Location of the ROOT components
  setenv ROOTVER                   SIX
  setenv ROOTSYS		   /usr
  setenv ROOTINC		   $ROOTSYS/include/root
  setenv ROOTLIB		   $ROOTSYS/lib64/root

  #===== Location of the Qt components
  setenv QTDIR  		   /user/service/Qt-5.6.2/5.6/gcc_64
  setenv QTCREATORDIR		   /user/service/QtCreator-4.3.0/

  #===== Location of the BOOST components
  setenv BOOSTINC		   /usr/local/include/boost
  setenv BOOSTLIB		   /usr/local/lib

  #===== Location of the XERCES components
  setenv XERCESCINC		   /opt/local/xerces/include
  setenv XERCESCLIB		   /opt/local/xerces/lib
  
  #===== C++ flags
  setenv CPLUSPLUSFLAGS            "-std=c++11"

  setenv LD_LIBRARY_PATH	   
  
  unalias qtcreator
    
endif

#===== Final PATH definitions
setenv PATH	       ${ROOTSYS}/bin:${QTDIR}/bin:${QTCREATORDIR}/bin:${PATH}
setenv LD_LIBRARY_PATH ${ROOTLIB}:${BOOSTLIB}:${QTDIR}/lib64:${QTCREATORDIR}:${XERCESCLIB}/:${MonicelliDir}/plugins/libs/:${MonicelliDir}/test/customWidgets/libs:${LD_LIBRARY_PATH}

alias qt qtcreator
#/user/service/QtCreator-4.3.0/bin:/usr/sue/sbin:/usr/sue/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/printmi/bin:/usr/bin:/root/bin
