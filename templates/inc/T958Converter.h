/****************************************************************************
** Authors: Lorenzo Uplegger, Luigi Vigani
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef __T958Converter_H__
#define __T958Converter_H__

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

} T958Event;

class T958Converter
{
public:
   T958Converter(EventReader* reader);
  ~T958Converter(void);
  
  void analyzeEvent(unsigned int event) ;

private: 
  Event       * theEvent_    ;
  EventHeader * theHeader_   ;
  Geometry    * theGeometry_ ;
   
  TFile*     theFile_;
  TTree*     theTree_;
  T958Event theT958Event_;
   
  int runNumber_;
};

#endif
