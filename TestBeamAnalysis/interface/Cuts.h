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
 
#ifndef Cuts_h
#define Cuts_h

class DataClass;
class Cuts
{
 public:
  Cuts(DataClass& data);	
//  Cuts(const Cuts& cuts);	
//  Cuts& operator=(const Cuts& cuts);	
  ~Cuts();
  void reset   	   (void       );
  bool ntrack  	   (int tracks );
  bool newEvent	   (void       );
  bool chi2Excl	   (int plane  );
  bool hasHits     (int plane  );
  bool allTrackProj(void       );
  bool hitTrackProj(int plane  );
  bool xResid	   (int plane  );
  bool yResid	   (int plane  );
  bool efficiency  (void);
  bool stationHits (int station);
  
 private:
  //Methods
  double getChi2Ndof(int plane);

  //Variables
  DataClass& theData_;
  int        oldEventId_;
  const double chi2Cut_;
};

#endif
