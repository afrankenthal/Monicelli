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
#include <TBranch.h>
#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>
#include <cstdlib>

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
  TFile file("treeClass.root") ;
  
  TTree * theTree = (TTree*)file.Get("treeClass") ;

  if( !theTree )
    {
      std::cout << "No 'treeClass' tree found in file 'treeClass.root'" << std::endl ;
      exit(0) ;
    }

  Event * event = new Event() ;
  
  TBranch * branch = theTree->GetBranch("eventBranch") ;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tBefore setAddress" << std::endl ;
  branch->SetAddress(&event) ;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tAfter  setAddress" << std::endl ;
  
  int events = theTree->GetEntries() ;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFound " << events << " events in tree" << std::endl ;
  
  for( int ev=0; ev<events; ++ev)
  {
   theTree->GetEntry(ev) ;
   int evNum   = event->getEventNumber() ;
   int nTracks = event->getNTracks() ;
   event->dumpTracks() ;
  }
  
  file.Close() ;
}
