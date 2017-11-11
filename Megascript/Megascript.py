#!/usr/bin/env python
import argparse
from argparse import RawTextHelpFormatter
import xml.etree.ElementTree as ET
import sys,os,commands,subprocess,time,datetime,string, threading, re
import distutils
from distutils import util
from email.mime.text import MIMEText

class Parameters:
    #################################################################################################
    #General parameters
    #################################################################################################
    MaxThreads  	= 8;
    CopyRawFiles	= False;
    MergeRawFiles	= False;
    RunMonicelli	= True;
    RunChewie		= True;
    TestBeamYear	= "2017";
    TestBeamMonth	= "11";
    TestBeamMonthName	= "November";
    TestBeamYearMonth	= TestBeamYear + TestBeamMonthName;
    BaseDataDir 	= "/data/TestBeam/" + TestBeamYear + "_" + TestBeamMonth + "_" + TestBeamMonthName + '/';
    MergedDataDir	= BaseDataDir + "Merged/";
    GeometriesDataDir	= BaseDataDir + "Geometries/"
    FileNameToProcess	= "Run{no}_Merged.dat";
    #mailList		 = "uplegger@fnal.gov,irene.zoi@desy.de,lorenzo.viliani@cern.ch,caterina.vernieri@gmail.com,mauro.dinardo@cern.ch,marco.meschini@cern.ch,dzuolo@fnal.gov";
    #mailList		 = "uplegger@fnal.gov,awalsh22@illinois.edu,shoh@fnal.gov,cvernier@fnal.gov,petra@fnal.gov,acanepa@fnal.gov,bschneid@fnal.gov";
    #mailList		 = "uplegger@fnal.gov,awalsh22@illinois.edu,shoh@fnal.gov";
    #mailList		 = "cms.pixel.testbeam@gmail.com";
    #mailList		 = "shoh@fnal.gov,uplegger@fnal.gov";
    MailList		= "uplegger@fnal.gov,dzuolo@fnal.gov";
    TemplateFilesDir	= "./templates/";

    #################################################################################################
    #CopyRawFiles parameters
    #################################################################################################
    RemoteComputer	= "testbeam01linux.dhcp.fnal.gov"
    RemoteDir		= "/media/sf_C_DRIVE/T992_" + TestBeamMonthName + "_" + TestBeamYear + '/';

    #################################################################################################
    #Monicelli parameters
    #################################################################################################
    MonicelliDir	= "/home/cmstestbeam/Programming/Monicelli/";
    MonicelliOutDataDir = BaseDataDir + "MonicelliOutput/"
    GeometryFile	= "Run172_Merged.geo";
    #File used as a template by the megascript to substitute the run numbers and geometry in order to create the xml file that MonicelliExpress uses to run 
    MonicelliExpressTemplateFilePath	= MonicelliDir + "Express/xml/";
    MonicelliExpressTemplateFileName	= TestBeamYearMonth + "_MonicelliExpress_template.xml";

    #################################################################################################
    #Chewie parameters
    #################################################################################################
    ChewieDir		= "/home/cmstestbeam/Programming/Chewie/";
    #File used as a template by the megascript to substitute the run numbers and chewie configuration in order to create the xml file that ChewieExpress uses to run 
    ChewieExpressTemplateFilePath	= ChewieDir + "Express/xml/";
    ChewieExpressTemplateFileName	= TestBeamYearMonth + "_ChewieExpress_template.xml";

    #Name of the configuration file for Chewie, written in the xml that ChewieExpress uses to run
    ChewieConfigurationFilePath 	= ChewieDir + "xml/";
    ChewieConfigurationFileName 	= TestBeamYearMonth + "_ChewieConfiguration.xml";

    #File used as a template by the megascript in case NO chewieConfigurationFileName exists!
    ChewieConfigurationTemplateFileName = "templates/Chewie_TestBeamConfiguration_template.xml";

    #################################################################################################
    #MonicelliExpressTemplate parameters
    #################################################################################################
    FilesPath		  = MergedDataDir; 
    GeometriesPath	  = GeometriesDataDir; 
    TrackFindingAlgorithm = "FirstAndLast"; 
    TrackFittingAlgorithm = "Simple"; 
    NumberOfEvents	  = "1000";
    TrackPoints 	  = "6"; 
    MaxPlanePoints	  = "5"; 
    Chi2Cut		  = "-1"; 
    XTolerance  	  = "500"; 
    YTolerance  	  = "500";
    FindDut		  = "true";
    FineAlignment	  = "true";
    FineTelescopeAlignment= "true";
    UseEtaFunction	  = "false";

    #################################################################################################
    #EnvironmentVariables parameters
    #################################################################################################
    #monicelliDir	     = MonicelliDir;									 
    Monicelli_RawData_Dir    = MergedDataDir;									      
    Monicelli_DataSample_Dir = MergedDataDir;									      
    Monicelli_CalSample_Dir  = BaseDataDir + "Calibrations";							      
    MonicelliOutputDir       = BaseDataDir + "MonicelliOutput";							      
    Monicelli_XML_Dir        = BaseDataDir + "Geometries";
    ROOTSYS		     = "/opt/local/root";
    QTDIR		     = "/opt/local/qt";										
    LD_LIBRARY_PATH	     = "/opt/local/root/lib:/opt/local/qt/lib:/opt/local/xerces/lib:" + MonicelliDir + "/plugins/libs/";
    MERGER_INPUT_DIR	     = BaseDataDir;									      
    MERGER_OUTPUT_DIR        = BaseDataDir + "Merged";								      
    #CHEWIEDIR  	     = chewieDir;									      
    CHEWIEDATADIR	     = BaseDataDir + "MonicelliOutput";							      
    CHEWIEINPUTDIR	     = BaseDataDir + "ChewieInput";  							      
    CHEWIEOUTPUTDIR	     = BaseDataDir + "ChewieOutput"; 							      
    CHEWIEXMLDIR	     = ChewieDir + "xml"									      
	

