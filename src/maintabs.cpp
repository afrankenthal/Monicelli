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
 
#include "maintabs.h"
#include "ui_maintabs.h"
#include "qmessagebox.h"
#include <QTableWidget>
#include "TBenchmark.h"
#include "Geometry.h"

#include <iterator>

#define ZOOMFACTOR  2

//===========================================================================
mainTabs::mainTabs(MainWindow * mainWindow) :
    QWidget(mainWindow),
    ui(new Ui::mainTabs)
{

    ui->setupUi(this); // Always call this as the FIRST statement in the ctor!!

    gStyle->SetPalette(1) ;

    mainWindow_          = NULL;
    theAligner_          = NULL;
    theFileEater_        = NULL;
    theGeometry_         = NULL;
    theBeamSimulator_    = NULL;
    theClusterizer_      = NULL;
    theTrackFinder_      = NULL;
    theCalibrationLoader_= NULL;
    theTrackFitter_      = NULL;
    theThreader_         = NULL;
    theFitter_           = NULL;
    theHManager_         = NULL;
    theHNavigator_       = NULL;
    theBenchmark_        = NULL ;
    //theParser_           = NULL;
    //timer_               = NULL;
    timer2_              = NULL;
    redoChi2_            = true;
    geometryDisplayShrinkFix_= 0;
    /*
    theClusterCanvas_[0] = ui->beamSpotProjXCanvas ;
    theClusterCanvas_[1] = ui->beamSpotProjYCanvas ;
    theClusterCanvas_[2] = ui->beamSpot2DCanvas    ;
    theClusterCanvas_[3] = ui->clustersCanvas      ;
*/
    // Initialize owned classes
    this->collectExistingWidgets(mainWindow) ;
    this->initializeSingletons()             ;

    this->ui->tabWidget         ->setCurrentIndex(1);
    this->ui->expertModeTabs    ->setCurrentIndex(0) ;
    this->ui->residualsMonitorTW->setCurrentIndex(0) ;

    //REGISTERING WITH QREGISTERMETATYPE:
    //Register the process* type with QMetaType, so that
    //it can be successfully passed through signals/slots
    qRegisterMetaType<process*>          ("process*"              );
    qRegisterMetaType<threadEnd_Function>("threadEnd_Function"    );
    qRegisterMetaType<HManager::stringVDef>("HManager::stringVDef");

    //widgets connections
    //    connect(this                                 , SIGNAL(makeHistos                     (threadEnd_Function, HManager::stringVDef) ),
    connect(this                                 , SIGNAL(processFinished                (threadEnd_Function, HManager::stringVDef) ),
            this                                 , SLOT  (endThreadSettings              (threadEnd_Function, HManager::stringVDef) ) );
    connect(theThreader_                         , SIGNAL(processFinished                (process *,          bool                ) ),
            this                                 , SLOT  (endProcessSettings             (process *,          bool                ) ) );
    connect( ui->findEventsMaxHitsToSearchSB     , SIGNAL( valueChanged                  (int                                     ) ),
             this                                , SLOT  ( findValuesChanged             (int                                     ) ) );
    connect( ui->findEventsMinHitsToSearchSB     , SIGNAL( valueChanged                  (int                                     ) ),
             this                                , SLOT  ( findValuesChanged             (int                                     ) ) );
    connect( ui->rawAlignmentFitAllCB            , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( swapRawAlignmentFitCBs        (bool                                    ) ) );
    connect( ui->rawAlignmentMyFitParXCB         , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( swapRawAlignmentFitCBs        (bool                                    ) ) );
    connect( ui->rawAlignmentMyFitParYCB         , SIGNAL( clicked                       (bool                                    )   ),
             this                                , SLOT  ( swapRawAlignmentFitCBs        (bool                                    ) ) );
    connect( ui->trackFinderAutoFitCB            , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( swapSlopeFitCBs               (bool                                    ) ) );
    connect( ui->trackFinderMyFitParXCB          , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( swapSlopeFitCBs               (bool                                    ) ) );
    connect( ui->trackFinderMyFitParYCB          , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( swapSlopeFitCBs               (bool                                    ) ) );
    connect( ui->residualsFitAllCB               , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( swapResidualsFitCBs           (bool                                    ) ) );
    connect( ui->trackFitterMyFitParXCB          , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( swapResidualsFitCBs           (bool                                    ) ) );
    connect( ui->trackFitterMyFitParYCB          , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( swapResidualsFitCBs           (bool                                    ) ) );
    connect( ui->parseFilePB                     , SIGNAL( toggled                       (bool                                    ) ),
             this                                , SLOT  ( swapParseButtonText           (bool                                    ) ) );
    connect(timer2_                              , SIGNAL( timeout                       (                                        ) ),
            this                                 , SLOT  ( advanceProgressBar2           (                                        ) ) );
    connect( ui->rawAlignmentClusterProfilesRB   , SIGNAL( toggled                       (bool                                    ) )  ,
             this                                , SLOT  ( swapBeamProfilesHistograms    (bool                                    ) ) );
    connect( ui->rawAlignmentPixelProfilesRB     , SIGNAL( toggled                       (bool                                    ) )  ,
             this                                , SLOT  ( swapBeamProfilesHistograms    (bool                                    ) ) );
    connect( ui->limitZCB                        , SIGNAL( stateChanged                  (int                                     ) ),
             this                                , SLOT  ( enableLimitZ                  (int                                     ) ) );
    connect( ui->dutFineAlignmentAlphaFixCB      , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( updateFixParMap               (bool                                    ) ) );
    connect( ui->dutFineAlignmentBetaFixCB       , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( updateFixParMap               (bool                                    ) ) );
    connect( ui->dutFineAlignmentGammaFixCB      , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( updateFixParMap               (bool                                    ) ) );
    connect( ui->dutFineAlignmentXFixCB          , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( updateFixParMap               (bool                                    ) ) );
    connect( ui->dutFineAlignmentYFixCB          , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( updateFixParMap               (bool                                    ) ) );
    connect( ui->dutFineAlignmentZFixCB          , SIGNAL( clicked                       (bool                                    ) ),
             this                                , SLOT  ( updateFixParMap               (bool                                    ) ) );
    connect( ui->showScatterResidualsPB          , SIGNAL( clicked                       (                                        ) ),
             this                                , SLOT  ( showResiduals                 (                                        ) ) );
    connect( ui->showResidualsPB                 , SIGNAL( clicked                       (                                        ) ),
             this                                , SLOT  ( showResiduals                 (                                        ) ) );
    connect( ui->showDeviationsPB                , SIGNAL( clicked                       (                                        ) ),
             this                                , SLOT  ( showResiduals                 (                                        ) ) );
    connect( ui->showPullsPB                     , SIGNAL( clicked                       (                                        ) ),
             this                                , SLOT  ( showResiduals                 (                                        ) ) );
    connect(ui->alignmentTypeCB                  , SIGNAL(currentIndexChanged            (const QString                           ) ),
             this                                , SLOT  (setAlignmentBoxes              (const QString                           ) ) );
    connect(ui->fixAllCB                         , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAll                         (int                                     ) ) );
    connect(ui->fixExtremesCB                    , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixExtremes                    (int                                     ) ) );
    connect(ui->fixAllXCB                        , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAllX                        (int                                     ) ) );
    connect(ui->fixAllYCB                        , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAllY                        (int                                     ) ) );
    connect(ui->fixAllZCB                        , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAllZ                        (int                                     ) ) );
    connect(ui->fixAllTransCB                    , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAllTrans                    (int                                     ) ) );
    connect(ui->fixAllAnglesCB                   , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAllAngles                   (int                                     ) ) );
    connect(ui->fixAllAlphaCB                    , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAllAlpha                    (int                                     ) ) );
    connect(ui->fixAllBetaCB                     , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAllBeta                     (int                                     ) ) );
    connect(ui->fixAllGammaCB                    , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (fixAllGamma                    (int                                     ) ) );
    connect(ui->enableAllCB                      , SIGNAL(stateChanged                   (int                                     ) ),
            ui->detectorsTableView               , SLOT  (enableAll                      (int                                     ) ) );
    connect(ui->fixStripsCB                      , SIGNAL(stateChanged                   (int                                     ) ),
             this                                , SLOT  (fixStrips                      (int                                     ) ) );
//     connect( ui->trackFinderLeftCanvas           , SIGNAL(RootEventProcessed             (TObject *, unsigned int, TCanvas *      ) ),
//              this                                , SLOT  (setCBslopeLimits               (TObject *, unsigned int, TCanvas *      ) ) );
//     connect( ui->trackFinderRightCanvas          , SIGNAL(RootEventProcessed             (TObject *, unsigned int, TCanvas *      ) ),
//              this                                , SLOT  (setCBslopeLimits               (TObject *, unsigned int, TCanvas *      ) ) );
    connect( ui->saveXMLResultsPB2               , SIGNAL(clicked                        (void                                    ) ),
             this                                , SLOT  (on_saveXMLResultsPB_clicked    (void                                    ) ) );
    connect( ui->saveXMLResultsPB3               , SIGNAL(clicked                        (void                                    ) ),
             this                                , SLOT  (on_saveXMLResultsPB_clicked    (void                                    ) ) );
    connect( ui->xRoadToleranceSB                , SIGNAL(valueChanged                   (int                                     ) ),
             ui->xRoadToleranceSB2               , SLOT  (setValue                       (int                                     ) ) );
    connect( ui->xRoadToleranceSB2               , SIGNAL(valueChanged                   (int                                     ) ),
             ui->xRoadToleranceSB                , SLOT  (setValue                       (int                                     ) ) );
    connect( ui->yRoadToleranceSB                , SIGNAL(valueChanged                   (int                                     ) ),
             ui->yRoadToleranceSB2               , SLOT  (setValue                       (int                                     ) ) );
    connect( ui->yRoadToleranceSB2               , SIGNAL(valueChanged                   (int                                     ) ),
             ui->yRoadToleranceSB                , SLOT  (setValue                       (int                                     ) ) );

    beamSpot2DCanvas_			                 = new QRootCanvas(ui->beamSpot2DCanvas                      	  ,"") ; // ToROOT6
    beamSpotProjXCanvas_ 		                 = new QRootCanvas(ui->beamSpotProjXCanvas		     	          ,"") ; // ToROOT6
    beamSpotProjYCanvas_ 		                 = new QRootCanvas(ui->beamSpotProjYCanvas		     	          ,"") ; // ToROOT6
    chargeADCCanvas_			                 = new QRootCanvas(ui->chargeADCCanvas			     	          ,"") ; // ToROOT6
    chargeElectronsCanvas_		                 = new QRootCanvas(ui->chargeElectronsCanvas		     	      ,"") ; // ToROOT6
    clustersCanvas_			                     = new QRootCanvas(ui->clustersCanvas			     	          ,"") ; // ToROOT6
    dutAlignmentPullsCanvasLeft_ 	             = new QRootCanvas(ui->dutAlignmentPullsCanvasLeft		          ,"") ; // ToROOT6
    dutAlignmentPullsCanvasRight_	             = new QRootCanvas(ui->dutAlignmentPullsCanvasRight		          ,"") ; // ToROOT6
    dutAlignmentPullsSize1CanvasLeft_	         = new QRootCanvas(ui->dutAlignmentPullsSize1CanvasLeft 	      ,"") ; // ToROOT6
    dutAlignmentPullsSize1CanvasRight_	         = new QRootCanvas(ui->dutAlignmentPullsSize1CanvasRight	      ,"") ; // ToROOT6
    dutAlignmentPullsSize2CanvasLeft_	         = new QRootCanvas(ui->dutAlignmentPullsSize2CanvasLeft 	      ,"") ; // ToROOT6
    dutAlignmentPullsSize2CanvasRight_	         = new QRootCanvas(ui->dutAlignmentPullsSize2CanvasRight	      ,"") ; // ToROOT6
    dutAlignmentResidualsCanvasLeft_	         = new QRootCanvas(ui->dutAlignmentResidualsCanvasLeft  	      ,"") ; // ToROOT6
    dutAlignmentResidualsCanvasRight_	         = new QRootCanvas(ui->dutAlignmentResidualsCanvasRight 	      ,"") ; // ToROOT6
    dutAlignmentResidualsSize1CanvasLeft_        = new QRootCanvas(ui->dutAlignmentResidualsSize1CanvasLeft	      ,"") ; // ToROOT6
    dutAlignmentResidualsSize1CanvasRight_       = new QRootCanvas(ui->dutAlignmentResidualsSize1CanvasRight	  ,"") ; // ToROOT6
    dutAlignmentResidualsSize2CanvasLeft_        = new QRootCanvas(ui->dutAlignmentResidualsSize2CanvasLeft	      ,"") ; // ToROOT6
    dutAlignmentResidualsSize2CanvasRight_       = new QRootCanvas(ui->dutAlignmentResidualsSize2CanvasRight	  ,"") ; // ToROOT6
    dutAlignmentResXYvsXYCanvasLeft_	         = new QRootCanvas(ui->dutAlignmentResXYvsXYCanvasLeft  	      ,"") ; // ToROOT6
    dutAlignmentResXYvsXYCanvasRight_	         = new QRootCanvas(ui->dutAlignmentResXYvsXYCanvasRight 	      ,"") ; // ToROOT6
    dutAlignmentResXYvsXYSize1CanvasLeft_        = new QRootCanvas(ui->dutAlignmentResXYvsXYSize1CanvasLeft	      ,"") ; // ToROOT6
    dutAlignmentResXYvsXYSize1CanvasRight_       = new QRootCanvas(ui->dutAlignmentResXYvsXYSize1CanvasRight	  ,"") ; // ToROOT6
    dutAlignmentResXYvsXYSize2CanvasLeft_        = new QRootCanvas(ui->dutAlignmentResXYvsXYSize2CanvasLeft	      ,"") ; // ToROOT6
    dutAlignmentResXYvsXYSize2CanvasRight_       = new QRootCanvas(ui->dutAlignmentResXYvsXYSize2CanvasRight	  ,"") ; // ToROOT6
    eventDisplayLeftCanvas_		                 = new QRootCanvas(ui->eventDisplayLeftCanvas		     	      ,"") ; // ToROOT6
    eventDisplayRightCanvas_		             = new QRootCanvas(ui->eventDisplayRightCanvas  		          ,"") ; // ToROOT6
    fineAlignmentPullsCanvasLeft_	             = new QRootCanvas(ui->fineAlignmentPullsCanvasLeft		          ,"") ; // ToROOT6
    fineAlignmentPullsCanvasRight_	             = new QRootCanvas(ui->fineAlignmentPullsCanvasRight		      ,"") ; // ToROOT6
    fineAlignmentPullsSize1CanvasLeft_	         = new QRootCanvas(ui->fineAlignmentPullsSize1CanvasLeft	      ,"") ; // ToROOT6
    fineAlignmentPullsSize1CanvasRight_	         = new QRootCanvas(ui->fineAlignmentPullsSize1CanvasRight	      ,"") ; // ToROOT6
    fineAlignmentPullsSize2CanvasLeft_	         = new QRootCanvas(ui->fineAlignmentPullsSize2CanvasLeft	      ,"") ; // ToROOT6
    fineAlignmentPullsSize2CanvasRight_	         = new QRootCanvas(ui->fineAlignmentPullsSize2CanvasRight	      ,"") ; // ToROOT6
    fineAlignmentResidualsCanvasLeft_	         = new QRootCanvas(ui->fineAlignmentResidualsCanvasLeft 	      ,"") ; // ToROOT6
    fineAlignmentResidualsCanvasRight_	         = new QRootCanvas(ui->fineAlignmentResidualsCanvasRight	      ,"") ; // ToROOT6
    fineAlignmentResidualsSize1CanvasLeft_       = new QRootCanvas(ui->fineAlignmentResidualsSize1CanvasLeft	  ,"") ; // ToROOT6
    fineAlignmentResidualsSize1CanvasRight_      = new QRootCanvas(ui->fineAlignmentResidualsSize1CanvasRight	  ,"") ; // ToROOT6
    fineAlignmentResidualsSize2CanvasLeft_       = new QRootCanvas(ui->fineAlignmentResidualsSize2CanvasLeft	  ,"") ; // ToROOT6
    fineAlignmentResidualsSize2CanvasRight_      = new QRootCanvas(ui->fineAlignmentResidualsSize2CanvasRight	  ,"") ; // ToROOT6
    fineAlignmentResXYvsXYCanvasLeft_	         = new QRootCanvas(ui->fineAlignmentResXYvsXYCanvasLeft 	      ,"") ; // ToROOT6
    fineAlignmentResXYvsXYCanvasRight_	         = new QRootCanvas(ui->fineAlignmentResXYvsXYCanvasRight	      ,"") ; // ToROOT6
    fineAlignmentResXYvsXYSize1CanvasLeft_       = new QRootCanvas(ui->fineAlignmentResXYvsXYSize1CanvasLeft	  ,"") ; // ToROOT6
    fineAlignmentResXYvsXYSize1CanvasRight_      = new QRootCanvas(ui->fineAlignmentResXYvsXYSize1CanvasRight	  ,"") ; // ToROOT6
    fineAlignmentResXYvsXYSize2CanvasLeft_       = new QRootCanvas(ui->fineAlignmentResXYvsXYSize2CanvasLeft	  ,"") ; // ToROOT6
    fineAlignmentResXYvsXYSize2CanvasRight_      = new QRootCanvas(ui->fineAlignmentResXYvsXYSize2CanvasRight	  ,"") ; // ToROOT6
    fineAlignmentResXYvsYXCanvasLeft_	         = new QRootCanvas(ui->fineAlignmentResXYvsYXCanvasLeft 	      ,"") ; // ToROOT6
    fineAlignmentResXYvsYXCanvasRight_	         = new QRootCanvas(ui->fineAlignmentResXYvsYXCanvasRight	      ,"") ; // ToROOT6
    loadCalibrationMainCanvas_                   = new QRootCanvas(ui->loadCalibrationMainCanvas   	              ,"") ; // ToROOT6
    mainTabsExpertCanvas_                        = new QRootCanvas(ui->mainTabsExpertCanvas   	                  ,"") ; // ToROOT6
    rawAlignmentLeftCanvas_		                 = new QRootCanvas(ui->rawAlignmentLeftCanvas		     	      ,"") ; // ToROOT6
    rawAlignmentRightCanvas_		             = new QRootCanvas(ui->rawAlignmentRightCanvas  		          ,"") ; // ToROOT6
    rawAlignmentSynpoticLeftCanvas_	             = new QRootCanvas(ui->rawAlignmentSynpoticLeftCanvas		      ,"") ; // ToROOT6
    rawAlignmentSynpoticRightCanvas_	         = new QRootCanvas(ui->rawAlignmentSynpoticRightCanvas  	      ,"") ; // ToROOT6
    residuals2DResidualsVsCoordinateLeftCanvas_  = new QRootCanvas(ui->residuals2DResidualsVsCoordinateLeftCanvas ,"") ; // ToROOT6
    residuals2DResidualsVsCoordinateRightCanvas_ = new QRootCanvas(ui->residuals2DResidualsVsCoordinateRightCanvas,"") ; // ToROOT6
    residualsManualFitLeftCanvas_	             = new QRootCanvas(ui->residualsManualFitLeftCanvas		          ,"") ; // ToROOT6
    residualsManualFitRightCanvas_	             = new QRootCanvas(ui->residualsManualFitRightCanvas		      ,"") ; // ToROOT6
    residualsPullsLeftCanvas_		             = new QRootCanvas(ui->residualsPullsLeftCanvas 		          ,"") ; // ToROOT6
    residualsPullsRightCanvas_		             = new QRootCanvas(ui->residualsPullsRightCanvas		          ,"") ; // ToROOT6
    residualsResidualsVsCoordinateLeftCanvas_    = new QRootCanvas(ui->residualsResidualsVsCoordinateLeftCanvas   ,"") ; // ToROOT6
    residualsResidualsVsCoordinateRightCanvas_   = new QRootCanvas(ui->residualsResidualsVsCoordinateRightCanvas  ,"") ; // ToROOT6
    residualsSynopticViewLeftCanvas_	         = new QRootCanvas(ui->residualsSynopticViewLeftCanvas  	      ,"") ; // ToROOT6
    residualsSynopticViewRightCanvas_	         = new QRootCanvas(ui->residualsSynopticViewRightCanvas 	      ,"") ; // ToROOT6
    trackFinderLeftCanvas_		                 = new QRootCanvas(ui->trackFinderLeftCanvas			          ,"") ; // ToROOT6
    trackFinderRightCanvas_		                 = new QRootCanvas(ui->trackFinderRightCanvas			          ,"") ; // ToROOT6
						 
//    ui->trackFinderLeftCanvas ->EnableSignalEvents(kMouseReleaseEvent); // ToROOT6
//    ui->trackFinderRightCanvas->EnableSignalEvents(kMouseReleaseEvent); // ToROOT6

    ui->loadedGeometryLE->setReadOnly(true);
    ui->xmlGeometryLE   ->setReadOnly(true);
    ui->loadedFileLE    ->setReadOnly(true);
    ui->outputFileLE    ->setReadOnly(true);

    statusBar_ = (mainWindow)->statusBar() ;

    path_ = QString(getenv("MonicelliDir")) ;

    // Initialize the CONTROLS combo box
    ui->fileEaterVerbosityCB->addItem("NULL"     );
    ui->fileEaterVerbosityCB->addItem("LOW"      );
    ui->fileEaterVerbosityCB->addItem("MEDIUM"   );
    ui->fileEaterVerbosityCB->addItem("HIGH"     );
    ui->fileEaterVerbosityCB->addItem("VERY HIGH");

    ui->fileEaterVerbosityCB->setCurrentIndex(kMEDIUM);

    //    ui->showAllPlaqPB->setToolTip("Show hits on all planes for the current event");
    ui->alignmentMonitorTW->setCurrentIndex(0) ;

    theFileEater_->setVerbosity(ui->fileEaterVerbosityCB->currentIndex());

    ui->parseFilePB->setCheckable  (true);
    ui->showAllPlaqPB->setCheckable(true);

/*
    ui->rawAlignmentLeftCanvas ->EnableSignalEvents(kMousePressEvent       ); // ToROOT6
    ui->rawAlignmentLeftCanvas ->EnableSignalEvents(kMouseDoubleClickEvent ); // ToROOT6
    ui->rawAlignmentRightCanvas->EnableSignalEvents(kMousePressEvent       ); // ToROOT6
    ui->rawAlignmentRightCanvas->EnableSignalEvents(kMouseDoubleClickEvent ); // ToROOT6
    ui->eventDisplayLeftCanvas ->EnableSignalEvents(kMousePressEvent       ); // ToROOT6
    ui->eventDisplayLeftCanvas ->EnableSignalEvents(kMouseDoubleClickEvent ); // ToROOT6
*/
//     connect(ui->rawAlignmentLeftCanvas,   SIGNAL(RootEventProcessed         (TObject *, unsigned int, TCanvas *)),
//             this,                         SLOT(  selectedCanvasObjectManager(TObject *, unsigned int, TCanvas *)) );
// 
//     connect(ui->rawAlignmentRightCanvas,  SIGNAL(RootEventProcessed         (TObject *, unsigned int, TCanvas *)),
//             this,                         SLOT(  selectedCanvasObjectManager(TObject *, unsigned int, TCanvas *)) );
// 
//     connect(ui->eventDisplayLeftCanvas,   SIGNAL(RootEventProcessed         (TObject *, unsigned int, TCanvas *)),
//             this,                         SLOT(  selectedCanvasObjectManager(TObject *, unsigned int, TCanvas *)) );


//    ui->eventDisplayLeftCanvas->GetCanvas()->SetBit(kNoContextMenu); // ToROOT6

    std::string telescopePicture = path_.toStdString() + std::string("/images/PixelTelescopeGeometry.jpg") ;
    QImage image(QString(telescopePicture.c_str()));
    if (image.isNull())
    {
        ss_.str("");
        ss_ << ACRed << ACBold << ACReverse
            << "FATAL:"
            << ACPlain
            << " Image " << telescopePicture
            << " not found. Aborting" ;
        STDLINE(ss_.str(),ACYellow) ;
        assert(0);
    }

    ui->pictureTelescopeLB->setPixmap(QPixmap::fromImage(image));

}

//===========================================================================
void mainTabs::initializeSingletons()
{
    if( !theThreader_         ) theThreader_          = new threader()                                                ;
    if( !theAligner_          ) theAligner_           = new aligner(theFileEater_, theHManager_)                                     ;
    if( !theFitter_           ) theFitter_            = new fitter()                                                  ;
    if( !theCalibrationLoader_) theCalibrationLoader_ = new calibrationLoader(theFileEater_, theHManager_, theFitter_);
    // if( !theParser_           ) theParser_            = new parser(theFileEater_);
    if( !theClusterizer_      ) theClusterizer_       = new clusterizer();
    if( !theTrackFitter_      ) theTrackFitter_       = new trackFitter();
    if( !theTrackFinder_      ) theTrackFinder_       = new trackFinder(theTrackFitter_);


    // Start timer to periodically update the main widget
    //if( !timer_ ) timer_  = new QTimer(this) ;
    if( !timer2_) timer2_ = new QTimer(this) ;
}

