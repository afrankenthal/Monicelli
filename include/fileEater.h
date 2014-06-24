/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
** This class encapsulates the execution of a heavy CPU-cyles consumer in
** a separate thread (a QThread). The 'run()' method is invoked when the thread
** is started via the 'start()' method: it is supposed to call a method of the
** 'fileEater' class and continue untill the method ends. In order to send
** eventual 'abort' commands to the 'fileEater', this class is supposed to
** instantiate a handle to the 'MessageBus' class which is in charge of
** broadcasting messages between a GUI and a slave process.
**
****************************************************************************/

#ifndef FILEEATER_H
#define FILEEATER_H

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <TBranch.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH2I.h>
#include <TROOT.h>
#include <TTree.h>
#include <vector>

/*
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>
*/
#include "beamSimulator.h"
//#include "clusterizer.h"
#include "Event.h"
#include "EventHeader.h"
#include "Geometry.h"
#include "geometryLoader.h"
#include "HManager.h"
#include "MessageTools.h"
#include "multiProcess.h"
#include "subProcess.h"
//#include "trackFinder.h"
//#include "trackFitter.h"
#include "verbosity.h"


class trackFinder;
class HManager   ;

#define INVALID_DIRECTORY "Invalid directory"

class fileEater : public multiProcess<fileEater,bool>
{

public:

    typedef std::map<std::string,TFile*>                fileMapDef       ;
    typedef std::vector<std::string>                    fileVDef         ;
    typedef std::vector<subProcess<Event,Geometry>*>    subProcessVDef   ;
    typedef std::map<std::string,TH2I*>                 hMapDef          ;

    fileEater()                       ;
    fileEater(int argc, char ** argv) ; // Call this for a stand-alone program
   ~fileEater()                       ;

    void            addSubProcess            (subProcess<Event,Geometry> * theSubProcess        ) ;
    std::string     openFile                 (std::string                  inputFile            ) ;
    bool            openGeometryFile         (std::string                  geometryFileNae      ) ;
    void            closeFile                (void                                              ) ;
    std::string     currentAction            (void                                              ) {return currentAction_             ;}
    void            defineAction             (std::string                  action               ) {currentAction_ = action           ;}
    void            setSubProcess            (subProcess<Event,Geometry> * theSubProcess        ) ;
    void            clearSubProcess          (void                                              ) ;
    bool            execute                  (void                                              ) ;
    bool            fullReconstruction       (void                                              ) ;
    std::string     getLabel                 (void                                              ) ;
    int             getMaxIterations         (void                                              ) ;
    int             getCurrentIteration      (void                                              ) ;
    Event         * getEvent                 (int                          event                ) ;
    EventHeader   * getEventHeader           (void                                              ) {return theEventHeader_            ;}
    Geometry      * getGeometry              (void                                              ) ;
    std::ifstream * getInputFile             (void                                              ) {return inputFile_                 ;}
    std::string     getName                  (void                                              ) {return "fileEater"                ;}
    subProcessVDef  getCurrentSubProcess     (void                                              ) ;
    int             getTrigNumber            (void                                              ) {return currentProcessingStep_     ;}
    int             getEventsNumber          (void                                              ) ;
    bool            growMagicTree2           (bool                         BinaryFile = true    ) ;
    bool            isRootTreeFile           (void                                              )
                                                                                                  {
                                                                                                   if(fileFormat_== "rootTree") return true ;
                                                                                                   else                         return false;
                                                                                                  }
    void            makeGeometryTreeFile     (std::string                  eventFileCompletePath) ;
    bool            updateEvents2            (void                                              ) ;
    bool            updateGeometry           (std::string                  what                 ) ;
    bool            parse                    (void                                              ) ;
    void            populate                 (void                                              ) ;
    void            setHManger               (HManager                   * hManager             ) {theHManager_ = hManager           ;}
    void            setEventsLimit           (int                          eventsLimit          ) {eventsLimit_ = eventsLimit        ;}
    void            setOperation             (multiProcess_function        function,
                                              subProcess<Event,Geometry> * theSubProcess = NULL ) ;
/*
    void            setTolerances            (double                       tolX,
                                              double                       tolY,
                                              double                       trackPoints,
                                              double                       chi2Cut    ,
                                              double                       incDUT               )
                                                                                                  {
                                                                                                   roadSearchToleranceX_ = tolX        ;
                                                                                                   roadSearchToleranceY_ = tolY        ;
                                                                                                   trackPoints_          = trackPoints ;
                                                                                                   trackFinderChi2cut_   = chi2Cut     ;
                                                                                                   trackFinderDUTinc_    = incDUT      ;
                                                                                                  }
*/
    void            setVerbosity             (int                          verbosity            ) ;
    fileMapDef      getOpenFiles             (void                                              ) {return inputTreesMap_             ;}
    std::string     getOutputTreeCompletePath(void                                              ) ;
    TFile         * getFileHandle            (std::string                  fullPath             ) ;
    EventHeader   * getHeader                (void                                              ) {return theEventHeader_            ;}
    beamSimulator * getBeamSimulator         (void                                              ) {return theBeamSimulator_          ;}


 
    hMapDef       & getHistograms            (void                                              )  ;

