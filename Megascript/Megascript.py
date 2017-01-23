#!/usr/bin/env python
import sys,os,commands,subprocess,time,datetime,string
from email.mime.text import MIMEText

maxThreads          = 8;
copyRawFiles        = False;
merge               = False;
runChewie           = False;
monicelliDir        = "/home/cmstestbeam/Programming/Monicelli/";
chewieDir           = "/home/cmstestbeam/Programming/Chewie/";
geometryFile        = "Run314_Merged.geo";
testBeamYear        = "2016";
testBeamMonth       = "12";
testBeamMonthName   = "December";
baseDataDir         = "/data/TestBeam/" + testBeamYear + "_" + testBeamMonth + "_" + testBeamMonthName + '/';
mergedDataDir       = baseDataDir + "Merged/";
geometriesDataDir   = baseDataDir + "Geometries/"
monicelliOutDataDir = baseDataDir + "MonicelliOutput/"
testBeamYearMonth   = testBeamYear + testBeamMonthName;
remoteComputer      = "testbeam01linux.dhcp.fnal.gov"
remoteDir           = "/media/sf_C_DRIVE/T992_" + testBeamMonthName + "_" + testBeamYear + '/';
#fileNameToProcess   = "Run{no}.dat";
fileNameToProcess   = "Run{no}_Merged.dat";
#mailList            = "mauro.dinardo@cern.ch,uplegger@fnal.gov,luigi.moroni@mib.infn.it,marco.meschini@cern.ch,irene.zoi@stud.unifi.it,caterina.vernieri@gmail.com,dzuolo@fnal.gov";
#mailList            = "uplegger@fnal.gov,cvernier@fnal.gov,obertino@to.infn.it,fabio.ravera@cern.ch";
#mailList	     = "cvernier@fnal.gov"#uplegger@fnal.gov,marco.meschini@cern.ch,giacomo.sguazzoni@cern.ch,mauro.dinardo@cern.ch,luigi.moroni@mib.infn.it,cvernier@fnal.gov";
mailList            = "uplegger@fnal.gov";
templateFilesDir    = "./templates/";
monicelliExpressTemplateName = "MonicelliExpress_TestBeamConfiguration_template.xml";
chewieExpressTemplateName    = "ChewieExpress_TestBeamConfiguration_template.xml";
chewieTemplateName           = "Chewie_TestBeamConfiguration_template.xml";
 

###########################################################################################
def makeMonicelliTemplate(fileOut):
    FilesPath             = mergedDataDir; 
    GeometriesPath        = geometriesDataDir; 
    TrackFindingAlgorithm = "FirstAndLast"; 
    TrackFittingAlgorithm = "Simple"; 
    NumberOfEvents        = "-1";
    TrackPoints           = "6"; 
    MaxPlanePoints        = "5"; 
    Chi2Cut               = "-1"; 
    XTolerance            = "500"; 
    YTolerance            = "500";
    FindDut               = "false";
    FineAlignment         = "false";
    FineTelescopeAlignment= "true";
    UseEtaFunction        = "false";
    
    outFile = open(fileOut,"w")
    outFile.write("<?xml version='1.0' encoding='iso-8859-1'?>\n");
    outFile.write("<!DOCTYPE MonicelliExpressConfiguration SYSTEM \"./dtd/ExpressConfiguration.dtd\">\n");
    outFile.write("<MonicelliExpressConfiguration>\n");
    outFile.write("  <Defaults FilesPath=\"" + FilesPath + "\"\n");
    outFile.write("	       GeometriesPath=\"" + GeometriesPath + "\"\n");
    outFile.write("	       TrackFindingAlgorithm=\"" + TrackFindingAlgorithm + "\"\n");
    outFile.write("	       TrackFittingAlgorithm=\"" + TrackFittingAlgorithm + "\"\n");
    outFile.write("	       NumberOfEvents=\"" + NumberOfEvents + "\"\n");
    outFile.write("	       TrackPoints=\"" + TrackPoints + "\"\n");
    outFile.write("	       MaxPlanePoints=\"" + MaxPlanePoints + "\"\n");
    outFile.write("	       Chi2Cut=\"" + Chi2Cut + "\"\n");
    outFile.write("	       XTolerance=\"" + XTolerance + "\"\n");
    outFile.write("	       YTolerance=\"" + YTolerance + "\"\n");
    outFile.write("	       FindDut=\"" + FindDut + "\"\n");
    outFile.write("	       FineAlignment=\"" + FineAlignment + "\"\n");
    outFile.write("	       FineTelescopeAlignment=\"" + FineTelescopeAlignment + "\"\n");
    outFile.write("	       UseEtaFunction=\"" + UseEtaFunction + "\"/>\n");
    outFile.write("</MonicelliExpressConfiguration>\n");
    outFile.close();
    f = open(fileOut, 'r')
    print f.read()
    f.close()

