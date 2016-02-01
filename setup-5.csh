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


alias do 'make -f MakefileAll '
setenv BASEDATADIR /user/gr1/e831/menasce/MonicelliNew/data/2015_10_October

#===============   Personalized configuration on this computer   ==============================================
if(${HOSTNAME} == "hal9000.mib.infn.it") then

  if(     ${USER} == "menasce") then
   setenv BASEDATADIR ../data/2015_10_October/
  else if(${USER} == "dinardo") then
   setenv BASEDATADIR /raid2/data1/vtx1/dinardo/TestBeamData2015
  endif
  

  #===== Local directories
  setenv MonicelliDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   ${BASEDATADIR}/MonicelliOutput
  setenv Monicelli_XML_Dir	   ${BASEDATADIR}/Geometries
  
  #===== Location of the ROOT components
  setenv ROOTVER                   FIVE
  setenv ROOTSYS		   /user/service/root-5.28.00g/
  setenv ROOTINC		   $ROOTSYS/include/root
  setenv ROOTLIB		   $ROOTSYS/lib/root

  #===== Location of the Qt components
  setenv QTDIR  		   /usr/local/Trolltech/Qt-4.8.0/
  setenv QTCREATORDIR		   $QTDIR/bin/

  #===== Location of the BOOST components
  setenv BOOSTINC		   /usr/local/include/boost
  setenv BOOSTLIB		   /usr/local/lib

  #===== Location of the XERCES components
  setenv XERCESCINC		   /opt/local/xerces/include
  setenv XERCESCLIB		   /opt/local/xerces/lib
  
  #===== C++ flags
  setenv CPLUSPLUSFLAGS            -std=c++0x

  setenv LD_LIBRARY_PATH	  
  
  alias qtcreator /opt/local/qtcreator-2.7.1/bin/qtcreator
    
endif

#===== Final PATH definitions
setenv PATH	       ${ROOTSYS}/bin:${QTDIR}/bin:${QTCREATORDIR}/bin:${PATH}/
setenv LD_LIBRARY_PATH ${ROOTLIB}:${BOOSTLIB}:${QTDIR}/lib:${XERCESCLIB}/:${MonicelliDir}/plugins/libs/:${MonicelliDir}/test/customWidgets/libs:${LD_LIBRARY_PATH}
