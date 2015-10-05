/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "EventReader.h"
#include "Event.h"
#include "EventHeader.h"
#include "Geometry.h"
#include <fstream>
#include <iostream>
#include <math.h>

#include <TFile.h>
#include <TTree.h>

using namespace std;

void copyEvents  (EventReader* reader, TTree* newEventTree);
bool analyzeEvent(Event* event, unsigned int eventNumber);

//====================================================================
int main(int argv, char **args)
{
  std::vector<std::string> fileNames;
  //fileNames.push_back("Run1358.root");
  //fileNames.push_back("Run1359.root");
//   fileNames.push_back("Run1361.root");
//   fileNames.push_back("Run1364.root");
//   fileNames.push_back("Run1369.root");
//   
//   fileNames.push_back("Run1363.root");
//   fileNames.push_back("Run1365.root");
//   fileNames.push_back("Run1366.root");
//   fileNames.push_back("Run1368.root");
//   fileNames.push_back("Run1370.root");
//   fileNames.push_back("Run1371.root");
//   fileNames.push_back("Run1374.root");
  
//   fileNames.push_back("Run3407_Merged.root");
   fileNames.push_back("Run3409_Merged.root");
//    fileNames.push_back("Run3411_Merged.root");
//    fileNames.push_back("Run3412_Merged.root");
//    fileNames.push_back("Run3413_Merged.root");
//    fileNames.push_back("Run3414_Merged.root");
//    fileNames.push_back("Run3416_Merged.root");

  std::vector<std::string> geoFileNames;
  for(std::vector<std::string>::iterator it=fileNames.begin(); it!=fileNames.end(); it++)
  {
    geoFileNames.push_back(*it);
    std::vector<std::string>::iterator geoFileNamesIt = geoFileNames.end()-1;
    (*geoFileNamesIt).replace((*geoFileNamesIt).size()-5, 5, ".geo");
  }  
  std::string monicelliOutputDir = string(getenv("MonicelliOutputDir")) + "/";
  
  cout << monicelliOutputDir << endl;
  


  EventReader reader; 
  reader.setPath(monicelliOutputDir);
  //WARNING -> The outfile name is tuned for a file name of the type PrefixRun#Postfix.root
  
  int prefixLength    = 3;//Run
  //int runNumberLength = 4;//3120
  int postFixLength   = 7;//_Merged
  int dotRootLength   = 5;//.root
  
  
  std::string outFileName = fileNames[0].substr(0, fileNames[0].size()-(postFixLength+dotRootLength))
                           + "_" 
			   + fileNames[fileNames.size()-1].substr(prefixLength, fileNames[fileNames.size()-1].size()-(postFixLength+dotRootLength+prefixLength))
			   + "_" + "Reduced.root";

  //std::string outFileName = fileNames[0];;
  //std::string outFileName = fileNames[0];
  TFile* outFile = TFile::Open(outFileName.c_str(),"RECREATE");
  outFile->cd();
  TTree* newEventTree       = new TTree( (outFileName+"Events").c_str(), "A tree with an Event class"  );
  TTree* newEventHeaderTree = new TTree( (outFileName+"Header").c_str(), "A tree with an Event Header"  );
  EventHeader* eventHeader;
  Event* event;
//  TTree* newEventTree	= 0;
//  TTree* newEventHeader = 0;
  for(unsigned int f=0; f<fileNames.size(); f++)
  {
    reader.openEventsFile(fileNames[f]   );
    reader.openGeoFile   (geoFileNames[f]);
    if(f==0)
    {
       eventHeader = reader.getEventHeaderPointer();
       event = reader.getEventPointer();  
       newEventTree      ->Branch("EventBranch", "Event",       &event, 16000, 0 ) ;
       newEventHeaderTree->Branch("EventHeader", "EventHeader", &eventHeader, 16000, 0 ) ;
       //newEventHeaderTree->SetBranchAddress("EventHeader", &eventHeader);
       newEventHeaderTree->Fill();
    } 
    copyEvents(&reader, newEventTree);
    reader.closeInputFiles();
  }
  
  //newEventHeader->Write();
  //newEventTree  ->Write();
  outFile->Write();
  outFile->Close();

  return 0 ;
}