//===========================================================================
mainTabs::~mainTabs()
{
    STDLINE("Destructor in action: ui",ACWhite) ;
    if( theThreader_         ) delete theThreader_         ;
    if( theAligner_          ) delete theAligner_          ;
    if( theFitter_           ) delete theFitter_           ;
    if( theCalibrationLoader_) delete theCalibrationLoader_;
    if( theClusterizer_      ) delete theClusterizer_      ;
    if( theTrackFitter_      ) delete theTrackFitter_      ;
    if( theTrackFinder_      ) delete theTrackFinder_      ;
    delete ui;
    STDLINE("Destruction done",ACWhite) ;
}

//===========================================================================
void mainTabs::collectExistingWidgets(MainWindow * mainWindow)
{
    mainWindow_       = mainWindow ;
    theHManager_      = mainWindow_->getHManager()       ;
    theHNavigator_    = mainWindow_->getHNavigator()     ;
    theFileEater_     = mainWindow_->getFileEater()      ;
    theBeamSimulator_ = theFileEater_->getBeamSimulator();
    //theTrackFitter_   = theFileEater_->getTrackFitter()  ;
    //theClusterizer_   = theFileEater_->getClusterizer()  ;
}
//===========================================================================
void mainTabs::on_eatRootFilePB_clicked()
{
    this->signalNewAction("Load ROOT file") ;

    QString localPath = this->getEnvPath("MonicelliOutputDir") ;
    QString fileName = QFileDialog::getOpenFileName(this,"Event root tree files",localPath,"Root Tree Files(*.root)");
    if (fileName.isEmpty())  return ;

    ui->loadedRootFileLE->setToolTip(fileName) ;

    this->openRootFile(fileName);
    ui->loadedGeometryLE ->setText("No geometry xml file selected") ;
    ui->loadedFileLE     ->setText("No file selected") ;
}
//=============================================================================
void mainTabs::openRootFile(QString fileName)
{
    for(std::map<std::string,GeometryParameters*>::iterator it=geometryParameters_.begin(); it!=geometryParameters_.end(); it++)
    {
        ui->geometryDisplayTable->removeRow(0);
        delete it->second;
    }
    geometryParameters_.clear();

    theHManager_->setRunSubDir( theFileEater_->openFile(fileName.toStdString()) );
    theGeometry_ = theFileEater_->getGeometry();
    showGeometry();

    ui->loadedRootFileLE         ->setText(fileName);
    ui->geoFileLE                ->setText(fileName.replace(".root",".geo")) ;
    ui->geometryGeoGeometryFileLE->setText(fileName.replace(".root",".geo")) ;
    ui->geometryLoadedGeoFileLE  ->setText("No file loaded") ;
    ui->xmlGeometryLE            ->setText(QString(theGeometry_->getGeometryFileName().c_str())) ;

    ui->showBeamProfilesPB->setText("Beam Profiles") ;

    //to be disabled for a new root file
    //Calibrations tab
    ui->calibrationsCalibrationControlsGB      ->setEnabled(false) ;
    ui->calibrationsHistogramControlsGB        ->setEnabled(false) ;
    //Clusters tab
    ui->findAndSolveClustersPB                 ->setEnabled(false) ;
    //Raw Alignment tab
    ui->showBeamProfilesPB                     ->setEnabled(false) ;
    ui->rawAlignmentClusterProfilesRB          ->setEnabled(false) ;
    ui->rawAlignmentFitAllCB                   ->setEnabled(false) ;
    ui->rawAlignmentFitAllCB                   ->setChecked(true ) ;
    ui->rawAlignmentFitComparePB               ->setEnabled(false) ;
    ui->rawAlignmentPixelProfilesRB            ->setEnabled(false) ;
    ui->rawAlignmentTabPagePlaqSelectCB        ->setEnabled(false) ;
    //Track Finder tab
    ui->findTracksGB                           ->setEnabled(false) ;
    ui->slopeDisplayGB                         ->setEnabled(false) ;
    ui->trackFitEstimatorsGB                   ->setEnabled(false) ;
    // Residuals tab
    ui->residualsGB                            ->setEnabled(false) ;
    ui->residualPlotsGB                        ->setEnabled(false) ;
    //Fine Alignment tab
    ui->fineAlignmentTrackFinderGB             ->setEnabled(false);
    ui->fineAlignmentAlignmentControlsGB       ->setEnabled(false);
    ui->fineAlignmentCutsGB                    ->setEnabled(false);
    ui->fineAlignmentGeometryGB                ->setEnabled(false);
    ui->fineAlignmentGlobalFixGB               ->setEnabled(false);
    ui->detectorsTableView                     ->setEnabled(false);

    //DUT Alignment tab
    ui->dutAlignmentDUTHitsFinderGB            ->setEnabled(false);
    ui->dutAlignmentTracksSelectionGB          ->setEnabled(false);
    //    ui->dutAlignmentDutSelectCB                ->setEnabled(false) ;

    //    ui->saveCalibToROOTPB                      ->setEnabled(false) ;
    //    ui->detectorsToBeAlignedLW                 ->setEnabled(false) ;
    //    ui->eventDisplayDUTListW                   ->setEnabled(false) ;
    //    ui->eventDisplayDUTprojectionPB            ->setEnabled(false) ;
    //    ui->eventDisplayShowBeamSpotsPB            ->setEnabled(false) ;
    //    ui->eventDisplayTabPagePlaqSelectCB        ->setEnabled(false) ;
    //    ui->eventSelectedSpinBox                   ->setEnabled(false) ;
    //    ui->loadCalibrationsTabPagePlaqSelectCB    ->setEnabled(false) ;
    //    ui->selectRawEventsRB                      ->setEnabled(false) ;
    //    ui->showAllPlaqPB                          ->setEnabled(false) ;
    //    ui->showHitsFreqPB                         ->setEnabled(false) ;
    //    ui->trackFitterExcludedDetectorsListW      ->setEnabled(false) ;
    //    ui->trackFitterFitPB                       ->setEnabled(false) ;
    //    ui->residualsTabPagePlaqSelectCB           ->setEnabled(false) ;
    //    ui->trackFitterWriteAlignmentPB            ->setEnabled(false) ;
    //    ui->writeAlignmentPB                       ->setEnabled(false) ;

    if( theFileEater_->isRootTreeFile() )
    {
        EventHeader* theHeader = 0;
        //Calibrations tab
        ui->calibrationsCalibrationControlsGB  ->setEnabled(true) ;
        ui->calibrationsHistogramControlsGB    ->setEnabled(true) ;
        //Clusters tab
        ui->findAndSolveClustersPB             ->setEnabled(true) ;
        //Raw Alignment tab
        ui->showBeamProfilesPB                 ->setEnabled(true) ;
        ui->rawAlignmentPixelProfilesRB        ->setEnabled(true) ;

        if( (theHeader = theFileEater_->getHeader()) != 0)
        {
            ui->calLoadedCB           ->setChecked(theGeometry_->calibrationDone());
            ui->clustersBuiltCB       ->setChecked(theHeader->clustersDone());
            ui->tracksFoundCB         ->setChecked(theHeader->tracksFound() );

            ui->saveCalibToROOTPB     ->setEnabled(theGeometry_->calibrationDone());
            ui->buildPlotsPB          ->setEnabled(theHeader->clustersDone());
            ui->showAdcDistributionsPB->setEnabled(theHeader->clustersDone());

            //Raw Alignment tab
            ui->rawAlignmentClusterProfilesRB->setEnabled(theHeader->clustersDone()) ;
            //Track Finder tab
            ui->findTracksGB          ->setEnabled(theHeader->clustersDone()) ;
            ui->fineAlignmentTrackFinderGB->setEnabled(theHeader->tracksFound());
            ui->slopeDisplayGB        ->setEnabled(theHeader->tracksFound()) ;
            ui->trackFitEstimatorsGB  ->setEnabled(theHeader->tracksFound()) ;
            //Residuals tab
            ui->residualsGB           ->setEnabled(theHeader->tracksFound()) ;
            ui->residualPlotsGB       ->setEnabled(theHeader->tracksFound()) ;
            //Fine Alignment tab
            ui->fineAlignmentAlignmentControlsGB ->setEnabled(theHeader->tracksFound());
            ui->fineAlignmentCutsGB              ->setEnabled(theHeader->tracksFound());
            ui->fineAlignmentMinPointsSelectionCB->setEnabled(theHeader->tracksFound());
            ui->fineAlignmentMinPointsSelectionSB->setEnabled(theHeader->tracksFound());
            ui->fineAlignmentGeometryGB          ->setEnabled(theHeader->tracksFound());
            ui->fineAlignmentGlobalFixGB         ->setEnabled(theHeader->tracksFound());
            ui->detectorsTableView               ->setEnabled(theHeader->tracksFound());
            //DUT Alignment tab
            ui->dutAlignmentDUTHitsFinderGB      ->setEnabled(theHeader->tracksFound());
            ui->dutAlignmentTracksSelectionGB    ->setEnabled(theHeader->tracksFound());
        }

        //to be enabled if the file is a valid root Tree
        //        ui->detectorsToBeAlignedLW             ->setEnabled(true) ;
        //        ui->eventDisplayDUTListW               ->setEnabled(true) ;
        //        ui->eventDisplayDUTprojectionPB        ->setEnabled(true) ;
        //        ui->eventDisplayShowBeamSpotsPB        ->setEnabled(true) ;
        //        ui->eventSelectedSpinBox               ->setEnabled(true) ;

        //        ui->loadCalibrationsTabPagePlaqSelectCB->setEnabled(true) ;
        //        ui->selectRawEventsRB                  ->setEnabled(true) ;
        //        ui->showAllPlaqPB                      ->setEnabled(true) ;
        //        ui->showHitsFreqPB                     ->setEnabled(true) ;

        //fill combo boxes with detectors from geometry
        ui->detectorsTableView                 ->clearTable() ;
        ui->detectorsToBeAlignedLW             ->clear();
        ui->dutAlignmentDutSelectCB            ->clear();
        ui->eventDisplayDUTListW               ->clear();
        ui->eventDisplayTabPagePlaqSelectCB    ->clear();
        ui->loadCalibrationsTabPagePlaqSelectCB->clear();
        ui->rawAlignmentTabPagePlaqSelectCB    ->clear();
        ui->trackFitterExcludedDetectorsListW  ->clear();
        ui->residualsTabPagePlaqSelectCB       ->clear();//needs to be always after ui->detectorsToBeAlignedLW

        for(Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); it++)
        {
            ui->eventDisplayTabPagePlaqSelectCB    ->addItem(QString::fromStdString( it->first )  );
            ui->loadCalibrationsTabPagePlaqSelectCB->addItem(QString::fromStdString( it->first )  );
            ui->rawAlignmentTabPagePlaqSelectCB    ->addItem(QString::fromStdString( it->first )  );
            ui->trackFitterExcludedDetectorsListW  ->addItem(QString::fromStdString( it->first )  );
            ui->detectorsToBeAlignedLW             ->addItem(QString::fromStdString( it->first )  );
            ui->residualsTabPagePlaqSelectCB       ->addItem(QString::fromStdString( it->first )  );//needs to be always after ui->detectorsToBeAlignedLW
            if( (*it).second->isDUT() )
            {
                ui->eventDisplayDUTListW            ->addItem(QString::fromStdString( it->first ) );
                ui->dutAlignmentDutSelectCB         ->addItem(QString::fromStdString( it->first ) );
            }
            else
            {
                ui->detectorsTableView              ->addDetector(it->first, it->second->getZPosition());
            }
        }
        // Post the table
        ui->detectorsTableView->post();
        ui->fixAllZCB->setChecked(true);
        //fixStrips(1);

        // set geometry dependent widgets
        ui->residualsMinPointsSB->setMaximum( theGeometry_->getDetectorsNumber() );
        ui->residualsMinPointsSB->setValue  ( theGeometry_->getDetectorsNumber() );

        // activate combo box and show events
        //ui->eventDisplayTabPagePlaqSelectCB->setEnabled(true                                              );
        //this->on_eventDisplayTabPagePlaqSelectCB_currentIndexChanged(QString::fromStdString(plaqSelected_));
        ss_.str("");
        ss_ << theFileEater_->getEventsNumber();
        ui->totalEventsLE ->setText(QString::fromStdString(ss_.str()));
        ui->eventsInTreeLE->setText(QString::fromStdString(ss_.str()));
        ui->eventSelectedSpinBox->setMaximum(theFileEater_->getEventsNumber());
        //ss_ << " events on file: " ; STDLINE(ss_.str(),ACPurple) ;
    }

    if( !theHNavigator_ )
    {
        STDLINE("No navigator pointer available: requesting it to mainWinow",ACRed) ;
        theHNavigator_ = mainWindow_->getHNavigator() ;
    }

    if( theHNavigator_ ) theHNavigator_->addNewFile(fileName) ; // Inform the hNavigator about this new file

}
//=================================================================
void mainTabs::getNxNy(int divider, int &nx, int &ny)
{
    double intPart, fracPart;
    fracPart = modf( sqrt(divider), &intPart );
    if ( fracPart > 0 && fracPart < 0.5 ) fracPart = 1;
    else if ( fracPart > 0.5 )            fracPart = 2;
    fracPart = fracPart + intPart;
    nx = (int)intPart;
    ny = (int)fracPart;
}

//===========================================================================
void mainTabs::drawAll(QRootCanvas * where,               // ToROOT6
                       std::string   what,
                       std::string   detector,
                       std::string   options,
                       double        lowRange,
                       double        highRange,
                       int           nXCanvasDivisions,
                       int           nYCanvasDivisions,
                       int           tPad //=1
                       )
{
    if(!theGeometry_) return;

    if(nXCanvasDivisions == 0 || nYCanvasDivisions == 0)
        this->getNxNy((int)(theGeometry_->getDetectorsNumber()), nXCanvasDivisions, nYCanvasDivisions) ;
    if(tPad == 1)
    {
        where->GetCanvas()->Clear();
        where->GetCanvas()->Divide(nXCanvasDivisions, nYCanvasDivisions,0.001,0.001);
        //gStyle->SetTitleFontSize(0.06);
        //gStyle->SetOptFit (0)      ;
        //gStyle->SetOptStat(0)      ;
        //gStyle->SetPalette(1)    ;
        //gStyle->SetStatW(0.40)        ;
    }

    for( Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it)
    {
        if(detector != "" && detector != it->first) continue;

        where->GetCanvas()->cd(tPad) ;

        TObject * o = theHManager_->getHistogram( what, it->first );

        bool is2D = false ;

        if( o->IsA() == TH1D::Class() ) is2D = false ;
        if( o->IsA() == TH2D::Class() ) is2D = true  ;
        if( o->IsA() == TH1F::Class() ) is2D = false ;
        if( o->IsA() == TH2F::Class() ) is2D = true  ;

        if( is2D )
        {
            TH2D * h = (TH2D*)theHManager_->getHistogram( what, it->first );
            if( lowRange != highRange )  h->GetXaxis()->SetRangeUser(lowRange, highRange) ;
            h->Draw("COLZ");

        }
        else
        {
            TH1D * h = (TH1D*)theHManager_->getHistogram( what, it->first );
            if( lowRange != highRange )  h->GetXaxis()->SetRangeUser(lowRange, highRange) ;
            h->Draw(options.c_str());
        }

        where->GetCanvas()->Modified() ;
        where->GetCanvas()->Update()   ;
        ++tPad;
    }

}

//===========================================================================
void mainTabs::setLogAxis(QRootCanvas * where, std::string axis) // ToROOT6
{
    if (axis == "xaxis")
    {
        if ( where->GetCanvas()->GetSelectedPad()->GetLogx() )
             where->GetCanvas()->GetSelectedPad()->SetLogx(0);
        else                              
	     where->GetCanvas()->GetSelectedPad()->SetLogx(1);
    }
    if (axis == "yaxis")
    {
        if ( where->GetCanvas()->GetSelectedPad()->GetLogy() )
             where->GetCanvas()->GetSelectedPad()->SetLogy(0);
        else                              
	     where->GetCanvas()->GetSelectedPad()->SetLogy(1);
    }
    if (axis == "zaxis")
    {
        if ( where->GetCanvas()->GetSelectedPad()->GetLogz() )
             where->GetCanvas()->GetSelectedPad()->SetLogz(0);
        else                              
	     where->GetCanvas()->GetSelectedPad()->SetLogz(1);
    }
}
//=========================================================================
void mainTabs::selectedCanvasObjectManager(TObject *,unsigned int,TCanvas *)
{
/* // ToROOT6
    TQtWidget *tipped = (TQtWidget *)sender();
    ss_.str("");
    ss_ << obj->ClassName();
    std::string className = ss_.str();
    ss_.str("");
    ss_ << obj->GetName();
    std::string objName = ss_.str();
    const char* objInfo = obj->GetObjectInfo(tipped->GetEventX(),tipped->GetEventY());

    int binx=0,biny=0,binc=0;
    ss_.str("");
    ss_ << "\\(x=(\\d+)"            //[1]  x integer part
        << "\\.?(\\d+)"          //[2]  x decimal part
        << ",\\s+y=(\\d+)?"       //[3]  y integer part
        << "\\.?(\\d+)?"         //[4]  y decimal part
        << ",\\s+binx=(\\d+)"    //[5]  binx
        << ",\\s+biny=(\\d+)"    //[6]  biny
        << ",\\s+bin\\w=(\\d+).+"; //[7]  binc

    boost::cmatch what;
    static const boost::regex info(ss_.str().c_str(), boost::regex::perl);
    if( boost::regex_match(objInfo, what, info, boost::match_extra) )
    {
        binx = Utils::toInt(what[5]);
        biny = Utils::toInt(what[6]);
        binc = Utils::toInt(what[7]);
    }

    TAxis *axis = NULL ;
    if ( className == "TAxis" ) axis = (TAxis*)obj;
    TH2I  *th2i;
    if ( className == "TH2I"  ) th2i = (TH2I *)obj;

    std::stringstream tipText;
    tipText << "You have "   ;


    if (event == kButton1Down)
    {
        tipText << "LEFT CLICKED";
    }
    if (event == kButton1Double)
    {
        tipText << "DOUBLE LEFT CLICKED";
        if ( tipped == ui->eventDisplayLeftCanvas )
        {
            if ( className == "TAxis" ) this->setLogAxis(tipped, objName);//set log axis on double click
        }
    }
    if (event == kButton2Down)
    {
        tipText << "WHEEL CLICKED";
        if ( className == "TAxis" ) { axis->UnZoom(); }
        if ( className == "TH2I"  )
        {
//          STDLINE("TH2I",ACRed) ;
//          ss_.str("");
//          double leftB,rightB;
//          if( (binx-th2i->GetXaxis()->GetFirst()) < (th2i->GetXaxis()->GetLast()-binx) )
//          {
//            leftB  = th2i->GetXaxis()->GetFirst() + 1.*( binx - th2i->GetXaxis()->GetFirst() ) / ZOOMFACTOR;
//            rightB = th2i->GetXaxis()->GetLast()  - 2.*( th2i->GetXaxis()->GetLast() - binx  ) / leftB     ;
//          }
//          else
//          {
//            rightB  = th2i->GetXaxis()->GetLast()  - 1.*( th2i->GetXaxis()->GetLast() -binx   ) / ZOOMFACTOR;
//            leftB   = th2i->GetXaxis()->GetFirst() + 2.*( binx - th2i->GetXaxis()->GetFirst()  ) / rightB    ;
//          }
//          ss_<< th2i->GetXaxis()->GetLast();
//          STDLINE(ss_.str(),ACRed) ;
//          th2i->GetXaxis()->SetRange((int)leftB,100);
//          ss_<< th2i->GetXaxis()->GetLast();
//          STDLINE(ss_.str(),ACGreen) ;
//          th2i->GetXaxis()->SetRange((int)leftB,20);

//          double downB,upB;
//          ss_.str("");
//          if( (biny-th2i->GetYaxis()->GetFirst()) < (th2i->GetYaxis()->GetLast()-biny ) )
//          {
//            downB  = th2i->GetYaxis()->GetFirst() + 1.*( biny - th2i->GetYaxis()->GetFirst() ) / ZOOMFACTOR;
//            upB = th2i->GetYaxis()->GetLast()  - 2.*( th2i->GetYaxis()->GetLast()-biny  ) / downB     ;
//          }
//          else
//          {
//            upB  = th2i->GetYaxis()->GetLast()  - 1.*( th2i->GetYaxis()->GetLast()-biny   ) / ZOOMFACTOR;
//            downB   = th2i->GetYaxis()->GetFirst() + 2.*( biny - th2i->GetYaxis()->GetFirst()  ) / upB    ;
//          }
//          th2i->GetYaxis()->SetRange((int)downB,(int)upB);
//          ss_ << downB << " = " << upB;
//          STDLINE(ss_.str(),ACGreen) ;
        }
    }

    //    tipText << " the object <" << obj->GetName() << "> of class " << obj->ClassName()
    //            << " : " << obj->GetObjectInfo(tipped->GetEventX(),tipped->GetEventY()) << "\n"
    //            << "This object belongs to " <<  tipped->objectName().toStdString();
    //    STDLINE(tipText.str(),ACCyan) ;
*/ // ToROOT6
}

//=========================================================================
//*** EXPERT   MODE   TAB ****************************************************************************************
//---------------------------RAW DATA PROCESSING TAB------------------------------------------
//===========================================================================
void mainTabs::on_eatFilePB_clicked()
{
    this->signalNewAction("Load file") ;

    QString localPath = this->getEnvPath("Monicelli_RawData_Dir") ;
    QString fileName = QFileDialog::getOpenFileName(this,"Merged files",localPath,"Dat Files(*.dat);;Text files (*.txt)");
    if (fileName.isEmpty())  return ;

    if( !this->on_loadGeometryPB_clicked()) return ;

    ui->parseFilePB->setEnabled(true ) ;

    //theFileEater_->openFile(fileName.toStdString());
    ui->loadedFileLE->setText   (fileName);
    ui->loadedFileLE->setToolTip(fileName);
    ui->loadedRootFileLE ->setText("No file loaded");
    ui->geoFileLE ->setText("No file loaded");
    ui->geometryGeoGeometryFileLE ->setText("No file loaded");
    ui->geometryLoadedGeoFileLE ->setText("No file loaded");


    if ( fileName.endsWith(".dat") )
    {
        ui->writeASCIICB->setEnabled(true ) ;

        if(ui->writeASCIICB->isChecked())
        {
            ui->browseOutputFilePB->setEnabled(true ) ;
            ui->outputFileLE      ->setEnabled(true ) ;
        }
    }
    else
    {
        ui->writeASCIICB        ->setEnabled(false) ;
        ui->browseOutputFilePB  ->setEnabled(false) ;
        ui->outputFileLE        ->setEnabled(false) ;
    }

}

//===========================================================================
void mainTabs::on_showBeamSpotPB_clicked()
{
    gStyle->SetOptStat(0) ;
    gStyle->SetOptFit (0) ;

    this->signalNewAction("Show beam spot") ;

    theFileEater_->populate();
    int pad = 1 ;
    //std::map<std::string,TH2I*> vetH = theFileEater_->getHistograms() ;
    vetH_.clear();
    vetH_ = theFileEater_->getHistograms() ;

    for(std::map<std::string,TH2I*>::iterator hIt=vetH_.begin(); hIt!=vetH_.end(); ++hIt)
    {
        mainTabsExpertCanvas_->GetCanvas()->cd(pad++)    ;
        if( ui->limitZCB->isChecked())
        {
            hIt->second->SetMaximum(ui->limitZSB->value()) ;
        }
        hIt->second->Draw("COLZ")  ;
    }

    mainTabsExpertCanvas_->GetCanvas()->Modified() ;
    mainTabsExpertCanvas_->GetCanvas()->Update()   ;
    mainTabsExpertCanvas_->GetCanvas()->cd(0)	;

}