###########################################################################################
def makeChewieTemplate(fileOut):
    FilesPath = monicelliOutDataDir; 
    outFile = open(fileOut,"w")
    outFile.write("<?xml version='1.0' encoding='iso-8859-1'?>\n");
    outFile.write("<!DOCTYPE ChewieExpressConfiguration SYSTEM \"./dtd/ExpressConfiguration.dtd\">\n");
    outFile.write("<ChewieExpressConfiguration>\n");
    outFile.write("  <Defaults FilesPath=\"" + FilesPath + "\"\n");
    outFile.write("	      Convert=\"yes\"\n");
    outFile.write("	      RunAnalysis=\"yes\"\n");
    outFile.write("	      NumberOfEvents=\"-1\"/>\n");
    outFile.write("</ChewieExpressConfiguration>\n");
    outFile.close();
    f = open(fileOut, 'r')
    print f.read()
    f.close()

###########################################################################################
def sshName(name):
    name = string.replace(name,"~","\\~");
    name = string.replace(name,"!","\\!");
    name = string.replace(name,"@","\\@");
    name = string.replace(name,"#","\\#");
    name = string.replace(name,"%","\\%");
    name = string.replace(name,"^","\\^");
    name = string.replace(name,"&","\\&");
    name = string.replace(name,"*","\\*");
    name = string.replace(name,"(","\\(");
    name = string.replace(name,")","\\)");
    name = string.replace(name,"[","\\[");
    name = string.replace(name,"]","\\]");
    name = string.replace(name,"{","\\{");
    name = string.replace(name,"}","\\}");
    name = string.replace(name,";","\\;");
    name = string.replace(name,"'","\\'");
    name = string.replace(name,",","\\,");
    name = string.replace(name,"<","\\<");
    name = string.replace(name,">","\\>");
    name = string.replace(name,"?","\\?");
    name = string.replace(name," ","\ ");
    #This must stay after the substitution of *
    name = string.replace(name,'"','*');
    
    return name
###########################################################################################
def copy(files, sourceDir, destinationDir, sourceHost, destinationHost):

    copiedFiles = [];
    for file in files:
	cmd = makeCPCommand(file, sourceDir, destinationDir, sourceHost, destinationHost);
	print cmd;
	proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
	out,err = proc.communicate();
	if(err != ''):
	    print err
	else:
	    copiedFiles.append(file)
    return copiedFiles;
    

###########################################################################################
def makeSysCommand(command, arg, host=''):
    cmd = '';
    if(host == ''):
	cmd = command + " \"" + arg + "\"";
    else:
	cmd = "ssh " + host + " \"" + command + " " + sshName(arg) + "\"";
    return cmd;

