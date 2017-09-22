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
 
#ifndef TRACKFITTER_H
#define TRACKFITTER_H

#include <vector>
#include <set>
#include <Math/SMatrix.h>
#include <Math/SVector.h>
#include <TMath.h>
#include <TVectorT.h>
#include <TMatrixTSym.h>
#include <TBenchmark.h>
#include <iostream>

using namespace std;

#include "Event.h"
#include "Geometry.h"
#include "KalmanPlaneInfo.h"

#include "subMultiProcess.h"

class trackFitter : public subMultiProcess<trackFitter,Event,Geometry>
{
public:
    trackFitter(void)   ;
   ~trackFitter(void)  {this->clear();}

   struct residualsStruct
   {
     string plaqID    ;
     int    trackN    ;
     double resX      ; 
     double resY      ; 
     double pullX     ; 
     double pullY     ;
     double chi2      ; 
     string direction ;
   } ;
   
   typedef ROOT::Math::SVector<double,4>                     SV4Def                  ;
   typedef std::pair<pair<SV4Def, Event::matrixDef>,double>  aFittedTrackDef         ;
   typedef std::pair<double, std::string>                    pairDef                 ;
   typedef std::vector<pairDef>::const_reverse_iterator      revIterDef              ;
   typedef std::map<std::string, std::pair<double, double> > resDef                  ;
   typedef std::vector<resDef>                               kalmanResidualsVectorDef;
   typedef vector<residualsStruct>                           residualsVDef           ;
    
   void                                   clear                           (void                                                            );
   void                                   clearSelectedDetectorsList      (void                                                            ){selectedDetectors_.clear()          ;}
   void                                   clearKalmanResiduals            (void                                                            ){residualsV_.clear()                 ;}
   Event::fittedTracksDef                 fitTracks                       (const Event::trackCandidatesDef         & tracks               ,
                                                                           Geometry                                * theGeometry          ,
                                                                           std::string                               excludedDetector = "" );
   aFittedTrackDef                        fitSingleTrack                  (const Event::alignedHitsCandidateMapDef & alignedHits          ,
                                                                           Geometry                                * theGeometry          ,
                                                                           std::string                               excludedDetector = "" );
   aFittedTrackDef                        kalmanFitSingleTrack            (const Event::alignedHitsCandidateMapDef & trackCandidate       ,
                                                                           Event::vectorDef                        & track                ,
                                                                           Event::matrixDef                        & cov                  ,
                                                                           Event::clustersMapDef                   & clusters             ,
                                                                           Geometry                                * theGeometry           );
   void                                   makeDetectorTrackResiduals      (ROOT::Math::SVector<double,4>           & fittedTrack          ,
                                                                           Event::matrixDef                        & covMat               ,
                                                                           const Event::clustersMapDef             & clusters             ,
                                                                           const Event::trackCandidatesDef         & tracks               ,
                                                                           Geometry                                * theGeometry          ,
                                                                           std::string                               detector             ,
                                                                           int                                       trackNum              );
   void                                   makeFittedTracksResiduals       (Event                                   * theEvent             ,
                                                                           Geometry                                * theGeometry           );
   void                                   makeFittedTrackDeviations       (Event                                   * theEvent             , 
                                                                           Geometry                                * theGeometry           );
   void                                   execute                         (Event                                   * theEvent             , 
                                                                           Geometry                                * theGeometry           ); 
   Event::fittedTracksDef               & getTracks                       (void                                                            ){return fittedTracks_                 ;}
   Event::fittedTracksCovarianceDef       getCovariance                   (void                                                            ){return covMat_                       ;}
   Event::chi2VectorDef                   getChi2                         (void                                                            ){return chi2_                         ;}
   std::string                            getLabel                        (void                                                            );
   std::string                            getName                         (void                                                            ){return "trackFitter"                 ;}
   residualsVDef                        & getKalmanResidualsMap           (void                                                            ){return residualsV_                   ;}
   void                                   includeResiduals                (bool                                      include               ){includeResiduals_ = include          ;}
   void                                   setSelectedDetectorsList        (std::set<std::string>                     selectedDetectors     ){selectedDetectors_= selectedDetectors;}
   void                                   setTracksFitted                 (bool                                      tracksFitted          ){tracksFitted_     = tracksFitted     ;}
   bool                                   tracksFitted                    (void                                                            ){return tracksFitted_                 ;}

   void                                   setFitMethodName                (std::string fitMethodName                                       ){fitMethodName_      = fitMethodName  ;}
   void                                   setNumberOfIterations           (int                                       iterations            ){nIterations_        = iterations     ;}
   void                                   setKalmanPlaneInfo              (KalmanPlaneInfo                           kalmanInfo            ){theKalmanPlaneInfo_ = kalmanInfo     ;}
   static ROOT::Math::SVector<double,4>   calculateParCorrections         (ROOT::Math::SVector<double,4>             pars                 ,
                                                                           Geometry                                * geo                  ,
                                                                           resDef                                    res                   );

private:

    residualsStruct                  residuals_                  ;
    residualsVDef                    residualsV_                 ;
    
    void printMatrix(std::string sm, Event::matrixDef & matrix)  ;
    void printMatrix(std::string sm, ROOT::Math::SMatrix<long double,4,4> & matrix) ;

    Event::residualsMapDef           residualsMap_               ;
    Event::residualsMapDef           pullMap_                    ;
    Event::fittedTracksDef           fittedTracks_               ;
    Event::fittedTracksCovarianceDef covMat_                     ;
    Event::chi2VectorDef             chi2_                       ;
    std::set<std::string>            selectedDetectors_          ;
    bool                             excludedDetectorFound_      ;
    bool                             tracksFitted_               ;
    int count;
    std::stringstream                ss_                         ;

    bool                             debug_                      ;
    Event::clustersMapDef            clusters_                   ;

    std::string                      fitMethodName_              ;
    int                              nIterations_                ;
    KalmanPlaneInfo                  theKalmanPlaneInfo_         ;
    bool                             includeResiduals_           ;
};

#endif // TRACKFITTER_H
