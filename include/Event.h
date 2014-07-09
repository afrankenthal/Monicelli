/************************************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
**
**  This is the header of the event class. An event is basically a collection of the following 
**  quantities:
** 
**  1] raw data hits (identified by row/column on a plaquette in the telescope)
**  2] the corresponding cluster coordinates (obtained by collapsing clusters of adjacent raw
**     data hits into a single x/y pair of coordinates by appliying a charge sharing algorithm) 
**  3] fitted tracks (obtained by fitting strings of aligned cluster-hits to a straight line in
**     space: these strings are identified by a road-search algorithm with a user defined 
*      tolerance in the x/y projections).
**  4] unconstrained fit residuals, chi2 and covariances of the reconstructed tracks
**
**  Below is a description of the basic data structures which are made persistent to disk by 
**  compiling the C++ code derived by this header with rootcint.
**
**  ----------------------------------------------------------------------------------------------
**  The raw data produced by each plaquette
** 
**  map<string, vector<map<string , int> > >                              theRawData_ 
**         |      |           |      |
**         |      |           |      +--------------> value
**         |      |           +---------------------> row | col | adc | dataType
**         |      +---------------------------------> vector of hits
**         +----------------------------------------> plaquette ID
**        
**  ----------------------------------------------------------------------------------------------
**  The cluster hit coordinates/errors computed for each plaquette by using the charge sharing
**  wheighting. The x/y coordinates are computed in the local plaquette reference frame.
**
**  map<string, map<int, map<string, double> > >                          clusters_  
**         |         |         |       |
**         |         |         |       +------------> value
**         |         |         +--------------------> x | y | xErr | yErr | charge | dataType | size
**         |         +------------------------------> cluster ID number
**         +----------------------------------------> plaquette ID
**  
**  ----------------------------------------------------------------------------------------------
**  The clusters found for each plaquette: each cluster (an assembly of contiguous hits).
**  To get the corresponding coordinates (after charge sharing has been applyed), use the 
**  clusters_ container above using the "cluster ID number" to match.
**
**  map<string, map<int, vector<map<string, int > > > >                   clustersHits_
**         |         |     |           |     |
**         |         |     |           |     +------> value
**         |         |     |           +------------> row | col | adc | dataType
**         |         |     +------------------------> vector of row/column hits
**         |         +------------------------------> cluster ID number
**         +----------------------------------------> plaquette ID
**         
**  ----------------------------------------------------------------------------------------------
**  A vector (parallel to trackCandidates_) containing the chi2 of the fit
**  
**  vector<double>                                                        fittedTracksChi2_
**            |
**            +-------------------------------------> value 
**            
**  ----------------------------------------------------------------------------------------------
**  The vector of fitted tracks: for each plaquette the coordinates/errors of the cluster hits 
**  used for the final fit are provided in the laboratory reference frame. Additionally it is 
**  possible to gain access to the cluster used to perform the fit by means of the "cluster ID"
**  
**  vector<map<string, map<string, double> > >                            trackCandidates_
**    |          |           |       |
**    |          |           |       +--------------> value
**    |          |           +----------------------> x | y | z | xErr | yErr | errZ | xyErr | size | cluster ID | dataType | charge
**    |          +----------------------------------> plaquette  ID
**    +---------------------------------------------> vector of track candidates
**    
**  ----------------------------------------------------------------------------------------------
**  The unconstrained residuals of the fitted tracks on the two projections. This vector is 
**  strictly parallel to fittedTrackDeviations_.
**  
**  map<unsigned int, map<string, pair<double, double> > >                fittedTrackResiduals_
**         |                 |           |       |
**         |                 |           |       +--> xRes
**         |                 |           +----------> yRes
**         |                 +----------------------> plaquette ID
**         +----------------------------------------> a progressive number (an ID)
**         
**  ----------------------------------------------------------------------------------------------
**  The constrained residuals of the fitted tracks on the two projections.
**  
**   map<unsigned int, map<string, pair<double, double> > >                fittedTrackDeviations_
**         |                 |           |       |
**         |                 |           |       |
**         |                 |           |       +--> xRes
**         |                 |           +----------> yRes
**         |                 +----------------------> plaquette ID
**         +----------------------------------------> a progressive number (an ID)
**    
**  ----------------------------------------------------------------------------------------------
**  The vector of the slope/intercept parameters of the fitted tracks. This vector is strictly
**  parallel to fittedTrackResiduals_.
**  
**  vector<SVector<double,4> >                                            fittedTracks_
**     |             |
**     |             +------------------------------> slope/intercept on x and y projections
**     +--------------------------------------------> vector of track parameters         
**
**  ----------------------------------------------------------------------------------------------
**  The vector of the covariance matrix of the fitted tracks. This vector is strictly
**  parallel to fittedTrackResiduals_.
**  
**  vector< SMatrix<double,4,4> >                                         fittedTracksCovariance_  
**     |              |
**     |              +-----------------------------> covariance matrix of track fit
**     +--------------------------------------------> vector of tracks covariance matrix
**     
**  ----------------------------------------------------------------------------------------------
**  The vector of the slope/intercept parameters of the unconstrained fitted tracks. This vector is strictly
**  parallel to fittedTracks_. The point excluded from the fit is on the plaquette ID
**
**  vector< map<string, SVector<double,4> > >                             unconstrainedFittedTracks_
**     |         |      |
**     |         |      +---------------------------> slope/intercept on x and y projections
**     |         +----------------------------------> plaquette ID
**     +--------------------------------------------> vector of track parameters
**
**  ----------------------------------------------------------------------------------------------
**  The vector of the covariance matrix of the unconstrained fitted tracks. This vector is strictly
**  parallel to unconstrainedFittedTrackResiduals_.
**
**  vector< map<string, < SMatrix<double,4,4> > >                         fittedTracksCovariance_
**     |              |
**     |              +-----------------------------> covariance matrix of track fit
**     +--------------------------------------------> vector of tracks covariance matrix
**
**=====================================================================================*/