//=============================================================================
void mainTabs::endProcessSettings(process * currentProcess, bool success)
{
    std::string label= currentProcess->getLabel()    ;

    ss_.str("");
    ss_ << currentProcess->getName()  << " process is ended with signal: " <<  success;
    STDLINE(ss_.str(), ACYellow);

    if(success)
    {
        ss_.str("");
        ss_ << label << " successful";
        STDLINE(ss_.str(),ACGreen)  ;
        ui->parsingActivityLB->setText(QString::fromStdString(ss_.str()));
    }
    else
    {   ss_.str("");
        ss_ << label << " error";
        STDLINE(ss_.str(),ACRed);
        ui->parsingActivityLB  -> setText(QString::fromStdString(ss_.str()));
    }

    if( currentProcess == theFileEater_ )
    {
        if(theFileEater_->getOperation() == &fileEater::parse)
        {
            if(success)
            {
                ui->parseFilePB        -> setChecked(false)  ;
                ui->showBeamSpotPB     -> setEnabled(true)   ;
                ui->calibrationLoaderPB-> setEnabled(true)   ;
                this->openRootFile(QString::fromStdString(theFileEater_->getOutputTreeCompletePath()));
            }
            else
            {
                ui->parseFilePB        -> setEnabled(true)   ;
                ui->eatFilePB          -> setEnabled(true)   ;
                ui->loadGeometryPB     -> setEnabled(true)   ;
                ui->maxRawEventsCB     -> setEnabled(true)   ;
                ui->maxRawEventsAllCB  -> setEnabled(true)   ;
                if( ui->loadGeometryPB->isChecked() ) ui->maxRawEventsSB-> setEnabled(true) ;
            }

            ui->beamSpotCB       -> setEnabled(true)     ;
            theFileEater_        -> closeFile()          ;
        }
        if(theFileEater_->getOperation() == &fileEater::fullReconstruction)
        {
            this->on_reconstructEventsPB_clicked();
        }

        if(theFileEater_->getOperation() == &fileEater::updateEvents2)
        {
            fileEater::subProcessVDef currentSubProcess = theFileEater_->getCurrentSubProcess();
            for(unsigned int i=0; i<currentSubProcess.size(); i++)
            {
                if( currentSubProcess[i] == theClusterizer_ )
                {
                    ui->clustersBuiltCB              ->setChecked(true) ;
                    ui->rawAlignmentClusterProfilesRB->setEnabled(true) ;
                    ui->findTracksGB                 ->setEnabled(true) ;
                    ui->fineAlignmentTrackFinderGB   ->setEnabled(true) ;
                    ui->buildPlotsPB                 ->setEnabled(true) ;
                    ui->showAdcDistributionsPB       ->setEnabled(true) ;
                    //this->on_eventSelectedSpinBox_valueChanged(ui->eventSelectedSpinBox->value());
                    if(ui->buildPlotsCB->isChecked() ) this->on_buildPlotsPB_clicked();
                }

                if( currentSubProcess[i] == theTrackFinder_ )
                {
                    if(//theTrackFinder_->getOperation() == &trackFinder::findFirstAndLastTrackCandidates        ||
                            //theTrackFinder_->getOperation() == &trackFinder::findAllFirstAndLast               ||
                            //theTrackFinder_->getOperation() == &trackFinder::findRoadSearchTrackCandidates     ||
                            //theTrackFinder_->getOperation() == &trackFinder::findAllRoadSearch                 ||
                            //theTrackFinder_->getOperation() == &trackFinder::fitKalmanTrackCandidates          ||
                            //theTrackFinder_->getOperation() == &trackFinder::fitAllKalman                      ||
                            //theTrackFinder_->getOperation() == &trackFinder::fitSimpleTrackCandidates          ||
                            //theTrackFinder_->getOperation() == &trackFinder::fitAllSimple                      ||
                            theTrackFinder_->getOperation() == &trackFinder::findAndFitTracks)
                        //)
                    {
                        ui->tracksFoundCB         ->setChecked(true) ;

                        ui->slopeDisplayGB        ->setEnabled(true) ;
                        ui->trackFitEstimatorsGB  ->setEnabled(true) ;
                        //Residuals tab
                        ui->residualsGB           ->setEnabled(true) ;
                        ui->residualPlotsGB       ->setEnabled(true) ;
                        //Fine Alignment tab
                        ui->fineAlignmentTrackFinderGB             ->setEnabled(true);
                        ui->fineAlignmentAlignmentControlsGB       ->setEnabled(true);
                        ui->fineAlignmentCutsGB                    ->setEnabled(true);
                        ui->fineAlignmentMinPointsSelectionCB      ->setEnabled(true);
                        ui->fineAlignmentMinPointsSelectionSB      ->setEnabled(true);
                        ui->fineAlignmentGeometryGB                ->setEnabled(true);
                        ui->fineAlignmentGlobalFixGB               ->setEnabled(true);
                        ui->detectorsTableView                     ->setEnabled(true);
                        //DUT Alignment tab
                        ui->dutAlignmentDUTHitsFinderGB            ->setEnabled(true);
                        ui->dutAlignmentTracksSelectionGB          ->setEnabled(true);
                        redoChi2_ = true;
                    }
                }

                if( currentSubProcess[i] == theTrackFitter_ )
                {
                    if(theTrackFitter_->getOperation() == &trackFitter::makeFittedTracksResiduals)
                    {
                        ui->onlyDetectorsToBeAlignedCB ->setChecked(false);
                        ui->trackFitterWriteAlignmentPB->setEnabled(true );
                    }
                    if(theTrackFitter_->getOperation() == &trackFitter::makeFittedTrackDeviations)
                    {
                        ui->showDeviationsPB ->click()        ;
                        ui->trackFitterWriteAlignmentPB ->setEnabled(true )  ;
                    }
                }
            }
        }
        theFileEater_->clearSubProcess();
    }

    if( currentProcess == theAligner_ )
    {
        aligner::alignmentResultsDef alignmentResults = theAligner_->getAlignmentResults();

        ui->writeFineAlignmentResultsPB->setEnabled(true);
        HManager::stringVDef histoTypeV = theAligner_->getAlignmentPlots();
        if(theAligner_->getOperation() == &aligner::align)
            gStyle->SetOptStat(0) ;
        else
            gStyle->SetOptStat(1111) ;

        int tPad = 1;
        int numberOfTelescopeDetectors = theGeometry_->getDetectorsNumber(true);
        int numberOfDUTs = theGeometry_->getDetectorsNumber(false) - theGeometry_->getDetectorsNumber(true);
        //cout << __PRETTY_FUNCTION__ << "# of DUTs: " << numberOfDUTs << endl;
        float nSqrt = sqrt(numberOfTelescopeDetectors);
        float dutSqrt = sqrt(numberOfDUTs);
        int xPlots = ceil(nSqrt) ;
        int yPlots = ceil(nSqrt);
        int dutPlots = ceil(dutSqrt);
        for(Geometry::iterator geo=theGeometry_->begin(); geo!=theGeometry_->end(); geo++)
        {
            if(theAligner_->getOperation() == &aligner::align && !geo->second->isDUT())
            {
                this->drawAll(fineAlignmentResidualsCanvasLeft_      , histoTypeV[0] , geo->first, "", 0, 0, xPlots, yPlots, tPad);
                this->drawAll(fineAlignmentResidualsCanvasRight_     , histoTypeV[3] , geo->first, "", 0, 0, xPlots, yPlots, tPad);

                this->drawAll(fineAlignmentResidualsSize1CanvasLeft_ , histoTypeV[1] , geo->first, "", 0, 0, xPlots, yPlots, tPad);
                this->drawAll(fineAlignmentResidualsSize1CanvasRight_, histoTypeV[4] , geo->first, "", 0, 0, xPlots, yPlots, tPad);

                this->drawAll(fineAlignmentResidualsSize2CanvasLeft_ , histoTypeV[2] , geo->first, "", 0, 0, xPlots, yPlots, tPad);
                this->drawAll(fineAlignmentResidualsSize2CanvasRight_, histoTypeV[5] , geo->first, "", 0, 0, xPlots, yPlots, tPad);

                this->drawAll(fineAlignmentPullsCanvasLeft_	     , histoTypeV[6] , geo->first, "", 0, 0, xPlots, yPlots, tPad);
                this->drawAll(fineAlignmentPullsCanvasRight_	     , histoTypeV[9] , geo->first, "", 0, 0, xPlots, yPlots, tPad);

                this->drawAll(fineAlignmentPullsSize1CanvasLeft_     , histoTypeV[7] , geo->first, "", 0, 0, xPlots, yPlots, tPad);
                this->drawAll(fineAlignmentPullsSize1CanvasRight_    , histoTypeV[10], geo->first, "", 0, 0, xPlots, yPlots, tPad);

                this->drawAll(fineAlignmentPullsSize2CanvasLeft_     , histoTypeV[8] , geo->first, "", 0, 0, xPlots, yPlots, tPad);
                this->drawAll(fineAlignmentPullsSize2CanvasRight_    , histoTypeV[11], geo->first, "", 0, 0, xPlots, yPlots, tPad);

                if (ui->fillAllAlignmentPlotsCB->isChecked())
                {
                    this->drawAll(fineAlignmentResXYvsYXCanvasLeft_	 , histoTypeV[12], geo->first, "", 0, 0, xPlots, yPlots, tPad);
                    this->drawAll(fineAlignmentResXYvsYXCanvasRight_	 , histoTypeV[15], geo->first, "", 0, 0, xPlots, yPlots, tPad);

                    this->drawAll(fineAlignmentResXYvsYXSize1CanvasLeft_ , histoTypeV[13], geo->first, "", 0, 0, xPlots, yPlots, tPad);
                    this->drawAll(fineAlignmentResXYvsYXSize1CanvasRight_, histoTypeV[16], geo->first, "", 0, 0, xPlots, yPlots, tPad);

                    this->drawAll(fineAlignmentResXYvsYXSize2CanvasLeft_ , histoTypeV[14], geo->first, "", 0, 0, xPlots, yPlots, tPad);
                    this->drawAll(fineAlignmentResXYvsYXSize2CanvasRight_, histoTypeV[17], geo->first, "", 0, 0, xPlots, yPlots, tPad);

                    this->drawAll(fineAlignmentResXYvsXYCanvasLeft_	 , histoTypeV[18], geo->first, "", 0, 0, xPlots, yPlots, tPad);
                    this->drawAll(fineAlignmentResXYvsXYCanvasRight_	 , histoTypeV[21], geo->first, "", 0, 0, xPlots, yPlots, tPad);

                    this->drawAll(fineAlignmentResXYvsXYSize1CanvasLeft_ , histoTypeV[19], geo->first, "", 0, 0, xPlots, yPlots, tPad);
                    this->drawAll(fineAlignmentResXYvsXYSize1CanvasRight_, histoTypeV[22], geo->first, "", 0, 0, xPlots, yPlots, tPad);

                    this->drawAll(fineAlignmentResXYvsXYSize2CanvasLeft_ , histoTypeV[20], geo->first, "", 0, 0, xPlots, yPlots, tPad);
                    this->drawAll(fineAlignmentResXYvsXYSize2CanvasRight_, histoTypeV[23], geo->first, "", 0, 0, xPlots, yPlots, tPad);
                }
            }
            else if(theAligner_->getOperation() == &aligner::alignDUT && geo->second->isDUT())
            {
                this->drawAll(dutAlignmentResidualsCanvasLeft_	    , histoTypeV[0], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                this->drawAll(dutAlignmentResidualsCanvasRight_     , histoTypeV[3], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                this->drawAll(dutAlignmentResidualsSize1CanvasLeft_ , histoTypeV[1], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                this->drawAll(dutAlignmentResidualsSize1CanvasRight_, histoTypeV[4], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                this->drawAll(dutAlignmentResidualsSize2CanvasLeft_ , histoTypeV[2], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                this->drawAll(dutAlignmentResidualsSize2CanvasRight_, histoTypeV[5], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                this->drawAll(dutAlignmentPullsCanvasLeft_	    , histoTypeV[6] , geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                this->drawAll(dutAlignmentPullsCanvasRight_	    , histoTypeV[9] , geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                this->drawAll(dutAlignmentPullsSize1CanvasLeft_     , histoTypeV[7] , geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                this->drawAll(dutAlignmentPullsSize1CanvasRight_    , histoTypeV[10], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                this->drawAll(dutAlignmentPullsSize2CanvasLeft_     , histoTypeV[8] , geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                this->drawAll(dutAlignmentPullsSize2CanvasRight_    , histoTypeV[11], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                if (ui->fillAllDUTAlignmentPlotsCB->isChecked())
                {
                    this->drawAll(dutAlignmentResXYvsYXCanvasLeft_      , histoTypeV[12], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                    this->drawAll(dutAlignmentResXYvsYXCanvasRight_     , histoTypeV[15], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                    this->drawAll(dutAlignmentResXYvsYXSize1CanvasLeft_ , histoTypeV[13], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                    this->drawAll(dutAlignmentResXYvsYXSize1CanvasRight_, histoTypeV[16], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                    this->drawAll(dutAlignmentResXYvsYXSize2CanvasLeft_ , histoTypeV[14], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                    this->drawAll(dutAlignmentResXYvsYXSize2CanvasRight_, histoTypeV[17], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                    this->drawAll(dutAlignmentResXYvsXYCanvasLeft_      , histoTypeV[18], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                    this->drawAll(dutAlignmentResXYvsXYCanvasRight_     , histoTypeV[21], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                    this->drawAll(dutAlignmentResXYvsXYSize1CanvasLeft_ , histoTypeV[19], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                    this->drawAll(dutAlignmentResXYvsXYSize1CanvasRight_, histoTypeV[22], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);

                    this->drawAll(dutAlignmentResXYvsXYSize2CanvasLeft_ , histoTypeV[20], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                    this->drawAll(dutAlignmentResXYvsXYSize2CanvasRight_, histoTypeV[23], geo->first, "", 0, 0, dutPlots, dutPlots, tPad);
                }
            }
            else
                continue;


            QString guiText;
            ss_.str(""); ss_ << "Final corrections for detector: " << geo->first; STDLINE(ss_.str(),ACRed);
            char buffer[128];
            sprintf (buffer, "Alpha -> Correction = %7.3f deg", alignmentResults[geo->first].alpha);STDLINE(buffer,ACCyan);
            guiText = QString(buffer) + '\n';
            sprintf (buffer, "Beta  -> Correction = %7.3f deg", alignmentResults[geo->first].beta );STDLINE(buffer,ACCyan);
            guiText += QString(buffer) + '\n';
            sprintf (buffer, "Gamma -> Correction = %7.3f deg", alignmentResults[geo->first].gamma);STDLINE(buffer,ACCyan);
            guiText += QString(buffer) + '\n';
            sprintf (buffer, "X     -> Correction = %7.3f um", 10*alignmentResults[geo->first].deltaTx);STDLINE(buffer,ACCyan);
            guiText += QString(buffer) + '\n';
            sprintf (buffer, "Y     -> Correction = %7.3f um", 10*alignmentResults[geo->first].deltaTy);STDLINE(buffer,ACCyan);
            guiText += QString(buffer) + '\n';
            sprintf (buffer, "Z     -> Correction = %7.3f um", 10*alignmentResults[geo->first].deltaTz);STDLINE(buffer,ACCyan);
            guiText += QString(buffer);
            if(ui->dutAlignmentDutSelectCB->currentText().toStdString() == geo->first)
                ui->dutAlignmentResultsTB->setText(guiText);
            ++tPad;
        }
    }

    if( currentProcess == theCalibrationLoader_ )
    {
        theFileEater_->updateGeometry("geometry");
        ui->saveCalibToROOTPB  ->setEnabled(true);
        ui->calLoadedCB        ->setChecked(true );
    }

    if( currentProcess == theHManager_ )
    {
        HManager::HManager_Make_function subFunc   = theHManager_->getSubProcessFunction();
        HManager::stringVDef             fullPaths = theHManager_->getOutputPaths();

        if(subFunc == &HManager::makeResidualDistributions)
        {
            int totalTracksFound           = theHManager_->getTotalTracksFound();
            int totalEventsWithTracksFound = theHManager_->getTotalEventsWithTracksFound();
            ui->passingEventsLE->setText(QString("%1/").arg(totalEventsWithTracksFound)+ui->eventsInTreeLE->text());
            double tracksPerEvent = 0;
            if(totalEventsWithTracksFound != 0)
                tracksPerEvent = (1.*totalTracksFound) / (1.*totalEventsWithTracksFound);
            ui->tracksPerEventFoundLE->setText(QString("%1").arg(tracksPerEvent));

            ss_.str("");
            ss_ << "Events passing selection: " << totalEventsWithTracksFound << "/" << ui->eventsInTreeLE->text().toStdString();
            STDLINE(ss_.str(),ACCyan);
            ss_.str("");
            ss_ << "Total tracks found: " << totalTracksFound << " with " << tracksPerEvent << " tracks per event.";
            STDLINE(ss_.str(),ACCyan);
        }

        if(fullPaths.size() == 0) return;

        if(subFunc == &HManager::makeAdcDistributions2)
        {
            ui->clusterResultsTab->setCurrentIndex(4);
            this->drawAll(chargeADCCanvas_      , fullPaths[0]);
            this->drawAll(chargeElectronsCanvas_, fullPaths[1]);
            this->drawAll(clustersCanvas_       , fullPaths[1]);
        }
        else if(subFunc == &HManager::makeResidualDistributions)
        {
            double lowRange = 0 ;
            double higRange = 0 ;
            residualsType_.clear();

            if( theHManager_->getResidualsToPlot() == "showResiduals" || theHManager_->getResidualsToPlot() == "showDeviations" )
            {
                residualsType_ = fullPaths;
                lowRange = ui->residualRangeLowCB->currentText().toDouble() ;
                higRange = ui->residualRangeHigCB->currentText().toDouble() ;
                this->drawAll(residualsSynopticViewLeftCanvas_ , fullPaths[0], "", "", lowRange, higRange);
                this->drawAll(residualsSynopticViewRightCanvas_, fullPaths[1], "", "", lowRange, higRange);
            }
            else if( theHManager_->getResidualsToPlot() == "showPulls" )
            {
                this->drawAll(residualsPullsLeftCanvas_ , fullPaths[0], "", "", lowRange, higRange);
                this->drawAll(residualsPullsRightCanvas_, fullPaths[1], "", "", lowRange, higRange);
            }
            else if( theHManager_->getResidualsToPlot() == "showScatterResiduals" )
            {

                this->drawAll(residuals2DResidualsVsCoordinateLeftCanvas_ , fullPaths[0], "", "", lowRange, higRange);
                this->drawAll(residuals2DResidualsVsCoordinateRightCanvas_, fullPaths[1], "", "", lowRange, higRange);
                fullPaths = theHManager_->makeMeanScatterResidual(fullPaths, true);
                if     ( ui->xResidualZRotationRB->isChecked() ||
                         ui->yResidualZRotationRB->isChecked()    )
                {
                    this->drawAll(residualsResidualsVsCoordinateLeftCanvas_ , fullPaths[0], "", "COLZ");
                    this->drawAll(residualsResidualsVsCoordinateRightCanvas_, fullPaths[1], "", "COLZ");
                }
                else if( ui->xResidualYRotationRB->isChecked() ||
                         ui->yResidualXRotationRB->isChecked()    )
                {
                    this->drawAll(residualsResidualsVsCoordinateLeftCanvas_ , fullPaths[2], "", "COLZ");
                    this->drawAll(residualsResidualsVsCoordinateRightCanvas_, fullPaths[3], "", "COLZ");
                }
            }


            if( theHManager_->getResidualsToPlot() == "showResiduals" || theHManager_->getResidualsToPlot() == "showDeviations" )
            {
                ui->residualsTabPagePlaqSelectCB  ->setEnabled(true);
                ui->trackFitterFitPB              ->setEnabled(true);
                ui->trackFitterWriteAlignmentPB   ->setEnabled(true);
                this->on_residualsTabPagePlaqSelectCB_currentIndexChanged(ui->residualsTabPagePlaqSelectCB->currentText());
            }
            else
            {
                ui->residualsTabPagePlaqSelectCB  ->setEnabled(false);
                ui->trackFitterFitPB              ->setEnabled(false);
                ui->trackFitterWriteAlignmentPB   ->setEnabled(false);
            }
        }
    }
}
//=============================================================================
void mainTabs::endThreadSettings(threadEnd_Function function, HManager::stringVDef histoType)
{
    STDLINE("Closing thread...",ACYellow);
    (this->*function)(histoType);
    STDLINE("Thread closed!"   ,ACGreen );
}
//=============================================================================
void mainTabs::advanceProgressBar2()
{
    ui->parseProgressBar->setValue(theThreader_->getCurrentIteration());

//    bool                           success = false;
//    if(theThreader_->isFinished()) success = true;

    if( !theThreader_->isRunning())
    {
        STDLINE("All events have been processed",ACPurple) ;
        timer2_->stop();
        if( !theBenchmark_ )
        {
            STDLINE("=============== No Benchmark was setup =================",ACWhite) ;
        }
        else
        {
            STDLINE("=============== Benchmark =====================",ACWhite) ;
            cout << endl ;
            theBenchmark_->Stop(theThreadProcess_.c_str());
            Float_t realTime = 0 ;
            Float_t CPUTime  = 0 ;
            cout << "                      " ;
            theBenchmark_->Summary(realTime, CPUTime);
            cout << endl ;
            ss_.str("") ; ss_ << "Process: " << theThreadProcess_ << "\t\t\tElapsed time: " << realTime << "\tCPU time: " << CPUTime << "\n";
            ui->benchmarksTE->insertPlainText(ss_.str().c_str()) ;
            STDLINE("=============== Benchmark =====================",ACWhite) ;
        }
        delete theBenchmark_ ;
        theBenchmark_ = NULL ;
        ui->parseProgressBar->setValue(ui->parseProgressBar->maximum()) ;
        //this->endProcessSettings(theThreader_->getCurrentProcess(),success);
    }
}
//=============================================================================
void mainTabs::on_abortActionPB_clicked()
{
    STDLINE("Sent ABORT signal (timer stopped as well)",ACRed) ;

    ui->abortActionPB->setEnabled(false ) ;

    QPalette palette;
    QBrush   redBrush(QColor(255, 0, 0, 255));
    redBrush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active,   QPalette::WindowText, redBrush);
    ui->parsingActivityLB->setPalette(palette                   );
    ui->parsingActivityLB->setText   ("WARNING: action aborted!");

    theThreader_->terminate();
    timer2_->stop();
}

//=============================================================================
void mainTabs::on_parseFilePB_clicked()
{
    theFileEater_->openFile        ( ui->loadedGeometryLE->text().toStdString() );
    theFileEater_->setInputFileName( ui->loadedFileLE->text().toStdString()     );
    //theFileEater_->openFile( ui->loadedFileLE->text().toStdString() );

    if (ui->maxRawEventsCB->isChecked())
        theFileEater_->setEventsLimit( ui->maxRawEventsSB->value() );
    else
        theFileEater_->setEventsLimit( -1 );

    theFileEater_->setOperation(&fileEater::parse);
    this->launchThread2(theFileEater_);
}

//=============================================================================
void mainTabs::swapParseButtonText(bool parsing)
{
    if( parsing )
    {
        ui->parseFilePB       ->setEnabled(false) ;
        ui->abortActionPB     ->setEnabled(true ) ;
        ui->eatFilePB         ->setEnabled(false) ;
        ui->loadGeometryPB    ->setEnabled(false) ;
        ui->writeASCIICB      ->setEnabled(false) ;
        ui->browseOutputFilePB->setEnabled(false) ;
        ui->outputFileLE      ->setEnabled(false) ;
        ui->showBeamSpotPB    ->setEnabled(false) ;
        ui->maxRawEventsCB    ->setEnabled(false) ;
        ui->maxRawEventsSB    ->setEnabled(false) ;
    }
    else
    {
        ui->parseFilePB       ->setEnabled(true ) ;
        ui->abortActionPB     ->setEnabled(false) ;
        ui->eatFilePB         ->setEnabled(true );
        ui->loadGeometryPB    ->setEnabled(true );
        ui->maxRawEventsCB    ->setEnabled(true );
        ui->maxRawEventsSB    ->setEnabled(true );

        if(ui->loadedFileLE->text().endsWith(".dat"))
        {
            ui->writeASCIICB        ->setEnabled(true ) ;
            if(ui->writeASCIICB     ->isChecked (     ))
            {
                ui->browseOutputFilePB->setEnabled(true ) ;
                ui->outputFileLE      ->setEnabled(true ) ;
            }
        }
    }
}

//=============================================================================
void mainTabs::on_writeASCIICB_toggled(bool checked)
{
    theFileEater_->setWriteOutASCII(checked);
    if(checked)
    {
        ui->browseOutputFilePB->setEnabled(true ) ;
        ui->outputFileLE      ->setEnabled(true ) ;
    }
    else
    {
        ui->browseOutputFilePB->setEnabled(false ) ;
        ui->outputFileLE      ->setEnabled(false ) ;
    }
}

//=============================================================================
void mainTabs::on_outputFileLE_textChanged(QString outputPath)
{
    theFileEater_->setOutputFileName(outputPath.toStdString());
}

//=============================================================================
void mainTabs::on_browseOutputFilePB_clicked()
{
    QString outputFileName = QFileDialog::getSaveFileName(this,"Output ASCII file",path_,"Txt Files(*.txt);;All Files(*.*)");
    if (outputFileName.isEmpty())  return ;
    ui->outputFileLE->setText(outputFileName);
}

//=====================================================================================
bool mainTabs::on_loadGeometryPB_clicked()
{
    return loadGeometry() ;
}

//=====================================================================================
bool mainTabs::on_loadXMLGeometryPB_clicked()
{
    return loadGeometry("xml") ;
}

//=====================================================================================
bool mainTabs::on_loadGeoGeometryPB_clicked()
{
    return loadGeometry("geo") ;
}

//=====================================================================================
bool mainTabs::loadGeometry(QString type)
{
    QString fileType = "xml Files(*.xml);;geo Files(*.geo)";
    if(type == "xml")
        fileType = "xml Files(*.xml)";
    else if(type == "geo")
        fileType = "geo Files(*.geo)";

    QString localPath = this->getEnvPath("Monicelli_XML_Dir") ;
    QString fileName = QFileDialog::getOpenFileName(this,"testBeamGeometry files",localPath,fileType);
    if (fileName.isEmpty()) return false  ;

    theFileEater_->openFile(fileName.toStdString());
    theGeometry_ = theFileEater_->getGeometry();
    showGeometry();
    theGeometry_->dump();

    Geometry::detectorsMapDef dets = theGeometry_->getDetectors() ;
    ss_.str("") ;
    int row  = 0 ;
    int rows = 0 ;
    for(Geometry::detectorsMapDef::iterator d=dets.begin(); d!=dets.end(); ++d)
    {
        rows += d->second->getNumberOfROCs();
    }
    ui->detGeoFileDetailsTW->setRowCount(rows);
    for(Geometry::detectorsMapDef::iterator d=dets.begin(); d!=dets.end(); ++d)
    {
        ss_.str("") ; ss_ << d->second->getID() ;
        QTableWidgetItem *rItemA = new QTableWidgetItem(ss_.str().c_str());
        rItemA->setTextAlignment(Qt::AlignVCenter);
        rItemA->setTextAlignment(Qt::AlignRight);
        ui->detGeoFileDetailsTW->setItem(row, 0, rItemA);
        ui->detGeoFileDetailsTW->setColumnWidth(0, 115) ;

        ss_.str("") ; ss_ << d->second->getName() ;
        QTableWidgetItem *rItemB = new QTableWidgetItem(ss_.str().c_str());
        rItemB->setTextAlignment(Qt::AlignVCenter);
        rItemB->setTextAlignment(Qt::AlignRight);
        ui->detGeoFileDetailsTW->setItem(row, 1, rItemB);
        ui->detGeoFileDetailsTW->setColumnWidth(1, 80) ;

        ss_.str("") ; ss_ << d->second->getNumberOfROCs() ;
        QTableWidgetItem *rItemC = new QTableWidgetItem(ss_.str().c_str());
        rItemC->setTextAlignment(Qt::AlignVCenter);
        rItemC->setTextAlignment(Qt::AlignRight);
        ui->detGeoFileDetailsTW->setItem(row, 2, rItemC);
        ui->detGeoFileDetailsTW->setColumnWidth(2, 50) ;

        ss_.str("") ; ss_ << d->second->getDetectorLengthX() ;
        QTableWidgetItem *rItemD = new QTableWidgetItem(ss_.str().c_str());
        rItemD->setTextAlignment(Qt::AlignVCenter);
        rItemD->setTextAlignment(Qt::AlignRight);
        ui->detGeoFileDetailsTW->setItem(row, 3, rItemD);
        ui->detGeoFileDetailsTW->setColumnWidth(3, 70) ;

        ss_.str("") ; ss_ << d->second->getDetectorLengthY() ;
        QTableWidgetItem *rItemE = new QTableWidgetItem(ss_.str().c_str());
        rItemE->setTextAlignment(Qt::AlignVCenter);
        rItemE->setTextAlignment(Qt::AlignRight);
        ui->detGeoFileDetailsTW->setItem(row, 4, rItemE);
        ui->detGeoFileDetailsTW->setColumnWidth(4, 70) ;

        ss_.str("") ;
        QBrush b(QColor(255,255,255)) ;
        if( d->second->isDUT() )
        {
            ss_ << "X";
            b.setColor(QColor(255,121,116)) ;
        }
        QTableWidgetItem *rItemF = new QTableWidgetItem(ss_.str().c_str());
        rItemF->setTextAlignment(Qt::AlignVCenter);
        rItemF->setTextAlignment(Qt::AlignRight);
        rItemF->setBackground(b) ;
        ui->detGeoFileDetailsTW->setItem(row, 5, rItemF);
        ui->detGeoFileDetailsTW->setColumnWidth(5, 40) ;

        for(unsigned int r=0; r<d->second->getNumberOfROCs(); ++r)
        {
            ROC * theROC = d->second->getROCByPosition(r) ;

            ss_.str("") ; ss_ << theROC->getID();
            QTableWidgetItem *rItemG = new QTableWidgetItem(ss_.str().c_str());
            rItemG->setTextAlignment(Qt::AlignVCenter);
            rItemG->setTextAlignment(Qt::AlignRight);
            ui->detGeoFileDetailsTW->setItem(row, 6, rItemG);
            ui->detGeoFileDetailsTW->setColumnWidth(6, 40) ;

            ss_.str("") ; ss_ << theROC->getNumberOfRows();
            QTableWidgetItem *rItemH = new QTableWidgetItem(ss_.str().c_str());
            rItemH->setTextAlignment(Qt::AlignVCenter);
            rItemH->setTextAlignment(Qt::AlignRight);
            ui->detGeoFileDetailsTW->setItem(row, 7, rItemH);
            ui->detGeoFileDetailsTW->setColumnWidth(7, 50) ;

            ss_.str("") ; ss_ << theROC->getNumberOfCols();
            QTableWidgetItem *rItemI = new QTableWidgetItem(ss_.str().c_str());
            rItemI->setTextAlignment(Qt::AlignVCenter);
            rItemI->setTextAlignment(Qt::AlignRight);
            ui->detGeoFileDetailsTW->setItem(row, 8, rItemI);
            ui->detGeoFileDetailsTW->setColumnWidth(8, 50) ;

            ss_.str("") ; ss_ << theROC->getCalibrationFilePath();
            QTableWidgetItem *rItemD = new QTableWidgetItem(ss_.str().c_str());
            rItemD->setTextAlignment(Qt::AlignVCenter);
            rItemD->setTextAlignment(Qt::AlignRight);
            ui->detGeoFileDetailsTW->setItem(row++, 9, rItemD);
        }
    }

    ui->loadedGeometryLE->setText   (fileName);
    ui->loadedGeometryLE->setToolTip(fileName);
    ui->xmlGeometryLE   ->setText   (theGeometry_->getGeometryFileName().c_str());
    ui->xmlGeometryLE   ->setToolTip(theGeometry_->getGeometryFileName().c_str());

    QString geoFileName = ui->loadedRootFileLE->text().replace(".root",".geo");
    ui->geoFileLE                ->setText(geoFileName) ;
    ui->geometryGeoGeometryFileLE->setText(geoFileName) ;
    if(fileName.contains(".geo"))
        ui->geometryLoadedGeoFileLE  ->setText(fileName) ;
    else
        ui->geometryLoadedGeoFileLE  ->setText("No file loaded") ;
    /*
    //divide canvas from geometry
    double intPart, fracPart;
    fracPart = modf( sqrt(theGeometry_->getDetectorsNumber()), &intPart );
    if ( fracPart > 0 && fracPart < 0.5 ) fracPart = 1;
    else if ( fracPart > 0.5 )            fracPart = 2;
    fracPart = fracPart + intPart;
    ui->mainTabsExpertCanvas  ->GetCanvas()->Clear();
    ui->mainTabsExpertCanvas  ->GetCanvas()->Divide((int)intPart, (int)fracPart,0.001,0.001);
*/
    if(type == "geo")
    {
        STDLINE("type == geo",ACRed);
        if(ui->loadedRootFileLE->text().contains(".root"))
            theFileEater_->makeGeometryTreeFile(ui->loadedRootFileLE->text().toStdString());
    }
    return true ;
}

//=====================================================================================
//---------------------------CALIBRATION LOADER TAB------------------------------------------
//===========================================================================
//=====================================================================================
void mainTabs::on_calibrationLoaderPB_clicked()
{
    //    QString localPath = "./calibrations/" ;
    //    QString fileName = QFileDialog::getOpenFileName(this,"testBeamCalibration files",localPath,"txt Files(*.txt)");
    //    if (fileName.isEmpty()) return;

    //    theCalibrationLoader_->loadASCIIcalibrationFile(fileName.toStdString());
    //    theCalibrationLoader_->makeHistograms(true);

    theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString());
    theCalibrationLoader_->setOutputASCIIfile(ui->writeASCIIcalibrationFileCB->isChecked());
    theCalibrationLoader_->setInputROOTfile(  ui->readROOTcalibrationFileCB->isChecked()  );
    this->launchThread2(theCalibrationLoader_);
}

//===========================================================================
void mainTabs::on_showPixelCalibrationPB_clicked()
{
    gStyle->SetOptStat(1111) ;
    gStyle->SetOptFit(111111);

    loadCalibrationMainCanvas_->GetCanvas()->cd();
    loadCalibrationMainCanvas_->GetCanvas()->Pad()->SetObjectStat(true);
    TH1* histo = theCalibrationLoader_->getHistogram(ui->loadCalibrationsTabPagePlaqSelectCB->currentText().toStdString(),
                                                     ui->calibrationROCSB->value(),
                                                     ui->calibrationRowSB->value(),
                                                     ui->calibrationColSB->value());

    histo->Draw();

    if(histo->GetFunction(theFitter_->getCalibrationFitFunctionName()) == 0)
        return;
    histo->GetFunction(theFitter_->getCalibrationFitFunctionName())->Draw("same");

    loadCalibrationMainCanvas_->GetCanvas()->Modified();
    loadCalibrationMainCanvas_->GetCanvas()->Update();

    //    theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString());

    //    //check
    //    theGeometry_ = theFileEater_->getGeometry();
    //    for (Geometry::iterator det=theGeometry_->begin(); det!=theGeometry_->end(); ++det)
    //    {
    //        for(Detector::iterator roc=(*det).second->begin(); roc!=(*det).second->end(); roc++ )
    //        {
    //            ss_.str("") ;
    //            ss_ << "Detector: " << (*det).first << " rocID: " << (*roc).second->getID() << " roc position " << (*roc).first;

    //            if((*roc).second->getCalibrationFunction(ui->calibrationRowSB->value(),ui->calibrationColSB->value())!=NULL)
    //                    ss_ << " par0: " << (*roc).second->getCalibrationFunction(ui->calibrationRowSB->value(),ui->calibrationColSB->value())[0];
    //            else    ss_ << " is empty";

    //            STDLINE(ss_.str(),ACCyan);
    //        }
    //    }
}

//===========================================================================
//---------------------------BUILD CLUSTERS TAB------------------------------
//===========================================================================
void mainTabs::on_findAndSolveClustersPB_clicked()
{
    if(ui->clustersUseEtaFunctionCB->isChecked())
        theClusterizer_->getChargeAsymmetryPlots(theGeometry_);
    else
        theClusterizer_->setUseEtaFunction(false);

    theFileEater_->setOperation(&fileEater::updateEvents2,theClusterizer_);
    this->launchThread2(theFileEater_);
}
//===========================================================================
void mainTabs::on_showAdcDistributionsPB_clicked()
{
    this->signalNewAction("Building cluster charge distribution plots");
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );
    theHManager_->setOperation(&HManager::eventsCycle, &HManager::makeAdcDistributions2);
    this->launchThread2(theHManager_);
}


//===========================================================================
//-----------------------------------------RAW ALIGNMENT-----------------------------------------
//===========================================================================
void mainTabs::on_showBeamProfilesPB_clicked()
{
    this->launchThread3(theHManager_, this, &mainTabs::showBeamProfiles);
}
//===========================================================================
void mainTabs::showBeamProfiles()
{
    bool redo = (ui->showBeamProfilesPB->text() == "Fit")? false:true;
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    HManager::stringVDef histoType; // (X[1],Y[2])
    if(ui->rawAlignmentClusterProfilesRB->isChecked())
    {
        theHManager_->setSubProcessFunction(&HManager::makeClusterPlots2,redo);
        histoType = theHManager_->eventsCycle();
    }
    else
    {
        theHManager_->setSubProcessFunction(&HManager::makeBeamSpots2,redo);
        histoType = theHManager_->eventsCycle();
    }
    emit mainTabs::processFinished(&mainTabs::showBeamProfiles_end, histoType);
}
//============================================================================
void mainTabs::showBeamProfiles_end(HManager::stringVDef histoType)
{
    gStyle->SetOptStat(1111);

    if ( ui->rawAlignmentTabPagePlaqSelectCB->isEnabled() )
    {
        if( ui->rawAlignmentFitAllCB->isChecked() )
        {
            int pad = 1;
            TH1* histo = 0;
            for( Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it, pad++ )
            {
                histo = (TH1*) theHManager_->getHistogram(histoType[1], (*it).first );
                theFitter_->gaussFit( histo );
                rawAlignmentSynpoticLeftCanvas_->GetCanvas()->cd(pad)->Modified() ;
                rawAlignmentSynpoticLeftCanvas_->GetCanvas()->cd(pad)->Update() ;

                histo = (TH1*) theHManager_->getHistogram(histoType[2], (*it).first );
                theFitter_->gaussFit( histo );
                rawAlignmentSynpoticRightCanvas_->GetCanvas()->cd(pad)->Modified();
                rawAlignmentSynpoticRightCanvas_->GetCanvas()->cd(pad)->Update();
            }
        }
        else
        {
            if( !ui->rawAlignmentMyFitParXCB->isChecked() )
                theFitter_->gaussFit( (TH1*) theHManager_->getHistogram(histoType[1], plaqSelected_ ) );
            else
                theFitter_->gaussFit( (TH1*) theHManager_->getHistogram(histoType[1], plaqSelected_ ),
                        Utils::toDouble( ui->xProfileMeanLE ->text().toStdString() ),
                        Utils::toDouble( ui->xProfileSigmaLE->text().toStdString() ),
                        ui->xProfileNsigmaSB->value()                                 );

            if( !ui->rawAlignmentMyFitParYCB->isChecked() )
                theFitter_->gaussFit( (TH1*) theHManager_->getHistogram(histoType[2], plaqSelected_ ) );
            else
                theFitter_->gaussFit( (TH1*) theHManager_->getHistogram(histoType[2], plaqSelected_ ),
                        Utils::toDouble( ui->yProfileMeanLE ->text().toStdString()  ),
                        Utils::toDouble( ui->yProfileSigmaLE->text().toStdString()  ),
                        ui->xProfileNsigmaSB->value()                                  );
        }
        ui->writeAlignmentPB        ->setEnabled(true);
        ui->rawAlignmentFitComparePB->setEnabled(true);
    }


    //Draw histograms
    gStyle->SetOptFit(0);
    this->drawAll(rawAlignmentSynpoticLeftCanvas_ , histoType[1]);
    this->drawAll(rawAlignmentSynpoticRightCanvas_, histoType[2]);

    ui->showBeamProfilesPB->setText("Fit")               ;
    ui->showBeamProfilesPB->update()                     ;
    ui->rawAlignmentTabPagePlaqSelectCB->setEnabled(true);
    ui->rawAlignmentFitAllCB->setEnabled(true)           ;
    emit mainTabs::on_rawAlignmentTabPagePlaqSelectCB_currentIndexChanged(ui->rawAlignmentTabPagePlaqSelectCB->currentText());
    STDLINE("Profiles fits done",ACPurple) ;
}
//===========================================================================
void mainTabs::on_rawAlignmentTabPagePlaqSelectCB_currentIndexChanged(QString plaqSelected)
{
    if( ui->rawAlignmentTabPagePlaqSelectCB->isEnabled() )
    {
        plaqSelected_= plaqSelected.toStdString();
        bool redo = (ui->showBeamProfilesPB->text() == "Fit")? false:true;
        theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

        HManager::stringVDef histoType; // (X[1],Y[2])
        if(ui->rawAlignmentClusterProfilesRB->isChecked())
        {
            theHManager_->setSubProcessFunction(&HManager::makeClusterPlots2,redo);
            histoType = theHManager_->eventsCycle();
        }
        else
        {
            theHManager_->setSubProcessFunction(&HManager::makeBeamSpots2,redo);
            histoType = theHManager_->eventsCycle();
        }
        TF1 *fitFunc;
        TH1 *histo;
        //xProfile
        histo = (TH1*)theHManager_->getHistogram(histoType[1], plaqSelected_);
        if ( (fitFunc = (TF1*)histo->FindObject("gausFitFunc")) )
        {
            gStyle->SetOptFit(11);
            ui->xProfileMeanLE->setText(QString::number(fitFunc->GetParameter(0)));
            ui->xProfileSigmaLE->setText(QString::number(fitFunc->GetParameter(1)));
        }
        else
        {
            gStyle->SetOptFit(0);
            ui->xProfileMeanLE->setText(QString::number(histo->GetMean()));
            ui->xProfileSigmaLE->setText(QString::number(histo->GetRMS()));
        }
        rawAlignmentLeftCanvas_->GetCanvas()->cd();
        histo->Draw();
        rawAlignmentLeftCanvas_->GetCanvas() ->Modified();
        rawAlignmentLeftCanvas_->GetCanvas() ->Update()  ;

        //yProfile
        histo = (TH1*)theHManager_->getHistogram(histoType[2], plaqSelected_);
        if ( (fitFunc = (TF1*)histo->FindObject("gausFitFunc")) )
        {
            gStyle->SetOptFit(11);
            ui->yProfileMeanLE->setText(QString::number(fitFunc->GetParameter(0)));
            ui->yProfileSigmaLE->setText(QString::number(fitFunc->GetParameter(1)));
        }
        else
        {
            gStyle->SetOptFit(0);
            ui->yProfileMeanLE->setText(QString::number(histo->GetMean()));
            ui->yProfileSigmaLE->setText(QString::number(histo->GetRMS()));
        }
        rawAlignmentRightCanvas_->GetCanvas()->cd();
        histo->Draw();
        rawAlignmentRightCanvas_->GetCanvas()->Modified();
        rawAlignmentRightCanvas_->GetCanvas()->Update()  ;
    }
}
//===========================================================================
void mainTabs::swapRawAlignmentFitCBs(bool checked)
{
    if (checked)
    {
        if ( sender() == (QObject*)ui->rawAlignmentFitAllCB )
        {
            ui->rawAlignmentMyFitParXCB->setChecked(false);
            ui->rawAlignmentMyFitParYCB->setChecked(false);
        }
        else ui->rawAlignmentFitAllCB->setChecked(false);
    }
}
//===========================================================================
void mainTabs::on_rawAlignmentFitComparePB_clicked()
{
    this->launchThread3(theHManager_,this,&mainTabs::rawAlignmentFitCompare);
}
//===========================================================================
void mainTabs::rawAlignmentFitCompare()
{
    //bool redo = (ui->showBeamProfilesPB->text() == "Fit")? false:true;
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );
    HManager::stringVDef histoType; //(X[1],Y[2])
    if(ui->rawAlignmentClusterProfilesRB->isChecked())
    {
        theHManager_->setSubProcessFunction(&HManager::makeClusterPlots2);
        histoType = theHManager_->eventsCycle();
    }
    else
    {
        theHManager_->setSubProcessFunction(&HManager::makeBeamSpots2);
        histoType = theHManager_->eventsCycle();
    }
    emit mainTabs::processFinished(&mainTabs::rawAlignmentFitCompare_end,histoType);
}
//===========================================================================
void mainTabs::rawAlignmentFitCompare_end(HManager::stringVDef histoType)
{
    TF1   *fitFunc ;
    TH1   *histo   ;
    TAxis *yAxis = NULL  ;
    TLegend *leg = new TLegend(0.66, 0.5, 0.88,  0.88 );

    // Find highest fit result and set maximum
    double maxX = 0 ;
    double maxY = 0 ;
    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it )
    {
        histo = (TH1*)theHManager_->getHistogram( histoType[1], (*it).first );
        if ( histo->FindObject("gausFitFunc") )
        {
            fitFunc = (TF1*)histo->FindObject("gausFitFunc")->Clone();
            double max = fitFunc->GetParameter(2) ;
            if( max > maxX ) maxX = max ;
        }
        histo = (TH1*)theHManager_->getHistogram( histoType[2], (*it).first );
        if ( histo->FindObject("gausFitFunc") )
        {
            fitFunc = (TF1*)histo->FindObject("gausFitFunc")->Clone();
            double max = fitFunc->GetParameter(2) ;
            ss_.str(""); ss_ << "max: " << max << " " << histo->GetName(); STDLINE(ss_.str(),ACPurple) ;
            if( max > maxY ) maxY = max ;
        }
    }

    maxX *= 1.1 ; // Boost by 10%
    maxY *= 1.1 ;
    ss_.str(""); ss_ << "max: " << maxX << " " << maxY ; STDLINE(ss_.str(),ACWhite) ;

    //xProfile
    double Amp = 0 ;
    bool fisrtDrawn=false;
    int color=1    ;
    rawAlignmentLeftCanvas_->GetCanvas()->cd();

    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it )
    {
        histo = (TH1*)theHManager_->getHistogram( histoType[1], (*it).first );
        if ( histo->FindObject("gausFitFunc") )
        {
            fitFunc = (TF1*)histo->FindObject("gausFitFunc")->Clone();
            leg->AddEntry( fitFunc, (*it).first.c_str() );
            histo->GetXaxis()->UnZoom() ;
            double range = histo->GetXaxis()->GetXmax();
            fitFunc->SetRange(-range, +range);
            fitFunc->SetParameter(0, 0);
            if ( Amp < fitFunc->GetParameter(2) ) Amp = fitFunc->GetParameter(2);
            fitFunc->SetParameter( 3, fitFunc->GetParameter(2)/fitFunc->Integral(-range, +range) );
            fitFunc->SetLineColor(color);
            if (!fisrtDrawn)
            {
                yAxis = fitFunc->GetYaxis();
                fitFunc->Draw();
                fisrtDrawn=true;
            }
            else      fitFunc->Draw("same");
        }
        color++;
    }
    yAxis->SetRangeUser(0, maxX);
    leg->DrawClone();

    //y Profile
    leg->Clear();
    Amp=0;
    fisrtDrawn=false;
    color=1;
    rawAlignmentRightCanvas_->GetCanvas()->cd();
    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it )
    {
        histo = (TH1*)theHManager_->getHistogram( histoType[2], (*it).first );
        if ( histo->FindObject("gausFitFunc") )
        {
            fitFunc = (TF1*)histo->FindObject("gausFitFunc")->Clone();
            leg->AddEntry(fitFunc, (*it).first.c_str());
            histo->GetXaxis()->UnZoom() ;
            double range = histo->GetXaxis()->GetXmax() ;
            fitFunc->SetRange(-range, +range);
            fitFunc->SetParameter(0, 0);
            if ( Amp < fitFunc->GetParameter(2) ) Amp = fitFunc->GetParameter(2);
            fitFunc->SetParameter( 3, fitFunc->GetParameter(2)/fitFunc->Integral(-range, +range) );
            fitFunc->SetLineColor(color);
            if (!fisrtDrawn)
            {
                yAxis = fitFunc->GetYaxis();
                fitFunc->Draw();
                fisrtDrawn=true;
            }
            else
                fitFunc->Draw("same");
        }
        color++;
    }
    yAxis->SetRangeUser(0, maxY);
    leg->DrawClone();

    delete leg;

    rawAlignmentLeftCanvas_->GetCanvas()->Modified() ;
    rawAlignmentLeftCanvas_->GetCanvas()->Update()   ;

    rawAlignmentRightCanvas_->GetCanvas()->Modified() ;
    rawAlignmentRightCanvas_->GetCanvas()->Update()   ;
}
//==============================================================================
void mainTabs::on_writeAlignmentPB_clicked()
{
    this->launchThread3(theHManager_,this,&mainTabs::writeAlignment);
}
//==============================================================================
void mainTabs::writeAlignment()
{
    //bool redo = (ui->showBeamProfilesPB->text() == "Fit")? false:true;
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );
    HManager::stringVDef histoType; //(X[1],Y[2])
    if(ui->rawAlignmentClusterProfilesRB->isChecked())
    {
        theHManager_->setSubProcessFunction(&HManager::makeClusterPlots2);
        histoType = theHManager_->eventsCycle();
    }
    else
    {
        theHManager_->setSubProcessFunction(&HManager::makeBeamSpots2);
        histoType = theHManager_->eventsCycle();
    }
    emit mainTabs::processFinished(&mainTabs::writeAlignment_end,histoType);
}
//==============================================================================
void mainTabs::writeAlignment_end(HManager::stringVDef histoType)
{
    if(!theGeometry_) return;

    TF1 *fitFunc;
    TH1 *histo;

    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it)
    {
        double xPosition=0, xPositionErr=0, yPosition=0, yPositionErr=0;
        //xProfile
        //        STDLINE((*it).first, ACGreen);
        histo = (TH1*)theHManager_->getHistogram( histoType[1], (*it).first );
        if ( histo->FindObject("gausFitFunc") )
        {
            fitFunc = (TF1*)histo->FindObject("gausFitFunc");
            xPosition    = fitFunc->GetParameter(0)    ;
            xPositionErr = fitFunc->GetParError(0)/2. ;
        }

        //y Profile
        histo = (TH1*)theHManager_->getHistogram( histoType[2], (*it).first );
        if ( histo->FindObject("gausFitFunc") )
        {
            fitFunc = (TF1*)histo->FindObject("gausFitFunc");
            yPosition    = fitFunc->GetParameter(0)    ;
            yPositionErr = fitFunc->GetParError(0)/2. ;
        }
        //convert to local flipped coordinate and fill geometry
        if(!ui->rawAlignmentClusterProfilesRB->isChecked())
            theGeometry_->getDetector( it->first )->flipPositionLocal(&xPosition, &yPosition, &xPositionErr, &yPositionErr);

        theGeometry_->getDetector( it->first )->setXPosition     ( xPosition    );
        theGeometry_->getDetector( it->first )->setXPositionError( xPositionErr );
        theGeometry_->getDetector( it->first )->setYPosition     ( yPosition    );
        theGeometry_->getDetector( it->first )->setYPositionError( yPositionErr );
    }

    //Operations added for strip detectors...

    for (int stat = 0; stat < 2; ++stat)
    {
        for (int plaq = 0; plaq < 8; ++plaq)
        {
            if (plaq%2 == 0)
            {
                if( theGeometry_->getDetector(getPlaneID(stat, plaq+1)) != 0 )
                {
                  theGeometry_->getDetector(getPlaneID(stat, plaq))->setYPosition(theGeometry_->getDetector(getPlaneID(stat, plaq+1))->getYPosition());
                  theGeometry_->getDetector(getPlaneID(stat, plaq))->setYPositionError(90000/sqrt(12));

                }
            }
            else
            {
                if( theGeometry_->getDetector(getPlaneID(stat, plaq-1)) != 0 )
                {
                  theGeometry_->getDetector(getPlaneID(stat, plaq))->setXPosition(theGeometry_->getDetector(getPlaneID(stat, plaq-1))->getXPosition());
                  theGeometry_->getDetector(getPlaneID(stat, plaq))->setXPositionError(90000/sqrt(12));

                }
            }
        }
    }

    //LUIGINO: thing added manually for calculating predicted errors on DUT, nothing more, must be cancelled soon

    if(theGeometry_->getDetector(getPlaneID(0, 7)) != 0 && theGeometry_->getDetector(getPlaneID(1, 0)) != 0)
    {
        theGeometry_->getDetector(getPlaneID(2, 0))->setXPosition((theGeometry_->getDetector(getPlaneID(0, 7))->getXPosition()+
                                                                   theGeometry_->getDetector(getPlaneID(1, 0))->getXPosition())/2);
        theGeometry_->getDetector(getPlaneID(2, 0))->setYPosition((theGeometry_->getDetector(getPlaneID(0, 7))->getYPosition()+
                                                                   theGeometry_->getDetector(getPlaneID(1, 0))->getYPosition())/2);
    }
    theFileEater_->updateGeometry("geometry");
    theGeometry_ = theFileEater_->getGeometry();
    showGeometry();
    theGeometry_->dump();
}
//==============================================================================
void mainTabs::swapBeamProfilesHistograms(bool toggled)
{
    if(toggled)
    {
        if( ui->rawAlignmentTabPagePlaqSelectCB->isEnabled() )
        {
            ui->rawAlignmentTabPagePlaqSelectCB->setEnabled(false);
            this->on_showBeamProfilesPB_clicked()                 ;
        }
    }
}

//=============================================================================
//------------------------------------------TRACK FINDER TAB-------------------
//=============================================================================
void mainTabs::on_trackFitNameCB_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "Simple")
        ui->trackFitterIterationsSB->setEnabled(true);
    else
        ui->trackFitterIterationsSB->setEnabled(false);

}

//=============================================================================
void mainTabs::on_trackFindAndFitPB_clicked()
{
    std::string fitMethod  = ui->trackFitNameCB ->currentText().toStdString();
    std::string findMethod = ui->trackFindNameCB->currentText().toStdString();
    theTrackFitter_->setFitMethodName(fitMethod);
    if (fitMethod == "Simple")
        theTrackFitter_->setNumberOfIterations(ui->trackFitterIterationsSB->value());
    else
        theTrackFitter_->setNumberOfIterations(0);
    findAndFitTrack(findMethod, fitMethod);
}

//===================================================================================================
void mainTabs::on_trackFitPB_clicked()
{
    std::string fitMethod = ui->trackFitNameCB->currentText().toStdString();

    theTrackFitter_->setFitMethodName(fitMethod);
    if (fitMethod == "Simple")
        theTrackFitter_->setNumberOfIterations(ui->trackFitterIterationsSB->value());
    else
        theTrackFitter_->setNumberOfIterations(0);

    //cout << __PRETTY_FUNCTION__ << "Fit Method begin:    " << fitMethod << endl;
    findAndFitTrack("", fitMethod);
}

//===================================================================================================
void mainTabs::on_trackFindPB_clicked()
{
    std::string findMethod = ui->trackFindNameCB->currentText().toStdString();

    //cout << __PRETTY_FUNCTION__ << "Search Method begin: " << searchMethod << endl;

    findAndFitTrack(findMethod,"");
}

//=============================================================================
void mainTabs::findAndFitTrack(std::string findMethod, std::string fitMethod)
{
    if(fitMethod == "")//it is just a search
    {
        ss_.str(""); ss_ << "Track finding procedure started"; STDLINE(ss_.str(),ACPurple) ;
        ui->parsingActivityLB->setText(tr("Finding tracks...")) ;
    }
    else if(findMethod == "")//it is just a fit
    {
        ss_.str(""); ss_ << "Track fitting procedure started"; STDLINE(ss_.str(),ACPurple) ;
        ui->parsingActivityLB->setText(tr("Fitting tracks...")) ;
    }
    else
    {
        ss_.str(""); ss_ << "Track finding and fitting procedure started"; STDLINE(ss_.str(),ACPurple) ;
        ui->parsingActivityLB->setText(tr("Finding and fitting tracks...")) ;
    }

    double chi2Cut = -1 ;
    if( ui->trackFinderChi2cutCB->isChecked() ) chi2Cut = ui->trackFinderChi2cutSB->value() ;

    double trackPoints = -1;
    if( ui->trackFinderTrackPointsCB->isChecked() ) trackPoints = ui->trackFinderTrackPointsSB->value();

    double maxPlanePoints = -1;
    if( ui->trackFinderPlanePointsCB->isChecked() ) maxPlanePoints = ui->trackFinderPlanePointsSB->value();

/*
    theFileEater_->setTolerances(ui->xRoadToleranceSB->value()*(1e-4)*CONVF,
                                 ui->yRoadToleranceSB->value()*(1e-4)*CONVF,
                                 trackPoints                               ,
                                 chi2Cut                                   ,
                                 ui->includeDUTfindCB->isChecked()          );
*/
    theTrackFinder_->setTrackSearchParameters(ui->xRoadToleranceSB->value()*(1e-4)*CONVF,
                                              ui->yRoadToleranceSB->value()*(1e-4)*CONVF,
                                              chi2Cut                                   ,
                                              trackPoints                               ,
                                              maxPlanePoints                            );

    theTrackFinder_->setTrackingOperationParameters(findMethod,fitMethod,ui->includeDUTfindCB->isChecked());
    theFileEater_  ->setOperation(&fileEater::updateEvents2,theTrackFinder_);
    theTrackFinder_->setOperation(&trackFinder::findAndFitTracks);
    STDLINE("Launching theFileEater_...",ACPurple) ;
    this->launchThread2(theFileEater_);
}

//=============================================================================
void mainTabs::launchThread2(process * theProcess)
{
    process * activeProcess = NULL ;
    activeProcess = theThreader_->getCurrentProcess();
    if(theThreader_->isRunning())
    {
        ss_.str("");
        ss_ << "WARNING: action temporarily disabled, " << activeProcess->getLabel() << " is running";
        STDLINE(ss_.str(),ACRed);
        return ;
    }

    ui->abortActionPB->setEnabled(true) ;

    ui->parseProgressBar->reset();
    ui->parseProgressBar->setMaximum(theProcess->getMaxIterations());

    theThreader_->setProcess(theProcess);
    //theThreader_->runTheMethod(theProcess,&process::execute());
    theBenchmark_     = new TBenchmark() ;
    theThreadProcess_ = theProcess->getLabel() ;
    theBenchmark_->Start(theThreadProcess_.c_str());
    theThreader_->start();

    ss_.str("");
    ss_ << theProcess->getLabel() << "...";
    ui->parsingActivityLB->setText(QString::fromStdString(ss_.str()));
    STDLINE(ss_.str(),ACGreen);

    timer2_->start(50) ;
}
//=============================================================================
template<class Class> void mainTabs::launchThread3(process * theProcess, Class * object, void (Class::*fn)())
{
    if(theThreader_->isRunning())
    {
        process * activeProcess = theThreader_->getCurrentProcess();
        ss_.str("");
        ss_ << "WARNING: action disabled, " << activeProcess->getLabel() << " is running";
        STDLINE(ss_.str(),ACRed);
        return ;
    }

    ui->abortActionPB->setEnabled(true) ;

    ui->parseProgressBar->reset();
    ui->parseProgressBar->setMaximum(theProcess->getMaxIterations());

    theThreader_->setProcess(theProcess  );
    theBenchmark_     = new TBenchmark() ;
    theThreadProcess_ = theProcess->getLabel() ;
    theBenchmark_->Start(theThreadProcess_.c_str());
    theThreader_->setFuture(QtConcurrent::run(object,fn));

    ss_.str("");
    ss_ << theProcess->getLabel() << " process is running...";
    ui->parsingActivityLB->setText(QString::fromStdString(ss_.str()));
    STDLINE(ss_.str(),ACGreen);

    timer2_->start(50) ;
}
//=============================================================================
void mainTabs::on_showChi2Distributions_clicked()
{
    gStyle->SetOptStat(1111) ;
    this->launchThread3(theHManager_,this,&mainTabs::showChi2Distributions);
}
//=============================================================================
void mainTabs::showChi2Distributions()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    theHManager_->setSubProcessFunction(&HManager::makeTracksDistr2, redoChi2_);
    HManager::stringVDef histoPaths = theHManager_->eventsCycle();
    emit mainTabs::processFinished(&mainTabs::showChi2Distributions_end, histoPaths);
}
//=============================================================================
void mainTabs::showChi2Distributions_end(HManager::stringVDef histoPaths)
{
    redoChi2_ = false;
    trackFinderLeftCanvas_->GetCanvas()->cd();
    theHManager_->getHistogram(histoPaths[0])->Draw();
    trackFinderLeftCanvas_->GetCanvas()->Modified();
    trackFinderLeftCanvas_->GetCanvas()->Update();
}
//=============================================================================
void mainTabs::on_showTrackErrorsOnDut_clicked()
{
    this->launchThread3(theHManager_,this,&mainTabs::showTrackErrorsOnDut);
}
//=============================================================================
void mainTabs::showTrackErrorsOnDut()
{
    STDLINE("Computing predicted error distibution on DUT(s)",ACCyan);

    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    theHManager_->setSubProcessFunction(&HManager::makeTrackErrorsOnDUTs2,true);
    HManager::stringVDef histoPaths = theHManager_->eventsCycle();
    emit mainTabs::processFinished(&mainTabs::showTrackErrorsOnDut_end, histoPaths);
}
//=============================================================================
void mainTabs::showTrackErrorsOnDut_end(HManager::stringVDef xyHList)
{
    std::vector<Detector*> DUTs = theGeometry_->getDUTs();
    int hNum = DUTs.size() ;

    trackFinderRightCanvas_->GetCanvas()->Divide(2,hNum,0.001,0.001) ;

    int padNum = 1 ;
    for(std::vector<Detector*>::iterator it=DUTs.begin(); it!=DUTs.end(); ++it)
    {
        trackFinderRightCanvas_->GetCanvas()->cd(padNum);
        theHManager_->getHistogram(xyHList[0],(*it)->getID())->Draw();
        padNum += 2 ;
    }
    padNum = 2 ;
    for(std::vector<Detector*>::iterator it=DUTs.begin(); it!=DUTs.end(); ++it)
    {
        trackFinderRightCanvas_->GetCanvas()->cd(padNum);
        theHManager_->getHistogram(xyHList[1],(*it)->getID())->Draw();
        padNum += 2 ;
    }

   trackFinderRightCanvas_->GetCanvas()->Modified();
   trackFinderRightCanvas_->GetCanvas()->Update();
}
//=============================================================================
void mainTabs::on_showSlopeDistributions_clicked()
{
    this->launchThread3(theHManager_,this,&mainTabs::showSlopeDistributions);
}
//=============================================================================
void mainTabs::showSlopeDistributions()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    theHManager_->setSubProcessFunction(&HManager::makeTracksDistr2, redoChi2_);
    HManager::stringVDef xyStringVec = theHManager_->eventsCycle();
    emit mainTabs::processFinished(&mainTabs::showSlopeDistributions_end, xyStringVec);
}
//=============================================================================
void mainTabs::showSlopeDistributions_end(HManager::stringVDef xyStringVec)
{
    gStyle->SetOptStat(1111);
    gStyle->SetOptFit(11);
    redoChi2_ =  false;

    TH1 * histo;
    trackFinderLeftCanvas_->GetCanvas()->cd();
    histo = (TH1*)theHManager_->getHistogram(xyStringVec[1]);

    if( !ui->fullRangeCB->isChecked() )
    {
        histo->GetXaxis()->SetRangeUser(ui->xLowerSlopeLimitSB->value(),
                                        ui->xHigherSlopeLimitSB->value()) ;
    }
    else
    {
        histo->GetXaxis()->SetRangeUser(histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax());
    }

    histo->Draw();

    trackFinderLeftCanvas_->GetCanvas()->Modified();
    trackFinderLeftCanvas_->GetCanvas()->Update();

    trackFinderRightCanvas_->GetCanvas()->cd();
    histo = (TH1*)theHManager_->getHistogram(xyStringVec[2]);

    if( !ui->fullRangeCB->isChecked() )
    {
        histo->GetXaxis()->SetRangeUser(ui->yLowerSlopeLimitSB->value(),
                                        ui->yHigherSlopeLimitSB->value()) ;
    }
    else
    {
        histo->GetXaxis()->SetRangeUser(histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax());
    }

    histo->Draw();

    trackFinderRightCanvas_->GetCanvas()->Modified();
    trackFinderRightCanvas_->GetCanvas()->Update();

    ui->trackFinderFitSlopePB->setEnabled(true);
    ui->applySlopeLimitsPB->setEnabled(true)   ;
}
//=============================================================================
void mainTabs::on_applySlopeLimitsPB_clicked()
{
    this->on_showSlopeDistributions_clicked();
}
//=============================================================================
void mainTabs::setCBslopeLimits(TObject *obj,unsigned int event,TCanvas *)
{
    QWidget *tipped = (QWidget *)sender();          
    ss_.str("");
    ss_ << obj->ClassName();
    std::string className = ss_.str();
    ss_.str("");
    ss_ << obj->GetName();
    std::string objName = ss_.str();

    std::stringstream tipText;
    tipText << "You have "   ;

    if (event == kButton1Up)
    {
        tipText << "MouseRelease";
        if ( className == "TAxis" && objName == "xaxis")
        {
            TAxis *axis = (TAxis*)obj;

            if ( tipped == ui->trackFinderLeftCanvas )
            {
                ui->xLowerSlopeLimitSB ->setValue( axis->GetBinLowEdge( axis->GetFirst() ) );
                ui->xHigherSlopeLimitSB->setValue( axis->GetBinUpEdge ( axis->GetLast()  ) );
            }
            if ( tipped == ui->trackFinderRightCanvas )
            {
                ui->yLowerSlopeLimitSB ->setValue( axis->GetBinLowEdge( axis->GetFirst() ) );
                ui->yHigherSlopeLimitSB->setValue( axis->GetBinUpEdge ( axis->GetLast()  ) );
            }
        }
    }
}
//=============================================================================
void mainTabs::on_trackFinderSlopeAlignPB_clicked()
{
    if(!theGeometry_) return;
    ui->trackFinderSlopeAlignPB->setEnabled(false);

    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it)
    {
        double xPosition=0, yPosition=0;
        //xProfile
        xPosition    = sin( atan( ui->trackFinderXslopeSB->value() ) )*theGeometry_->getDetector( it->first )->getZPositionTotal();
        //y Profile
        yPosition    = sin( atan( ui->trackFinderYslopeSB->value() ) )*theGeometry_->getDetector( it->first )->getZPositionTotal();

        if( ui->alignSlopeXCB->isChecked() )
            theGeometry_->getDetector( it->first )->setXPositionCorrection( theGeometry_->getDetector( it->first )->getXPositionCorrection()+xPosition );
        if( ui->alignSlopeYCB->isChecked() )
            theGeometry_->getDetector( it->first )->setYPositionCorrection( theGeometry_->getDetector( it->first )->getYPositionCorrection()+yPosition );
    }
    theFileEater_->updateGeometry("geometry");
    theGeometry_ = theFileEater_->getGeometry();
    showGeometry();
    theGeometry_->dump();
}
//==============================================================================
void mainTabs::on_trackFinderFitSlopePB_clicked()
{
    this->launchThread3(theHManager_,this,&mainTabs::trackFinderFitSlope);
}

//==============================================================================
void mainTabs::on_trackFinderFitChi2PB_clicked()
{

}
//==============================================================================
void mainTabs::trackFinderFitSlope()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );
    theHManager_->setSubProcessFunction(&HManager::makeTracksDistr2, redoChi2_);
    HManager::stringVDef xyStringVec = theHManager_->eventsCycle();
    emit mainTabs::processFinished(&mainTabs::trackFinderFitSlope_end, xyStringVec);
}
//==============================================================================
void mainTabs::trackFinderFitSlope_end(HManager::stringVDef xyStringVec)
{
    redoChi2_ =  false;
    if( ui->trackFinderAutoFitCB->isChecked() )
    {
        theFitter_->gaussFit( (TH1*)theHManager_->getHistogram(xyStringVec[1]) );
        theFitter_->gaussFit( (TH1*)theHManager_->getHistogram(xyStringVec[2]) );
    }
    else
    {
        if (ui->trackFinderMyFitParXCB->isChecked())
            theFitter_->gaussFit( (TH1*)theHManager_->getHistogram(xyStringVec[1])            ,
                    ui->trackFinderXslopeSB->value()                                ,
                    Utils::toDouble( ui->xSlopeSigmaLE->text().toStdString() )      ,
                    ui->xSlopeNsigmaSB->value()                                      );
        if (ui->trackFinderMyFitParYCB->isChecked())
            theFitter_->gaussFit( (TH1*)theHManager_->getHistogram(xyStringVec[2])           ,
                    ui->trackFinderYslopeSB->value()                                ,
                    Utils::toDouble( ui->ySlopeSigmaLE->text().toStdString() )      ,
                    ui->ySlopeNsigmaSB->value()                                      );
    }

    TF1 *fitFunc;
    TH1 *histo;
    //xProfile
    histo = (TH1*)theHManager_->getHistogram(xyStringVec[1]);
    if ( histo->FindObject("gausFitFunc") )
    {
        fitFunc = (TF1*)histo->FindObject("gausFitFunc");
        ui->trackFinderXslopeSB->setValue(fitFunc->GetParameter(0));
        ss_.str("");
        ss_ << fitFunc->GetParameter(1);
        ui->xSlopeSigmaLE->setText(QString::fromStdString(ss_.str()));
    }
    else
    {
        ui->trackFinderXslopeSB->setValue( histo->GetMean() );
        ss_.str("");
        ss_ << histo->GetRMS();
        ui->xSlopeSigmaLE->setText(QString::fromStdString(ss_.str()));
    }


    //y Profile
    histo = (TH1*)theHManager_->getHistogram(xyStringVec[2]);
    if ( histo->FindObject("gausFitFunc") )
    {
        fitFunc = (TF1*)histo->FindObject("gausFitFunc");
        ui->trackFinderYslopeSB->setValue(fitFunc->GetParameter(0));
        ss_.str("");
        ss_ << fitFunc->GetParameter(1);
        ui->ySlopeSigmaLE->setText(QString::fromStdString(ss_.str()));
    }
    else
    {
        ui->trackFinderYslopeSB->setValue( histo->GetMean() );
        ss_.str("");
        ss_ << histo->GetRMS();
        ui->ySlopeSigmaLE->setText(QString::fromStdString(ss_.str()));
    }

    theHManager_->getHistogram(xyStringVec[1])->Draw();
    theHManager_->getHistogram(xyStringVec[2])->Draw();

    trackFinderLeftCanvas_ ->GetCanvas()->Modified() ;
    trackFinderLeftCanvas_ ->GetCanvas()->Update()   ;
    trackFinderRightCanvas_->GetCanvas()->Modified();
    trackFinderRightCanvas_->GetCanvas()->Update()  ;

    ui->trackFinderSlopeAlignPB->setEnabled(true);
}
//==============================================================================
void mainTabs::swapSlopeFitCBs (bool checked )
{
    if (checked)
    {
        if ( sender() == (QObject*)ui->trackFinderAutoFitCB )
        {
            ui->trackFinderMyFitParXCB->setChecked(false);
            ui->trackFinderMyFitParYCB->setChecked(false);
        }
        else ui->trackFinderAutoFitCB->setChecked(false);
    }
}

//==============================================================================
//------------------------------------------RESIDUALS TAB---------------------------------------------
//==============================================================================
void mainTabs::showResiduals()
{
    std::string type;
    if     ( sender() == ui->showScatterResidualsPB ){ type = "showScatterResiduals"; theHManager_->clearScatterResidual();}
    else if( sender() == ui->showResidualsPB        ){ type = "showResiduals"       ; theHManager_->clearResiduals() ;}
    else if( sender() == ui->showDeviationsPB       ){ type = "showDeviations"      ; theHManager_->clearDeviations();}
    else if( sender() == ui->showPullsPB            ){ type = "showPulls"           ; theHManager_->clearPulls()     ;}
    else return;

    ui->eventDisplayTrackFitterEventsSelected->clear();
    selectedEvents_.clear();

    double chi2filter         = -1;
    int maxTracksFilter       = -1;
    int maxPlanePointsFilter  = -1;
    int minTrackHitsFilter    = -1;
    int maxClusterSizeFilter  = -1;
    int onlyClusterSizeFilter = -1;

    theHManager_->clearAllFilters();
    for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        if( ui->trackFitterExcludedDetectorsListW->findItems(QString::fromStdString(git->first),0)[0]->isSelected() )
            theHManager_->addSelectedDetector(git->first);

    if( ui->residualsChi2CB            ->isChecked()) chi2filter            = ui->residualsChi2SB            ->value();
    if( ui->residualsMaxTrackPerEventCB->isChecked()) maxTracksFilter       = ui->residualsMaxTrackPerEventSB->value();
    if( ui->residualsMaxPlanePointsCB  ->isChecked()) maxPlanePointsFilter  = ui->residualsMaxPlanePointsSB  ->value();
    if( ui->residualsMinPointsCB       ->isChecked()) minTrackHitsFilter    = ui->residualsMinPointsSB       ->value();
    if( ui->residualsMaxClusterSizeCB  ->isChecked()) maxClusterSizeFilter  = ui->residualsMaxClusterSizeSB  ->value();
    if( ui->residualsOnlyClusterSizeCB ->isChecked()) onlyClusterSizeFilter = ui->residualsOnlyClusterSizeSB ->value();

    theHManager_->resetResidualCounters();
    theHManager_->setResidualFilters   (
                                        type,
                                        chi2filter,
				        maxTracksFilter,
				        maxPlanePointsFilter,
				        minTrackHitsFilter,
				        maxClusterSizeFilter,
				        onlyClusterSizeFilter
				       );
    theHManager_->setOperation        (
                                       &HManager::eventsCycle,
				       &HManager::makeResidualDistributions
				      );
    theHManager_->setRunSubDir        ( 
                                       theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) 
				      );
    this->launchThread2(theHManager_);
}

//==============================================================================
void mainTabs::showResiduals2()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    int processedEvents = theFileEater_->getEventsNumber();

    HManager::stringVDef histoType;
    if( sender() == ui->showScatterResidualsPB )                   histoType = theHManager_->clearScatterResidual();
    if( sender() == ui->showResidualsPB        )  residualsType_ = histoType = theHManager_->clearResiduals() ;
    if( sender() == ui->showDeviationsPB       )  residualsType_ = histoType = theHManager_->clearDeviations();
    if( sender() == ui->showPullsPB            )                   histoType = theHManager_->clearPulls()     ;


    int totalTracksFound           = 0;
    int totalEventsWithTracksFound = 0;

    ui->eventDisplayTrackFitterEventsSelected->clear();
    selectedEvents_.clear();

    for( int ev = 0; ev < processedEvents; ev++ )
    {
        if( ev%1000 == 0 )
        {
            ss_.str(""); ss_ << "Processing event " << ev;
            STDSNAP(ss_.str(),ACPurple) ;
        }
        Event * theEvent = theFileEater_->getEvent(ev);
        //int pass=0;

        //Max tracks per event filter
        if( ui->residualsMaxTrackPerEventCB->isChecked()
                && theEvent->getTrackCandidates().size() > (unsigned int)ui->residualsMaxTrackPerEventSB->value())
            continue;

        //Max clusters on a plane
        bool nextEvent = false;
        if( ui->residualsMaxPlanePointsCB->isChecked() )
            for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
            {
                if ( (int)theEvent->getClusters()[git->first].size() >  ui->residualsMaxPlanePointsSB->value())
                {
                    nextEvent = true;
                    break;
                }
            }
        if(nextEvent) continue;

        //Track quality filters
        if ( ui->residualsChi2CB           ->isChecked() ||
             ui->residualsMinPointsCB      ->isChecked() ||
             ui->residualsMaxClusterSizeCB ->isChecked() ||
             ui->residualsOnlyClusterSizeCB->isChecked()   )
        {
            bool goodEvent = false;
            for(unsigned int tr=0; tr < theEvent->getTrackCandidates().size(); tr++)
            {
                //chi2/DOF filter
                if( ui->residualsChi2CB->isChecked() && theEvent->getFittedTracksChi2()[tr] > ui->residualsChi2SB->value()   ) continue;

                /*
                if( ui->trackFitterSlopeCutsCB->isChecked()                                   &&
                    ( theEvent->getFittedTracks()[tr][0] < ui->xLowerSlopeLimitSB->value()  ||
                      theEvent->getFittedTracks()[tr][0] > ui->xHigherSlopeLimitSB->value() ||
                      theEvent->getFittedTracks()[tr][2] < ui->yLowerSlopeLimitSB->value()  ||
                      theEvent->getFittedTracks()[tr][2] > ui->yHigherSlopeLimitSB->value()   ) ) continue;
*/
                //Checking for minimum hits requirement and max cluster size
                if( ui->residualsMinPointsCB->isChecked() || ui->residualsMaxClusterSizeCB->isChecked() )
                {
                    int pass    = 0;
                    int purePass= 0;
                    for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
                    {
                        if( !ui->trackFitterExcludedDetectorsListW->findItems(QString::fromStdString((*git).first),0)[0]->isSelected() )
                        {
                            if( theEvent->getTrackCandidates()[tr].count( (*git).first ) != 0 )
                            {
                                ++pass;
                                if( theEvent->getTrackCandidates()[tr][(*git).first]["size"] <= ui->residualsMaxClusterSizeSB->value())
                                    ++purePass;

                            }
                        }
                    }
                    if( ui->residualsMinPointsCB     ->isChecked() && pass < ui->residualsMinPointsSB->value()) continue;
                    //If I am here I already passed the min points selection, if active.
                    if( ui->residualsMaxClusterSizeCB->isChecked() && purePass != pass ) continue;
                }
                int  clusterSizeSel = -1   ;
                if( ui->residualsOnlyClusterSizeCB->isChecked() )
                    clusterSizeSel = ui->residualsOnlyClusterSizeSB->value();
                if( sender() == ui->showScatterResidualsPB )  theHManager_->fillScatterResidual( theEvent, (int)tr, clusterSizeSel);
                if( sender() == ui->showResidualsPB        )  theHManager_->fillResiduals      ( theEvent, (int)tr, clusterSizeSel);
                if( sender() == ui->showDeviationsPB       )  theHManager_->fillDeviations     ( theEvent, (int)tr, clusterSizeSel);
                if( sender() == ui->showPullsPB            )  theHManager_->fillPulls          ( theEvent, (int)tr, clusterSizeSel);
                ++totalTracksFound;
                goodEvent = true;
            }
            if(goodEvent)
            {
                ++totalEventsWithTracksFound;
                ss_.str("");
                ss_ << ev;
                ui->eventDisplayTrackFitterEventsSelected->addItem( QString::fromStdString(ss_.str()) );
                selectedEvents_.push_back(ev);
            }
        }
        else
        {
            if( sender() == ui->showScatterResidualsPB ) theHManager_->fillScatterResidual( theEvent );
            if( sender() == ui->showResidualsPB        ) theHManager_->fillResiduals      ( theEvent );
            if( sender() == ui->showDeviationsPB       ) theHManager_->fillDeviations     ( theEvent );
            if( sender() == ui->showPullsPB            ) theHManager_->fillPulls          ( theEvent );

            int nTracks = theEvent->getTrackCandidates().size();
            totalTracksFound += nTracks;
            if (nTracks > 0 )
            {
                totalEventsWithTracksFound++;
                ss_.str("");
                ss_ << ev;
                ui->eventDisplayTrackFitterEventsSelected->addItem( QString::fromStdString(ss_.str()) );
                selectedEvents_.push_back(ev);
            }
            //theHManager_->fillResiduals( theEvent->makeFittedTrackDeviations(), theEvent );
        }
    }
    std::cout << std::endl;//After STDSNAP

    ui->passingEventsLE->setText(QString("%1/%2").arg(totalEventsWithTracksFound).arg(processedEvents));
    double tracksPerEvent = (1.*totalTracksFound) / (1.*totalEventsWithTracksFound);
    ui->tracksPerEventFoundLE->setText(QString("%1").arg(tracksPerEvent));

    ss_.str("");
    ss_ << "Events passing selection: " << totalEventsWithTracksFound << "/" << processedEvents;
    STDLINE(ss_.str(),ACCyan);
    ss_.str("");
    ss_ << "Total tracks found: " << totalTracksFound << " with " << tracksPerEvent << " tracks per event.";
    STDLINE(ss_.str(),ACCyan);

    double lowRange = 0 ;
    double higRange = 0 ;


    if( sender() == ui->showResidualsPB || sender() == ui->showDeviationsPB )
    {
        lowRange = ui->residualRangeLowCB->currentText().toDouble() ;
        higRange = ui->residualRangeHigCB->currentText().toDouble() ;
        this->drawAll(residualsSynopticViewLeftCanvas_ , histoType[0], "", "", lowRange, higRange);
        this->drawAll(residualsSynopticViewRightCanvas_, histoType[1], "", "", lowRange, higRange);
    }
    if( sender() == ui->showPullsPB)
    {
        this->drawAll(residualsPullsLeftCanvas_ , histoType[0], "", "", lowRange, higRange);
        this->drawAll(residualsPullsRightCanvas_, histoType[1], "", "", lowRange, higRange);
    }
    if( sender() == ui->showScatterResidualsPB )
    {

        this->drawAll(residuals2DResidualsVsCoordinateLeftCanvas_ , histoType[0], "", "", lowRange, higRange);
        this->drawAll(residuals2DResidualsVsCoordinateRightCanvas_, histoType[1], "", "", lowRange, higRange);
        histoType = theHManager_->makeMeanScatterResidual(histoType, true);
        if     ( ui->xResidualZRotationRB->isChecked() ||
                 ui->yResidualZRotationRB->isChecked()    )
        {
            this->drawAll(residualsResidualsVsCoordinateLeftCanvas_ , histoType[0], "", "COLZ");
            this->drawAll(residualsResidualsVsCoordinateRightCanvas_, histoType[1], "", "COLZ");
        }
        else if( ui->xResidualYRotationRB->isChecked() ||
                 ui->yResidualXRotationRB->isChecked()    )
        {
            this->drawAll(residualsResidualsVsCoordinateLeftCanvas_ , histoType[2], "", "COLZ");
            this->drawAll(residualsResidualsVsCoordinateRightCanvas_, histoType[3], "", "COLZ");
        }
    }

    ui->residualsTabPagePlaqSelectCB->setEnabled(true);
    ui->trackFitterFitPB              ->setEnabled(true);
    ui->trackFitterWriteAlignmentPB   ->setEnabled(true);

    if( sender() == ui->showResidualsPB || sender() == ui->showDeviationsPB )
    {
        this->on_residualsTabPagePlaqSelectCB_currentIndexChanged(ui->residualsTabPagePlaqSelectCB->currentText());
    }
}
//=============================================================================
/*
void mainTabs::showResiduals2()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    selectedEvents_.clear();
    ui->eventDisplayTrackFitterEventsSelected->clear();

    //found tracks filter
    int maxTracks = -1;
    if ( ui->residualsMaxTrackPerEventCB->isChecked()      ) maxTracks = ui->residualsMaxTrackPerEventSB->value()        ;
    int minPlanes = -1;
    if ( ui->residualsMinPointsCB->isChecked() ) minPlanes = ui->residualsMinPointsSB->value()     ;
    int maxPoints = -1;
    if ( ui->residualsMaxPlanePointsCB->isChecked() ) maxPoints = ui->residualsMaxPlanePointsSB->value();

    theHManager_->setEventFilters(maxTracks,minPlanes,maxPoints);

    double maxChi2   = -1;
    if ( ui->residualsChi2CB->isChecked() ) maxChi2   = ui->residualsChi2SB->value()   ;
    int minPoints = -1;
    if ( ui->residualsMinPointsCB->isChecked()  ) minPoints = ui->residualsMinPointsSB->value()      ;

    theHManager_->setTrackFilters(maxChi2, minPoints, ui->trackFitterSlopeCutsCB->isChecked(),
                                  ui->xLowerSlopeLimitSB->value()        ,
                                  ui->xHigherSlopeLimitSB->value()       ,
                                  ui->yLowerSlopeLimitSB->value()        ,
                                  ui->yHigherSlopeLimitSB->value()         );

    std::set<int> clusterSizes;
    minPoints = -1;
    if( ui->trackFitterClusterSizeSelection->isChecked() )
    {
        clusterSizes.insert(ui->trackFitterClusterSizeSelectionSB->value());
        minPoints = ui->trackFitterMinDetectorsWithClusterSizeSB->value()  ;
    }
    else clusterSizes.clear();

    theHManager_->setClusterFilters(clusterSizes,minPoints);


    //if( sender() == ui->showScatterResidualsPB   ) theHManager_->setSubProcessFunction(&HManager::fillScatterResidual);
    //if( sender() == ui->showResidualsPB          ) theHManager_->setSubProcessFunction(&HManager::fillResiduals      );
    //if( sender() == ui->showDeviationsPB         ) theHManager_->setSubProcessFunction(&HManager::fillDeviations     );
    //if( sender() == ui->showPullsPB              ) theHManager_->setSubProcessFunction(&HManager::fillPulls          );

    residualsType_ = theHManager_->eventsFilter();
    emit mainTabs::processFinished(&mainTabs::showResiduals2_end,residualsType_);
}
*/
//=======================================================================================================================
/*
void mainTabs::showResiduals2_end(HManager::stringVDef histoType)
{
    double lowRange = 0 ;
    double higRange = 0 ;

    //lowRange = ui->residualRangeLowCB->currentText().toDouble() ;
    //higRange = ui->residualRangeHigCB->currentText().toDouble() ;

    STDLINE("Drawing residuals",ACRed);
    this->drawAll(ui->residualsSynopticViewLeftCanvas , histoType[0], "", "", lowRange, higRange);
    this->drawAll(ui->residualsSynopticViewRightCanvas, histoType[1], "", "", lowRange, higRange);


//    histoType = theHManager_->makeMeanScatterResidual(histoType, true);
//    if     ( ui->xResidualZRotationRB->isChecked() ||
//             ui->yResidualZRotationRB->isChecked()    )
//    {
//         this->drawAll(ui->trackFitterScatterResidualsLeftCanvas , histoType[0], "", "COLZ");
//         this->drawAll(ui->trackFitterScatterResidualsRightCanvas, histoType[1], "", "COLZ");
//    }
//    else if( ui->xResidualYRotationRB->isChecked() ||
//             ui->yResidualXRotationRB->isChecked()    )
//    {
//         this->drawAll(ui->trackFitterScatterResidualsLeftCanvas , histoType[2], "", "COLZ");
//         this->drawAll(ui->trackFitterScatterResidualsRightCanvas, histoType[3], "", "COLZ");
//    }

//    ui->residualsTabPagePlaqSelectCB->setEnabled(true);
//    ui->trackFitterFitPB              ->setEnabled(true);
//    ui->trackFitterWriteAlignmentPB   ->setEnabled(true);

//    this->on_residualsTabPagePlaqSelectCB_currentIndexChanged(ui->residualsTabPagePlaqSelectCB->currentText());
}
*/

//=============================================================================
void mainTabs::on_trackFitterFitPB_clicked()
{
    gStyle->SetOptFit (1111);
    gStyle->SetOptStat(11);
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    double lowRange = 0 ;
    double higRange = 0 ;

    if( !ui->residualFullRangeCB->isChecked() )
    {
        lowRange = ui->residualRangeLowCB->currentText().toDouble() ;
        higRange = ui->residualRangeHigCB->currentText().toDouble() ;
    }

    if( ui->residualsFitAllCB->isChecked() )
    {
        for( Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it )
        {
            if( ui->detectorsToBeAlignedLW->findItems(QString::fromStdString((*it).first),0)[0]->isSelected() ) continue;
            theFitter_->gaussFit( (TH1*) theHManager_->getHistogram(residualsType_[0] , (*it).first ) );
            theFitter_->gaussFit( (TH1*) theHManager_->getHistogram(residualsType_[1] , (*it).first ) );
            //            this->drawAll(ui->residualsSynopticViewLeftCanvas , residualsType_.first, "", lowRange, higRange);
            //            this->drawAll(ui->residualsSynopticViewRightCanvas, residualsType_.second, "", lowRange, higRange);
        }
    }
    else
    {
        if( ui->trackFitterMyFitParXCB->isChecked() )
            theFitter_->gaussFit( (TH1*) theHManager_->getHistogram(residualsType_[0], plaqSelected_),
                    Utils::toDouble( ui->xResidualMeanLE ->text().toStdString() ) ,
                    Utils::toDouble( ui->xResidualSigmaLE->text().toStdString() ) ,
                    ui->xResidualNsigmaSB->value()                                  );
        //        this->drawAll(ui->residualsSynopticViewLeftCanvas , residualsType_.first, "", lowRange, higRange);

        if( ui->trackFitterMyFitParYCB->isChecked() )
            theFitter_->gaussFit( (TH1*) theHManager_->getHistogram(residualsType_[1], plaqSelected_),
                    Utils::toDouble( ui->yResidualMeanLE ->text().toStdString() ) ,
                    Utils::toDouble( ui->yResidualSigmaLE->text().toStdString() ) ,
                    ui->yResidualNsigmaSB->value()                                  );
        //        this->drawAll(ui->residualsSynopticViewRightCanvas, residualsType_.second, "", lowRange, higRange);
    }

    this->drawAll(residualsSynopticViewLeftCanvas_ , residualsType_[0], "", "", lowRange, higRange);
    this->drawAll(residualsSynopticViewRightCanvas_, residualsType_[1], "", "", lowRange, higRange);

    //    this->drawAll(ui->residualsSynopticViewLeftCanvas , "Xresiduals", "", "", lowRange, higRange);
    //    this->drawAll(ui->residualsSynopticViewRightCanvas, "Yresiduals", "", "", lowRange, higRange);

    this->on_residualsTabPagePlaqSelectCB_currentIndexChanged(QString::fromStdString(plaqSelected_));
}
//=============================================================================
void mainTabs::on_trackFitterWriteAlignmentPB_clicked()
{
    if(!theGeometry_) return;
    ui->trackFitterWriteAlignmentPB->setEnabled(false);

    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    TF1 *fitFunc;
    TH1 *histo;

    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it)
    {
        if( ui->detectorsToBeAlignedLW->findItems(QString::fromStdString((*it).first),0)[0]->isSelected() ) continue;

        double xPosition=0, xPositionErr=0, yPosition=0, yPositionErr=0;

        //xProfile
        histo = (TH1*)theHManager_->getHistogram(residualsType_[0] , (*it).first );
        if ( ui->alignMethodsCB->currentIndex() == 0 )
        {
            if ( histo->FindObject("gausFitFunc") )
            {
                fitFunc = (TF1*)histo->FindObject("gausFitFunc");
                xPosition    = fitFunc->GetParameter(0)    ;
                xPositionErr = fitFunc->GetParameter(1)    ;
            }
        }
        else if ( ui->alignMethodsCB->currentIndex() == 1 )
        {
            xPosition    = histo->GetMean()  ;
            xPositionErr = histo->GetRMS()/2.;
        }


        //y Profile
        histo = (TH1*)theHManager_->getHistogram(residualsType_[1] , (*it).first );
        if ( ui->alignMethodsCB->currentIndex() == 0 )
        {
            if ( histo->FindObject("gausFitFunc") )
            {
                fitFunc = (TF1*)histo->FindObject("gausFitFunc");
                yPosition    = fitFunc->GetParameter(0)    ;
                yPositionErr = fitFunc->GetParameter(1)    ;
            }
        }
        else if ( ui->alignMethodsCB->currentIndex() == 1 )
        {
            yPosition    = histo->GetMean()  ;
            yPositionErr = histo->GetRMS()/2.;
        }

        Detector* det = theGeometry_->getDetector( it->first );
        det->setXPositionCorrection( det->getXPositionCorrection()+xPosition );
        det->setXPositionError     ( xPositionErr );
        det->setYPositionCorrection( det->getYPositionCorrection()+yPosition );
        det->setYPositionError     ( yPositionErr );
    }
    theFileEater_->updateGeometry("geometry");
    theGeometry_ = theFileEater_->getGeometry();
    showGeometry();
    theGeometry_->dump();

}
//==============================================================================
void mainTabs::on_trackFitterExcludedDetectorsListW_itemSelectionChanged()
{
    if ( ui->trackFitterExcludedDetectorsListW->count() - ui->trackFitterExcludedDetectorsListW->selectedItems().count() < 4 && ui->residualsMinPointsCB->isChecked() )
        ui->trackFitterExcludedDetectorsListW->setSelectionMode(QAbstractItemView::SingleSelection );
    else
        ui->trackFitterExcludedDetectorsListW->setSelectionMode(QAbstractItemView::MultiSelection  );

    if ( ui->residualsMinPointsCB->isChecked() )
        ui->residualsMinPointsSB->setMaximum( ui->trackFitterExcludedDetectorsListW->count() -
                                              ui->trackFitterExcludedDetectorsListW->selectedItems().count() );
}

//=============================================================================
void mainTabs::signalNewAction(std::string newAction)
{
    STDLINE("",ACWhite) ;
    ss_.str("") ;
    ss_ << ACYellow << ACBold
        << "=========================== "
        << ACCyan   << ACBold
        << "New action: "
        << ACPurple << ACBold
        << newAction
        << ACYellow << ACBold
        << " =========================== " ;
    STDLINE(ss_.str(),ACWhite) ;
}

//=============================================================================
void mainTabs::on_unconstrainedResidualsPB_clicked()
{
    //cout << __PRETTY_FUNCTION__ << "Button Clicked" << endl;
    this->ui->residualsMonitorTW->setCurrentIndex(0) ;

    if(ui->onlyDetectorsToBeAlignedCB->isChecked())
    {
        std::set<std::string> selectedDetectorsList;
        for(Geometry::iterator git=theGeometry_->begin(); git!=theGeometry_->end(); git++)
        {
            if( !ui->detectorsToBeAlignedLW->findItems(QString::fromStdString((*git).first),0)[0]->isSelected() )
            {
                selectedDetectorsList.insert( (*git).first );
            }
        }
        theTrackFitter_->setSelectedDetectorsList(selectedDetectorsList);
    }
    else theTrackFitter_->clearSelectedDetectorsList();

    theTrackFitter_->setOperation(&trackFitter::makeFittedTracksResiduals);
    theFileEater_->setOperation(&fileEater::updateEvents2,theTrackFitter_);
    this->launchThread2(theFileEater_);
}
//=============================================================================
void mainTabs::on_makeFittedTracksDeviationsPB_clicked()
{
    theFileEater_->setOperation(&fileEater::updateEvents2,theTrackFitter_);
    theTrackFitter_->setOperation(&trackFitter::makeFittedTrackDeviations);
    this->launchThread2(theFileEater_);
}
//=============================================================================
void mainTabs::on_residualsTabPagePlaqSelectCB_currentIndexChanged(const QString& plaqSelected)
{
    plaqSelected_= plaqSelected.toStdString();
    if( !ui->residualsTabPagePlaqSelectCB->isEnabled() || residualsType_.size() < 2) return;
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    if( ui->detectorsToBeAlignedLW->findItems(plaqSelected,0).first()->isSelected() )
        ui->trackFitterExcludeDetectorCB->setChecked(true );
    else
        ui->trackFitterExcludeDetectorCB->setChecked(false);

    //xProfile
    TH1* histo  ;
    TF1* fitFunc;
    histo = (TH1*)theHManager_->getHistogram(residualsType_[0], plaqSelected_);
    if ( histo->FindObject("gausFitFunc") )
    {
        fitFunc = (TF1*)histo->FindObject("gausFitFunc");
        ss_.str("");
        ss_ << fitFunc->GetParameter(0);
        ui->xResidualMeanLE->setText(QString::fromStdString(ss_.str()));
        ss_.str("");
        ss_ << fitFunc->GetParameter(1);
        ui->xResidualSigmaLE->setText(QString::fromStdString(ss_.str()));
    }
    else
    {
        ss_.str("");
        ss_ << histo->GetMean();
        ui->xResidualMeanLE->setText(QString::fromStdString(ss_.str()));
        ss_.str("");
        ss_ << histo->GetRMS();
        ui->xResidualSigmaLE->setText(QString::fromStdString(ss_.str()));
    }

    //y Profile
    histo = (TH1*)theHManager_->getHistogram(residualsType_[1], plaqSelected_);
    if ( histo->FindObject("gausFitFunc") )
    {
        fitFunc = (TF1*)histo->FindObject("gausFitFunc");
        ss_.str("");
        ss_ << fitFunc->GetParameter(0);
        ui->yResidualMeanLE->setText(QString::fromStdString(ss_.str()));
        ss_.str("");
        ss_ << fitFunc->GetParameter(1);
        ui->yResidualSigmaLE->setText(QString::fromStdString(ss_.str()));
    }
    else
    {
        ss_.str("");
        ss_ << histo->GetMean();
        ui->yResidualMeanLE->setText(QString::fromStdString(ss_.str()));
        ss_.str("");
        ss_ << histo->GetRMS();
        ui->yResidualSigmaLE->setText(QString::fromStdString(ss_.str()));
    }

    residualsManualFitLeftCanvas_->GetCanvas()->cd() ;
    gStyle->SetOptFit(11);
    theHManager_->getHistogram(residualsType_[0], plaqSelected_)->Draw() ;
    residualsManualFitRightCanvas_->GetCanvas()->cd();
    gStyle->SetOptFit(11);
    theHManager_->getHistogram(residualsType_[1], plaqSelected_)->Draw();

    residualsManualFitLeftCanvas_ ->GetCanvas()->Modified();
    residualsManualFitLeftCanvas_ ->GetCanvas()->Update()  ;
    residualsManualFitRightCanvas_->GetCanvas()->Modified();
    residualsManualFitRightCanvas_->GetCanvas()->Update()  ;
}
//===========================================================================
void mainTabs::swapResidualsFitCBs (bool checked )
{
    if (checked)
    {
        if ( sender() == (QObject*)ui->residualsFitAllCB )
        {
            ui->trackFitterMyFitParXCB->setChecked(false);
            ui->trackFitterMyFitParYCB->setChecked(false);
        }
        else ui->residualsFitAllCB->setChecked(false);
    }
}
//===========================================================================
void mainTabs::on_trackFitterExcludeDetectorCB_clicked(bool checked)
{
    if(plaqSelected_ == "") return;
    if(checked)
    {
        ui->detectorsToBeAlignedLW->findItems(QString::fromStdString(plaqSelected_),0).first()->setSelected(true );
        ui->trackFitterSoloDetectorAlignmentCB->setChecked(false                                                 );
    }
    else
        ui->detectorsToBeAlignedLW->findItems(QString::fromStdString(plaqSelected_),0).first()->setSelected(false);
}
//===========================================================================
void mainTabs::on_trackFitterSoloDetectorAlignmentCB_toggled(bool checked)
{
    if(plaqSelected_ == "") return;

    if(checked)
    {
        ui->detectorsToBeAlignedLW->selectAll();
        ui->detectorsToBeAlignedLW->findItems(QString::fromStdString(plaqSelected_),0).first()->setSelected(false);
        ui->trackFitterExcludeDetectorCB->setChecked(false  );
        ui->residualsTabPagePlaqSelectCB->setEnabled(false);
    }
    else
    {
        ui->residualsTabPagePlaqSelectCB->setEnabled(true);
    }
}
//===========================================================================
void mainTabs::on_detectorsToBeAlignedLW_itemClicked(QListWidgetItem* item)
{
    ui->trackFitterSoloDetectorAlignmentCB->setChecked(false);

    if( QString::fromStdString(plaqSelected_) ==  item->text() )
    {
        if ( ui->trackFitterExcludeDetectorCB->isChecked() ) ui->trackFitterExcludeDetectorCB->setChecked(false);
        else                                                 ui->trackFitterExcludeDetectorCB->setChecked(true );
    }
}
//===========================================================================
void mainTabs::on_alignDutCB_clicked(bool checked)
{
    ui->trackFitterSoloDetectorAlignmentCB->setChecked(false);

    for(int det=0; det < ui->detectorsToBeAlignedLW->count(); det++)
    {
        if ( theGeometry_->getDetector(ui->detectorsToBeAlignedLW->item(det)->text().toStdString())->isDUT() )
        {
            if (checked) ui->detectorsToBeAlignedLW->item(det)->setSelected(false);
            else         ui->detectorsToBeAlignedLW->item(det)->setSelected(true );
        }
        else
        {
            if (checked)
            {
                ui->detectorsToBeAlignedLW->item(det)->setSelected(true );
                ui->detectorsToBeAlignedLW->item(det)->setHidden  (true );
            }
            else
            {
                ui->detectorsToBeAlignedLW->item(det)->setSelected(false);
                ui->detectorsToBeAlignedLW->item(det)->setHidden  (false);
            }
        }
    }
}
//==============================================================================
void mainTabs::on_FitScatterResidualsPB_clicked()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    for( Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it )
    {
        double slope = 0, q = 0;
        theFitter_->linearFit((TH1*) theHManager_->getHistogram(X_RES_Y_POS_MEAN, (*it).first ),&slope,&q );
        theFitter_->linearFit((TH1*) theHManager_->getHistogram(Y_RES_X_POS_MEAN, (*it).first ),&slope,&q );
        theFitter_->linearFit((TH1*) theHManager_->getHistogram(X_RES_X_POS_MEAN, (*it).first ),&slope,&q );
        theFitter_->linearFit((TH1*) theHManager_->getHistogram(Y_RES_Y_POS_MEAN, (*it).first ),&slope,&q );
    }

    //gStyle->SetOptFit (1111);
    //gStyle->SetOptStat(1111);
    this->drawAll(residualsResidualsVsCoordinateLeftCanvas_ , X_RES_Y_POS_MEAN);
    this->drawAll(residualsResidualsVsCoordinateRightCanvas_, Y_RES_X_POS_MEAN);
}
//============================================================================
void mainTabs::on_applyZrotationsPB_clicked()
{
    if(!theGeometry_) return;

    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    TF1 *fitFunc;
    TH1 *histo;

    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it)
    {
        if( ui->detectorsToBeAlignedLW->findItems(QString::fromStdString((*it).first),0)[0]->isSelected() ) continue;

        double zRotation=0,yRotation=0,xRotation=0;

        //xProfile
        if ( ui->xResidualZRotationRB->isChecked() )
        {
            histo = (TH1*)theHManager_->getHistogram(X_RES_Y_POS_MEAN, (*it).first );
            if ( histo->FindObject("linearFitFunc") )
            {
                fitFunc   = (TF1*)histo->FindObject("linearFitFunc");
                zRotation = atan( fitFunc->GetParameter(0) );
            }

            zRotation = zRotation*180./TMath::Pi();
        }
        else if ( ui->yResidualZRotationRB->isChecked() )
        {
            //y Profile
            histo = (TH1*)theHManager_->getHistogram(Y_RES_X_POS_MEAN , (*it).first );
            if ( histo->FindObject("linearFitFunc") )
            {
                fitFunc   = (TF1*)histo->FindObject("linearFitFunc");
                zRotation = atan( fitFunc->GetParameter(0) );
            }

            zRotation = -zRotation*180./TMath::Pi();
        }

        if ( ui->xResidualYRotationRB->isChecked() )
        {
            histo = (TH1*)theHManager_->getHistogram(X_RES_X_POS_MEAN, (*it).first );
            if ( histo->FindObject("linearFitFunc") )
            {
                fitFunc   = (TF1*)histo->FindObject("linearFitFunc");
                yRotation = atan( fitFunc->GetParameter(0) );
            }

            yRotation = yRotation*180./TMath::Pi();
        }

        if ( ui->yResidualXRotationRB->isChecked() )
        {
            histo = (TH1*)theHManager_->getHistogram(Y_RES_Y_POS_MEAN, (*it).first );
            if ( histo->FindObject("linearFitFunc") )
            {
                fitFunc   = (TF1*)histo->FindObject("linearFitFunc");
                xRotation = atan( fitFunc->GetParameter(0) );
            }

            xRotation = xRotation*180./TMath::Pi();
        }

        //update values
        ss_.str("");
        ss_ << "Detector: "       << (*it).first ;
        ss_ << "\t zRotation "    << theGeometry_->getDetector( (*it).first )->getZRotationCorrection()
            << " zRotationCorrection: "  << zRotation ;
        STDLINE(ss_.str(),ACRed);
        ss_.str("");
        ss_ << "Detector: "       << (*it).first ;
        ss_ << "\t yRotation "    << theGeometry_->getDetector( (*it).first )->getYRotationCorrection()
            << " yRotationCorrection: "  << yRotation ;
        STDLINE(ss_.str(),ACRed);
        ss_.str("");
        ss_ << "Detector: "       << (*it).first ;
        ss_ << "\t xRotation "    << theGeometry_->getDetector( (*it).first )->getXRotationCorrection()
            << " xRotationCorrection: "  << xRotation ;
        STDLINE(ss_.str(),ACRed);

        theGeometry_->getDetector( (*it).first )->setZRotationCorrection(theGeometry_->getDetector( (*it).first )->getZRotationCorrection()+zRotation);
        theGeometry_->getDetector( (*it).first )->setYRotationCorrection(theGeometry_->getDetector( (*it).first )->getYRotationCorrection()+yRotation);
        theGeometry_->getDetector( (*it).first )->setXRotationCorrection(theGeometry_->getDetector( (*it).first )->getXRotationCorrection()+xRotation);
        if(ui->resetZRotationsCB->isChecked())
        {
            theGeometry_->getDetector( (*it).first )->setXRotationCorrection(0);
            theGeometry_->getDetector( (*it).first )->setYRotationCorrection(0);
            theGeometry_->getDetector( (*it).first )->setZRotationCorrection(0);
        }
    }
    theFileEater_->updateGeometry("geometry");
}
//==============================================================================
void mainTabs::on_manualRotationsPB_clicked()
{
    if(!theGeometry_) return;
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    for (Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it)
    {
        if( ui->detectorsToBeAlignedLW->findItems(QString::fromStdString((*it).first),0)[0]->isSelected() ) continue;

        theGeometry_->getDetector( (*it).first )->setXRotationCorrection(theGeometry_->getDetector( (*it).first )->getXRotationCorrection()+
                                                                         ui->xManualRotationSB->value()                                      );
        theGeometry_->getDetector( (*it).first )->setYRotationCorrection(theGeometry_->getDetector( (*it).first )->getYRotationCorrection()+
                                                                         ui->yManualRotationSB->value()                                      );
        theGeometry_->getDetector( (*it).first )->setZRotationCorrection(theGeometry_->getDetector( (*it).first )->getZRotationCorrection()+
                                                                         ui->zManualRotationSB->value()                                      );

        STDLINE((*it).first,ACYellow) ;
        ss_.str("");
        ss_ << "XRotationCorrection " << theGeometry_->getDetector((*it).first)->getXRotationCorrection();
        STDLINE(ss_.str(),ACRed);
        ss_.str("");
        ss_ << "YRotationCorrection " << theGeometry_->getDetector((*it).first)->getYRotationCorrection();
        STDLINE(ss_.str(),ACRed);
        ss_.str("");
        ss_ << "ZRotationCorrection " << theGeometry_->getDetector((*it).first)->getZRotationCorrection();
        STDLINE(ss_.str(),ACRed);

    }
    theFileEater_->updateGeometry("geometry");
}
//==============================================================================
void mainTabs::on_showSelectedEventsElectronDistribuitionPB_clicked()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    std::string histoType;

    if( selectedEvents_.empty() )
    {
        theHManager_->setOperation(&HManager::eventsCycle,&HManager::makeAdcDistributions2);
        //histoType = theHManager_->eventsCycle()[1]                ;
        this->launchThread2(theHManager_);
    }
    else
    {
        ss_.str("");
        ss_ << "Electron distributions for " << selectedEvents_.size() << " selected events";
        STDLINE(ss_.str(), ACGreen);
        int cSize = -1;
        if(ui->residualsOnlyClusterSizeCB->isChecked())
            cSize = ui->residualsOnlyClusterSizeSB->value();
        histoType = theHManager_->makeElectronDistribution(selectedEvents_, cSize)[0];
    }
}
//==============================================================================
//------------------------------------------FINE ALIGNMENT TAB---------------------------------------------
//=============================================================================
//===================================================================================================
void mainTabs::on_geometrySetPB_clicked()
{
    STDLINE("They never call me!",ACWhite);
    if(theGeometry_ == NULL) return;

    for(std::map<std::string,GeometryParameters*>::iterator it=geometryParameters_.begin(); it!=geometryParameters_.end(); it++ )
    {
        if(!it->second->isEnabled()) continue;
        Detector* det = theGeometry_->getDetector( it->first );
        det->setXRotation          (it->second->getDetectorParBase      (0));
        det->setXRotationCorrection(it->second->getDetectorParCorrection(0));
        det->setYRotation          (it->second->getDetectorParBase      (1));
        det->setYRotationCorrection(it->second->getDetectorParCorrection(1));
        det->setZRotation          (it->second->getDetectorParBase      (2));
        det->setZRotationCorrection(it->second->getDetectorParCorrection(2));
        det->setXPosition          (it->second->getDetectorParBase      (3));
        det->setXPositionCorrection(it->second->getDetectorParCorrection(3));
        det->setYPosition          (it->second->getDetectorParBase      (4));
        det->setYPositionCorrection(it->second->getDetectorParCorrection(4));
        det->setZPosition          (it->second->getDetectorParBase      (5));
        det->setZPositionCorrection(it->second->getDetectorParCorrection(5));
        det->dump();
    }

    theFileEater_->updateGeometry("geometry");
}
//===================================================================================
void mainTabs::on_fineAlignmentPB_clicked()
{
    if(!theGeometry_) return;
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    std::string alignmentFitMethod = ui->alignmentTypeCB ->currentText().toStdString();
    theAligner_->setAlignmentFitMethodName(alignmentFitMethod);
    if (alignmentFitMethod == "Simple")
        theAligner_->setNumberOfIterations(ui->fineTrackFitterIterationsSB->value());
    else
        theAligner_->setNumberOfIterations(0);

    //theAligner_->clearFixParMap();
    for(Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); ++it)
    {
        if( !(*it).second->isDUT() )
        {
            unsigned int val = 0;
            if( ui->detectorsTableView->isFixed( (*it).first, "Alfa"  ) ) val += 1;
            if( ui->detectorsTableView->isFixed( (*it).first, "Beta"  ) ) val += 10;
            if( ui->detectorsTableView->isFixed( (*it).first, "Gamma" ) ) val += 100;
            if( ui->detectorsTableView->isFixed( (*it).first, "X"     ) ) val += 1000;
            if( ui->detectorsTableView->isFixed( (*it).first, "Y"     ) ) val += 10000;
            if( ui->detectorsTableView->isFixed( (*it).first, "Z"     ) ) val += 100000;
            theAligner_->setFixParMap((*it).first, val);
        }
    }

    double       chi2cut    = -1;
    int          clusterSel = -1;
    unsigned int minPoints  = -1;
    int          maxTracks  = -1 ;
    int          nEvents    = -1 ;
    if( ui->fineAlignmentChi2SelectionCB       ->isChecked() ) chi2cut    = ui->fineAlignmentChi2SelectionSB       ->value();
    if( ui->fineAlignmentClusterSizeSelectionCB->isChecked() ) clusterSel = ui->fineAlignmentClusterSizeSelectionSB->value();
    if( ui->fineAlignmentMinPointsSelectionCB  ->isChecked() ) minPoints  = ui->fineAlignmentMinPointsSelectionSB  ->value();
    if( ui->fineAlignmentMaxTrackPerEventCB    ->isChecked() ) maxTracks  = ui->fineAlignmentMaxTrackPerEventSB    ->value();
    if( ui->fineAlignmentLimitCB               ->isChecked() ) nEvents    = ui->fineAlignmentLimitSB               ->value();

    theAligner_->setAlignmentPreferences(ui->maxTrialSB->value(),
                                         ui->fineAlignmentStrategySB->value(),
                                         chi2cut,
                                         clusterSel,
                                         minPoints,
                                         maxTracks,
                                         ui->fineAlignmentNoDiagonalClustersCB->isChecked(),
                                         "",
                                         nEvents);
    theAligner_->setOperation(&aligner::align);
    this->launchThread2(theAligner_);
}
//==============================================================================
void mainTabs::on_writeFineAlignmentResultsPB_clicked()
{
    if(!theGeometry_) return;

    ui->writeFineAlignmentResultsPB->setEnabled(false);
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    aligner::alignmentResultsDef alignmentResults = theAligner_->getAlignmentResults();

    for (Geometry::iterator geo=theGeometry_->begin(); geo!=theGeometry_->end(); ++geo)
    {
        if( !ui->detectorsTableView->isFixed( geo->first, "Update"  ) ) continue;

        Detector * theDetector = theGeometry_->getDetector( geo->first ) ;
        double xPositionCorrection = theDetector->getXPositionCorrection() + alignmentResults[geo->first].deltaTx;
        double yPositionCorrection = theDetector->getYPositionCorrection() + alignmentResults[geo->first].deltaTy;
        double zPositionCorrection = theDetector->getZPositionCorrection() + alignmentResults[geo->first].deltaTz;
        double xRotationCorrection = theDetector->getXRotationCorrection() + alignmentResults[geo->first].alpha  ;
        double yRotationCorrection = theDetector->getYRotationCorrection() + alignmentResults[geo->first].beta   ;
        double zRotationCorrection = theDetector->getZRotationCorrection() + alignmentResults[geo->first].gamma  ;

        theDetector->setXPositionCorrection(xPositionCorrection) ;
        theDetector->setYPositionCorrection(yPositionCorrection) ;
        theDetector->setZPositionCorrection(zPositionCorrection) ;
        theDetector->setXRotationCorrection(xRotationCorrection) ;
        theDetector->setYRotationCorrection(yRotationCorrection) ;
        theDetector->setZRotationCorrection(zRotationCorrection) ;
    }

    theFileEater_->updateGeometry("geometry");
    theGeometry_ = theFileEater_->getGeometry();
    showGeometry();
    theGeometry_->dump();
}

