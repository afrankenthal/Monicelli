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
 
#include "EventAnalyzer.h"
#include "EventReader.h"
#include "MessageTools.h"
#include "Event.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <stdlib.h>

using namespace std;

int hit_=0, total_=0;

//====================================================================
// This class receives a pointer to the eventReader class. This pointer 
// is used to retrieve individual pointers to the objects stored on
// file (theHeader_, theEvent_ and theGeometry_)
//
EventAnalyzer::EventAnalyzer(EventReader * reader)
{
  theHeader_   = reader->getEventHeaderPointer() ;
  theEvent_    = reader->getEventPointer      () ;
  theGeometry_ = reader->getGeometryPointer   () ;
  
  runNumber_ = reader->getRunNumber();
  
  stringstream fileName;
  fileName << "./Run" << runNumber_ << "_Histograms.root";

  theFile_ = TFile::Open(fileName.str().c_str(),"RECREATE");

  hXSlope     = new TH1F("hXSlope","Track Slope X distribution", 100, -0.0006, 0.0006);
  hYSlope     = new TH1F("hYSlope","Track Slope Y distribution", 100, -0.0006, 0.0006);
  hChi2       = new TH1F("hChi2",  "Chi 2"                     , 100,       0,      5);
  for(int i=0; i<10; i++)
  {
    ss_.str(""); ss_ << "Residual" << i;
    hXResiduals.push_back(new TH1F(("hXResiduals"+ ss_.str()).c_str(), "X Residual", 100, -200, 200));
    hYResiduals.push_back(new TH1F(("hYResiduals"+ ss_.str()).c_str(), "Y Residual", 100, -200, 200));
  }
  // Retrieve from file the number of stored events  
  unsigned int numberOfEvents = reader->getNumberOfEvents() ;
  
  ss_.str(""); ss_ << "Found " << numberOfEvents << " events on file" ;
  STDLINE(ss_.str(),ACYellow) ;
  STDLINE("       ",ACYellow) ;

  // Retrieve individual events and analyze them individually
  hit_=0  ;
  total_=0;
//  for(unsigned int event=0; event<numberOfEvents; ++event)
  for(unsigned int event=0; event<1000; ++event)
  {
    if(event%1000 == 0)
    {
      ss_.str(""); 
      ss_ << "Progress " << (int)(((float)event/numberOfEvents)*100) << " %" ;
      STDSNAP(ss_.str(),ACRed) ;
    }      
    reader->readEvent(event) ;
    this->analyzeEvent(event) ;   
  }
  STDSNAP("Progress 100 %\n",ACGreen) ;
  
//  double efficency = (1.*hit_)/(1.*total_) * 100.;
//  std::cout << "efficiency    = " << efficency << std::endl;
//  std::cout << "total tracks = " << total_    << std::endl;
//  std::cout << "hits         = " << hit_      << std::endl;
  
  STDLINE("Writing Output File", ACWhite);
  theFile_->Write("",TObject::kOverwrite);
  theFile_->Close();
  STDLINE("Writing done!", ACGreen);
//  	delete theNtuple_;
//	delete theFile_;
}

