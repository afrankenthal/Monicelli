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

void doItWithClone() ;
void doItWithoutClone() ;

int main(int argc, char** argv)
{
  TApplication app("App",&argc, argv);
 
//  doItWithClone() ;
  doItWithoutClone() ;
 
  app.Run() ;
}

//==================================================================
void doItWithoutClone() 
{
  //TTree newTree("treeClass", "A new tree with expanded containers") ;

  TFile oldFile("treeClass.root", "update") ;
  
  TRandom * r     = new TRandom() ;
  Event   * event = new Event() ;

  TTree * theTree = (TTree*)oldFile.Get("treeClass") ;

  if( !theTree )
    {
      std::cout << "No 'treeClass' tree found in file 'treeClass.root'" << std::endl ;
      exit(0) ;
    }

  
  TBranch * branch = theTree->GetBranch("eventBranch") ;
  branch->SetAddress(&event) ;

  int events = theTree->GetEntries() ;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFound " << events << " events in tree" << std::endl ;
  
  for( int ev=0; ev<events; ++ev)
  {
   //oldFile.cd() ;
   theTree->GetEntry(ev) ;
   int evNum   = event->getEventNumber() ;
   int nTracks = event->getNTracks() ;
   
   for(int i=0; i<(int)r->Gaus(5,2); ++i)
   {
    event->addVertex((int)r->Gaus(25,12)) ;
   }

   //event->dumpTracks() ;

   theTree->Fill()      ;
   event->cleanData()  ;
  }
  
  oldFile.Write("", TObject::kOverwrite) ;
//  theTree->AutoSave();
  
  oldFile.Close() ;
}

//==================================================================
void doItWithClone() 
{
  //TTree newTree("treeClass", "A new tree with expanded containers") ;

  TFile oldFile("treeClass.root",        "update"    ) ;
  
  TRandom * r     = new TRandom() ;
  Event   * event = new Event() ;
  
  TTree * theTree = (TTree*)oldFile.Get("treeClass") ;

  if( !theTree )
    {
      std::cout << "No 'treeClass' tree found in file 'treeClass.root'" << std::endl ;
      exit(0) ;
    }

  
  TBranch * branch = theTree->GetBranch("eventBranch") ;
  branch->SetAddress(&event) ;

  //newFile.cd() ;
  //newTree.Branch("eventBranch", "Event", &event, 16000, 0) ; // No splitting
  TFile newFile("treeClassExpanded.root","recreate") ;
  TTree *newTree = theTree->CloneTree(0);
    
  //oldFile.cd() ;
  int events = theTree->GetEntries() ;
  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFound " << events << " events in tree" << std::endl ;
  
  for( int ev=0; ev<events; ++ev)
  {
   //oldFile.cd() ;
   theTree->GetEntry(ev) ;
   int evNum   = event->getEventNumber() ;
   int nTracks = event->getNTracks() ;
   
   for(int i=0; i<(int)r->Gaus(5,2); ++i)
   {
    event->addVertex((int)r->Gaus(25,12)) ;
   }

   //event->dumpTracks() ;

   //newFile.cd()        ;
   newTree->Fill()      ;
   event->cleanData()  ;
  }
  
  newTree->AutoSave();
  //newFile.cd()    ;
  //newFile.Write() ;
  
  //newFile.Close() ;
    
  oldFile.Close() ;
  newFile.Close() ;
  system("cp treeClassExpanded.root treeClass.root" ) ;
  
  //theTree = newTree->CloneTree();
  
  //oldFile.cd();
  //theTree->AutoSave();
  //theTree->Write() ;
  //newFile.Close() ;
  
  //theTree->Refresh();
  
/*
  for( int ev=0; ev<theTree->GetEntries(); ++ev)
  {
   theTree->GetEntry(ev) ;
   int evNum   = event->getEventNumber() ;
   int nTracks = event->getNTracks() ;
   std::cout << "Got " << nTracks << " tracks for event " << evNum << std::endl ;
   event->dumpTracks() ;
  }
*/

  std::cout << __LINE__ << "]\t[" << __PRETTY_FUNCTION__ << "]\tFile treeClass.root updated with vertices" << std::endl ;
}