###########################################################################################
def convertEnvVariables(name, parameters):
    #All Env variables preceded by $ will be substitute
    expression = "(\$(\(|\{)*(\w+)(\)|\})*)";
    regExp = re.search(expression,name);
    while( regExp ):
    	variable = getattr(parameters, regExp.group(3));
	#print "VARIABLE:"
	#print variable;
        shortName = name[name.find(regExp.group(1))+len(regExp.group(1)):len(name)]
	#if(varList == [] or regExp.group(3) in varList): 
	#  if( not variable ):
	#    exit("You must set " + regExp.group(3) + " if you want to use it!");
	name = name.replace(regExp.group(1),variable);
	regExp = re.search(expression,shortName);
    return name

#################################################################################################
def parseMegascriptConfigurationFile(configurationFile):
    pars = Parameters();
    tree = ET.parse(configurationFile)
    root = tree.getroot()
    #root = ET.fromstring(country_data_as_string)
    for child in root:
        print child.tag
	if(child.tag == "General"):
	    pars.MaxThreads	   = int(child.find("MaxThreads").text);
	    pars.CopyRawFiles	   = bool(distutils.util.strtobool((child.find("CopyRawFiles").text).replace(" ","").replace('\t',"")));
	    pars.MergeRawFiles     = bool(distutils.util.strtobool((child.find("MergeRawFiles").text).replace(" ","").replace('\t',"")));
	    pars.RunMonicelli	   = bool(distutils.util.strtobool((child.find("RunMonicelli").text).replace(" ","").replace('\t',"")));
	    pars.RunChewie	   = bool(distutils.util.strtobool((child.find("RunChewie").text).replace(" ","").replace('\t',"")));
	   
	    pars.TestBeamYear	   = (child.find("TestBeamYear").text).replace(" ","").replace('\t',"");
	    pars.TestBeamMonth     = (child.find("TestBeamMonth").text).replace(" ","").replace('\t',"");
	    pars.TestBeamMonthName = (child.find("TestBeamMonthName").text).replace(" ","").replace('\t',"");
	    pars.TestBeamYearMonth = convertEnvVariables((child.find("TestBeamYearMonth").text).replace(" ","").replace('\t',""), pars);
	    pars.BaseDataDir	   = convertEnvVariables((child.find("BaseDataDir").text).replace(" ","").replace('\t',""), pars);
	    pars.MergedDataDir     = convertEnvVariables((child.find("MergedDataDir").text).replace(" ","").replace('\t',""), pars);
	    pars.GeometriesDataDir = convertEnvVariables((child.find("GeometriesDataDir").text).replace(" ","").replace('\t',""), pars);
	    pars.FileNameToProcess = (child.find("FileNameToProcess").text).replace(" ","").replace('\t',"");
	    pars.MailList	   = (child.find("MailList").text).replace(" ","").replace('\t',"");
	    pars.TemplateFilesDir  = (child.find("TemplateFilesDir").text).replace(" ","").replace('\t',"");

        elif(child.tag == "CopyRawFiles"):
	    pars.RemoteComputer      = (child.find("RemoteComputer").text).replace(" ","").replace('\t',"");
	    pars.RemoteDir	     = convertEnvVariables((child.find("RemoteDir").text).replace(" ","").replace('\t',""), pars);

        elif(child.tag == "Monicelli"):
	    pars.MonicelliDir			  = convertEnvVariables((child.find("MonicelliDir").text).replace(" ","").replace('\t',""), pars);
	    pars.MonicelliOutDataDir		  = convertEnvVariables((child.find("MonicelliOutDataDir").text).replace(" ","").replace('\t',""), pars);
	    pars.GeometryFile			  = (child.find("GeometryFile").text).replace(" ","").replace('\t',"");
	    pars.MonicelliExpressTemplateFilePath = convertEnvVariables((child.find("MonicelliExpressTemplateFilePath").text).replace(" ","").replace('\t',""), pars);
	    pars.MonicelliExpressTemplateFileName = convertEnvVariables((child.find("MonicelliExpressTemplateFileName").text).replace(" ","").replace('\t',""), pars);

        elif(child.tag == "Chewie"):
	    pars.ChewieDir			     = convertEnvVariables((child.find("ChewieDir").text).replace(" ","").replace('\t',""), pars);
	    pars.ChewieExpressTemplateFilePath       = convertEnvVariables((child.find("ChewieExpressTemplateFilePath").text).replace(" ","").replace('\t',""), pars);
	    pars.ChewieExpressTemplateFileName       = convertEnvVariables((child.find("ChewieExpressTemplateFileName").text).replace(" ","").replace('\t',""), pars);
	    pars.ChewieConfigurationFilePath	     = convertEnvVariables((child.find("ChewieConfigurationFilePath").text).replace(" ","").replace('\t',""), pars);
	    pars.ChewieConfigurationFileName	     = convertEnvVariables((child.find("ChewieConfigurationFileName").text).replace(" ","").replace('\t',""), pars);
            pars.ChewieConfigurationTemplateFileName = convertEnvVariables((child.find("ChewieConfigurationTemplateFileName").text).replace(" ","").replace('\t',""), pars);

        elif(child.tag == "MonicelliExpressTemplate"):
    	    pars.FilesPath	       = convertEnvVariables((child.find("FilesPath").text).replace(" ","").replace('\t',""), pars);
    	    pars.GeometriesPath        = convertEnvVariables((child.find("GeometriesPath").text).replace(" ","").replace('\t',""), pars);
    	    pars.TrackFindingAlgorithm = (child.find("TrackFindingAlgorithm").text).replace(" ","").replace('\t',"");
    	    pars.TrackFittingAlgorithm = (child.find("TrackFittingAlgorithm").text).replace(" ","").replace('\t',"");
    	    pars.NumberOfEvents        = (child.find("NumberOfEvents").text).replace(" ","").replace('\t',"");
    	    pars.TrackPoints	       = (child.find("TrackPoints").text).replace(" ","").replace('\t',"");
    	    pars.MaxPlanePoints        = (child.find("MaxPlanePoints").text).replace(" ","").replace('\t',"");
    	    pars.Chi2Cut	       = (child.find("Chi2Cut").text).replace(" ","").replace('\t',"");
    	    pars.XTolerance	       = (child.find("XTolerance").text).replace(" ","").replace('\t',"");
    	    pars.YTolerance	       = (child.find("YTolerance").text).replace(" ","").replace('\t',"");
    	    pars.FindDut	       = (child.find("FindDut").text).replace(" ","").replace('\t',"");
    	    pars.FineAlignment         = (child.find("FineAlignment").text).replace(" ","").replace('\t',"");
	    #print "-" + pars.FineAlignment + "-"
	    #exit();
    	    pars.FineTelescopeAlignment= (child.find("FineTelescopeAlignment").text).replace(" ","").replace('\t',"");
    	    pars.UseEtaFunction        = (child.find("UseEtaFunction").text).replace(" ","").replace('\t',"");

        elif(child.tag == "EnvironmentVariables"):
    	    #pars.monicelliDir  	  = convertEnvVariables((child.find("MonicelliDir").text);  	 
    	    pars.Monicelli_RawData_Dir   = convertEnvVariables((child.find("Monicelli_RawData_Dir").text).replace(" ","").replace('\t',""), pars);   
    	    pars.Monicelli_DataSample_Dir= convertEnvVariables((child.find("Monicelli_DataSample_Dir").text).replace(" ","").replace('\t',""), pars);
    	    pars.Monicelli_CalSample_Dir = convertEnvVariables((child.find("Monicelli_CalSample_Dir").text).replace(" ","").replace('\t',""), pars); 
    	    pars.MonicelliOutputDir	 = convertEnvVariables((child.find("MonicelliOutputDir").text).replace(" ","").replace('\t',""), pars);	 
    	    pars.Monicelli_XML_Dir	 = convertEnvVariables((child.find("Monicelli_XML_Dir").text).replace(" ","").replace('\t',""), pars);	 
    	    pars.ROOTSYS		 = convertEnvVariables((child.find("ROOTSYS").text).replace(" ","").replace('\t',""), pars);		  
    	    pars.QTDIR  		 = convertEnvVariables((child.find("QTDIR").text).replace(" ","").replace('\t',""), pars);  		  
    	    pars.LD_LIBRARY_PATH	 = convertEnvVariables((child.find("LD_LIBRARY_PATH").text).replace(" ","").replace('\t',""), pars);	  
    	    pars.MERGER_INPUT_DIR	 = convertEnvVariables((child.find("MERGER_INPUT_DIR").text).replace(" ","").replace('\t',""), pars);	  
    	    pars.MERGER_OUTPUT_DIR	 = convertEnvVariables((child.find("MERGER_OUTPUT_DIR").text).replace(" ","").replace('\t',""), pars);	 
    	    #pars.CHEWIEDIR		  = convertEnvVariables((child.find("CHEWIEDIR").text).replace(" ","").replace('\t',""), pars);		  
    	    pars.CHEWIEDATADIR  	 = convertEnvVariables((child.find("CHEWIEDATADIR").text).replace(" ","").replace('\t',""), pars);  	  
    	    pars.CHEWIEINPUTDIR 	 = convertEnvVariables((child.find("CHEWIEINPUTDIR").text).replace(" ","").replace('\t',""), pars); 	  
    	    pars.CHEWIEOUTPUTDIR	 = convertEnvVariables((child.find("CHEWIEOUTPUTDIR").text).replace(" ","").replace('\t',""), pars);	  
    	    pars.CHEWIEXMLDIR		 = convertEnvVariables((child.find("CHEWIEXMLDIR").text).replace(" ","").replace('\t',""), pars);
    return pars;
	    	     
