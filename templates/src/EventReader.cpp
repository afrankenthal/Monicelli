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
 
/****************************************************************************
** This class is provided as a service to deal with the details of how to 
** extract events with hits and reconstructed tracks from a file produced by 
** Monicelli.
**
** NOTE:
**     casual users are not supposed nor required to modify or even understand 
**     implementation details of this class, see instead the eventAnalyzer.ccp 
**     files for a template of your own analysis algorithm                     
**
****************************************************************************/

#include "EventReader.h"
#include "Event.h"
#include "EventHeader.h"
#include "Geometry.h"
#include "MessageTools.h"
#include <TFile.h>
#include <TBranch.h>
#include <TTree.h>

#include <iostream>

//====================================================================
EventReader::EventReader(void) :
  inputGeometryTree_    (0),
  inputEventTree_       (0),
  inputEventHeaderTree_ (0),
  theEvent_	        (0),
  theEventHeader_       (0),
  //theEventBranch_       (0),
  //theEventTree_         (0),
  theGeometry_          (0),
  path_ 	        (""),
  runNumber_            (-1)
{
  STDLINE("                                   ", ACCyan) ;
  STDLINE("+=================================+", ACCyan) ;
  STDLINE("|                                 |", ACCyan) ;
  STDLINE("|    Welcome to readerTemplate    |", ACCyan) ;
  STDLINE("|                                 |", ACCyan) ;
  STDLINE("| Template analyzer of data files |", ACCyan) ;
  STDLINE("|  aligned and reconstructed by   |", ACCyan) ;
  STDLINE("|        M o n i c e l l i        |", ACCyan) ;
  STDLINE("|                                 |", ACCyan) ;
  STDLINE("+=================================+", ACCyan) ;
  STDLINE("                                   ", ACCyan) ;
   
}

//====================================================================
EventReader::~EventReader(void)
{
  //delete theEvent_;
  //delete theEventHeader_;
  //delete theGeometry_;
}

//====================================================================
TTree* EventReader::cloneEventTree  (void)
{
  if(inputEventTree_ != 0)
    return inputEventTree_->CloneTree(0);
  else
    return 0;
} 

//====================================================================
TTree* EventReader::cloneEventHeaderTree(void)
{
  if(inputEventHeaderTree_ != 0)
    return inputEventHeaderTree_->CloneTree(0);
  else
    return 0;
}

//====================================================================
bool EventReader::openEventsFile(std::string inputFileName)
{
  std::stringstream ss;
  inputEventTree_ = 0;
  
  std::string fullPath = path_ + inputFileName ;
  
  unsigned int endRunNumber = inputFileName.find(".dat");
  
  if(inputFileName.find("_") != std::string::npos) endRunNumber = inputFileName.find("_"); 
  
  unsigned int runNumberLength = endRunNumber-3;
  STDLINE("WARNING: I am searching for a run number that is only " 
          << runNumberLength 
          << " digit long in a file name that is RunXXX_Merged.dat. If the file name is different you have to fix the line after this one!", ACRed) ;
  std::string runNumberS = inputFileName.substr(3, runNumberLength);
  STDLINE (runNumberS, ACYellow);
  runNumber_ = atoi(runNumberS.substr(0,runNumberLength).c_str());

  inputEventsFile_ = TFile::Open( fullPath.c_str(), "READ" );
  if( !inputEventsFile_->IsOpen() )
  {
     STDLINE(std::string("Could not open file ") + fullPath, ACRed) ;
     exit(EXIT_FAILURE) ;
  }
  fileName_ = inputFileName;
  ss.str(""); ss << "File " << ACYellow << fullPath << ACGreen << " successfully opened" ;
  STDLINE(ss.str(), ACGreen) ;
  STDLINE("       ", ACGreen) ;
  
  std::string eventsTreeName   = inputFileName + "Events";
  std::string eventsHeaderName = inputFileName + "Header";
  
  STDLINE("",ACWhite) ; STDLINE("----- File content ------", ACYellow) ;
  inputEventsFile_->ls() ;
  STDLINE("--------------------------", ACYellow) ;STDLINE("",ACWhite) ;

  STDLINE(eventsTreeName,ACPurple) ;
  
  STDLINE("Checking for EventBranch...",ACWhite) ;
  if ( (inputEventTree_   = (TTree*)inputEventsFile_->Get(eventsTreeName.c_str())) )
  {
     inputEventTree_    ->SetBranchAddress("EventBranch",    &theEvent_      );

     ss.str("");
     ss << "Found " << inputEventTree_->GetEntries() << " events on file";
     STDLINE(ss.str(),ACGreen) ;

     if( (inputEventHeaderTree_ = (TTree*)inputEventsFile_->Get(eventsHeaderName.c_str())))
     {
       inputEventHeaderTree_->SetBranchAddress("EventHeader",    &theEventHeader_);
       inputEventHeaderTree_->GetEntry(0);
 
       ss.str("");
       ss << "Found " << inputEventHeaderTree_->GetEntries() << " headers";
       STDLINE(ss.str(),ACGreen) ;
     }
  }
  
  if( !inputEventTree_ )
  {
    STDLINE("FATAL: no branches found in file. Aborting",ACRed) ;
    return false ;
  }
  
  return true;
 }

