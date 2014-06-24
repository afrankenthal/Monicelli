/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "Event.h"
#include <sstream>
//#include "HManager.h"

//#define MINIMUM_TRACK_POINTS 6
#define WINDOW               0.005

ClassImp(Event)

//================================================================================
Event::Event (void)
{
  //STDLINE("Empty constructor",ACWhite) ;
    //theAlignedHitsCandidate_ = new alignedHitsCandidate(this);
}

//================================================================================
Event::~Event(void)
{
//    STDLINE("Event::~Event",ACRed);
}

//================================================================================
void Event::setRawData (int trig, const plaqMapDef& theRawData)
{
  theRawData_ = theRawData;
  trig_       = trig      ;
}
//================================================================================
void Event::clear (void )
{
    trig_=0                                    ;
    theRawData_.clear()                        ;
    clusters_.clear()                          ;
    clustersHits_.clear()                      ;
    trackCandidates_.clear()                   ;
    fittedTracksChi2_.clear()                  ;
    fittedTrackResiduals_.clear()               ;
    fittedTracks_.clear()                      ;
    fittedTracksCovariance_.clear()            ;
}
//================================================================
Event::residualsMapDef  Event::makeFittedTrackDeviations  (int trackNumber)
{
    std::stringstream ss_;
    fittedTrackDeviations_.clear();
    if ( trackCandidates_.size() != fittedTracks_.size() )
    {
      ss_.str("");
      ss_ << "track candidates size: " << trackCandidates_.size()
          << " fitted tracks   size: " << fittedTracks_.size()    ;
      //STDLINE(ss_.str(),ACYellow)
    }

    if( trackNumber < 0 )
    {
      for( unsigned int i=0; i < trackCandidates_.size(); i++ )
      {
          this->makeSingleTrackDeviations(i);
      }
    }
    else
    {
        this->makeSingleTrackDeviations(trackNumber);
    }

    return  fittedTrackDeviations_;
}

//================================================================
void Event::makeSingleTrackDeviations( int trackNum )
{
     for(Event::alignedHitsCandidateMapDef::iterator pit = trackCandidates_[trackNum].begin(); pit!= trackCandidates_[trackNum].end()  ; pit++)
     {
        double resX = ( (*pit).second["x"] - fittedTracks_[trackNum][0]*(*pit).second["z"] - fittedTracks_[trackNum][1] ) ;
        double resY = ( (*pit).second["y"] - fittedTracks_[trackNum][2]*(*pit).second["z"] - fittedTracks_[trackNum][3] ) ;
        fittedTrackDeviations_[trackNum][(*pit).first] = std::make_pair(resX, resY);
     }
}

//================================================================
void Event::setClustersHits(const clustersHitsMapDef& clustersHits)
{
  clustersHits_  = clustersHits ;
}

//================================================================
void Event::setClusters(const clustersMapDef& clusters)
{
  clusters_  = clusters;
}

//================================================================
void Event::addUnconstrainedFittedTrack(unsigned int trackN, std::string detector, const vectorDef& fittedTrack, const matrixDef& covarianceMatrix, double fittedTrackChi2)
{
    while(unconstrainedFittedTracks_.size() <= trackN)
    {
        unconstrainedFittedTracks_          .push_back(std::map<std::string,vectorDef>());
        unconstrainedFittedTracksCovariance_.push_back(std::map<std::string,matrixDef>());
        unconstrainedFittedTracksChi2_      .push_back(std::map<std::string,double>());
    }
    unconstrainedFittedTracks_          [trackN][detector] = fittedTrack;
    unconstrainedFittedTracksCovariance_[trackN][detector] = covarianceMatrix;
    unconstrainedFittedTracksChi2_      [trackN][detector] = fittedTrackChi2;
}