###########################################################################################
def exportVars(parameters):
    os.environ['MonicelliDir'            ] = parameters.MonicelliDir;           
    os.environ['Monicelli_RawData_Dir'   ] = parameters.Monicelli_RawData_Dir;   
    os.environ['Monicelli_DataSample_Dir'] = parameters.Monicelli_DataSample_Dir;
    os.environ['Monicelli_CalSample_Dir' ] = parameters.Monicelli_CalSample_Dir; 
    os.environ['MonicelliOutputDir'      ] = parameters.MonicelliOutputDir;      
    os.environ['Monicelli_XML_Dir'       ] = parameters.Monicelli_XML_Dir;       
    os.environ['ROOTSYS'                 ] = parameters.ROOTSYS;                 
    os.environ['QTDIR'                   ] = parameters.QTDIR;                   
    os.environ['LD_LIBRARY_PATH'         ] = parameters.LD_LIBRARY_PATH;         
    os.environ['MERGER_INPUT_DIR'        ] = parameters.MERGER_INPUT_DIR;        
    os.environ['MERGER_OUTPUT_DIR'       ] = parameters.MERGER_OUTPUT_DIR;       
    os.environ['CHEWIEDIR'               ] = parameters.ChewieDir;               
    os.environ['CHEWIEDATADIR'           ] = parameters.CHEWIEDATADIR;           
    os.environ['CHEWIEINPUTDIR'          ] = parameters.CHEWIEINPUTDIR;          
    os.environ['CHEWIEOUTPUTDIR'         ] = parameters.CHEWIEOUTPUTDIR;         
    os.environ['CHEWIEXMLDIR'            ] = parameters.CHEWIEXMLDIR;            

