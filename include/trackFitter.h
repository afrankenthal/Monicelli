/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef TRACKFITTER_H
#define TRACKFITTER_H

#include <vector>
#include <set>
#include <Math/SMatrix.h>
#include <Math/SVector.h>
#include <TMath.h>
#include <iostream>

using namespace std;

#include "Event.h"
#include "Geometry.h"
//#include "maintabs.h"

#include "subMultiProcess.h"

class trackFitter : public subMultiProcess<trackFitter,Event,Geometry>
{
public:
    trackFitter(void)   ;
   ~trackFitter(void)  {this->clear();}
   
   typedef ROOT::Math::SVector<double,4>                             SV4Def ;
   typedef std::pair< std::pair< SV4Def, Event::matrixDef >, double> aFittedTrackDef;

   void                             clear                           (void                                                            );
   void                             clearSelectedDetectorsList      (void                                                            ){selectedDetectors_.clear()          ;}
   Event::fittedTracksDef           fitTracks                       (const Event::trackCandidatesDef         & tracks,
                                                                    Geometry                                 * theGeometry,
                                                                    std::string                                excludedDetector = "" );
   aFittedTrackDef                  fitSingleTrack                  (const Event::alignedHitsCandidateMapDef & alignedHits,
                                                                    Geometry                                 * theGeometry,
                                                                    std::string                                excludedDetector = "" );
   aFittedTrackDef                  kalmanFitSingleTrack            (const Event::alignedHitsCandidateMapDef & trackCandidate,
                                                                    Event::vectorDef                         & track,
                                                                    Event::matrixDef                         & cov,
                                                                    Event::clustersMapDef                    & clusters,
                                                                    Geometry                                 * theGeometry,
                                                                    std::string                                excludedDetector = "" );
   void                             makeDetectorTrackResiduals      (ROOT::Math::SVector<double,4>           & fittedTrack,
                                                                    Event::matrixDef                         & covMat,
                                                                    const Event::clustersMapDef              & clusters,
                                                                    const Event::trackCandidatesDef          & tracks,
                                                                    Geometry                                 * theGeometry,
                                                                    std::string                                detector,
                                                                    int                                        trackNum              );
   void                             makeFittedTracksResiduals       (Event                                   * theEvent,
                                                                    Geometry                                 * theGeometry           );
   void                             makeFittedTrackDeviations       (Event* theEvent, Geometry* theGeometry                          );

   void                             execute                         (Event* theEvent, Geometry* theGeometry                          );
   Event::fittedTracksDef        &  getTracks                       (void                                                            ){return fittedTracks_                ;}
   //Event::residualsMapDef          getResiduals                     (void                                                            ){return residualsMap_                ;}
   //Event::residualsMapDef          getPulls                         (void                                                            ){return pullMap_                     ;}
   Event::fittedTracksCovarianceDef getCovariance                   (void                                                            ){return covMat_                      ;}
   Event::chi2VectorDef             getChi2                         (void                                                            ){return chi2_                        ;}
   std::string                      getLabel                        (void                                                            );
   std::string                      getName                         (void                                                            ){return "trackFitter"                ;}


   void                             setSelectedDetectorsList        (std::set<std::string>                     selectedDetectors     ){selectedDetectors_=selectedDetectors;}
   void                             setTracksFitted                 (bool                                      tracksFitted          ){tracksFitted_ = tracksFitted        ;}
   bool                             tracksFitted                    (void                                                            ){return tracksFitted_                ;}

   void                             setFitMethodName                (std::string fitMethodName                                       ){fitMethodName_=fitMethodName        ;}
   //std::string                      getFitMethodName                (void                                                            ){return fitMethodName_               ;}

private:

    void printMatrix(std::string sm, Event::matrixDef & matrix) ;
    void printMatrix(std::string sm, ROOT::Math::SMatrix<long double,4,4> & matrix) ;

    Event::residualsMapDef           residualsMap_         ;
    Event::residualsMapDef           pullMap_              ;
    Event::fittedTracksDef           fittedTracks_         ;
    Event::fittedTracksCovarianceDef covMat_               ;
    Event::chi2VectorDef             chi2_                 ;
    std::set<std::string>            selectedDetectors_    ;
    bool                             excludedDetectorFound_;
    bool                             tracksFitted_         ;
    int count;
    std::stringstream                ss_                   ;

    bool                             debug_                ;
    Event::clustersMapDef            clusters_             ;

    std::string                      fitMethodName_        ;

};

#endif // TRACKFITTER_H
