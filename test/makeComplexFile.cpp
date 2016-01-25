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
 
// use make to compile

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <TApplication.h>
#include <TBrowser.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TRandom.h>
#include <TTree.h>

#include "Event.h"

void doIt(void) ; 
void makeHistograms(std::string name, int num) ;

TCanvas * canvas_ ;
std::stringstream ss_ ;
std::map<std::string, std::map<std::string, std::vector<TH1D*> > > hMap_ ;

//==================================================================
int main(int argc, char** argv)
{
  TApplication app("Folders",&argc, argv);
 
  doIt() ;
 
  app.Run() ;
  
  return 0 ;
}

//==================================================================
void doIt(void) 
{
  canvas_ = new TCanvas("Canvas","Canvas") ;
  
  TRandom * r = new TRandom() ;

  TFile * file = new TFile("complexFile.root","recreate") ;
  
  // Put a TTree in as a first thing
  std::cout << "Put a TTree in as a first thing" << std::endl ;
  TTree theTree("treeClass", "A tree with a class") ;

  Event * event = new Event() ;
  
  theTree.Branch("eventBranch", "Event", &event, 16000, 0) ; // No splitting

  for( int ev=0; ev<10; ++ev)
  {
   event->setEventNumber(ev+1547) ;
   for(int i=0; i<(int)r->Gaus(5,2); ++i)
   {
    event->addTrack(r->Gaus(50,12)) ;
   }
   theTree.Fill() ;
   event->cleanData() ;
  }

  // Now go with folders
  std::cout << "Now go with folders" << std::endl ;
  TDirectory * mainFolder = file->mkdir("Main Folder") ;
  
  mainFolder->cd() ;
  
  TDirectory * folderRaw      = gDirectory->mkdir("Raw") ;
  TDirectory * folderClusters = gDirectory->mkdir("Clusters") ;
  TDirectory * folderTracks   = gDirectory->mkdir("Tracks") ;
  TDirectory * folderAnalysis = gDirectory->mkdir("Analysis") ;
  
  folderRaw->cd()      ; makeHistograms("hRaw", 5) ; makeHistograms("hHits", 3) ;
  
  folderClusters->cd() ;
  
   makeHistograms("hClusterSize", 3) ; 
   TDirectory * specialClusters = gDirectory->mkdir("Special clusters") ;
   specialClusters->cd() ; makeHistograms("hSpecial", 4) ;

  folderTracks->cd()   ; makeHistograms("hTracks",   2) ;  
  folderAnalysis->cd() ; makeHistograms("hAnalysis", 4) ; 

  file->Write() ;
  file->Close() ;
  
  std::cout << "File complexFile.root created" << std::endl ;
}

//==================================================================
void makeHistograms(std::string name, int num)
{
  std::string thisFolder = gDirectory->GetName() ;

  TRandom * r = new TRandom() ;

  for(int i=0; i<num; ++i)
  {
   ss_.str("") ;
   ss_ << name << "_" << i ;
   TH1D * h = new TH1D(ss_.str().c_str(),ss_.str().c_str(),100,0,100);
   for(int x=0; x<(int)r->Gaus(50000,2000); ++x)
   {
     h->Fill(r->Gaus(50,5)) ;
   }
   
   hMap_[thisFolder][name].push_back(h) ;
   ss_.str("") ;
   ss_ << "Canvas: " << name << "_" << i ;
   canvas_->SetName(ss_.str().c_str()) ;
   canvas_->SetTitle(ss_.str().c_str()) ;
   canvas_->cd() ;
   h->Draw() ;
   canvas_->Write() ;
   
  }
}
