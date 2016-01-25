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
 
// g++ -o folders -I../include folders.cpp `root-config --cflags --libs` && ./folders


#include <TApplication.h>
#include <TBrowser.h>
#include <TFile.h>
#include <TH1D.h>
#include <TRandom.h>
#include <TTree.h>

#include "MessageTools.h"

void doIt(void) ; 
void dump(int) ;

//==================================================================
int main(int argc, char** argv)
{
  TApplication app("Folders",&argc, argv);
 
  doIt() ;
 
  app.Run() ;
  
  return 0 ;
}

//==================================================================
void doIt() 
{
  TDirectory * topDir = gDirectory ;


  TH1D * h1 = new TH1D("h1","h1",100,0,100) ;
//  h1->SetDirectory(0) ;  

  dump(__LINE__) ;

  TDirectory * mainDir = gDirectory->mkdir("Main Directory") ;

  dump(__LINE__) ;
  
  mainDir->cd() ;
  
  TH1D * h2 = new TH1D("h2","h2",100,0,200) ;  
//  h2->SetDirectory(0) ;  

  dump(__LINE__) ;
  
  //TFile * outFile = new TFile("folders.root","recreate") ;
  
//  TBrowser t("t","t") ;

  dump(__LINE__) ;  
  
  //outFile->Append((TObject*)mainDir) ;
//  h1->SetDirectory(mainDir) ;  
//  h2->SetDirectory(mainDir) ;  
//  mainDir->Write() ;
 // outFile->Write() ;
//  h1->Write() ;
//  h2->Write() ;
 // outFile->Close() ;
}

//==================================================================
void dump(int lineNumber)
{
  std::cout << "\n" << lineNumber << "]\tNow in " ;
  gDirectory->pwd() ;
  std::cout << lineNumber << "]\tName of gDirectory: " << gDirectory->GetName() << std::endl ;
} 
