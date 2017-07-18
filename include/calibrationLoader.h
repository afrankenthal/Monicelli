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
 
#ifndef CALIBRATIONLOADER_H
#define CALIBRATIONLOADER_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <TH2.h>
#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TKey.h>
#include <vector>
#include <sys/stat.h>

#include <QtWidgets/QProgressBar>

#include "EventHeader.h"
#include "Geometry.h"
#include "geometryLoader.h"
#include "fileEater.h"
#include "HManager.h"
#include "fitter.h"
#include "MessageTools.h"
#include "process.h"

class calibrationLoader : public process
{
 public :
    //                     row           col            plot     fitPar and covMat
    typedef       std::map<int, std::map<int, std::pair<TH1I*, fitter::fitResultDef> > > pixelPlotsMapDef       ;
    //                     detectorID          rocID   pixel
    typedef       std::map<std::string, std::map<int,pixelPlotsMapDef> >                 calibrationPlotsMapDef ;
    //                     detectorID          rocID 1D/2D
    typedef       std::map<std::string, std::map<int,TH1I*> >                            calibrationPlotsMapDefH;
    typedef       std::map<std::string, std::map<int,TH2F*> >                            calibrationPlotsMapDefS;

                  calibrationLoader        (fileEater      * theFileEater         ,
                                            HManager       * theHManager          ,
                                            fitter         * theFitter            ,
                                            geometryLoader * theGeometryLoader    ,
                                            QProgressBar   * parseProgressBar               );
  
                 ~calibrationLoader        (void                                            );
  
    bool          loadASCIIcalibrationFile (std::string      fileName                       );
    bool          loadROOTcalibrationFiles (std::string      detector             ,
                                            ROC            * roc                  ,
                                            std::string      fileName                       );
    bool          loadAllCalibrationFiles  (void                                            );
    void          saveROOTcalibrationFiles (std::string      fileDirectory                  );
    void          removeCalibrationFiles   (std::string      fileDirectory                  );
    void          restoreCalibrationFiles  (std::string      fileDirectory                  );
    void          listHeader               (void                                            );
    bool          makeHistograms           (std::string      detector             ,
                                            ROC            * roc                  ,
                                            bool             fit           = false,
                                            bool             writeGeometry = false,
                                            bool             writeASCII    = false          );
    bool          makeDUTHistograms        (std::string      detector             ,
                                            ROC            * roc                  ,
                                            bool             fit           = false,
                                            bool             writeGeometry = false,
                                            bool             writeASCII    = false          );
    void          makeChi2Histograms       (void                                            );
    TH1*          getHistogram             (std::string      detectorID           ,
                                            int              rocID                ,
                                            int              row                  ,
                                            int              col                            );
    bool          getCalibrationLoaded     (void                                            ){return calibrationsLoaded_;}
    TH1I*         get1DChi2                (std::string      detectorID           ,
                                            int              rocID                          );
    TH2F*         get2DChi2                (std::string      detectorID           ,
                                            int              rocID                          );
    vector<TH1I*> getAll1DChi2             (void                                            );
    vector<TH2F*> getAll2DChi2             (void                                            );
    void          writeGeometry            (std::string      detector             ,
                                            ROC            * roc                            );

    void          setOutputASCIIfile       (bool             writeASCII,
                                            std::string      outputFile = "calib_output.txt"){
                                                                                              writeASCII_     = writeASCII;
                                                                                              outputASCIIfile_= outputFile;
                                                                                             }
    void          setInputROOTfile         (bool             readROOT                       ){ readROOT_      = readROOT  ;}

    bool          execute                  (void                                            );

    std::string   getLabel                 (void                                            );
    int           getMaxIterations         (void                                            );
    std::string   getName                  (void                                            ){return "calibrationLoader"  ;}
    bool          checkFileStatus          (std::string      path                           );
//  void          fitHistograms            (std::string      detector,
//                                              ROC            * roc,
//                                              double           xmin,
//                                              double           xmax                       );

private :

    //                reg            hits  adc
    typedef std::map< int , std::pair<int, int> >              aPixelDataMapDef           ;
    //               row           col      data
    typedef std::map<int, std::map<int,aPixelDataMapDef> >     pixelDataMapDef            ;
    typedef std::map<std::string, std::string>                 headerMapDef               ;
    typedef std::map<int, std::map<int,fitter::fitResultDef> > calibrationfitResultsMapDef;

    ofstream                  outputFile_         ;
    std::string               outputASCIIfile_    ;
    fileEater               * theFileEater_       ;
    HManager                * theHManager_        ;
    fitter                  * theFitter_          ;
    geometryLoader          * theGeometryLoader_  ;
    TH1I                    * emptyTH1I_          ;

    headerMapDef              header_             ;
    pixelDataMapDef           pixels_             ;
    calibrationPlotsMapDef    calibrations_       ;
    calibrationPlotsMapDefH   chisquaresH_        ;
    calibrationPlotsMapDefS   chisquaresS_        ;
    bool                      readROOT_           ;
    bool                      writeASCII_         ;
    bool                      calibrationsLoaded_ ;
    QProgressBar            * parseProgressBar_   ;
    std::stringstream         ss_                 ;
    TH2F                    * firstBinHisto_      ;
    TH2F                    * lastBinHisto_       ;
} ;

#endif // CALIBRATIONLOADER_H
