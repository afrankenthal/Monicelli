/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

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
