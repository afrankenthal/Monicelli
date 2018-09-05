/****************************************************************************
** Authors: Lorenzo Uplegger, Luigi Vigani
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef _MAPSAConverter_h_
#define _MAPSAConverter_h_

#include "TTree.h"

class EventReader;
class Event;
class EventHeader;
class Geometry;
class TFile;
class TTree;


typedef struct 
{
  double    xSlope;
  double    ySlope;
  double    xIntercept;
  double    yIntercept;
  double    chi2;
  int       trigger;
  int       runNumber;
  long long timestamp;

} MAPSAEvent;

class MAPSAConverter
{
public:
   MAPSAConverter(EventReader* reader);
  ~MAPSAConverter(void);
  
  void analyzeEvent(unsigned int event) ;

private: 
  Event       * theEvent_    ;
  EventHeader * theHeader_   ;
  Geometry    * theGeometry_ ;
   
  TFile*     theFile_;
  TTree*     theTree_;
  MAPSAEvent theMAPSAEvent_;
   
  int runNumber_;
};

#endif
