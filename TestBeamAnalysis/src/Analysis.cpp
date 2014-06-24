#include <iostream>
#include <fstream>
#include <vector> 
#include <sstream>
#include <string>

#include "HManager.h"
#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include "Utils.h"
#include "../include/MessageTools.h"

using namespace std;

int main(int argv, char **args)
{
  if( argv < 4 ) 
  {
    STDLINE("FATAL: not enough arguments",ACRed ) ;
    STDLINE("", ACWhite);
    STDLINE("  Usage: ", ACCyan) ;
    STDLINE("    bin/TBAnalysis input-dir output-dir input-file1 input-file2 ... input-filen", ACYellow) ;
    exit(0) ;
  }

  std::stringstream ss_ ;
    
  std::string inputPathDir  = args[1] ;
  std::string outputPathDir = args[2] ;
  
  std::string  inFileName;
  std::string  outFileName;
  TFile*  inFile;
  TTree*  theTree;

  inFileName  = inputPathDir  + std::string("/") + args[3];
  outFileName = outputPathDir + std::string("/ChewieTest.root");

  ss_.str(""); ss_ << "Opening file: " << inFileName ; STDLINE(ss_.str(),ACWhite);
  ss_.str(""); ss_ << "Ouput to    : " << outFileName; STDLINE(ss_.str(),ACWhite);  
  
  inFile  = new TFile(TString(inFileName.c_str()),"READ");
  theTree = (TTree*)inFile->Get("CaptanTrack"); 
  
  HManager theHManager(0);
  theHManager.openFile(TString(outFileName.c_str()));
  theHManager.book();
  //theHManager.fill();
  //inFile->Close();
  //STDLINE("Deleting file",ACGreen);
  //delete inFile;

  for(int i=3; i<argv; ++i)
  { 
    inFileName  = inputPathDir  + std::string("/") + args[i];
    inFile      = new TFile(TString(inFileName.c_str()),"READ");
    theTree     = (TTree*)inFile->Get("CaptanTrack");   
    if(theTree)
    {      
       theHManager.setWindow(theTree);    
     }
     else STDLINE(std::string("Error could not find file: ") + inFileName,ACRed) ;      
  }
         
  for(int i=3; i<argv; ++i)
  { 
    inFileName  = inputPathDir  + std::string("/") + args[i];
    ss_.str(""); ss_ << "Opening file: " << inFileName ; STDLINE(ss_.str(),ACWhite) ;
    ss_.str(""); ss_ << "Ouput to    : " << outFileName; STDLINE(ss_.str(),ACWhite) ;
    inFile  = new TFile(TString(inFileName.c_str()),"READ");
    theTree = (TTree*)inFile->Get("CaptanTrack");   
    if(theTree)
    {     
       theHManager.setupTree(theTree);  
       theHManager.fill();
       inFile->Close();
       STDLINE("Deleting file",ACGreen);
       delete inFile;
     }
     else STDLINE(std::string("Error could not find file: ") + inFileName,ACRed) ;      
  }
  
  theHManager.endJob();
  theHManager.writeAndCloseFile();   
  return 0; 
   
}
