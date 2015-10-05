/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
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
