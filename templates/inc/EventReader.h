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

#include <string>

// #include <EventHeader.h>
// 
// #include "MessageTools.h"
// 
// #include <TApplication.h>
// #include <TFile.h>
// #include <TBranch.h>
// #include <TTree.h>
// #include <TMatrixD.h>
// 
// #include "Event.h"
// #include "EventReader.h"
// #include "Geometry.h"

class Event;
class EventHeader;
class Geometry;
class TTree;
class TFile;


class EventReader
{
 public:
      EventReader(); 
     ~EventReader();
     
      bool openEventsFile (std::string fileName);
      bool openGeoFile    (std::string fileName);
      void closeInputFiles(void                );
      
      unsigned int  getRunNumber             (void              );
      unsigned int  getNumberOfEvents        (void              );
      std::string   getFileName              (void              );               
      
      void          readEvent                (unsigned int event);
      void          setPath                  (std::string  path );	 

      Event       * getEventPointer          (void              );
      EventHeader * getEventHeaderPointer    (void              );
      Geometry    * getGeometryPointer       (void              );

      TTree       * cloneEventTree           (void              ); 
      TTree       * cloneEventHeaderTree     (void              ); 

      TTree       * getEventTreePointer      (void              );
      TTree       * getEventHeaderTreePointer(void              );
      
      
      
 private:     
      TFile       * inputGeometryFile_   ;
      TFile       * inputEventsFile_     ;
      TTree       * inputGeometryTree_   ;
      TTree       * inputEventTree_      ;
      TTree       * inputEventHeaderTree_;

      Event       * theEvent_          ;
      EventHeader * theEventHeader_    ;
      Geometry    * theGeometry_       ;

      std::string   path_              ;
      std::string   fileName_          ;
      
      int runNumber_;
} ;

#endif
