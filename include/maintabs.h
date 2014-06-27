/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef MAINTABS_H
#define MAINTABS_H

#include <math.h>

#include <TLegend.h>
#include <QProgressBar>
#include <QListWidgetItem>
#include <QStatusBar>
#include <QTimer>
#include <QWidget>
#include <TQtWidget.h>

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
#include "trackFitter.h"
#include "calibrationLoader.h"
#include "clusterizer.h"
#include "Event.h"
#include "fileEater.h"
#include "fitter.h"
#include "Geometry.h"
#include "mainwindow.h"
#include "MessageTools.h"
//#include "parser.h"
#include "process.h"
#include "xmleditor.h"
#include "threader.h"
#include "trackFinder.h"
#include "GeometryParameters.h"

class MainWindow ;
class HNavigator ;
class TQtWidget  ;

namespace Ui {
    class mainTabs;
}

class mainTabs : public QWidget
{
    Q_OBJECT

public:
    typedef void (mainTabs::*threadEnd_Function)(HManager::stringVDef );

    explicit       mainTabs                    (MainWindow           * mainWindow = 0 );
                  ~mainTabs                    (void                                  );

    fileEater    * getFileEater                (void                                  ) {return theFileEater_ ;}
    void           collectExistingWidgets      (MainWindow           * mainWindow = 0 );
    Ui::mainTabs * getUi                       (void                                  ) {return ui;           ;}

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
    std::map<int, TQtWidget*>     theClusterCanvas_      ;
    HManager::stringVDef          residualsType_         ;
    bool                          redoChi2_              ;
    std::map<std::string,GeometryParameters*> geometryParameters_;

signals:
    void processFinished (threadEnd_Function function   ,
                          HManager::stringVDef histoType   );

private slots:
//    void on_findTrackRoadSearchPB_clicked                         (void                              );
//    void on_findTrackFirstAndLastPB_clicked                       (void                              );
//    void on_findKalmanFitPB_clicked                               (void                              );
    void on_trackFitPB_clicked                                    (                                  );
    void on_trackSearchPB_clicked();
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
    void on_detectorsToBeAlignedLW_itemClicked                    (QListWidgetItem * item            );
    void on_trackFitterExcludeDetectorCB_clicked                  (bool              checked         );
    void on_residualsTabPagePlaqSelectCB_currentIndexChanged      (const QString&    plaqSelected    );
    void on_eventDisplayDUTprojectionPB_clicked                   (void                              );
    void on_trackFinderSlopeAlignPB_clicked                       (void                              );
    void on_showSlopeDistributions_clicked                        (void                              );
    void on_buildPlotsPB_clicked            		              (void                              );
    void on_showChi2Distributions_clicked   		              (void                              );
    void on_unconstrainedResidualsPB_clicked    	    		                  (void                              );
    void on_show3dFittedTracksBeamPB_clicked		              (void                              );
    void on_trackFitterExcludedDetectorsListW_itemSelectionChanged(void                              );
    void on_trackFitterWriteAlignmentPB_clicked 	              (void                              );
    void on_trackFitterFitPB_clicked	        	              (void                              );
    void on_findAndSolveClustersPB_clicked      	              (void                              );
    void on_writeAlignmentPB_clicked	        	              (void                              );
    void on_rawAlignmentFitComparePB_clicked                      (void                              );
    void on_rawAlignmentTabPagePlaqSelectCB_currentIndexChanged   (QString                           );
    void on_showBeamProfilesPB_clicked            	              (void                              );
    bool on_loadGeometryPB_clicked                	              (void                              );
    void on_eventDisplayShowBeamSpotsPB_clicked   	              (void                              );
    void findValuesChanged                        	              (int  	     newValue            );
    void on_fileEaterVerbosityCB_activated        	              (int  	     index               );
    void on_findEventsPB_clicked                  	              (void                              );
    void on_fineAlignmentPB_clicked                               (void                              );
    void on_writeFineAlignmentResultsPB_clicked                   (void                              );
    void drawAll                                  	              (TQtWidget*  where           ,
                                                                   std::string what            ,
                                                                   std::string detector = ""   ,
                                                                   std::string option = "",
                                                                   double      lowRange = 0,
                                                                   double      higRange = 0,
                                                                   int         nXCanvasDivisions = 0,
                                                                   int         nYCanvasDivisions = 0,
                                                                   int         tPad = 1              );
    void on_showAllPlaqPB_clicked                                 (void 	                         );
    void on_eventDisplayTabPagePlaqSelectCB_currentIndexChanged   (QString	                         );
    void on_eventSelectedSpinBox_valueChanged                     (int  	                         );
    void on_eatRootFilePB_clicked                                 (void 	                         );
    void on_showHitsFreqPB_clicked                                (void 	                         );
    void on_browseOutputFilePB_clicked                            (void 	                         );
    void on_outputFileLE_textChanged                              (QString	     outputPath          );
    void on_writeASCIICB_toggled                                  (bool 	     checked             );
    void on_parseFilePB_clicked                                   (void 	                         );
    void advanceProgressBar2                                      (void                              );
    void on_showBeamSpotPB_clicked                                (void 	                         );
    void on_eatFilePB_clicked                                     (void 	                         );
    bool on_changeXmlGeometryPB_clicked                           (void                              );
    void openRootFile                                             (QString           fileName        );
    void setLogAxis                                               (TQtWidget	   * where           ,
                                                                   std::string       axis            );
    void showAllPlaq                                              (void                              );
    void swapBeamProfilesHistograms                               (bool              toggled         );
    void swapParseButtonText                                      (bool 	         parsing         );
    void swapRawAlignmentFitCBs                                   (bool              checked         );
    void swapResidualsFitCBs                                      (bool              checked         );
    void swapSlopeFitCBs                                          (bool              checked         );
    void selectedCanvasObjectManager                              (TObject	   * obj             ,
                                                                   unsigned int      event           ,
                                                                   TCanvas	   *                     );
    void setCBslopeLimits                                         (TObject         * obj             ,
                                                                   unsigned int      event           ,
                                                                   TCanvas         *                 );
    void comboBoxPlaqSelectedConnection                           (int               tabIndex        );
    void enableLimitZ                                             (int               state           );
    void updateFixParMap                                          (bool              fix             );
    void on_copyGeoGeometryPB_clicked                             (void                              );
    void on_copyToGeoGeometryPB_clicked                           (void                              );
    void on_saveCalibToROOTPB_clicked();
    void on_geometrySetPB_clicked();
    void on_geometryDisableEnableAllPB_clicked();
    void on_geometrySumAngleCorrToBasePB_clicked();
    void on_geometrySumTransCorrToBasePB_clicked();
    void on_geometrySumAllCorrToBasePB_clicked();
    bool on_loadXMLGeometryPB_clicked();
    bool on_loadGeoGeometryPB_clicked();
    void on_geometryClearAllCorrectionsPB_clicked();
};

#endif // MAINTABS_H