###########################################################################################
def makeCPCommand(fileName, sourceHost, sourceDir, destinationHost, destinationDir):
    cmd = '';
    #print fileName
    destinationDir += fileName[:fileName.rfind('/')+1];
    #print directory
    
    #if(sourceHost == '' and destinationHost == ''):
    #	 cmd = "cp -p " + sshName(sourceDir + fileName) + " " + sshName(destinationDir + fileName);
    #elif(sourceHost == '' and destinationHost != ''):
    #	 cmd = "scp -p "  + sshName(sourceDir + fileName) + " " + destinationHost + ":\"" + sshName(destinationDir + fileName) + '\"';
    #elif(sourceHost != '' and destinationHost == ''):
    #	 cmd = "scp -p " + sourceHost + ":\"" + sshName(sourceDir + fileName) + "\" " + sshName(destinationDir + fileName);
    #elif(sourceHost != '' and destinationHost != ''):
    #	 cmd = "scp -p " + sourceHost + ":\"" + sshName(sourceDir + fileName) + "\" " + destinationHost + ":\"" + sshName(destinationDir + fileName) + '\"';
    if(sourceHost == '' and destinationHost == ''):
	cmd = "cp -p \"" + sourceDir + fileName + "\" " + '\"' + destinationDir + '\"';
    elif(sourceHost == '' and destinationHost != ''):
	cmd = "scp -p \""  + sourceDir + fileName + "\" " + destinationHost + ":\"" + sshName(destinationDir) + '\"';
    elif(sourceHost != '' and destinationHost == ''):
	cmd = "scp -p " + sourceHost + ":\"" + sshName(sourceDir + fileName) + "\" \"" + destinationDir + '\"';
    elif(sourceHost != '' and destinationHost != ''):
	cmd = "scp -p " + sourceHost + ":\"" + sshName(sourceDir + fileName) + "\" " + destinationHost + ":\"" + sshName(destinationDir) + '\"';
    return cmd;

###########################################################################################
def makeFileList(host, directory, filter=''):
    cmd = makeSysCommand("ls -la", directory, host);
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
    out,err = proc.communicate();
    if(err.find("Permission denied") != -1):
       return ;
    fileList = out.split('\n');
    print cmd;
    #print fileList;
    del fileList[0];
    fileList.pop(); #for some reason it also gives you an empty line at the end
    files = [];
    for file in fileList:
	parsed = parsell(file);
	#print parsed
	if(parsed[8] == '.' or parsed[8] == '..'):
	    continue ;
	if(parsed[8].find('"') != -1):
	    print "The file or dir " + directory + parsed[8] + " contains a \" character that I dont' want to deal with!! I won't copy it"
	    continue ;
	name = parsed[8];
	#print name;
	if('d' not in parsed[0] and name.find(filter) != -1):
	    files.append(name)

    #print files;
    return files;

###########################################################################################
def parsell(line):
    fields = line.split(' ');
    nFields = len(fields);
    i = 0;
    withoutName = '';
    for counter in range(nFields):
        #print str(i) + " " + fields[i];
        if(i>=8):
            del fields[i];
        else:
            if(fields[i] == ''):
                del fields[i];
            else:
                withoutName += fields[i];
                i += 1;
            withoutName += ' ';
    #print line
    #print withoutName
    fields.append(line[len(withoutName):]);
    return fields;

###########################################################################################
def splitHostDir(name):
    split = name.split(':');
    host = '';
    dir  = name;
    if(len(split) == 2):
	host = split[0];
	dir  = split[1];
    #if(dir[len(dir)-1] == '/'):
#	dir = dir[:len(dir)-1];
    if(dir[len(dir)-1] != '/'):
	dir += '/';

    return host,dir;


###########################################################################################
def replaceInFile(fileIn, fileOut, replaceWhat, replaceWith):
    testFile = open(fileIn,"r")
    testFileContent = testFile.readlines()
    testFile.close()
    i=0
    outFile = open(fileOut,"w")
    for line in testFileContent:
       if(line.find(replaceWhat) ):
         testFileContent[i] = line.replace(replaceWhat,replaceWith);
       outFile.write(testFileContent[i])
       i = i+1
    outFile.close()

###########################################################################################
def monicelliReplace(fileIn, fileOut, runList, geometry):
    testFile = open(fileIn,"r")
    testFileContent = testFile.readlines()
    testFile.close()
    i=0
    outFile = open(fileOut,"w")
    for line in testFileContent:
       if(line.find("</MonicelliExpressConfiguration>") != -1):
         for run in runList:
	     print run
	     outFile.write("   <File Name=\"" + run + "\" Geometry=\"" + geometry + "\" />\n");
       outFile.write(line)
    outFile.close()

