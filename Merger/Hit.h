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
 
#ifndef _Hit_h_
#define _Hit_h_

#include <iostream>
#include <iomanip>

//#include "DataDecoder.h"
//#include "HitTranslator.h"

class Hit{
 public:
  Hit();
  ~Hit();
  unsigned long long data;
  short detId  ;
  short station;
  short plaq   ;
  short cid    ;
  short row    ;
  short col    ;
  short adc    ;
  int 	trig   ;
  short	bco   ;
  const unsigned long long& getData(void) const {return data;}
 private:
  //static DataDecoder   theDataDecoder_;
  //static HitTranslator theHitTranslator_;

  friend std::ostream& operator<<(std::ostream& os, const Hit& hit){
    os << "Data: "     << std::hex << (hit.data >> 32) << (hit.data & 0xffffffff) << std::dec
       << " detId: "   << hit.detId
       << " station: " << hit.station
       << " plaq: "    << hit.plaq
       << " cid: "     << hit.cid
       << " row: "     << hit.row
       << " col: "     << hit.col
       << " adc: "     << hit.adc;
    return os;		      
  } 
};


#endif