###########################################################################################
def makeMonicelliExpressTemplate(fileOut, parameters):
    outFile = open(fileOut,"w")
    outFile.write("<?xml version='1.0' encoding='iso-8859-1'?>\n");
    outFile.write("<!DOCTYPE MonicelliExpressConfiguration SYSTEM \"./dtd/ExpressConfiguration.dtd\">\n");
    outFile.write("<MonicelliExpressConfiguration>\n");
    outFile.write("  <Defaults FilesPath=\"" + parameters.FilesPath + "\"\n");
    outFile.write("	       GeometriesPath=\"" + parameters.GeometriesPath + "\"\n");
    outFile.write("	       TrackFindingAlgorithm=\"" + parameters.TrackFindingAlgorithm + "\"\n");
    outFile.write("	       TrackFittingAlgorithm=\"" + parameters.TrackFittingAlgorithm + "\"\n");
    outFile.write("	       NumberOfEvents=\"" + parameters.NumberOfEvents + "\"\n");
    outFile.write("	       TrackPoints=\"" + parameters.TrackPoints + "\"\n");
    outFile.write("	       MaxPlanePoints=\"" + parameters.MaxPlanePoints + "\"\n");
    outFile.write("	       Chi2Cut=\"" + parameters.Chi2Cut + "\"\n");
    outFile.write("	       XTolerance=\"" + parameters.XTolerance + "\"\n");
    outFile.write("	       YTolerance=\"" + parameters.YTolerance + "\"\n");
    outFile.write("	       FindDut=\"" + parameters.FindDut + "\"\n");
    outFile.write("	       FineAlignment=\"" + parameters.FineAlignment + "\"\n");
    outFile.write("	       FineTelescopeAlignment=\"" + parameters.FineTelescopeAlignment + "\"\n");
    outFile.write("	       UseEtaFunction=\"" + parameters.UseEtaFunction + "\"/>\n");
    outFile.write("</MonicelliExpressConfiguration>\n");
    outFile.close();
    f = open(fileOut, 'r')
    print f.read()
    f.close()

