/****************************************************************************
** Authors: Lorenzo Uplegger, Luigi Vigani
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include "T1037Converter.h"
#include "EventReader.h"
#include "Event.h"
#include "MessageTools.h"

#include <TFile.h>
#include <sstream>

using namespace std;

//------------------------------------------------------------------------------------
T1037Converter::T1037Converter(EventReader* reader)
{
  theHeader_   = reader->getEventHeaderPointer() ;
  theEvent_    = reader->getEventPointer      () ;
  theGeometry_ = reader->getGeometryPointer   () ;
  
  runNumber_ = reader->getRunNumber();
  
  stringstream fileName;
  fileName << "/data/TestBeam/2014_02_February_T1037/ConvertedOutput//Run" << runNumber_ << "_Merged_T1037_converted.root";

  theFile_ = TFile::Open(fileName.str().c_str(),"RECREATE");
  theTree_ = new TTree ("T1037", "The reconstructed telescope tracks");
  theTree_->Branch("event", &theT1037Event_, "xSlope/D:ySlope/D:xIntercept/D:yIntercept/D:chi2/D:trigger/I:runNumber/I:timestamp/L");

  // Retrieve from file the number of stored events  
  unsigned int numberOfEvents = reader->getNumberOfEvents() ;
  
  std::stringstream ss;
  ss.str(""); 
  ss << "Found " << numberOfEvents << " events on file" ;
  STDLINE(ss.str(),ACYellow) ;
  STDLINE("       ",ACYellow) ;
  for(unsigned int event=0; event<numberOfEvents; ++event)
  {
    reader->readEvent(event) ;
    this->analyzeEvent(event) ;   
  }
  
  STDLINE("Writing Output File", ACRed);
  theFile_->Write("",TObject::kOverwrite);
  theFile_->Close();
  STDLINE("Done!", ACGreen);
}


//------------------------------------------------------------------------------------
T1037Converter::~T1037Converter ()
{
}

//------------------------------------------------------------------------------------
// This is the method of the T1037Converter class where users are supposed 
// to implement their own private code for analysis. Here you will find 
// an exampl of how to retrieve and manipulate components of the event
// and the geometry.
//
// NOTE: for a detailed description of the Event class, its public access
//       methods and the meaning of invidual containers, please consult
//       ../include/Event.h and comments therein.
//
void T1037Converter::analyzeEvent(unsigned int event)
{
  //Event::clustersMapDef            & clusters                 = theEvent_->getClusters        ()     ;
  Event::fittedTracksDef           & fittedTracks             = theEvent_->getFittedTracks    ()     ;
  Event::chi2VectorDef             & chi2                     = theEvent_->getFittedTracksChi2()     ;
  //Event::fittedTracksCovarianceDef & fittedTrackCovariance    = theEvent_->getFittedTracksCovariance();
  //Event::trackCandidatesDef        & trackPoints              = theEvent_->getTrackCandidates ()     ;

  if( fittedTracks.size() == 0 ) return ;

  for(unsigned int tr=0; tr<fittedTracks.size(); ++tr)
  {
    ROOT::Math::SVector<double,4> tParameters = fittedTracks[tr] ;
    theT1037Event_.xSlope     = tParameters[0];
    theT1037Event_.ySlope     = tParameters[2];
    theT1037Event_.xIntercept = tParameters[1];
    theT1037Event_.yIntercept = tParameters[3];
    theT1037Event_.chi2       = chi2[tr];
    theT1037Event_.trigger    = theEvent_->getTrigger();
    theT1037Event_.runNumber  = runNumber_;
    theT1037Event_.timestamp  = theEvent_->getUTC();
    theTree_->Fill();
//    stringstream ss;
//    ss.str("") ;
//    ss.setf(std::ios_base::right,std::ios_base::adjustfield) ;
//    ss << std::setprecision(8) << " ";
//    ss << std::setw( 4) << tr  
//        << std::setw(15) << tParameters[0] 
//        << std::setw(15) << tParameters[2] 
//        << std::setw(15) << tParameters[1] 
//        << std::setw(15) << tParameters[3] 
//        << std::setprecision(3)
//        << std::setw( 6) << chi2[tr] ; 
//    STDLINE(ss.str(),ACGreen) ;
    
    
  }
                                 
}
