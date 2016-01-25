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
 
#ifndef __EventReader_H__
#define __EventReader_H__

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