    void            setWriteOutASCII         (bool                         writeOutFile         ) {writeOutFile_    = writeOutFile   ;}
    void            setInputFileName         (std::string                  inputFileName        ) {inputFileName_   =  inputFileName ;}
    void            setInputFileNames        (fileVDef                     inputFileNames       ) {inputFileNames_  =  inputFileNames;}
    void            setOutputFileName        (std::string                  outputFileName       ) {outputFileName_  = outputFileName ;}
    void            setOutputTreePath        (std::string                  outputTreePath       ) {outputTreePath_  = outputTreePath ;}

private:

    typedef std::map<int,Event::plaqMapDef>   rawDataMapDef ;
    typedef std::map<std::string,TFile*>      fileHandleDef ;

    void   fillMagicPlaqComposition (int            station,
                                     int            plaq   ,
                                     unsigned int   row    ,
                                     unsigned int   col    );
    int    getNLines                (void )                 ;
    bool   parseMagicASCII          (int          & trig   ,
                                     int          & nByte  ,
                                     int          & nLine  );
    bool   parseBinary3             (TTree* tree);
    int    computeSensorStrip       (int chip,int set,int strip);


//    double                        roadSearchToleranceX_     ;
//    double                        roadSearchToleranceY_     ;
//    double                        trackPoints_              ;
//    double                        trackFinderChi2cut_       ;
//    bool                          trackFinderDUTinc_        ;
    TTree                       * inputGeometryTree_        ;
    TTree                       * inputEventTree_           ;
    TTree                       * inputEventHeader_         ;
    TH2I                        * emptyH_                   ;
    std::string                   fileFormat_               ;
    std::ifstream               * inputFile_                ;
    fileHandleDef                 inputTreesMap_            ;
    std::string                   outputTreeFileName_       ;
    std::string                   inputFileName_            ;
    fileVDef                      inputFileNames_            ;
    std::string                   outputFileName_           ;
    std::string                   outputTreePath_           ;
    Event::plaqMapDef             plaqMap_                  ;
    rawDataMapDef                 rawDataMap_               ;

    //fileEater_function            operation_                ;

    ofstream                      outputFile_               ;

    std::stringstream             ss_                       ;
    beamSimulator               * theBeamSimulator_         ;
    Event                       * theEvent_                 ;
    EventHeader                 * theEventHeader_           ;
    TBranch                     * theEventBranch_           ;
    TTree                       * theEventTree_             ;
    geometryLoader              * theGeometryLoader_        ;
    Geometry                    * theGeometry_              ;
    std::map<std::string,TH2I*>   beamSpotsH_               ;
    std::map<std::string,TH2I*>   flippedBeamSpotsH_        ;
    std::map<std::string,TH1D*>   xProfileH_                ;
    std::map<std::string,TH1D*>   yProfileH_                ;
    TFolder                     * histogramsFolder_         ;
    TFolder                     * beamSpots_                ;
    TFolder                     * lowResBeamSpots_          ;
    TFolder                     * flippedBeamSpots_         ;
    TFolder                     * beamProfiles_             ;
    TFolder                     * xProfiles_                ;
    TFolder                     * yProfiles_                ;

    HManager                    * theHManager_              ;
    
    bool                          writeOutFile_             ;
    int                           eventsLimit_              ;
    int                           currentVerbosity_         ;
    std::string                   currentAction_            ;
    subProcessVDef                currentSubProcess_        ;
    int                           currentProcessingStep_    ;
};

#endif // FILEEATER_H
