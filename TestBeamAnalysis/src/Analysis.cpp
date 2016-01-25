/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
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
