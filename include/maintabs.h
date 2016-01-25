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
 
#ifndef MAINTABS_H
#define MAINTABS_H

#include <math.h>

#include <TLegend.h>
#include <QProgressBar>
#include <QListWidgetItem>
#include <QStatusBar>
#include <QTimer>
#include <QWidget>
// #include <TQtWidget.h> // ToROOT6
#include "qrootcanvas.h"  // ToROOT6
#include "TBenchmark.h"
#include <TCanvas.h>
#include <TH2I.h>
#include <TStyle.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <QButtonGroup>
#include <QFileDialog>
#include <QImage>
#include <QScrollArea>
#include <QString>

#include <qtconcurrentrun.h>
#include <QFutureWatcher>

#include <QVBoxLayout>
#include <boost/filesystem/operations.hpp>

#include "aligner.h"
#include "beamSimulator.h"
#include "trackFinder.h"
//#include "trackFitter.h"
#include "calibrationLoader.h"
#include "clusterizer.h"
//#include "Event.h"
//#include "fileEater.h"
//#include "fitter.h"
//#include "Geometry.h"
#include "mainwindow.h"
//#include "MessageTools.h"
//#include "parser.h"
//#include "process.h"
#include "xmleditor.h"
#include "threader.h"
//#include "trackFinder.h"
#include "GeometryParameters.h"
#include "customTableView.h"
#include "qrootcanvas.h" // ToROOT6

//class trackFitter;
class MainWindow ;
class HNavigator ;
//class TQtWidget  ; // ToROOT6

namespace Ui {
    class mainTabs;
}

class mainTabs : public QWidget
{
    Q_OBJECT

public:
    typedef void (mainTabs::*threadEnd_Function)(HManager::stringVDef );

    explicit       mainTabs                 (MainWindow 	  * mainWindow = 0 );
                  ~mainTabs                 (void				   );

    fileEater    * getFileEater             (void				   ) {return theFileEater_ ;}
    void           collectExistingWidgets   (MainWindow 	  * mainWindow = 0 );
    Ui::mainTabs * getUi                    (void				   ) {return ui;	   ;}

private:
    void        initializeSingletons        (void                                  );
    void        getNxNy                     (int                    divider,
                                             int                  & nx,
                                             int                  & ny             );
    QString     getEnvPath                  (QString                environmentName);
    template<class Class> void launchThread3(process              * theProcess,
                                             Class                * object    ,
                                             void (Class::*fn)()                   );
    void signalNewAction                    (std::string            newAction      );
    void showBeamProfiles                   (void                                  );
    void showBeamProfiles_end               (HManager::stringVDef   histoType      );
    void showChi2Distributions              (void                                  );
    void showChi2Distributions_end          (HManager::stringVDef   histoType      );
    void showTrackErrorsOnDut               (void                                  );
    void showTrackErrorsOnDut_end           (HManager::stringVDef   xyHList        );
    void showSlopeDistributions             (void                                  );
    void showSlopeDistributions_end         (HManager::stringVDef   xyStringVec    );
    void trackFinderFitSlope                (void                                  );
    void trackFinderFitSlope_end            (HManager::stringVDef   xyStringVec    );
    void rawAlignmentFitCompare             (void                                  );
    void rawAlignmentFitCompare_end         (HManager::stringVDef   histoType      );
    void writeAlignment                     (void                                  );
    void writeAlignment_end                 (HManager::stringVDef   histoType      );
    void buildClusterPlots                  (void                                  );
    void buildClusterPlots_end              (HManager::stringVDef   histoType      );
    bool loadGeometry                       (QString                type=""        );
    void showGeometry                       (void                                  );
    void copyGeoFileTo                      (QString                fileName       );

    void showResiduals2                     (void                                  );
    //void showResiduals2_end                 (HManager::stringVDef   histoType      );
    void findAndFitTrack                    (std::string            searchMethod,
                                             std::string            fitMethod      );
    void findTrack                          (std::string            searchMethod   );
    void fitTrack                           (std::string            fitMethod      );


    std::string getPlaneID (int station, int plaquette);

    Ui::mainTabs                * ui                     ;
    MainWindow                  * mainWindow_            ;

