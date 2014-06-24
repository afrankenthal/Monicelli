/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

// g++ -o foldersInMemory -I../include foldersInMemory.cpp `root-config --cflags --libs` && ./foldersInMemory

#include <map> 
#include <sstream> 
#include <string> 

#include <TApplication.h>
#include <TBrowser.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1D.h>
#include <TObject.h>
#include <TRandom.h>
#include <TRint.h>
#include <TROOT.h>
#include <TTree.h>

#include "MessageTools.h"

void doIt       (void                ) ; 
void inspectTree(TFolder * thisFolder) ;

std::map<std::string, TObject *> map_ ;

std::stringstream ss_ ;

int level_ ;

//==================================================================
int main(int argc, char** argv)
{
  TApplication app("Folders",&argc, argv);
 
  level_ = 0 ;

  doIt() ;

  TFolder * rootFolder = gROOT->GetRootFolder() ;

  app.Run() ;
  
  int i = 0 ;
  std::cin >> i ;
   
  return 0 ;
}

//==================================================================
void doIt() 
{
 TFile * cFile = new TFile("complexFile.root") ;
 
 TFolder * mainFolder  = gROOT->GetRootFolder()->AddFolder("MainFolder","Main Folder"        ) ;
 TH1D    * hMain       = new TH1D   ("hMain",     "hMain",    100,0,100) ;
 hMain->SetDirectory(0) ;
 mainFolder->Add(hMain);
 mainFolder->SetOwner(kTRUE);

 TFolder * subFolder1 = mainFolder->AddFolder             ("SubFolder1","Sub Folder 1"        ) ;
 TH1D    * hSub1      = new TH1D    ("hSub1",     "hSub1",    200,0,200) ;
 hSub1->SetDirectory(0) ;
 subFolder1->Add(hSub1) ;

 TFolder * subFolder2 = mainFolder->AddFolder             ("SubFolder2","Sub Folder 2"        ) ;
 TH1D    * hSub2      = new TH1D    ("hSub2",     "hSub2",    200,0,200) ;
 TH1D    * hSub3      = new TH1D    ("hSub3",     "hSub3",    200,0,200) ;
 hSub2->SetDirectory(0) ;
 hSub3->SetDirectory(0) ;
 subFolder2->Add(hSub2) ;
 subFolder2->Add(hSub3) ;
  
 TFolder * topFolder   = gROOT->GetRootFolder() ;
 
 level_ = 0 ;
 inspectTree(topFolder) ;
 
 TFile * file = new TFile("testFile.root","recreate" ) ;
 
 mainFolder->Write() ;
 
 file->Close() ;

 TFolder * secondFolder = subFolder2->AddFolder("SecondFolder","Second Folder"  ) ;
 TH1D    * hSecond      = new TH1D   ("hSecond", "hSecond", 100,0,100) ;
 hSecond->SetDirectory(0) ;
 secondFolder->Add(hSecond) ;
}

//===================================================================
void inspectTree(TFolder * thisFolder) 
{
 std::string blanks = "" ;
 
 level_++ ;
 for(int l=0; l<level_; ++l) {blanks += " ";}

 TCollection * rootFoldersColl     = (TCollection *)thisFolder->GetListOfFolders() ;
 TIterator   * rootFoldersIterator = rootFoldersColl->MakeIterator() ;
 
 TObject * obj = NULL ;
 
 std::cout << blanks << "=============== " << thisFolder->GetName() << " ===============" << std::endl ;
 while(obj = rootFoldersIterator->Next())
 {
//  cout << "   Object: '" << obj->GetName() << "'" << endl ;

  std::string hName = " root memory " ;
  if( obj->IsA() == TH1D::Class() ) 
  {
   if( ((TH1D*)obj)->GetDirectory() ) hName = ((TH1D*)obj)->GetDirectory()->GetName() ;
   std::cout << blanks << "    a histogram: " << obj->GetName() << " in " << hName<< std::endl ;
  }
  else
  {
    if( obj->IsA() == TFolder::Class() ) 
    { 
      std::cout << blanks << "    Enter folder: " << obj->GetName() << std::endl ;
      inspectTree((TFolder*)obj) ;
    }
  }
  if( obj->IsA() == TDirectory::Class() ) 
  {
      std::cout << blanks << "    Enter directory: " << obj->GetName() << std::endl ;
  }
 }

 level_-- ;
}