//==============================================================================
void mainTabs::on_trackFindAndFitAlignmentPB_clicked()
{
    std::string fitMethod  = ui->trackFitAlignmentNameCB ->currentText().toStdString();
    std::string findMethod = ui->trackFindAlignmentNameCB->currentText().toStdString();
    if (fitMethod == "Simple")
        theTrackFitter_->setNumberOfIterations(ui->trackFitterIterationsSB->value());
    else
        theTrackFitter_->setNumberOfIterations(0);

    theTrackFitter_->setFitMethodName(fitMethod);

    findAndFitTrack(findMethod, fitMethod);
}

//==============================================================================
//------------------------------------------DUT ALIGNMENT TAB---------------------------------------------
//=============================================================================
void mainTabs::on_dutAlignmentPB_clicked()
{
    if(!theGeometry_) return;
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    double       chi2cut            = -1;
    int          clusterSel         = -1;
    unsigned int minPoints          = -1;
    int          maxTracks          = -1;
    int          nEvents            = -1;
    if( ui->dutAlignmentChi2SelectionCB       ->isChecked() ) chi2cut            = ui->dutAlignmentChi2SelectionSB       ->value();
    if( ui->dutAlignmentClusterSizeSelectionCB->isChecked() ) clusterSel         = ui->dutAlignmentClusterSizeSelectionSB->value();
    if( ui->dutAlignmentMinPointsSelectionCB  ->isChecked() ) minPoints          = ui->dutAlignmentMinPointsSelectionSB  ->value();
    if( ui->dutAlignmentMaxTrackPerEventCB    ->isChecked() ) maxTracks          = ui->dutAlignmentMaxTrackPerEventSB    ->value();
    if( ui->dutAlignmentLimitCB               ->isChecked() ) nEvents            = ui->dutAlignmentLimitSB               ->value();

    theAligner_->setAlignmentPreferences(ui->dutMaxTrialsSB->value(),
                                         minPoints,
                                         chi2cut,
                                         clusterSel,
                                         minPoints,
                                         maxTracks,
                                         ui->dutNoDiagonalClustersCB->isChecked(),
                                         ui->dutAlignmentDutSelectCB->currentText().toStdString(),
                                         nEvents);

    theAligner_->setOperation(&aligner::alignDUT);
    this->launchThread2(theAligner_);
}
//========================================================================================
void mainTabs::on_findDUThitsPB_clicked()
{
    ui->abortActionPB               ->setEnabled(true) ;
    ui->tracksFoundCB               ->setChecked(true) ;

    double tolX = ui->DUTxRoadToleranceSB->value();
    double tolY = ui->DUTyRoadToleranceSB->value();

    double chi2Cut        = -1;
    double trackPoints    = -1;
    double maxPlanePoints = -1;

    theTrackFinder_->setTrackSearchParameters(tolX*(1e-4)*CONVF,
                                              tolY*(1e-4)*CONVF,
                                              chi2Cut          ,
                                              trackPoints      ,
                                              maxPlanePoints   );

    theTrackFinder_->setOperation(&trackFinder::findDUTCandidates);
    theFileEater_->setOperation(&fileEater::updateEvents2,theTrackFinder_);
    this->launchThread2(theFileEater_);
}
//==============================================================================
void mainTabs::on_dutWriteFineAlignmentPB_clicked()
{
    if(!theGeometry_) return;
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    aligner::alignmentResultsDef alignmentResults = theAligner_->getAlignmentResults();
    std::string dut = ui->dutAlignmentDutSelectCB->currentText().toStdString();

    Detector * theDetector = theGeometry_->getDetector( dut ) ;
    double xPositionCorrection = theDetector->getXPositionCorrection() + alignmentResults[dut].deltaTx;
    double yPositionCorrection = theDetector->getYPositionCorrection() + alignmentResults[dut].deltaTy;
    double zPositionCorrection = theDetector->getZPositionCorrection() + alignmentResults[dut].deltaTz;
    double xRotationCorrection = theDetector->getXRotationCorrection() + alignmentResults[dut].alpha  ;
    double yRotationCorrection = theDetector->getYRotationCorrection() + alignmentResults[dut].beta   ;
    double zRotationCorrection = theDetector->getZRotationCorrection() + alignmentResults[dut].gamma  ;

    theDetector->setXPositionCorrection(xPositionCorrection) ;
    theDetector->setYPositionCorrection(yPositionCorrection) ;
    theDetector->setZPositionCorrection(zPositionCorrection) ;
    theDetector->setXRotationCorrection(xRotationCorrection) ;
    theDetector->setYRotationCorrection(yRotationCorrection) ;
    theDetector->setZRotationCorrection(zRotationCorrection) ;

    theFileEater_->updateGeometry("geometry");
    theGeometry_ = theFileEater_->getGeometry();
    showGeometry();
    theGeometry_->dump();
}
//============================================================================================
void mainTabs::on_dutAlignmentDutSelectCB_currentIndexChanged(const QString detector)
{
    unsigned int fixParMap = theAligner_->getFixParMap( detector.toStdString() );

    //ss_.str("");
    //ss_ << detector.toStdString() << " code: " << fixParMap;
    //STDLINE(ss_.str(), ACPurple);

    if( fixParMap >= 100000 ) {ui->dutFineAlignmentZFixCB    ->setChecked(true ); fixParMap-= 100000;}
    else                       ui->dutFineAlignmentZFixCB    ->setChecked(false);
    if( fixParMap >=  10000 ) {ui->dutFineAlignmentYFixCB    ->setChecked(true ); fixParMap-=  10000;}
    else                       ui->dutFineAlignmentYFixCB    ->setChecked(false);
    if( fixParMap >=   1000 ) {ui->dutFineAlignmentXFixCB    ->setChecked(true ); fixParMap-=   1000;}
    else                       ui->dutFineAlignmentXFixCB    ->setChecked(false);
    if( fixParMap >=    100 ) {ui->dutFineAlignmentGammaFixCB->setChecked(true ); fixParMap-=    100;}
    else                       ui->dutFineAlignmentGammaFixCB->setChecked(false);
    if( fixParMap >=     10 ) {ui->dutFineAlignmentBetaFixCB ->setChecked(true ); fixParMap-=     10;}
    else                       ui->dutFineAlignmentBetaFixCB->setChecked(false);
    if( fixParMap ==      1 ) {ui->dutFineAlignmentAlphaFixCB->setChecked(true );}
    else                       ui->dutFineAlignmentAlphaFixCB->setChecked(false);
}
//===============================================================================
void mainTabs::updateFixParMap(bool)
{
    std::string dut = ui->dutAlignmentDutSelectCB->currentText().toStdString();
    unsigned int val = 0;
    if(ui->dutFineAlignmentAlphaFixCB->isChecked()) val += 1;
    if(ui->dutFineAlignmentBetaFixCB ->isChecked()) val += 10;
    if(ui->dutFineAlignmentGammaFixCB->isChecked()) val += 100;
    if(ui->dutFineAlignmentXFixCB    ->isChecked()) val += 1000;
    if(ui->dutFineAlignmentYFixCB    ->isChecked()) val += 10000;
    if(ui->dutFineAlignmentZFixCB    ->isChecked()) val += 100000;

    theAligner_->setFixParMap(dut, val);

    ss_.str("");
    ss_ << dut << " code: " << theAligner_->getFixParMap(dut);
    STDLINE(ss_.str(), ACYellow);
}

