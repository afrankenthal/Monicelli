#!/bin/tcsh

set originComputer = testbeam01linux.dhcp.fnal.gov
set destinationComputer = ftbftracker02
set destinationFolder = 2013_11_November
set runName = Run

#echo $#argv
if ($#argv == 0) then
  echo "Usage ./copy.csh Run number to copy"
  exit
else if ($#argv == 1) then
  set runBegin = $argv[1]
  set runEnd   = $argv[1]
else if ($#argv == 2) then
  set runBegin = $argv[1]
  set runEnd   = $argv[2]
endif

echo "Copying runs from: " $runBegin " to " $runEnd "\n"
foreach  runNumber(`seq $runBegin 1 $runEnd`)
#  set cmd = `scp -rp ${originComputer}:"/media/sf_C_DRIVE/T992_November_2013/Analysis/Merges/${runName}${runNumber}*"
#  /data/TestBeam/${destinationFolder}/Merged/`
  echo "*****************************   Copying run ${runNumber}... *******************************************"
  scp -rp uplegger@${originComputer}:"/media/sf_C_DRIVE/T992_November_2013/Software/CC_DUT/Burst\ Archive/${runName}${runNumber}_*" /data/TestBeam/${destinationFolder}/Raw/CC_DUT/Burst\ Archive/  
  scp -rp uplegger@${originComputer}:"/media/sf_C_DRIVE/T992_November_2013/Software/CC_DS/Burst\ Archive/${runName}${runNumber}_*"  /data/TestBeam/${destinationFolder}/Raw/CC_DS/Burst\ Archive/   
  scp -rp uplegger@${originComputer}:"/media/sf_C_DRIVE/T992_November_2013/Software/CC_US/Burst\ Archive/${runName}${runNumber}_*"  /data/TestBeam/${destinationFolder}/Raw/CC_US/Burst\ Archive/   
  #scp -rp ${originComputer}:"/media/sf_C_DRIVE/T992_November_2013/Analysis/Merges/${runName}${runNumber}*" /data/TestBeam/${destinationFolder}/Merged
  echo "************************             Done!             ***************************************\n"
end
