#!/usr/bin/env python
import sys,os,commands,subprocess,time,datetime,string, threading
from email.mime.text import MIMEText

fileNameToProcess   = "Run{no}_Merged";
#fileNameToProcess   = "Run{no}";

###########################################################################################
def makeFileListToProcess(Min, Max):
    runList = []
    i=Min
    for i in range(Min,Max+1):
       s = fileNameToProcess.format(no=i)
       runList.append(s)
       print "files to process: " + s
    return runList

###########################################################################################
def main():
    if(len(sys.argv) != 2):
        print "Usage: RunMAPSA.py RunList"
	print "Example of Runlist:"
	print "1 -> only Run1"
	print "1-10 -> from Run1 to Run10"
	print "1-10,20,30-35 -> from Run1 to Run10, Run20, from Run30 to Run35"
        sys.exit();
    option = '';

    rangedFileList = sys.argv[1].split(',');
    print rangedFileList;
    for runs in rangedFileList:
        #print runs;
	if(runs.find('-') != -1):
	    begin,end = runs.split('-');
        else:
	    begin = end = runs;
	#print "B: " + begin + " E: " + end; 
	if(int(begin) > int(end)):
	    print "Range " + runs + " is not valid since the starting run is greater than the end run!";
	    sys.exit();
    

    for runs in rangedFileList:
	if(runs.find('-') != -1):
	    begin,end = runs.split('-');
    	else:
	    begin = end = runs;
        runList = makeFileListToProcess(int(begin),int(end))
	for run in runList:
	    cmd = "./bin/MAPSAConverter " + run + ".root " + run + ".geo";
	    print cmd;
	    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
	    out,err = proc.communicate();
	    if(err != ''):
	    	print err
	    print out;
   
############################################################################################
if __name__ == "__main__":
    main()



