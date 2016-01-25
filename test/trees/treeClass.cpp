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
 
#include <iostream>

#include "Event.h"

#include <TApplication.h>
#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>

void doIt() ;

int main(int argc, char** argv)
{
  TApplication app("App",&argc, argv);
 
  doIt() ;
 
  app.Run() ;
}

//==================================================================
void doIt() 
{
  TFile file("treeClass.root","recreate") ;
  
  TRandom * r = new TRandom() ;

  TTree theTree("treeClass", "A tree with a class") ;

  Event * event = new Event() ;
  
  theTree.Branch("eventBranch", "Event", &event, 16000, 0) ; // No splitting

  for( int ev=0; ev<10; ++ev)
  {
   event->setEventNumber(ev) ;
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tEvent: " << ev         << std::endl ;
   for(int i=0; i<(int)r->Gaus(5,2); ++i)
   {
    double t = r->Gaus(50,12) ;
    std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t   t: " << t          << std::endl ;
    event->addTrack(t) ;
   }
   double x = r->Gaus(150,12) ;
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t   xw: " << x          << std::endl ;
   event->setX(x) ;
   std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\t   xr: " << event->x() << std::endl ;
   theTree.Fill() ;
   event->cleanData() ;
  }
  
//  theTree.BuildIndex("getEventNumber") ;
  
  file.Write() ;
  file.Close() ;
  
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFile treeClass.root written" << std::endl ;
}
