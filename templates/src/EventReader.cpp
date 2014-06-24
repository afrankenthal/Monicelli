/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
** This class is provided as a service to deal with the details of how to 
** extract events with hits and recnstructed tracks from a file produced by 
** Monicelli.
**
** NOTE:
**     casual users are not supposed nor required to modify or even understand 
**     implementation details of this class, see instead the eventAnalyzer.ccp 
**     files for a template of your own analysis algorithm                     
**
****************************************************************************/

#include "EventReader.h"
//#include <QRegExp>
//#include <QString>

//====================================================================
EventReader::EventReader(void) :
  inputGeometryTree_(0),
  inputEventTree_   (0),
  inputEventHeader_ (0),
  theEvent_	    (new Event()),
  theEventHeader_   (new EventHeader()),
  theEventBranch_   (0),
  theEventTree_     (0),
  theGeometry_      (new Geometry()),
  path_ 	    (""),
  runNumber_        (-1)
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
  delete theEvent_;
  delete theEventHeader_;
  delete theGeometry_;
}
//====================================================================
bool EventReader::openEventsFile(std::string inputFileName)
{
  std::stringstream ss;
  inputEventTree_ = 0;
  
  std::string fullPath = path_ + inputFileName ;
  inputEventsFile_ = new TFile( fullPath.c_str(), "read" );
  
  STDLINE("WARNING: I am searching for a run number that is only 3 digit long in a file name that is RunXXX_Merged.dat. If the file name is different you have to fix the line after this one!", ACRed) ;
  std::string runNumberS = inputFileName.substr(3, inputFileName.length()-7);
  STDLINE (runNumberS, ACYellow);
  runNumber_ = atoi(runNumberS.substr(0,4).c_str());

  if( !inputEventsFile_->IsOpen() )
  {
     STDLINE(std::string("Could not open file ") + fullPath, ACRed) ;
     exit(EXIT_FAILURE) ;
  }
  
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

     if( (inputEventHeader_ = (TTree*)inputEventsFile_->Get(eventsHeaderName.c_str())))
     {
       inputEventHeader_->SetBranchAddress("EventHeader",    &theEventHeader_);
       inputEventHeader_->GetEntry(0);
 
       ss.str("");
       ss << "Found " << inputEventHeader_->GetEntries() << " headers";
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
  inputGeometryFile_ = new TFile( fullPath.c_str(), "read" );
  
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
