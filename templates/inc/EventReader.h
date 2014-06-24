#ifndef __EventReader_H__
#define __EventReader_H__

/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include <iostream>
#include <string>

#include <EventHeader.h>

#include "MessageTools.h"

#include <TApplication.h>
#include <TFile.h>
#include <TBranch.h>
#include <TTree.h>
#include <TMatrixD.h>

#include "Event.h"
#include "EventReader.h"
#include "Geometry.h"

class EventReader
{
 public:
      EventReader(); 
     ~EventReader();
     
      bool openEventsFile(std::string fileName) ;
      bool openGeoFile   (std::string fileName) ;
      
      unsigned int  getNumberOfEvents    (void              ) {return inputEventTree_->GetEntries();}
      std::string   getFileName          (void)               {return path_+fileName_;              }
      
      void          readEvent            (unsigned int event) {inputEventTree_->GetEntry(event)    ;}
      void          setPath              (std::string  path ) {path_ = path                        ;}       

      Event       * getEventPointer      (void              ) {return theEvent_                    ;}
      EventHeader * getEventHeaderPointer(void              ) {return theEventHeader_              ;}
      Geometry    * getGeometryPointer   (void              ) {return theGeometry_                 ;}
      
      int getRunNumber () {return runNumber_;}
      
      
 private:     
      TFile       * inputGeometryFile_ ;
      TFile       * inputEventsFile_   ;
      TTree       * inputGeometryTree_ ;
      TTree       * inputEventTree_    ;
      TTree       * inputEventHeader_  ;

      Event       * theEvent_          ;
      EventHeader * theEventHeader_    ;
      TBranch     * theEventBranch_    ;
      TTree       * theEventTree_      ;
      Geometry    * theGeometry_       ;

      std::string   path_              ;
      std::string   fileName_          ;
      
      int runNumber_;
} ;

#endif