//==============================================================================
//------------------------------------------EVENT DISPLAY TAB---------------------------------------------
//=============================================================================
void mainTabs::on_showHitsFreqPB_clicked()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    HManager::stringVDef histoType;
    if ( ui->selectClusterRB->isChecked() )
    {
        theHManager_->setSubProcessFunction(&HManager::makeClusterPlots2);
        histoType = theHManager_->eventsCycle();
    }
    else
    {
        theHManager_->setSubProcessFunction(&HManager::makeHitsFreq2);
        histoType = theHManager_->eventsCycle();
    }

    this->drawAll(eventDisplayLeftCanvas_, histoType[0]);
}
//=============================================================================
void mainTabs::on_eventSelectedSpinBox_valueChanged(int eventSelected)
{
    //theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    STDLINE("next print is event",ACRed);
    Event* event = theFileEater_->getEvent(eventSelected);
    STDLINE(event,ACRed);

    std::string histoType;
    if( ui->selectClusterRB->isChecked() && !event->getClusters().empty())
    {
        histoType = theHManager_->makeClusterEvent(event)[0];
    }
    else
    {
        histoType = theHManager_->makeRawEvent(event);
    }
    //check clustersHits in the tree
    if ( event->getClustersHits().empty() )
    {
        STDLINE("No clusters in this event",ACCyan) ;
        //        ui->solveClustersPB->setEnabled(false);
        ui->selectClusterRB->setEnabled(false);
    }
    //    else
    //    {
    //        theHManager_->markClustersHits(event);
    //        ui->solveClustersPB->setEnabled(true);
    //ui->selectClusterRB->setEnabled(true);
    //    }

    //check clusters in the tree
    if ( !event->getClusters().empty() )
    {
        //STDLINE("-------------------------------------------------------------------------",ACYellow) ;
        //theHManager_->markClusters(event);
        ui->selectClusterRB              ->setEnabled(true);
        ui->rawAlignmentClusterProfilesRB->setEnabled(true);
    }


    eventDisplayRightCanvas_->GetCanvas()->cd();
    theHManager_->getHistogram(histoType, plaqSelected_)->Draw("COLZ")    ;

    if(ui->showAllPlaqPB->isChecked()) this->showAllPlaq();

    eventDisplayRightCanvas_->GetCanvas()->Modified() ;
    eventDisplayRightCanvas_->GetCanvas()->Update()   ;
}
//=================================================================
void mainTabs::on_eventDisplayTabPagePlaqSelectCB_currentIndexChanged(QString plaqSelected)
{
    if ( ui->eventDisplayTabPagePlaqSelectCB->isEnabled() )
    {
        plaqSelected_= plaqSelected.toStdString();
        this->on_eventSelectedSpinBox_valueChanged(ui->eventSelectedSpinBox->value());
    }
}
//=================================================================
void mainTabs::on_showAllPlaqPB_clicked()
{
    if(ui->showAllPlaqPB->isChecked())
    {
        this->showAllPlaq();
    }
}
//===========================================================================
void mainTabs::showAllPlaq()
{
    if(ui->selectRawEventsRB->isChecked())
    {
        this->drawAll(eventDisplayLeftCanvas_, "rawEvent", "", "COLZ");
    }
    if(ui->selectClusterRB->isChecked())
    {
        this->drawAll(eventDisplayLeftCanvas_, "clusterEvent", "", "COLZ");
    }
}
//===========================================================================
void mainTabs::on_findEventsPB_clicked()
{

    int ev=ui->eventSelectedSpinBox->value()+1;
    int size=0;
    int startValue = ui->eventSelectedSpinBox->value()      ;
    int loop = ui->eventDisplayTabPagePlaqSelectCB->count() ;

    while( ( size > ui->findEventsMaxHitsToSearchSB->value() || size < ui->findEventsMinHitsToSearchSB->value() ) && loop >= 0)
    {
        if( startValue == ev++ )
        {
            STDLINE("No event found in plaquette: " + plaqSelected_,ACYellow);
            if( ui->findEventsWhereComboBox->currentText() == "selected" ) break;

            ui->eventDisplayTabPagePlaqSelectCB->setCurrentIndex( ui->eventDisplayTabPagePlaqSelectCB->currentIndex()+1 );
            loop--;
        }

        if( ev > theFileEater_->getEventsNumber() )
        {
            ev=0;
            if( ui->findEventsWhereComboBox->currentText() == "all" )
                ui->eventDisplayTabPagePlaqSelectCB->setCurrentIndex( ui->eventDisplayTabPagePlaqSelectCB->currentIndex()+1 );
        }

        size = theFileEater_->getEvent(ev)->getRawData().find(plaqSelected_)->second.size();
    }

    ui->eventSelectedSpinBox->setValue(ev);
}
//===========================================================================
void mainTabs::findValuesChanged(int newValue)
{
    if( ui->findEventsMinHitsToSearchSB->value() > ui->findEventsMaxHitsToSearchSB->value() )
    {
        ui->findEventsMaxHitsToSearchSB->setValue(  newValue  );
        ui->findEventsMinHitsToSearchSB->setValue(  newValue  );
    }
}

