/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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

/*
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>
*/
#include "EventHeader.h"
#include "Geometry.h"
#include "fileEater.h"
#include "HManager.h"
#include "fitter.h"
#include "MessageTools.h"
#include "process.h"

//#define ELECTRONS_NUMBER 421
#define ELECTRONS_NUMBER 332 // 1800/260*48

class calibrationLoader : public process
{
public :
    calibrationLoader(fileEater * theFileEater,
                      HManager  * theHManager,
                      fitter    * theFitter   ) ;
    ~calibrationLoader(void                    ) ;

    //               row           col            plot     fitPar and covMat
    typedef std::map<int, std::map<int, std::pair<TH1I*, fitter::fitResultDef> > >   pixelPlotsMapDef      ;
    //               detectorID          rocID   pixel
    typedef std::map<std::string, std::map<int,pixelPlotsMapDef> >                   calibrationPlotsMapDef;

    bool    loadASCIIcalibrationFile (std::string   fileName                       );
    bool    loadROOTcalibrationFiles (std::string   detector,
                                      ROC         * roc,
                                      std::string   fileName                       );
    bool    loadAllCalibrationFiles  (void                                         );
    void    saveROOTcalibrationFiles (std::string   fileDirectory                  );
    void    listHeader               (void                                         );
    bool    makeHistograms           (std::string   detector,
                                      ROC         * roc,
                                      bool          fit           = false,
                                      bool          writeGeometry = false,
                                      bool          writeASCII    = false          );
    bool    makeDUTHistograms        (std::string   detector,
                                      ROC         * roc,
                                      bool          fit           = false,
                                      bool          writeGeometry = false,
                                      bool          writeASCII    = false          );
    void    makeChi2Histograms       (void                                         );
    //    void    fitHistograms            (std::string   detector,
    //                                      ROC         * roc,
    //                                      double        xmin,
    //                                      double        xmax                           );
    TH1*    getHistogram             (std::string   detectorID,
                                      int           rocID,
                                      int           row,
                                      int           col                            );
    bool    getCalibrationLoaded     (void                                         ){return calibrationsLoaded_;}
    void    writeGeometry            (std::string   detector,
                                      ROC         * roc                            );

    void    setOutputASCIIfile      (bool writeASCII,
                                     std::string outputFile = "calib_output.txt"   ){
        writeASCII_     =writeASCII;
        outputASCIIfile_=outputFile;
    }
    void    setInputROOTfile        (bool readROOT)                                 { readROOT_       =readROOT; }

    bool execute (void);

    std::string  getLabel           (void                        ) ;
    int          getMaxIterations   (void                        ) ;
    std::string  getName            (void                        ) {return "calibrationLoader";}

private :

    //                reg            hits  adc
    typedef std::map< int , std::pair<int, int> >                 aPixelDataMapDef           ;
    //               row           col      data
    typedef std::map<int, std::map<int,aPixelDataMapDef> >        pixelDataMapDef            ;
    typedef std::map<std::string, std::string>                    headerMapDef               ;
    typedef std::map<int, std::map<int,fitter::fitResultDef> >    calibrationfitResultsMapDef;

    ofstream                             outputFile_         ;
    std::string                          outputASCIIfile_    ;
    fileEater                          * theFileEater_       ;
    HManager                           * theHManager_        ;
    fitter                             * theFitter_          ;
    TH1I                               * emptyTH1I_          ;

    headerMapDef                         header_             ;
    pixelDataMapDef                      pixels_             ;
    calibrationPlotsMapDef               calibrations_       ;
    bool                                 readROOT_           ;
    bool                                 writeASCII_         ;
    bool                                 calibrationsLoaded_ ;

    std::stringstream ss_ ;
} ;

#endif // CALIBRATIONLOADER_H
