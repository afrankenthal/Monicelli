/****************************************************************************
** Authors: Lorenzo Uplegger, Luigi Vigani
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef __AdamParaConverter_H__
#define __AdamParaConverter_H__

#include "TTree.h"
#include <fstream>

class EventReader;
class Event;
class EventHeader;
class Geometry;


typedef struct 
{
  double    xIntercept;
  double    yIntercept;
  double    xSlope;
  double    ySlope;
  double    x[8];
  double    y[8];
  double    z[8];  
  double    xError[8];
  double    yError[8];
  double    zError[8];  
} AdamParaEvent;

class AdamParaConverter
{
public:
   AdamParaConverter(EventReader* reader);
  ~AdamParaConverter(void);
  
  void analyzeEvent(unsigned int event) ;

private: 
  Event       * theEvent_    ;
  EventHeader * theHeader_   ;
  Geometry    * theGeometry_ ;
   
  ofstream      theFile_;
  AdamParaEvent theAdamParaEvent_;
   
  int runNumber_;
};

#endif
