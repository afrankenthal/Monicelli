/****************************************************************************
** Authors: Lorenzo Uplegger, Luigi Vigani
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef __T1037Converter_H__
#define __T1037Converter_H__

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

} T1037Event;

class T1037Converter
{
public:
   T1037Converter(EventReader* reader);
  ~T1037Converter(void);
  
  void analyzeEvent(unsigned int event) ;

private: 
  Event       * theEvent_    ;
  EventHeader * theHeader_   ;
  Geometry    * theGeometry_ ;
   
  TFile*     theFile_;
  TTree*     theTree_;
  T1037Event theT1037Event_;
   
  int runNumber_;
};

#endif