###########################################################################################
def makeChewieExpressTemplate(fileOut, parameters):
    outFile = open(fileOut,"w")
    outFile.write("<?xml version='1.0' encoding='iso-8859-1'?>\n");
    outFile.write("<!DOCTYPE ChewieExpressConfiguration SYSTEM \"./dtd/ExpressConfiguration.dtd\">\n");
    outFile.write("<ChewieExpressConfiguration>\n");
    outFile.write("  <Defaults FilesPath=\"" + parameters.MonicelliOutDataDir + "\"\n");
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
def monicelliExpressTemplateReplace(fileIn, fileOut, runList, geometry):
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
def chewieExpressTemplateReplace(parameters, fileIn, fileOut, runList):
    testFile = open(fileIn,"r")
    testFileContent = testFile.readlines()
    testFile.close()
    i=0
    outFile = open(fileOut,"w")
    for line in testFileContent:
       if(line.find("</ChewieExpressConfiguration>") != -1):
         for run in runList:
	     outFile.write("    <Files Configuration=\"" + parameters.ChewieConfigurationFileName + "\" OutFileName=\"" + run.replace(".dat","") + "_Chewie.root\">\n      <File Name=\"" + run.replace(".dat",".root")  + "\"/>\n    </Files>\n");
       outFile.write(line)
    outFile.close()

