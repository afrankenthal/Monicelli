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
 
#ifndef _EventAnalyzer_h_
#define _EventAnalyzer_h_

#include <vector>
#include <sstream>

class EventReader;
class Event;
class EventHeader;
class Geometry;
class TFile;
class TH1F;

//====================================================================
class EventAnalyzer
{
 public:
        EventAnalyzer(EventReader* reader);
       ~EventAnalyzer(void               );
  
   void analyzeEvent (unsigned int event );

 private: 
   Event       * theEvent_    ;
   EventHeader * theHeader_   ;
   Geometry    * theGeometry_ ;
   
   TFile*  	      theFile_;
   TH1F*   	      hChi2;
   TH1F*   	      hXSlope;
   TH1F*   	      hYSlope;
   std::vector<TH1F*> hXResiduals;
   std::vector<TH1F*> hYResiduals;
   std::vector<TH1F*> hXPulls;
   std::vector<TH1F*> hYPulls;
   std::vector<TH1F*> hXResidualVsXCoordinate;
   std::vector<TH1F*> hXResidualVsYCoordinate;
   
   int runNumber_;
   
   std::stringstream ss_ ;
} ;

#endif