//------------------------------------------------------------------------------------
void copyEvents(EventReader* reader, TTree* newEventTree)
{
  Event* event = reader->getEventPointer();  
  //newEventTree->SetBranchAddress("EventBranch", &event);

  //int runNumber = reader->getRunNumber();
  
  // Retrieve from file the number of stored events  
  unsigned int numberOfEvents = reader->getNumberOfEvents() ;
  
  std::stringstream ss;
  ss.str(""); 
  ss << "Found " << numberOfEvents << " events on file" ;
  STDLINE(ss.str(),ACYellow) ;
  STDLINE("       ",ACYellow) ;
  STDLINE("Reading and Analyzing events...",ACGreen) ;
  for(unsigned int eventNumber=0; eventNumber<numberOfEvents; eventNumber++)
  //for(unsigned int eventNumber=0; eventNumber<10; eventNumber++)
  {
    if(eventNumber%1000 == 0)
    {
      ss.str(""); 
      ss << "Progress " << (int)(((float)eventNumber/numberOfEvents)*100) << " %" ;
      STDSNAP(ss.str(),ACRed) ;
    }      
    reader->readEvent(eventNumber) ;
    if(analyzeEvent(event,eventNumber))
    {
      //cout << "Filling" << endl;
      newEventTree->Fill();
      if(newEventTree->GetEntries() >= 10000) return;
    }
  }
  STDSNAP("Progress 100 %\n",ACGreen) ;
  
  STDLINE("Writing Output File", ACRed);
  STDLINE("Done!", ACGreen);
}


//------------------------------------------------------------------------------------
bool analyzeEvent(Event* event, unsigned int eventNumber)
{

    //Event::plaqMapDef  	                          & theRawData	           = event->getRawData		     ();
    Event::clustersMapDef	                  & clusters  	           = event->getClusters		     ();
    //Event::clustersHitsMapDef                     & clustersHits	   = event->getClustersHits	     ();
    //Event::residualsMapDef	                  & fittedTrackResiduals   = event->getFittedTrackResiduals  ();
    Event::trackCandidatesDef                     & trackCandidates	   = event->getTrackCandidates       ();
    //Event::fittedTracksDef                        & fittedTracks           = event->getFittedTracks  	     ();
    Event::fittedTracksCovarianceDef              & fittedTracksCovariance = event->getFittedTracksCovariance();
    //Event::chi2VectorDef	                  & fittedTracksChi2       = event->getFittedTracksChi2      ();
    //Event::clustersMapDef&     alignedClusters       = event->getAlignedClusters()       ;
    //Event::trackCandidatesDef& alignedHitsCandidates = event->getAlignedHitsCandidates() ;

  unsigned int minPoints      = 8;
  unsigned int maxPlanePoints = 1;

   if ( clusters.size() == 0 || clusters.size() < minPoints) return false;
 
   for(Event::clustersMapDef::const_iterator clustersIt=clusters.begin(); clustersIt!=clusters.end(); clustersIt++)
   {
       if(maxPlanePoints > 0 && clustersIt->second.size() > maxPlanePoints)
	 return false;
       for (Event::aClusterMapDef::const_iterator clusterIt=clustersIt->second.begin(); clusterIt!=clustersIt->second.end(); clusterIt++)
       {
	 if(clusterIt->second.find("size")->second > 2)
	   return false;
       }
   }
   for(unsigned int t=0; t<fittedTracksCovariance.size(); t++)
   {
       //cout << __PRETTY_FUNCTION__ << "Intercept: " << fittedTracks[t][1]*10 << endl;
       //cout << __PRETTY_FUNCTION__ << "Sigma: " << sqrt(covMat[t](1,1))*10 << endl;
       //if(fittedTracksCovariance[t](1,1) != 0) 
       //  cout << __PRETTY_FUNCTION__ << sqrt(fittedTracksCovariance[t](1,1))*10 << endl;
       if(sqrt(fittedTracksCovariance[t](1,1))*10 > 0.465)
	 return false;
   }
  
  
  return true;                              
}

