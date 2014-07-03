setenv MERGER_DIR		 `pwd`
#setenv MERGER_INPUT_DIR 	 /data/TestBeam/2013_11_November/Raw/
#setenv MERGER_INPUT_DIR 	 /data/TestBeam/2014_01_January/Raw/
setenv MERGER_INPUT_DIR 	 /data/TestBeam/2014_04_April/
#setenv MERGER_OUTPUT_DIR	 ${MERGER_INPUT_DIR}Merged/
setenv MERGER_OUTPUT_DIR	 ../MergerOutput/

#===== Location of the ROOT components
setenv ROOTSYS  		 /opt/local/root

#===== Final PATH definitions
setenv PATH			$ROOTSYS/bin:${PATH}
setenv LD_LIBRARY_PATH          $ROOTSYS/lib

#----------------------------------------------------------------------------------------------------
