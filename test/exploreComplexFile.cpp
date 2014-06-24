/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

// use make to compile

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <TApplication.h>
#include <TBrowser.h>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TH1F.h>
#include <TIterator.h>
#include <TKey.h>
#include <TRandom.h>
#include <TTree.h>

#include "Event.h"

std::stringstream ss_ ;

int level = 0 ;

void doIt(void) ; 
void explore(TDirectory * currentDirectory) ;

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
  TFile * file = new TFile("complexFile.root") ;

  explore(gDirectory) ;    
}

//==================================================================
void explore(TDirectory * currentDirectory) 
{
  level++ ;

  std::string blanks = "" ;
  for(int l=0; l<level; ++l)
  {
    blanks += " " ;
  }
    
  ss_.str("") ;
  ss_ << __LINE__ << "]\t" << blanks << "Exploring '" << currentDirectory->GetName() << "' Level: " << level ;
  std::cout << "\n" << ss_.str() << std::endl ;

//  currentDirectory->ReadAll() ;

  TKey * keyH = NULL ;
  TIter hList(currentDirectory->GetListOfKeys());

  while((keyH = (TKey*)hList()))
  {
    if( keyH->IsFolder() ) 
      {
       currentDirectory->cd(keyH->GetName());
       TDirectory * subDir = gDirectory ;
       explore(subDir) ;
       level-- ;
      }
    else
      {
       std::string hName = keyH->GetName() ;  // Capture histogram name from file (at this stage the histogram is NOT YET trensfered to memory!!)
       ss_.str("") ;
       ss_ << __LINE__ << "]\t" << blanks << "Found '" << hName << "'";
       std::cout << ss_.str() << std::endl ;
      }
  }
}