    fileEater::fileVDef           inputFiles_            ;
    calibrationLoader           * theCalibrationLoader_  ;
    std::vector<int>              selectedEvents_        ;
    aligner                     * theAligner_            ;
    beamSimulator               * theBeamSimulator_      ;
    clusterizer                 * theClusterizer_        ;
    fileEater                   * theFileEater_          ;
    fitter                      * theFitter_             ;
    Geometry                    * theGeometry_           ;
    HManager                    * theHManager_           ;
    HNavigator                  * theHNavigator_         ;
    trackFinder                 * theTrackFinder_        ;
    trackFitter                 * theTrackFitter_        ;
    threader                    * theThreader_           ;
    long                          SELF_ID                ;
    std::stringstream             ss_                    ;
    QTimer                      * timer2_                ;
    QString                       path_                  ;
    std::string                   plaqSelected_          ;
    QStatusBar                  * statusBar_             ;
    std::map<std::string,TH2I*>   vetH_                  ;
    std::map<int, QRootCanvas*>   theClusterCanvas_      ; // ToROOT6
    HManager::stringVDef          residualsType_         ;
    bool                          redoChi2_              ;
    std::map<std::string,GeometryParameters*> geometryParameters_;
    int                           geometryDisplayShrinkFix_;

    TBenchmark     * theBenchmark_                            ;

    std::string      theThreadProcess_                        ;