//===========================================================================
void mainTabs::on_eventDisplayShowBeamSpotsPB_clicked()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    theHManager_->setSubProcessFunction(&HManager::makeBeamSpots2);
    std::string histoType = theHManager_->eventsCycle()[0];

    this->drawAll(eventDisplayLeftCanvas_, histoType, "", "COLZ");
}
//===========================================================================
void mainTabs::on_eventDisplayDUTprojectionPB_clicked()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    std::vector<std::string> selectedDUTs;
    for(int dut=0; dut < ui->eventDisplayDUTListW->selectedItems().size(); dut++)
    {
        selectedDUTs.push_back( ui->eventDisplayDUTListW->selectedItems()[dut]->text().toStdString() );
        //       ss_.str("");
        //       ss_ << "selected DUT: " << ui->eventDisplayDUTListW->selectedItems()[dut]->text().toStdString();
        //       STDLINE(ss_.str(),ACGreen);
    }
    //std::string histoType;
    //histoType = theHManager_->makeDUTprojections(selectedDUTs)[0];
    STDLINE("WARNING: Work in progress, function disabled!",ACRed);

    //this->drawAll(ui->eventDisplayLeftCanvas, histoType, "COLZ");
}
//=============================================================================
void mainTabs::on_show3dFittedTracksBeamPB_clicked()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    std::string histoType = theHManager_->makeFittedTracksBeam(selectedEvents_)[0];

    this->drawAll(eventDisplayLeftCanvas_, histoType, "", "COLZ");
}

