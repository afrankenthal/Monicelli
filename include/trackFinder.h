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
 
#ifndef TRACKFINDER_H
#define TRACKFINDER_H

#include <map>
#include <vector>
#include <string>
#include <TObject.h>
#include <TGraphErrors.h>
//#include <TVectorT.h>
//#include <TMatrixTSym.h>

//#include "Event.h"
//#include "Geometry.h"
#include "trackFitter.h"
//#include "subMultiProcess.h"
#include "MessageTools.h"



//class  Event;

class trackFinder : public subMultiProcess<trackFinder,Event,Geometry>
{
public:
                trackFinder                    (trackFitter *theTrackFitter      ){theTrackFitter_ = theTrackFitter;}
               ~trackFinder                    (void                             ){;}

    void        execute                        (Event       * theEvent           ,
                                                Geometry    * theGeometry        );
    void        findAndFitTracks               (Event       * theEvent           ,
                                                Geometry    * theGeometry        );
    void        findFirstAndLastTrackCandidates(Event       * theEvent           ,
                                                Geometry    * theGeometry        );
    void        findRoadSearchTrackCandidates  (Event       * theEvent           ,
                                                Geometry    * theGeometry        );
    void        fitKalmanTrackCandidates       (Event       * theEvent           ,
                                                Geometry    * theGeometry        );
    void        fitSimpleTrackCandidates       (Event       * theEvent           ,
                                                Geometry    * theGeometry        );
    void        setTrackingOperationParameters (std::string   searchMethod       ,
                                                std::string   fitMethod          ,
                                                bool          findDUT            );
    void        setTrackSearchParameters       (double        xTol               ,
                                                double        yTol               ,
                                                double        chi2cut        = -1,
                                                double        minPoints      = -1,
                                                double        maxPlanePoints = -1);
    void        findDUTCandidates              (Event       * theEvent          ,
                                                Geometry    * theGeometry        );
    void        findAllFirstAndLast            (Event       * theEvent          ,
                                                Geometry    * theGeometry        );
    void        findAllRoadSearch              (Event       * theEvent          ,
                                                Geometry    * theGeometry        );
    void        fitAllKalman                   (Event       * theEvent          ,
                                                Geometry    * theGeometry        );
    void        fitAllSimple                   (Event       * theEvent          ,
                                                Geometry    * theGeometry        );

    std::string getFindMethod                  (void                             ){return findMethod_             ;}
    std::string getFitMethod                   (void                             ){return fitMethod_              ;}
    std::string getLabel                       (void                             );
    std::string getName                        (void                             ){return "trackFinder"           ;}
    void        setHeader                      (EventHeader * theHeader          ){theHeader->setTracksFound(true);}
    int         getNumberOfTracks              (void                             ){return numberOfTracks_         ;}

private:
    typedef std::vector<Event::alignedHitsCandidateMapDef> vecAHCDef ;
    
    void        searchHits                     (vecAHCDef                         & alignedHitsCandidates,
                                                Event::alignedHitsCandidateMapDef & alignedHitsCandidate ,
                                                Event::clustersMapDef             & alignedClusters      ,
                                                std::vector<std::string>          & orderedNames         ,
                                                unsigned int                        currentDetector      ,
                                                Event                             * theEvent             ,
                                                Geometry                          * theGeometry          ,
                                                bool                                allPlanes             );
    void        cleanUpTracks                  (vecAHCDef                         & alignedHitsCandidates,
                                                Event::clustersMapDef             & alignedClusters      ,
                                                Event                             * theEvent             ,
                                                Geometry                          * theGeometry           );

    std::string getPlaneID                     (int                                 station              , 
                                                int                                 plaquette             );


    Event::alignedHitsCandidateMapDef   alignedHitsCandidate_ ;
    Event::clustersMapDef               theClustersMap_        ;
    trackFitter                       * theTrackFitter_        ;

    double      xTol_             ;
    double      yTol_             ;
    double      chi2cut_          ;
    double      minPoints_        ;
    double      maxPlanePoints_   ;
    std::string findMethod_       ;
    std::string fitMethod_        ;
    bool        findDUT_          ;
    bool        clusterCorrected_ ;
    int         numberOfTracks_   ;

    std::stringstream ss_;
};

#endif // TRACKFINDER_H
