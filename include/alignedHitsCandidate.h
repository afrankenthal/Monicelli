/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef ALIGNEDHITSCANDIDATE_H
#define ALIGNEDHITSCANDIDATE_H

#include <map>
#include <string>
#include <TObject.h>

#include "Event.h"
#include "HManager.h"
#include "Geometry.h"
#include "MessageTools.h"


//class  Event;

class alignedHitsCandidate : public TObject
{
public:
    alignedHitsCandidate(HManager *theHManager)             ;
    alignedHitsCandidate(Event *theEvent)                   ;
   ~alignedHitsCandidate(void)                             {;}

    //               x,y,z,sz,sy,sz|value
    typedef std::map<std::string,   double>               clusterCoordinateDef       ;
    //               planeId       |cluster coordinate
    typedef std::map<std::string,   clusterCoordinateDef> alignedHitsCandidateMapDef ;
    typedef std::vector<alignedHitsCandidateMapDef>       trackCandidatesDef         ;

    typedef std::map<std::string, float>                  aClusterDef                ;
    typedef std::map<int, aClusterDef>                    aClusterMapDef             ;
    typedef std::map<std::string, aClusterMapDef>         clustersMapDef             ;

    int                    size(void) ;

    int                          findTrackCandidates(Event *theEvent, Geometry *theGeometry,
                                                     std::string *histoTypeX, std::string *histoTypeY);
    alignedHitsCandidateMapDef   getAlignedCombination(std::string exludedPlaneId);

private:
    alignedHitsCandidateMapDef  alignedHitsCandidate_ ;
    trackCandidatesDef          trackCandidates_      ;
    clustersMapDef              theClustersMap_       ;

    HManager                   *theHManager_          ;

    std::stringstream             ss_                 ;

    //ClassDef(alignedHitsCandidate,1)
};

#endif // ALIGNEDHITSCANDIDATE_H
