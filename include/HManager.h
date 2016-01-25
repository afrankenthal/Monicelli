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
 
#ifndef HMANAGER_H
#define HMANAGER_H

#include <map>
#include <string>
#include <vector>
#include <set>

#include <Math/SMatrix.h>
#include <Math/SVector.h>
#include <TF1.h>
#include <TFolder.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TROOT.h>
#include <vector>

#include "Event.h"
#include "fileEater.h"
#include "Geometry.h"
#include "histogramPaths.h"
#include "MessageTools.h"
#include "multiProcess.h"


class fileEater ;

class HManager : public multiProcess< HManager,std::vector<std::string> >
{

public:

    typedef std::pair  <std::string    , std::string        > xyStringPairDef ;
    typedef std::vector<std::string                         > stringVDef      ;

    typedef stringVDef (HManager::*HManager_Make_function)(Event*,bool&);

                           HManager            	     (fileEater*    theFileEater) ;
                           ~HManager            	 (void		                ) ;

    bool                   execute                   (void                      ) ;
    std::string            getLabel                  (void                      ) {return "Histogramming";}
    int                    getMaxIterations          (void                      ) ;
    std::string            getName                   (void                      ) {return "HManager"     ;}


    stringVDef             clearAlignmentResults     (std::string detector = "" ) ;
    void                   clearAllFilters           (void                      ) ;
    stringVDef             clearDeviations	         (void		                ) ;
    stringVDef             clearPulls	    	     (void		                ) ;
    stringVDef             clearResiduals	         (void		                ) ;
    stringVDef             clearScatterResidual	     (void		                ) ;

    stringVDef             eventsCycle               (void                      ) ;
    //stringVDef             eventsFilter              (void                      ) ;

    stringVDef             fillAlignmentResults      (std::string detector        ,
                                                      int         clusterSizeX    , int         clusterSizeY            ,
                                                      double      resX            , double sigx2                        ,
                                                      double      resY            , double sigy2                        ,
                                                      double      xPrime          , double yPrime                       ,
                                                      bool        clear = false                                       ) ;
    stringVDef             fillDeviations	         (Event*      theEvent        , int    trackNum    = -1             ,
                                                      int         clusterSize = -1, bool   clear       = false        ) ;
    stringVDef             fillPulls	    	     (Event*      theEvent        , int    trackNum    = -1             ,
                                                      int         clusterSize = -1, bool   clear       = false        ) ;
    stringVDef             fillResiduals	         (Event*      theEvent        , int    trackNum    = -1             ,
                                                      int         clusterSize = -1, bool   clear       = false        ) ;
    stringVDef             fillScatterResidual 	     (Event*      theEvent,
                                                      int         trackNum    = -1, int    clusterSize = -1            ,
                                                      bool        clear       = false  		                          ) ;

    TFile*                 getFileHandle	         (std::string fullPath 		                                      ) ;
    TObject*               getHistogram	    	     (std::string histogramType, int         station    , int plaq    ) ;
    TObject*               getHistogram	    	     (std::string histogramType, std::string detectorID		          ) ;
    TObject*               getHistogram        	     (std::string histogramPath		                                  ) ;
    stringVDef             getOutputPaths            (void                                                            ) {return histoPaths_        ;}
    HManager_Make_function getSubProcessFunction     (void                                                            ) {return subProcessFunction_;}
    std::string            getResidualsToPlot        (void                                                            ) {return residualsToPlot_;}
    int                    getTotalTracksFound       (void                                                            ) {return totalTracksFound_;}
    int                    getTotalEventsWithTracksFound(void                                                         ) {return totalEventsWithTracksFound_;}