###########################################################################################
def makeFileListToProcess(parameters, Min, Max):
    runList = []
    i=Min
    for i in range(Min,Max+1):
       s = parameters.FileNameToProcess.format(no=i)
       runList.append(s)
       print "Files to process: " + s
    return runList

###########################################################################################
def sendEmail(mailList, title, body):
    if(mailList == ""): return;
    print "Sending email to " + mailList + " with body:\n" + body
    list = mailList.split(',')
    for email in list:
        p = os.popen("mail -s \"" + title + "\" " + email ,"w")
        p.write(body)
        status = p.close() 


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


############################################################################################
def main():

    ###########################################################################################
    #Parsing Arguments 
    ###########################################################################################
    parser = argparse.ArgumentParser(formatter_class=RawTextHelpFormatter)
    parser.add_argument("-r", "--runlist", required=True, type=str, help="Run list.\n"
    "Examples of run list:\n"
    "1 -> only Run1\n"
    "1-10 -> from Run1 to Run10\n"
    "1-10,20,30-35 -> from Run1 to Run10, Run20, from Run30 to Run35"
    )
    parser.add_argument("-c", "--configuration", help="Megascript xml configuration file")
    parser.add_argument("-d", "--delete",        help="Removes all the template files", action='store_true')
    parser.add_argument("-e", "--events",        type=int, help="Number of events to process.(-1 means all)")
    parser.add_argument("-f", "--fit",           type=str, help="Monicelli track fit (Simple, Kalman)")
    parser.add_argument("-g", "--geometry",      type=str, help="Geometry file name.")
    parser.add_argument("--runmonicelli",        default=True, type=lambda x:bool(distutils.util.strtobool(x)), help="Run Monicelli (default True).")
    parser.add_argument("--runchewie",           default=True, type=lambda x:bool(distutils.util.strtobool(x)), help="Run Chewie (default True).")
    parser.add_argument("--chewietemplate",      type=str, help="Chewie template file name for ChewieExpress.")
    parser.add_argument("--chewieconfiguration", type=str, help="Chewie analysis configuration file.")

    parser.parse_args()
    args = parser.parse_args()
    
    ###########################################################################################
    #Creating run list
    ###########################################################################################
    rangedFileList = args.runlist.split(',');
    runList = [];
    print "Printing Runs"
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
	print runs;
	for run in range(int(begin),int(end)+1):
	    runList.append(str(run));

    parameters = Parameters();
    if(args.configuration):
	parameters = parseMegascriptConfigurationFile(args.configuration);

    if(args.events):
	parameters.NumberOfEvents = str(args.events);

    if(args.fit):
	if(args.fit != "Simple" and args.fit != "Kalman"):
	    print "fit parameter can only be Simple or Kalman, not " + args.fit;
	    exit();
	parameters.TrackFittingAlgorithm = args.fit;

    if(args.geometry):
	parameters.GeometryFile = args.geometry;

    if(args.chewietemplate):
	parameters.ChewieExpressTemplateName = args.chewietemplate;

    if(not args.runmonicelli): #The default must be True so only when we don't want to run it then we get in here
        print "I change running"
	parameters.RunMonicelli = args.runmonicelli;

    if(not args.runchewie): #The default must be True so only when we don't want to run it then we get in here
	parameters.RunChewie = args.runchewie;
    
    if(args.chewieconfiguration):
	parameters.ChewieConfigurationFileName = args.chewieconfiguration;

    if(args.delete):
 	cmd = "rm " + parameters.MonicelliExpressTemplateFilePath + parameters.MonicelliExpressTemplateFileName + " " + parameters.ChewieExpressTemplateFilePath + parameters.ChewieExpressTemplateFileName;
 	print "Running command: " + cmd;
 	cleanProcess = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
	out,err = cleanProcess.communicate();

    #Done parsing arguments
    exportVars(parameters);

    destinationHost, destinationDir = splitHostDir(os.environ['Monicelli_RawData_Dir']);

    if(parameters.CopyRawFiles):
    	#Making source files list
    	sourceHost, sourceDir = splitHostDir(parameters.RemoteComputer + ':' + parameters.RemoteDir + "Analysis/Merged/");
    	print "Reading data from host: " + sourceHost
    	print "Stored in directory: " + sourceDir