###########################################################################################
def chewieReplace(fileIn, fileOut, runList):
    testFile = open(fileIn,"r")
    testFileContent = testFile.readlines()
    testFile.close()
    i=0
    outFile = open(fileOut,"w")
    for line in testFileContent:
       if(line.find("</ChewieExpressConfiguration>") != -1):
         for run in runList:
	     outFile.write("    <Files Configuration=\"" + testBeamYearMonth + "_ChewieConfiguration.xml\" OutFileName=\"" + run.replace(".dat","") + "_Chewie.root\">\n      <File Name=\"" + run.replace(".dat",".root")  + "\"/>\n    </Files>\n");
       outFile.write(line)
    outFile.close()

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
def sendEmail(mailList, title, body):
    print "Sending email to " + mailList + " with body:\n" + body
    list = mailList.split(',')
    for email in list:
        p = os.popen("mail -s \"" + title + "\" " + email ,"w")
        p.write(body)
        status = p.close() 

###########################################################################################
def exportVars():
    os.environ['MonicelliDir'            ] = monicelliDir;
    os.environ['Monicelli_RawData_Dir'   ] = baseDataDir + "Merged";
    os.environ['Monicelli_DataSample_Dir'] = baseDataDir + "Merged";
    os.environ['Monicelli_CalSample_Dir' ] = baseDataDir + "Calibrations";
    os.environ['MonicelliOutputDir'      ] = baseDataDir + "MonicelliOutput";
    os.environ['Monicelli_XML_Dir'       ] = baseDataDir + "Geometries";
    os.environ['ROOTSYS'                 ] = "/opt/local/root";
    os.environ['QTDIR'                   ] = "/opt/local/qt";
    os.environ['LD_LIBRARY_PATH'         ] = "/opt/local/root/lib:/opt/local/qt/lib:/opt/local/xerces/lib:"+monicelliDir+"/plugins/libs/"
    os.environ['MERGER_INPUT_DIR'        ] = baseDataDir;
    os.environ['MERGER_OUTPUT_DIR'       ] = baseDataDir + "Merged";
    os.environ['CHEWIEDIR'               ] = chewieDir;
    os.environ['CHEWIEDATADIR'           ] = baseDataDir + "MonicelliOutput"
    os.environ['CHEWIEINPUTDIR'          ] = baseDataDir + "ChewieInput"
    os.environ['CHEWIEOUTPUTDIR'         ] = baseDataDir + "ChewieOutput"
    os.environ['CHEWIEXMLDIR'            ] = chewieDir   + "xml"

###########################################################################################
def rawCopy(runBegin, runEnd):
    controllersList = ["CC_DUT", "CC_DS", "CC_US", "CC_STIB_MASTER"];
    for run in range(runBegin,runEnd+1):
        for controller in controllersList:
            cmd = "scp -rp " + os.environ["LOGNAME"] + '@' + remoteComputer + ':\"' + remoteDir + "Software/" + controller + "/Burst\ Archive/Run" + str(run) + "_*\" " + baseDataDir + "Raw/" + controller + "/Burst\ Archive";
            print "Running command: " + cmd;

            proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
            out,error = proc.communicate();
            if(error != ''):
                print error
                subject = "FATAL: Raw copy error!"
                sendEmail(mailList, subject, error);
		break;
            print out;
#${originComputer}:"/media/sf_C_DRIVE/${originFolder}/Software/CC_DUT/Burst\ Archive/${runName}${runNumber}_*" /data/TestBeam/${destinationFolder}/Raw/CC_DUT/Burst\ Archive/  
        #scp -rp os.environ["LOGNAME"]@${originComputer}:"/media/sf_C_DRIVE/${originFolder}/Software/CC_DUT/Burst\ Archive/${runName}${runNumber}_*" /data/TestBeam/${destinationFolder}/Raw/CC_DUT/Burst\ Archive/  
    

###########################################################################################
def is_ascii(s):
#    print s, ord(s)
    if(ord(s) > 31 and ord(s) < 128):
        return True;
    return False;

###########################################################################################
def main():
    if(len(sys.argv) < 3):
        print "Usage: Megascript.py (From Run#) (To Run#)"
        sys.exit()
    option = '';
    exportVars();

    #Making source files list
    sourceHost, sourceDir = splitHostDir(remoteComputer + ':' + remoteDir + "Analysis/Merged/");
    print "Reading data from host: " + sourceHost
    print "Stored in directory: " + sourceDir