//====================================================================
bool EventReader::openGeoFile(std::string inputFileName)
{
  std::stringstream ss;
  std::string fullPath = path_ + inputFileName ;
  STDLINE(path_        ,ACPurple) ;
  STDLINE(inputFileName,ACPurple) ;
  inputGeometryFile_ = TFile::Open( fullPath.c_str(), "READ" );
  
  if( !inputGeometryFile_->IsOpen() )
  {
     STDLINE(std::string("Could not open file ") + fullPath, ACRed) ;
     return false;
  }
  
  ss.str(""); ss << "File " << ACYellow << fullPath << ACGreen << " successfully opened" ;
  STDLINE(ss.str(), ACGreen) ;
  STDLINE("       ", ACGreen) ;
  
  std::string geometryTreeName = "Geometry";
  STDLINE("Checking for GeometryBranch...",ACWhite) ;
  if ( (inputGeometryTree_ = (TTree*)inputGeometryFile_->Get(geometryTreeName.c_str())) )
  {
     inputGeometryTree_->SetBranchAddress("GeometryBranch", &theGeometry_   );
     inputGeometryTree_->GetEntry(0);
     ss.str("");
     ss << "Found " << inputGeometryTree_->GetEntries() << " geometry entries";
     STDLINE(ss.str(),ACGreen) ;
  }
  return (inputGeometryTree_ != 0);
}

//====================================================================
void EventReader::closeInputFiles(void)
{
  if(inputEventsFile_ != 0)
  {
    inputEventsFile_->Close();
    inputEventsFile_ = 0;
  }
  if(inputGeometryFile_ != 0)
  {
    inputGeometryFile_->Close();
    inputGeometryFile_ = 0;
  }
}

//====================================================================
unsigned int EventReader::getNumberOfEvents(void)
{
  return inputEventTree_->GetEntries();
}

//====================================================================
std::string EventReader::getFileName(void)
{
  return path_+fileName_;
}

//====================================================================
void EventReader::readEvent(unsigned int event)
{
  inputEventTree_->GetEntry(event);
}

//====================================================================
void EventReader::setPath(std::string path)
{
  path_ = path;
}	  

//====================================================================
Event* EventReader::getEventPointer(void)
{
  return theEvent_;
}

//====================================================================
EventHeader* EventReader::getEventHeaderPointer(void)
{
  return theEventHeader_;
}

//====================================================================
Geometry* EventReader::getGeometryPointer(void)
{
  return theGeometry_;
}

//====================================================================
TTree* EventReader::getEventTreePointer(void)
{
  return inputEventTree_;
}

//====================================================================
TTree* EventReader::getEventHeaderTreePointer(void) 
{
  return inputEventHeaderTree_;
}

//====================================================================
unsigned int EventReader::getRunNumber(void) 
{
  return runNumber_;
}