#   	 sourceList = makeFileList(sourceHost, sourceDir, ".dat");

    	#Making destination files list
    	for run in runList:
           rawCopy(int(run), int(run));
   
    if(parameters.MergeRawFiles):
    	for run in runList:
            cmd = baseDataDir + "../Exes/Merger/Merger " + run;
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
    fileList = [];
    print runList
    for run in runList:
        fileList += makeFileListToProcess(parameters, int(run), int(run));

    #print fileList;


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
    monicelliTemplateFile = parameters.MonicelliExpressTemplateFilePath + parameters.MonicelliExpressTemplateFileName;
    if not os.path.isfile(monicelliTemplateFile):
        print "WARNING: I couldn't find the MonicelliExpress template file " + monicelliTemplateFile + ", so I am creating one using the template at the top of the megascript!"
        makeMonicelliExpressTemplate(monicelliTemplateFile, parameters);
    print monicelliTemplateFile;
    
    ################################################################################################################
    #If Chewie Express template does not exist then create it...
    chewieTemplateFile = parameters.ChewieExpressTemplateFilePath + parameters.ChewieExpressTemplateFileName;
    if not os.path.isfile(chewieTemplateFile):
        print "WARNING: I couldn't find the ChewieExpress template file " + chewieTemplateFile + ", so I am creating one using the template at the top of the megascript!"
        makeChewieExpressTemplate(chewieTemplateFile, parameters);
    print chewieTemplateFile;

    ################################################################################################################
    #If Chewie Configuration template does not exist then create it...
    chewieConfigurationFile = parameters.ChewieConfigurationFilePath + parameters.ChewieConfigurationFileName;
    if not os.path.isfile(chewieConfigurationFile):
        print "WARNING: I couldn't find the Chewie Configuration file " + chewieConfigurationFile + ", so I am creating one using the template at the top of the megascript!"
 	cmd = "cp " + parameters.ChewieConfigurationTemplateFileName + " " + chewieConfigurationFile;
 	print "Running command: " + cmd;
 	copyProcess = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
	out,err = copyProcess.communicate();
        

    maxThreads = parameters.MaxThreads;
    if (maxThreads > len(fileList)):
        maxThreads = len(fileList);
    
    firstState = 0;
    if(not parameters.RunMonicelli):
    	firstState = 2;
    
    
    processes        = [None]       * len(fileList);
    output           = [""]         * len(fileList);
    error            = [""]         * len(fileList);
    status           = [firstState] * len(fileList);#0=ready, 1=Running Monicelli, 2=Done running Monicelli, 3=Running Chewie, 4=Done running Chewie
    nOfIncorrectData = [0]          * len(fileList);
    runName          = [""]         * len(fileList);
    outFiles         = [None]       * len(fileList);
    
    done = False;
    runningProcesses = 0;
    lastState = 4;#run Monicelli and Chewie
    if(not parameters.RunChewie):
	lastState = 2;#Only Monicelli
    while(not done):
    	for p in range (0, len(processes)):
	    #print "Process: " +str(p) + " status: " + str(status[p]);
       	    runName[p] = fileList[p].replace(".dat","");
	    if(status[p] == 0 and runningProcesses < maxThreads):
		status[p] = 1;
		################################################################################################################
 	    	#Run Monicelli
 	    	print "Running Monicelli for " + runName[p];
 
 	    	#Create Express xml files from template
 	    	templateOutName = parameters.MonicelliExpressTemplateFileName.replace("template", runName[p]);
 	    	#print templateOutName;

 	    	monicelliExpressTemplateReplace(monicelliTemplateFile, parameters.MonicelliExpressTemplateFilePath + templateOutName, [fileList[p]], parameters.GeometryFile);
 
 	    	#Run Monicelli express
		cmd = "cd " + parameters.MonicelliDir + "Express; ./MonicelliExpress " + templateOutName;
 	    	print "Running command: " + cmd;
                outFiles[p] = open("Monicelli_stdout_" + fileList[p], 'w');
 	    	processes[p] = subprocess.Popen(cmd, shell=True, stdout=outFiles[p], stderr=subprocess.PIPE);
		runningProcesses += 1;
	    
	    elif(status[p] == 1):
	        #print "Checking process " + str(p)
		if not (processes[p].poll() is None):
		    outFiles[p].close();
	            #print "Communicating..."
		    nada,error = processes[p].communicate();
		    status[p] = 2;
		    outFiles[p] = open("Monicelli_stdout_" + fileList[p],'r');
		    out = "";
		    for line in iter(outFiles[p]):
                        out += line;
		    outFiles[p].close();
		    #print "ERROR:" + error;
		    print out;
		    cmd = "rm Monicelli_stdout_" + fileList[p];
 	    	    subprocess.Popen(cmd, shell=True);
		    
 		    if((error != '') and ('Error in <TCling' not in error)):
 		       print error
 		       error = "FATAL: There was an error executing Monicelli!"
 		       sendEmail(parameters.MailList, runName[p], error);
 
 		    for line in out.split('\n'):
 		    	#print line;
		    	if(line.find("Incorrect data at block") != -1):
		    	    nOfIncorrectData[p] += 1;	    
                    if(not parameters.RunChewie):
		    	runningProcesses -= 1;
		    	print "Number of process running " + str(runningProcesses);