//====================================================================
// This is the method of the eventAnalyzer class where users are supposed 
// to implement their own private code for analysis. Here you will find 
// an exampl of how to retrieve and manipulate components of the event
// and the geometry.
//
// NOTE: for a detailed description of the Event class, its public access
//       methods and the meaning of invidual containers, please consult
//       ../include/Event.h and comments therein.
//
void EventAnalyzer::analyzeEvent(unsigned int event)
{
  Event::clustersMapDef            & clusters                 = theEvent_->getClusters              ();
  Event::fittedTracksDef           & fittedTracks             = theEvent_->getFittedTracks          ();
  Event::chi2VectorDef             & chi2                     = theEvent_->getFittedTracksChi2      ();
  //Event::fittedTracksCovarianceDef & fittedTrackCovariance    = theEvent_->getFittedTracksCovariance();
  //Event::trackCandidatesDef        & trackPoints              = theEvent_->getTrackCandidates       ();
  Event::residualsMapDef           & fittedTrackResiduals     = theEvent_->getFittedTrackResiduals  ();
  

  if( fittedTracks.size() == 0 ) return ;

  //STDLINE("",ACWhite) ;
  ss_.str(""); ss_ << "Event "           << event ;               //STDLINE(ss_.str(),ACPurple) ;
  ss_.str(""); ss_ << "Clusters found: " << clusters.size() 
                   << "  Tracks: "       << fittedTracks.size()  //STDLINE(ss_.str(),ACWhite)  ;
                   << "  Residuals: "    << fittedTrackResiduals.size(); //STDLINE(ss_.str(),ACWhite)  ;
  //STDLINE("Track           Slope                           Intercept          Chi2",ACLightBlue) ;
  //STDLINE("          X                Y              X                Y           ",ACRed      ) ;

//             X             Y            X             Y           
//    0   -0.00015444948 -299.69584     0.00023105554  -116.2111      1.1585256
//   0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-

  for(unsigned int tr=0; tr<fittedTracks.size(); ++tr)
  {
//    std::cout << theEvent_->getTrigger() << std::endl;
    ROOT::Math::SVector<double,4> tParameters = fittedTracks[tr] ;
    ss_.str("") ;
    ss_.setf(std::ios_base::right,std::ios_base::adjustfield) ;
    ss_ << std::setprecision(8) << " ";
    ss_ << std::setw( 4) << tr  
        << std::setw(15) << tParameters[0] 
        << std::setw(15) << tParameters[2] 
        << std::setw(15) << tParameters[1] 
        << std::setw(15) << tParameters[3] 
        << std::setprecision(3)
        << std::setw( 6) << chi2[tr] ; 
//    STDLINE(ss_.str(),ACGreen) ;
    
   hXSlope->Fill(tParameters[0]);
   hYSlope->Fill(tParameters[2]);
   hChi2  ->Fill(chi2[tr]);
   int i=0;
   for(Event::trackResidualsMapDef::iterator it=fittedTrackResiduals[tr].begin(); it!=fittedTrackResiduals[tr].end(); it++, i++)
   {
     //Detector::xyPair predSigmas = theGeometry->getDetector(it->first)->propagateTrackErrors(fittedTracks[tr],AtVAInv,fRInv[exl->first],fTz[exl->first]);
     hXResiduals[i]->Fill(it->second.first*10);
     hYResiduals[i]->Fill(it->second.second*10);
     //hXPulls[i]->Fill(it->second.first*10);
     //hYPulls[i]->Fill(it->second.second*10/);
   }
   //hResiduals;
   //hPulls;
   //hXResidualVsXCoordinate;
   //hXResidualVsYCoordinate;
   //hChi2;
    
    //selection for tracks with chi2/DOF
//    if( chi2[tr] > 5 ) continue;

    // Detector    *dut         = theGeometry_->getDetector(4,0)  ;
    // std::string detectorName = theGeometry_->getDetectorID(4,0);
    // double xp, yp, zp, tmp, xm ,ym;
    
    //get the track impact point on dut in the local detector frame
    // dut->getPredictedLocal( tParameters, xp, yp );                                                                                         
    
    
    //check if the impact point is on the dut surface
    // if( xp < 0 || xp > dut->getDetectorLengthX(true) || 
    //     yp < 0 || yp > dut->getDetectorLengthY(true)   ) continue;

    //increse the total tracks number
    // total_++;

    //check if there's a hit for this track on dut
    // if( trackPoints[tr].find(detectorName) == trackPoints[tr].end() ) continue;
    // int clusterNumber = (int)trackPoints[tr][detectorName]["num"];
    
    //get the relative cluster coordinate (that's in the local coordinate frame)
    // xm = clusters[detectorName][ clusterNumber ]["x"];
    // ym = clusters[detectorName][ clusterNumber ]["y"];
    
    //or compute the local coordinate from the global ones
    //xm = trackPoints[tr][detectorName]["x"] 
    //ym = trackPoints[tr][detectorName]["y"]
    //dut->fromGlobalToDetector(&xm   ,
    //                          &ym   ,
    //                          &trackPoints[tr][detectorName]["xErr"],
    //                          &trackPoints[tr][detectorName]["yErr"],
    //                          &trackPoints[tr][detectorName]["zErr"]  );
    
    //compute the x and y distances between mesured and predicted point (unostrained residuals)
    // double distX  = ( xm - xp );
    // double distY  = ( ym - yp );
    
    
    //get the track error and sum to the cluster error

    // Detector::xyPair predPair = dut->getTrackErrorsOnPlane( fittedTracks[tr], fittedTrackCovariance[tr] );
    // double xWindow = predPair.first  + pow( clusters[detectorName][ clusterNumber ]["xErr"] , 2 );
    // double yWindow = predPair.second + pow( clusters[detectorName][ clusterNumber ]["yErr"] , 2 );
    // //if the point is within n sigma got a hit
    // double nSigma = 2;
    // if( fabs(distX) > nSigma*sqrt(xWindow) ) continue;
    // if( fabs(distY) > nSigma*sqrt(yWindow) ) continue;
    
    // hit_++;
  }
                                 
}

EventAnalyzer::~EventAnalyzer ()
{
//	theFile_->Write("",TObject::kOverwrite);
//	theFile_->Close();

	delete theFile_;
}