#    sourceList = makeFileList(sourceHost, sourceDir, ".dat");

    #Making destination files list
    destination = os.environ['Monicelli_RawData_Dir'];
    destinationHost, destinationDir = splitHostDir(destination);
    print baseDataDir

    if(copyRawFiles):
        rawCopy(int(sys.argv[1]), int(sys.argv[2]));
    if(merge):
        cmd = baseDataDir + "../Exes/Merger/Merger " + sys.argv[1] + ' ' + sys.argv[2];
        print "Running command: " + cmd;

        proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
        out,error = proc.communicate();
        if(error != ''):
            print error
            subject = "FATAL: Raw copy error!"
            sendEmail(mailList, subject, error);
        print out;
        print "Done Merging......";
	time.sleep(2);
        print "Done Sleeping......";

    destinationList = makeFileList(destinationHost, destinationDir);
    #Copying files that are missing in the destination directory
#    filesToCopy = [];
#    for file in sourceList:
#        if(file not in destinationList):
#            filesToCopy.append(file);
#    
#    for file in filesToCopy:
#        print file;
    #print filesToCopy;
#    copiedFiles = copy(filesToCopy, sourceHost, sourceDir, destinationHost, destinationDir);
#    
#    if(len(copiedFiles) == 0):
#    	exit(0);

    fileList = makeFileListToProcess(int(sys.argv[1]), int(sys.argv[2]));
    print fileList;
    tmpFileList = fileList;

    #print "List of files to process:";
    #print "Destination list:\n"
    #print destinationList
    for file in tmpFileList:
       if (file not in destinationList):
          fileList.remove(file);
	  print "removing file: " + file
       else:
          print file;

    print "Number of files to process: " + str(len(fileList));
    
    if(len(fileList) == 0):
    	print "There are no files in that range. Are you sure you copied them with the right permissions?";
	sys.exit();
    

    print fileList

    ################################################################################################################
    #If Monicelli Express template does not exist then create it...
    templateName = testBeamYearMonth+"_TestBeamConfiguration_template.xml";
    monicelliTemplateFileName = monicelliDir + "Express/xml/" + templateName;
    if not os.path.isfile(monicelliTemplateFileName):
        print "WARNING: I couldn't find the MonicelliExpress template file " + monicelliTemplateFileName + ", so I am creating one using the template at the top of the megascript!"
        makeMonicelliTemplate(monicelliTemplateFileName);
    print monicelliTemplateFileName;
    
    ################################################################################################################
    #If Chewie Express template does not exist then create it...
    chewieTemplateFileName = chewieDir + "Express/xml/" + templateName;
    if not os.path.isfile(chewieTemplateFileName):
        print "WARNING: I couldn't find the ChewieExpress template file " + chewieTemplateFileName + ", so I am creating one using the template at the top of the megascript!"
        makeChewieTemplate(chewieTemplateFileName);
    print chewieTemplateFileName;

    ################################################################################################################
    #If Chewie Configuration template does not exist then create it...
    chewieConfigurationName = testBeamYearMonth+"_ChewieConfiguration.xml";
    chewieConfigurationFileName = chewieDir + "xml/" + chewieConfigurationName;
    if not os.path.isfile(chewieConfigurationFileName):
        print "WARNING: I couldn't find the Chewie Configuration file " + chewieConfigurationFileName + ", so I am creating one using the template at the top of the megascript!"
 	cmd = "cp templates/Chewie_TestBeamConfiguration_template.xml " + chewieConfigurationFileName;
 	print "Running command: " + cmd;
 	processes[p] = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
        
    print templateName;

    global maxThreads;
    if (maxThreads > len(fileList)):
        maxThreads = len(fileList);
    
    processes        = [None] * len(fileList);
    status           = [0]    * len(fileList);#0 ready, 1 running Monicelli, 2, done a, 3 running Chewie, 4 done
    nOfIncorrectData = [0]    * len(fileList);
    runName          = [""]   * len(fileList);
    done = False;
    runningProcesses = 0;
    lastState = 4;#run Monicelli and Chewie
    if(not runChewie):
	lastState = 2;#Only Monicelli
    while(not done):
    	for p in range (0, len(processes)):
	    if(status[p] == 0 and runningProcesses < maxThreads):
		status[p] = 1;
		################################################################################################################
 	    	#Run Monicelli
 	    	runName[p] = fileList[p].replace(".dat","");
 	    	print "Running Monicelli for " + runName[p];
 
 	    	#Create Express xml files from template
 	    	templateOutName = templateName.replace("template",runName[p]);
 	    	print templateOutName;

 	    	monicelliReplace(monicelliTemplateFileName, monicelliDir + "Express/xml/" + templateOutName, [fileList[p]], geometryFile);
 
 	    	#Run Monicelli express
 	    	cmd = "cd " + monicelliDir + "Express" + ";./MonicelliExpress " + templateOutName;
 	    	print "Running command: " + cmd;
 	    	processes[p] = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
		runningProcesses += 1;
	    
	    elif(status[p] == 1):
	        if not (processes[p].poll() is None):
		    status[p] = 2;
		    out,error = processes[p].communicate();
		    #print out;
 		    if(error != ''):
 		       print error
 		       error = "FATAL: There was an error executing Monicelli!"
 		       sendEmail(mailList, runName[p], error);
 		    #print out
 
 		    for line in out.split('\n'):
 		    	#print line;
		    	if(line.find("Incorrect data at block") != -1):
		    	    nOfIncorrectData[p] += 1;	    
                    if(not runChewie):
		        runningProcesses -= 1;
	    	        
 	    elif(runChewie and status[p] == 2):
 		status[p] = 3;
 		#Run Chewie
 		print "Running Chewie..." ;
 		templateOutName = templateName.replace("template",runName[p]);
 		chewieReplace(chewieTemplateFileName, chewieDir + "/Express/xml/" + templateOutName, [fileList[p]]);
 		cmd = "cd " + chewieDir + "Express;" + "./ChewieExpress " + templateOutName;
 		print "Running command: " + cmd;
 		processes[p] = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
 	    
 	    elif(status[p] == 3):
 		 #print "Checking chewie: " + str(p);
		 if not (processes[p].poll() is None):
  	    	     status[p] = 4;
		     runningProcesses -= 1;
 		     out,error = processes[p].communicate();
 		     if(error != ''):
 		       realError = '';
 		       for line in error.split('\n'):
		           #print ">-----------------------";
			   #print "---" + line + "---";
		           #print "<-----------------------";
 			   #ignore a line like this!
			   if(line.find("Warning") and line.find("no dictionary")):
 			       continue;
 			   if((line.find("Warning") == -1 and (error.find("Info") == -1 or error.find("Fit") or error.find("no dictionary")))):
			       print line
			       realError += line + '\n';
 		       if(realError != '') and ("created default TCanvas with name" not in realError):
 			   realError = "FATAL: There was an error executing Chewie! " + realError;
 			   sendEmail(mailList, runName[p], realError);
 		     #print out;
		     #print error;
 
 		     emailBody = "";
 		     for line in out.split('\n'):
 			 if(line.find("Detector") != -1):
 			     line = line[line.find("Detector"):len(line)-3];
 			     for char in line:
 				 if(is_ascii(char)):
 				     emailBody += char;
 			     emailBody += '\n';
 			 if(line.find("Converting Run") != -1 and line.find("done") == -1):
 			     emailBody += "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
 			     line = line[line.find("Run"):len(line)-9];
 			     for char in line:
 				 if(is_ascii(char)):
 				     emailBody += char;
 			     emailBody += '\n\n';
 			     emailBody += "There are " + str(nOfIncorrectData[p]) + " incorrect data while decoding the Merged file.\n\n";
 		     emailBody += "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
 		     print emailBody;
 		     sendEmail(mailList, runName[p], emailBody);

            done = True;
            for p in range (0, len(processes)):
            	if(status[p] != lastState):
	    	    done = False;
	    	    break;
            if(not done):
	        time.sleep(1)
############################################################################################
if __name__ == "__main__":
    main()



