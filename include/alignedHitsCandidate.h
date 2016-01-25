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
