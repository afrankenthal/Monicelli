#----------------------------------------------------------------------------------------------------
# Author: D. Menasce

# The following script sets up the environment to run qt and Monicelli on local machines.
# Once you have fetched this copy from the cvs repository, make a copy (name it something
# like mySetupQt.csh) and modify the copy to suit your needs. Please do NOT put bac in cvs  
# a modified copy of the original setupQt.csh if you have by mistake accidentally edited it.
#
# Before running Monicelli, source YOUR own, PRIVATE **copy** of this file:

alias do='make -f MakefileAll'
export BASEDATADIR=/data/TestBeam/2015_12_December

#===============   Personalized configuration on this computer   ==============================================
if [ ${HOSTNAME} = "rulinux03.dhcp.fnal.gov" ]; then

  #===== Local directories
  export MonicelliDir=`pwd`
  export Monicelli_RawData_Dir=${BASEDATADIR}/Merged
  export Monicelli_DataSample_Dir=${BASEDATADIR}/Merged
  export Monicelli_CalSample_Dir=${BASEDATADIR}/Calibrations
  export MonicelliOutputDir=${BASEDATADIR}/MonicelliOutput
  export Monicelli_XML_Dir=${BASEDATADIR}/Geometries
  
  #===== Location of the ROOT components
  export ROOTSYS=/opt/local/root


  #===== Location of the Qt components
  export QTDIR=/opt/local/qt
  export QTCREATORDIR=/opt/local/qtcreator

  #===== Location of the BOOST components
  export BOOSTINC=/usr/include
  export BOOSTLIB=/usr/lib64

  #===== Location of the XERCES components
  export XERCESCINC=/opt/local/xerces/include
  export XERCESCLIB=/opt/local/xerces/lib
  
  export LD_LIBRARY_PATH=/usr/lib64
  
elif [ ${HOSTNAME} = "ftbftracker01.fnal.gov" || ${HOSTNAME} = "ftbftracker02.fnal.gov" || ${HOSTNAME} = "rulinux01.dhcp.fnal.gov" || ${HOSTNAME} = "rulinux02.dhcp.fnal.gov" || ${HOSTNAME} = "rulinux03.dhcp.fnal.gov" ]; then

  #===== Local directories
  export MonicelliDir=`pwd`
  export Monicelli_RawData_Dir=${BASEDATADIR}/Merged
  export Monicelli_DataSample_Dir=${BASEDATADIR}/Merged
  export Monicelli_CalSample_Dir=${BASEDATADIR}/Calibrations
  export MonicelliOutputDir=${BASEDATADIR}/MonicelliOutput
  export Monicelli_XML_Dir=${MonicelliDir}/xml
  
  #===== Location of the ROOT components
  export ROOTSYS=/opt/local/root

  #===== Location of the Qt components
  export QTDIR=/opt/local/qt
  export QTCREATORDIR=/opt/local/qtcreator

  #===== Location of the BOOST components
  export BOOSTINC=/usr/include
  export BOOSTLIB=/usr/lib64

  #===== Location of the XERCES components
  export XERCESCINC=/opt/local/xerces/include
  export XERCESCLIB=/opt/local/xerces/lib
  
  export LD_LIBRARY_PATH=/usr/lib64
  
fi

#===== Final PATH definitions
export PATH=$ROOTSYS/bin:$QTDIR/bin:$QTCREATORDIR/bin:$PATH
export LD_LIBRARY_PATH=$ROOTSYS/lib:$QTDIR/lib:$QTCREATORDIR/lib:$XERCESCLIB/:$MonicelliDir/plugins/libs/:$MonicelliDir/test/customWidgets/libs:$LD_LIBRARY_PATH
