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
 
#include "EventReader.h"
#include "T1037Converter.h"

#include <iostream>
//====================================================================
int main(int argv, char **argc)
{
  
  if ( argv < 3)
    {
      std::cout << __LINE__<< " " << __FILE__ << " usage: " << argc[0] << " root-file geo-file" << std::endl;  
      return 1;
    }

  //TApplication app("T1037Converter",&argv, argc);
  EventReader * reader = new EventReader() ; 
  reader->setPath ("/data/TestBeam/2014_02_February_T1037/MonicelliOutput/");
  reader->openEventsFile(argc[1]);
  reader->openGeoFile   (argc[2]);
  T1037Converter* analyzer = new T1037Converter(reader) ;
  
  // app.Run() ;
    
  return 0 ;
}