//===========================================================================
//**************************************** CONTROLS TAB **************************************************************
//===========================================================================
void mainTabs::on_fileEaterVerbosityCB_activated(int verbosity)
{
    theFileEater_->setVerbosity(verbosity);
}
//===========================================================================
void mainTabs::on_buildPlotsPB_clicked()
{
    this->launchThread3(theHManager_,this,&mainTabs::buildClusterPlots);
}
//===========================================================================
void mainTabs::buildClusterPlots()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    theHManager_->setSubProcessFunction(&HManager::makeClusterPlots2);
    HManager::stringVDef histoType = theHManager_->eventsCycle();
    emit mainTabs::processFinished(&mainTabs::buildClusterPlots_end,histoType);
}
//===========================================================================
void mainTabs::buildClusterPlots_end(HManager::stringVDef histoType)
{
    this->drawAll(beamSpotProjXCanvas_,histoType[1]);
    this->drawAll(beamSpotProjYCanvas_,histoType[2]);
    this->drawAll(beamSpot2DCanvas_,   histoType[3]);
    this->drawAll(clustersCanvas_,     histoType[0]);
}

//===========================================================================
void mainTabs::on_TEMPSHOWTRACKSPB_clicked()
{
    theHManager_->setRunSubDir( theFileEater_->openFile(ui->loadedRootFileLE->text().toStdString()) );

    Event *theEvent = theFileEater_->getEvent(ui->eventSelectedSpinBox->value());

    HManager::stringVDef xyVec = theHManager_->makeTrackEvent( theEvent );

    //       ui->eventDisplayLeftCanvas->GetCanvas()->Clear();
    //       ui->eventDisplayLeftCanvas->GetCanvas()->cd();
    //       theHManager_->getHistogram(xyPair.first)->Draw();

    //       ui->eventDisplayLeftCanvas->GetCanvas()->Clear();
    //       ui->eventDisplayLeftCanvas->GetCanvas()->cd();
    //       theHManager_->getHistogram(xyPair.second)->Draw();
}

