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