#	    	    else:
#	            	print "Reading out..."
#		    	for line in processes[p].stdout:
#		    	    output[p] += line;
#	            	print "Reading err..."
#		    	for line in processes[p].stderr:
#		    	    error[p]  += line;
		    
 	    elif(parameters.RunChewie and status[p] == 2):
 		status[p] = 3;
 		#Run Chewie
 		print "Running Chewie for " + runName[p];
 		templateOutName = parameters.ChewieExpressTemplateFileName.replace("template",runName[p]);
 		chewieExpressTemplateReplace(parameters, chewieTemplateFile, parameters.ChewieExpressTemplateFilePath + templateOutName, [fileList[p]]);
 		cmd = "cd " + parameters.ChewieDir + "Express; ./ChewieExpress " + templateOutName;
 		print "Running command: " + cmd;
                outFiles[p] = open("Chewie_stdout_" + fileList[p], 'w');
 		processes[p] = subprocess.Popen(cmd, shell=True, stdout=outFiles[p], stderr=subprocess.PIPE);
 	    
 	    elif(status[p] == 3):
 		 #print "Checking chewie: " + str(p);
		 if not (processes[p].poll() is None):
		    outFiles[p].close();
		    nada,error = processes[p].communicate();
		    outFiles[p] = open("Chewie_stdout_" + fileList[p],'r');
		    out = "";
		    for line in iter(outFiles[p]):
                        out += line;
		    outFiles[p].close();
		    print out;
		    cmd = "rm Chewie_stdout_" + fileList[p];
 	    	    subprocess.Popen(cmd, shell=True);
  	    	    status[p] = 4;
		    runningProcesses -= 1;
		    print "Number of process running " + str(runningProcesses);
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
 		    	  sendEmail(parameters.MailList, runName[p], realError);
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
 		    sendEmail(parameters.MailList, runName[p], emailBody);

            done = True;
            for p in range (0, len(processes)):
            	if(status[p] != lastState):
	    	    done = False;
	    	    break;
            if(not done):
	        time.sleep(1)
    #sendEmail(parameters.MailList, "Megascript Done!", "");
############################################################################################
if __name__ == "__main__":
    main()



