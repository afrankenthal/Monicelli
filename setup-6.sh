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

# scl enable devtoolset-2 bash
# source setup-6.sh
# ./Monicelli

alias do='make -f MakefileAll'
alias cleanAll='make -f MakefileAll clean'
alias distCleanAll='make -f MakefileAll distclean'

#===============   Personalized configuration on this computer   ==============================================
if [ ${HOSTNAME} == "hal9000.mib.infn.it" ]; then

    if [ ${USER} == "menasce" ]; then
	export BASEDATADIR=../data/2015_10_October/
    elif [ ${USER} == "dinardo" ]; then
	export BASEDATADIR=/raid2/data1/vtx1/dinardo/TestBeamFBKPlanarBeforeRad_Batch01
    elif [ ${USER} == "dzuolo" ]; then
	export BASEDATADIR=../data/TestBeamData2015
    fi

    export PATH=/opt/rh/devtoolset-2/root/usr/bin:/bin:/usr/bin:/usr/local/bin:/usr/printmi/bin

    #===== Local directories
    export MonicelliDir=`pwd`
    export Monicelli_RawData_Dir=${BASEDATADIR}/Merged
    export Monicelli_DataSample_Dir=${BASEDATADIR}/Merged
    export Monicelli_CalSample_Dir=${BASEDATADIR}/Calibrations
    export MonicelliOutputDir=${BASEDATADIR}/MonicelliOutput
    export Monicelli_XML_Dir=${BASEDATADIR}/Geometries
    
    #===== Location of the ROOT components
    export ROOTVER=SIX
    export ROOTSYS=/user/service/root-6.04.10/
    export ROOTINC=$ROOTSYS/include/root
    export ROOTLIB=$ROOTSYS/lib/root
    
    #===== Location of the Qt components
    export QTDIR=/usr/local/Trolltech/Qt-4.8.5/
    export QTCREATORDIR=/opt/local/qtcreator-2.7.1/bin
    
    #===== Location of the BOOST components
    export BOOSTINC=/usr/local/include/boost
    export BOOSTLIB=/usr/local/lib
    
    #===== Location of the XERCES components
    export XERCESCINC=/opt/local/xerces/include
    export XERCESCLIB=/opt/local/xerces/lib
    
    #===== C++ flags
    export CPLUSPLUSFLAGS=-std=c++11
   
    export LD_LIBRARY_PATH	   
    
    alias qtcreator=${QTCREATORDIR}/qtcreator
    
fi
	
#===== Final PATH definitions
export PATH=${ROOTSYS}/bin:${QTDIR}/bin:${QTCREATORDIR}/bin:${PATH}
export LD_LIBRARY_PATH=${ROOTLIB}:${BOOSTLIB}:${QTDIR}/lib:${XERCESCLIB}/:${MonicelliDir}/plugins/libs/:${MonicelliDir}/test/customWidgets/libs:${LD_LIBRARY_PATH}