    QRootCanvas    * beamSpot2DCanvas_			          ; // ToROOT6
    QRootCanvas    * beamSpotProjXCanvas_ 		          ; // ToROOT6
    QRootCanvas    * beamSpotProjYCanvas_			  ; // ToROOT6
    QRootCanvas    * chargeADCCanvas_				  ; // ToROOT6
    QRootCanvas    * chargeElectronsCanvas_			  ; // ToROOT6
    QRootCanvas    * clustersCanvas_				  ; // ToROOT6
    QRootCanvas    * dutAlignmentPullsCanvasLeft_		  ; // ToROOT6
    QRootCanvas    * dutAlignmentPullsCanvasRight_		  ; // ToROOT6
    QRootCanvas    * dutAlignmentPullsSize1CanvasLeft_  	  ; // ToROOT6
    QRootCanvas    * dutAlignmentPullsSize1CanvasRight_ 	  ; // ToROOT6
    QRootCanvas    * dutAlignmentPullsSize2CanvasLeft_  	  ; // ToROOT6
    QRootCanvas    * dutAlignmentPullsSize2CanvasRight_ 	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResidualsCanvasLeft_		  ; // ToROOT6
    QRootCanvas    * dutAlignmentResidualsCanvasRight_  	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResidualsSize1CanvasLeft_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResidualsSize1CanvasRight_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResidualsSize2CanvasLeft_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResidualsSize2CanvasRight_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsXYCanvasLeft_		  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsXYCanvasRight_  	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsXYSize1CanvasLeft_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsXYSize1CanvasRight_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsXYSize2CanvasLeft_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsXYSize2CanvasRight_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsYXCanvasLeft_	   	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsYXCanvasRight_     	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsYXSize1CanvasLeft_ 	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsYXSize1CanvasRight_	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsYXSize2CanvasLeft_ 	  ; // ToROOT6
    QRootCanvas    * dutAlignmentResXYvsYXSize2CanvasRight_	  ; // ToROOT6
    QRootCanvas    * eventDisplayLeftCanvas_			  ; // ToROOT6
    QRootCanvas    * eventDisplayRightCanvas_			  ; // ToROOT6
    QRootCanvas    * fineAlignmentPullsCanvasLeft_		  ; // ToROOT6
    QRootCanvas    * fineAlignmentPullsCanvasRight_		  ; // ToROOT6
    QRootCanvas    * fineAlignmentPullsSize1CanvasLeft_ 	  ; // ToROOT6
    QRootCanvas    * fineAlignmentPullsSize1CanvasRight_	  ; // ToROOT6
    QRootCanvas    * fineAlignmentPullsSize2CanvasLeft_ 	  ; // ToROOT6
    QRootCanvas    * fineAlignmentPullsSize2CanvasRight_	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResidualsCanvasLeft_  	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResidualsCanvasRight_ 	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResidualsSize1CanvasLeft_	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResidualsSize1CanvasRight_      ; // ToROOT6
    QRootCanvas    * fineAlignmentResidualsSize2CanvasLeft_	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResidualsSize2CanvasRight_      ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsXYCanvasLeft_  	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsXYCanvasRight_ 	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsXYSize1CanvasLeft_	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsXYSize1CanvasRight_      ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsXYSize2CanvasLeft_	  ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsXYSize2CanvasRight_      ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsYXCanvasLeft_            ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsYXCanvasRight_           ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsYXSize1CanvasLeft_       ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsYXSize1CanvasRight_      ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsYXSize2CanvasLeft_       ; // ToROOT6
    QRootCanvas    * fineAlignmentResXYvsYXSize2CanvasRight_      ; // ToROOT6
    QRootCanvas    * loadCalibrationMainCanvas_                   ; // ToROOT6
    QRootCanvas    * mainTabsExpertCanvas_                        ; // ToROOT6
    QRootCanvas    * rawAlignmentLeftCanvas_			  ; // ToROOT6
    QRootCanvas    * rawAlignmentRightCanvas_			  ; // ToROOT6
    QRootCanvas    * rawAlignmentSynpoticLeftCanvas_		  ; // ToROOT6
    QRootCanvas    * rawAlignmentSynpoticRightCanvas_		  ; // ToROOT6
    QRootCanvas    * residuals2DResidualsVsCoordinateLeftCanvas_  ; // ToROOT6
    QRootCanvas    * residuals2DResidualsVsCoordinateRightCanvas_ ; // ToROOT6
    QRootCanvas    * residualsManualFitLeftCanvas_		  ; // ToROOT6
    QRootCanvas    * residualsManualFitRightCanvas_		  ; // ToROOT6
    QRootCanvas    * residualsPullsLeftCanvas_  		  ; // ToROOT6
    QRootCanvas    * residualsPullsRightCanvas_ 		  ; // ToROOT6
    QRootCanvas    * residualsResidualsVsCoordinateLeftCanvas_    ; // ToROOT6
    QRootCanvas    * residualsResidualsVsCoordinateRightCanvas_   ; // ToROOT6
    QRootCanvas    * residualsSynopticViewLeftCanvas_		  ; // ToROOT6
    QRootCanvas    * residualsSynopticViewRightCanvas_  	  ; // ToROOT6
    QRootCanvas    * trackFinderLeftCanvas_			  ; // ToROOT6
    QRootCanvas    * trackFinderRightCanvas_			  ; // ToROOT6

signals:
    void processFinished (threadEnd_Function function   ,
                          HManager::stringVDef histoType   );

private slots:
    void on_trackFindAndFitPB_clicked                             (void                              );
    void on_trackFitPB_clicked                                    (void                              );
    void on_trackFindPB_clicked                                   (void                              );
    void on_testButtonPB_clicked                                  (void                              );
    void on_showTrackErrorsOnDut_clicked                          (void                              );
    void on_selectGeometryPB_clicked                              (void                              );
    void on_selectFilesPB_clicked                                 (void                              );
    void on_reconstructEventsPB_clicked                           (void                              );
    void on_userModeAlignPB_clicked                               (void                              );
    void on_dutAlignmentDutSelectCB_currentIndexChanged           (const QString                     );
    void on_dutWriteFineAlignmentPB_clicked                       (void                              );
    void on_saveXMLResultsPB_clicked                              (void                              );
    void showResiduals                                            (void                              );
    void on_makeFittedTracksDeviationsPB_clicked                  (void                              );
    void on_findDUThitsPB_clicked                                 (void                              );
    void on_residualRangeHigCB_activated                          (QString                           );
    void on_residualRangeLowCB_activated                          (QString                           );
    void on_dutAlignmentPB_clicked                                (void                              );
    void launchThread2                                            (process*  theProcess              );
    void endProcessSettings                                       (process*  currentProcess,
                                                                   bool      success                 );
    void endThreadSettings                                        (threadEnd_Function function    ,
                                                                   HManager::stringVDef histoType    );
    void setAlignmentBoxes                                        (const     QString alignmentMethod );
    void fixStrips                                                (int       state                   );
    void on_abortActionPB_clicked                                 (void                              );
    void on_showSelectedEventsElectronDistribuitionPB_clicked     (void                              );
    void on_showAdcDistributionsPB_clicked                        (void                              );
    void on_loadFakeBeam_clicked                                  (void                              );
    void on_showPixelCalibrationPB_clicked                        (void                              );
    void on_calibrationLoaderPB_clicked                           (void                              );
    void on_TEMPSHOWTRACKSPB_clicked                              (void                              );
    void on_manualRotationsPB_clicked                             (void                              );
    void on_applyZrotationsPB_clicked                             (void                              );
    void on_FitScatterResidualsPB_clicked                         (void                              );
    void on_trackFitterSoloDetectorAlignmentCB_toggled            (bool              checked         );
    void on_alignDutCB_clicked                                    (bool              checked         );
    void on_applySlopeLimitsPB_clicked                            (void                              );
    void on_trackFinderFitSlopePB_clicked                         (void                              );
    void on_trackFinderFitChi2PB_clicked                          (void                              );
    void on_detectorsToBeAlignedLW_itemClicked                    (QListWidgetItem * item            );
    void on_trackFitterExcludeDetectorCB_clicked                  (bool              checked         );
    void on_residualsTabPagePlaqSelectCB_currentIndexChanged      (const QString&    plaqSelected    );
    void on_eventDisplayDUTprojectionPB_clicked                   (void                              );
    void on_trackFinderSlopeAlignPB_clicked                       (void                              );
    void on_showSlopeDistributions_clicked                        (void                              );
    void on_buildPlotsPB_clicked            		          (void                              );
    void on_showChi2Distributions_clicked   		          (void                              );
    void on_unconstrainedResidualsPB_clicked    	    	  (void                              );
    void on_show3dFittedTracksBeamPB_clicked		          (void                              );
    void on_trackFitterExcludedDetectorsListW_itemSelectionChanged(void                              );
    void on_trackFitterWriteAlignmentPB_clicked 	          (void                              );
    void on_trackFitterFitPB_clicked	        	          (void                              );
    void on_findAndSolveClustersPB_clicked      	          (void                              );
    void on_writeAlignmentPB_clicked	        	          (void                              );
    void on_rawAlignmentFitComparePB_clicked                      (void                              );
    void on_rawAlignmentTabPagePlaqSelectCB_currentIndexChanged   (QString                           );
    void on_showBeamProfilesPB_clicked            	          (void                              );
    bool on_loadGeometryPB_clicked                	          (void                              );
    void on_eventDisplayShowBeamSpotsPB_clicked   	          (void                              );
    void findValuesChanged                        	          (int           newValue            );
    void on_fileEaterVerbosityCB_activated        	          (int           index               );
    void on_findEventsPB_clicked                  	          (void                              );
    void on_fineAlignmentPB_clicked                               (void                              );
    void on_trackFindAndFitAlignmentPB_clicked                    (void                              );
    void on_writeFineAlignmentResultsPB_clicked                   (void                              );
    void drawAll                                  	          (QRootCanvas*  where                 , // ToROOT6
                                                                   std::string   what                  ,
                                                                   std::string   detector          = "",
                                                                   std::string   option            = "",
                                                                   double        lowRange          = 0,
                                                                   double        higRange          = 0,
                                                                   int           nXCanvasDivisions = 0,
                                                                   int           nYCanvasDivisions = 0,
                                                                   int           tPad = 1            );
    void on_showAllPlaqPB_clicked                                 (void 	                     );
    void on_eventDisplayTabPagePlaqSelectCB_currentIndexChanged   (QString	                     );
    void on_eventSelectedSpinBox_valueChanged                     (int  	                     );
    void on_eatRootFilePB_clicked                                 (void 	                     );
    void on_showHitsFreqPB_clicked                                (void 	                     );
    void on_browseOutputFilePB_clicked                            (void 	                     );
    void on_outputFileLE_textChanged                              (QString	     outputPath      );
    void on_writeASCIICB_toggled                                  (bool 	     checked         );
    void on_parseFilePB_clicked                                   (void 	                     );
    void advanceProgressBar2                                      (void                              );
    void on_showBeamSpotPB_clicked                                (void 	                     );
    void on_eatFilePB_clicked                                     (void 	                     );
    bool on_changeXmlGeometryPB_clicked                           (void                              );
    void openRootFile                                             (QString           fileName        );
    void setLogAxis                                               (QRootCanvas	   * where           , // ToROOT6
                                                                   std::string       axis            );
    void showAllPlaq                                              (void                              );
    void swapBeamProfilesHistograms                               (bool              toggled         );
    void swapParseButtonText                                      (bool 	         parsing     );
    void swapRawAlignmentFitCBs                                   (bool              checked         );
    void swapResidualsFitCBs                                      (bool              checked         );
    void swapSlopeFitCBs                                          (bool              checked         );
    void selectedCanvasObjectManager                              (TObject	   * obj             ,
                                                                   unsigned int      event           ,
                                                                   TCanvas	   *                 );
    void setCBslopeLimits                                         (TObject         * obj             ,
                                                                   unsigned int      event           ,
                                                                   TCanvas         *                 );
    void enableLimitZ                                             (int               state           );
    void updateFixParMap                                          (bool              fix             );
    void on_copyGeoGeometryPB_clicked                             (void                              );
    void on_copyToGeoGeometryPB_clicked                           (void                              );
    void on_saveCalibToROOTPB_clicked                             (                                  );
    void on_geometrySetPB_clicked                                 (                                  );
    void on_geometryDisableEnableAllPB_clicked                    (                                  );
    void on_geometrySumAngleCorrToBasePB_clicked                  (                                  );
    void on_geometrySumTransCorrToBasePB_clicked                  (                                  );
    void on_geometrySumAllCorrToBasePB_clicked                    (                                  );
    bool on_loadXMLGeometryPB_clicked                             (                                  );
    bool on_loadGeoGeometryPB_clicked                             (                                  );
    void on_geometryClearAllCorrectionsPB_clicked                 (                                  );
    void on_trackFitNameCB_currentIndexChanged                    (const QString   & arg1            );
    void on_clearBulkFilesPB_clicked                              (                                  );
};

#endif // MAINTABS_H
