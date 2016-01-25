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
 
#ifndef _DataDecoder_h_
#define _DataDecoder_h_

#include "Hit.h"
#include "Status.h"
#include <string>

struct PxDecodedHit
{
   unsigned int chan; 
   unsigned int chipid;
   unsigned int set;  
   unsigned int strip;
   unsigned int bco;  
   unsigned int adc;  
   unsigned int istrip;
};

class DataDecoder{
 public:
  DataDecoder (void);
  DataDecoder (const std::string& dataString, bool isTimestamp=false, int station=-1);
  DataDecoder (unsigned long long data,       bool isTimestamp=false, int station=-1);
  ~DataDecoder(void);

  //Setters
  void setData     (const std::string& dataString, int station=-1);
  void setData     (unsigned long long data,       int station=-1);
  void setTimestamp(const std::string& dataString);
  void setTimestamp(unsigned long long data);

  //Getters
  const unsigned long long& getData        (void){return data_;}
  unsigned long long        getTimestamp   (void);
  unsigned long             getPacketNumber(void);
 
  bool               isData       (void);
  unsigned long long order        (void);
  unsigned long long convert      (const std::string& dataString);
  Hit                decodeHit    (const std::string& dataString, int station=-1);
  Hit                decodeHit    (unsigned long long data,       int station=-1);
  Hit                decodeHit    (void);
  Status             decodeStatus (void);
  int                decodeTrigger(const std::string& dataString);
  int                decodeTrigger(unsigned long long data);
  int                decodeTrigger(void);
  void               addStation   (int station);
 private:
  unsigned long long unOrderedData(void);
  unsigned long long data_;
  unsigned long long unOrderedData_;
  bool               ordered_;
};

#endif