//===========================================================================
void mainTabs::on_loadFakeBeam_clicked()
{
    theBeamSimulator_->setErrors(ui->simulationXerrorSB->value(),ui->simulationYerrorSB->value());

    QString localPath = "../BeamEmulator/" ;
    QString fileName = QFileDialog::getOpenFileName(this,"Beam simulation files",localPath,"Binary Files(*.nhd)");
    if (fileName.isEmpty())  return ;

    this->on_loadGeometryPB_clicked();

    std::string outputFile = theFileEater_->openFile(fileName.toStdString());

    ss_.str("");
    ss_ << path_.toStdString() << "/" << outputFile;

    this->openRootFile(QString::fromStdString(ss_.str()).replace(".nhd",".root"));
}
//===========================================================================
void mainTabs::enableLimitZ(int state)
{
    ss_.str("") ; ss_ << "State " << state ; STDLINE(ss_.str(),ACPurple) ;
    if( state == Qt::Checked )
    {
        ui->limitZSB->setEnabled(true) ;
    }
    else
    {
        ui->limitZSB->setEnabled(false) ;
    }
}

//===========================================================================
void mainTabs::on_residualRangeLowCB_activated(QString )
{
    if( ui->residuaLockRangeCB->isChecked() )
    {
        ui->residualRangeHigCB->setCurrentIndex(ui->residualRangeLowCB->currentIndex()) ;
    }
}

//===========================================================================
void mainTabs::on_residualRangeHigCB_activated(QString )
{
    if( ui->residuaLockRangeCB->isChecked() )
    {
        ui->residualRangeLowCB->setCurrentIndex(ui->residualRangeHigCB->currentIndex()) ;
    }
}

//===========================================================================
void mainTabs::on_saveXMLResultsPB_clicked()
{
    QFile xmlFile(ui->xmlGeometryLE->text());
    if(!xmlFile.exists())
    {
        QString msg = "File " + ui->xmlGeometryLE->text() + " doesn't exist. Go in the Geometry tab and click Change to change the xml file with an existing one!";
        QMessageBox::information(this, tr("Monicelli"), tr(msg.toStdString().c_str()));
        return;
    }
    mainWindow_->editXMLPanel(ui->xmlGeometryLE->text()) ;
    XMLEditor * theXMLEditor = mainWindow_->getXMLEditor() ;

    for (Geometry::iterator geo=theGeometry_->begin(); geo!=theGeometry_->end(); ++geo)
    {

        Detector * theDetector = theGeometry_->getDetector( geo->first ) ;
        double xPosition           = theDetector->getXPosition()           ;
        double yPosition           = theDetector->getYPosition()           ;
        double zPosition           = theDetector->getZPosition()           ;
        double xPositionCorrection = theDetector->getXPositionCorrection() ;
        double yPositionCorrection = theDetector->getYPositionCorrection() ;
        double zPositionCorrection = theDetector->getZPositionCorrection() ;
        double xRotation           = theDetector->getXRotation()           ;
        double yRotation           = theDetector->getYRotation()           ;
        double zRotation           = theDetector->getZRotation()           ;
        double xRotationCorrection = theDetector->getXRotationCorrection() ;
        double yRotationCorrection = theDetector->getYRotationCorrection() ;
        double zRotationCorrection = theDetector->getZRotationCorrection() ;

        boost::cmatch what;
        static const boost::regex exp("Station: (\\d+) - Plaq: (\\d+)", boost::regex::perl);

        int stationId  = -1 ;
        int detectorId = -1 ;

        if( boost::regex_match(geo->first.c_str(), what, exp, boost::match_extra) )
        {
            stationId  = Utils::toInt(what[1]);
            detectorId = Utils::toInt(what[2]);
        }
        else
        {
            STDLINE(std::string("Invalid address: ")+geo->first,ACRed) ;
            assert(0) ;
        }

        theXMLEditor->setXPosition(stationId,detectorId,xPosition / CONVF) ;
        theXMLEditor->setYPosition(stationId,detectorId,yPosition / CONVF) ;
        theXMLEditor->setZPosition(stationId,detectorId,zPosition / CONVF) ;
        theXMLEditor->setXPositionCorrection(stationId,detectorId,xPositionCorrection     / CONVF) ;
        theXMLEditor->setYPositionCorrection(stationId,detectorId,yPositionCorrection     / CONVF) ;
        theXMLEditor->setZPositionCorrection(stationId,detectorId,zPositionCorrection     / CONVF) ;
        theXMLEditor->setXRotation          (stationId,detectorId,xRotation                 ) ;
        theXMLEditor->setYRotation          (stationId,detectorId,yRotation                 ) ;
        theXMLEditor->setZRotation          (stationId,detectorId,zRotation                 ) ;
        theXMLEditor->setXRotationCorrection(stationId,detectorId,xRotationCorrection       ) ;
        theXMLEditor->setYRotationCorrection(stationId,detectorId,yRotationCorrection       ) ;
        theXMLEditor->setZRotationCorrection(stationId,detectorId,zRotationCorrection       ) ;
    }
}

//===========================================================================
QString mainTabs::getEnvPath(QString environmentName)
{
    QString localPath = getenv(environmentName.toStdString().c_str());

    if(localPath.isEmpty())
    {
        localPath = path_ ;
        ss_.str("") ;
        ss_ << ACRed << ACBold
            << "WARNING: "
            << ACPlain
            << "environment variable "
            << environmentName.toStdString()
            << " is undefined. Assuming "
            << localPath.toStdString()
            << " as its value." ;
        STDLINE(ss_.str(),ACYellow) ;
    }
    if(localPath[localPath.size()-1] != '/')
        localPath += '/';

    return localPath ;
}

//===========================================================================
void mainTabs::on_selectFilesPB_clicked()
{
    QString localPath = this->getEnvPath("Monicelli_RawData_Dir") ;
    QStringList fileNames = QFileDialog::getOpenFileNames(this,"Merged files",localPath,"Dat Files(*.dat);;Text files (*.txt)");
    if (fileNames.size() == 0) return  ;

    for(int i=0; i<fileNames.size(); ++i )
    {
        ui->selectedFilesLW->insertItem(i,fileNames.at(i)) ;
    }

    if( ui->geometryLE->text() != QString("") ) ui->reconstructEventsPB->setEnabled(true) ;
    //  this->ui->maxRawEventsAllCB->setEnabled(true);
}

//===========================================================================
void mainTabs::on_selectGeometryPB_clicked()
{
    QString localPath = this->getEnvPath("Monicelli_XML_Dir") ;
    QString fileName = QFileDialog::getOpenFileName(this,"testBeamGeometry files",localPath,"xml Files(*.xml);;geo Files(*.geo)");
    if (fileName.isEmpty()) return ;

    ui->geometryLE->setText(fileName) ;

    if( ui->selectedFilesLW->count() > 0) ui->reconstructEventsPB->setEnabled(true) ;
}

//===========================================================================
void mainTabs::on_reconstructEventsPB_clicked()
{
    STDLINE("",ACWhite) ;
    STDLINE("================ Batch processing for the track reconstruction of the following files ===================", ACYellow) ;

    //inputFiles_.clear();

    std::string inputFile = "";

    for(int i=0; i<ui->selectedFilesLW->count(); ++i)
    {
        std::string fileName = ui->selectedFilesLW->item(i)->text().toStdString() ;
        inputFiles_.push_back(fileName) ;
        ss_.str("");
        ss_ << i << "]\t" << fileName ;
        STDLINE(ss_.str(),ACGreen) ;
    }

    for(int i=0; i<ui->selectedFilesLW->count(); ++i)
    {
        if(ui->selectedFilesLW->item(i)->isSelected()) continue;
        inputFile = ui->selectedFilesLW->item(i)->text().toStdString();
        ui->selectedFilesLW->item(i)->setSelected(true);
        break;
    }

    if(inputFile.empty())
    {
        STDLINE("ALL files have been processed",ACGreen);
        return;
    }
    //if(inputFiles_.empty()) return;
    STDLINE("",ACWhite) ;

    theFileEater_->openFile( ui->geometryLE->text().toStdString() );
    //theFileEater_->setInputFileNames( inputFiles_ );

    double chi2Cut        = -1 ;
    double trackPoints    = -1 ;
    double maxPlanePoints = -1 ;
    if( ui->trackFinderChi2cutCB->isChecked()     ) chi2Cut        = ui->trackFinderChi2cutSB->value() ;
    if( ui->trackFinderTrackPointsCB->isChecked() ) trackPoints    = ui->trackFinderTrackPointsSB->value();
    if( ui->trackFinderPlanePointsCB->isChecked() ) maxPlanePoints = ui->trackFinderPlanePointsSB->value();

    theTrackFinder_->setTrackSearchParameters(ui->xRoadToleranceSB->value()*(1e-4)*CONVF,
                                              ui->yRoadToleranceSB->value()*(1e-4)*CONVF,
                                              chi2Cut                                   ,
                                              trackPoints                               ,
                                              maxPlanePoints                            );
    STDLINE("",ACWhite) ;

    if (ui->maxRawEventsAllCB->isChecked())
           theFileEater_->setEventsLimit( ui->maxRawEventsAllSB->value() );
    else   theFileEater_->setEventsLimit( -1                             );

    theFileEater_  ->setOperation(&fileEater::fullReconstruction,theClusterizer_);
    theTrackFinder_->setOperation(&trackFinder::findAndFitTracks);//Might not work, used to be road search, but road search no longer fits
    theFileEater_  ->addSubProcess(theTrackFinder_);
    theTrackFitter_->setOperation(&trackFitter::makeFittedTracksResiduals);
    theFileEater_  ->addSubProcess(theTrackFitter_);

    theFileEater_->setInputFileName(inputFile);
    this->launchThread2(theFileEater_);
}

//===========================================================================
void mainTabs::on_userModeAlignPB_clicked()
{
    //    //find tracks with 8 points
    //    ui->trackFinderTrackPointsCB->setChecked(true);
    //    ui->trackFinderTrackPointsSB->setValue(8)     ;
    //    ui->findTrackFirstAndLastPB->click();
    //    ui->findTrackRoadSearchPB->click();
    //    //make costrained residuals
    //    ui->makeFittedTracksDeviationsPB->click();
    //    //align with histogram mean
    //    ui->alignMethodsCB->setCurrentIndex(2);
    //    ui->trackFitterFitPB->click();
    //    ui->trackFitterWriteAlignmentPB->click();
    //    //find tracks with 8 points
    //    ui->trackFinderTrackPointsCB->setChecked(true);
    //    ui->trackFinderTrackPointsSB->setValue(8)     ;
    //    //go for the fine alignment
    //    ui->maxTrialSB->setValue(10);
    //    //find first and last detector in space
    //    std::string firstDet = theGeometry_->begin()->first;
    //    std::string lastDet  = theGeometry_->begin()->first;
    //    for ( Geometry::iterator det=theGeometry_->begin(); det!=theGeometry_->end(); det++ )
    //    {
    //        if ( det->second->isDUT() ) continue;
    //        if ( det->second->getZPosition() <
    //             theGeometry->getDetector(firstDet)->getZPosition() )
    //        {
    //            firstDet = (*det).first;
    //            theAligner_->setFixParMap(firstDet , 111111);
    //        }
    //        else if ( det->second->getZPosition() >
    //                  theGeometry->getDetector(lastDet)->getZPosition()  )
    //        {
    //            lastDet  = (*det).first;
    //            theAligner_->setFixParMap(lastDet, 111111);
    //        }
    //        else
    //        {
    //            theAligner_->setFixParMap(lastDet, 000001);
    //        }
    //    }

}

//===================================================================================================
void mainTabs::on_testButtonPB_clicked()
{
    Detector *det = theGeometry_->getDetector(ui->eventDisplayTabPagePlaqSelectCB->currentText().toStdString());
    Event::clustersMapDef &clus = theFileEater_->getEvent(ui->eventSelectedSpinBox->value())->getClusters();
    ss_.str("");
    ss_ << "Detector " << ui->eventDisplayTabPagePlaqSelectCB->currentText().toStdString();
    STDLINE(ss_.str(),ACPurple);
    for(Event::aClusterMapDef::iterator it=clus[det->getID()].begin(); it!=clus[det->getID()].end(); ++it)
    {
        double x = it->second["x"];
        double y = it->second["y"];
        ss_.str("");
        ss_ << "hit number " << it->first << "--> x: " << x << " y: " << y;
        STDLINE(ss_.str(),ACPurple);
        Detector::rowColPair rowCol = det->getPixelCellFromLocal(x,y);
        ss_.str("");
        ss_ << "cell " << "row: " << rowCol.first << " col: " << rowCol.second;
        STDLINE(ss_.str(),ACGreen);
    }
}

//===================================================================================================
void mainTabs::on_saveCalibToROOTPB_clicked()
{
    QString calibOutputDirectory = getEnvPath("Monicelli_CalSample_Dir");
    // Pass command to output to text file to calibrationLoader_
    theCalibrationLoader_->saveROOTcalibrationFiles(calibOutputDirectory.toStdString());
}

//===================================================================================================
bool mainTabs::on_changeXmlGeometryPB_clicked()
{
    QString localPath = this->getEnvPath("Monicelli_XML_Dir") ;
    QString fileName = QFileDialog::getOpenFileName(this,"testBeamGeometry files",localPath,"xml Files(*.xml)");
    if (fileName.isEmpty())
    {
        //ui->loadedGeometryLE->setText("No geometry xml file selected");
        //ui->xmlGeometryLE->setText("No geometry xml file selected");
        return false  ;
    }
    ui->xmlGeometryLE->setText(fileName);
    theFileEater_->getGeometry()->setGeometryFileName(fileName.toStdString());
    theFileEater_->updateGeometry("geometry");
    theGeometry_ = theFileEater_->getGeometry();

    return true;
}

//===================================================================================================
void mainTabs::on_copyGeoGeometryPB_clicked()
{
    if(ui->geometryGeoGeometryFileLE->text().lastIndexOf("/") == -1 )
    {
        QMessageBox::information(this, tr("Monicelli"), tr("There is no geo file selected."));
        return;
    }
    QString localPath = this->getEnvPath("Monicelli_XML_Dir");
    QString fileName = ui->geometryGeoGeometryFileLE->text();
    fileName.remove(0,fileName.lastIndexOf("/")+1);
    fileName = localPath + fileName;
    this->copyGeoFileTo(fileName);
}

//===================================================================================================
void mainTabs::on_copyToGeoGeometryPB_clicked()
{
    QString localPath = this->getEnvPath("Monicelli_XML_Dir") ;
    QString fileName = QFileDialog::getSaveFileName(this,"testBeamGeometry files",localPath,"geo Files(*.geo)");
    if (!fileName.isEmpty())
    {
        this->copyGeoFileTo(fileName);
    }

}

//===================================================================================================
void mainTabs::copyGeoFileTo(QString fileName)
{
    if(ui->geometryGeoGeometryFileLE->text().lastIndexOf("/") == -1 )
    {
        QMessageBox::information(this, tr("Monicelli"), tr("There is no geo file selected."));
        return;
    }
    STDLINE(fileName.toStdString(),ACRed);
    QString cmd = "cp " + ui->geometryGeoGeometryFileLE->text() + " " + fileName;
    system(cmd.toStdString().c_str() ) ;
    STDLINE(cmd.toStdString(),ACCyan)  ;
}

//===================================================================================================
void mainTabs::showGeometry()
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(theGeometry_ == NULL) return;
    int yPos = 0;
    int row  = 0;
    GeometryParameters* tmpGeoPars;
    for(Geometry::iterator it=theGeometry_->begin(); it!=theGeometry_->end(); it++)
    {
        if(geometryParameters_.find(it->first) == geometryParameters_.end())
        {
            tmpGeoPars = new GeometryParameters();
            tmpGeoPars->setGeometry(0,yPos,tmpGeoPars->width(),tmpGeoPars->height());
            yPos += tmpGeoPars->height();
            geometryParameters_[it->first] = tmpGeoPars;
        }
        else
        {
            tmpGeoPars = geometryParameters_[it->first];
        }
        if(row >= ui->geometryDisplayTable->rowCount())
        {
            ui->geometryDisplayTable->insertRow(ui->geometryDisplayTable->rowCount());
            ui->geometryDisplayTable->setRowHeight(row, tmpGeoPars->height());
            ui->geometryDisplayTable->setColumnWidth(0, tmpGeoPars->width()+4);
            ui->geometryDisplayTable->setCellWidget(row, 0, tmpGeoPars);
        }

        tmpGeoPars->showDetectorPars(it->second);
        //tmpGeoPars->show();
        row++;

    }
    ui->geometryDisplayTable->show();
    geometryDisplayShrinkFix_++;
}
//===================================================================================================
void mainTabs::on_geometryDisableEnableAllPB_clicked()
{
    if(theGeometry_ == NULL) return;
    bool enable = false;
    if(ui->geometryDisableEnableAllPB->text() == "Disable All")
    {
        enable = false;
        ui->geometryDisableEnableAllPB->setText("Enable All");
        ui->geometryDisableEnableAllPB->setStyleSheet("* { background-color: rgb(0,170,0) }");
    }
    else
    {
        enable = true;
        ui->geometryDisableEnableAllPB->setText("Disable All");
        ui->geometryDisableEnableAllPB->setStyleSheet("* { background-color: red }");
    }

    for(std::map<std::string,GeometryParameters*>::iterator it=geometryParameters_.begin(); it!=geometryParameters_.end(); it++ )
    {
        it->second->enable(enable);
    }

}

//===================================================================================================
void mainTabs::on_geometrySumAngleCorrToBasePB_clicked()
{
    if(theGeometry_ == NULL) return;
    double base;
    double correction;
    Detector* detector;
    for(std::map<std::string,GeometryParameters*>::iterator it=geometryParameters_.begin(); it!=geometryParameters_.end(); it++ )
    {
        if(!it->second->isEnabled()) continue;
        detector   = theGeometry_->getDetector(it->first);
        base       = detector->getXRotation();
        correction = detector->getXRotationCorrection();
        base += correction;
        detector->setXRotation(base);
        detector->setXRotationCorrection(0);

        base       = detector->getYRotation();
        correction = detector->getYRotationCorrection();
        base += correction;
        detector->setYRotation(base);
        detector->setYRotationCorrection(0);

        base       = detector->getZRotation();
        correction = detector->getZRotationCorrection();
        base += correction;
        detector->setZRotation(base);
        detector->setZRotationCorrection(0);

        it->second->showDetectorPars(detector);
    }
    theFileEater_->updateGeometry("geometry");
}

//===================================================================================================
void mainTabs::on_geometrySumTransCorrToBasePB_clicked()
{
    if(theGeometry_ == NULL) return;
    double base;
    double correction;
    Detector* detector;
    for(std::map<std::string,GeometryParameters*>::iterator it=geometryParameters_.begin(); it!=geometryParameters_.end(); it++ )
    {
        if(!it->second->isEnabled()) continue;
        detector   = theGeometry_->getDetector(it->first);
        base       = detector->getXPosition();
        correction = detector->getXPositionCorrection();
        base += correction;
        detector->setXPosition(base);
        detector->setXPositionCorrection(0);

        base       = detector->getYPosition();
        correction = detector->getYPositionCorrection();
        base += correction;
        detector->setYPosition(base);
        detector->setYPositionCorrection(0);

        base       = detector->getZPosition();
        correction = detector->getZPositionCorrection();
        base += correction;
        detector->setZPosition(base);
        detector->setZPositionCorrection(0);

        it->second->showDetectorPars(detector);
    }
    theFileEater_->updateGeometry("geometry");
}

//===================================================================================================
void mainTabs::on_geometrySumAllCorrToBasePB_clicked()
{
    on_geometrySumAngleCorrToBasePB_clicked();
    on_geometrySumTransCorrToBasePB_clicked();
}

//===================================================================================================
void mainTabs::on_geometryClearAllCorrectionsPB_clicked()
{
    if(theGeometry_ == NULL) return;
    Detector* detector;
    for(std::map<std::string,GeometryParameters*>::iterator it=geometryParameters_.begin(); it!=geometryParameters_.end(); it++ )
    {
        if(!it->second->isEnabled()) continue;
        detector   = theGeometry_->getDetector(it->first);
        detector->setXRotationCorrection(0);
        detector->setYRotationCorrection(0);
        detector->setZRotationCorrection(0);
        detector->setXPositionCorrection(0);
        detector->setYPositionCorrection(0);
        detector->setZPositionCorrection(0);

        it->second->showDetectorPars(detector);
    }
    theFileEater_->updateGeometry("geometry");
}

//===================================================================================================
std::string mainTabs::getPlaneID (int station, int plaquette)
{
    std::stringstream ss;
    ss << "Station: " << station << " - Plaq: " << plaquette;
    return ss.str();
}

//===================================================================================================
void mainTabs::fixStrips(int state)
{
    for(int row = 0; row<=ui->detectorsTableView->rowCount()-1; row++)
    {
        std::string plaqID = ui->detectorsTableView->indexAt(QPoint(0,row * ui->detectorsTableView->rowHeight(row))).data().toString().toUtf8().constData();
        //std::cout << __PRETTY_FUNCTION__ << plaqID << std::endl;
        Detector * theDetector = theGeometry_->getDetector(plaqID);
        //std::cout << __PRETTY_FUNCTION__ << theDetector->isStrip() << std::endl;
        if(theDetector->isStrip())
        {
                if (theGeometry_->getDetectorModule(plaqID)%2 == 0) ui->detectorsTableView->fixXStrip(state, row);
                else ui->detectorsTableView->fixYStrip(state, row);
        }
    }
}

//================================================================================
void mainTabs::setAlignmentBoxes(const QString alignmentMethod)
{
    std::string alignmentMethodString = alignmentMethod.toUtf8().constData();
    if (alignmentMethodString == "Kalman")
    {
        ui->fineTrackFitterIterationsSB->setEnabled(false);
        fixStrips(1);
    }
    else if (alignmentMethodString == "Simple")
    {
        ui->fineTrackFitterIterationsSB->setEnabled(true);
        //ui->detectorsTableView->enableAll(1);
        fixStrips(0);
    }

}

//================================================================================
void mainTabs::on_clearBulkFilesPB_clicked()
{
    ui->selectedFilesLW->clear() ;
    ui->geometryLE->clear() ;
}
