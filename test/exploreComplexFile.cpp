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