#ifndef __EVENT__
#define __EVENT__


#include <map>
#include <string>
#include <TH2I.h>
#include <vector>
#include <TMatrix.h>
#include <TObject.h>

#include <Math/SMatrix.h>
#include <Math/SVector.h>

#include "MessageTools.h"

class Event : public TObject
{
public:
    typedef ROOT::Math::SMatrix<double,4,4>                matrixDef                   ;
    typedef ROOT::Math::SVector<double,4>                  vectorDef                   ;
    typedef std::map<std::string, int>                     aHitDef                     ;
    typedef std::vector<aHitDef>                           hitsDef                     ;
    typedef std::map<std::string, hitsDef>                 plaqMapDef                  ;
    typedef std::map<std::string, double>                  aClusterDef                 ;
    typedef std::map<int, aClusterDef>                     aClusterMapDef              ;
    typedef std::map<int, hitsDef>                         aClusterHitsMapDef          ;
    typedef std::map<std::string, aClusterMapDef>          clustersMapDef              ;
    typedef std::map<std::string, aClusterHitsMapDef>      clustersHitsMapDef          ;
    typedef std::map<std::string, double>                  clusterCoordinateDef        ;
    typedef std::map<std::string, clusterCoordinateDef>    alignedHitsCandidateMapDef  ;
    typedef std::vector<alignedHitsCandidateMapDef>        trackCandidatesDef          ;
    typedef std::pair<double, double>                      xyResiduePairDef            ;
    typedef std::map<std::string, xyResiduePairDef>        trackResidualsMapDef        ;
    typedef std::map<unsigned int, trackResidualsMapDef>   residualsMapDef             ;
    typedef std::vector<vectorDef>                         fittedTracksDef             ;
    typedef std::vector<matrixDef>                         fittedTracksCovarianceDef   ;
    typedef std::vector<double>                            chi2VectorDef               ;
    typedef std::vector<std::map<std::string, vectorDef> > unconstrainedFittedTracksDef;
    typedef std::vector<std::map<std::string, matrixDef> > unconstrainedFittedTracksCovarianceDef;
    typedef std::vector<std::map<std::string, double> >    unconstrainedChi2VectorDef  ;

    Event(void) ;
    ~Event(void);

    void                       clear                     (void                                     );

    unconstrainedFittedTracksDef          & getUnconstrainedFittedTracks          (void            ){return unconstrainedFittedTracks_          ;}
    unconstrainedFittedTracksCovarianceDef& getUnconstrainedFittedTracksCovariance(void            ){return unconstrainedFittedTracksCovariance_;}
    unconstrainedChi2VectorDef            & getUnconstrainedFittedTracksChi2      (void            ){return unconstrainedFittedTracksChi2_      ;}
    clustersMapDef           & getClusters               (void                                     ){return clusters_                           ;}
    clustersHitsMapDef       & getClustersHits           (void                                     ){return clustersHits_                       ;}
    plaqMapDef               & getRawData                (void                                     ){return theRawData_                         ;}
    trackCandidatesDef       & getTrackCandidates        (void                                     ){return trackCandidates_                    ;}

