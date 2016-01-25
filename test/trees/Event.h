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
 
#include <vector>

#include <TObject.h>

class Event : public TObject
{
 public:
   Event(void)  ;
  ~Event(void) {;}
  
   void             setEventNumber (int    event) ;
   int              getEventNumber (void)         ;
   void             addTrack       (double p)     ;
   void             addVertex      (int    v)     ;
   int              getNTracks     (void)         ;
   std::vector<int> getVertices    (void)         ;
   void             dumpTracks     (void)         ;  
   void             cleanData      (void)         ;
   
   void             setX           (double x)     {myStruct_.x_ = x ;}
   double           x              (void)         {return myStruct_.x_ ;}
   
 private:
   int eventNumber_       ;
   std::vector<double> p_ ;
   std::vector<int> vertices_ ;
   
   struct myStruct_
   {
    double x_ ;
    double y_ ;
   } myStruct_ ;
   
 ClassDef(Event,1)
} ;
