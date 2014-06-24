/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef TRACKFINDER_H
#define TRACKFINDER_H

#include <map>
#include <vector>
#include <string>
#include <TObject.h>
#include <TGraphErrors.h>
//#include <TVectorT.h>
//#include <TMatrixTSym.h>

#include "Event.h"
#include "Geometry.h"
#include "subMultiProcess.h"
#include "trackFitter.h"
#include "MessageTools.h"



//class  Event;

class trackFinder : public subMultiProcess<trackFinder,Event,Geometry>
{
public:
    trackFinder(trackFitter *theTrackFitter)      {
                                                    theTrackFitter_ = theTrackFitter;
                                                  }
   ~trackFinder(void)                             {;}

    void                                execute                        (Event       * theEvent,
                                                                        Geometry    * theGeometry );
    void                                findFirstAndLastTrackCandidates(Event       * theEvent,
                                                                        Geometry    * theGeometry );
    void                                findRoadSearchTrackCandidates  (Event       * theEvent,
                                                                        Geometry    * theGeometry );
    void                                fitKalmanTrackCandidates       (Event       * theEvent,
                                                                        Geometry    * theGeometry );
    void                                fitSimpleTrackCandidates       (Event       * theEvent,
                                                                        Geometry    * theGeometry );
    void                                setTrackSearchParameters (double        xTol,
                                                                  double        yTol,
                                                                  double        chi2cut        = -1,
                                                                  double        minPoints      = -1,
                                                                  double        maxPlanePoints = -1);
    void                                findDUTCandidates        (Event       * theEvent   ,
                                                                  Geometry    * theGeometry   );
    void                                findAllFirstAndLast      (Event *theEvent,
                                                                  Geometry *theGeometry       );
    void                                findAllRoadSearch        (Event *theEvent,
                                                                  Geometry *theGeometry       );
    void                                fitAllKalman             (Event *theEvent,
                                                                  Geometry *theGeometry       );
    void                                fitAllSimple             (Event *theEvent,
                                                                  Geometry *theGeometry       );

    std::string                         getLabel                 (void                           );
    std::string                         getName                  (void                           ){return "trackFinder"           ;}
    void                                setHeader                (EventHeader *theHeader         ){theHeader->setTracksFound(true);}

private:
    void                                searchHits   (std::vector<Event::alignedHitsCandidateMapDef>& alignedHitsCandidates,
                                                      Event::alignedHitsCandidateMapDef&              alignedHitsCandidate,
                                                      Event::clustersMapDef&                          alignedClusters,
                                                      std::vector<std::string>&                       orderedNames,
                                                      unsigned int                                    currentDetector, Event *theEvent,
                                                      Geometry*                                       theGeometry,
                                                      bool                                            allPlanes);
    void                                cleanUpTracks(std::vector<Event::alignedHitsCandidateMapDef>& alignedHitsCandidates,
                                                      Event::clustersMapDef&                          alignedClusters,
                                                      Event*                                          theEvent,
                                                      Geometry*                                       theGeometry);

    std::string getPlaneID (int station, int plaquette);


    Event::alignedHitsCandidateMapDef   alignedHitsCandidate_ ;
    Event::clustersMapDef               theClustersMap_        ;
    trackFitter                       * theTrackFitter_        ;

    double xTol_           ;
    double yTol_           ;
    double chi2cut_        ;
    double minPoints_      ;
    double maxPlanePoints_ ;

    std::stringstream                   ss_                   ;
};

#endif // TRACKFINDER_H
