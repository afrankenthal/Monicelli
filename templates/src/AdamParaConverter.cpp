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
 
#include "AdamParaConverter.h"
#include "EventReader.h"
#include "MessageTools.h"
#include "Event.h"

#include <sstream>

using namespace std;

//------------------------------------------------------------------------------------
AdamParaConverter::AdamParaConverter(EventReader* reader)
{
  theHeader_   = reader->getEventHeaderPointer() ;
  theEvent_    = reader->getEventPointer      () ;
  theGeometry_ = reader->getGeometryPointer   () ;
  
  runNumber_ = reader->getRunNumber();
  
  stringstream fileName;
  //fileName << "/home/nfs/cmstestbeam/Adam_2014_01_January/Run" << runNumber_ << "_AdamPara_converted.txt";
  fileName << "./Run" << runNumber_ << "_AdamPara_converted.txt";

  theFile_.open(fileName.str().c_str());

  // Retrieve from file the number of stored events  
  unsigned int numberOfEvents = reader->getNumberOfEvents() ;
  
  std::stringstream ss;
  ss.str(""); 
  ss << "Found " << numberOfEvents << " events on file" ;
  STDLINE(ss.str(),ACYellow) ;
  STDLINE("       ",ACYellow) ;
  STDLINE("Reading and Analyzing events...",ACGreen) ;
  for(unsigned int event=0; event<numberOfEvents; ++event)
  //for(unsigned int event=0; event<10; ++event)
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
  STDSNAP("Progress 100 %\n",ACGreen) ;
  
  STDLINE("Writing Output File", ACRed);
  theFile_.close();
  STDLINE("Done!", ACGreen);
}


//------------------------------------------------------------------------------------
AdamParaConverter::~AdamParaConverter ()
{
}

//------------------------------------------------------------------------------------
// This is the method of the AdamParaConverter class where users are supposed 
// to implement their own private code for analysis. Here you will find 
// an exampl of how to retrieve and manipulate components of the event
// and the geometry.
//
// NOTE: for a detailed description of the Event class, its public access
//       methods and the meaning of invidual containers, please consult
//       ../include/Event.h and comments therein.
//
void AdamParaConverter::analyzeEvent(unsigned int event)
{
  Event::fittedTracksDef           & fittedTracks             = theEvent_->getFittedTracks    ()     ;
  Event::trackCandidatesDef        & trackPoints              = theEvent_->getTrackCandidates ()     ;

  if( !(fittedTracks.size() == 1 && trackPoints[0].size() == 8)) return ;

  for(unsigned int tr=0; tr<fittedTracks.size(); ++tr)
  {
    ROOT::Math::SVector<double,4> tParameters = fittedTracks[tr] ;
    theAdamParaEvent_.xIntercept = tParameters[1];
    theAdamParaEvent_.yIntercept = tParameters[3];
    theAdamParaEvent_.xSlope     = tParameters[0];
    theAdamParaEvent_.ySlope     = tParameters[2];
    theFile_ << std::fixed << std::setprecision(10)
             << theAdamParaEvent_.xIntercept << " "
             << theAdamParaEvent_.yIntercept << " "
             << theAdamParaEvent_.xSlope << " "
             << theAdamParaEvent_.ySlope << " ";
    Event::alignedHitsCandidateMapDef::iterator it;
    int hit=0;
    for(it = trackPoints[tr].begin(); it != trackPoints[tr].end(); it++, hit++)
    {
      theAdamParaEvent_.x[hit]      = it->second["x"];
      theAdamParaEvent_.y[hit]      = it->second["y"];
      theAdamParaEvent_.z[hit]      = it->second["z"];
      theAdamParaEvent_.xError[hit] = it->second["xErr"];
      theAdamParaEvent_.yError[hit] = it->second["yErr"];
      theAdamParaEvent_.zError[hit] = it->second["zErr"];
      theFile_ 
               << theAdamParaEvent_.x[hit] << " "
               << theAdamParaEvent_.y[hit] << " "
               << theAdamParaEvent_.z[hit] << " "
               << theAdamParaEvent_.xError[hit] << " "
               << theAdamParaEvent_.yError[hit] << " "
               << theAdamParaEvent_.zError[hit] << " ";
	       
    }
    theFile_ << endl;
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