    trackCandidatesDef       & getAlignedHitsCandidates (void                                      ){return alignedHitsCandidates_              ;}
    clustersMapDef           & getAlignedClusters       (void                                      ){return alignedClusters_                    ;}

    residualsMapDef          & getFittedTrackDeviations  (void                                     ){return fittedTrackDeviations_              ;}
    residualsMapDef          & getFittedTrackResiduals   (void                                     ){return fittedTrackResiduals_               ;}
    residualsMapDef          & getFittedTrackPulls       (void                                     ){return fittedTrackPulls_                   ;}
    fittedTracksDef          & getFittedTracks           (void                                     ){return fittedTracks_                       ;}
    fittedTracksCovarianceDef& getFittedTracksCovariance (void                                     ){return fittedTracksCovariance_             ;}
    chi2VectorDef            & getFittedTracksChi2       (void                                     ){return fittedTracksChi2_                   ;}
    int                        getTrigger                (void                                     ){return trig_                               ;}
    long long                  getUTC                    (void                                     ){return utc_                                ;}
    unsigned int               getTimestamp              (void                                     ){return timestamp_                          ;}
    bool                       getBubbleSignal           (void                                     ){return bubbleSignal_                       ;}
    bool                       getBubbleAltSignal        (void                                     ){return bubbleAltSignal_                    ;}

    void                       setRawData                (int                             trig,
                                                          const plaqMapDef&               theRawData      );
    void                       setClustersHits           (const clustersHitsMapDef&       clustersHits    );
    void                       setClusters               (const clustersMapDef&           clusters        );

    void                       addUnconstrainedFittedTrack(unsigned int     trackN,
                                                           std::string      detector,
                                                           const vectorDef& fittedTrack,
                                                           const matrixDef& covarianceMatrix,
                                                           double           fittedTrackChi2);

    residualsMapDef            makeFittedTrackDeviations (int                             trackNumber = -1);

    //    void                       setTrackCandidates        (const trackCandidatesDef&       trackCandidates ){trackCandidates_        = trackCandidates  ;}
    void                       setFittedTrackResiduals   (const residualsMapDef&          residuals       ){fittedTrackResiduals_   = residuals        ;}
    void                       setFittedTrackDeviations  (const residualsMapDef&          residuals       ){fittedTrackDeviations_  = residuals        ;}
    void                       setFittedTrackPulls       (const residualsMapDef&          pulls           ){fittedTrackPulls_       = pulls            ;}
    void                       setTrigger                (int                             trigger         ){trig_                   = trigger          ;}
    void                       setUTC                    (long long                       utc             ){utc_                    = utc              ;}
    void                       setTimestamp              (unsigned int                    timestamp       ){timestamp_              = timestamp        ;}
    void                       setBubbleSignal           (bool                            bubbleSignal    ){bubbleSignal_           = bubbleSignal     ;}
    void                       setBubbleAltSignal        (bool                            bubbleSignal    ){bubbleAltSignal_        = bubbleSignal     ;}


    //plaqMapDef & getTelescopeHits(void)            ;
    //hitsDef    & getPlaquetteHits(std::string)     ;

private:

    void makeSingleTrackDeviations ( int trackNum );

    int                          trig_                     ;
    plaqMapDef                   theRawData_               ;
    clustersMapDef               clusters_                 ;
    clustersHitsMapDef           clustersHits_             ;
    trackCandidatesDef           trackCandidates_          ;

    trackCandidatesDef           alignedHitsCandidates_    ;
    clustersMapDef               alignedClusters_          ;

    residualsMapDef              fittedTrackResiduals_     ;
    residualsMapDef              fittedTrackDeviations_    ;
    residualsMapDef              fittedTrackPulls_         ;
    fittedTracksDef              fittedTracks_             ;
    fittedTracksCovarianceDef    fittedTracksCovariance_   ;
    chi2VectorDef                fittedTracksChi2_         ;
    unconstrainedFittedTracksDef           unconstrainedFittedTracks_          ;
    unconstrainedFittedTracksCovarianceDef unconstrainedFittedTracksCovariance_;
    unconstrainedChi2VectorDef             unconstrainedFittedTracksChi2_      ;
    long long                    utc_                      ;
    unsigned int                 timestamp_                ;
    bool                         bubbleSignal_             ;
    bool                         bubbleAltSignal_          ;
    //std::stringstream             ss_                       ; //! temporary state value

    ClassDef(Event,6);

} ;

#endif
