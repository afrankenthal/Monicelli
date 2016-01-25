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
 
#include "T958Converter.h"
#include "EventReader.h"
#include "Event.h"
#include "MessageTools.h"

#include <TFile.h>

#include <sstream>

using namespace std;

//------------------------------------------------------------------------------------
T958Converter::T958Converter(EventReader* reader)
{
  theHeader_   = reader->getEventHeaderPointer() ;
  theEvent_    = reader->getEventPointer      () ;
  theGeometry_ = reader->getGeometryPointer   () ;
  
  runNumber_ = reader->getRunNumber();
  
  stringstream fileName;
  fileName << "/data/TestBeam/2014_08_August/T958/Run" << runNumber_ << "_converted.root";

  theFile_ = TFile::Open(fileName.str().c_str(),"RECREATE");
  theTree_ = new TTree ("T958", "The reconstructed telescope tracks");
  theTree_->Branch("event", &theT958Event_, "xSlope/D:ySlope/D:xIntercept/D:yIntercept/D:chi2/D:trigger/I:runNumber/I:timestamp/L:fastCounter/L");

  // Retrieve from file the number of stored events  
  unsigned int numberOfEvents = reader->getNumberOfEvents() ;
  
  std::stringstream ss;
  ss.str(""); 
  ss << "Found " << numberOfEvents << " events on file" ;
  STDLINE(ss.str(),ACYellow) ;
  STDLINE("       ",ACYellow) ;
  for(unsigned int event=0; event<numberOfEvents; ++event)
  {
    if(event%1000 == 0)
    {
      ss.str(""); 
      ss << "Progress " << (int)(((float)event/numberOfEvents)*100) << " %" ;
      STDSNAP(ss.str(),ACRed) ;
    }      
    reader->readEvent(event) ;
    this->analyzeEvent(event) ;   
  }
  std::cout << std::endl;
  STDLINE("Writing Output File", ACRed);
  theFile_->Write("",TObject::kOverwrite);
  theFile_->Close();
  STDLINE("Done!", ACGreen);
}


//------------------------------------------------------------------------------------
T958Converter::~T958Converter ()
{
}

//------------------------------------------------------------------------------------
// This is the method of the T958Converter class where users are supposed 
// to implement their own private code for analysis. Here you will find 
// an exampl of how to retrieve and manipulate components of the event
// and the geometry.
//
// NOTE: for a detailed description of the Event class, its public access
//       methods and the meaning of invidual containers, please consult
//       ../include/Event.h and comments therein.
//
void T958Converter::analyzeEvent(unsigned int event)
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
    theT958Event_.xSlope      = tParameters[0];
    theT958Event_.ySlope      = tParameters[2];
    theT958Event_.xIntercept  = tParameters[1]*10;//In um
    theT958Event_.yIntercept  = tParameters[3]*10;//In um
    theT958Event_.chi2        = chi2[tr];
    theT958Event_.trigger     = theEvent_->getTrigger();
    theT958Event_.runNumber   = runNumber_;
    theT958Event_.timestamp   = theEvent_->getUTC()>> 44;
    theT958Event_.fastCounter = theEvent_->getUTC()&0xfffffffffff;
    theTree_->Fill();
    stringstream ss;
    ss.str("") ;
//    ss.setf(std::ios_base::right,std::ios_base::adjustfield) ;
//    ss << std::setprecision(8) << " ";
    ss << std::setw( 4) << tr  
//        << std::setw(15) << tParameters[0] 
//        << std::setw(15) << tParameters[2] 
//        << std::setw(15) << tParameters[1] 
//        << std::setw(15) << tParameters[3] 
//        << std::setprecision(3)
//        << std::setw( 6) << chi2[tr] 
          << std::setw(10)  << theEvent_->getTrigger()
          << hex << std::setw(20)  << theT958Event_.timestamp 
          << std::setw(20) << theT958Event_.fastCounter << dec; 
    STDLINE(ss.str(),ACGreen) ;
    
    
  }
                                 
}