    void                   markClusters	             (Event* theEvent	    	                                      ) ;
    void                   markClustersHits    	     (Event* theEvent 		                                          ) ;
    std::string            makeRawEvent	    	     (Event* theEvent 			                                      ) ;
    stringVDef             makeTrackEvent            (Event* theEvent                                                 ) ;
    std::string            makeClusterEvent    	     (Event* theEvent 		                                          ) ;
    stringVDef             makeResidualDistributions (Event* theEvent, bool &redo                                     ) ;
    stringVDef             makeAdcDistributions2     (Event* theEvent, bool &redo                                     ) ;
    stringVDef             makeBeamSpots2	         (Event* theEvent, bool &redo                                     ) ;
    stringVDef             makeHitsFreq2	         (Event* theEvent, bool &redo                                     ) ;
    stringVDef             makeClusterPlots2         (Event* theEvent, bool &redo                                     ) ;
    stringVDef             makeTracksDistr2          (Event* theEvent, bool &redo                                     ) ;
    stringVDef             makeTrackErrorsOnDUTs2    (Event* theEvent, bool &redo                                     ) ;
    stringVDef             makeDUTprojections2       (Event* theEvent, bool &redo, std::vector<std::string> DUTs      ) ;
    stringVDef             makeMeanScatterResidual   (stringVDef       histoTypes    , bool redo        = true        ) ;
    stringVDef             makeElectronDistribution  (std::vector<int> selectedEvents, int  clusterSize = -1          ) ;
    stringVDef             makeFittedTracksBeam	     (std::vector<int> selectedEvents                                 ) ;

    void                   setEventFilters           (int           maxTracks                                           ,
                                                      int           minPlanesWithHits                       = -1        ,
                                                      int           maxPointsPerDetector                    = -1      ) ;

    void                   setResidualFilters        (std::string   type                                                ,
                                                      double        maxChi2                                 = -1        ,
                                                      int           maxTracks                               = -1        ,
                                                      int           maxPlanePoints                          = -1        ,
                                                      int           minPoints                               = -1        ,
                                                      int           maxClusterSize                          = -1        ,
                                                      int           onlyClusterSize                         = -1      ) ;
    void                   resetResidualCounters     (void                                                            );
    void                   addSelectedDetector       (std::string   detectorName                                      ) {selectedDetectors_.insert(detectorName);}

    void                   setOperation              (multiProcess_function  operation                                  ,
                                                      HManager_Make_function function                       = NULL    ) ;
    void                   setRunSubDir	    	     (std::string            runSubDirName		             ) ;
    void                   setSubProcessFunction     (HManager_Make_function function, bool        redo    = false    ) {subProcessFunction_=function; redo_=redo;}

    stringVDef             storeCustomTObject  	     (TObject*               object  , std::string objectType         ) ;

private:

    std::string	           addItem	                 (std::string            fullPath, TObject*    object             ) ;
    static stringVDef      splitPath                 (std::string            s		    	                     ) ;

    TH2I*                  emptyTH2I_                ;
    TH1I*                  emptyTH1I_                ;
    std::stringstream      ss_                       ;
    TFolder*               currentFolder_            ;
    TFolder*               mainCacheFolder_          ;
    TFolder*               runSubFolder_             ;
    fileEater*             theFileEater_             ;
    Geometry*              theGeometry_              ;
    stringVDef             histoPaths_               ;
    bool                   redo_                     ;
    HManager_Make_function subProcessFunction_       ;

    std::string            residualsToPlot_          ;
    //Filters
    std::set<std::string>  selectedDetectors_        ;
    int                    maxTracksFilter_          ;
    int                    minPlanesFilter_          ;
    double                 chi2filter_               ;
    int                    minTrackHitsFilter_       ;
    int                    maxClustersFilter_        ;
    int                    maxClusterSizeFilter_     ;
    int                    onlyClusterSizeFilter_    ;
    int                    maxPlanePointsFilter_     ;

    //bool                   slopeCuts_                ;
    //double                 xLowerSlopeLimit_         ;
    //double                 xHigherSlopeLimit_        ;
    //double                 yLowerSlopeLimit_         ;
    //double                 yHigherSlopeLimit_        ;

    //Counters
    int                    totalTracksFound_          ;
    int                    totalEventsWithTracksFound_;

    std::vector<int>       selectedEvents_           ;
};

#endif // HMANAGER_H
